from pumapy.utilities.timer import Timer
from pumapy.utilities.logger import print_warning
from pumapy.utilities.workspace import Workspace
from pumapy.physicsmodels.linear_solvers import PropertySolver
from pumapy.materialproperties.volumefraction import compute_volume_fraction
from pumapy.utilities.generic_checks import estimate_max_memory
from scipy.sparse import coo_matrix, diags
from scipy.sparse.linalg import LinearOperator
import numpy as np


class ElasticityFE(PropertySolver):

    def __init__(self, workspace, elast_map, direction, tolerance, maxiter, solver_type, display_iter, matrix_free):
        #preconditioner, output_fields):

        allowed_solvers = ['minres', 'direct', 'cg', 'bicgstab', 'gmres']
        super().__init__(workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter)

        self.elast_map = elast_map
        self.ws = workspace.copy()
        self.matrix_free = matrix_free
        self.direction = direction
        self.solver_type = solver_type
        self.voxlength = self.ws.voxel_length
        self.len_x, self.len_y, self.len_z = self.ws.matrix.shape
        self.mat_elast = dict()

        if self.direction == 'x':
            self.axis = 0
        elif self.direction == 'y':
            self.axis = 1
        elif self.direction == 'z':
            self.axis = 2
        elif self.direction == 'xy':
            self.axis = 3
        elif self.direction == 'xz':
            self.axis = 4
        else:
            self.axis = 5

        self.Ceff = None
        self.u = None
        self.s = None
        self.t = None

    def compute(self):
        self.initialize()
        self.element_stiffness_matrices()
        self.compute_rhs()
        self.assemble_Amatrix()
        super().solve()
        self.compute_effective_coefficient()

    def initialize(self):
        print("Initializing index matrices")
        self.nElems = self.len_x * self.len_y * self.len_z
        self.nDOFs = self.nElems * 3
        self.elemMatMap = np.zeros(self.len_x * self.len_y * self.len_z, dtype=int)
        self.pElemDOFNum = np.zeros((24, self.nElems), dtype=int)
        nElemS = self.len_x * self.len_y
        nNodeS = (self.len_x + 1) * (self.len_y + 1)
        DOFMap = np.zeros((self.len_x + 1) * (self.len_y + 1) * (self.len_z + 1), dtype=int)

        rows = self.len_y
        cols = self.len_x
        slices = self.len_z
        for k in range(1, slices + 1):
            for i in range(1, rows + 1):
                for j in range(1, cols + 1):
                    self.elemMatMap[i + (j - 1) * self.len_y + (k - 1) * self.len_x * self.len_y -1] = self.ws[j -1, i -1, k -1]

        for n in range(nNodeS * (self.len_z + 1)):
            i = n % nNodeS
            DOFMap[n] = (i - (i // (self.len_y + 1)) - self.len_y * ((i % (self.len_y + 1)) // self.len_y)) \
                            % nElemS + ((n // nNodeS) % self.len_z) * nElemS + 1

        for e in range(self.nElems):
            N1 = 2 + e % nElemS + (e % nElemS) // self.len_y + e // nElemS * nNodeS - 1
            N3 = N1 + self.len_y
            N2 = N3 + 1
            N4 = N1 - 1
            N5 = N1 + nNodeS
            N6 = N2 + nNodeS
            N7 = N3 + nNodeS
            N8 = N4 + nNodeS
            self.pElemDOFNum[0, e] = DOFMap[N1] * 3 - 3
            self.pElemDOFNum[1, e] = DOFMap[N1] * 3 - 2
            self.pElemDOFNum[2, e] = DOFMap[N1] * 3 - 1
            self.pElemDOFNum[3, e] = DOFMap[N2] * 3 - 3
            self.pElemDOFNum[4, e] = DOFMap[N2] * 3 - 2
            self.pElemDOFNum[5, e] = DOFMap[N2] * 3 - 1
            self.pElemDOFNum[6, e] = DOFMap[N3] * 3 - 3
            self.pElemDOFNum[7, e] = DOFMap[N3] * 3 - 2
            self.pElemDOFNum[8, e] = DOFMap[N3] * 3 - 1
            self.pElemDOFNum[9, e] = DOFMap[N4] * 3 - 3
            self.pElemDOFNum[10, e] = DOFMap[N4] * 3 - 2
            self.pElemDOFNum[11, e] = DOFMap[N4] * 3 - 1
            self.pElemDOFNum[12, e] = DOFMap[N5] * 3 - 3
            self.pElemDOFNum[13, e] = DOFMap[N5] * 3 - 2
            self.pElemDOFNum[14, e] = DOFMap[N5] * 3 - 1
            self.pElemDOFNum[15, e] = DOFMap[N6] * 3 - 3
            self.pElemDOFNum[16, e] = DOFMap[N6] * 3 - 2
            self.pElemDOFNum[17, e] = DOFMap[N6] * 3 - 1
            self.pElemDOFNum[18, e] = DOFMap[N7] * 3 - 3
            self.pElemDOFNum[19, e] = DOFMap[N7] * 3 - 2
            self.pElemDOFNum[20, e] = DOFMap[N7] * 3 - 1
            self.pElemDOFNum[21, e] = DOFMap[N8] * 3 - 3
            self.pElemDOFNum[22, e] = DOFMap[N8] * 3 - 2
            self.pElemDOFNum[23, e] = DOFMap[N8] * 3 - 1

    def compute_rhs(self):
        print("Computing RHS")
        self.bvec = np.zeros(self.nDOFs, dtype=float)

        for e in range(self.nElems):
            np.add.at(self.bvec, self.pElemDOFNum[:, e], self.m_B[self.axis, :, self.elemMatMap[e]])

    def assemble_Amatrix(self):
        print("Assembling system")
        y = np.zeros(self.nDOFs, dtype=float)

        if self.matrix_free and self.solver_type != 'direct':
            def matvec(x):
                y.fill(0)
                np.add.at(y, self.pElemDOFNum, np.einsum("ijk, jk -> ik", self.m_K[:, :, self.elemMatMap], x[self.pElemDOFNum]))
                return y
            self.Amat = LinearOperator(shape=(self.nDOFs, self.nDOFs), matvec=matvec)

        else:
            I, J, V = [], [], []
            for e in range(self.nElems):
                for i in range(24):
                    for j in range(24):
                        I.append(self.pElemDOFNum[i, e])
                        J.append(self.pElemDOFNum[j, e])
                        V.append(self.m_K[i, j, self.elemMatMap[e]])
            self.Amat = coo_matrix((V, (I, J))).tocsc()

    def compute_effective_coefficient(self):
        print("Computing effective elasticity")
        self.Ceff = np.zeros((6, 6), dtype=float)

        t = np.zeros(24, dtype=float)
        if self.axis == 0:
            t[[3, 6, 15, 18]] = 1
        elif self.axis == 1:
            t[[7, 10, 19, 22]] = 1
        elif self.axis == 2:
            t[[14, 17, 20, 23]] = 1
        elif self.axis == 3:
            t[[6, 9, 18, 21]] = 1
        elif self.axis == 4:
            t[[12, 15, 18, 21]] = 1
        elif self.axis == 5:
            t[[8, 11, 20, 23]] = 1

        SX = 0; SY = 0; SZ = 0; SXY = 0; SXZ = 0; SYZ = 0
        for e in range(self.nElems):
            SX  += (self.m_B[0, :, self.elemMatMap[e]] * (t - self.x[self.pElemDOFNum[:, e]])).sum()
            SY  += (self.m_B[1, :, self.elemMatMap[e]] * (t - self.x[self.pElemDOFNum[:, e]])).sum()
            SZ  += (self.m_B[2, :, self.elemMatMap[e]] * (t - self.x[self.pElemDOFNum[:, e]])).sum()
            SXY += (self.m_B[3, :, self.elemMatMap[e]] * (t - self.x[self.pElemDOFNum[:, e]])).sum()
            SXZ += (self.m_B[4, :, self.elemMatMap[e]] * (t - self.x[self.pElemDOFNum[:, e]])).sum()
            SYZ += (self.m_B[5, :, self.elemMatMap[e]] * (t - self.x[self.pElemDOFNum[:, e]])).sum()

        self.Ceff[self.axis, 0] = SX / self.nElems
        self.Ceff[self.axis, 1] = SY / self.nElems
        self.Ceff[self.axis, 2] = SZ / self.nElems
        self.Ceff[self.axis, 3] = SXY / self.nElems
        self.Ceff[self.axis, 4] = SXZ / self.nElems
        self.Ceff[self.axis, 5] = SYZ / self.nElems

    def element_stiffness_matrices(self):
        k = np.zeros((24, 24), dtype=float)
        BC = np.zeros((6, 24), dtype=float)
        B = np.zeros((6, 24), dtype=float)
        C = np.zeros((6, 6), dtype=float)

        nMat = len(self.mat_elast)
        self.m_K = np.zeros((24, 24, nMat), dtype=float)
        self.m_B = np.zeros((6, 24, nMat), dtype=float)

        i_mat = 0
        for j_mat in range(0, 256):

            if j_mat not in self.mat_elast.keys():
                continue

            # Element coords
            x = np.array([0., 1., 1., 0., 0., 1., 1., 0.])
            y = np.array([0., 0., 1., 1., 0., 0., 1., 1.])
            z = np.array([0., 0., 0., 0., 1., 1., 1., 1.])

            # Constitutive matrix
            C.fill(0)
            cs = self.mat_elast[j_mat]
            C[:, :] = np.array([[cs[0], cs[1], cs[2], cs[3], cs[4], cs[5]],
                                [cs[1], cs[6], cs[7], cs[8], cs[9], cs[10]],
                                [cs[2], cs[7], cs[11], cs[12], cs[13], cs[14]],
                                [cs[3], cs[8], cs[12], cs[15]/2., cs[16], cs[17]],
                                [cs[4], cs[9], cs[13], cs[16], cs[18]/2., cs[19]],
                                [cs[5], cs[10], cs[14], cs[17], cs[19], cs[20]/2.]])

            # Gauss Points and Weights
            gp = [-1. / np.sqrt(3), 1. / np.sqrt(3)]
            k.fill(0)
            BC.fill(0)
            B.fill(0)
            for i in range(2):
                r = gp[i]
                for j in range(2):
                    s = gp[j]
                    for l in range(2):
                        t = gp[l]

                        X = np.array([x.T, y.T, z.T])

                        # Compute B matrix and Jacobian
                        dN1dr = -(1 - s) * (1 - t) * .125
                        dN2dr =  (1 - s) * (1 - t) * .125
                        dN3dr =  (1 + s) * (1 - t) * .125
                        dN4dr = -(1 + s) * (1 - t) * .125
                        dN5dr = -(1 - s) * (1 + t) * .125
                        dN6dr =  (1 - s) * (1 + t) * .125
                        dN7dr =  (1 + s) * (1 + t) * .125
                        dN8dr = -(1 + s) * (1 + t) * .125
                        dN1ds = -(1 - r) * (1 - t) * .125
                        dN2ds = -(1 + r) * (1 - t) * .125
                        dN3ds =  (1 + r) * (1 - t) * .125
                        dN4ds =  (1 - r) * (1 - t) * .125
                        dN5ds = -(1 - r) * (1 + t) * .125
                        dN6ds = -(1 + r) * (1 + t) * .125
                        dN7ds =  (1 + r) * (1 + t) * .125
                        dN8ds =  (1 - r) * (1 + t) * .125
                        dN1dt = -(1 - r) * (1 - s) * .125
                        dN2dt = -(1 + r) * (1 - s) * .125
                        dN3dt = -(1 + r) * (1 + s) * .125
                        dN4dt = -(1 - r) * (1 + s) * .125
                        dN5dt =  (1 - r) * (1 - s) * .125
                        dN6dt =  (1 + r) * (1 - s) * .125
                        dN7dt =  (1 + r) * (1 + s) * .125
                        dN8dt =  (1 - r) * (1 + s) * .125
                        dN = np.array([[dN1dr, dN2dr, dN3dr, dN4dr, dN5dr, dN6dr, dN7dr, dN8dr],
                                       [dN1ds, dN2ds, dN3ds, dN4ds, dN5ds, dN6ds, dN7ds, dN8ds],
                                       [dN1dt, dN2dt, dN3dt, dN4dt, dN5dt, dN6dt, dN7dt, dN8dt]])
                        J = dN @ X.T
                        dNdx = np.linalg.inv(J) @ dN
                        B[0, 0]  = dNdx[0, 0]
                        B[1, 1]  = dNdx[1, 0]
                        B[2, 2]  = dNdx[2, 0]
                        B[0, 3]  = dNdx[0, 1]
                        B[1, 4]  = dNdx[1, 1]
                        B[2, 5]  = dNdx[2, 1]
                        B[0, 6]  = dNdx[0, 2]
                        B[1, 7]  = dNdx[1, 2]
                        B[2, 8]  = dNdx[2, 2]
                        B[0, 9] = dNdx[0, 3]
                        B[1, 10] = dNdx[1, 3]
                        B[2, 11] = dNdx[2, 3]
                        B[0, 12] = dNdx[0, 4]
                        B[1, 13] = dNdx[1, 4]
                        B[2, 14] = dNdx[2, 4]
                        B[0, 15] = dNdx[0, 5]
                        B[1, 16] = dNdx[1, 5]
                        B[2, 17] = dNdx[2, 5]
                        B[0, 18] = dNdx[0, 6]
                        B[1, 19] = dNdx[1, 6]
                        B[2, 20] = dNdx[2, 6]
                        B[0, 21] = dNdx[0, 7]
                        B[1, 22] = dNdx[1, 7]
                        B[2, 23] = dNdx[2, 7]
                        B[3, 0]  = dNdx[1, 0]
                        B[4, 0]  = dNdx[2, 0]
                        B[5, 1]  = dNdx[2, 0]
                        B[3, 1]  = dNdx[0, 0]
                        B[4, 2]  = dNdx[0, 0]
                        B[5, 2]  = dNdx[1, 0]
                        B[3, 3]  = dNdx[1, 1]
                        B[4, 3]  = dNdx[2, 1]
                        B[5, 4]  = dNdx[2, 1]
                        B[3, 4]  = dNdx[0, 1]
                        B[4, 5]  = dNdx[0, 1]
                        B[5, 5]  = dNdx[1, 1]
                        B[3, 6]  = dNdx[1, 2]
                        B[4, 6]  = dNdx[2, 2]
                        B[5, 7]  = dNdx[2, 2]
                        B[3, 7]  = dNdx[0, 2]
                        B[4, 8]  = dNdx[0, 2]
                        B[5, 8]  = dNdx[1, 2]
                        B[3, 9] = dNdx[1, 3]
                        B[4, 9] = dNdx[2, 3]
                        B[5, 10] = dNdx[2, 3]
                        B[3, 10] = dNdx[0, 3]
                        B[4, 11] = dNdx[0, 3]
                        B[5, 11] = dNdx[1, 3]
                        B[3, 12] = dNdx[1, 4]
                        B[4, 12] = dNdx[2, 4]
                        B[5, 13] = dNdx[2, 4]
                        B[3, 13] = dNdx[0, 4]
                        B[4, 14] = dNdx[0, 4]
                        B[5, 14] = dNdx[1, 4]
                        B[3, 15] = dNdx[1, 5]
                        B[4, 15] = dNdx[2, 5]
                        B[5, 16] = dNdx[2, 5]
                        B[3, 16] = dNdx[0, 5]
                        B[4, 17] = dNdx[0, 5]
                        B[5, 17] = dNdx[1, 5]
                        B[3, 18] = dNdx[1, 6]
                        B[4, 18] = dNdx[2, 6]
                        B[5, 19] = dNdx[2, 6]
                        B[3, 19] = dNdx[0, 6]
                        B[4, 20] = dNdx[0, 6]
                        B[5, 20] = dNdx[1, 6]
                        B[3, 21] = dNdx[1, 7]
                        B[4, 21] = dNdx[2, 7]
                        B[5, 22] = dNdx[2, 7]
                        B[3, 22] = dNdx[0, 7]
                        B[4, 23] = dNdx[0, 7]
                        B[5, 23] = dNdx[1, 7]

                        detJ = np.linalg.det(J)
                        k  += (B.T @ C @ B) * detJ
                        BC += (C @ B) * detJ

            self.m_K[:, :, i_mat], self.m_B[:, :, i_mat] = k, BC
            i_mat += 1

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

        # direction checks
        if self.direction.lower() in ['x', 'y', 'z', 'yz', 'xz', 'xy']:
            self.direction = self.direction.lower()
        else:
            raise Exception("Invalid simulation direction, it can only be 'x', 'y', 'z', 'yz', 'xz', 'xy'.")

    def log_input(self):
        pass

    def log_output(self):
        pass
