"""
Further explained in publication:
Semeraro, F., Ferguson, J.C., Acin, M., Panerai, F. and Mansour, N.N., 2021.
Anisotropic analysis of fibrous and woven materials part 2: Computation of effective conductivity.
Computational Materials Science, 186, p.109956.
https://www.sciencedirect.com/science/article/abs/pii/S092702562030447X

Please cite using this BibTex:
@article{semeraro2021anisotropic,
  title={Anisotropic analysis of fibrous and woven materials part 2: Computation of effective conductivity},
  author={Semeraro, Federico and Ferguson, Joseph C and Acin, Marcos and Panerai, Francesco and Mansour, Nagi N},
  journal={Computational Materials Science},
  volume={186},
  pages={109956},
  year={2021},
  publisher={Elsevier}
}
"""
from pumapy.physics_models.finite_volume.anisotropic_conductivity_utils import create_Ab_indices_cy, create_T_ivs_cy, assign_prescribed_bc_cy
from pumapy.physics_models.finite_volume.mpxa_matrices import (fill_Ampfa, fill_Bmpfa, fill_Cmpfa, fill_Dmpfa, create_d1_mpfa,
                                                               create_d2_mpfa, div_Eu_mpfa, div_Ed_mpfa, create_mpfa_indices)
from pumapy.physics_models.finite_volume.elasticity_utils import pad_domain_cy
from pumapy.physics_models.utils.linear_solvers import PropertySolver
from pumapy.physics_models.utils.boundary_conditions import AnisotropicConductivityBC
from pumapy.utilities.timer import Timer
from pumapy.utilities.logger import print_warning
from pumapy.utilities.generic_checks import estimate_max_memory
from scipy.sparse import coo_matrix, diags
import numpy as np
import sys


