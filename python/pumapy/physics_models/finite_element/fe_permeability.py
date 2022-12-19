"""
The following FE numerical method and implementation are based on the following research paper:

Pedro C. F. Lopes, Rafael S. Vianna, Victor W. Sapucaia, Federico Semeraro, Ricardo Leiderman, Andre M. B. Pereira, 2022.
Simulation Toolkit for Digital Material Characterization of Large Image-based Microstructures.
"""
from pumapy.utilities.timer import Timer
from pumapy.utilities.logger import print_warning
from pumapy.utilities.workspace import Workspace
from pumapy.physics_models.utils.linear_solvers import PropertySolver
from pumapy.material_properties.volume_fraction import compute_volume_fraction
from pumapy.utilities.generic_checks import estimate_max_memory
from scipy.sparse import coo_matrix, diags
from scipy.sparse.linalg import LinearOperator
import numpy as np


class Permeability(PropertySolver):

    def __init__(self, workspace, solid_cutoff, direction, tolerance, maxiter, solver_type, display_iter,
                 matrix_free, preconditioner, output_fields):
        allowed_solvers = ['minres', 'direct', 'cg']
        super().__init__(workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter)

        self.solid_cutoff = solid_cutoff

        self.ws = workspace.copy()
        self.ws.binarize_range(self.solid_cutoff)
        if solver_type == "direct" or matrix_free:
            self.ws.matrix = self.ws.matrix.swapaxes(1, 0)

        self.direction = direction
        self.solver_type = solver_type
        self.matrix_free = matrix_free
        self.voxlength = self.ws.voxel_length
        self.len_x, self.len_y, self.len_z = self.ws.matrix.shape
        self.nel = self.len_x * self.len_y
        self.nels = self.nel * self.len_z
        self.nnP2 = (self.len_x + 1) * (self.len_y + 1)

        # calculate_element_matrices
        self.k = np.zeros((24, 24), dtype=float)
        self.g = np.zeros((24, 8), dtype=float)
        self.p = np.zeros((8, 8), dtype=float)
        self.f = np.zeros((8, 1), dtype=float)
        self.SN = np.zeros((3, 24), dtype=float)

        # from initialize
        self.mgdlF = None
        self.nelF = None
        self.reduce = None

        # from generate_mf_inds_and_preconditioner
        self.el_dof_v, self.el_dof_p, self.only_fluid, self.v_fluid = None, None, None, None
        self.preconditioner = preconditioner

        # from solve
        self.x_full = None
        self.keff = np.zeros((3, 3))
        self.solve_time = -1
        self.output_fields = output_fields
        self.ux, self.uy, self.uz = None, None, None

    def compute(self):
        t = Timer()
        estimate_max_memory("permeability", self.ws.get_shape(), self.solver_type,
                            mf=self.matrix_free, perm_fluid_vf=self.ws.porosity((0, 0), display=False))
        self.create_element_matrices()
        self.initialize()
        self.assemble_Amatrix()
        print(f"Time to setup system: {t.elapsed()}"); t.reset()
        self.solve()
        print(f"Time to solve: {t.elapsed()}\n")
        self.solve_time = t.elapsed()

    def initialize(self):
        print("Initializing indexing matrices ... ", flush=True, end='')

        # matrix with element connectivity with Periodic boundary conditions (PBC)
        mConectP1 = np.zeros((self.nel, 4, self.len_z + 1), dtype=np.uint32)
        aux = 1
        for k in range(self.len_z):
            mIdNosP = np.reshape(np.arange(aux, aux + self.nel, dtype=np.uint32), (self.len_x, self.len_y), order='F')
            mIdNosP = np.append(mIdNosP, mIdNosP[0][np.newaxis], axis=0)  # Numbering bottom nodes
            mIdNosP = np.append(mIdNosP, mIdNosP[:, 0][:, np.newaxis], axis=1)  # Numbering right nodes
            mConectP1[:, 0, k] = np.ravel(mIdNosP[1:, :-1], order='F')
            mConectP1[:, 1, k] = np.ravel(mIdNosP[1:, 1:], order='F')
            mConectP1[:, 2, k] = np.ravel(mIdNosP[:-1, 1:], order='F')
            mConectP1[:, 3, k] = np.ravel(mIdNosP[:-1, :-1], order='F')
            aux += self.nel
        del mIdNosP
        mConectP1[:, :, -1] = mConectP1[:, :, 0]

        mConectP = np.zeros((self.nels, 8), dtype=np.uint32)
        for k in range(self.len_z):
            mConectP[self.nel * k:self.nel * (k + 1), :4] = mConectP1[:, :, k]
            mConectP[self.nel * k:self.nel * (k + 1), 4:] = mConectP1[:, :, k + 1]
        del mConectP1

        # matrix to reduce the system
        resolveF_u = np.arange(1, self.nels * 3 + 1, dtype=np.uint32)
        nosnulos = np.unique(mConectP[np.where(self.ws.matrix.ravel(order='F') > 0)[0], :8])
        gdlnulos = np.hstack((nosnulos * 3 - 2, nosnulos * 3 - 1, nosnulos * 3))
        resolveF_u = np.delete(resolveF_u, gdlnulos - 1)
        resolveF_p = self.nels * 3 + np.unique(mConectP[np.where(self.ws.matrix.ravel(order='F')==0)[0].astype(np.uint32), :8])
        del nosnulos, gdlnulos
        self.reduce = np.hstack((resolveF_u, resolveF_p)) - 1
        del resolveF_u, resolveF_p

        # degrees of freedom matrix
        velF = np.where(self.ws.matrix.ravel(order='F') == 0)[0].astype(np.uint32)  # only fluid elements
        self.nelF = velF.shape[0]
        self.mgdlF = np.zeros((32, self.nelF), dtype=np.uint32)
        self.mgdlF[np.arange(0, 24, 3)] = mConectP[velF, np.arange(8)[:, np.newaxis]] * 3 - 3
        self.mgdlF[np.arange(1, 24, 3)] = mConectP[velF, np.arange(8)[:, np.newaxis]] * 3 - 2
        self.mgdlF[np.arange(2, 24, 3)] = mConectP[velF, np.arange(8)[:, np.newaxis]] * 3 - 1
        self.mgdlF[24:] = 3 * self.nels + np.swapaxes(mConectP[velF], 1, 0) - 1
        del mConectP, velF

        # creates indexing for matrix-free and M preconditioner
        self.generate_mf_inds_and_preconditioner()
        print("Done")

    def assemble_Amatrix(self):
        print("Creating A matrix ... ", flush=True, end='')
        if not self.matrix_free or self.solver_type == 'direct':
            if self.solver_type == 'direct':
                del self.el_dof_v, self.el_dof_p, self.only_fluid, self.v_fluid
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
            k_f = self.k.ravel()
            g_f = self.g.ravel()
            p_f = self.p.ravel()
            coeff = np.hstack((np.tile(k_f, self.nelF), np.tile(g_f, self.nelF),
                               np.tile(g_f, self.nelF), -np.tile(p_f, self.nelF)))

            self.Amat = coo_matrix((coeff, (iA, jA))).tocsc()
            del coeff, iA, jA

            # Reducing system of equations
            self.Amat = self.Amat[self.reduce][:, self.reduce]

        else:  # matrix-free mat-vec function
            y = np.zeros(self.reduce.shape[0], dtype=float)
            el_dof_v, el_dof_p, only_fluid = self.el_dof_v, self.el_dof_p, self.only_fluid

            def matvec(x):
                y.fill(0)
                tmp = np.einsum('ij, jk, jk -> ik', self.k, x[el_dof_v], only_fluid) * only_fluid
                tmp += np.einsum('ij, jk -> ik', self.g, x[el_dof_p]) * only_fluid
                np.add.at(y, el_dof_v, tmp)
                tmp = np.einsum('ji, jk, jk -> ik', self.g, x[el_dof_v], only_fluid)
                tmp -= np.einsum('ij, jk -> ik', self.p, x[el_dof_p])
                np.add.at(y, el_dof_p, tmp)
                return y
            self.Amat = LinearOperator(shape=(self.reduce.shape[0], self.reduce.shape[0]), matvec=matvec)
        print("Done")

    def assemble_bvector(self, direction):
        print("Creating b vector ... ", flush=True, end='')
        if direction == 'd':  # direct solver
            iF = np.hstack((np.reshape(self.mgdlF[1:24:3], self.nelF * 8, order='F'),
                            np.reshape(self.mgdlF[:24:3], self.nelF * 8, order='F'),
                            np.reshape(self.mgdlF[2:24:3], self.nelF * 8, order='F')))
            jF = np.hstack((np.zeros(self.nelF * 8, dtype=np.uint8),
                            np.ones(self.nelF * 8, dtype=np.uint8),
                            np.full(self.nelF * 8, 2, dtype=np.uint8)))
            sF = np.squeeze(np.tile(self.f, (self.nelF * 3, 1)))
            shape = (4 * self.nels, 3)
        else:
            if direction == 'x':
                iF = np.reshape(self.mgdlF[1:24:3], self.nelF * 8, order='F')
            elif direction == 'y':
                iF = np.reshape(self.mgdlF[:24:3], self.nelF * 8, order='F')
            else:
                iF = np.reshape(self.mgdlF[2:24:3], self.nelF * 8, order='F')
            jF = np.zeros(self.nelF * 8, dtype=np.uint8)
            sF = np.squeeze(np.tile(self.f, (self.nelF, 1)))
            shape = (4 * self.nels, 1)

        self.bvec = coo_matrix((sF, (iF, jF)), shape=shape).tocsc()[self.reduce]
        print("Done")

    def solve(self):
        if self.solver_type != "direct":
            self.x_full = np.zeros(4 * self.nels, dtype=float)
            self.del_matrices = False
        else:
            self.x_full = np.zeros((4 * self.nels, 3), dtype=float)

        for j, d in enumerate(self.direction):
            if self.solver_type != "direct":
                print(f"Running {d} direction")
            self.assemble_bvector(d)
            super().solve()
            self.x_full[self.reduce] = self.x
            self.compute_effective_coefficient(d)

        self.keff = (self.keff * self.voxlength ** 2) / self.nels
        print(f'\nEffective permeability tensor: \n{self.keff}')

    def compute_effective_coefficient(self, d):
        inds = np.arange(1, self.nels + 1)

        if d == "d":  # if direct solver, then x_full contains the 3 solutions
            self.keff[0, 0] = np.sum(self.x_full[inds * 3 - 3, 1])
            self.keff[1, 0] = np.sum(self.x_full[inds * 3 - 2, 1])
            self.keff[2, 0] = np.sum(self.x_full[inds * 3 - 1, 1])
            self.keff[0, 1] = np.sum(self.x_full[inds * 3 - 3, 0])
            self.keff[1, 1] = np.sum(self.x_full[inds * 3 - 2, 0])
            self.keff[2, 1] = np.sum(self.x_full[inds * 3 - 1, 0])
            self.keff[0, 2] = np.sum(self.x_full[inds * 3 - 3, 2])
            self.keff[1, 2] = np.sum(self.x_full[inds * 3 - 2, 2])
            self.keff[2, 2] = np.sum(self.x_full[inds * 3 - 1, 2])
            if self.output_fields:
                self.ux = np.zeros((self.len_x, self.len_y, self.len_z, 3))
                self.uy = np.zeros((self.len_x, self.len_y, self.len_z, 3))
                self.uz = np.zeros((self.len_x, self.len_y, self.len_z, 3))
                self.ux[:, :, :, 0] =   np.reshape(self.x_full[inds * 3 - 3, 1], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                self.ux[:, :, :, 1] = - np.reshape(self.x_full[inds * 3 - 2, 1], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                self.ux[:, :, :, 2] = - np.reshape(self.x_full[inds * 3 - 1, 1], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                self.uy[:, :, :, 0] = - np.reshape(self.x_full[inds * 3 - 3, 0], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                self.uy[:, :, :, 1] =   np.reshape(self.x_full[inds * 3 - 2, 0], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                self.uy[:, :, :, 2] =   np.reshape(self.x_full[inds * 3 - 1, 0], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                self.uz[:, :, :, 0] = - np.reshape(self.x_full[inds * 3 - 3, 2], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                self.uz[:, :, :, 1] =   np.reshape(self.x_full[inds * 3 - 2, 2], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                self.uz[:, :, :, 2] =   np.reshape(self.x_full[inds * 3 - 1, 2], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                self.ux = self.ux.swapaxes(0, 1)
                self.uy = self.uy.swapaxes(0, 1)
                self.uz = self.uz.swapaxes(0, 1)
        else:
            if d == "x":
                self.keff[0, 0] =   np.sum(self.x_full[inds * 3 - 2])
                self.keff[1, 0] =   np.sum(self.x_full[inds * 3 - 3])
                self.keff[2, 0] = - np.sum(self.x_full[inds * 3 - 1])
            elif d == "y":
                self.keff[0, 1] =   np.sum(self.x_full[inds * 3 - 2])
                self.keff[1, 1] =   np.sum(self.x_full[inds * 3 - 3])
                self.keff[2, 1] = - np.sum(self.x_full[inds * 3 - 1])
            else:
                self.keff[0, 2] = - np.sum(self.x_full[inds * 3 - 2])
                self.keff[1, 2] = - np.sum(self.x_full[inds * 3 - 3])
                self.keff[2, 2] =   np.sum(self.x_full[inds * 3 - 1])

            if self.output_fields:
                if d == "x":
                    if not self.matrix_free:
                        self.ux = np.zeros((self.len_x, self.len_y, self.len_z, 3))
                        self.ux[:, :, :, 0] =   np.reshape(self.x_full[inds * 3 - 2], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                        self.ux[:, :, :, 1] = - np.reshape(self.x_full[inds * 3 - 3], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                        self.ux[:, :, :, 2] = - np.reshape(self.x_full[inds * 3 - 1], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                    else:
                        self.ux = self.reconstruct_velocity()
                        self.ux[:, :, :, [1, 2]] = - self.ux[:, :, :, [1, 2]]
                elif d == "y":
                    if not self.matrix_free:
                        self.uy = np.zeros((self.len_x, self.len_y, self.len_z, 3))
                        self.uy[:, :, :, 0] = - np.reshape(self.x_full[inds * 3 - 2], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                        self.uy[:, :, :, 1] =   np.reshape(self.x_full[inds * 3 - 3], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                        self.uy[:, :, :, 2] =   np.reshape(self.x_full[inds * 3 - 1], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                    else:
                        self.uy = self.reconstruct_velocity()
                        self.uy[:, :, :, 0] = - self.uy[:, :, :, 0]
                else:
                    if not self.matrix_free:
                        self.uz = np.zeros((self.len_x, self.len_y, self.len_z, 3))
                        self.uz[:, :, :, 0] = - np.reshape(self.x_full[inds * 3 - 2], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                        self.uz[:, :, :, 1] =   np.reshape(self.x_full[inds * 3 - 3], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                        self.uz[:, :, :, 2] =   np.reshape(self.x_full[inds * 3 - 1], (self.len_x, self.len_y, self.len_z), order='F') * self.voxlength **2
                    else:
                        self.uz = self.reconstruct_velocity()
                        self.uz[:, :, :, 0] = - self.uz[:, :, :, 0]

    def reconstruct_velocity(self):
        u = np.sum(self.SN[np.arange(3), 3 * np.arange(8)[:, np.newaxis] + np.arange(3), np.newaxis] * \
                   self.x[self.el_dof_v[3 * np.arange(8)[:, np.newaxis] + np.arange(3)]] * \
                   self.only_fluid[3 * np.arange(8)[:, np.newaxis] + np.arange(3)], axis=0)
        u_full = np.zeros((3, self.nels), dtype=float)
        u_full[:, self.v_fluid - 1] = u
        u_full = np.stack((np.reshape(u_full[1], (self.len_y, self.len_x, self.len_z), order='F'),
                           np.reshape(u_full[0], (self.len_y, self.len_x, self.len_z), order='F'),
                           np.reshape(u_full[2], (self.len_y, self.len_x, self.len_z), order='F')), axis=3)
        return u_full * self.voxlength **2

    def generate_mf_inds_and_preconditioner(self):

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
        mat_map = self.ws.matrix.swapaxes(1, 0).ravel(order='F') > 0  # NB swapaxes

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
            dof_map[nms] += ((1 - flag_lastslices[nms]) * (flag_lastrows[nms] * (-dof_map[nms] + dof_map[top_nodes[nms]]) +
                                                           flag_lastcols[nms] * (-dof_map[nms] + dof_map[left_nodes[nms]]) +
                                                           flag_lastrows[nms] * flag_lastcols[nms] * (dof_map[nms] - dof_map[left_nodes[nms]])) +
                             flag_lastslices[nms] * (-dof_map[nms] + dof_map[front_nodes[nms]]))
        del top_nodes, left_nodes, front_nodes, slices, mat_map, flag_one_fluid, flag_one_solid, flag_only_fluid

        # Correction of the numbering of interface nodes and reduction of the vector of fluid elements
        nms = ns - 1
        dof_map[nms] += (dof_map[nms] > vel_nodes_n[-1]) * (vel_nodes_n[-1] - nodes_n)
        self.v_fluid = np.zeros(fluid_el_n[-1], dtype=int)
        np.add.at(self.v_fluid, (ns <= fluid_el_n[-1]) * ns + (ns > fluid_el_n[-1]) - 1, v_fluid_tmp[nms] * (ns <= fluid_el_n[-1]))

        v_nodes_n = vel_nodes_n[-1]
        del vel_nodes_n, nms, ns, fluid_el_n, v_fluid_tmp

        # dofs variable tranforms into dofs, but this is n
        dofs = np.zeros((8, self.v_fluid.shape[0]), dtype=np.int64)
        dofs[0] = ((self.v_fluid - 1) % self.nel + (((self.v_fluid - 1) % self.nel) // self.len_y) +
                   ((self.v_fluid - 1) // self.nel) * (self.len_x + 1) * (self.len_y + 1)) + 2
        dofs[1] = dofs[0] + self.len_y + 1
        dofs[2] = dofs[1] - 1
        dofs[3] = dofs[2] - (self.len_y + 1)
        dofs[4] = dofs[3] + (self.len_x + 1) * (self.len_y + 1) + 1
        dofs[5] = dofs[4] + self.len_y + 1
        dofs[6] = dofs[5] - 1
        dofs[7] = dofs[6] - (self.len_y + 1)

        dofs = dof_map[dofs - 1]

        self.el_dof_p = (3 * v_nodes_n + dofs - 1).astype(np.uint32)
        self.el_dof_v = np.swapaxes(np.expand_dims(v_nodes_n >= dofs[np.arange(8)], axis=2) *
                                    (np.expand_dims(dofs[np.arange(8)], axis=2) * 3 - 4 +
                                     np.tile(np.arange(1, 4), (self.v_fluid.shape[0], 1))), 2, 1)
        self.el_dof_v = self.el_dof_v.reshape(24, self.v_fluid.shape[0]).astype(np.uint32)
        self.only_fluid = np.repeat(v_nodes_n >= dofs, 3, axis=0).astype(bool)

        if self.preconditioner:
            self.M = np.zeros(self.reduce.shape[0])
            inds = np.arange(8)
            np.add.at(self.M, 3 * v_nodes_n + dofs[inds] - 1, -np.expand_dims(self.p[inds, inds], axis=1))
            np.add.at(self.M, self.el_dof_v, np.einsum('i, jk -> jik', self.k[inds[:3], inds[:3]],
                                                       v_nodes_n >= dofs[np.arange(8)]).reshape(24, self.v_fluid.shape[0]))
            self.M = diags(1./self.M, 0).tocsr()

    def create_element_matrices(self):
        delta = 1.
        coordsElem = np.array([[0, 0, 0], [delta, 0, 0], [delta, delta, 0],
                               [0, delta, 0], [0, 0, delta], [delta, 0, delta],
                               [delta, delta, delta], [0, delta, delta]])
        rr = np.array([-1. / np.sqrt(3), 1. / np.sqrt(3)])
        ss = rr.copy()
        tt = rr.copy()
        C = np.diag([2., 2., 2., 1., 1., 1.])
        h2 = 3.
        stab = h2 / 18. / 1.
        mat111000 = np.array([[1.], [1.], [1.], [0.], [0.], [0.]])
        N = np.zeros((3, 24), dtype=float)
        for i in range(2):
            r = rr[i]
            for j in range(2):
                s = ss[j]
                for k in range(2):
                    t = tt[k]
                    N[0,  0] = 0.125 * (1 - r) * (1 - s) * (1 - t)
                    N[0,  3] = 0.125 * (1 + r) * (1 - s) * (1 - t)
                    N[0,  6] = 0.125 * (1 + r) * (1 + s) * (1 - t)
                    N[0,  9] = 0.125 * (1 - r) * (1 + s) * (1 - t)
                    N[0, 12] = 0.125 * (1 - r) * (1 - s) * (1 + t)
                    N[0, 15] = 0.125 * (1 + r) * (1 - s) * (1 + t)
                    N[0, 18] = 0.125 * (1 + r) * (1 + s) * (1 + t)
                    N[0, 21] = 0.125 * (1 - r) * (1 + s) * (1 + t)
                    N[1,  1] = N[0,  0]
                    N[1,  4] = N[0,  3]
                    N[1,  7] = N[0,  6]
                    N[1, 10] = N[0,  9]
                    N[1, 13] = N[0, 12]
                    N[1, 16] = N[0, 15]
                    N[1, 19] = N[0, 18]
                    N[1, 22] = N[0, 21]
                    N[2,  2] = N[0,  0]
                    N[2,  5] = N[0,  3]
                    N[2,  8] = N[0,  6]
                    N[2, 11] = N[0,  9]
                    N[2, 14] = N[0, 12]
                    N[2, 17] = N[0, 15]
                    N[2, 20] = N[0, 18]
                    N[2, 23] = N[0, 21]
                    dN1dr = - 0.125 * (1 - s) * (1 - t)
                    dN2dr = + 0.125 * (1 - s) * (1 - t)
                    dN3dr = + 0.125 * (1 + s) * (1 - t)
                    dN4dr = - 0.125 * (1 + s) * (1 - t)
                    dN5dr = - 0.125 * (1 - s) * (1 + t)
                    dN6dr = + 0.125 * (1 - s) * (1 + t)
                    dN7dr = + 0.125 * (1 + s) * (1 + t)
                    dN8dr = - 0.125 * (1 + s) * (1 + t)
                    dN1ds = - 0.125 * (1 - r) * (1 - t)
                    dN2ds = - 0.125 * (1 + r) * (1 - t)
                    dN3ds = + 0.125 * (1 + r) * (1 - t)
                    dN4ds = + 0.125 * (1 - r) * (1 - t)
                    dN5ds = - 0.125 * (1 - r) * (1 + t)
                    dN6ds = - 0.125 * (1 + r) * (1 + t)
                    dN7ds = + 0.125 * (1 + r) * (1 + t)
                    dN8ds = + 0.125 * (1 - r) * (1 + t)
                    dN1dt = - 0.125 * (1 - r) * (1 - s)
                    dN2dt = - 0.125 * (1 + r) * (1 - s)
                    dN3dt = - 0.125 * (1 + r) * (1 + s)
                    dN4dt = - 0.125 * (1 - r) * (1 + s)
                    dN5dt = + 0.125 * (1 - r) * (1 - s)
                    dN6dt = + 0.125 * (1 + r) * (1 - s)
                    dN7dt = + 0.125 * (1 + r) * (1 + s)
                    dN8dt = + 0.125 * (1 - r) * (1 + s)
                    DN = [[dN1dr, dN2dr, dN3dr, dN4dr, dN5dr, dN6dr, dN7dr, dN8dr],
                          [dN1ds, dN2ds, dN3ds, dN4ds, dN5ds, dN6ds, dN7ds, dN8ds],
                          [dN1dt, dN2dt, dN3dt, dN4dt, dN5dt, dN6dt, dN7dt, dN8dt]]
                    J = DN @ coordsElem
                    detJ = np.linalg.det(J)
                    invJ = np.linalg.inv(J)
                    DNxy = invJ @ DN
                    B = np.array([[DNxy[0, 0], 0, 0, DNxy[0, 1], 0, 0, DNxy[0, 2], 0, 0, DNxy[0, 3], 0, 0, DNxy[0, 4], 0, 0, DNxy[0, 5], 0, 0, DNxy[0, 6], 0, 0, DNxy[0, 7], 0, 0],
                                  [0, DNxy[1, 0], 0, 0, DNxy[1, 1], 0, 0, DNxy[1, 2], 0, 0, DNxy[1, 3], 0, 0, DNxy[1, 4], 0, 0, DNxy[1, 5], 0, 0, DNxy[1, 6], 0, 0, DNxy[1, 7], 0],
                                  [0, 0, DNxy[2, 0], 0, 0, DNxy[2, 1], 0, 0, DNxy[2, 2], 0, 0, DNxy[2, 3], 0, 0, DNxy[2, 4], 0, 0, DNxy[2, 5], 0, 0, DNxy[2, 6], 0, 0, DNxy[2, 7]],
                                  [DNxy[1, 0], DNxy[0, 0], 0, DNxy[1, 1], DNxy[0, 1], 0, DNxy[1, 2], DNxy[0, 2], 0, DNxy[1, 3], DNxy[0, 3], 0, DNxy[1, 4], DNxy[0, 4], 0, DNxy[1, 5], DNxy[0, 5], 0, DNxy[1, 6], DNxy[0, 6], 0, DNxy[1, 7], DNxy[0, 7], 0],
                                  [DNxy[2, 0], 0, DNxy[0, 0], DNxy[2, 1], 0, DNxy[0, 1], DNxy[2, 2], 0, DNxy[0, 2], DNxy[2, 3], 0, DNxy[0, 3], DNxy[2, 4], 0, DNxy[0, 4], DNxy[2, 5], 0, DNxy[0, 5], DNxy[2, 6], 0, DNxy[0, 6], DNxy[2, 7], 0, DNxy[0, 7]],
                                  [0, DNxy[2, 0], DNxy[1, 0], 0, DNxy[2, 1], DNxy[1, 1], 0, DNxy[2, 2], DNxy[1, 2], 0, DNxy[2, 3], DNxy[1, 3], 0, DNxy[2, 4], DNxy[1, 4], 0, DNxy[2, 5], DNxy[1, 5], 0, DNxy[2, 6], DNxy[1, 6], 0, DNxy[2, 7], DNxy[1, 7]]])
                    self.k  += (B.T @ C @ B) * detJ
                    self.g  += (B.T @ mat111000 @ np.array([N[0,0], N[0,3], N[0,6], N[0,9], N[0,12], N[0,15], N[0,18], N[0,21]])[np.newaxis]) * detJ
                    self.p  += stab * (DNxy.T @ DNxy) * detJ
                    self.f  += np.array([N[0,0], N[0,3], N[0,6], N[0,9], N[0,12], N[0,15], N[0,18], N[0,21]])[np.newaxis].T * detJ
                    self.SN += N * detJ

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

        if self.direction not in ['xyz', 'x', 'y', 'z']:
            print_warning("Direction can only be 'xyz', 'x', 'y', 'z', defaulting to 'xyz'")
            self.direction = 'xyz'

        if self.solver_type == 'minres' and self.preconditioner:
            print_warning("Cannot have the minres solver together with the Jacobi preconditioner "
                          "(it leads to an asymmetric matrix), defaulting to cg")
            self.solver_type = "cg"

        if self.solver_type == 'direct':
            self.direction = 'd'

        if self.tolerance > 1e-7 and self.solver_type == 'minres':
            print_warning("The minres permeability solver requires a lower tolerance value than other PuMA solvers. A tolerance value of 1e-7 or lower is recommended")

        if compute_volume_fraction(self.ws, (1, 1), display=False) == 1:
            raise Exception("Entire domain is solid - double check the provided cutoff")

        if compute_volume_fraction(self.ws, (0, 0), display=False) == 0:
            raise Exception("Entire domain is empty - double check the provided cutoff")

        if not (isinstance(self.solid_cutoff, tuple) and len(self.solid_cutoff) == 2 and self.solid_cutoff[0] <= self.solid_cutoff[1]):
            raise Exception("solid_cutoff must be a tuple(int, int) indicating the solid ID range in the workspace.")
