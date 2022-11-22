"""
The following FE numerical method and implementation are based on the following research paper:

Pedro C. F. Lopes, Rafael S. Vianna, Victor W. Sapucaia, Federico Semeraro, Ricardo Leiderman, Andre M. B. Pereira, 2022.
Simulation Toolkit for Digital Material Characterization of Large Image-based Microstructures.
"""
from pumapy.utilities.timer import Timer
from pumapy.physics_models.utils.linear_solvers import PropertySolver
from pumapy.utilities.generic_checks import estimate_max_memory
from pumapy.physics_models.utils.property_maps import AnisotropicConductivityMap
from scipy.sparse import coo_matrix
from scipy.sparse.linalg import LinearOperator
import numpy as np


class ConductivityFE(PropertySolver):

    def __init__(self, workspace, cond_map, direction, tolerance, maxiter, solver_type, display_iter, matrix_free):

        allowed_solvers = ['minres', 'direct', 'bicgstab', 'gmres']
        super().__init__(workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter)

        self.cond_map = cond_map
        self.matrix_free = matrix_free
        self.direction = direction
        self.voxlength = self.ws.voxel_length
        self.len_x, self.len_y, self.len_z = self.ws.matrix.shape
        self.mat_cond = dict()
        self.need_to_orient = False  # changes if (E_axial, E_radial, nu_poissrat_12, nu_poissrat_23, G12) detected

        if self.direction == 'x':
            self.axis = 0
        elif self.direction == 'y':
            self.axis = 1
        elif self.direction == 'z':
            self.axis = 2

        self.keff = None
        self.T = None
        self.q = None

    def compute(self):
        t = Timer()
        estimate_max_memory("elasticity_fe", self.ws.matrix.shape, self.solver_type, self.need_to_orient, mf=self.matrix_free)
        self.initialize()
        self.compute_rhs()
        self.assemble_Amatrix()
        print("Time to assemble matrices: ", t.elapsed()); t.reset()
        super().solve()
        print("Time to solve: ", t.elapsed())
        self.compute_effective_coefficient()
        self.solve_time = t.elapsed()

    def initialize(self):
        print("Initializing indexing matrices ... ", flush=True, end='')
        self.nElems = self.len_x * self.len_y * self.len_z
        self.pElemDOFNum = np.zeros((8, self.nElems), dtype=np.uint32)
        self.nElemS = self.len_x * self.len_y
        nNodeS = (self.len_x + 1) * (self.len_y + 1)
        DOFMap = np.zeros((self.len_x + 1) * (self.len_y + 1) * (self.len_z + 1), dtype=int)

        # Segmenting domain according to cond_map
        for i in range(self.cond_map.get_size()):
            low, high, _ = self.cond_map.get_material(i)
            self.ws[np.logical_and(self.ws.matrix >= low, self.ws.matrix <= high)] = low

        # remap segmented domain and property map to start from 0
        if not self.need_to_orient:
            keys, props = self.mat_cond.keys(), self.mat_cond.values()
            del self.mat_cond; self.mat_cond = dict()
            counter = 0
            for key, prop in sorted(zip(keys, props)):
                self.mat_cond[counter] = prop
                self.ws.matrix[self.ws.matrix == key] = counter
                counter += 1

        self.elemMatMap = np.zeros(self.nElems, dtype=int)
        for k in range(self.len_z):
            for i in range(self.len_y):
                for j in range(self.len_x):
                    self.elemMatMap[i + j * self.len_y + k * self.len_x * self.len_y] = self.ws[j, i, k]

        if self.need_to_orient:
            self.elemMatMap_orient = np.zeros((self.nElems, 3), dtype=int)
            for k in range(self.len_z):
                for i in range(self.len_y):
                    for j in range(self.len_x):
                        self.elemMatMap_orient[i + j * self.len_y + k * self.len_x * self.len_y] = self.ws.orientation[j, i, k]

        # compute self.m_K and self.m_B
        self.create_element_matrices(onlyB=False)

        for n in range(nNodeS * (self.len_z + 1)):
            i = n % nNodeS
            DOFMap[n] = (i - (i // (self.len_y + 1)) - self.len_y * ((i % (self.len_y + 1)) // self.len_y)) \
                        % self.nElemS + ((n // nNodeS) % self.len_z) * self.nElemS

        for e in range(self.nElems):
            N1 = 2 + e % self.nElemS + (e % self.nElemS) // self.len_y + e // self.nElemS * nNodeS - 1
            N3 = N1 + self.len_y
            N2 = N3 + 1
            N4 = N1 - 1
            N5 = N1 + nNodeS
            N6 = N2 + nNodeS
            N7 = N3 + nNodeS
            N8 = N4 + nNodeS
            self.pElemDOFNum[0, e] = DOFMap[N1]
            self.pElemDOFNum[1, e] = DOFMap[N2]
            self.pElemDOFNum[2, e] = DOFMap[N3]
            self.pElemDOFNum[3, e] = DOFMap[N4]
            self.pElemDOFNum[4, e] = DOFMap[N5]
            self.pElemDOFNum[5, e] = DOFMap[N6]
            self.pElemDOFNum[6, e] = DOFMap[N7]
            self.pElemDOFNum[7, e] = DOFMap[N8]
        print("Done")

    def compute_rhs(self):
        print("Creating b vector ... ", flush=True, end='')
        self.bvec = np.zeros(self.nElems, dtype=float)

        if not self.need_to_orient:
            for e in range(self.nElems):
                np.add.at(self.bvec, self.pElemDOFNum[:, e], self.m_B[self.axis, :, self.elemMatMap[e]])
        else:
            for e in range(self.nElems):
                np.add.at(self.bvec, self.pElemDOFNum[:, e], self.m_B[self.axis, :, e])
        del self.m_B
        print("Done")

    def assemble_Amatrix(self):
        print("Creating A matrix ... ", flush=True, end='')
        y = np.zeros(self.nElems, dtype=float)

        if self.matrix_free and self.solver_type != 'direct' and not self.need_to_orient:
            def matvec(x):  # overload matvec for Amat=LinearOperator - only for isotropic phases
                y.fill(0)
                np.add.at(y, self.pElemDOFNum, np.einsum("ijk, jk -> ik", self.m_K[:, :, self.elemMatMap], x[self.pElemDOFNum]))
                return y
            self.Amat = LinearOperator(shape=(self.nElems, self.nElems), matvec=matvec)

        else:  # actually assemble sparse matrix
            range8 = np.arange(8)
            m_K_inds = (np.tile(range8, 8), np.repeat(range8, 8))
            I, J = np.zeros((2, self.nElems * 64), dtype=np.uint32)
            V = np.zeros(self.nElems * 64, dtype=float)
            counter = 0
            if self.need_to_orient:
                for e in range(self.nElems):
                    I[counter:counter + 64] = np.repeat(self.pElemDOFNum[range8, e], 8)
                    J[counter:counter + 64] = np.tile(self.pElemDOFNum[range8, e], 8)
                    V[counter:counter + 64] = self.m_K[m_K_inds[0], m_K_inds[1], e]
                    counter += 64
            else:
                for e in range(self.nElems):
                    I[counter:counter + 64] = np.repeat(self.pElemDOFNum[range8, e], 8)
                    J[counter:counter + 64] = np.tile(self.pElemDOFNum[range8, e], 8)
                    V[counter:counter + 64] = self.m_K[m_K_inds[0], m_K_inds[1], self.elemMatMap[e]]
                    counter += 64
            del self.m_K
            self.Amat = coo_matrix((V, (I, J))).tocsc()

        print("Done")

    def compute_effective_coefficient(self):
        print("Computing stresses ... ", flush=True, end='')

        if self.matrix_free and self.solver_type != 'direct' and not self.need_to_orient:
            del self.m_K

        # compute self.m_B
        self.create_element_matrices(onlyB=True)

        self.T = np.zeros((self.len_x, self.len_y, self.len_z), dtype=float)
        for k in range(self.len_z):
            for i in range(self.len_y):
                for j in range(self.len_x):
                    self.T[j, i, k] = self.x[i + j * self.len_y + k * self.len_x * self.len_y]

        t = np.zeros(8, dtype=float)
        if self.axis == 0:
            t[[1, 2, 5, 6]] = 1
        elif self.axis == 1:
            t[[2, 3, 6, 7]] = 1
        elif self.axis == 2:
            t[[4, 5, 6, 7]] = 1

        q_f = np.zeros((self.nElems, 3), dtype=float)
        if not self.need_to_orient:
            for e in range(self.nElems):
                q_f[e, 0] += (self.m_B[0, :, self.elemMatMap[e]] * (t - self.x[self.pElemDOFNum[:, e]])).sum()
                q_f[e, 1] += (self.m_B[1, :, self.elemMatMap[e]] * (t - self.x[self.pElemDOFNum[:, e]])).sum()
                q_f[e, 2] += (self.m_B[2, :, self.elemMatMap[e]] * (t - self.x[self.pElemDOFNum[:, e]])).sum()
        else:
            for e in range(self.nElems):
                q_f[e, 0] += (self.m_B[0, :, e] * (t - self.x[self.pElemDOFNum[:, e]])).sum()
                q_f[e, 1] += (self.m_B[1, :, e] * (t - self.x[self.pElemDOFNum[:, e]])).sum()
                q_f[e, 2] += (self.m_B[2, :, e] * (t - self.x[self.pElemDOFNum[:, e]])).sum()

        self.q = np.zeros((self.len_x, self.len_y, self.len_z, 3), dtype=float)
        for k in range(self.len_z):
            for i in range(self.len_y):
                for j in range(self.len_x):
                    self.q[j, i, k, 0] = q_f[i + j * self.len_y + k * self.len_x * self.len_y, 0]
                    self.q[j, i, k, 1] = q_f[i + j * self.len_y + k * self.len_x * self.len_y, 1]
                    self.q[j, i, k, 2] = q_f[i + j * self.len_y + k * self.len_x * self.len_y, 2]

        self.keff = [self.q[:, :, :, 0].sum() / self.nElems, self.q[:, :, :, 1].sum() / self.nElems, self.q[:, :, :, 2].sum() / self.nElems]
        print("Done")

    def create_element_matrices(self, onlyB):

        if self.need_to_orient:
            if not onlyB:
                self.m_K = np.zeros((8, 8, self.nElems), dtype=float)
            self.m_B = np.zeros((3, 8, self.nElems), dtype=float)
        else:
            nMat = len(self.mat_cond)
            if not onlyB:
                self.m_K = np.zeros((8, 8, nMat), dtype=float)
            self.m_B = np.zeros((3, 8, nMat), dtype=float)

        k = np.zeros((8, 8), dtype=float)
        BC = np.zeros((3, 8), dtype=float)
        B = np.zeros((3, 8), dtype=float)
        cond = np.zeros((3, 3), dtype=float)

        if self.need_to_orient:
            KBiso_prev = dict()
            for e in range(self.nElems):
                ks = self.mat_cond[self.elemMatMap[e]]

                if len(ks) == 6:
                    if self.elemMatMap[e] in KBiso_prev.keys():
                        if onlyB:
                            self.m_B[:, :, e] = KBiso_prev[self.elemMatMap[e]]
                        else:
                            self.m_K[:, :, e], self.m_B[:, :, e] = KBiso_prev[self.elemMatMap[e]]
                        continue
                    cond[:] = self.create_k(ks)
                else:
                    cond[:] = self.orient_k(ks, e)

                self.compute_element_conductivity(cond, k, BC, B, e, onlyB)

                if len(ks) == 6:  # store previously computed k,BC for isotropic phases
                    if onlyB:
                        KBiso_prev[self.elemMatMap[e]] = self.m_B[:, :, e]
                    else:
                        KBiso_prev[self.elemMatMap[e]] = self.m_K[:, :, e], self.m_B[:, :, e]
        else:
            for i_mat, mat_props in enumerate(self.mat_cond.values()):
                cond[:] = self.create_k(mat_props)
                self.compute_element_conductivity(cond, k, BC, B, i_mat, onlyB)

    def create_k(self, ks):
        return np.array([[ks[0], ks[3], ks[4]],
                         [ks[3], ks[1], ks[5]],
                         [ks[4], ks[5], ks[2]]])

    def orient_k(self, ks, e):
        phi = np.arctan2(self.elemMatMap_orient[e, 1], self.elemMatMap_orient[e, 0])
        theta = np.arcsin(self.elemMatMap_orient[e, 2])

        Rz = np.zeros((3, 3), dtype=float)
        Ry = np.zeros((3, 3), dtype=float)
        Rz[0, 0] = np.cos(phi)
        Rz[1, 1] = Rz[0, 0]
        Rz[1, 0] = np.sin(phi)
        Rz[0, 1] = -Rz[1, 0]
        Rz[2, 2] = 1
        Ry[1, 1] = 1
        Ry[0, 0] = np.cos(theta)
        Ry[2, 2] = Ry[0, 0]
        Ry[0, 2] = np.sin(theta)
        Ry[2, 0] = -Ry[0, 2]
        R = Rz @ Ry

        Kinit = np.zeros((3, 3), dtype=float)
        Kinit[[0, 1, 2], [0, 1, 2]] = [ks[0], ks[1], ks[1]]
        return R @ Kinit @ R.T

    def compute_element_conductivity(self, cond, k, BC, B, ind, onlyB):
        if not onlyB:
            k.fill(0)
        BC.fill(0)

        gauss_points = [0.21132486540518708, 0.78867513459481290]
        for zp in gauss_points:
            for yp in gauss_points:
                for xp in gauss_points:
                    B[:] = np.array([[(-1+yp)*(1-zp), (1-yp)*(1-zp), yp*(1-zp), -yp*(1-zp), (-1+yp)*zp, (1-yp)*zp, yp*zp, -yp*zp],
                                     [(-1+xp)*(1-zp), -xp*(1-zp), xp*(1-zp), (1-xp)*(1-zp), (-1+xp)*zp, -xp*zp, xp*zp, (1-xp)*zp],
                                     [(-1+xp)*(1-yp), -xp*(1-yp), -xp*yp, (-1+xp)*yp, (1-xp)*(1-yp), xp*(1-yp), xp*yp, (1-xp)*yp]])
                    if not onlyB:
                        k += B.T @ cond @ B
                    BC += cond @ B

        if not onlyB:
            self.m_K[:, :, ind] = k * 0.125
        self.m_B[:, :, ind] = BC * 0.125

    def log_input(self):
        self.ws.log.log_section("Computing Conductivity")
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
        self.ws.log.log_section("Finished Conductivity Calculation")
        self.ws.log.log_line("Conductivity: " + "[" + str(self.keff) + "]")
        self.ws.log.log_line("Solver Time: " + str(self.solve_time))
        self.ws.log.write_log()

    def error_check(self):
        # cond_map checks
        if not isinstance(self.cond_map, AnisotropicConductivityMap):
            raise Exception("cond_map has to be an AnisotropicConductivityMap object when method='fe'.")
        ws_tmp_tocheck = self.ws.matrix.copy()
        for i in range(self.cond_map.get_size()):
            low, high, k = self.cond_map.get_material(i)
            self.mat_cond[low] = k

            if len(k) == 2:
                self.need_to_orient = True
                if self.ws.orientation.shape[:3] != self.ws.matrix.shape or \
                        not 0.9 < np.min(np.linalg.norm(self.ws.orientation[(self.ws.matrix >= low) &
                                                                            (self.ws.matrix <= high)], axis=1)) < 1.1:
                    raise Exception("The Workspace needs an orientation in order to align the conductivities.")

            # segmenting tmp domain to check if all values covered by mat_cond
            ws_tmp_tocheck[(self.ws.matrix >= low) & (self.ws.matrix <= high)] = low

        unique_matrixvalues = np.unique(ws_tmp_tocheck)
        if (unique_matrixvalues.size != self.cond_map.get_size() or
                np.all(np.array(list(self.mat_cond.keys()), dtype=np.uint16) != unique_matrixvalues)):
            raise Exception("All values in workspace must be included in ConductivityMap.")

        # direction checks
        if self.direction.lower() in ['x', 'y', 'z']:
            self.direction = self.direction.lower()
        else:
            raise Exception("Invalid simulation direction, it can only be 'x', 'y', 'z'.")
