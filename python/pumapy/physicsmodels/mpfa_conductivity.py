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
from pumapy.physicsmodels.anisotropic_conductivity_utils import (pad_domain, add_nondiag, divP,
                                                                 fill_flux, flatten_Kmat)
from pumapy.physicsmodels.mpxa_matrices import fill_Ampfa, fill_Bmpfa, fill_Cmpfa, fill_Dmpfa, create_mpfa_indices
from pumapy.physicsmodels.conductivity_parent import Conductivity
from pumapy.utilities.timer import Timer
from pumapy.utilities.generic_checks import estimate_max_memory
from scipy.sparse import coo_matrix, diags
import numpy as np
import sys


class AnisotropicConductivity(Conductivity):

    def __init__(self, workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter, solver_type,
                 display_iter, print_matrices):
        super().__init__(workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter, solver_type, display_iter)
        self.print_matrices = print_matrices
        self.mat_cond = dict()
        self.need_to_orient = False  # changes if conductivities (k_axial, k_radial) detected
        self.orient_pad = None

    def compute(self):
        t = Timer()
        self.initialize()
        estimate_max_memory("anisotropic_conductivity", self.ws_pad[1:-1, 1:-1, 1:-1].shape, self.solver_type, self.need_to_orient)
        self.assemble_Amatrix()
        self.assemble_bvector()
        print("Time to assemble matrices: ", t.elapsed()); t.reset()
        super().solve()
        print("Time to solve: ", t.elapsed())
        self.compute_effective_coefficient()
        self.solve_time = t.elapsed()

    def initialize(self):
        print("Initializing and padding domains ... ", flush=True, end='')

        # Rotating domain to avoid cases and padding
        shape = [self.len_x + 2, self.len_y + 2, self.len_z + 2]
        reorder = [0, 1, 2]
        reorder_nondiagcond = [3, 4, 5]
        if self.direction == 'y':
            shape = [self.len_y + 2, self.len_x + 2, self.len_z + 2]
            reorder = [1, 0, 2]
            reorder_nondiagcond = [3, 5, 4]
        elif self.direction == 'z':
            shape = [self.len_z + 2, self.len_y + 2, self.len_x + 2]
            reorder = [2, 1, 0]
            reorder_nondiagcond = [5, 4, 3]

        self.ws_pad = np.zeros(shape, dtype=np.uint16)
        self.ws_pad[1:-1, 1:-1, 1:-1] = self.ws.matrix.transpose(reorder)

        if self.need_to_orient:
            self.orient_pad = np.zeros(shape + [3], dtype=float)
            self.orient_pad[1:-1, 1:-1, 1:-1, :] = self.ws.orientation.transpose(reorder + [3])[:, :, :, reorder]

        for key in self.mat_cond.keys():
            tmp_cond = list(self.mat_cond[key])
            if len(tmp_cond) == 6:
                self.mat_cond[key] = tuple([x for _, x in sorted(zip(reorder + reorder_nondiagcond, tmp_cond))])

        self.len_x, self.len_y, self.len_z = shape
        self.len_xy = self.len_x * self.len_y
        self.len_xyz = self.len_x * self.len_y * self.len_z

        # Padding domain, imposing symmetric or periodic BC on faces
        pad_domain(self.ws_pad, self.orient_pad, self.need_to_orient, self.len_x, self.len_y, self.len_z, self.side_bc)

        # Segmenting padded domain
        for i in range(self.cond_map.get_size()):
            low, high, _ = self.cond_map.get_material(i)
            self.ws_pad[(self.ws_pad >= low) & (self.ws_pad <= high)] = i

        # Placing True on dirichlet boundaries to skip them
        self.dir_vox = np.zeros(shape, dtype=np.uint8)
        self.dir_vox[[1, -2], 1:-1, 1:-1] = 1
        if self.prescribed_bc is not None:
            self.dir_vox[1:-1, 1:-1, 1:-1][self.prescribed_bc.dirichlet != np.Inf] = 1
        print("Done")

        # Initialize initial guess for iterative solver
        if self.solver_type != 'direct':
            self.initial_guess = np.zeros((self.len_x, self.len_y, self.len_z), dtype=float)
            for i in range(self.len_x - 1):
                self.initial_guess[i] = i / (self.len_x - 2.)
            self.initial_guess = self.initial_guess.flatten('F')

    def assemble_Amatrix(self):
        print("Initializing large data structures ... ", flush=True, end='')
        I, J = np.zeros((2, 27 * self.len_xyz), dtype=np.uint32)
        V = np.zeros(27 * self.len_xyz, dtype=float)
        counter = 0
        I_dirvox = []
        self.__initialize_MPFA()
        j_indices = np.zeros((27 * (self.len_y - 2) * (self.len_z - 2)), dtype=np.uint32)
        values = np.zeros((27 * (self.len_y - 2) * (self.len_z - 2)), dtype=float)
        print("Done")

        # Iterating through interior
        for i in range(1, self.len_x - 1):
            self.__compute_Kmat(2, i + 1)  # Computing third layer of Kmat
            self.__compute_transmissibility(1, i)  # Computing second layer of E

            # Creating j indices and divergence values for slice
            j_indices.fill(0)
            values.fill(np.NaN)

            # compute flux divergence: if a row of A is detected to be all zeros, then the voxel is flagged as dirichlet
            divP(i, self.len_x, self.len_y, self.len_z, self.dir_vox, j_indices, values, self.Emat)

            # Creating i indices for slice
            i_indices, i_dirvox = self.__creating_indices(i)
            if i_indices.size > 0:
                I[counter:counter + i_indices.size] = i_indices
            I_dirvox.extend(i_dirvox)

            if j_indices[j_indices != 0].size > 0:
                J[counter:counter + i_indices.size] = j_indices[~np.isnan(values)]
                V[counter:counter + i_indices.size] = values[~np.isnan(values)]
                counter += i_indices.size

            # Passing second layer to first
            self.Emat[0] = self.Emat[1]
            self.Kmat[:2] = self.Kmat[1:]
            sys.stdout.write("\rAssembling A matrix ... {:.1f}% ".format(i / (self.len_x - 2) * 100))

        # Clear unnecessary variables before creating A
        del self.Emat, self.kf, self.Kmat
        del self.dir_vox, i_indices, j_indices, values, i_dirvox, i
        del self.mpfa12x12, self.zeros, self.Aind, self.Cind, self.Dind

        # Adding all dirichlet voxels
        I[counter:counter + len(I_dirvox)] = I_dirvox
        J[counter:counter + len(I_dirvox)] = I_dirvox
        V[counter:counter + len(I_dirvox)] = 1
        counter += len(I_dirvox)
        del I_dirvox

        # Add diagonal 1s for exterior voxels
        diag_1s = np.ones_like(self.ws_pad, dtype=int)
        diag_1s[1:-1, 1:-1, 1:-1] = 0
        ind = np.array(np.where(diag_1s > 0))
        diag_1s = self.len_x * (self.len_y * ind[2] + ind[1]) + ind[0]
        diag_1s = diag_1s.astype(np.uint32)
        del ind
        I[counter:counter + diag_1s.size] = diag_1s
        J[counter:counter + diag_1s.size] = diag_1s
        V[counter:counter + diag_1s.size] = 1
        counter += diag_1s.size

        # Add non-diagonal 1s for exterior voxels
        if self.side_bc != "d":
            I[counter:counter + diag_1s.size] = diag_1s
            add_nondiag(diag_1s, self.len_x, self.len_y, self.len_z, self.side_bc)
            J[counter:counter + diag_1s.size] = diag_1s
            V[counter:counter + diag_1s.size] = -1
            counter += diag_1s.size
        del diag_1s, counter

        # Assemble sparse A matrix
        self.Amat = coo_matrix((V, (I, J)), shape=(self.len_xyz, self.len_xyz)).tocsr()

        # Simple preconditioner
        diag = self.Amat.diagonal()
        if np.any(diag == 0) or self.solver_type == "direct":
            self.M = None  # identity matrix if singularity has happened in MPFA
        else:
            self.M = diags(1. / self.Amat.diagonal(), 0).tocsr()

        if self.print_matrices[2]:
            self._print_A(self.print_matrices[2])
        print("Done")

    def assemble_bvector(self):
        print("Assembling b vector ... ", flush=True, end='')

        I, V = ([] for _ in range(2))

        if self.prescribed_bc is not None:
            for i in range(1, self.len_x - 1):
                for j in range(1, self.len_y - 1):
                    for k in range(1, self.len_z - 1):
                        if self.prescribed_bc[i - 1, j - 1, k - 1] != np.Inf:
                            I.append(self.len_x * (self.len_y * k + j) + i)
                            V.append(self.prescribed_bc[i - 1, j - 1, k - 1])
        else:
            # Setting unit temperature
            i = self.len_x - 2
            for j in range(1, self.len_y - 1):
                for k in range(1, self.len_z - 1):
                    I.append(self.len_x * (self.len_y * k + j) + i)
                    V.append(1.)

        # Setting linear temperature on the boundaries if Dirichlet
        if self.side_bc == 'd':
            x = np.linspace(0, 1, self.len_x - 2)
            for j in [1, self.len_y - 2]:
                for i in range(1, self.len_x - 1):
                    for k in range(1, self.len_z - 1):
                        I.append(self.len_x * (self.len_y * k + j) + i)
                        V.append(x[i - 1])
            for k in [1, self.len_z - 2]:
                for i in range(2, self.len_x - 2):
                    for j in range(2, self.len_y - 2):
                        I.append(self.len_x * (self.len_y * k + j) + i)
                        V.append(x[i - 1])

        self.bvec = coo_matrix((V, (I, np.zeros(len(I)))), shape=(self.len_xyz, 1)).tocsr()

        if self.print_matrices[0]:
            self._print_b(self.print_matrices[0])
        print("Done")

    def compute_effective_coefficient(self):
        # reshaping solution
        self.T = self.x.reshape([self.len_x, self.len_y, self.len_z], order='F')
        del self.x

        # Mirroring boundaries for flux computation
        self.T[0] = self.T[1]
        self.T[-1] = self.T[-2]
        if self.side_bc == "d":
            self.T[:, 0] = self.T[:, 1]
            self.T[:, -1] = self.T[:, -2]
            self.T[:, :, 0] = self.T[:, :, 1]
            self.T[:, :, -1] = self.T[:, :, -2]

        self.__compute_fluxes()

        # Accumulating and volume averaging stresses
        fluxes = [np.sum(self.q[:, :, :, i]) / ((self.len_x - 2) * (self.len_y - 2) * (self.len_z - 2))
                  for i in range(3)]
        self.keff = [-fluxes[i] * (self.len_x - 2) * self.ws.voxel_length for i in range(3)]

        # Rotating output back
        if self.direction == 'y':
            self.T = self.T.transpose(1, 0, 2)
            self.q = self.q.transpose(1, 0, 2, 3)[:, :, :, [1, 0, 2]]
            self.keff = [self.keff[1], self.keff[0], self.keff[2]]
        elif self.direction == 'z':
            self.T = self.T.transpose(2, 1, 0)
            self.q = self.q.transpose(2, 1, 0, 3)[:, :, :, [2, 1, 0]]
            self.keff = [self.keff[2], self.keff[1], self.keff[0]]

        d = {'x': 'first', 'y': 'second', 'z': 'third'}
        print(f'\nEffective conductivity tensor ({d[self.direction]} row): \n{self.keff}\n')

    def __compute_Kmat(self, i, i_cv):
        # reset layer of Cmat
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

                Ry_krot = R @ Rz_kinit @ np.linalg.inv(R)
                self.Kmat[i, mask] = Ry_krot[:, [0, 1, 2, 0, 0, 1], [0, 1, 2, 1, 2, 2]]

    def __initialize_MPFA(self):
        # Initialize matrix slice of conductivities
        self.Kmat = np.zeros((3, self.len_y, self.len_z, 6), dtype=float)  # per CV: kxx, kyy, kzz, kxy, kxz, kyz
        self.__compute_Kmat(0, 0)  # Computing first layer of Kmat
        self.__compute_Kmat(1, 1)  # Computing second layer of Kmat

        # Initialize MPFA variables
        self.kf = np.zeros((48, self.len_y - 1, self.len_z - 1), dtype=float)  # per IV
        self.Emat = np.zeros((2, self.len_y - 1, self.len_z - 1, 12, 8), dtype=float)
        self.mpfa12x12 = np.zeros((self.len_y - 1, self.len_z - 1, 12, 12), dtype=float)  # A, C
        self.zeros = np.zeros(self.kf[0].shape)
        self.Aind, self.Cind, self.Dind = create_mpfa_indices()
        self.__compute_transmissibility(0, 0)  # Computing first layer of E

    def __compute_transmissibility(self, i, i_cv):
        # reset layers
        self.Emat[i].fill(0)
        self.mpfa12x12.fill(0)
        self.kf.fill(0)
        flatten_Kmat(self.len_y, self.len_z, self.Kmat[i:i + 2], self.kf)

        # Computing transmissibility matrix as: A @ (Cinv @ D) + B
        # creating C
        self.mpfa12x12[:, :, self.Cind[0], self.Cind[1]] = fill_Cmpfa(self.kf)

        # C becomes singular in IVs with both air and solid --> stabilize those stress continuity equations
        inds_rows = np.where(self.mpfa12x12.sum(axis=3) == 0)
        inds_cols = np.where(self.mpfa12x12.sum(axis=2) == 0)
        self.mpfa12x12[inds_rows[0], inds_rows[1], inds_rows[2], inds_cols[2]] = 1.

        # creating D
        self.Emat[i, :, :, self.Dind[0], self.Dind[1]] = fill_Dmpfa(self.kf)

        # computing: Cinv
        self.mpfa12x12[:] = np.linalg.inv(self.mpfa12x12)

        # computing: Cinv @ D
        self.Emat[i] = self.mpfa12x12 @ self.Emat[i]

        # creating A
        self.mpfa12x12.fill(0)
        self.mpfa12x12[:, :, self.Aind[0], self.Aind[1]] = fill_Ampfa(self.kf)

        # computing: A @ (Cinv @ D)
        self.Emat[i] = self.mpfa12x12 @ self.Emat[i]

        # creating and adding B: A @ (Cinv @ D) + B
        self.Emat[i] += fill_Bmpfa(self.kf, self.zeros)

        if self.print_matrices[1]:
            self._print_E(i, i_cv, self.print_matrices[1])

    def __creating_indices(self, i):
        # Finding all indices for slice
        i_indices = np.ones_like(self.ws_pad[i], dtype=np.uint32)
        i_indices[[0, -1], :] = 0
        i_indices[:, [0, -1]] = 0
        i_indices = np.where(i_indices > 0)
        i_indices = self.len_x * (self.len_y * i_indices[1] + i_indices[0]) + np.full(i_indices[0].size, i)

        # Removing dirichlet voxels
        i_dirvox = np.where(self.dir_vox[i])
        i_dirvox = self.len_x * (self.len_y * i_dirvox[1] + i_dirvox[0]) + np.full(i_dirvox[0].size, i)
        i_indices = i_indices[~np.in1d(i_indices, i_dirvox)]

        # Duplicating the voxel indices where divergence happens
        i_indices = np.repeat(i_indices, 27)
        return i_indices, i_dirvox  # returning dirichlet voxel indices

    def __compute_fluxes(self):
        # Initialize required data structures
        self.q = np.zeros((self.len_x - 2, self.len_y - 2, self.len_z - 2, 3), dtype=float)
        self.__initialize_MPFA()
        T_sw, T_se, T_nw, T_ne, T_tsw, T_tse, T_tnw, T_tne = np.zeros((8, self.len_y - 2, self.len_z - 2, 8))
        E_sw, E_se, E_nw, E_ne, E_tsw, E_tse, E_tnw, E_tne = np.zeros((8, self.len_y - 2, self.len_z - 2, 12, 8))

        # Iterating through interior
        for i in range(1, self.len_x - 1):
            self.__compute_Kmat(2, i + 1)  # Computing third layer of Kmat
            self.__compute_transmissibility(1, i)  # Computing second layer of E

            # filling eight IVs
            fill_flux(i, self.len_x, self.len_y, self.len_z, self.T[i - 1:i + 2], self.Emat,
                      E_sw, E_se, E_nw, E_ne, E_tsw, E_tse, E_tnw, E_tne,
                      T_sw, T_se, T_nw, T_ne, T_tsw, T_tse, T_tnw, T_tne)

            # Computing fluxes by computing E x T
            q_sw = np.squeeze(E_sw @ T_sw[:, :, :, np.newaxis])
            q_se = np.squeeze(E_se @ T_se[:, :, :, np.newaxis])
            q_nw = np.squeeze(E_nw @ T_nw[:, :, :, np.newaxis])
            q_ne = np.squeeze(E_ne @ T_ne[:, :, :, np.newaxis])
            q_tsw = np.squeeze(E_tsw @ T_tsw[:, :, :, np.newaxis])
            q_tse = np.squeeze(E_tse @ T_tse[:, :, :, np.newaxis])
            q_tnw = np.squeeze(E_tnw @ T_tnw[:, :, :, np.newaxis])
            q_tne = np.squeeze(E_tne @ T_tne[:, :, :, np.newaxis])

            # Summing fluxes CV-wise
            self.q[i - 1, :, :, 0] = (q_se[:, :, 3] + q_ne[:, :, 2] + q_tse[:, :, 1] + q_tne[:, :, 0] +
                                      q_sw[:, :, 3] + q_nw[:, :, 2] + q_tsw[:, :, 1] + q_tnw[:, :, 0]) / 4
            self.q[i - 1, :, :, 1] = (q_nw[:, :, 7] + q_ne[:, :, 6] + q_tnw[:, :, 5] + q_tne[:, :, 4] +
                                      q_sw[:, :, 7] + q_se[:, :, 6] + q_tsw[:, :, 5] + q_tse[:, :, 4]) / 4
            self.q[i - 1, :, :, 2] = (q_tsw[:, :, 11] + q_tse[:, :, 10] + q_tnw[:, :, 9] + q_tne[:, :, 8] +
                                      q_sw[:, :, 11] + q_se[:, :, 10] + q_nw[:, :, 9] + q_ne[:, :, 8]) / 4

            # Passing second layer to first
            self.Emat[0] = self.Emat[1]
            self.Kmat[0:2] = self.Kmat[1:3]
            sys.stdout.write("\rComputing fluxes ... {:.1f}% ".format(i / (self.len_x - 2) * 100))
        del self.Emat, self.kf, self.Kmat

        # Extract only interior temperature, ignoring exterior used as bc
        self.T = self.T[1:-1, 1:-1, 1:-1]
        self.q /= self.ws.voxel_length

        if self.print_matrices[3]:
            print_T(self.T, self.print_matrices[3])
        if self.print_matrices[4]:
            print_flux(self.q, self.print_matrices[4])
        print("Done")

    def error_check(self):
        if Conductivity.error_check(self):
            return False

        # cond_map checks
        ws_tmp_tocheck = self.ws.matrix.copy()
        for i in range(self.cond_map.get_size()):
            low, high, k = self.cond_map.get_material(i)
            self.mat_cond[i] = k

            if len(k) == 2:
                self.need_to_orient = True
                if self.ws.orientation.shape[:3] != self.ws.matrix.shape or \
                        not 0.9 < np.min(np.linalg.norm(self.ws.orientation[(self.ws.matrix >= low) &
                                                                            (self.ws.matrix <= high)], axis=1)) < 1.1:
                    raise Exception("The Workspace needs an orientation in order to align the conductivities.")

            # segmenting tmp domain to check if all values covered by mat_cond
            ws_tmp_tocheck[(self.ws.matrix >= low) & (self.ws.matrix <= high)] = i

        unique_matrixvalues = np.unique(ws_tmp_tocheck)
        if (unique_matrixvalues.size != self.cond_map.get_size() or
                np.all(np.array(list(self.mat_cond.keys()), dtype=np.uint16) != unique_matrixvalues)):
            raise Exception("All values in workspace must be included in ConductivityMap.")

        # print_matrices checks
        if type(self.print_matrices) is not tuple or len(self.print_matrices) != 5:
            raise Exception("Print_matrices must be a tuple with 5 booleans.")
        return False

    # Printing functions of system matrices
    def _print_E(self, i, i_cv, dec=4):
        np.set_printoptions(precision=dec)
        for j in range(self.len_y - 1):
            for k in range(self.len_z - 1):
                print()
                print("index {},{},{}".format(i_cv, j, k))
                print(self.Emat[i, j, k])

    def _print_A(self, dec=4):
        np.set_printoptions(linewidth=10000)
        np.set_printoptions(threshold=sys.maxsize)
        np.set_printoptions(formatter={'float': lambda x: "{:.{}f}".format(x, dec).rstrip('0').rstrip('.')})
        print()
        print("A matrix:")
        print(self.Amat.toarray())

    def _print_b(self, dec=1):
        print()
        print("b vector:")
        for k in range(self.len_z):
            for i in range(self.len_x):
                for j in range(self.len_y):
                    print('{:.{}f}'.format(self.bvec[self.len_x * (self.len_y * k + j) + i, 0], dec), end=' ')
                print()
            print()


def print_T(temperature, dec=4):
    print()
    print("Temperature:")
    for k in range(temperature.shape[2]):
        for i in range(temperature.shape[0]):
            for j in range(temperature.shape[1]):
                print('{:.{}f}'.format(temperature[i, j, k], dec), end=' ')
            print()
        print()


def print_flux(flux, dec=4):
    print()
    print("Flux:")
    for k in range(flux.shape[2]):
        for i in range(flux.shape[0]):
            for j in range(flux.shape[1]):
                print('({:.{}f}, {:.{}f}, {:.{}f})'.format(flux[i, j, k, 0], dec,
                                                           flux[i, j, k, 1], dec,
                                                           flux[i, j, k, 2], dec), end=' ')
            print()
        print()
