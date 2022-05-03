"""
The following FE numerical method and implementation are based on the following research paper:

Pedro C. F. Lopes, Rafael S. Vianna, Victor W. Sapucaia, Federico Semeraro, Ricardo Leiderman, Andre M. B. Pereira, 2022.
Simulation Toolkit for Digital Material Characterization of Large Image-based Microstructures.
(to appear)
"""
from pumapy.utilities.timer import Timer
from pumapy.utilities.logger import print_warning
from pumapy.utilities.workspace import Workspace
from pumapy.physicsmodels.linear_solvers import PropertySolver
from pumapy.utilities.generic_checks import estimate_max_memory
from scipy.sparse import coo_matrix
from scipy.sparse.linalg import LinearOperator
import numpy as np


class Permeability(PropertySolver):

    def __init__(self, workspace, solid_cutoff, direction, tolerance, maxiter, solver_type, display_iter, matrix_free):
        allowed_solvers = ['minres', 'direct', 'cg', 'bicgstab']
        super().__init__(workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter)

        self.ws = workspace.copy()
        self.solid_cutoff = solid_cutoff
        self.direction = direction
        self.direction_dict = {"x": [1], "y": [0], "z": [2], "all": [1, 0, 2]}
        self.ws.binarize_range(self.solid_cutoff)

        self.solver_type = solver_type
        self.matrix_free = matrix_free
        self.len_x, self.len_y, self.len_z = self.ws.matrix.shape
        self.voxlength = self.ws.voxel_length

        self.nel = self.len_x * self.len_y
        self.nels = self.nel * self.len_z
        self.nnP2 = (self.len_x + 1) * (self.len_y + 1)
        self.velF = np.where(self.ws.matrix.ravel(order='F') == 0)[0].astype(np.uint32)  # only fluid elements
        self.nelF = self.velF.shape[0]

        self.k = np.zeros((24, 24), dtype=float)
        self.g = np.zeros((24, 8), dtype=float)
        self.p = np.zeros((8, 8), dtype=float)
        self.f = np.zeros((8, 1), dtype=float)
        self.mgdlF = np.zeros((32, self.nelF), dtype=np.uint32)
        self.solveF = None
        self.x_full = None
        self.bvec_full = None

        self.keff = np.zeros((3, 3))
        self.solve_time = -1
        self.u_x, self.u_y, self.u_z = None, None, None

    def compute(self):
        t = Timer()
        estimate_max_memory("permeability", self.ws.get_shape(), self.solver_type,
                            perm_mf=self.matrix_free, perm_fluid_vf=self.ws.porosity((0, 0)))
        self.initialize()
        self.assemble_bvector()
        self.assemble_Amatrix()
        print("Time to setup system: ", t.elapsed()); t.reset()
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
        self.solveF = np.hstack((resolveF_u, resolveF_p)) - 1
        del resolveF_u, resolveF_p

        # degrees of freedom matrix
        counter = 0
        for i in range(8):
            self.mgdlF[counter] = mConectP[self.velF, i] * 3 - 2
            counter += 1
            self.mgdlF[counter] = mConectP[self.velF, i] * 3 - 1
            counter += 1
            self.mgdlF[counter] = mConectP[self.velF, i] * 3
            counter += 1
        self.mgdlF[24:] = 3 * self.nels + np.swapaxes(mConectP[self.velF], 1, 0)
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
        sF = np.squeeze(np.tile(self.f, (self.nelF * 3, 1)))
        self.bvec_full = coo_matrix((sF, (iF, jF)), shape=(4 * self.nels, 3)).tocsc()
        self.bvec_full = self.bvec_full[self.solveF]  # reducing vector
        print("Done")

    def assemble_Amatrix(self):
        print("Initializing large data structures ... ", flush=True, end='')

        if not self.matrix_free or self.solver_type == 'direct':
            iK = np.repeat(np.reshape(self.mgdlF[:24], self.nelF * 24, order='F'), 24)
            iG = np.repeat(np.reshape(self.mgdlF[:24], self.nelF * 24, order='F'), 8)
            jG = np.reshape(np.repeat(self.mgdlF[24:], 24, axis=1), self.nelF * 192, order='F')
            iP = np.repeat(np.reshape(self.mgdlF[24:], self.nelF * 8, order='F'), 8)
            iA = np.hstack((iK, iG, jG, iP)) - 1
            del iK, iP
            jK = np.reshape(np.repeat(self.mgdlF[:24], 24, axis=1), self.nelF * 576, order='F')
            jP = np.reshape(np.repeat(self.mgdlF[24:], 8, axis=1), self.nelF * 64, order='F')
            jA = np.hstack((jK, jG, iG, jP)) - 1
            del jK, jG, iG, jP, self.mgdlF
            self.k = self.k.ravel()
            self.g = self.g.ravel()
            self.p = self.p.ravel()
            coeff = np.hstack((np.tile(self.k, self.nelF), np.tile(self.g, self.nelF),
                               np.tile(self.g, self.nelF), -np.tile(self.p, self.nelF)))
            del self.f, self.k, self.g, self.p

            print("Done\nAssembling A matrix ... ", flush=True, end='')
            self.Amat = coo_matrix((coeff, (iA, jA))).tocsc()
            del coeff, iA, jA

            # Reducing system of equations
            self.Amat = self.Amat[self.solveF][:, self.solveF]
        else:
            el_dof_v, el_dof_p, only_fluid = self.generate_inds()
            print("Done\nCreating matvec function for matrix-free ... ", flush=True, end='')

            def matvec(x):
                y = np.zeros_like(x)
                ops = np.einsum('ij, jk -> ik', self.g, x[el_dof_p]) * only_fluid
                ops += np.einsum('ij, jk, jk -> ik', self.k, x[el_dof_v], only_fluid) * only_fluid
                np.add.at(y, el_dof_v, ops)
                ops = np.einsum('ji, jk, jk -> ik', self.g, x[el_dof_v], only_fluid)
                ops -= np.einsum('ij, jk -> ik', self.p, x[el_dof_p])
                np.add.at(y, el_dof_p, ops)
                return y

            self.Amat = LinearOperator(shape=(self.solveF.shape[0], self.solveF.shape[0]), matvec=matvec)
            print("Done")

    def generate_inds(self):
        del self.mgdlF

        nodes_n = (self.len_x + 1) * (self.len_y + 1) * (self.len_z + 1)
        slice_n = (self.len_x + 1) * (self.len_y + 1)
        ns = np.arange(1, nodes_n + 1, dtype=int)

        # Nodes's column and row
        slices = ((ns - 1) / ((self.len_x + 1) * (self.len_y + 1)) + 1).astype(int)
        cs = (((ns - (slices - 1) * slice_n - 1) / (self.len_y + 1)) + 1).astype(int)
        rs = (ns - (slices - 1) * slice_n - (cs - 1) * (self.len_y + 1)).astype(int)

        flag_lastcols = (cs // (self.len_x + 1) == 1)
        flag_lastrows = (rs // (self.len_y + 1) == 1)
        flag_lastslices = (slices // (self.len_z + 1) == 1)
        flag_firstcols = cs == 1
        flag_firstrows = rs == 1
        flag_firstslices = slices == 1

        # Check Neighboring   |nw|ne|
        #                      - N -
        #                     |sw|se|
        reps = ns - (slices - 1) * slice_n - cs + (slices - 1) * self.nel + flag_lastslices * ((1 - slices) * self.nel)
        el_se_back = (reps - flag_lastrows * self.len_y + flag_lastcols * (-self.len_x * (self.len_y + 1) + cs - 1) + 1).astype(int)
        el_ne_back = (reps + flag_firstrows * self.len_y + flag_lastcols * (-self.nel)).astype(int)
        el_nw_back = (reps - self.len_y + flag_firstrows * self.len_y + flag_firstcols * self.nel).astype(int)
        el_sw_back = (reps - self.len_y + flag_firstcols * self.nel + flag_lastrows * (-self.len_y) + 1).astype(int)
        reps = flag_firstslices * self.nels + flag_lastslices * self.nels
        del cs, rs, flag_firstcols, flag_firstrows, flag_firstslices
        el_se_front = el_se_back - self.nel + reps
        el_ne_front = el_ne_back - self.nel + reps
        el_nw_front = el_nw_back - self.nel + reps
        el_sw_front = el_sw_back - self.nel + reps
        elem_se_back_ms = el_se_back - 1
        elem_ne_back_ms = el_ne_back - 1
        elem_nw_back_ms = el_nw_back - 1
        elem_sw_back_ms = el_sw_back - 1
        elem_se_front_ms = el_se_front - 1
        elem_ne_front_ms = el_ne_front - 1
        elem_nw_front_ms = el_nw_front - 1
        elem_sw_front_ms = el_sw_front - 1

        # Flags - type of node (At least one fluid elem / At least one solid elem / Only fluid elems)
        mat_map = self.ws.matrix.ravel(order='F') > 0
        flag_one_fluid = - (mat_map[elem_se_front_ms] * mat_map[elem_ne_front_ms] * mat_map[elem_nw_front_ms] *
                            mat_map[elem_sw_front_ms] * mat_map[elem_se_back_ms] * mat_map[elem_ne_back_ms] *
                            mat_map[elem_nw_back_ms] * mat_map[elem_sw_back_ms] - 1)
        flag_one_solid = (mat_map[elem_se_front_ms] + mat_map[elem_ne_front_ms] + mat_map[elem_nw_front_ms] +
                          mat_map[elem_sw_front_ms] + mat_map[elem_se_back_ms] + mat_map[elem_ne_back_ms] +
                          mat_map[elem_nw_back_ms] + mat_map[elem_sw_back_ms]) > 0
        flag_only_fluid = (mat_map[elem_se_front_ms] + mat_map[elem_ne_front_ms] + mat_map[elem_nw_front_ms] +
                           mat_map[elem_sw_front_ms] + mat_map[elem_se_back_ms] + mat_map[elem_ne_back_ms] +
                           mat_map[elem_nw_back_ms] + mat_map[elem_sw_back_ms]) == 0
        del reps, elem_ne_back_ms, elem_nw_back_ms, elem_sw_back_ms, elem_se_front_ms, elem_ne_front_ms, elem_nw_front_ms, elem_sw_front_ms

        vel_nodes_n = np.cumsum(flag_only_fluid * (1 - flag_lastrows) * (1 - flag_lastcols) * (1 - flag_lastslices))
        dof_map = flag_only_fluid * vel_nodes_n + flag_one_fluid * flag_one_solid * \
                  (nodes_n + np.cumsum(flag_one_solid * flag_one_fluid * (1 - flag_lastrows) * (1 - flag_lastcols) * (1 - flag_lastslices)))

        # Application of PBC
        top_nodes = flag_lastrows * (ns - self.len_y) + (1 - flag_lastrows) - 1
        left_nodes = flag_lastcols * (ns - self.len_x * (self.len_y + 1)) + (1 - flag_lastcols) - 1
        front_nodes = flag_lastslices * (ns - (slices - 1) * slice_n) + (1 - flag_lastslices) - 1

        # Vector of Fluid Elements
        fluid_el_n = np.cumsum((mat_map[elem_se_back_ms] == 0) * (1 - flag_lastrows) * (1 - flag_lastcols) * (1 - flag_lastslices))
        v_fluid_tmp = np.zeros(nodes_n, dtype=int)
        np.add.at(v_fluid_tmp, fluid_el_n * (mat_map[elem_se_back_ms] == 0) + (mat_map[elem_se_back_ms] != 0) - 1,
                  ((mat_map[elem_se_back_ms] == 0) * el_se_back) * (1 - flag_lastrows) * (1 - flag_lastcols) * (1 - flag_lastslices))
        for nms in range(nodes_n):
            dof_map[nms] += (1 - flag_lastslices[nms]) * (flag_lastrows[nms] * (-dof_map[nms] + dof_map[top_nodes[nms]]) + flag_lastcols[nms] * (-dof_map[nms] + dof_map[left_nodes[nms]]) +
                                                          flag_lastrows[nms] * flag_lastcols[nms] * (+dof_map[nms] - dof_map[left_nodes[nms]])) + flag_lastslices[nms] * (-dof_map[nms] + dof_map[front_nodes[nms]])
        del top_nodes, left_nodes, front_nodes, slices, mat_map, flag_one_fluid, flag_one_solid, flag_only_fluid

        # Correction of the numbering of interface nodes and reduction of the vector of fluid elements
        nms = ns - 1
        dof_map[nms] += (dof_map[nms] > vel_nodes_n[-1]) * (-nodes_n + vel_nodes_n[-1])
        v_fluid = np.zeros(fluid_el_n[-1], dtype=int)
        np.add.at(v_fluid, (ns <= fluid_el_n[-1]) * ns + (ns > fluid_el_n[-1]) - 1, v_fluid_tmp[nms] * (ns <= fluid_el_n[-1]))

        v_nodes_n = vel_nodes_n[-1]
        del vel_nodes_n, nms, ns, fluid_el_n, v_fluid_tmp

        # dofs variable tranforms into dofs, but this is n
        dofs = np.zeros((8, v_fluid.shape[0]), dtype=np.int64)
        dofs[0] = ((v_fluid - 1) % (self.nel) + (((v_fluid - 1) % self.nel) // self.len_y) +
                   ((v_fluid - 1) // self.nel) * (self.len_x + 1) * (self.len_y + 1)) + 2
        dofs[1] = dofs[0] + self.len_y + 1
        dofs[2] = dofs[1] - 1
        dofs[3] = dofs[2] - (self.len_y + 1)
        dofs[4] = dofs[3] + (self.len_x + 1) * (self.len_y + 1) + 1
        dofs[5] = dofs[4] + self.len_y + 1
        dofs[6] = dofs[5] - 1
        dofs[7] = dofs[6] - (self.len_y + 1)

        dofs[:] = dof_map[dofs - 1]
        el_dof_p = 3 * v_nodes_n + dofs - 1

        tiles = np.tile(np.arange(1, 4), (v_fluid.shape[0], 1))
        el_dof_v = np.zeros((24, v_fluid.shape[0]), dtype=np.int64)
        el_dof_v[:3] = np.swapaxes(np.expand_dims(v_nodes_n >= dofs[0], axis=1) *
                                   (np.expand_dims(dofs[0], axis=1) * 3 - 4 + tiles), 1, 0)
        el_dof_v[3:6] = np.swapaxes(np.expand_dims(v_nodes_n >= dofs[1], axis=1) *
                                    (np.expand_dims(dofs[1], axis=1) * 3 - 4 + tiles), 1, 0)
        el_dof_v[6:9] = np.swapaxes(np.expand_dims(v_nodes_n >= dofs[2], axis=1) *
                                    (np.expand_dims(dofs[2], axis=1) * 3 - 4 + tiles), 1, 0)
        el_dof_v[9:12] = np.swapaxes(np.expand_dims(v_nodes_n >= dofs[3], axis=1) *
                                     (np.expand_dims(dofs[3], axis=1) * 3 - 4 + tiles), 1, 0)
        el_dof_v[12:15] = np.swapaxes(np.expand_dims(v_nodes_n >= dofs[4], axis=1) *
                                      (np.expand_dims(dofs[4], axis=1) * 3 - 4 + tiles), 1, 0)
        el_dof_v[15:18] = np.swapaxes(np.expand_dims(v_nodes_n >= dofs[5], axis=1) *
                                      (np.expand_dims(dofs[5], axis=1) * 3 - 4 + tiles), 1, 0)
        el_dof_v[18:21] = np.swapaxes(np.expand_dims(v_nodes_n >= dofs[6], axis=1) *
                                      (np.expand_dims(dofs[6], axis=1) * 3 - 4 + tiles), 1, 0)
        el_dof_v[21:] = np.swapaxes(np.expand_dims(v_nodes_n >= dofs[7], axis=1) *
                                    (np.expand_dims(dofs[7], axis=1) * 3 - 4 + tiles), 1, 0)

        only_fluid = np.repeat(v_nodes_n >= dofs, 3, axis=0).astype(bool)
        return el_dof_v.astype(np.uint32), el_dof_p.astype(np.uint32), only_fluid

    def solve(self):
        self.x_full = np.zeros((4 * self.nels, 3), dtype=float)
        # solves the system in the 3 directions directly
        if self.solver_type == 'direct':
            self.bvec = self.bvec_full
            super().solve()
            self.x_full[self.solveF] = self.x.toarray()
        else:  # solves one direction at a time
            self.del_matrices = False
            directions = ['y', 'x', 'z']
            for d in self.direction_dict[self.direction]:
                print(f"Running {directions[d]} direction")
                self.bvec = self.bvec_full[:, d]
                super().solve()
                self.x_full[self.solveF, d] = self.x
            del self.Amat, self.bvec, self.initial_guess
        del self.bvec_full, self.solveF

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
        self.keff[1, 0] = - np.sum(self.x_full[vIdNosP * 3 - 3, 1])
        self.keff[2, 0] = - np.sum(self.x_full[vIdNosP * 3 - 1, 1])
        self.keff[0, 1] = - np.sum(self.x_full[vIdNosP * 3 - 2, 0])
        self.keff[1, 1] =   np.sum(self.x_full[vIdNosP * 3 - 3, 0])
        self.keff[2, 1] =   np.sum(self.x_full[vIdNosP * 3 - 1, 0])
        self.keff[0, 2] = - np.sum(self.x_full[vIdNosP * 3 - 2, 2])
        self.keff[1, 2] =   np.sum(self.x_full[vIdNosP * 3 - 3, 2])
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

                    self.k += weight * B.T @ C @ B
                    self.g += weight * B.T @ mat111000 @ N.ravel(order='F')[::9][np.newaxis]
                    self.p += weight * stab * DNxy.T @ DNxy
                    self.f += weight * N[0][::3][:, np.newaxis]

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

        if self.direction not in self.direction_dict:
            print_warning("Direction can only be 'all', 'x', 'y', 'z', defaulting to 'all'")
            self.direction = 'all'
