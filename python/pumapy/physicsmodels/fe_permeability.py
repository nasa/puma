"""
The following FE numerical method and implementation are based on the following research paper:

Vianna, R.S., Cunha, A.M., Azeredo, R.B., Leiderman, R. and Pereira, A., 2020.
Computing Effective Permeability of Porous Media with FEM and Micro-CT: An Educational Approach.
Fluids, 5(1), p.16.

See https://www.mdpi.com/2311-5521/5/1/16 for the publication.
See https://zenodo.org/record/3612168#.YUYlSWZKhTZ for 2D MATLAB implementation.
"""
from pumapy.utilities.timer import Timer
from pumapy.utilities.workspace import Workspace
from pumapy.physicsmodels.linear_solvers import PropertySolver
from pumapy.utilities.generic_checks import estimate_max_memory
from scipy.sparse import coo_matrix
import numpy as np


class Permeability(PropertySolver):

    def __init__(self, workspace, solid_cutoff, tolerance, maxiter, solver_type, display_iter):
        allowed_solvers = ['minres', 'direct', 'cg', 'bicgstab']
        super().__init__(workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter)

        self.ws = workspace.copy()
        self.solid_cutoff = solid_cutoff
        self.ws.binarize_range(self.solid_cutoff)

        self.solver_type = solver_type
        self.len_x, self.len_y, self.len_z = self.ws.matrix.shape
        self.voxlength = self.ws.voxel_length

        self.nel = self.len_x * self.len_y
        self.nels = self.len_x * self.len_y * self.len_z
        self.nnP2 = (self.len_x + 1) * (self.len_y + 1)
        self.velF = np.where(self.ws.matrix.ravel(order='F') == 0)[0].astype(np.uint32)  # only fluid elements
        self.nelF = self.velF.shape[0]

        self.ke = np.zeros((24, 24), dtype=float)
        self.ge = np.zeros((24, 8),  dtype=float)
        self.pe = np.zeros((8, 8),   dtype=float)
        self.fe = np.zeros((8, 1),   dtype=float)
        self.mgdlF = np.zeros((32, self.nelF), dtype=np.uint32)
        self.resolveF = None
        self.x_full = None
        self.bvec_full = None

        self.keff = np.zeros((3, 3))
        self.solve_time = -1
        self.u_x, self.u_y, self.u_z = None, None, None

    def compute(self):
        t = Timer()
        estimate_max_memory("permeability", self.ws.get_shape(), self.solver_type)
        self.initialize()
        self.assemble_bvector()
        self.assemble_Amatrix()
        print("Time to assemble matrices: ", t.elapsed()); t.reset()
        self.solve()
        print("Time to solve: ", t.elapsed())
        self.compute_effective_coefficient()
        self.solve_time = t.elapsed()

    def initialize(self):
        print("Initializing indexing matrices ... ", flush=True, end='')

        # Matrix with element connectivity with Periodic boundary conditions (PBC)
        mConectP1 = np.zeros((self.nel, 4, self.len_z + 1), dtype=np.uint32)
        aux = 1
        for slice in range(self.len_z):
            mIdNosP = np.reshape(np.arange(aux, aux + self.nel, dtype=np.uint32), (self.len_x, self.len_y), order='F')
            mIdNosP = np.append(mIdNosP, mIdNosP[0][np.newaxis], axis=0)  # Numbering bottom nodes
            mIdNosP = np.append(mIdNosP, mIdNosP[:, 0][:, np.newaxis], axis=1)  # Numbering right nodes
            mConectP1[:, 0, slice] = np.ravel(mIdNosP[1:, :-1], order='F')
            mConectP1[:, 1, slice] = np.ravel(mIdNosP[1:, 1:], order='F')
            mConectP1[:, 2, slice] = np.ravel(mIdNosP[:-1, 1:], order='F')
            mConectP1[:, 3, slice] = np.ravel(mIdNosP[:-1, :-1], order='F')
            aux += self.nel
        mConectP1[:, :, -1] = mConectP1[:, :, 0]

        mConectP = np.zeros((self.nels, 8), dtype=np.uint32)
        for slice in range(self.len_z):
            mConectP[self.nel * slice:self.nel * (slice + 1), :4] = mConectP1[:, :, slice]
            mConectP[self.nel * slice:self.nel * (slice + 1), 4:] = mConectP1[:, :, slice + 1]
        del mConectP1

        # matrix to reduce the system
        resolveF_u = np.arange(1, self.nels * 3 + 1, dtype=np.uint32)
        nosnulos = np.unique(mConectP[np.where(self.ws.matrix.ravel(order='F') > 0)[0], :8])
        gdlnulos = np.hstack((nosnulos * 3 - 2, nosnulos * 3 - 1, nosnulos * 3))
        resolveF_u = np.delete(resolveF_u, gdlnulos - 1)
        resolveF_p = self.nels * 3 + np.unique(mConectP[np.where(self.ws.matrix.ravel(order='F') == 0)[0].astype(np.uint32), :8])
        del nosnulos, gdlnulos
        self.resolveF = np.hstack((resolveF_u, resolveF_p)) - 1
        del resolveF_u, resolveF_p

        # degrees of freedom matrix
        self.mgdlF[0] =  mConectP[self.velF, 0] * 3 - 2
        self.mgdlF[1] =  mConectP[self.velF, 0] * 3 - 1
        self.mgdlF[2] =  mConectP[self.velF, 0] * 3
        self.mgdlF[3] =  mConectP[self.velF, 1] * 3 - 2
        self.mgdlF[4] =  mConectP[self.velF, 1] * 3 - 1
        self.mgdlF[5] =  mConectP[self.velF, 1] * 3
        self.mgdlF[6] =  mConectP[self.velF, 2] * 3 - 2
        self.mgdlF[7] =  mConectP[self.velF, 2] * 3 - 1
        self.mgdlF[8] =  mConectP[self.velF, 2] * 3
        self.mgdlF[9] =  mConectP[self.velF, 3] * 3 - 2
        self.mgdlF[10] = mConectP[self.velF, 3] * 3 - 1
        self.mgdlF[11] = mConectP[self.velF, 3] * 3
        self.mgdlF[12] = mConectP[self.velF, 4] * 3 - 2
        self.mgdlF[13] = mConectP[self.velF, 4] * 3 - 1
        self.mgdlF[14] = mConectP[self.velF, 4] * 3
        self.mgdlF[15] = mConectP[self.velF, 5] * 3 - 2
        self.mgdlF[16] = mConectP[self.velF, 5] * 3 - 1
        self.mgdlF[17] = mConectP[self.velF, 5] * 3
        self.mgdlF[18] = mConectP[self.velF, 6] * 3 - 2
        self.mgdlF[19] = mConectP[self.velF, 6] * 3 - 1
        self.mgdlF[20] = mConectP[self.velF, 6] * 3
        self.mgdlF[21] = mConectP[self.velF, 7] * 3 - 2
        self.mgdlF[22] = mConectP[self.velF, 7] * 3 - 1
        self.mgdlF[23] = mConectP[self.velF, 7] * 3
        self.mgdlF[24] = 3 * self.nels + mConectP[self.velF, 0]
        self.mgdlF[25] = 3 * self.nels + mConectP[self.velF, 1]
        self.mgdlF[26] = 3 * self.nels + mConectP[self.velF, 2]
        self.mgdlF[27] = 3 * self.nels + mConectP[self.velF, 3]
        self.mgdlF[28] = 3 * self.nels + mConectP[self.velF, 4]
        self.mgdlF[29] = 3 * self.nels + mConectP[self.velF, 5]
        self.mgdlF[30] = 3 * self.nels + mConectP[self.velF, 6]
        self.mgdlF[31] = 3 * self.nels + mConectP[self.velF, 7]
        print("Done")

    def assemble_bvector(self):
        print("Assembling b vector ... ", flush=True, end='')
        iF = np.hstack((np.reshape(self.mgdlF[:24:3],  self.nelF * 8, order='F'),
                        np.reshape(self.mgdlF[1:24:3], self.nelF * 8, order='F'),
                        np.reshape(self.mgdlF[2:24:3], self.nelF * 8, order='F'))) - 1
        jF = np.hstack((np.zeros(self.nelF * 8, dtype=np.uint8),
                        np.ones(self.nelF * 8, dtype=np.uint8),
                        np.full(self.nelF * 8, 2, dtype=np.uint8)))

        self.calculate_element_matrices()
        sF = np.squeeze(np.tile(self.fe, (self.nelF * 3, 1)))
        self.bvec_full = coo_matrix((sF, (iF, jF)), shape=(4 * self.nels, 3)).tocsr()
        self.bvec_full = self.bvec_full[self.resolveF]  # reducing vector
        print("Done")

    def assemble_Amatrix(self):
        print("Initializing large data structures ... ", flush=True, end='')
        iK = np.repeat(np.reshape(self.mgdlF[:24], self.nelF * 24, order='F'), 24)
        iG = np.repeat(np.reshape(self.mgdlF[:24], self.nelF * 24, order='F'), 8)
        jG = np.reshape(np.repeat(self.mgdlF[24:], 24, axis=1), self.nelF * 192, order='F')
        iP = np.repeat(np.reshape(self.mgdlF[24:], self.nelF * 8, order='F'), 8)
        iA = np.hstack((iK, iG, jG, iP))
        del iK, iP
        jK = np.reshape(np.repeat(self.mgdlF[:24], 24, axis=1), self.nelF * 576, order='F')
        jP = np.reshape(np.repeat(self.mgdlF[24:], 8, axis=1), self.nelF * 64, order='F')
        jA = np.hstack((jK, jG, iG, jP))
        del jK, jG, iG, jP
        iA -= 1
        jA -= 1
        coeff = np.hstack((np.tile(self.ke, self.nelF), np.tile(self.ge, self.nelF),
                           np.tile(self.ge, self.nelF), -np.tile(self.pe, self.nelF)))
        del self.fe, self.ke, self.ge, self.pe

        print("Done\nAssembling A matrix ... ", flush=True, end='')
        self.Amat = coo_matrix((coeff, (iA, jA))).tocsr()
        del coeff, iA, jA
        print("Done")

        # Reducing system of equations
        self.Amat = self.Amat[self.resolveF][:, self.resolveF]

        print("Done")

    def solve(self):
        self.x_full = np.zeros((4 * self.nels, 3), dtype=float)
        # solves the system in the 3 directions directly
        if self.solver_type == 'direct':
            self.bvec = self.bvec_full
            super().solve()
            self.x_full[self.resolveF] = self.x.toarray()
        else:  # solves one direction at a time
            self.del_matrices = False
            for i in range(3):
                self.bvec = self.bvec_full[:, i]
                super().solve()
                self.x_full[self.resolveF, i] = self.x
            del self.Amat, self.bvec, self.initial_guess
        del self.bvec_full, self.resolveF

    def compute_effective_coefficient(self):
        aux = 1
        vIdNosP = np.zeros(self.len_z * self.nnP2, dtype=np.uint32)
        for slice in range(self.len_z):
            mIdNosP = np.reshape(np.arange(aux, aux + self.nel, dtype=np.uint32), (self.len_x, self.len_y), order='F')
            mIdNosP = np.append(mIdNosP, mIdNosP[0][np.newaxis], axis=0)  # Numbering bottom nodes
            mIdNosP = np.append(mIdNosP, mIdNosP[:, 0][:, np.newaxis], axis=1)  # Numbering right nodes

            vIdNosP[self.nnP2 * slice:self.nnP2 * (slice + 1)] = mIdNosP.ravel(order='F')
            aux += self.nel
        del mIdNosP

        vIdNosP = np.append(vIdNosP, (vIdNosP[:self.nnP2]))
        vIdNosP = np.unique(vIdNosP)

        self.keff[0, 0] =   np.sum(self.x_full[vIdNosP * 3 - 2, 1])
        self.keff[0, 1] = - np.sum(self.x_full[vIdNosP * 3 - 3, 1])
        self.keff[0, 2] = - np.sum(self.x_full[vIdNosP * 3 - 1, 1])
        self.keff[1, 0] = - np.sum(self.x_full[vIdNosP * 3 - 2, 0])
        self.keff[1, 1] =   np.sum(self.x_full[vIdNosP * 3 - 3, 0])
        self.keff[1, 2] =   np.sum(self.x_full[vIdNosP * 3 - 1, 0])
        self.keff[2, 0] = - np.sum(self.x_full[vIdNosP * 3 - 2, 2])
        self.keff[2, 1] =   np.sum(self.x_full[vIdNosP * 3 - 3, 2])
        self.keff[2, 2] =   np.sum(self.x_full[vIdNosP * 3 - 1, 2])
        self.keff /= self.nels
        print(f'\nEffective permeability tensor: \n{self.keff}')

        # Extracting velocity fields
        self.u_x = np.zeros((self.len_x, self.len_y, self.len_z, 3))
        self.u_y = np.zeros((self.len_x, self.len_y, self.len_z, 3))
        self.u_z = np.zeros((self.len_x, self.len_y, self.len_z, 3))
        self.u_x[:, :, :, 0] =   np.reshape(self.x_full[vIdNosP * 3 - 2, 1], (self.len_x, self.len_y, self.len_z), order='F')
        self.u_x[:, :, :, 1] = - np.reshape(self.x_full[vIdNosP * 3 - 3, 1], (self.len_x, self.len_y, self.len_z), order='F')
        self.u_x[:, :, :, 2] = - np.reshape(self.x_full[vIdNosP * 3 - 1, 1], (self.len_x, self.len_y, self.len_z), order='F')
        self.u_y[:, :, :, 0] = - np.reshape(self.x_full[vIdNosP * 3 - 2, 0], (self.len_x, self.len_y, self.len_z), order='F')
        self.u_y[:, :, :, 1] =   np.reshape(self.x_full[vIdNosP * 3 - 3, 0], (self.len_x, self.len_y, self.len_z), order='F')
        self.u_y[:, :, :, 2] =   np.reshape(self.x_full[vIdNosP * 3 - 1, 0], (self.len_x, self.len_y, self.len_z), order='F')
        self.u_z[:, :, :, 0] = - np.reshape(self.x_full[vIdNosP * 3 - 2, 2], (self.len_x, self.len_y, self.len_z), order='F')
        self.u_z[:, :, :, 1] =   np.reshape(self.x_full[vIdNosP * 3 - 3, 2], (self.len_x, self.len_y, self.len_z), order='F')
        self.u_z[:, :, :, 2] =   np.reshape(self.x_full[vIdNosP * 3 - 1, 2], (self.len_x, self.len_y, self.len_z), order='F')

        # Extracting pressure fields
        self.p_x = np.reshape(self.x_full[vIdNosP + self.nels * 3 - 1, 1], (self.len_x, self.len_y, self.len_z), order='F')
        self.p_y = np.reshape(self.x_full[vIdNosP + self.nels * 3 - 1, 0], (self.len_x, self.len_y, self.len_z), order='F')
        self.p_z = np.reshape(self.x_full[vIdNosP + self.nels * 3 - 1, 2], (self.len_x, self.len_y, self.len_z), order='F')

    def calculate_element_matrices(self):
        coordsElem = np.array([[0, 0, 0], [self.voxlength, 0, 0], [self.voxlength, self.voxlength, 0],
                               [0, self.voxlength, 0], [0, 0, self.voxlength], [self.voxlength, 0, self.voxlength],
                               [self.voxlength, self.voxlength, self.voxlength], [0, self.voxlength, self.voxlength]])
        rr = np.array([-1. / np.sqrt(3), 1. / np.sqrt(3)])
        ss = rr.copy()
        tt = rr.copy()
        ww = np.array([1, 1])
        C = np.diag([2., 2., 2., 1., 1., 1.])
        stab = (self.voxlength ** 2 + self.voxlength ** 2 + self.voxlength ** 2) / 18.
        mat111000 = np.array([[1.], [1.], [1.], [0.], [0.], [0.]])

        for i in range(2):
            r = rr[i]
            for j in range(2):
                s = ss[j]
                for k in range(2):
                    t = tt[k]

                    N1 = (1 - r) * (1 - s) * (1 - t)
                    N2 = (1 + r) * (1 - s) * (1 - t)
                    N3 = (1 + r) * (1 + s) * (1 - t)
                    N4 = (1 - r) * (1 + s) * (1 - t)
                    N5 = (1 - r) * (1 - s) * (1 + t)
                    N6 = (1 + r) * (1 - s) * (1 + t)
                    N7 = (1 + r) * (1 + s) * (1 + t)
                    N8 = (1 - r) * (1 + s) * (1 + t)
                    N = 0.125 * np.array([[N1, 0, 0, N2, 0, 0, N3, 0, 0, N4, 0, 0, N5, 0, 0, N6, 0, 0, N7, 0, 0, N8, 0, 0],
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
                    weight = detJ * ww[i] * ww[j] * ww[k]
                    invJ = np.linalg.inv(J)
                    DNxy = invJ @ DN
                    B = np.array([[DNxy[0, 0], 0, 0, DNxy[0, 1], 0, 0, DNxy[0, 2], 0, 0, DNxy[0, 3], 0, 0, DNxy[0, 4], 0, 0, DNxy[0, 5], 0, 0, DNxy[0, 6], 0, 0, DNxy[0, 7], 0, 0],
                                  [0, DNxy[1, 0], 0, 0, DNxy[1, 1], 0, 0, DNxy[1, 2], 0, 0, DNxy[1, 3], 0, 0, DNxy[1, 4], 0, 0, DNxy[1, 5], 0, 0, DNxy[1, 6], 0, 0, DNxy[1, 7], 0],
                                  [0, 0, DNxy[2, 0], 0, 0, DNxy[2, 1], 0, 0, DNxy[2, 2], 0, 0, DNxy[2, 3], 0, 0, DNxy[2, 4], 0, 0, DNxy[2, 5], 0, 0, DNxy[2, 6], 0, 0, DNxy[2, 7]],
                                  [DNxy[1, 0], DNxy[0, 0], 0, DNxy[1, 1], DNxy[0, 1], 0, DNxy[1, 2], DNxy[0, 2], 0, DNxy[1, 3], DNxy[0, 3], 0,
                                   DNxy[1, 4], DNxy[0, 4], 0, DNxy[1, 5], DNxy[0, 5], 0, DNxy[1, 6], DNxy[0, 6], 0, DNxy[1, 7], DNxy[0, 7], 0],
                                  [DNxy[2, 0], 0, DNxy[0, 0], DNxy[2, 1], 0, DNxy[0, 1], DNxy[2, 2], 0, DNxy[0, 2], DNxy[2, 3], 0, DNxy[0, 3],
                                   DNxy[2, 4], 0, DNxy[0, 4], DNxy[2, 5], 0, DNxy[0, 5], DNxy[2, 6], 0, DNxy[0, 6], DNxy[2, 7], 0, DNxy[0, 7]],
                                  [0, DNxy[2, 0], DNxy[1, 0], 0, DNxy[2, 1], DNxy[1, 1], 0, DNxy[2, 2], DNxy[1, 2], 0, DNxy[2, 3], DNxy[1, 3],
                                   0, DNxy[2, 4], DNxy[1, 4], 0, DNxy[2, 5], DNxy[1, 5], 0, DNxy[2, 6], DNxy[1, 6], 0, DNxy[2, 7], DNxy[1, 7]]])

                    self.ke += weight * B.T @ C @ B
                    self.ge += weight * B.T @ mat111000 @ N.ravel(order='F')[::9][np.newaxis]
                    self.pe += weight * stab * DNxy.T @ DNxy
                    self.fe += weight * N[0][::3][:, np.newaxis]

        self.ke = self.ke.ravel()
        self.ge = self.ge.ravel()
        self.pe = self.pe.ravel()

    def log_input(self):
        self.ws.log.log_section("Computing Permeability")
        self.ws.log.log_line("Domain Size: " + str(self.ws.get_shape()))
        self.ws.log.log_line("Solver Type: " + str(self.solver_type))
        self.ws.log.log_line("Solver Tolerance: " + str(self.tolerance))
        self.ws.log.log_line("Max Iterations: " + str(self.maxiter))
        self.ws.log.write_log()

    def log_output(self):
        self.ws.log.log_section("Finished Permeability Calculation")
        self.ws.log.log_line("Permeability: " + str(self.keff))
        self.ws.log.log_line("Solver Time: " + str(self.solve_time))
        self.ws.log.write_log()

    def error_check(self):
        if not isinstance(self.ws, Workspace):
            raise Exception("Workspace must be a puma.Workspace.")
