from pumapy.utilities.timer import Timer
from pumapy.physics_models.utils.linear_solvers import PropertySolver
from pumapy.physics_models.utils.boundary_conditions import IsotropicConductivityBC
from pumapy.utilities.workspace import Workspace
from pumapy.physics_models.finite_volume.isotropic_conductivity_utils import setup_matrices_cy, compute_flux, matvec_cy, vecvec_prec_cy
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

            self.kf_i = np.copy(self.cond)
            self.kf_i[1:-1,1:-1,1:-1] = (- self.cond[0:-2,1:-1,1:-1] * self.cond[1:-1,1:-1,1:-1] / (self.cond[0:-2,1:-1,1:-1] + self.cond[1:-1,1:-1,1:-1]) - self.cond[2:,1:-1,1:-1] * self.cond[1:-1,1:-1,1:-1] / (self.cond[2:,1:-1,1:-1] + self.cond[1:-1,1:-1,1:-1])
                             - self.cond[1:-1,0:-2,1:-1] * self.cond[1:-1,1:-1,1:-1] / (self.cond[1:-1,0:-2,1:-1] + self.cond[1:-1,1:-1,1:-1]) - self.cond[1:-1,2:,1:-1] * self.cond[1:-1,1:-1,1:-1] / (self.cond[1:-1,2:,1:-1] + self.cond[1:-1,1:-1,1:-1])
                             - self.cond[1:-1,1:-1,0:-2] * self.cond[1:-1,1:-1,1:-1] / (self.cond[1:-1,1:-1,0:-2] + self.cond[1:-1,1:-1,1:-1]) - self.cond[1:-1,1:-1,2:] * self.cond[1:-1,1:-1,1:-1] / (self.cond[1:-1,1:-1,2:] + self.cond[1:-1,1:-1,1:-1]))
            self.kf_i = self.kf_i.flatten('F')

            self.kf_ixm = np.copy(self.cond)
            self.kf_ixm[1:-1, 1:-1, 1:-1] = (self.cond[0:-2,1:-1,1:-1] * self.cond[1:-1,1:-1,1:-1] / (self.cond[0:-2,1:-1,1:-1] + self.cond[1:-1,1:-1,1:-1]))
            self.kf_ixm = self.kf_ixm.flatten('F')

            self.kf_ixp = np.copy(self.cond)
            self.kf_ixp[1:-1, 1:-1, 1:-1] = (self.cond[2:,1:-1,1:-1] * self.cond[1:-1,1:-1,1:-1] / (self.cond[2:,1:-1,1:-1] + self.cond[1:-1,1:-1,1:-1]))
            self.kf_ixp = self.kf_ixp.flatten('F')

            self.kf_iym = np.copy(self.cond)
            self.kf_iym[1:-1, 1:-1, 1:-1] = (self.cond[1:-1,0:-2,1:-1] * self.cond[1:-1,1:-1,1:-1] / (self.cond[1:-1,0:-2,1:-1] + self.cond[1:-1,1:-1,1:-1]))
            self.kf_iym = self.kf_iym.flatten('F')

            self.kf_iyp = np.copy(self.cond)
            self.kf_iyp[1:-1, 1:-1, 1:-1] = (self.cond[1:-1,2:,1:-1] * self.cond[1:-1,1:-1,1:-1] / (self.cond[1:-1,2:,1:-1] + self.cond[1:-1,1:-1,1:-1]))
            self.kf_iyp = self.kf_iyp.flatten('F')

            self.kf_izm = np.copy(self.cond)
            self.kf_izm[1:-1, 1:-1, 1:-1] = (self.cond[1:-1,1:-1,0:-2] * self.cond[1:-1,1:-1,1:-1] / (self.cond[1:-1,1:-1,0:-2] + self.cond[1:-1,1:-1,1:-1]))
            self.kf_izm = self.kf_izm.flatten('F')

            self.kf_izp = np.copy(self.cond)
            self.kf_izp[1:-1, 1:-1, 1:-1] = (self.cond[1:-1,1:-1,2:] * self.cond[1:-1,1:-1,1:-1] / (self.cond[1:-1,1:-1,2:] + self.cond[1:-1,1:-1,1:-1]))
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
            y = np.zeros(self.len_xyz, dtype=float)




            kf = self.cond.flatten('F')
            def matvec(x):  # overload matvec for Amat=LinearOperator
                y.fill(0)
                matvec_cy(kf, self.kf_i, self.kf_ixm, self.kf_ixp, self.kf_iym, self.kf_iyp, self.kf_izm, self.kf_izp, x, y, self.len_x, self.len_y, self.len_z, self.domain_bc_check, self.bc_check, self.prescribed_bc, self.side_bc)
                return y
            self.Amat = LinearOperator(shape=(self.len_xyz, self.len_xyz), matvec=matvec)

            def vecvec_prec(x):
                y.fill(0)
                vecvec_prec_cy(kf, self.kf_i, self.kf_ixm, self.kf_ixp, self.kf_iym, self.kf_iyp, self.kf_izm, self.kf_izp, x, y, self.len_x, self.len_y, self.len_z, self.domain_bc_check, self.bc_check, self.prescribed_bc, self.side_bc)
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

        # side_bc checks
        if self.side_bc == "periodic" or self.side_bc == "Periodic" or self.side_bc == "p":
            self.side_bc = "p"
        elif self.side_bc == "symmetric" or self.side_bc == "Symmetric" or self.side_bc == "s":
            self.side_bc = "s"
        else:
            raise Exception("Invalid side boundary conditions.")
