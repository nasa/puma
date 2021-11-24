from pumapy.utilities.logger import print_warning
from pumapy.utilities.timer import Timer
from pumapy.physicsmodels.boundary_conditions import Isotropic_periodicBC, Isotropic_symmetricBC
from pumapy.physicsmodels.conductivity_parent import Conductivity
from pumapy.physicsmodels.isotropic_conductivity_utils import setup_matrices_cy, compute_flux
from pumapy.utilities.generic_checks import estimate_max_memory
from scipy.sparse import coo_matrix, diags
import numpy as np


class IsotropicConductivity(Conductivity):

    def __init__(self, workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter, solver_type,
                 display_iter):
        super().__init__(workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter, solver_type,
                         display_iter)
        self._bc_func = None
        self.cond = np.zeros([1, 1, 1])

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
        if self.side_bc == "p" or self.side_bc == "periodic":
            self._bc_func = Isotropic_periodicBC(self.len_x, self.len_y, self.len_z)
        elif self.side_bc == "s" or self.side_bc == "symmetric":
            self._bc_func = Isotropic_symmetricBC(self.len_x, self.len_y, self.len_z)
        else:
            print_warning("Side Boundary Condition not recognized. Defaulting to symmetric")
            self._bc_func = Isotropic_symmetricBC(self.len_x, self.len_y, self.len_z)

        self.n_iter = 0
        self.Amat = None
        self.M = None
        self.bvec = np.zeros(1)
        self.initial_guess = np.zeros(1)

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
        self._row, self._col, self._data = setup_matrices_cy(self.cond.flatten('F'), self.len_x, self.len_y, self.len_z,
                                                             self.bc_check, self.prescribed_bc)
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

        d = {'x': 'first', 'y': 'second', 'z': 'third'}
        print(f'\nEffective conductivity tensor ({d[self.direction]} row): \n{self.keff}\n')

        # making the flux have the correct spacial units
        self.q /= self.ws.voxel_length
