from pumapy.physics_models.finite_volume.elasticity_utils import pad_domain_cy, create_Ab_indices_cy, create_u_ivs_cy, assign_prescribed_bc_cy
from pumapy.physics_models.finite_volume.mpxa_matrices import (fill_Ampsa, fill_Bmpsa, fill_Cmpsa, fill_Dmpsa, create_d1_mpsa, create_d2_mpsa,
                                                               div_Eu_mpsa, div_Ed_mpsa, create_mpsa_indices)
from pumapy.physics_models.utils.linear_solvers import PropertySolver
from pumapy.physics_models.utils.boundary_conditions import ElasticityBC
from pumapy.utilities.timer import Timer
from pumapy.utilities.generic_checks import estimate_max_memory
from scipy.sparse import coo_matrix, diags
import numpy as np
import sys


class Elasticity(PropertySolver):

    def __init__(self, workspace, elast_map, direction, side_bc, tolerance, maxiter, solver_type, display_iter, dirichlet_bc):
        allowed_solvers = ['direct', 'gmres', 'bicgstab']
        super().__init__(workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter)

        self.elast_map = elast_map
        self.mat_elast = dict()
        self.direction = direction
        self.side_bc = side_bc

        self.dirichlet_bc = dirichlet_bc

        self.need_to_orient = False  # changes if (E_axial, E_radial, nu_poissrat_12, nu_poissrat_23, G12) detected
        self.u = None
        self.s = None
        self.t = None
        self.Ceff = None

        self.orient_pad = None

        # index arrays
        self.c_i_inds = np.tile(np.tile(np.array([0, 1], dtype=np.int8), 2), 2)
        self.c_j_inds = np.tile(np.repeat(np.array([0, 1], dtype=np.int8), 2), 2)
        self.c_k_inds = np.repeat(np.array([0, 1], dtype=np.int8), 4)
        self.Aind, self.Bind, self.Cind, self.Dind = create_mpsa_indices()
        self.inds_sw = [9, 22, 35, 34, 23, 33, 11, 21, 10]
        self.inds_se = [9, 19, 32, 31, 20, 30, 11, 18, 10]
        self.inds_nw = [6, 22, 29, 28, 23, 27, 8, 21, 7]
        self.inds_ne = [6, 19, 26, 25, 20, 24, 8, 18, 7]
        self.inds_tsw = [3, 16, 35, 34, 17, 33, 5, 15, 4]
        self.inds_tse = [3, 13, 32, 31, 14, 30, 5, 12, 4]
        self.inds_tnw = [0, 16, 29, 28, 17, 27, 2, 15, 1]
        self.inds_tne = [0, 13, 26, 25, 14, 24, 2, 12, 1]

    def compute(self):
        t = Timer()
        estimate_max_memory("elasticity", self.ws.matrix.shape, self.solver_type, self.need_to_orient)
        self.initialize()
        self.assemble_matrices()
        print("Time to assemble matrices: ", t.elapsed()); t.reset()
        super().solve()
        print("Time to solve: ", t.elapsed())
        self.compute_stresses()
        if self.direction != '':
            self.compute_effective_coefficient()
        self.solve_time = t.elapsed()

    def initialize(self):

        # Rotating domain to avoid having different cases and padding
        shape = [self.len_x, self.len_y, self.len_z]
        reorder = [0, 1, 2]
        if self.direction in ['y', 'z', 'yz', 'xz']:
            if self.direction == 'y':
                shape = [self.len_y, self.len_z, self.len_x]
                reorder = [1, 2, 0]
                a1, b1, g1, a2, b2, g2, a3, b3, g3 = (0, 1, 0,
                                                      0, 0, 1,
                                                      1, 0, 0)
            elif self.direction == 'z':
                shape = [self.len_z, self.len_x, self.len_y]
                reorder = [2, 0, 1]
                a1, b1, g1, a2, b2, g2, a3, b3, g3 = (0, 0, 1,
                                                      1, 0, 0,
                                                      0, 1, 0)
            elif self.direction == 'yz':
                shape = [self.len_z, self.len_y, self.len_x]
                reorder = [2, 1, 0]
                a1, b1, g1, a2, b2, g2, a3, b3, g3 = (0, 0, 1,
                                                      0, 1, 0,
                                                      1, 0, 0)
            else:  # xz
                shape = [self.len_x, self.len_z, self.len_y]
                reorder = [0, 2, 1]
                a1, b1, g1, a2, b2, g2, a3, b3, g3 = (1, 0, 0,
                                                      0, 0, 1,
                                                      0, 1, 0)

            # Rotating mat_elast
            C = np.zeros((6, 6), dtype=float)
            R = np.array([[a1 ** 2, a2 ** 2, a3 ** 2, -2 * a2 * a3, -2 * a3 * a1, -2 * a1 * a2],
                          [b1 ** 2, b2 ** 2, b3 ** 2, -2 * b2 * b3, -2 * b3 * b1, -2 * b1 * b2],
                          [g1 ** 2, g2 ** 2, g3 ** 2, -2 * g2 * g3, -2 * g3 * g1, -2 * g1 * g2],
                          [-b1 * g1, -b2 * g2, -b3 * g3, b2 * g3 + b3 * g2, b1 * g3 + b3 * g1, b1 * g2 + b2 * g1],
                          [-g1 * a1, -g2 * a2, -g3 * a3, g2 * a3 + g3 * a2, g1 * a3 + g3 * a1, g1 * a2 + g2 * a1],
                          [-a1 * b1, -a2 * b2, -a3 * b3, a2 * b3 + a3 * b2, a1 * b3 + a3 * b1, a1 * b2 + a2 * b1]])
            for key, value in self.mat_elast.items():
                if len(value) == 21:
                    C[np.triu_indices(6)] = value
                    C[np.tril_indices(6, -1)] = np.tril(C.T, -1)[np.tril_indices(6, -1)]
                    C = R.T @ C @ R
                    self.mat_elast[key] = C[np.triu_indices(6)]

            self.ws.matrix = self.ws.matrix.transpose(reorder)

        self.len_x, self.len_y, self.len_z = shape
        self.len_xy = self.len_x * self.len_y
        self.len_xyz = self.len_x * self.len_y * self.len_z

        # Segmenting domain according to elast_map
        for i in range(self.elast_map.get_size()):
            low, high, _ = self.elast_map.get_material(i)
            self.ws[np.logical_and(self.ws.matrix >= low, self.ws.matrix <= high)] = low

        # Padding domain, imposing symmetric or periodic BC on faces
        self.ws_pad = np.zeros(np.array(shape) + 2, dtype=np.int16)
        self.ws_pad[1:-1, 1:-1, 1:-1] = self.ws.matrix

        if self.need_to_orient:
            self.orient_pad = np.zeros((np.array(shape) + 2).tolist() + [3], dtype=float)
            self.orient_pad[1:-1, 1:-1, 1:-1, :] = self.ws.orientation.transpose(reorder + [3])[:, :, :, reorder]

        pad_domain_cy(self.ws_pad, self.orient_pad, self.need_to_orient, self.len_x, self.len_y, self.len_z, self.side_bc)

        if self.solver_type != 'direct' and self.solver_type != 'spsolve':
            self.initial_guess = np.zeros((self.len_x, self.len_y, self.len_z, 3), dtype=float)
            for i in range(self.len_x):
                self.initial_guess[i, :, :, 0] = i / max((self.len_x - 1), 1)
            self.initial_guess = self.initial_guess.flatten('F')

    def compute_Cmat(self, i, i_cv):

        # Reset layer of Cmat
        self.Cmat[i].fill(0)

        # Assigning elasticities throughout domain
        for key, value in self.mat_elast.items():
            mask = self.ws_pad[i_cv] == key
            if len(value) == 21:
                self.Cmat[i, mask] = value

            else:  # local elasticity orientation
                E1, E2, v12, v23, G12 = value
                v21 = v12 * E2 / E1
                delta = 1 - 2 * v12 * v21 - v23 * v23 - 2 * v21 * v12 * v23
                C_tmp = np.array([[((1 - v23 * v23) * E1) / delta, (v21 * (1 + v23) * E1) / delta, (v21 * (1 + v23) * E1) / delta, 0, 0, 0],
                                  [(v21 * (1 + v23) * E1) / delta, ((1 - v12 * v21) * E2) / delta, ((v23 + v21 * v12) * E2) / delta, 0, 0, 0],
                                  [(v21 * (1 + v23) * E1) / delta, ((v23 + v21 * v12) * E2) / delta, ((1 - v21 * v12) * E2) / delta, 0, 0, 0],
                                  [0, 0, 0, ((1 - v23 - 2 * v21 * v12) * E2) / delta, 0, 0],
                                  [0, 0, 0, 0, 2 * G12, 0],
                                  [0, 0, 0, 0, 0, 2 * G12]])
                size = np.sum(mask)
                C_init = np.repeat(C_tmp[:, :, np.newaxis], size, axis=2)

                # Rotation matrix
                theta = np.arctan2(self.orient_pad[i_cv, mask, 1], self.orient_pad[i_cv, mask, 0])
                a21 = -np.sin(theta)
                a22 = np.cos(theta)
                beta = np.arcsin(self.orient_pad[i_cv, mask, 2])
                a13 = np.sin(beta)
                a33 = np.cos(beta)
                a11 = a22 * a33
                a12 = - a21 * a33
                a31 = - a22 * a13
                a32 = a21 * a13
                a23 = np.zeros(size)
                R = np.array([[a11 ** 2, a12 ** 2, a13 ** 2, a12 * a13, a11 * a13, a11 * a12],
                              [a21 ** 2, a22 ** 2, a23, a23, a23, a21 * a22],
                              [a31 ** 2, a32 ** 2, a33 ** 2, a32 * a33, a33 * a31, a31 * a32],
                              [2 * a21 * a31, 2 * a32 * a22, a23, a22 * a33, a21 * a33, a21 * a32 + a22 * a31],
                              [2 * a11 * a31, 2 * a12 * a32, 2 * a13 * a33, a32 * a13 + a33 * a12, a11 * a33 + a13 * a31, a31 * a12 + a32 * a11],
                              [2 * a11 * a21, 2 * a12 * a22, a23, a13 * a22, a13 * a21, a11 * a22 + a12 * a21]])

                C_final = R.transpose((2, 1, 0)) @ C_init.transpose((2, 0, 1)) @ R.transpose((2, 0, 1))
                ind = np.triu_indices(6)
                self.Cmat[i, mask] = C_final[:, ind[0], ind[1]]

    def initialize_mpsa(self):

        # Initialize matrix slice of elasticities
        self.Cmat = np.zeros((2, self.len_y + 2, self.len_z + 2, 21), dtype=float)  # per CV
        self.compute_Cmat(0, 0)
        self.compute_Cmat(1, 1)

        # Initialize MPSA variables (variables per IV)
        self.Eu = np.zeros((2, self.len_y + 1, self.len_z + 1, 36, 24), dtype=float)
        self.Ed = np.zeros((2, self.len_y + 1, self.len_z + 1, 36, 1), dtype=float)
        self.mpsa36x36 = np.zeros((self.len_y + 1, self.len_z + 1, 36, 36), dtype=float)
        self.c = np.zeros((168, self.len_y + 1, self.len_z + 1), dtype=float)
        self.Dd = np.zeros((36, self.len_y + 1, self.len_z + 1), dtype=float)
        self.not_dir_x = np.ones((24, self.len_y + 1, self.len_z + 1), dtype=np.int8)
        self.not_dir_y = np.ones((24, self.len_y + 1, self.len_z + 1), dtype=np.int8)
        self.not_dir_z = np.ones((24, self.len_y + 1, self.len_z + 1), dtype=np.int8)

        # Computing first layer of E
        self.compute_transmissibility(0, 0)
        self.Cmat[0] = self.Cmat[1]

    def compute_transmissibility(self, i, i_cv_in):

        i_iv = i_cv_in + i

        # flattening Cmat per IVs
        for j_iv in range(self.len_y + 1):
            js = j_iv + self.c_j_inds
            for k_iv in range(self.len_z + 1):
                ks = k_iv + self.c_k_inds
                self.c[:, j_iv, k_iv] = self.Cmat[self.c_i_inds, js, ks].ravel()

        # Dd arranged as:
        # Pe: 0,1,2     Ne: 3,4,5     Te: 6,7,8     TNe: 9,10,11
        # Pn: 12,13,14  En: 15,16,17  Tn: 18,19,20  TEn: 21,22,23
        # Pt: 24,25,26  Et: 27,28,29  Nt: 30,31,32  NEt: 33,34,35
        # not_dir_xyz arranged as:
        # P: 0,8,16    E: 1,9,17    N: 2,10,18   NE: 3,11,19
        # T: 4,12,20  TE: 5,13,21  TN: 6,14,22  TNE: 7,15,23
        self.not_dir_x.fill(1)
        self.not_dir_y.fill(1)
        self.not_dir_z.fill(1)
        self.Dd.fill(0)
        if self.direction == '':
            assign_prescribed_bc_cy(self.not_dir_x, self.not_dir_y, self.not_dir_z, self.Dd,
                                    self.dirichlet_bc.xfaces, self.dirichlet_bc.yfaces, self.dirichlet_bc.zfaces,
                                    self.len_x, self.len_y, self.len_z, i_iv)
        else:
            if self.direction in ["x", "y", "z"]:
                if i_iv == 0:
                    self.not_dir_x[[0, 2, 4, 6]] = 0
                elif i_iv == self.len_x:
                    self.not_dir_x[[1, 3, 5, 7]] = 0
                    self.Dd[[0, 3, 6, 9]] = 1
            else:  # yz, xz, xy
                if i_iv == 0:
                    self.not_dir_x[[0 + 8, 2 + 8, 4 + 8, 6 + 8]] = 0
                if i_iv == self.len_x:
                    self.not_dir_x[[1 + 8, 3 + 8, 5 + 8, 7 + 8]] = 0
                    self.Dd[[1, 4, 7, 10]] = 1
                self.not_dir_y[[0, 1, 4, 5], 0] = 0
                self.not_dir_y[[2, 3, 6, 7], self.len_y] = 0
                self.Dd[[12, 15, 18, 21], self.len_y] = 1

        self.Eu[i].fill(0)
        self.Ed[i].fill(0)
        self.mpsa36x36.fill(0)

        # mpsa36x36 = C
        self.mpsa36x36[:, :, self.Cind[0], self.Cind[1]] = fill_Cmpsa(self.c, self.not_dir_x, self.not_dir_y, self.not_dir_z)

        # mpsa36x36 = Cinv
        try:
            self.mpsa36x36[:] = np.linalg.inv(self.mpsa36x36)
        except np.linalg.LinAlgError:
            raise Exception("Singular MPSA matrix: remember that air cannot be exactly 0 (set it to a low value e.g. 1e-5).")

        # Eu = D
        self.Eu[i, :, :, self.Dind[0], self.Dind[1]] = fill_Dmpsa(self.c)

        # Eu = Cinv * D
        self.Eu[i] = self.mpsa36x36 @ self.Eu[i]

        # Ed = Cinv * d1
        self.Ed[i] = self.mpsa36x36 @ create_d1_mpsa(self.c, self.Dd)

        # mpsa36x36 = A
        self.mpsa36x36.fill(0)
        self.mpsa36x36[:, :, self.Aind[0], self.Aind[1]] = fill_Ampsa(self.c, self.not_dir_x, self.not_dir_y, self.not_dir_z)

        # Eu = A * Eu = A * (Cinv * D)
        self.Eu[i] = self.mpsa36x36 @ self.Eu[i]

        # Ed = A * Ed = A * (Cinv * d1)
        self.Ed[i] = self.mpsa36x36 @ self.Ed[i]
        self.Ed[i] += create_d2_mpsa(self.c, self.Dd)

        # mpsa36x36 = B
        self.mpsa36x36[:, :, :, :24].fill(0)
        self.mpsa36x36[:, :, self.Bind[0], self.Bind[1]] = fill_Bmpsa(self.c)

        # Eu = Eu + B = A * (Cinv * D) + B
        self.Eu[i] += self.mpsa36x36[:, :, :, :24]

    def assemble_matrices(self):
        print("Creating system:")
        print("Initializing large data structures ... ", flush=True, end='')
        I_A, J_A = np.zeros((2, 81 * 3 * self.len_xyz), dtype=np.uint32)
        V_A = np.zeros(81 * 3 * self.len_xyz, dtype=float)
        A_layer_n = 3 * 81 * self.len_y * self.len_z
        counter_A = 0
        I_b = np.zeros(3 * self.len_xyz, dtype=np.uint32)
        V_b = np.zeros(3 * self.len_xyz, dtype=float)
        b_layer_n = 3 * self.len_y * self.len_z
        counter_b = 0
        self.initialize_mpsa()
        print("Done")

        for i_cv in range(self.len_x):
            sys.stdout.write("\rCreating indices ... {:.1f}% ".format(max(1, i_cv) / max(1, self.len_x - 1) * 100))

            self.compute_Cmat(1, i_cv + 2)
            self.compute_transmissibility(1, i_cv)

            # x,y,z divergence equations for P control volume
            # local_b
            V_b[counter_b:counter_b + b_layer_n] = \
                -div_Ed_mpsa(self.Ed[0, :-1, :-1, :, 0], self.Ed[1, :-1, :-1, :, 0],
                        self.Ed[0, 1:, :-1, :, 0], self.Ed[1, 1:, :-1, :, 0],
                        self.Ed[0, :-1, 1:, :, 0], self.Ed[1, :-1, 1:, :, 0],
                        self.Ed[0, 1:, 1:, :, 0], self.Ed[1, 1:, 1:, :, 0]).transpose((1, 2, 0)).ravel()

            # local_A
            V_A[counter_A:counter_A + A_layer_n] = \
                div_Eu_mpsa(self.Eu[0, :-1, :-1], self.Eu[1, :-1, :-1],
                       self.Eu[0, 1:, :-1], self.Eu[1, 1:, :-1],
                       self.Eu[0, :-1, 1:], self.Eu[1, :-1, 1:],
                       self.Eu[0, 1:, 1:], self.Eu[1, 1:, 1:]).transpose((2, 3, 0, 1)).ravel()

            create_Ab_indices_cy(I_A, J_A, I_b, counter_A, counter_b, i_cv,
                                 self.len_x, self.len_y, self.len_z, self.len_xyz, self.side_bc)

            counter_A += A_layer_n
            counter_b += b_layer_n

            # Passing layers
            self.Eu[0] = self.Eu[1]
            self.Ed[0] = self.Ed[1]
            self.Cmat[0] = self.Cmat[1]

        # delete mpsa variables
        del self.Cmat, self.Eu, self.Ed, self.mpsa36x36, self.c, self.Dd, self.not_dir_x, self.not_dir_y, self.not_dir_z

        print("\nAssembling sparse matrices ... ", end='')
        nonzero_V = V_A != 0
        self.Amat = coo_matrix((V_A[nonzero_V], (I_A[nonzero_V], J_A[nonzero_V])),
                               shape=(3 * self.len_xyz, 3 * self.len_xyz)).tocsr()
        del I_A, J_A, V_A

        nonzero_V = V_b != 0
        self.bvec = coo_matrix((V_b[nonzero_V], (I_b[nonzero_V], np.zeros_like(I_b[nonzero_V]))),
                               shape=(3 * self.len_xyz, 1)).tocsr()
        del I_b, V_b

        # Jacobi preconditioner
        diag = self.Amat.diagonal()
        if np.any(diag == 0):
            self.M = None  # identity matrix if singularity has happened in MPSA
        else:
            self.M = diags(1. / diag, 0).tocsr()
        del diag

        print("Done")

    def compute_stresses(self):

        self.u = self.x.reshape([self.len_x, self.len_y, self.len_z, 3], order='F')
        del self.x

        # Initialize required data structures
        self.s = np.zeros((self.len_x, self.len_y, self.len_z, 3))
        self.t = np.zeros((self.len_x, self.len_y, self.len_z, 3))

        self.initialize_mpsa()

        u_sw, u_se, u_nw, u_ne, u_tsw, u_tse, u_tnw, u_tne = np.zeros((8, self.len_y, self.len_z, 24))
        uf = np.zeros(81)

        # Compute stresses
        for i_cv in range(self.len_x):
            sys.stdout.write("\rComputing stresses ... {:.1f}% ".format(max(1, i_cv) / max(1, self.len_x - 1) * 100))

            self.compute_Cmat(1, i_cv + 2)
            self.compute_transmissibility(1, i_cv)

            create_u_ivs_cy(self.u, uf, i_cv, self.len_x, self.len_y, self.len_z, self.len_xyz, self.side_bc,
                            u_sw, u_se, u_nw, u_ne, u_tsw, u_tse, u_tnw, u_tne)

            # Computing: stresses = Eu @ u + End
            s_sw  = np.einsum('ijko,jko->ijk', self.Eu[0, :-1, :-1, self.inds_sw],  u_sw)  + self.Ed[0, :-1, :-1, self.inds_sw, 0]
            s_se  = np.einsum('ijko,jko->ijk', self.Eu[1, :-1, :-1, self.inds_se],  u_se)  + self.Ed[1, :-1, :-1, self.inds_se, 0]
            s_nw  = np.einsum('ijko,jko->ijk', self.Eu[0, 1:, :-1,  self.inds_nw],  u_nw)  + self.Ed[0, 1:, :-1,  self.inds_nw, 0]
            s_ne  = np.einsum('ijko,jko->ijk', self.Eu[1, 1:, :-1,  self.inds_ne],  u_ne)  + self.Ed[1, 1:, :-1,  self.inds_ne, 0]
            s_tsw = np.einsum('ijko,jko->ijk', self.Eu[0, :-1, 1:,  self.inds_tsw], u_tsw) + self.Ed[0, :-1, 1:,  self.inds_tsw, 0]
            s_tse = np.einsum('ijko,jko->ijk', self.Eu[1, :-1, 1:,  self.inds_tse], u_tse) + self.Ed[1, :-1, 1:,  self.inds_tse, 0]
            s_tnw = np.einsum('ijko,jko->ijk', self.Eu[0, 1:, 1:,   self.inds_tnw], u_tnw) + self.Ed[0, 1:, 1:,   self.inds_tnw, 0]
            s_tne = np.einsum('ijko,jko->ijk', self.Eu[1, 1:, 1:,   self.inds_tne], u_tne) + self.Ed[1, 1:, 1:,   self.inds_tne, 0]

            # Summing stresses CV-wise
            self.s[i_cv, :, :, 0] = (s_se[0] + s_ne[0] + s_tse[0] + s_tne[0] +
                                     s_sw[0] + s_nw[0] + s_tsw[0] + s_tnw[0])  # sigma_x
            self.s[i_cv, :, :, 1] = (s_nw[1] + s_ne[1] + s_tnw[1] + s_tne[1] +
                                     s_sw[1] + s_se[1] + s_tsw[1] + s_tse[1])  # sigma_y
            self.s[i_cv, :, :, 2] = (s_tsw[2] + s_tse[2] + s_tnw[2] + s_tne[2] +
                                     s_sw[2] + s_se[2] + s_nw[2] + s_ne[2])  # sigma_z

            self.t[i_cv, :, :, 0] = (s_tsw[3] + s_tse[3] + s_tnw[3] + s_tne[3] +
                                     s_sw[3] + s_se[3] + s_nw[3] + s_ne[3] +
                                     s_nw[4] + s_ne[4] + s_tnw[4] + s_tne[4] +
                                     s_sw[4] + s_se[4] + s_tsw[4] + s_tse[4])  # tau_yz
            self.t[i_cv, :, :, 1] = (s_tsw[5] + s_tse[5] + s_tnw[5] + s_tne[5] +
                                     s_sw[5] + s_se[5] + s_nw[5] + s_ne[5] +
                                     s_se[6] + s_ne[6] + s_tse[6] + s_tne[6] +
                                     s_sw[6] + s_nw[6] + s_tsw[6] + s_tnw[6])  # tau_xz
            self.t[i_cv, :, :, 2] = (s_nw[7] + s_ne[7] + s_tnw[7] + s_tne[7] +
                                     s_sw[7] + s_se[7] + s_tsw[7] + s_tse[7] +
                                     s_se[8] + s_ne[8] + s_tse[8] + s_tne[8] +
                                     s_sw[8] + s_nw[8] + s_tsw[8] + s_tnw[8])  # tau_xy

            # Passing layers
            self.Eu[0] = self.Eu[1]
            self.Ed[0] = self.Ed[1]
            self.Cmat[0] = self.Cmat[1]

        # delete mpsa variables
        del self.Cmat, self.Eu, self.Ed, self.mpsa36x36, self.c, self.Dd, self.not_dir_x, self.not_dir_y, self.not_dir_z

        self.s /= 8 * self.ws.voxel_length
        self.t /= 16 * self.ws.voxel_length

    def compute_effective_coefficient(self):

        # Accumulating and volume averaging stresses
        stresses = [np.sum(self.s[:, :, :, i]) / self.len_xyz for i in range(3)]
        stresses.extend([np.sum(self.t[:, :, :, i]) / self.len_xyz for i in range(3)])

        if self.direction in ["x", "y", "z"]:
            self.Ceff = [stresses[i] * self.len_x * self.ws.voxel_length for i in range(6)]
        else:
            self.Ceff = [stresses[i] / (0.5 / (self.len_x * self.ws.voxel_length) +
                                        0.5 / (self.len_y * self.ws.voxel_length)) for i in range(6)]

        # Rotating output back
        if self.direction == 'y':
            self.u = self.u.transpose(2, 0, 1, 3)[:, :, :, [2, 0, 1]]
            self.s = self.s.transpose(2, 0, 1, 3)[:, :, :, [2, 0, 1]]
            self.t = self.t.transpose(2, 0, 1, 3)[:, :, :, [2, 0, 1]]
            self.Ceff = [self.Ceff[2], self.Ceff[0], self.Ceff[1],
                         self.Ceff[5], self.Ceff[3], self.Ceff[4]]
        elif self.direction == 'z':
            self.u = self.u.transpose(1, 2, 0, 3)[:, :, :, [1, 2, 0]]
            self.s = self.s.transpose(1, 2, 0, 3)[:, :, :, [1, 2, 0]]
            self.t = self.t.transpose(1, 2, 0, 3)[:, :, :, [1, 2, 0]]
            self.Ceff = [self.Ceff[1], self.Ceff[2], self.Ceff[0],
                         self.Ceff[4], self.Ceff[5], self.Ceff[3]]

        elif self.direction == 'yz':
            self.u = self.u.transpose(2, 1, 0, 3)[:, :, :, [2, 1, 0]]
            self.s = self.s.transpose(2, 1, 0, 3)[:, :, :, [2, 1, 0]]
            self.t = self.t.transpose(2, 1, 0, 3)[:, :, :, [2, 1, 0]]
            self.Ceff = [self.Ceff[2], self.Ceff[1], self.Ceff[0],
                         self.Ceff[5], self.Ceff[4], self.Ceff[3]]
        elif self.direction == 'xz':
            self.u = self.u.transpose(0, 2, 1, 3)[:, :, :, [0, 2, 1]]
            self.s = self.s.transpose(0, 2, 1, 3)[:, :, :, [0, 2, 1]]
            self.t = self.t.transpose(0, 2, 1, 3)[:, :, :, [0, 2, 1]]
            self.Ceff = [self.Ceff[0], self.Ceff[2], self.Ceff[1],
                         self.Ceff[3], self.Ceff[5], self.Ceff[4]]

    def log_input(self):
        self.ws.log.log_section("Computing Elasticity")
        self.ws.log.log_line("Simulation direction: " + str(self.direction))
        self.ws.log.log_line("Domain Size: " + str(self.ws.get_shape()))
        self.ws.log.log_line("Elasticity Map: ")
        for i in range(self.elast_map.get_size()):
            low, high, cond = self.elast_map.get_material(i)
            self.ws.log.log_line("  - Material " + str(i) + "[" + str(low) + "," + str(high) + "," + str(cond) + "]")
        self.ws.log.log_line("Solver Type: " + str(self.solver_type))
        self.ws.log.log_line("Solver Tolerance: " + str(self.tolerance))
        self.ws.log.log_line("Max Iterations: " + str(self.maxiter))
        self.ws.log.write_log()

    def log_output(self):
        self.ws.log.log_section("Finished Elasticity Calculation")
        self.ws.log.log_line("Elasticity: " + "[" + str(self.Ceff) + "]")
        self.ws.log.log_line("Solver Time: " + str(self.solve_time))
        self.ws.log.write_log()

    def error_check(self):
        # elast_map checks
        ws_tmp_tocheck = self.ws.matrix.copy()
        for i in range(self.elast_map.get_size()):
            low, high, C = self.elast_map.get_material(i)
            self.mat_elast[low] = np.array(C)
            if len(C) == 5:
                self.need_to_orient = True
                if self.ws.orientation.shape[:3] != self.ws.matrix.shape or \
                        not 0.9 < np.min(np.linalg.norm(self.ws.orientation[np.logical_and(self.ws.matrix >= low,
                                                                                           self.ws.matrix <= high)],
                                                        axis=1)) < 1.1:
                    raise Exception("The Workspace needs an orientation in order to align the elasticity.")

            # segmenting tmp domain to check if all values covered by mat_elast
            ws_tmp_tocheck[np.logical_and(self.ws.matrix >= low, self.ws.matrix <= high)] = low

        unique_matrixvalues = np.unique(ws_tmp_tocheck)
        if (unique_matrixvalues.size != len(self.mat_elast.keys()) or
                np.all(np.sort(list(self.mat_elast.keys())).astype(np.uint16) != unique_matrixvalues)):
            raise Exception("All values in workspace must match the IDs in ElasticityMap.")

        # direction and prescribed_bc checks
        if self.direction != '':
            if self.direction.lower() in ['x', 'y', 'z', 'yz', 'xz', 'xy']:
                self.direction = self.direction.lower()
            else:
                raise Exception("Invalid simulation direction, it can only be 'x', 'y', 'z', 'yz', 'xz', 'xy'.")
        else:
            if not isinstance(self.dirichlet_bc, ElasticityBC):
                raise Exception("For compute_stress_analysis function, dirichlet_bc needs to be a puma.ElasticityBC object.")

        # side_bc checks
        if self.side_bc.lower() == "periodic" or self.side_bc == "p":
            self.side_bc = "p"
        elif self.side_bc.lower() == "symmetric" or self.side_bc == "s":
            self.side_bc = "s"
        else:
            raise Exception("Invalid side boundary conditions.")
