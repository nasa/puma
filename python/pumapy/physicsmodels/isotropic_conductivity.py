from pumapy.utilities.logger import print_warning
from pumapy.utilities.timer import Timer
from pumapy.utilities.boundary_conditions import Isotropic_periodicBC, Isotropic_symmetricBC
from pumapy.physicsmodels.conductivity_parent import Conductivity, SolverDisplay
from pumapy.physicsmodels.isotropic_conductivity_utils import setup_matrices_cy, compute_flux
import numpy as np
from scipy.sparse import csr_matrix, diags
from scipy.sparse.linalg import bicgstab, spsolve, cg, gmres  # LinearOperator, spilu
import math


class IsotropicConductivity(Conductivity):

    def __init__(self, workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter, solver_type, display_iter):
        super().__init__(workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter, solver_type, display_iter)
        self._bc_func = None
        self.cond = np.zeros([1, 1, 1])

        if self.direction == 'x':
            self._matrix = workspace.matrix
        elif self.direction == 'y':
            self._matrix = workspace.matrix.transpose(1, 0, 2)
        elif self.direction == 'z':
            self._matrix = workspace.matrix.transpose(2, 1, 0)

        self.len_x = self._matrix.shape[0]
        self.len_y = self._matrix.shape[1]
        self.len_z = self._matrix.shape[2]
        self.len_xy = self.len_x * self.len_y
        self.len_xyz = self.len_xy * self.len_z

        if self.side_bc == "p" or self.side_bc == "periodic":
            self._bc_func = Isotropic_periodicBC(self.len_x, self.len_y, self.len_z)
        elif self.side_bc == "s" or self.side_bc == "symmetric":
            self._bc_func = Isotropic_symmetricBC(self.len_x, self.len_y, self.len_z)
        else:
            print_warning("Side Boundary Condition not recognized. Defaulting to symmetric")
            self._bc_func = Isotropic_symmetricBC(self.len_x, self.len_y, self.len_z)

        self.n_iter = 0
        self._A = None
        self._M = None
        self._bf = np.zeros(1)
        self._Tf = np.zeros(1)
        self._kf = np.zeros(1)

    def compute(self):
        self.__create_cond_matrix()
        self.__init_temperature()
        t = Timer()
        self.__setup_matrices_cy()
        print("Time to sep up A matrix: " , t.elapsed())
        t.reset()
        self.__solve()
        print("Time to solve: ", t.elapsed())
        t.reset()
        self.__compute_conductivity()
        print("Time to compute fluxes: ", t.elapsed())

    def __create_cond_matrix(self):
        print("Creating conductivity matrix ... ", end='')
        self.cond = np.zeros(self._matrix.shape, dtype=float)
        for i in range(self.cond_map.get_size()):
            low, high, k = self.cond_map.get_material(i)
            mask_low = self._matrix >= low
            mask_high = self._matrix <= high
            mask_low = mask_low * mask_high
            self.cond[mask_low] = k

        print("Done")

    def __init_temperature(self):
        print("Initializing temperature field ... ", end='')
        self.T = np.zeros([self.len_x, self.len_y, self.len_z])
        for i in range(self.len_x):
            self.T[i, :, :] = i / (self.len_x - 1.)
        self._Tf = self.T.flatten('F')
        print("Done")

    def __setup_matrices_cy(self):

        print("Setting up b matrix ... ", end='')
        bsq = np.zeros([self.len_x, self.len_y, self.len_z])
        if self.prescribed_bc is not None:
            for i in range(self.len_x):
                for j in range(self.len_y):
                    for k in range(self.len_z):
                        if self.prescribed_bc[i, j, k] != np.Inf:
                            bsq[i, j, k] = self.prescribed_bc[i, j, k]

            self.prescribed_bc = self.prescribed_bc.dirichlet  # because of cython, cannot pass object
            bc_check = 1
        else:
            bc_check = 0
            self.prescribed_bc = np.full(self._matrix.shape, np.Inf, dtype=float)
            bsq[-1, :, :] = 1

        # check for zero conductivity
        for i in range(self.cond_map.get_size()):
            low, high, k = self.cond_map.get_material(i)
            if k == 0:
                bc_check = 1
                self.prescribed_bc[(self._matrix >= low) * (self._matrix <= high)] = 0

        self._bf = bsq.flatten('F')
        print("Done")

        self._kf = self.cond.flatten('F')
        self._row, self._col, self._data = setup_matrices_cy(self._kf, self.len_x, self.len_y, self.len_z,
                                                             bc_check, self.prescribed_bc)
        n_elem = self.len_xyz
        self._A = csr_matrix((self._data, (self._row, self._col)), shape=(n_elem, n_elem))
        print("Done")

        print("Setting up preconditioner ...", end ='')
        # ilu = spilu(self._A.tocsc(), drop_tol=1e-5)
        # Mx = lambda x: ilu.solve(x)
        # self._M = LinearOperator((n_elem, n_elem), Mx)
        diag = self._A.diagonal()
        diag[diag==0] = 1
        diag[:] = 1./diag[:]
        self._M = diags(diag,0).tocsr()
        print("Done")



    def __solve(self):
        print("Solving Ax=b system ... ", end='')

        info = 0

        if self.solver_type == 'direct':
            print("Direct solver", end='')
            self._Tf = spsolve(self._A, self._bf)
        elif self.solver_type == 'cg':
            print("Conjugate Gradient:")
            if self.display_iter:
                self._Tf, info = cg(self._A, self._bf, x0=self._Tf, atol=self.tolerance, maxiter=self.maxiter,
                                    callback=SolverDisplay(), M=self._M)
            else:
                self._Tf, info = cg(self._A, self._bf, x0=self._Tf, atol=self.tolerance, maxiter=self.maxiter, M=self._M)

        elif self.solver_type == 'gmres':
            print("gmres:")
            if self.display_iter:
                self._Tf, info = gmres(self._A, self._bf, x0=self._Tf, atol=self.tolerance,
                                       maxiter=self.maxiter, callback=SolverDisplay(), M=self._M)
            else:
                self._Tf, info = gmres(self._A, self._bf, x0=self._Tf, atol=self.tolerance,
                                       maxiter=self.maxiter, M=self._M)
        else:
            if self.solver_type != 'bicgstab':
                print_warning("Unrecognized solver, defaulting to bicgstab.")
            print("Bicgstab:")
            if self.display_iter:
                self._Tf, info = bicgstab(self._A, self._bf, x0=self._Tf, atol=self.tolerance,
                                          maxiter=self.maxiter, callback=SolverDisplay(), M=self._M)
            else:
                self._Tf, info = bicgstab(self._A, self._bf, x0=self._Tf, atol=self.tolerance,
                                          maxiter=self.maxiter, M=self._M)

        if info != 0:
            raise Exception("Solver error: " + str(info))
        self.T = self._Tf.reshape([self.len_x, self.len_y, self.len_z], order='F')
        print(" ... Done")

    def __compute_conductivity(self):

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

        print("Computing effective conductivity... ", end='')
