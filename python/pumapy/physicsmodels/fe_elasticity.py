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

    def __init__(self, workspace, elast_map, direction, tolerance, maxiter, solver_type, display_iter):
        #matrix_free, preconditioner, output_fields):

        allowed_solvers = ['minres', 'direct', 'cg', 'bicgstab', 'gmres']
        super().__init__(workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter)

        self.elast_map = elast_map
        self.ws = workspace.copy()

        self.direction = direction
        self.solver_type = solver_type
        self.voxlength = self.ws.voxel_length
        self.len_x, self.len_y, self.len_z = self.ws.matrix.shape
        self.mat_elast = dict()

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
        print("Initializing")
        self.elemMatMap = np.zeros(self.len_x * self.len_y * self.len_z, dtype=int)

        rows = self.len_y
        cols = self.len_x
        slices = self.len_z
        for k in range(1, slices + 1):
            for i in range(1, rows + 1):
                for j in range(1, cols + 1):
                    self.elemMatMap[i + (j - 1) * self.len_y + (k - 1) * self.len_x * self.len_y -1] = self.ws[j -1, i -1, k -1]

        self.nElems = self.len_x * self.len_y * self.len_z
        self.nDOFs = self.nElems * 3
        self.nElemS = self.len_x * self.len_y
        self.nNodeS = (self.len_x + 1) * (self.len_y + 1)
        self.DOFMap = np.zeros((self.len_x + 1) * (self.len_y + 1) * (self.len_z + 1), dtype=int)

        for n in range(1, (self.nNodeS * (self.len_z + 1)) + 1):
            i = (n - 1) % self.nNodeS
            self.DOFMap[n - 1] = (i - (i // (self.len_y + 1)) - self.len_y * ((i % (self.len_y + 1)) // self.len_y)) \
                                 % self.nElemS + (((n - 1) // self.nNodeS) % self.len_z) * self.nElemS + 1

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

    def compute_rhs(self):
        print("Computing RHS")
        self.bvec = np.zeros(self.nDOFs, dtype=float)
        pElemDOFNum = np.zeros(24, dtype=int)

        for e in range(1, self.nElems + 1):
            N1 = 2 + (e - 1) % self.nElemS + ((e - 1) % self.nElemS) // self.len_y + (e - 1) // self.nElemS * self.nNodeS
            N3 = N1 + self.len_y; N2 = N3 + 1; N4 = N1 - 1
            N5 = N1 + self.nNodeS; N6 = N2 + self.nNodeS; N7 = N3 + self.nNodeS; N8 = N4 + self.nNodeS
            pElemDOFNum[1 -1]  = self.DOFMap[N1 -1] * 3 - 2; pElemDOFNum[2 -1]  = self.DOFMap[N1 -1] * 3 - 1; pElemDOFNum[3 -1]  = self.DOFMap[N1 -1] * 3
            pElemDOFNum[4 -1]  = self.DOFMap[N2 -1] * 3 - 2; pElemDOFNum[5 -1]  = self.DOFMap[N2 -1] * 3 - 1; pElemDOFNum[6 -1]  = self.DOFMap[N2 -1] * 3
            pElemDOFNum[7 -1]  = self.DOFMap[N3 -1] * 3 - 2; pElemDOFNum[8 -1]  = self.DOFMap[N3 -1] * 3 - 1; pElemDOFNum[9 -1]  = self.DOFMap[N3 -1] * 3
            pElemDOFNum[10 -1] = self.DOFMap[N4 -1] * 3 - 2; pElemDOFNum[11 -1] = self.DOFMap[N4 -1] * 3 - 1; pElemDOFNum[12 -1] = self.DOFMap[N4 -1] * 3
            pElemDOFNum[13 -1] = self.DOFMap[N5 -1] * 3 - 2; pElemDOFNum[14 -1] = self.DOFMap[N5 -1] * 3 - 1; pElemDOFNum[15 -1] = self.DOFMap[N5 -1] * 3
            pElemDOFNum[16 -1] = self.DOFMap[N6 -1] * 3 - 2; pElemDOFNum[17 -1] = self.DOFMap[N6 -1] * 3 - 1; pElemDOFNum[18 -1] = self.DOFMap[N6 -1] * 3
            pElemDOFNum[19 -1] = self.DOFMap[N7 -1] * 3 - 2; pElemDOFNum[20 -1] = self.DOFMap[N7 -1] * 3 - 1; pElemDOFNum[21 -1] = self.DOFMap[N7 -1] * 3
            pElemDOFNum[22 -1] = self.DOFMap[N8 -1] * 3 - 2; pElemDOFNum[23 -1] = self.DOFMap[N8 -1] * 3 - 1; pElemDOFNum[24 -1] = self.DOFMap[N8 -1] * 3
            for i in range(24):
                self.bvec[pElemDOFNum[i] -1] += self.m_B[self.axis, i, self.elemMatMap[e -1] -1]

    def assemble_Amatrix(self):
        print("Assembling system")
        y = np.zeros(self.nDOFs, dtype=float)
        pElemDOFNum = np.zeros(24, dtype=int)

        def matvec(x):
            for e in range(1, self.nElems + 1):
                N1 = 2 + (e - 1) % self.nElemS + ((e - 1) % self.nElemS) // self.len_y + (e - 1) // self.nElemS * self.nNodeS - 1
                N3 = N1 + self.len_y
                N2 = N3 + 1
                N4 = N1 - 1
                N5 = N1 + self.nNodeS
                N6 = N2 + self.nNodeS
                N7 = N3 + self.nNodeS
                N8 = N4 + self.nNodeS
                pElemDOFNum[1 -1]  = self.DOFMap[N1] * 3 - 2
                pElemDOFNum[2 -1]  = self.DOFMap[N1] * 3 - 1
                pElemDOFNum[3 -1]  = self.DOFMap[N1] * 3
                pElemDOFNum[4 -1]  = self.DOFMap[N2] * 3 - 2
                pElemDOFNum[5 -1]  = self.DOFMap[N2] * 3 - 1
                pElemDOFNum[6 -1]  = self.DOFMap[N2] * 3
                pElemDOFNum[7 -1]  = self.DOFMap[N3] * 3 - 2
                pElemDOFNum[8 -1]  = self.DOFMap[N3] * 3 - 1
                pElemDOFNum[9 -1]  = self.DOFMap[N3] * 3
                pElemDOFNum[10 -1] = self.DOFMap[N4] * 3 - 2
                pElemDOFNum[11 -1] = self.DOFMap[N4] * 3 - 1
                pElemDOFNum[12 -1] = self.DOFMap[N4] * 3
                pElemDOFNum[13 -1] = self.DOFMap[N5] * 3 - 2
                pElemDOFNum[14 -1] = self.DOFMap[N5] * 3 - 1
                pElemDOFNum[15 -1] = self.DOFMap[N5] * 3
                pElemDOFNum[16 -1] = self.DOFMap[N6] * 3 - 2
                pElemDOFNum[17 -1] = self.DOFMap[N6] * 3 - 1
                pElemDOFNum[18 -1] = self.DOFMap[N6] * 3
                pElemDOFNum[19 -1] = self.DOFMap[N7] * 3 - 2
                pElemDOFNum[20 -1] = self.DOFMap[N7] * 3 - 1
                pElemDOFNum[21 -1] = self.DOFMap[N7] * 3
                pElemDOFNum[22 -1] = self.DOFMap[N8] * 3 - 2
                pElemDOFNum[23 -1] = self.DOFMap[N8] * 3 - 1
                pElemDOFNum[24 -1] = self.DOFMap[N8] * 3
                for i in range(24):
                    q_temp = 0
                    for j in range(24):
                        q_temp += self.m_K[i, j, self.elemMatMap[e -1] -1] * x[pElemDOFNum[j] -1]
                    y[pElemDOFNum[i] -1] += q_temp
            return y

        self.Amat = LinearOperator(shape=(self.nDOFs, self.nDOFs), matvec=matvec)

    def compute_effective_coefficient(self):
        print("Computing effective elasticity")
        self.Ceff = np.zeros((6, 6), dtype=float)
        pElemDOFNum = np.zeros(24, dtype=int)

        t = np.zeros(24, dtype=float)
        if   self.axis == 0: t[4 -1] = 1; t[7 -1] = 1; t[16 -1] = 1; t[19 -1] = 1
        elif self.axis == 1: t[8 -1] = 1; t[11 -1] = 1; t[20 -1] = 1; t[23 -1] = 1
        elif self.axis == 2: t[15 -1] = 1; t[18 -1] = 1; t[21 -1] = 1; t[24 -1] = 1
        elif self.axis == 3: t[7 -1] = 1; t[10 -1] = 1; t[19 -1] = 1; t[22 -1] = 1
        elif self.axis == 4: t[13 -1] = 1; t[16 -1] = 1; t[19 -1] = 1; t[22 -1] = 1
        elif self.axis == 5: t[9 -1] = 1; t[12 -1] = 1; t[21 -1] = 1; t[24 -1] = 1

        SX = 0; SY = 0; SZ = 0; SXY = 0; SXZ = 0; SYZ = 0

        for e in range(1, self.nElems + 1):
            N1 = 2 + (e - 1) % self.nElemS + ((e - 1) % self.nElemS) // self.len_y + (e - 1) // self.nElemS * self.nNodeS
            N3 = N1 + self.len_y; N2 = N3 + 1; N4 = N1 - 1
            N5 = N1 + self.nNodeS; N6 = N2 + self.nNodeS; N7 = N3 + self.nNodeS; N8 = N4 + self.nNodeS
            pElemDOFNum[1 -1]  = self.DOFMap[N1 -1] * 3 - 2; pElemDOFNum[2 -1]  = self.DOFMap[N1 -1] * 3 - 1; pElemDOFNum[3 -1]  = self.DOFMap[N1 -1] * 3
            pElemDOFNum[4 -1]  = self.DOFMap[N2 -1] * 3 - 2; pElemDOFNum[5 -1]  = self.DOFMap[N2 -1] * 3 - 1; pElemDOFNum[6 -1]  = self.DOFMap[N2 -1] * 3
            pElemDOFNum[7 -1]  = self.DOFMap[N3 -1] * 3 - 2; pElemDOFNum[8 -1]  = self.DOFMap[N3 -1] * 3 - 1; pElemDOFNum[9 -1]  = self.DOFMap[N3 -1] * 3
            pElemDOFNum[10 -1] = self.DOFMap[N4 -1] * 3 - 2; pElemDOFNum[11 -1] = self.DOFMap[N4 -1] * 3 - 1; pElemDOFNum[12 -1] = self.DOFMap[N4 -1] * 3
            pElemDOFNum[13 -1] = self.DOFMap[N5 -1] * 3 - 2; pElemDOFNum[14 -1] = self.DOFMap[N5 -1] * 3 - 1; pElemDOFNum[15 -1] = self.DOFMap[N5 -1] * 3
            pElemDOFNum[16 -1] = self.DOFMap[N6 -1] * 3 - 2; pElemDOFNum[17 -1] = self.DOFMap[N6 -1] * 3 - 1; pElemDOFNum[18 -1] = self.DOFMap[N6 -1] * 3
            pElemDOFNum[19 -1] = self.DOFMap[N7 -1] * 3 - 2; pElemDOFNum[20 -1] = self.DOFMap[N7 -1] * 3 - 1; pElemDOFNum[21 -1] = self.DOFMap[N7 -1] * 3
            pElemDOFNum[22 -1] = self.DOFMap[N8 -1] * 3 - 2; pElemDOFNum[23 -1] = self.DOFMap[N8 -1] * 3 - 1; pElemDOFNum[24 -1] = self.DOFMap[N8 -1] * 3
            for i in range(24):
                SX  += self.m_B[1 -1, i, self.elemMatMap[e -1] -1] * (t[i] - self.x[pElemDOFNum[i] -1])
                SY  += self.m_B[2 -1, i, self.elemMatMap[e -1] -1] * (t[i] - self.x[pElemDOFNum[i] -1])
                SZ  += self.m_B[3 -1, i, self.elemMatMap[e -1] -1] * (t[i] - self.x[pElemDOFNum[i] -1])
                SXY += self.m_B[4 -1, i, self.elemMatMap[e -1] -1] * (t[i] - self.x[pElemDOFNum[i] -1])
                SXZ += self.m_B[5 -1, i, self.elemMatMap[e -1] -1] * (t[i] - self.x[pElemDOFNum[i] -1])
                SYZ += self.m_B[6 -1, i, self.elemMatMap[e -1] -1] * (t[i] - self.x[pElemDOFNum[i] -1])

        self.Ceff[self.axis, 0] = SX / self.nElems;  self.Ceff[self.axis, 1] = SY / self.nElems;  self.Ceff[self.axis, 2] = SZ / self.nElems
        self.Ceff[self.axis, 3] = SXY / self.nElems; self.Ceff[self.axis, 4] = SXZ / self.nElems; self.Ceff[self.axis, 5] = SYZ / self.nElems

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
            # elemProps = self.mat_elast[j_mat]
            # E = elemProps[0]
            # p = elemProps[1]
            # E = E / ((1. + p) * (1. - 2 * p))
            # C[1 -1, 1 -1] = 1 - p;            C[1 -1, 2 -1] = p;                C[1 -1, 3 -1] = p
            # C[2 -1, 1 -1] = p;                C[2 -1, 2 -1] = 1 - p;            C[2 -1, 3 -1] = p
            # C[3 -1, 1 -1] = p;                C[3 -1, 2 -1] = p;                C[3 -1, 3 -1] = 1 - p
            # C[4 -1, 4 -1] = (1 - 2 * p) / 2;  C[5 -1, 5 -1] = (1 - 2 * p) / 2;  C[6 -1, 6 -1] = (1 - 2 * p) / 2
            # C *= E

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
                        dN1dr = -(1 - s) * (1 - t) * .125; dN2dr =  (1 - s) * (1 - t) * .125; dN3dr =  (1 + s) * (1 - t) * .125; dN4dr = -(1 + s) * (1 - t) * .125
                        dN5dr = -(1 - s) * (1 + t) * .125; dN6dr =  (1 - s) * (1 + t) * .125; dN7dr =  (1 + s) * (1 + t) * .125; dN8dr = -(1 + s) * (1 + t) * .125
                        dN1ds = -(1 - r) * (1 - t) * .125; dN2ds = -(1 + r) * (1 - t) * .125; dN3ds =  (1 + r) * (1 - t) * .125; dN4ds =  (1 - r) * (1 - t) * .125
                        dN5ds = -(1 - r) * (1 + t) * .125; dN6ds = -(1 + r) * (1 + t) * .125; dN7ds =  (1 + r) * (1 + t) * .125; dN8ds =  (1 - r) * (1 + t) * .125
                        dN1dt = -(1 - r) * (1 - s) * .125; dN2dt = -(1 + r) * (1 - s) * .125; dN3dt = -(1 + r) * (1 + s) * .125; dN4dt = -(1 - r) * (1 + s) * .125
                        dN5dt =  (1 - r) * (1 - s) * .125; dN6dt =  (1 + r) * (1 - s) * .125; dN7dt =  (1 + r) * (1 + s) * .125; dN8dt =  (1 - r) * (1 + s) * .125
                        dN = np.array([[dN1dr, dN2dr, dN3dr, dN4dr, dN5dr, dN6dr, dN7dr, dN8dr],
                                       [dN1ds, dN2ds, dN3ds, dN4ds, dN5ds, dN6ds, dN7ds, dN8ds],
                                       [dN1dt, dN2dt, dN3dt, dN4dt, dN5dt, dN6dt, dN7dt, dN8dt]])
                        J = dN @ X.T
                        dNdx = np.linalg.inv(J) @ dN
                        B[1 -1, 1 -1]  = dNdx[1 -1, 1 -1];  B[2 -1, 2 -1]  = dNdx[2 -1, 1 -1];  B[3 -1, 3 -1]  = dNdx[3 -1, 1 -1]
                        B[1 -1, 4 -1]  = dNdx[1 -1, 2 -1];  B[2 -1, 5 -1]  = dNdx[2 -1, 2 -1];  B[3 -1, 6 -1]  = dNdx[3 -1, 2 -1]
                        B[1 -1, 7 -1]  = dNdx[1 -1, 3 -1];  B[2 -1, 8 -1]  = dNdx[2 -1, 3 -1];  B[3 -1, 9 -1]  = dNdx[3 -1, 3 -1]
                        B[1 -1, 10 -1] = dNdx[1 -1, 4 -1];  B[2 -1, 11 -1] = dNdx[2 -1, 4 -1];  B[3 -1, 12 -1] = dNdx[3 -1, 4 -1]
                        B[1 -1, 13 -1] = dNdx[1 -1, 5 -1];  B[2 -1, 14 -1] = dNdx[2 -1, 5 -1];  B[3 -1, 15 -1] = dNdx[3 -1, 5 -1]
                        B[1 -1, 16 -1] = dNdx[1 -1, 6 -1];  B[2 -1, 17 -1] = dNdx[2 -1, 6 -1];  B[3 -1, 18 -1] = dNdx[3 -1, 6 -1]
                        B[1 -1, 19 -1] = dNdx[1 -1, 7 -1];  B[2 -1, 20 -1] = dNdx[2 -1, 7 -1];  B[3 -1, 21 -1] = dNdx[3 -1, 7 -1]
                        B[1 -1, 22 -1] = dNdx[1 -1, 8 -1];  B[2 -1, 23 -1] = dNdx[2 -1, 8 -1];  B[3 -1, 24 -1] = dNdx[3 -1, 8 -1]
                        B[4 -1, 1 -1]  = dNdx[2 -1, 1 -1];  B[5 -1, 1 -1]  = dNdx[3 -1, 1 -1];  B[6 -1, 2 -1]  = dNdx[3 -1, 1 -1]
                        B[4 -1, 2 -1]  = dNdx[1 -1, 1 -1];  B[5 -1, 3 -1]  = dNdx[1 -1, 1 -1];  B[6 -1, 3 -1]  = dNdx[2 -1, 1 -1]
                        B[4 -1, 4 -1]  = dNdx[2 -1, 2 -1];  B[5 -1, 4 -1]  = dNdx[3 -1, 2 -1];  B[6 -1, 5 -1]  = dNdx[3 -1, 2 -1]
                        B[4 -1, 5 -1]  = dNdx[1 -1, 2 -1];  B[5 -1, 6 -1]  = dNdx[1 -1, 2 -1];  B[6 -1, 6 -1]  = dNdx[2 -1, 2 -1]
                        B[4 -1, 7 -1]  = dNdx[2 -1, 3 -1];  B[5 -1, 7 -1]  = dNdx[3 -1, 3 -1];  B[6 -1, 8 -1]  = dNdx[3 -1, 3 -1]
                        B[4 -1, 8 -1]  = dNdx[1 -1, 3 -1];  B[5 -1, 9 -1]  = dNdx[1 -1, 3 -1];  B[6 -1, 9 -1]  = dNdx[2 -1, 3 -1]
                        B[4 -1, 10 -1] = dNdx[2 -1, 4 -1];  B[5 -1, 10 -1] = dNdx[3 -1, 4 -1];  B[6 -1, 11 -1] = dNdx[3 -1, 4 -1]
                        B[4 -1, 11 -1] = dNdx[1 -1, 4 -1];  B[5 -1, 12 -1] = dNdx[1 -1, 4 -1];  B[6 -1, 12 -1] = dNdx[2 -1, 4 -1]
                        B[4 -1, 13 -1] = dNdx[2 -1, 5 -1];  B[5 -1, 13 -1] = dNdx[3 -1, 5 -1];  B[6 -1, 14 -1] = dNdx[3 -1, 5 -1]
                        B[4 -1, 14 -1] = dNdx[1 -1, 5 -1];  B[5 -1, 15 -1] = dNdx[1 -1, 5 -1];  B[6 -1, 15 -1] = dNdx[2 -1, 5 -1]
                        B[4 -1, 16 -1] = dNdx[2 -1, 6 -1];  B[5 -1, 16 -1] = dNdx[3 -1, 6 -1];  B[6 -1, 17 -1] = dNdx[3 -1, 6 -1]
                        B[4 -1, 17 -1] = dNdx[1 -1, 6 -1];  B[5 -1, 18 -1] = dNdx[1 -1, 6 -1];  B[6 -1, 18 -1] = dNdx[2 -1, 6 -1]
                        B[4 -1, 19 -1] = dNdx[2 -1, 7 -1];  B[5 -1, 19 -1] = dNdx[3 -1, 7 -1];  B[6 -1, 20 -1] = dNdx[3 -1, 7 -1]
                        B[4 -1, 20 -1] = dNdx[1 -1, 7 -1];  B[5 -1, 21 -1] = dNdx[1 -1, 7 -1];  B[6 -1, 21 -1] = dNdx[2 -1, 7 -1]
                        B[4 -1, 22 -1] = dNdx[2 -1, 8 -1];  B[5 -1, 22 -1] = dNdx[3 -1, 8 -1];  B[6 -1, 23 -1] = dNdx[3 -1, 8 -1]
                        B[4 -1, 23 -1] = dNdx[1 -1, 8 -1];  B[5 -1, 24 -1] = dNdx[1 -1, 8 -1];  B[6 -1, 24 -1] = dNdx[2 -1, 8 -1]

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
