from pumapy.utilities.timer import Timer
from pumapy.utilities.generic_checks import check_ws_cutoff
from pumapy.utilities.logger import print_warning
from pumapy.utilities.linear_solvers import PropertySolver
from scipy.sparse import csc_matrix, diags
import numpy as np
import sys


class Permeability(PropertySolver):

    def __init__(self, workspace, cutoff, tolerance, maxiter, solver_type, display_iter):
        allowed_solvers = ['minres', 'direct', 'gmres', 'cg', 'bicgstab']
        super().__init__(workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter)

        self.ws = workspace.copy()
        self.cutoff = cutoff
        self.solver_type = solver_type
        self.len_x, self.len_y, self.len_z = self.ws.matrix.shape
        self.voxel_length = self.ws.voxel_length

        self.nel = self.len_x * self.len_y
        self.nels = self.len_x * self.len_y * self.len_z
        self.nnP2 = (self.len_x + 1) * (self.len_y + 1)

        self.ke, self.ge, self.pe = None, None, None

    def compute(self):
        self.calculate_element_matrices()
        self.assemble_matrices()
        super().solve()
        self.__compute_effective_coefficient()

    def calculate_element_matrices(self):
        coordsElem = np.array([[0, 0, 0], [dx, 0, 0], [dx, dy, 0], [0, dy, 0], [0, 0, dz], [dx, 0, dz], [dx, dy, dz], [0, dy, dz]])
        rr = np.array([-1. / np.sqrt(3), 1. / np.sqrt(3)])
        ss = rr.copy()
        tt = rr.copy()
        ww = np.array([1, 1])
        k = np.zeros((24, 24))
        C = np.diag([2., 2., 2., 1., 1., 1.])
        g = np.zeros((24, 8))
        pe = np.zeros((8, 8))
        stab = (dx ** 2 + dy ** 2 + dz ** 2) / 18.
        f = np.zeros((8, 1))
        mat111000 = np.array([[1.], [1.], [1.], [0.], [0.], [0.]])

        for i in range(2):
            r = rr[i]
            for j in range(2):
                s = ss[j]
                for ks in range(2):
                    t = tt[ks]

                    N1 = (1 - r) * (1 - s) * (1 - t)
                    N2 = (1 + r) * (1 - s) * (1 - t)
                    N3 = (1 + r) * (1 + s) * (1 - t)
                    N4 = (1 - r) * (1 + s) * (1 - t)
                    N5 = (1 - r) * (1 - s) * (1 + t)
                    N6 = (1 + r) * (1 - s) * (1 + t)
                    N7 = (1 + r) * (1 + s) * (1 + t)
                    N8 = (1 - r) * (1 + s) * (1 + t)
                    N = 0.125 * np.array(
                        [[N1, 0, 0, N2, 0, 0, N3, 0, 0, N4, 0, 0, N5, 0, 0, N6, 0, 0, N7, 0, 0, N8, 0, 0],
                         [0, N1, 0, 0, N2, 0, 0, N3, 0, 0, N4, 0, 0, N5, 0, 0, N6, 0, 0, N7, 0, 0, N8, 0],
                         [0, 0, N1, 0, 0, N2, 0, 0, N3, 0, 0, N4, 0, 0, N5, 0, 0, N6, 0, 0, N7, 0, 0, N8]])

                    dN1dr = -0.125 * (1 - s) * (1 - t)
                    dN1ds = -0.125 * (1 - r) * (1 - t)
                    dN1dt = -0.125 * (1 - r) * (1 - s)
                    dN2dr = +0.125 * (1 - s) * (1 - t)
                    dN2ds = -0.125 * (1 + r) * (1 - t)
                    dN2dt = -0.125 * (1 + r) * (1 - s)
                    dN3dr = +0.125 * (1 + s) * (1 - t)
                    dN3ds = +0.125 * (1 + r) * (1 - t)
                    dN3dt = -0.125 * (1 + r) * (1 + s)
                    dN4dr = -0.125 * (1 + s) * (1 - t)
                    dN4ds = +0.125 * (1 - r) * (1 - t)
                    dN4dt = -0.125 * (1 - r) * (1 + s)
                    dN5dr = -0.125 * (1 - s) * (1 + t)
                    dN5ds = -0.125 * (1 - r) * (1 + t)
                    dN5dt = +0.125 * (1 - r) * (1 - s)
                    dN6dr = +0.125 * (1 - s) * (1 + t)
                    dN6ds = -0.125 * (1 + r) * (1 + t)
                    dN6dt = +0.125 * (1 + r) * (1 - s)
                    dN7dr = +0.125 * (1 + s) * (1 + t)
                    dN7ds = +0.125 * (1 + r) * (1 + t)
                    dN7dt = +0.125 * (1 + r) * (1 + s)
                    dN8dr = -0.125 * (1 + s) * (1 + t)
                    dN8ds = +0.125 * (1 - r) * (1 + t)
                    dN8dt = +0.125 * (1 - r) * (1 + s)
                    DN = np.array([[dN1dr, dN2dr, dN3dr, dN4dr, dN5dr, dN6dr, dN7dr, dN8dr],
                                   [dN1ds, dN2ds, dN3ds, dN4ds, dN5ds, dN6ds, dN7ds, dN8ds],
                                   [dN1dt, dN2dt, dN3dt, dN4dt, dN5dt, dN6dt, dN7dt, dN8dt]])

                    J = DN @ coordsElem
                    detJ = np.linalg.det(J)
                    invJ = np.linalg.inv(J)
                    DNxy = invJ @ DN
                    B = np.array([[DNxy[0, 0], 0, 0, DNxy[0, 1], 0, 0, DNxy[0, 2], 0, 0, DNxy[0, 3], 0, 0, DNxy[0, 4],
                                   0, 0, DNxy[0, 5], 0, 0, DNxy[0, 6], 0, 0, DNxy[0, 7], 0, 0],
                                  [0, DNxy[1, 0], 0, 0, DNxy[1, 1], 0, 0, DNxy[1, 2], 0, 0, DNxy[1, 3], 0, 0,
                                   DNxy[1, 4], 0, 0, DNxy[1, 5], 0, 0, DNxy[1, 6], 0, 0, DNxy[1, 7], 0],
                                  [0, 0, DNxy[2, 0], 0, 0, DNxy[2, 1], 0, 0, DNxy[2, 2], 0, 0, DNxy[2, 3], 0, 0,
                                   DNxy[2, 4], 0, 0, DNxy[2, 5], 0, 0, DNxy[2, 6], 0, 0, DNxy[2, 7]],
                                  [DNxy[1, 0], DNxy[0, 0], 0, DNxy[1, 1], DNxy[0, 1], 0, DNxy[1, 2], DNxy[0, 2], 0,
                                   DNxy[1, 3], DNxy[0, 3], 0, DNxy[1, 4], DNxy[0, 4], 0, DNxy[1, 5], DNxy[0, 5], 0,
                                   DNxy[1, 6], DNxy[0, 6], 0, DNxy[1, 7], DNxy[0, 7], 0],
                                  [DNxy[2, 0], 0, DNxy[0, 0], DNxy[2, 1], 0, DNxy[0, 1], DNxy[2, 2], 0, DNxy[0, 2],
                                   DNxy[2, 3], 0, DNxy[0, 3], DNxy[2, 4], 0, DNxy[0, 4], DNxy[2, 5], 0, DNxy[0, 5],
                                   DNxy[2, 6], 0, DNxy[0, 6], DNxy[2, 7], 0, DNxy[0, 7]],
                                  [0, DNxy[2, 0], DNxy[1, 0], 0, DNxy[2, 1], DNxy[1, 1], 0, DNxy[2, 2], DNxy[1, 2], 0,
                                   DNxy[2, 3], DNxy[1, 3], 0, DNxy[2, 4], DNxy[1, 4], 0, DNxy[2, 5], DNxy[1, 5], 0,
                                   DNxy[2, 6], DNxy[1, 6], 0, DNxy[2, 7], DNxy[1, 7]]])
                    weight = detJ * ww[i] * ww[j] * ww[ks]

                    k += weight * B.T @ C @ B
                    g += weight * B.T @ mat111000 @ N.ravel(order='F')[::9][np.newaxis]
                    Bp = invJ @ DN
                    pe += weight * stab * Bp.T @ Bp
                    f += weight * N[0][::3][:, np.newaxis]

        self.ke = self.ke.ravel()
        self.ge = self.ge.ravel()
        self.pe = self.pe.ravel()

    def assemble_matrices():
        mConectP = np.zeros((nel * nelz, 8), dtype=np.uint64)
        mConectP1 = np.zeros((nel, 4, nelz + 1), dtype=np.uint64)
        aux = 1
        for slice in range(nelz):
            mIdNosP = np.reshape(np.arange(aux, aux + nel, dtype=np.uint64), (nely, nelx), order='F')
            mIdNosP = np.append(mIdNosP, mIdNosP[0][np.newaxis], axis=0)  # Numbering bottom nodes
            mIdNosP = np.append(mIdNosP, mIdNosP[:, 0][:, np.newaxis], axis=1)  # Numbering right nodes
            mConectP1[:, 0, slice] = np.ravel(mIdNosP[1:, :-1], order='F')
            mConectP1[:, 1, slice] = np.ravel(mIdNosP[1:, 1:], order='F')
            mConectP1[:, 2, slice] = np.ravel(mIdNosP[:-1, 1:], order='F')
            mConectP1[:, 3, slice] = np.ravel(mIdNosP[:-1, :-1], order='F')
            aux += nelx * nely
        mConectP1[:, :, -1] = mConectP1[:, :, 0]

        for slice in range(nelz):
            mConectP[nel * slice:nel * (slice + 1), :4] = mConectP1[:, :, slice]
            mConectP[nel * slice:nel * (slice + 1), 4:] = mConectP1[:, :, slice + 1]
        del mIdNosP, mConectP1

        # Assembling the system of linear algebraic equations with triplets
        print('--- ASSEMBLY ---')
        velF = np.where(img.ravel(order='F') == 0)[0]  # Vector containing only fluid elements
        nelF = velF.shape[0]

        mgdlF = np.zeros((32, nelF), dtype=np.uint64)
        mgdlF[0] = mConectP[velF, 0] * 3 - 2
        mgdlF[1] = mConectP[velF, 0] * 3 - 1
        mgdlF[2] = mConectP[velF, 0] * 3
        mgdlF[3] = mConectP[velF, 1] * 3 - 2
        mgdlF[4] = mConectP[velF, 1] * 3 - 1
        mgdlF[5] = mConectP[velF, 1] * 3
        mgdlF[6] = mConectP[velF, 2] * 3 - 2
        mgdlF[7] = mConectP[velF, 2] * 3 - 1
        mgdlF[8] = mConectP[velF, 2] * 3
        mgdlF[9] = mConectP[velF, 3] * 3 - 2
        mgdlF[10] = mConectP[velF, 3] * 3 - 1
        mgdlF[11] = mConectP[velF, 3] * 3
        mgdlF[12] = mConectP[velF, 4] * 3 - 2
        mgdlF[13] = mConectP[velF, 4] * 3 - 1
        mgdlF[14] = mConectP[velF, 4] * 3
        mgdlF[15] = mConectP[velF, 5] * 3 - 2
        mgdlF[16] = mConectP[velF, 5] * 3 - 1
        mgdlF[17] = mConectP[velF, 5] * 3
        mgdlF[18] = mConectP[velF, 6] * 3 - 2
        mgdlF[19] = mConectP[velF, 6] * 3 - 1
        mgdlF[20] = mConectP[velF, 6] * 3
        mgdlF[21] = mConectP[velF, 7] * 3 - 2
        mgdlF[22] = mConectP[velF, 7] * 3 - 1
        mgdlF[23] = mConectP[velF, 7] * 3
        mgdlF[24] = 3 * nels + mConectP[velF, 0]
        mgdlF[25] = 3 * nels + mConectP[velF, 1]
        mgdlF[26] = 3 * nels + mConectP[velF, 2]
        mgdlF[27] = 3 * nels + mConectP[velF, 3]
        mgdlF[28] = 3 * nels + mConectP[velF, 4]
        mgdlF[29] = 3 * nels + mConectP[velF, 5]
        mgdlF[30] = 3 * nels + mConectP[velF, 6]
        mgdlF[31] = 3 * nels + mConectP[velF, 7]
        del velF

        iK = np.repeat(np.reshape(mgdlF[:24], nelF * 24, order='F'), 24)
        jK = np.reshape(np.repeat(mgdlF[:24], 24, axis=1), nelF * 576, order='F')
        iG = np.repeat(np.reshape(mgdlF[:24], nelF * 24, order='F'), 8)
        jG = np.reshape(np.repeat(mgdlF[24:], 24, axis=1), nelF * 192, order='F')
        iP = np.repeat(np.reshape(mgdlF[24:], nelF * 8, order='F'), 8)
        jP = np.reshape(np.repeat(mgdlF[24:], 8, axis=1), nelF * 64, order='F')
        iA = np.hstack((iK, iG, jG, iP)) - 1
        del iK, iP
        jA = np.hstack((jK, jG, iG, jP)) - 1
        del jK, iG, jG, jP
        coeff = np.hstack((np.tile(ke, nelF), np.tile(ge, nelF), np.tile(ge, nelF), -np.tile(pe, nelF)))
        A = csc_matrix((coeff, (iA, jA)))

        iF = np.hstack((np.reshape(mgdlF[:24:3], nelF * 8, order='F'),
                        np.reshape(mgdlF[1:24:3], nelF * 8, order='F'),
                        np.reshape(mgdlF[2:24:3], nelF * 8, order='F'))) - 1
        del mgdlF
        jF = np.hstack((np.ones(nelF * 8), np.full(nelF * 8, 2), np.full(nelF * 8, 3))) - 1
        sF = np.squeeze(np.tile(fe, (nelF * 3, 1)))
        F = csc_matrix((sF, (iF, jF)), shape=(ngdlsP, 3))

        # Reducing system of equations
        resolveF_u = np.arange(1, nels * 3 + 1, dtype=np.uint64)
        nosnulos = np.unique(mConectP[np.where(img.ravel(order='F') > 0)[0], :8])
        gdlnulos = np.hstack((nosnulos * 3 - 2, nosnulos * 3 - 1, nosnulos * 3))
        resolveF_u = np.delete(resolveF_u, gdlnulos - 1)
        resolveF_p = nels * 3 + np.unique(mConectP[np.where(img.ravel(order='F') == 0)[0], :8])
        resolveF = np.hstack((resolveF_u, resolveF_p)) - 1
        del resolveF_u, resolveF_p, nosnulos, gdlnulos, mConectP

    def __compute_effective_coefficient(self):
        aux = 1
        vIdNosP = np.zeros(nelz * nnP2, dtype=np.uint64)
        for slice in range(nelz):
            mIdNosP = np.reshape(np.arange(aux, aux + nel, dtype=np.uint64), (nely, nelx), order='F')
            mIdNosP = np.append(mIdNosP, mIdNosP[0][np.newaxis], axis=0)  # Numbering bottom nodes
            mIdNosP = np.append(mIdNosP, mIdNosP[:, 0][:, np.newaxis], axis=1)  # Numbering right nodes

            vIdNosP[nnP2 * slice:nnP2 * (slice + 1)] = mIdNosP.ravel(order='F')
            aux += nelx * nely
        del mIdNosP

        vIdNosP = np.append(vIdNosP, (vIdNosP[:nnP2]))
        vIdNosP = np.unique(vIdNosP)

        KH = np.zeros((3, 3))
        KH[0, 1] = - np.sum(X[vIdNosP * 3 - 3, 1])
        KH[0, 0] = np.sum(X[vIdNosP * 3 - 2, 1])
        KH[0, 2] = - np.sum(X[vIdNosP * 3 - 1, 1])
        KH[1, 1] = np.sum(X[vIdNosP * 3 - 3, 0])
        KH[1, 0] = - np.sum(X[vIdNosP * 3 - 2, 0])
        KH[1, 2] = np.sum(X[vIdNosP * 3 - 1, 0])
        KH[2, 1] = np.sum(X[vIdNosP * 3 - 3, 2])
        KH[2, 0] = - np.sum(X[vIdNosP * 3 - 2, 2])
        KH[2, 2] = np.sum(X[vIdNosP * 3 - 1, 2])
        KH = KH / nels
        print(f'\nEffective permeability tensor: \n{KH}')

        # Extracting velocity and pressure fields
        u_x = np.zeros((img.shape[0], img.shape[1], img.shape[2], 3))
        u_x[:, :, :, 1] = - np.reshape(X[vIdNosP * 3 - 3, 1], (nely, nelx, nelz), order='F')
        u_x[:, :, :, 0] = np.reshape(X[vIdNosP * 3 - 2, 1], (nely, nelx, nelz), order='F')
        u_x[:, :, :, 2] = - np.reshape(X[vIdNosP * 3 - 1, 1], (nely, nelx, nelz), order='F')
        # p_x = np.reshape(X[np.max(vIdNosP) + vIdNosP * 3 - 3, 1], (nely, nelx, nelz), order='F')
        u_y = np.zeros((img.shape[0], img.shape[1], img.shape[2], 3))
        u_y[:, :, :, 1] = np.reshape(X[vIdNosP * 3 - 3, 0], (nely, nelx, nelz), order='F')
        u_y[:, :, :, 0] = - np.reshape(X[vIdNosP * 3 - 2, 0], (nely, nelx, nelz), order='F')
        u_y[:, :, :, 2] = np.reshape(X[vIdNosP * 3 - 1, 0], (nely, nelx, nelz), order='F')
        # p_y = np.reshape(X[np.max(vIdNosP) + vIdNosP * 3:, 0], (nely, nelx, nelz), order='F')
        u_z = np.zeros((img.shape[0], img.shape[1], img.shape[2], 3))
        u_z[:, :, :, 1] = np.reshape(X[vIdNosP * 3 - 3, 2], (nely, nelx, nelz), order='F')
        u_z[:, :, :, 0] = - np.reshape(X[vIdNosP * 3 - 2, 2], (nely, nelx, nelz), order='F')
        u_z[:, :, :, 2] = np.reshape(X[vIdNosP * 3 - 1, 2], (nely, nelx, nelz), order='F')
        # p_z = np.reshape(X[np.max(vIdNosP) + vIdNosP * 3:, 2], (nely, nelx, nelz), order='F')
