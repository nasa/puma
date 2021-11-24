from pumapy.physicsmodels.anisotropic_conductivity_utils import pad_domain
from pumapy.physicsmodels.elasticity_utils import fill_stress_matrices, flatten_Cmat, add_nondiag, divP
from pumapy.physicsmodels.mpxa_matrices import fill_Ampsa, fill_Bmpsa, fill_Cmpsa, fill_Dmpsa, create_mpsa_indices
from pumapy.utilities.workspace import Workspace
from pumapy.physicsmodels.boundary_conditions import ElasticityBC
from pumapy.physicsmodels.linear_solvers import PropertySolver
from pumapy.utilities.timer import Timer
from pumapy.utilities.generic_checks import estimate_max_memory
from scipy.sparse import coo_matrix, diags
import numpy as np
import sys


class Elasticity(PropertySolver):
    def __init__(self, workspace, elast_map, direction, side_bc, prescribed_bc, tolerance, maxiter, solver_type,
                 display_iter, print_matrices):
        allowed_solvers = ['direct', 'gmres', 'cg', 'bicgstab']
        super().__init__(workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter)

        self.elast_map = elast_map
        self.direction = direction
        self.side_bc = side_bc
        self.prescribed_bc = prescribed_bc
        self.print_matrices = print_matrices
        self.mat_elast = dict()
        self.need_to_orient = False  # changes if (E_axial, E_radial, nu_poissrat_12, nu_poissrat_23, G12) detected
        self.orient_pad = None
        self.dir_cv = None

        self.Ceff = [-1., -1., -1.]
        self.solve_time = -1
        self.u = np.zeros([1, 1, 1])
        self.s = np.zeros([1, 1, 1, 3])
        self.t = np.zeros([1, 1, 1, 3])

    def compute(self):
        t = Timer()
        self.initialize()
        estimate_max_memory("elasticity", self.ws_pad[1:-1, 1:-1, 1:-1].shape, self.solver_type, self.need_to_orient)
        self.assemble_bvector()
        self.assemble_Amatrix()
        print("Time to assemble matrices: ", t.elapsed()); t.reset()
        super().solve()
        print("Time to solve: ", t.elapsed())
        self.compute_effective_coefficient()
        self.solve_time = t.elapsed()

    def initialize(self):
        print("Initializing and padding domains ... ", flush=True, end='')

        # Rotating domain to avoid having different cases and padding
        shape = [self.len_x + 2, self.len_y + 2, self.len_z + 2]
        reorder = [0, 1, 2]
        if self.direction == 'y' or self.direction == 'z':
            if self.direction == 'y':
                shape = [self.len_y + 2, self.len_z + 2, self.len_x + 2]
                reorder = [1, 2, 0]
                a1, b1, g1, a2, b2, g2, a3, b3, g3 = (0, 1, 0, 0, 0, 1, 1, 0, 0)
            else:
                shape = [self.len_z + 2, self.len_x + 2, self.len_y + 2]
                reorder = [2, 0, 1]
                a1, b1, g1, a2, b2, g2, a3, b3, g3 = (0, 0, 1, 1, 0, 0, 0, 1, 0)

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
                    self.mat_elast[key] = tuple(C[np.triu_indices(6)])

        self.ws_pad = np.zeros(shape, dtype=np.uint16)
        self.ws_pad[1:-1, 1:-1, 1:-1] = self.ws.matrix.transpose(reorder)

        if self.need_to_orient:
            self.orient_pad = np.zeros(shape + [3], dtype=float)
            self.orient_pad[1:-1, 1:-1, 1:-1, :] = self.ws.orientation.transpose(reorder + [3])[:, :, :, reorder]

        self.len_x, self.len_y, self.len_z = shape
        self.len_xy = self.len_x * self.len_y
        self.len_xyz = self.len_x * self.len_y * self.len_z

        if self.side_bc != "f":
            # Padding domain, imposing symmetric or periodic BC on faces
            pad_domain(self.ws_pad, self.orient_pad, self.need_to_orient, self.len_x, self.len_y, self.len_z, self.side_bc)

        # Segmenting padded domain
        for i in range(self.elast_map.get_size()):
            low, high, _ = self.elast_map.get_material(i)
            self.ws_pad[np.logical_and(self.ws_pad >= low, self.ws_pad <= high)] = low

        # Placing True on dirichlet boundaries to skip them
        self.dir_cv = np.zeros(shape + [3], dtype=bool)
        # ID=0 is reserved for gas phase, which is put as dirichlet (i.e. disp=0)
        gas_mask = self.ws_pad[1:-1, 1:-1, 1:-1] == 0
        self.ndofs = 3 * (self.len_xyz - gas_mask.sum())
        self.dir_cv[1:-1, 1:-1, 1:-1][gas_mask] = True
        if self.direction is not None:
            self.dir_cv[[1, -2], 1:-1, 1:-1] = True
        if self.prescribed_bc is not None:
            self.dir_cv[1:-1, 1:-1, 1:-1][self.prescribed_bc.dirichlet != np.Inf] = True
        print("Done")

        # Initialize initial guess for iterative solver
        if self.solver_type != 'direct' and self.solver_type != 'spsolve':
            self.initial_guess = np.zeros((self.len_x, self.len_y, self.len_z, 3), dtype=float)
            for i in range(self.len_x - 1):
                self.initial_guess[i, :, :, 0] = i / (self.len_x - 2.)
            self.initial_guess = self.initial_guess.flatten('F')

    def assemble_bvector(self):
        print("Assembling b vector ... ", flush=True, end='')

        I, V = ([] for _ in range(2))

        if self.prescribed_bc is not None:
            for i in range(1, self.len_x - 1):
                for j in range(1, self.len_y - 1):
                    for k in range(1, self.len_z - 1):
                        if self.prescribed_bc.dirichlet[i - 1, j - 1, k - 1, 0] != np.Inf:
                            I.append(self.len_x * (self.len_y * k + j) + i)
                            V.append(self.prescribed_bc.dirichlet[i - 1, j - 1, k - 1, 0])  # ux
                        if self.prescribed_bc.dirichlet[i - 1, j - 1, k - 1, 1] != np.Inf:
                            I.append(self.len_xyz + self.len_x * (self.len_y * k + j) + i)
                            V.append(self.prescribed_bc.dirichlet[i - 1, j - 1, k - 1, 1])  # uy
                        if self.prescribed_bc.dirichlet[i - 1, j - 1, k - 1, 2] != np.Inf:
                            I.append(2 * self.len_xyz + self.len_x * (self.len_y * k + j) + i)
                            V.append(self.prescribed_bc.dirichlet[i - 1, j - 1, k - 1, 2])  # uz
        else:
            # Setting unit displacement
            i = self.len_x - 2
            for j in range(1, self.len_y - 1):
                for k in range(1, self.len_z - 1):
                    I.append(self.len_x * (self.len_y * k + j) + i)
                    V.append(1.)

        # Setting linear displacement on the boundaries if Dirichlet
        if self.side_bc == 'd' and self.direction is not None:
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

        self.bvec = coo_matrix((V, (I, np.zeros(len(I)))), shape=(3 * self.len_xyz, 1)).tocsr()

        if self.print_matrices[0]:
            self._print_b(self.print_matrices[0])
        print("Done")

    def assemble_Amatrix(self):
        print("Initializing large data structures ... ", flush=True, end='')
        I, J = np.zeros((2, 81 * self.ndofs), dtype=np.uint32)
        V = np.zeros(81 * self.ndofs, dtype=float)
        counter = 0  # counter to keep record of the index in Amat
        I_dirvox = []
        self.__initialize_MPSA()
        j_indices = np.zeros((81 * 3 * (self.len_y - 2) * (self.len_z - 2)), dtype=np.uint32)
        values = np.zeros((81 * 3 * (self.len_y - 2) * (self.len_z - 2)), dtype=float)
        self.dir_cv = self.dir_cv.astype(np.uint8)
        print("Done")

        # Iterating through interior CVs
        for i in range(1, self.len_x - 1):
            self.__compute_Cmat(2, i + 1)  # Computing third layer of Cmat
            self.__compute_transmissibility(1, i)  # Computing second layer of E

            # Creating j indices and divergence values for slice
            j_indices.fill(-1)
            values.fill(np.NaN)
            divP(i, self.len_x, self.len_y, self.len_z, self.dir_cv, j_indices, values, self.Emat)

            # Creating i indices for slice
            i_indices, i_dirvox = self.__creating_indices(i)
            if i_indices.size > 0:
                I[counter:counter + i_indices.size] = i_indices
            I_dirvox.extend(i_dirvox)

            if j_indices[j_indices != -1].size > 0:
                J[counter:counter + i_indices.size] = j_indices[~np.isnan(values)]
                V[counter:counter + i_indices.size] = values[~np.isnan(values)]
                counter += i_indices.size

            # Passing second layer to first
            self.Emat[0] = self.Emat[1]
            self.Cmat[:2] = self.Cmat[1:]
            sys.stdout.write("\rAssembling A matrix ... {:.1f}% ".format(i / (self.len_x - 2) * 100))

        # Clear unnecessary variables before creating A
        del self.Emat, self.Cf, self.Cmat, self.mpsa36x36
        del self.dir_cv, i_indices, i_dirvox, i, j_indices, values

        # Adding all dirichlet voxels
        I[counter:counter + len(I_dirvox)] = I_dirvox
        J[counter:counter + len(I_dirvox)] = I_dirvox
        V[counter:counter + len(I_dirvox)] = 1
        counter += len(I_dirvox)
        del I_dirvox

        # Add diagonal 1s for exterior voxels
        diag_1s = np.ones_like(self.ws_pad, dtype=int)
        diag_1s[1:-1, 1:-1, 1:-1] = 0  # interior to 0
        ind = np.array(np.where(diag_1s > 0))  # indices of contour
        diag_1s = self.len_x * (self.len_y * ind[2] + ind[1]) + ind[0]
        diag_1s = np.hstack((diag_1s, self.len_xyz + diag_1s, 2 * self.len_xyz + diag_1s))
        diag_1s = diag_1s.astype(np.uint32)
        del ind
        I[counter:counter + diag_1s.size] = diag_1s
        J[counter:counter + diag_1s.size] = diag_1s
        V[counter:counter + diag_1s.size] = 1
        counter += diag_1s.size

        # Add non-diagonal 1s for exterior voxels
        if self.side_bc != "d" and self.side_bc != "f":
            I[counter:counter + diag_1s.size] = diag_1s
            nondiag1s = np.ones_like(diag_1s, dtype=np.int8)
            add_nondiag(diag_1s, nondiag1s, self.len_x, self.len_y, self.len_z, self.side_bc)
            J[counter:counter + diag_1s.size] = diag_1s  # CAREFUL: diag_1s reused for nondiag to optimize memory
            if self.side_bc == "s":
                V[counter:counter + diag_1s.size] = nondiag1s
            else:
                V[counter:counter + diag_1s.size] = -1
            del nondiag1s
        del diag_1s, counter

        # Assemble sparse A matrix
        self.Amat = coo_matrix((V, (I, J)), shape=(3 * self.len_xyz, 3 * self.len_xyz)).tocsr()

        # Simple preconditioner
        diag = self.Amat.diagonal()
        if np.any(diag == 0):
            self.M = None  # identity matrix if singularity has happened in MPSA
        else:
            self.M = diags(1. / self.Amat.diagonal(), 0).tocsr()

        # reduce system of eqs
        # self.I = np.where(abs(self.Amat).sum(1) + abs(self.bvec) != 1)[0].flatten() # np.delete(np.arange(3 * self.len_xyz), I_dirvox)
        # self.Amat = self.Amat[self.I][:, self.I]
        # self.bvec = self.bvec[self.I]
        # if self.solver_type != 'direct' and self.solver_type != 'spsolve':
        #     self.initial_guess = self.initial_guess[self.I]
        #     if self.M is not None:
        #         self.M = self.M[self.I][:, self.I]

        if self.print_matrices[2]:
            self._print_A(self.print_matrices[2])
        print("Done")

    def __initialize_MPSA(self):
        # Initialize matrix slice of conductivities
        self.Cmat = np.zeros((3, self.len_y, self.len_z, 21), dtype=float)  # per CV
        self.__compute_Cmat(0, 0)  # Computing first layer of Kmat
        self.__compute_Cmat(1, 1)  # Computing second layer of Kmat

        # Initialize MPSA variables (variables per IV)
        self.Cf = np.zeros((168, self.len_y - 1, self.len_z - 1), dtype=float)
        self.Emat = np.zeros((2, self.len_y - 1, self.len_z - 1, 36, 24), dtype=float)
        # self.A = np.zeros((self.len_y - 1, self.len_z - 1, 36, 36), dtype=float)
        # self.B = np.zeros((self.len_y - 1, self.len_z - 1, 36, 24), dtype=float)
        # self.C = np.zeros((self.len_y - 1, self.len_z - 1, 36, 36), dtype=float)
        # self.D = np.zeros((self.len_y - 1, self.len_z - 1, 36, 24), dtype=float)
        self.mpsa36x36 = np.zeros((self.len_y - 1, self.len_z - 1, 36, 36), dtype=float)  # A, C
        self.Aind, self.Cind, self.Dind = create_mpsa_indices()
        self.__compute_transmissibility(0, 0)  # Computing first layer of E

    def __compute_transmissibility(self, i, i_cv):
        # Reset layers
        self.Emat[i].fill(0)
        self.mpsa36x36.fill(0)
        self.Cf.fill(0)
        flatten_Cmat(i, self.len_y, self.len_z, self.Cmat[i:i + 2], self.Cf)

        # Equivalent procedure, but with more matrices instead of just one
        # (NB the computation results in very slightly different results because of float multiplication)
        # self.A.fill(0), self.B.fill(0), self.C.fill(0), self.D.fill(0)
        # self.A[:, :, self.Aind[0], self.Aind[1]] = fill_Ampsa(self.Cf)
        # self.B[:, :] = fill_Bmpsa(self.Cf)
        # self.C[:, :, self.Cind[0], self.Cind[1]] = fill_Cmpsa(self.Cf)
        # inds_rows = np.where(self.C.sum(axis=3) == 0)
        # inds_cols = np.where(self.C.sum(axis=2) == 0)
        # self.C[inds_rows[0], inds_rows[1], inds_rows[2], inds_cols[2]] = 1.
        # self.D[:, :, self.Dind[0], self.Dind[1]] = fill_Dmpsa(self.Cf).transpose((1, 2, 0))
        # self.Emat[i] = (self.B + self.A @ np.linalg.inv(self.C) @ self.D) / 8.

        # Computing transmissibility matrix as: (A @ (Cinv @ D) + B) / 8
        # creating C
        self.mpsa36x36[:, :, self.Cind[0], self.Cind[1]] = fill_Cmpsa(self.Cf)

        # C becomes singular in IVs with both air and solid --> stabilize those stress continuity equations
        inds_rows = np.where(self.mpsa36x36.sum(axis=3) == 0)
        inds_cols = np.where(self.mpsa36x36.sum(axis=2) == 0)
        self.mpsa36x36[inds_rows[0], inds_rows[1], inds_rows[2], inds_cols[2]] = 1.

        # creating D
        self.Emat[i, :, :, self.Dind[0], self.Dind[1]] = fill_Dmpsa(self.Cf)

        # computing: Cinv
        self.mpsa36x36[:] = np.linalg.inv(self.mpsa36x36)

        # computing: Cinv @ D
        self.Emat[i] = self.mpsa36x36 @ self.Emat[i]

        # creating A
        self.mpsa36x36.fill(0)
        self.mpsa36x36[:, :, self.Aind[0], self.Aind[1]] = fill_Ampsa(self.Cf)

        # computing: A @ (Cinv @ D)
        self.Emat[i] = self.mpsa36x36 @ self.Emat[i]

        # creating and adding B: (A @ (Cinv @ D) + B)/8
        self.Emat[i] += fill_Bmpsa(self.Cf)
        self.Emat[i] /= 8

        if self.print_matrices[1]:
            self._print_E(i, i_cv, self.print_matrices[1])

    def __creating_indices(self, i):
        # Finding all indices for slice
        i_indices = np.ones_like(self.ws_pad[i], dtype=np.uint32)
        i_indices[[0, -1], :] = 0
        i_indices[:, [0, -1]] = 0
        i_indices = np.where(i_indices > 0)
        i_indices = self.len_x * (self.len_y * i_indices[1] + i_indices[0]) + np.full(i_indices[0].size, i)
        i_indices = np.hstack((i_indices, self.len_xyz + i_indices, 2 * self.len_xyz + i_indices))

        # Removing dirichlet voxels
        i_dirvox = np.where(self.dir_cv[i, :, :, 0])
        i_dirvox = self.len_x * (self.len_y * i_dirvox[1] + i_dirvox[0]) + np.full(i_dirvox[0].size, i)
        i_dirvox1 = np.where(self.dir_cv[i, :, :, 1])
        i_dirvox1 = self.len_x * (self.len_y * i_dirvox1[1] + i_dirvox1[0]) + np.full(i_dirvox1[0].size, i)
        i_dirvox = np.hstack((i_dirvox, self.len_xyz + i_dirvox1))
        i_dirvox1 = np.where(self.dir_cv[i, :, :, 2])
        i_dirvox1 = self.len_x * (self.len_y * i_dirvox1[1] + i_dirvox1[0]) + np.full(i_dirvox1[0].size, i)
        i_dirvox = np.hstack((i_dirvox, 2 * self.len_xyz + i_dirvox1))
        i_indices = i_indices[~np.in1d(i_indices, i_dirvox)]

        # Duplicating the voxel indices where divergence happens
        i_indices = np.repeat(i_indices, 81)
        return i_indices, i_dirvox  # returning dirichlet voxel indices

    def compute_effective_coefficient(self):
        # reshaping solution
        self.u = self.x.reshape([self.len_x, self.len_y, self.len_z, 3], order='F')
        # self.u = np.zeros((self.len_x * self.len_y * self.len_z * 3))  # reduced system
        # self.u[self.I] = self.x
        # self.u = self.u.reshape([self.len_x, self.len_y, self.len_z, 3], order='F')
        del self.x

        # Mirroring boundaries for flux computation
        if self.direction is not None:
            self.u[0] = self.u[1]
            self.u[-1] = self.u[-2]
            if self.side_bc == "d" or self.side_bc == "f":
                self.u[:, 0] = self.u[:, 1]
                self.u[:, -1] = self.u[:, -2]
                self.u[:, :, 0] = self.u[:, :, 1]
                self.u[:, :, -1] = self.u[:, :, -2]
        if self.print_matrices[3]:
            show_u(self.u, self.print_matrices[3])

        self.__compute_stresses()

        if self.direction is not None:
            # Accumulating and volume averaging stresses
            stresses = [np.sum(self.s[:, :, :, i]) / ((self.len_x - 2) * (self.len_y - 2) * (self.len_z - 2)) for i in
                        range(3)]
            stresses += [np.sum(self.t[:, :, :, i]) / ((self.len_x - 2) * (self.len_y - 2) * (self.len_z - 2)) for i in
                         range(3)]
            self.Ceff = [stresses[i] * (self.len_x - 2) * self.ws.voxel_length for i in range(6)]

            # Rotating output back
            if self.direction == 'y':
                self.u = self.u.transpose(2, 0, 1, 3)[:, :, :, [2, 0, 1]]
                self.s = self.s.transpose(2, 0, 1, 3)[:, :, :, [2, 0, 1]]
                self.t = self.t.transpose(2, 0, 1, 3)[:, :, :, [2, 0, 1]]
                self.Ceff = [self.Ceff[2], self.Ceff[0], self.Ceff[1], self.Ceff[5], self.Ceff[3], self.Ceff[4]]
            elif self.direction == 'z':
                self.u = self.u.transpose(1, 2, 0, 3)[:, :, :, [1, 2, 0]]
                self.s = self.s.transpose(1, 2, 0, 3)[:, :, :, [1, 2, 0]]
                self.t = self.t.transpose(1, 2, 0, 3)[:, :, :, [1, 2, 0]]
                self.Ceff = [self.Ceff[1], self.Ceff[2], self.Ceff[0], self.Ceff[4], self.Ceff[5], self.Ceff[3]]

            d = {'x': 'first', 'y': 'second', 'z': 'third'}
            print(f'\nEffective elasticity tensor ({d[self.direction]} row): \n{self.Ceff}\n')

    def index_at(self, index, size):
        if self.side_bc == "p":
            if index == 0:
                return size - 1
            elif index == size:
                return 1
        else:
            if index == 0:
                return 1
            elif index == size:
                return size - 1
        return index

    def __compute_Cmat(self, i, i_cv):
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
                C_tmp = np.array([[((1 - v23 * v23) * E1) / delta, (v21 * (1 + v23) * E1) / delta,
                                   (v21 * (1 + v23) * E1) / delta, 0, 0, 0],
                                  [(v21 * (1 + v23) * E1) / delta, ((1 - v12 * v21) * E2) / delta,
                                   ((v23 + v21 * v12) * E2) / delta, 0, 0, 0],
                                  [(v21 * (1 + v23) * E1) / delta, ((v23 + v21 * v12) * E2) / delta,
                                   ((1 - v21 * v12) * E2) / delta, 0, 0, 0],
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
                              [2 * a21 * a31, 2 * a32 * a22, a23, a22 * a33,
                               a21 * a33, a21 * a32 + a22 * a31],
                              [2 * a11 * a31, 2 * a12 * a32, 2 * a13 * a33, a32 * a13 + a33 * a12,
                               a11 * a33 + a13 * a31, a31 * a12 + a32 * a11],
                              [2 * a11 * a21, 2 * a12 * a22, a23, a13 * a22,
                               a13 * a21, a11 * a22 + a12 * a21]])

                C_final = R.transpose((2, 1, 0)) @ C_init.transpose((2, 0, 1)) @ R.transpose((2, 0, 1))
                ind = np.triu_indices(6)
                self.Cmat[i, mask] = C_final[:, ind[0], ind[1]]

    def __compute_stresses(self):
        # Initialize required data structures
        self.s, self.t = np.zeros((2, self.len_x - 2, self.len_y - 2, self.len_z - 2, 3))
        self.__initialize_MPSA()
        u_sw, u_se, u_nw, u_ne, u_tsw, u_tse, u_tnw, u_tne = np.zeros((8, self.len_y - 2, self.len_z - 2, 24))  # per CV
        E_sw, E_se, E_nw, E_ne, E_tsw, E_tse, E_tnw, E_tne = np.zeros((8, self.len_y - 2, self.len_z - 2, 36, 24))

        # Computing first layer of E
        self.__compute_transmissibility(0, 0)

        # Iterating through interior
        for i in range(1, self.len_x - 1):
            self.__compute_Cmat(2, i + 1)  # Computing third layer of Cmat
            self.__compute_transmissibility(1, i)  # Computing second layer of E

            # filling eight IVs
            fill_stress_matrices(i, self.len_x, self.len_y, self.len_z, self.u[i - 1:i + 2], self.Emat,
                                 E_sw, E_se, E_nw, E_ne, E_tsw, E_tse, E_tnw, E_tne,
                                 u_sw, u_se, u_nw, u_ne, u_tsw, u_tse, u_tnw, u_tne)

            # Computing stresses by computing E @ u
            s_sw = np.squeeze(E_sw @ u_sw[:, :, :, np.newaxis])
            s_se = np.squeeze(E_se @ u_se[:, :, :, np.newaxis])
            s_nw = np.squeeze(E_nw @ u_nw[:, :, :, np.newaxis])
            s_ne = np.squeeze(E_ne @ u_ne[:, :, :, np.newaxis])
            s_tsw = np.squeeze(E_tsw @ u_tsw[:, :, :, np.newaxis])
            s_tse = np.squeeze(E_tse @ u_tse[:, :, :, np.newaxis])
            s_tnw = np.squeeze(E_tnw @ u_tnw[:, :, :, np.newaxis])
            s_tne = np.squeeze(E_tne @ u_tne[:, :, :, np.newaxis])

            # Summing fluxes CV-wise
            self.s[i - 1, :, :, 0] = (s_se[:, :, 9] + s_ne[:, :, 6] + s_tse[:, :, 3] + s_tne[:, :, 0] +
                                      s_sw[:, :, 9] + s_nw[:, :, 6] + s_tsw[:, :, 3] + s_tnw[:, :, 0])  # sigma_x
            self.s[i - 1, :, :, 1] = (s_nw[:, :, 21] + s_ne[:, :, 18] + s_tnw[:, :, 15] + s_tne[:, :, 12] +
                                      s_sw[:, :, 21] + s_se[:, :, 18] + s_tsw[:, :, 15] + s_tse[:, :, 12])  # sigma_y
            self.s[i - 1, :, :, 2] = (s_tsw[:, :, 33] + s_tse[:, :, 30] + s_tnw[:, :, 27] + s_tne[:, :, 24] +
                                      s_sw[:, :, 33] + s_se[:, :, 30] + s_nw[:, :, 27] + s_ne[:, :, 24])  # sigma_z

            self.t[i - 1, :, :, 0] = (s_tsw[:, :, 34] + s_tse[:, :, 31] + s_tnw[:, :, 28] + s_tne[:, :, 25] +
                                      s_sw[:, :, 34] + s_se[:, :, 31] + s_nw[:, :, 28] + s_ne[:, :, 25] +
                                      s_nw[:, :, 23] + s_ne[:, :, 20] + s_tnw[:, :, 17] + s_tne[:, :, 14] +
                                      s_sw[:, :, 23] + s_se[:, :, 20] + s_tsw[:, :, 17] + s_tse[:, :, 14])  # tau_yz
            self.t[i - 1, :, :, 1] = (s_tsw[:, :, 35] + s_tse[:, :, 32] + s_tnw[:, :, 29] + s_tne[:, :, 26] +
                                      s_sw[:, :, 35] + s_se[:, :, 32] + s_nw[:, :, 29] + s_ne[:, :, 26] +
                                      s_se[:, :, 10] + s_ne[:, :, 7] + s_tse[:, :, 4] + s_tne[:, :, 1] +
                                      s_sw[:, :, 10] + s_nw[:, :, 7] + s_tsw[:, :, 4] + s_tnw[:, :, 1])  # tau_xz
            self.t[i - 1, :, :, 2] = (s_nw[:, :, 22] + s_ne[:, :, 19] + s_tnw[:, :, 16] + s_tne[:, :, 13] +
                                      s_sw[:, :, 22] + s_se[:, :, 19] + s_tsw[:, :, 16] + s_tse[:, :, 13] +
                                      s_se[:, :, 11] + s_ne[:, :, 8] + s_tse[:, :, 5] + s_tne[:, :, 2] +
                                      s_sw[:, :, 11] + s_nw[:, :, 8] + s_tsw[:, :, 5] + s_tnw[:, :, 2])  # tau_xy

            # Passing second layer to first
            self.Emat[0] = self.Emat[1]
            self.Cmat[:2] = self.Cmat[1:]
            sys.stdout.write("\rComputing stresses ... {:.1f}% ".format(i / (self.len_x - 2) * 100))
        del self.Emat, self.Cf, self.Cmat, self.mpsa36x36

        self.s /= 8 * self.ws.voxel_length
        self.t /= 16 * self.ws.voxel_length

        if self.print_matrices[4]:
            show_s(self.s, self.t, self.print_matrices[4])

        # Extract only interior displacement, ignoring exterior used as bc
        self.u = self.u[1:-1, 1:-1, 1:-1]
        print("Done")

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
        # ws checks
        if not isinstance(self.ws, Workspace):
            raise Exception("Workspace must be a puma.Workspace.")
        if self.ws.len_x() < 3 or self.ws.len_y() < 3 or self.ws.len_z() < 3:
            raise Exception("Workspace must be at least 3x3x3 for Elasticity MPSA method.")

        # elast_map checks
        ws_tmp_tocheck = self.ws.matrix.copy()
        for i in range(self.elast_map.get_size()):
            low, high, C = self.elast_map.get_material(i)
            self.mat_elast[low] = C
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
        if 0 in unique_matrixvalues and 0 not in self.mat_elast.keys():
            self.elast_map.add_isotropic_material((0, 0), 0., 0.)
            _, _, self.mat_elast[0] = self.elast_map.get_material(self.elast_map.get_size() - 1)
        if (unique_matrixvalues.size != len(self.mat_elast.keys()) or
                np.all(np.sort(list(self.mat_elast.keys())).astype(np.uint16) != unique_matrixvalues)):
            raise Exception("All values in workspace must be included in ElasticityMap.")

        # direction checks
        if self.direction is not None:
            if self.direction.lower() in ['x', 'y', 'z']:
                self.direction = self.direction.lower()
            else:
                raise Exception("Invalid simulation direction, it can only be 'x', 'y', 'z'.")

        # side_bc checks
        if self.side_bc.lower() == "periodic" or self.side_bc == "p":
            self.side_bc = "p"
        elif self.side_bc.lower() == "symmetric" or self.side_bc == "s":
            self.side_bc = "s"
        elif self.side_bc.lower() == "dirichlet" or self.side_bc == "d":
            self.side_bc = "d"
        elif self.side_bc.lower() == "free" or self.side_bc == "f":
            self.side_bc = "f"
        else:
            raise Exception("Invalid side boundary conditions.")

        # print_matrices checks
        if type(self.print_matrices) is not tuple or len(self.print_matrices) != 5:
            raise Exception("Print_matrices must be a tuple with 5 booleans.")

        # prescribed_bc checks
        if self.prescribed_bc is not None:
            if not isinstance(self.prescribed_bc, ElasticityBC):
                raise Exception("prescribed_bc must be a puma.ElasticityBC.")
            if self.prescribed_bc.dirichlet.shape[:3] != self.ws.matrix.shape:
                raise Exception("prescribed_bc must be of the same size as the domain.")
            if self.prescribed_bc.dirichlet.shape[3] != 3:
                raise Exception("prescribed_bc must be of shape (X,Y,Z,3), third dimension not equal to 3.")

            # rotate it
            if self.direction == 'y':
                self.prescribed_bc.dirichlet = self.prescribed_bc.dirichlet.transpose((1, 0, 2, 3))
                self.prescribed_bc.dirichlet = self.prescribed_bc.dirichlet[:, :, :, [1, 0, 2]]
            elif self.direction == 'z':
                self.prescribed_bc.dirichlet = self.prescribed_bc.dirichlet.transpose((2, 1, 0, 3))
                self.prescribed_bc.dirichlet = self.prescribed_bc.dirichlet[:, :, :, [2, 1, 0]]

            if self.direction is not None:
                if np.any((self.prescribed_bc.dirichlet[[0, -1]] == np.Inf)):
                    raise Exception("prescribed_bc must be defined on the direction sides")
        else:
            if self.direction is None:
                raise Exception("prescribed_bc must be defined for compute_stress_analysis")

    # Printing functions of system matrices
    def _print_b(self, dec=1):
        """ Print b vector (first int in print_matrices) """
        print()
        print("b vector:")
        print("  o---> y")
        print("  |")
        print("x v")
        for k in range(self.len_z):
            for i in range(self.len_x):
                for j in range(self.len_y):
                    print('({:.{}f}, {:.{}f}, {:.{}f})'.format(self.bvec[self.len_x * (self.len_y * k + j) + i, 0], dec,
                                                               self.bvec[self.len_xyz + self.len_x * (
                                                                       self.len_y * k + j) + i, 0], dec,
                                                               self.bvec[2 * self.len_xyz + self.len_x * (
                                                                       self.len_y * k + j) + i, 0], dec), end=' ')
                print()
            print()

    def _print_E(self, i, i_cv, dec=4):
        """ Print E transmissibility matrix (second int in print_matrices) """
        np.set_printoptions(precision=dec)
        np.set_printoptions(linewidth=10000)
        for j in range(self.len_y - 1):
            for k in range(self.len_z - 1):
                print()
                print("index {},{},{}".format(i_cv, j, k))
                print(self.Emat[i, j, k])

    def _print_A(self, dec=4):
        """ Print A matrix (third int in print_matrices) """
        np.set_printoptions(linewidth=10000)
        np.set_printoptions(threshold=sys.maxsize)
        np.set_printoptions(formatter={'float': lambda x: "{:.{}f}".format(x, dec).rstrip('0').rstrip('.')})
        print()
        print("A matrix:")
        print(self.Amat.toarray())


