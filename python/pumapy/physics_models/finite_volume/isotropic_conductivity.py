from pumapy.utilities.timer import Timer
from pumapy.physics_models.utils.linear_solvers import PropertySolver
from pumapy.physics_models.utils.boundary_conditions import IsotropicConductivityBC
from pumapy.utilities.workspace import Workspace
from pumapy.physics_models.finite_volume.isotropic_conductivity_utils import setup_matrices_cy, compute_flux, matvec_cy
from pumapy.utilities.generic_checks import estimate_max_memory
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

        if self.direction == 'x':
            self._matrix = workspace.matrix
        elif self.direction == 'y':
            self._matrix = workspace.matrix.transpose(1, 0, 2)
        elif self.direction == 'z':
            self._matrix = workspace.matrix.transpose(2, 1, 0)

        self.len_x, self.len_y, self.len_z = self._matrix.shape
        self.len_xy = self.len_x * self.len_y
        self.len_xyz = self.len_xy * self.len_z
        self.bc_check = 0
        self.side_bc = side_bc

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
            self.initial_guess = np.zeros([self.len_x, self.len_y, self.len_z])
            for i in range(self.len_x):
                self.initial_guess[i, :, :] = i / (self.len_x - 1.)
            self.initial_guess = self.initial_guess.flatten('F')
        print("Done")

    def assemble_bvector(self):
        print("Setting up b matrix ... ", end='')
        bsq = np.zeros([self.len_x, self.len_y, self.len_z])
        if self.prescribed_bc is not None:
            for i in range(self.len_x):
                for j in range(self.len_y):
                    for k in range(self.len_z):
                        if self.prescribed_bc.dirichlet[i, j, k] != np.Inf:
                            bsq[i, j, k] = self.prescribed_bc[i, j, k]

            self.prescribed_bc = self.prescribed_bc.dirichlet  # because of cython, cannot pass object
            self.bc_check = 1
        else:
            self.bc_check = 0
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

    def assemble_Amatrix(self):
        if not self.matrix_free or self.solver_type == "direct":
            self._row, self._col, self._data = setup_matrices_cy(self.cond.flatten('F'), self.len_x, self.len_y, self.len_z,
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
            y = np.zeros(self.len_xyz, dtype=float)
            kf = self.cond.flatten('F')
            def matvec(x):  # overload matvec for Amat=LinearOperator
                y.fill(0)
                matvec_cy(kf, x, y, self.len_x, self.len_y, self.len_z, self.bc_check, self.prescribed_bc, self.side_bc)
                return y
            self.Amat = LinearOperator(shape=(self.len_xyz, self.len_xyz), matvec=matvec)

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

        # direction checks
        if self.direction == "x" or self.direction == "X":
            self.direction = "x"
        elif self.direction == "y" or self.direction == "Y":
            self.direction = "y"
        elif self.direction == "z" or self.direction == "Z":
            self.direction = "z"
        else:
            raise Exception("Invalid simulation direction.")

        # side_bc checks
        if self.side_bc == "periodic" or self.side_bc == "Periodic" or self.side_bc == "p":
            self.side_bc = "p"
        elif self.side_bc == "symmetric" or self.side_bc == "Symmetric" or self.side_bc == "s":
            self.side_bc = "s"
        else:
            raise Exception("Invalid side boundary conditions.")

        # prescribed_bc checks
        if self.prescribed_bc is not None:
            if not isinstance(self.prescribed_bc, IsotropicConductivityBC):
                raise Exception("prescribed_bc must be a puma.ConductivityBC.")
            if self.prescribed_bc.dirichlet.shape != self.ws.matrix.shape:
                raise Exception("prescribed_bc must be of the same size as the domain.")

            # rotate it
            if self.direction == 'y':
                self.prescribed_bc.dirichlet = self.prescribed_bc.dirichlet.transpose((1, 0, 2))
            elif self.direction == 'z':
                self.prescribed_bc.dirichlet = self.prescribed_bc.dirichlet.transpose((2, 1, 0))
            if np.any((self.prescribed_bc.dirichlet[[0, -1]] == np.Inf)):
                raise Exception("prescribed_bc must be defined on the direction sides")