class AnisotropicConductivity(PropertySolver):

    def __init__(self, workspace, cond_map, direction, side_bc, dirichlet_bc, tolerance, maxiter, solver_type, display_iter):
        allowed_solvers = ['direct', 'gmres', 'bicgstab']
        super().__init__(workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter)

        self.cond_map = cond_map
        self.mat_cond = dict()
        self.direction = direction
        self.side_bc = side_bc

        self.dirichlet_bc = dirichlet_bc

        self.need_to_orient = False  # changes if (E_axial, E_radial, nu_poissrat_12, nu_poissrat_23, G12) detected
        self.T = None
        self.q = None
        self.keff = None

        self.orient_pad = None

        # index arrays
        self.kf_i_inds = np.tile(np.tile(np.array([0, 1], dtype=np.int8), 2), 2)
        self.kf_j_inds = np.tile(np.repeat(np.array([0, 1], dtype=np.int8), 2), 2)
        self.kf_k_inds = np.repeat(np.array([0, 1], dtype=np.int8), 4)
        self.Aind, self.Bind, self.Cind, self.Dind = create_mpfa_indices()
        self.inds_sw = [3, 7, 11]
        self.inds_se = [3, 6, 10]
        self.inds_nw = [2, 7, 9]
        self.inds_ne = [2, 6, 8]
        self.inds_tsw = [1, 5, 11]
        self.inds_tse = [1, 4, 10]
        self.inds_tnw = [0, 5, 9]
        self.inds_tne = [0, 4, 8]

    def compute(self):
        t = Timer()
        self.initialize()
        estimate_max_memory("anisotropic_conductivity", self.ws.matrix.shape, self.solver_type, self.need_to_orient)
        self.assemble_matrices()
        print("Time to assemble matrices: ", t.elapsed()); t.reset()
        super().solve()
        print("Time to solve: ", t.elapsed())
        self.compute_fluxes()
        self.compute_effective_coefficient()
        self.solve_time = t.elapsed()

    def initialize(self):

        # Rotating domain to avoid having different cases and padding
        shape = [self.len_x, self.len_y, self.len_z]
        reorder = [0, 1, 2]
        if self.direction in ['y', 'z']:
            if self.direction == 'y':
                shape = [self.len_y, self.len_x, self.len_z]
                reorder = [1, 0, 2]
                reorder_nondiagcond = [3, 5, 4]
            else:
                shape = [self.len_z, self.len_y, self.len_x]
                reorder = [2, 1, 0]
                reorder_nondiagcond = [5, 4, 3]

            # Rotating mat_cond
            for key in self.mat_cond.keys():
                tmp_cond = list(self.mat_cond[key])
                if len(tmp_cond) == 6:
                    self.mat_cond[key] = tuple([x for _, x in sorted(zip(reorder + reorder_nondiagcond, tmp_cond))])

            self.ws.matrix = self.ws.matrix.transpose(reorder)

        self.len_x, self.len_y, self.len_z = shape
        self.len_xy = self.len_x * self.len_y
        self.len_xyz = self.len_x * self.len_y * self.len_z

        # Segmenting padded domain
        for i in range(self.cond_map.get_size()):
            low, high, _ = self.cond_map.get_material(i)
            self.ws[np.logical_and(self.ws.matrix >= low, self.ws.matrix <= high)] = low

        # Padding domain, imposing symmetric or periodic BC on faces
        self.ws_pad = np.zeros(np.array(shape) + 2, dtype=np.int16)
        self.ws_pad[1:-1, 1:-1, 1:-1] = self.ws.matrix

        if self.need_to_orient:
            self.orient_pad = np.zeros((np.array(shape) + 2).tolist() + [3], dtype=float)
            self.orient_pad[1:-1, 1:-1, 1:-1, :] = self.ws.orientation.transpose(reorder + [3])[:, :, :, reorder]

        pad_domain_cy(self.ws_pad, self.orient_pad, self.need_to_orient, self.len_x, self.len_y, self.len_z, self.side_bc)

        if self.solver_type != 'direct' and self.solver_type != 'spsolve':
            self.initial_guess = np.zeros((self.len_x, self.len_y, self.len_z), dtype=float)
            for i in range(self.len_x):
                self.initial_guess[i, :, :] = i / (self.len_x - 1)
            self.initial_guess = self.initial_guess.flatten('F')

    def compute_Kmat(self, i, i_cv):

        # Reset layer
        self.Kmat[i].fill(0)

        for key, value in self.mat_cond.items():
            mask = self.ws_pad[i_cv] == key
            if len(value) == 6:  # passing input conductivity
                self.Kmat[i, mask] = value
            else:  # tensor rotation of input conductivity with orientation
                phi = np.arctan2(self.orient_pad[i_cv, mask, 1], self.orient_pad[i_cv, mask, 0])
                theta = np.arcsin(self.orient_pad[i_cv, mask, 2])

                size = np.sum(mask)
                Rz_kinit = np.zeros((size, 3, 3), dtype=float)
                Ry_krot = np.zeros((size, 3, 3), dtype=float)

                Rz_kinit[:, 0, 0] = np.cos(phi)
                Rz_kinit[:, 1, 1] = Rz_kinit[:, 0, 0]
                Rz_kinit[:, 1, 0] = np.sin(phi)
                Rz_kinit[:, 0, 1] = -Rz_kinit[:, 1, 0]
                Rz_kinit[:, 2, 2] = 1
                Ry_krot[:, 1, 1] = 1
                Ry_krot[:, 0, 0] = np.cos(theta)
                Ry_krot[:, 2, 2] = Ry_krot[:, 0, 0]
                Ry_krot[:, 0, 2] = np.sin(theta)
                Ry_krot[:, 2, 0] = -Ry_krot[:, 0, 2]

                R = Rz_kinit @ Ry_krot

                Rz_kinit.fill(0)
                Rz_kinit[:, [0, 1, 2], [0, 1, 2]] = [value[0], value[1], value[1]]

                Ry_krot = R @ Rz_kinit @ R.transpose(0, 2, 1)
                self.Kmat[i, mask] = Ry_krot[:, [0, 1, 2, 0, 0, 1], [0, 1, 2, 1, 2, 2]]

    def initialize_mpfa(self):

        # Initialize matrix slice of conductivities (per CV)
        self.Kmat = np.zeros((2, self.len_y + 2, self.len_z + 2, 6), dtype=float)
        self.compute_Kmat(0, 0)
        self.compute_Kmat(1, 1)

        # Initialize mpfa variables (variables per IV)
        self.Eu = np.zeros((2, self.len_y + 1, self.len_z + 1, 12, 8), dtype=float)
        self.Ed = np.zeros((2, self.len_y + 1, self.len_z + 1, 12, 1), dtype=float)
        self.mpfa12x12 = np.zeros((self.len_y + 1, self.len_z + 1, 12, 12), dtype=float)
        self.kf = np.zeros((48, self.len_y + 1, self.len_z + 1), dtype=float)
        self.Td = np.zeros((12, self.len_y + 1, self.len_z + 1), dtype=float)
        self.not_dir_x = np.ones((8, self.len_y + 1, self.len_z + 1), dtype=np.int8)
        self.not_dir_y = np.ones((8, self.len_y + 1, self.len_z + 1), dtype=np.int8)
        self.not_dir_z = np.ones((8, self.len_y + 1, self.len_z + 1), dtype=np.int8)

        # compute first layer of transmissibility
        self.compute_transmissibility(0, 0)
        self.Kmat[0] = self.Kmat[1]

    def compute_transmissibility(self, i, i_cv_in):
        i_iv = i_cv_in + i

        # flattening Cmat per IVs
        for j_iv in range(self.len_y + 1):
            js = j_iv + self.kf_j_inds
            for k_iv in range(self.len_z + 1):
                ks = k_iv + self.kf_k_inds
                self.kf[:, j_iv, k_iv] = self.Kmat[self.kf_i_inds, js, ks].ravel()

        # Td arranged as:
        # Pe: 0  Ne: 1  Te: 2   TNe: 3
        # Pn: 4  En: 5  Tn: 6   TEn: 7
        # Pt: 8  Et: 9  Nt: 10  NEt: 11
        # not_dir_xyz arranged as:
        # P: 0   E: 1   N: 2   NE: 3
        # T: 4  TE: 5  TN: 6  TNE: 7
        self.not_dir_x.fill(1)
        self.not_dir_y.fill(1)
        self.not_dir_z.fill(1)
        self.Td.fill(0)
        if self.direction == '':
            assign_prescribed_bc_cy(self.not_dir_x, self.not_dir_y, self.not_dir_z, self.Td,
                                    self.dirichlet_bc.xfaces, self.dirichlet_bc.yfaces, self.dirichlet_bc.zfaces,
                                    self.len_x, self.len_y, self.len_z, i_iv)
        else:
            if i_iv == 0:
                self.not_dir_x[[0, 2, 4, 6]] = 0
            elif i_iv == self.len_x:
                self.not_dir_x[[1, 3, 5, 7]] = 0
                self.Td[[0, 1, 2, 3]] = 1

        self.Eu[i].fill(0)
        self.Ed[i].fill(0)
        self.mpfa12x12.fill(0)

        # mpfa36x36 = C
        self.mpfa12x12[:, :, self.Cind[0], self.Cind[1]] = fill_Cmpfa(self.kf, self.not_dir_x, self.not_dir_y, self.not_dir_z)

        # mpfa36x36 = Cinv
        try:
            self.mpfa12x12[:] = np.linalg.inv(self.mpfa12x12)
        except np.linalg.LinAlgError:
            raise Exception("Singular MPFA matrix: remember that a phase cannot have conductivity of exactly 0 (set it to a low value e.g. 1e-5).")

        # Eu = D
        self.Eu[i, :, :, self.Dind[0], self.Dind[1]] = fill_Dmpfa(self.kf)

        # Eu = Cinv * D
        self.Eu[i] = self.mpfa12x12 @ self.Eu[i]

        # Ed = Cinv * d1
        self.Ed[i] = self.mpfa12x12 @ create_d1_mpfa(self.kf, self.Td)

        # mpfa12x12 = A
        self.mpfa12x12.fill(0)
        self.mpfa12x12[:, :, self.Aind[0], self.Aind[1]] = fill_Ampfa(self.kf, self.not_dir_x, self.not_dir_y, self.not_dir_z)

        # Eu = A * Eu = A * (Cinv * D)
        self.Eu[i] = self.mpfa12x12 @ self.Eu[i]

        # Ed = A * Ed = A * (Cinv * d1)
        self.Ed[i] = self.mpfa12x12 @ self.Ed[i]
        self.Ed[i] += create_d2_mpfa(self.kf, self.Td)

        # mpfa12x12 = B
        self.mpfa12x12[:, :, :, :8].fill(0)
        self.mpfa12x12[:, :, self.Bind[0], self.Bind[1]] = fill_Bmpfa(self.kf)

        # Eu = Eu + B = A * (Cinv * D) + B
        self.Eu[i] += self.mpfa12x12[:, :, :, :8]

        # to avoid all the *2 in mpfa_matrices
        self.Eu[i] *= 2
        self.Ed[i] *= 2

    def assemble_matrices(self):
        print("Creating system:")
        print("Initializing large data structures ... ", flush=True, end='')
        I_A, J_A = np.zeros((2, 27 * self.len_xyz), dtype=np.uint32)
        V_A = np.zeros(27 * self.len_xyz, dtype=float)
        A_layer_n = 27 * self.len_y * self.len_z
        counter_A = 0
        I_b = np.zeros(self.len_xyz, dtype=np.uint32)
        V_b = np.zeros(self.len_xyz, dtype=float)
        b_layer_n = self.len_y * self.len_z
        counter_b = 0
        self.initialize_mpfa()
        print("Done")

        for i_cv in range(self.len_x):
            sys.stdout.write("\rCreating indices ... {:.1f}% ".format(max(1, i_cv) / max(1, self.len_x - 1) * 100))

            self.compute_Kmat(1, i_cv + 2)
            self.compute_transmissibility(1, i_cv)

            # x,y,z divergence equations for P control volume
            # local_b
            V_b[counter_b:counter_b + b_layer_n] = -div_Ed_mpfa(self.Ed[0, :-1, :-1, :, 0], self.Ed[1, :-1, :-1, :, 0],
                                                                self.Ed[0, 1:, :-1, :, 0], self.Ed[1, 1:, :-1, :, 0],
                                                                self.Ed[0, :-1, 1:, :, 0], self.Ed[1, :-1, 1:, :, 0],
                                                                self.Ed[0, 1:, 1:, :, 0], self.Ed[1, 1:, 1:, :, 0]).ravel()

            # local_A
            V_A[counter_A:counter_A + A_layer_n] = div_Eu_mpfa(self.Eu[0, :-1, :-1], self.Eu[1, :-1, :-1],
                                                               self.Eu[0, 1:, :-1], self.Eu[1, 1:, :-1],
                                                               self.Eu[0, :-1, 1:], self.Eu[1, :-1, 1:],
                                                               self.Eu[0, 1:, 1:], self.Eu[1, 1:, 1:]).transpose((1, 2, 0)).ravel()

            create_Ab_indices_cy(I_A, J_A, I_b, counter_A, counter_b, i_cv,
                                 self.len_x, self.len_y, self.len_z, self.len_xyz, self.side_bc)

            counter_A += A_layer_n
            counter_b += b_layer_n

            # Passing layers
            self.Eu[0] = self.Eu[1]
            self.Ed[0] = self.Ed[1]
            self.Kmat[0] = self.Kmat[1]

        # delete mpfa variables
        del self.Kmat, self.Eu, self.Ed, self.mpfa12x12, self.kf, self.Td, self.not_dir_x, self.not_dir_y, self.not_dir_z

        print("\nAssembling sparse matrices ... ", end='')
        nonzero_V = V_A != 0
        self.Amat = coo_matrix((V_A[nonzero_V], (I_A[nonzero_V], J_A[nonzero_V])),
                               shape=(self.len_xyz, self.len_xyz)).tocsr()
        del I_A, J_A, V_A

        nonzero_V = V_b != 0
        self.bvec = coo_matrix((V_b[nonzero_V], (I_b[nonzero_V], np.zeros_like(I_b[nonzero_V]))),
                               shape=(self.len_xyz, 1)).tocsr()
        del I_b, V_b

        # Jacobi preconditioner
        diag = self.Amat.diagonal()
        if np.any(diag == 0):
            self.M = None  # identity matrix if singularity has happened in mpfa
        else:
            self.M = diags(1. / diag, 0).tocsr()
        del diag

        print("Done")

    def compute_fluxes(self):

        # reduced system
        self.T = self.x.reshape([self.len_x, self.len_y, self.len_z], order='F')
        del self.x

        # Initialize required data structures
        self.q = np.zeros((self.len_x, self.len_y, self.len_z, 3))

        self.initialize_mpfa()

        t_sw, t_se, t_nw, t_ne, t_tsw, t_tse, t_tnw, t_tne = np.zeros((8, self.len_y, self.len_z, 8))
        tf = np.zeros(27)

        for i_cv in range(self.len_x):
            sys.stdout.write("\rComputing fluxes ... {:.1f}% ".format(max(1, i_cv) / max(1, self.len_x - 1) * 100))

            self.compute_Kmat(1, i_cv + 2)
            self.compute_transmissibility(1, i_cv)

            create_T_ivs_cy(self.T, tf, i_cv, self.len_x, self.len_y, self.len_z, self.len_xyz, self.side_bc,
                            t_sw, t_se, t_nw, t_ne, t_tsw, t_tse, t_tnw, t_tne)

            # Computing: stresses = Eu @ T + Ed
            q_sw  = np.einsum('ijko,jko->ijk', self.Eu[0, :-1, :-1, self.inds_sw],  t_sw)  + self.Ed[0, :-1, :-1, self.inds_sw, 0]
            q_se  = np.einsum('ijko,jko->ijk', self.Eu[1, :-1, :-1, self.inds_se],  t_se)  + self.Ed[1, :-1, :-1, self.inds_se, 0]
            q_nw  = np.einsum('ijko,jko->ijk', self.Eu[0, 1:, :-1,  self.inds_nw],  t_nw)  + self.Ed[0, 1:, :-1,  self.inds_nw, 0]
            q_ne  = np.einsum('ijko,jko->ijk', self.Eu[1, 1:, :-1,  self.inds_ne],  t_ne)  + self.Ed[1, 1:, :-1,  self.inds_ne, 0]
            q_tsw = np.einsum('ijko,jko->ijk', self.Eu[0, :-1, 1:,  self.inds_tsw], t_tsw) + self.Ed[0, :-1, 1:,  self.inds_tsw, 0]
            q_tse = np.einsum('ijko,jko->ijk', self.Eu[1, :-1, 1:,  self.inds_tse], t_tse) + self.Ed[1, :-1, 1:,  self.inds_tse, 0]
            q_tnw = np.einsum('ijko,jko->ijk', self.Eu[0, 1:, 1:,   self.inds_tnw], t_tnw) + self.Ed[0, 1:, 1:,   self.inds_tnw, 0]
            q_tne = np.einsum('ijko,jko->ijk', self.Eu[1, 1:, 1:,   self.inds_tne], t_tne) + self.Ed[1, 1:, 1:,   self.inds_tne, 0]

            # Summing stresses CV-wise
            self.q[i_cv, :, :, 0] = (q_se[0] + q_ne[0] + q_tse[0] + q_tne[0] + q_sw[0] + q_nw[0] + q_tsw[0] + q_tnw[0])
            self.q[i_cv, :, :, 1] = (q_nw[1] + q_ne[1] + q_tnw[1] + q_tne[1] + q_sw[1] + q_se[1] + q_tsw[1] + q_tse[1])
            self.q[i_cv, :, :, 2] = (q_tsw[2] + q_tse[2] + q_tnw[2] + q_tne[2] + q_sw[2] + q_se[2] + q_nw[2] + q_ne[2])

            # Passing layers
            self.Eu[0] = self.Eu[1]
            self.Ed[0] = self.Ed[1]
            self.Kmat[0] = self.Kmat[1]

        # delete mpfa variables
        del self.Kmat, self.Eu, self.Ed, self.mpfa12x12, self.kf, self.Td, self.not_dir_x, self.not_dir_y, self.not_dir_z

        self.q /= 8 * self.ws.voxel_length

    def compute_effective_coefficient(self):

        # Accumulating and volume averaging fluxes
        fluxes = [np.sum(self.q[:, :, :, i]) / self.len_xyz for i in range(3)]
        self.keff = [fluxes[i] * self.len_x * self.ws.voxel_length for i in range(3)]

        # Rotating output back
        if self.direction == 'y':
            self.T = self.T.transpose(1, 0, 2)
            self.q = self.q.transpose(1, 0, 2, 3)[:, :, :, [1, 0, 2]]
            self.keff = [self.keff[1], self.keff[0], self.keff[2]]
        elif self.direction == 'z':
            self.T = self.T.transpose(2, 1, 0)
            self.q = self.q.transpose(2, 1, 0, 3)[:, :, :, [2, 1, 0]]
            self.keff = [self.keff[2], self.keff[1], self.keff[0]]

    def log_input(self):
        self.ws.log.log_section("Computing Anisotropic Conductivity")
        self.ws.log.log_line("Simulation direction: " + str(self.direction))
        self.ws.log.log_line("Domain Size: " + str(self.ws.get_shape()))
        self.ws.log.log_line("Conductivity Map: ")
        for i in range(self.cond_map.get_size()):
            low, high, cond = self.cond_map.get_material(i)
            self.ws.log.log_line("  - Material " + str(i) + "[" + str(low) + "," + str(high) + "," + str(cond) + "]")
        self.ws.log.log_line("Solver Type: " + str(self.solver_type))
        self.ws.log.log_line("Solver Tolerance: " + str(self.tolerance))
        self.ws.log.log_line("Max Iterations: " + str(self.maxiter))
        self.ws.log.write_log()

    def log_output(self):
        self.ws.log.log_section("Finished Anisotropic Conductivity Calculation")
        self.ws.log.log_line("Conductivity: " + "[" + str(self.keff) + "]")
        self.ws.log.log_line("Solver Time: " + str(self.solve_time))
        self.ws.log.write_log()

    def error_check(self):
        # cond_map checks
        ws_tmp_tocheck = self.ws.matrix.copy()
        for i in range(self.cond_map.get_size()):
            low, high, k = self.cond_map.get_material(i)
            self.mat_cond[low] = np.array(k)
            if len(k) == 2:
                self.need_to_orient = True
                if self.ws.orientation.shape[:3] != self.ws.matrix.shape or \
                        not 0.9 < np.min(np.linalg.norm(self.ws.orientation[np.logical_and(self.ws.matrix >= low,
                                                                                           self.ws.matrix <= high)], axis=1)) < 1.1:
                    raise Exception("The Workspace needs an orientation in order to align the conductivity.")

            # segmenting tmp domain to check if all values covered by mat_cond
            ws_tmp_tocheck[np.logical_and(self.ws.matrix >= low, self.ws.matrix <= high)] = low

        unique_matrixvalues = np.unique(ws_tmp_tocheck)
        if 0 in unique_matrixvalues and 0 not in self.mat_cond.keys():
            self.cond_map.add_isotropic_material((0, 0), 0., 0.)
            _, _, self.mat_cond[0] = self.cond_map.get_material(self.cond_map.get_size() - 1)
        if (unique_matrixvalues.size != len(self.mat_cond.keys()) or
                np.all(np.sort(list(self.mat_cond.keys())).astype(np.uint16) != unique_matrixvalues)):
            raise Exception("All values in workspace must be included in ConductivityMap.")

        # direction and prescribed_bc checks
        if self.direction != '':
            if self.direction.lower() in ['x', 'y', 'z']:
                self.direction = self.direction.lower()
            else:
                raise Exception("Invalid simulation direction, it can only be 'x', 'y', 'z', or '' for prescribed_bc.")
            if self.dirichlet_bc is not None:
                print_warning(f"{self.direction} Direction specified, prescribed_bc ignored.")
        else:
            if not isinstance(self.dirichlet_bc, AnisotropicConductivityBC):
                raise Exception("If prescribed_bc provided, the object needs to be a puma.AnisotropicConductivityBC object.")

        if self.tolerance > 1e-4:
            print_warning(f"{self.tolerance} tolerance value is large. 1e-4 or smaller is recommended")

        # side_bc checks
        if self.side_bc.lower() == "periodic" or self.side_bc == "p":
            self.side_bc = "p"
        elif self.side_bc.lower() == "symmetric" or self.side_bc == "s":
            self.side_bc = "s"
        else:
            raise Exception("Invalid side boundary conditions.")