def show_u(u, dec=4):
    """ Print u displacement (fourth int in print_matrices) """
    print()
    print("3D Displacement:")
    print("  o---> y")
    print("  |")
    print("x v")
    for k in range(u.shape[2]):
        for i in range(u.shape[0]):
            for j in range(u.shape[1]):
                print('({:.{}f}, {:.{}f}, {:.{}f})'.format(u[i, j, k, 0], dec,
                                                           u[i, j, k, 1], dec,
                                                           u[i, j, k, 2], dec), end=' ')
            print()
        print()


def show_s(s, t, dec=4):
    """ Print s stress (fifth int in print_matrices) """
    print()
    print("3D Stress:")
    print("  o---> y")
    print("  |")
    print("x v")
    for k in range(s.shape[2]):
        for i in range(s.shape[0]):
            for j in range(s.shape[1]):
                print('({:.{}f}, {:.{}f}, {:.{}f}, {:.{}f}, {:.{}f}, {:.{}f})'.format(s[i, j, k, 0], dec,
                                                                                      s[i, j, k, 1], dec,
                                                                                      s[i, j, k, 2], dec,
                                                                                      t[i, j, k, 0], dec,
                                                                                      t[i, j, k, 1], dec,
                                                                                      t[i, j, k, 2], dec), end=' ')
            print()
        print()
