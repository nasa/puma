from pumapy.utilities.timer import Timer
from pumapy.physics_models.utils.linear_solvers import PropertySolver
from pumapy.physics_models.utils.boundary_conditions import IsotropicConductivityBC
from pumapy.utilities.workspace import Workspace
from pumapy.physics_models.finite_volume.isotropic_conductivity_utils import setup_matrices_cy, compute_flux
from pumapy.utilities.generic_checks import estimate_max_memory
from pumapy.utilities.logger import print_warning
from scipy.sparse import coo_matrix, diags
from scipy.sparse.linalg import LinearOperator
import numpy as np


class IsotropicConductivity(PropertySolver):

    def __init__(self, workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter, solver_type,
                 display_iter, matrix_free):
        allowed_solvers = ['direct', 'gmres', 'bicgstab', 'cg']
        super().__init__(workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter)
        self._bc_func = None
        self.cond = np.zeros([1, 1, 1])

        self.matrix_free = matrix_free
        self.cond_map = cond_map
        self.direction = direction
        self.side_bc = side_bc
        self.prescribed_bc = prescribed_bc

        if self.direction == 'x' or self.direction == '':
            self._matrix = workspace.matrix
        elif self.direction == 'y':
            self._matrix = workspace.matrix.transpose(1, 0, 2)
        elif self.direction == 'z':
            self._matrix = workspace.matrix.transpose(2, 1, 0)

        self.len_x, self.len_y, self.len_z = self._matrix.shape
        self.len_xy = self.len_x * self.len_y
        self.len_xyz = self.len_xy * self.len_z
        self.bc_check = 0
        self.domain_bc_check = 1
        self.side_bc = side_bc

        self.kf_i = None
        self.kf_inonzero = None
        self.kf_ixm = None
        self.kf_ixp = None
        self.kf_iym = None
        self.kf_iyp = None
        self.kf_izm = None
        self.kf_izp = None

        self.n_iter = 0
        self.Amat = None
        self.M = None
        self.bvec = np.zeros(1)
        self.initial_guess = np.zeros(1)

        self.keff = [-1., -1., -1.]
        self.solve_time = -1
        self.T = np.zeros([1, 1, 1])
        self.q = np.zeros([1, 1, 1, 3])

    def compute(self):
        t = Timer()
        estimate_max_memory("isotropic_conductivity", self.ws.get_shape(), self.solver_type)
        self.initialize()
        self.assemble_bvector()  # needs to be first because of prescribed_bc
        self.assemble_Amatrix()
        print("Time to assemble matrices: ", t.elapsed()); t.reset()
        super().solve()
        print("Time to solve: ", t.elapsed())
        self.compute_effective_coefficient()
        self.solve_time = t.elapsed()

    def initialize(self):
        print("Creating conductivity matrix ... ", end='')
        self.cond = np.zeros(self._matrix.shape, dtype=float)
        for i in range(self.cond_map.get_size()):
            low, high, k = self.cond_map.get_material(i)
            mask_low = self._matrix >= low
            mask_high = self._matrix <= high
            mask_low = mask_low * mask_high
            self.cond[mask_low] = k
        print("Done")

        print("Initializing temperature field ... ", end='')
        if self.solver_type != "direct":
            self.initial_guess = np.ones([self.len_x, self.len_y, self.len_z])
            if self.direction != "":
                for i in range(self.len_x):
                    self.initial_guess[i, :, :] = i / (self.len_x - 1.)
            self.initial_guess = self.initial_guess.flatten('F')
        print("Done")

    def assemble_bvector(self):
        print("Setting up b matrix ... ", end='')
        bsq = np.zeros([self.len_x, self.len_y, self.len_z])

        if self.direction == '':
            for i in range(self.len_x):
                for j in range(self.len_y):
                    for k in range(self.len_z):
                        if self.prescribed_bc.dirichlet[i, j, k] != np.Inf:
                            bsq[i, j, k] = self.prescribed_bc.dirichlet[i, j, k]

            self.prescribed_bc = self.prescribed_bc.dirichlet  # because of cython, cannot pass object
            self.bc_check = 1
            self.domain_bc_check = 0

        else:
            self.bc_check = 0
            self.domain_bc_check = 1
            self.prescribed_bc = np.full(self._matrix.shape, np.Inf, dtype=float)
            bsq[-1, :, :] = 1

        # check for zero conductivity
        for i in range(self.cond_map.get_size()):
            low, high, k = self.cond_map.get_material(i)
            if k == 0:
                self.bc_check = 1
                self.prescribed_bc[(self._matrix >= low) * (self._matrix <= high)] = 0

        self.bvec = bsq.flatten('F')
        print("Done")

    def setup_cond_matrices(self):

        [X,Y,Z] = self.cond.shape

        self.kf_i = np.copy(self.cond)

        if self.side_bc == 'p':
            roll_xm = np.roll(self.cond, 1, axis=0)
            roll_xp = np.roll(self.cond, -1, axis=0)
            roll_ym = np.roll(self.cond, 1, axis=1)
            roll_yp = np.roll(self.cond, -1, axis=1)
            roll_zm = np.roll(self.cond, 1, axis=2)
            roll_zp = np.roll(self.cond, -1, axis=2)
        else:
            roll_xm = np.roll(self.cond, 1, axis=0)
            roll_xm[0, :, :] = roll_xm[1, :, :]
            roll_xp = np.roll(self.cond, -1, axis=0)
            roll_xp[-1, :, :] = roll_xp[-2, :, :]
            roll_ym = np.roll(self.cond, 1, axis=1)
            roll_ym[:, 0, :] = roll_ym[:, 1, :]
            roll_yp = np.roll(self.cond, -1, axis=1)
            roll_yp[:, -1, :] = roll_yp[:, -2, :]
            roll_zm = np.roll(self.cond, 1, axis=2)
            roll_zm[:, :, 0] = roll_zm[:, :, 1]
            roll_zp = np.roll(self.cond, -1, axis=2)
            roll_zp[:, :, -1] = roll_zp[:, :, -2]

        self.kf_i = (- roll_xm * self.cond / (roll_xm + self.cond) - roll_xp * self.cond / (roll_xp + self.cond)
                     - roll_ym * self.cond / (roll_ym + self.cond) - roll_yp * self.cond / (roll_yp + self.cond)
                     - roll_zm * self.cond / (roll_zm + self.cond) - roll_zp * self.cond / (roll_zp + self.cond))
        if self.bc_check==1:
            self.kf_i[self.prescribed_bc != np.Inf] = 1
        if self.domain_bc_check == 1:
            self.kf_i[[0,X-1],:,:] = 1
        self.kf_i = self.kf_i.flatten('F')

        self.kf_inonzero = np.copy(self.kf_i)
        self.kf_inonzero[self.kf_i == 0] = 1
        self.kf_inonzero[:] = 1./self.kf_inonzero[:]
        self.kf_inonzero = self.kf_inonzero.flatten('F')

        self.kf_ixm = np.copy(self.cond)
        self.kf_ixm = (roll_xm * self.cond / (roll_xm + self.cond))
        if self.bc_check==1:
            self.kf_ixm[self.prescribed_bc != np.Inf] = 0
        if self.domain_bc_check == 1:
            self.kf_ixm[[0, X - 1], :, :] = 0
        self.kf_ixm = self.kf_ixm.flatten('F')

        self.kf_ixp = np.copy(self.cond)
        self.kf_ixp = (roll_xp * self.cond / (roll_xp + self.cond))
        if self.bc_check==1:
            self.kf_ixp[self.prescribed_bc != np.Inf] = 0
        if self.domain_bc_check == 1:
            self.kf_ixp[[0, X - 1], :, :] = 0
        self.kf_ixp = self.kf_ixp.flatten('F')

        self.kf_iym = np.copy(self.cond)
        self.kf_iym = (roll_ym * self.cond / (roll_ym + self.cond))
        if self.bc_check==1:
            self.kf_iym[self.prescribed_bc != np.Inf] = 0
        if self.domain_bc_check == 1:
            self.kf_iym[[0, X - 1], :, :] = 0
        self.kf_iym = self.kf_iym.flatten('F')

        self.kf_iyp = np.copy(self.cond)
        self.kf_iyp = (roll_yp * self.cond / (roll_yp + self.cond))
        if self.bc_check==1:
            self.kf_iyp[self.prescribed_bc != np.Inf] = 0
        if self.domain_bc_check == 1:
            self.kf_iyp[[0, X - 1], :, :] = 0
        self.kf_iyp = self.kf_iyp.flatten('F')

        self.kf_izm = np.copy(self.cond)
        self.kf_izm = (roll_zm * self.cond / (roll_zm + self.cond))
        if self.bc_check==1:
            self.kf_izm[self.prescribed_bc != np.Inf] = 0
        if self.domain_bc_check == 1:
            self.kf_izm[[0, X - 1], :, :] = 0
        self.kf_izm = self.kf_izm.flatten('F')

        self.kf_izp = np.copy(self.cond)
        self.kf_izp = (roll_zp * self.cond / (roll_zp + self.cond))
        if self.bc_check==1:
            self.kf_izp[self.prescribed_bc != np.Inf] = 0
        if self.domain_bc_check == 1:
            self.kf_izp[[0, X - 1], :, :] = 0
        self.kf_izp = self.kf_izp.flatten('F')

    def assemble_Amatrix(self):
        if not self.matrix_free or self.solver_type == "direct":
            self._row, self._col, self._data = setup_matrices_cy(self.cond.flatten('F'), self.len_x, self.len_y, self.len_z, self.domain_bc_check,
                                                                 self.bc_check, self.prescribed_bc, self.side_bc)
            del self.prescribed_bc
            self.Amat = coo_matrix((self._data, (self._row, self._col)), shape=(self.len_xyz, self.len_xyz)).tocsr()
            del self._data, self._row, self._col
            print("Done")

            print("Setting up preconditioner ...", end ='')
            diag = self.Amat.diagonal()
            diag[diag==0] = 1
            diag[:] = 1./diag[:]
            self.M = diags(diag, 0).tocsr()
            print("Done")
        else:
            self.setup_cond_matrices()
            del self.prescribed_bc

            def matvec(x):  # overload matvec for Amat=LinearOperator
                y = self.kf_i * x
                x = x.reshape((self.len_x, self.len_y, self.len_z), order='F')

                if self.side_bc == 'p':
                    y += self.kf_ixm * np.roll(x, 1, axis=0).flatten('F')
                    y += self.kf_ixp * np.roll(x, -1, axis=0).flatten('F')
                    y += self.kf_iym * np.roll(x, 1, axis=1).flatten('F')
                    y += self.kf_iyp * np.roll(x, -1, axis=1).flatten('F')
                    y += self.kf_izm * np.roll(x, 1, axis=2).flatten('F')
                    y += self.kf_izp * np.roll(x, -1, axis=2).flatten('F')
                else:  # symmetric bc
                    x_temp = np.roll(x, 1, axis=0)
                    x_temp[0, :, :] = x_temp[1, :, :]
                    y += self.kf_ixm * x_temp.flatten('F')

                    x_temp = np.roll(x, -1, axis=0)
                    x_temp[-1, :, :] = x_temp[-2, :, :]
                    y += self.kf_ixp * x_temp.flatten('F')

                    x_temp = np.roll(x, 1, axis=1)
                    x_temp[:, 0, :] = x_temp[:, 1, :]
                    y += self.kf_iym * x_temp.flatten('F')

                    x_temp = np.roll(x, -1, axis=1)
                    x_temp[:, -1, :] = x_temp[:, -2, :]
                    y += self.kf_iyp * x_temp.flatten('F')

                    x_temp = np.roll(x, 1, axis=2)
                    x_temp[:, :, 0] = x_temp[:, :, 1]
                    y += self.kf_izm * x_temp.flatten('F')

                    x_temp = np.roll(x, -1, axis=2)
                    x_temp[:, :, -1] = x_temp[:, :, -2]
                    y += self.kf_izp * x_temp.flatten('F')
                return y
            self.Amat = LinearOperator(shape=(self.len_xyz, self.len_xyz), matvec=matvec)

            def vecvec_prec(x):
                y = self.kf_inonzero * x
                return y
            self.M = LinearOperator(shape=(self.len_xyz, self.len_xyz), matvec=vecvec_prec)

    def compute_effective_coefficient(self):
        # reshaping solution
        self.T = self.x.reshape([self.len_x, self.len_y, self.len_z], order='F')
        del self.x

        if self.direction == 'y':
            self.T = self.T.transpose(1, 0, 2)
            self.cond = self.cond.transpose(1, 0, 2)
            self.len_x = self.T.shape[0]
            self.len_y = self.T.shape[1]
            self.len_z = self.T.shape[2]
        elif self.direction == 'z':
            self.T = self.T.transpose(2, 1, 0)
            self.cond = self.cond.transpose(2, 1, 0)
            self.len_x = self.T.shape[0]
            self.len_y = self.T.shape[1]
            self.len_z = self.T.shape[2]

        print("Computing flux from converged temperature field ... ", end='')
        flux_x, flux_y, flux_z, self.q = compute_flux(self.T, self.cond, self.len_x, self.len_y, self.len_z)
        print("Done")

        self.keff[0] = flux_x * (self.len_x - 1)
        self.keff[1] = flux_y * (self.len_y - 1)
        self.keff[2] = flux_z * (self.len_z - 1)

        # making the flux have the correct spacial units
        self.q /= self.ws.voxel_length

    def log_input(self):
        self.ws.log.log_section("Computing Conductivity using Isotropic solver")
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
        # ws checks
        if not isinstance(self.ws, Workspace):
            raise Exception("Workspace must be a puma.Workspace.")
        if self.ws.len_x() < 3 or self.ws.len_y() < 3 or self.ws.len_z() < 3:
            raise Exception("Workspace must be at least 3x3x3 for Conductivity finite volume solvers.")

        # direction and prescribed_bc checks
        if self.direction != '':
            if self.direction.lower() in ['x', 'y', 'z']:
                self.direction = self.direction.lower()
            else:
                raise Exception("Invalid simulation direction, it can only be 'x', 'y', 'z', or '' for prescribed_bc.")
            if self.prescribed_bc is not None:
                print_warning(f"{self.direction} Direction specified, prescribed_bc ignored.")
        else:
            if not isinstance(self.prescribed_bc, IsotropicConductivityBC):
                raise Exception("If prescribed_bc provided, the object needs to be a puma.AnisotropicConductivityBC object.")

        if self.tolerance > 1e-4:
            print_warning(f"{self.tolerance} tolerance value is large. 1e-4 or smaller is recommended")

        # side_bc checks
        if self.side_bc == "periodic" or self.side_bc == "Periodic" or self.side_bc == "p":
            self.side_bc = "p"
        elif self.side_bc == "symmetric" or self.side_bc == "Symmetric" or self.side_bc == "s":
            self.side_bc = "s"
        else:
            raise Exception("Invalid side boundary conditions.")
