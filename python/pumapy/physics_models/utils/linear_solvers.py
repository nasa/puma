import numpy as np
from pumapy.utilities.logger import print_warning
from scipy.sparse.linalg import bicgstab, spsolve, cg, gmres, minres
import inspect
import sys


class PropertySolver:
    def __init__(self, workspace, solver_type, allowed_solvers, tolerance, maxiter, display_iter):
        self.ws = workspace.copy()
        self.tolerance = tolerance
        self.maxiter = maxiter
        self.solver_type = solver_type
        self.allowed_solvers = allowed_solvers

        # First two sparse matrices need to be defined in Property child class, last two are optional
        self.Amat = None
        self.bvec = None
        self.initial_guess = None
        self.M = None

        # it returns answer in
        self.x = None

        self.del_matrices = True

        self.len_x, self.len_y, self.len_z = self.ws.matrix.shape
        self.len_xy = self.len_x * self.len_y
        self.len_xyz = self.len_xy * self.len_z

        self.callback = None
        self.display_iter = display_iter

    def solve(self):
        if self.solver_type not in self.allowed_solvers:
            print_warning(f"Unrecognized solver, defaulting to {self.allowed_solvers[0]}.")
            self.solver_type = self.allowed_solvers[0]

        print(f"Solving Ax=b using {self.solver_type} solver", end='')
        if self.display_iter:
            if self.solver_type == "minres":
                self.callback = MinResSolverDisplay()
            else:
                self.callback = SolverDisplay()

        info = 0
        if (self.solver_type == 'direct') and self.solver_type in self.allowed_solvers:
            self.x = spsolve(self.Amat, self.bvec)
            if not isinstance(self.x, np.ndarray):
                self.x = self.x.toarray()
        else:  # in order to use UMFPACK in spsolve, bvec needs to be a sparse matrix
            if not isinstance(self.bvec, np.ndarray):
                self.bvec = self.bvec.todense()
            print()

        # iterative solvers
        if self.solver_type == 'gmres' and self.solver_type in self.allowed_solvers:
            self.x, info = gmres(self.Amat, self.bvec, x0=self.initial_guess, M=self.M,
                                 atol=self.tolerance, maxiter=self.maxiter, callback=self.callback)

        elif self.solver_type == 'minres' and self.solver_type in self.allowed_solvers:
            self.x, info = minres(self.Amat, self.bvec, x0=self.initial_guess, M=self.M,
                                  tol=self.tolerance, maxiter=self.maxiter, callback=self.callback)

        elif self.solver_type == 'cg' and self.solver_type in self.allowed_solvers:
            self.x, info = cg(self.Amat, self.bvec, x0=self.initial_guess, M=self.M,
                              atol=self.tolerance, maxiter=self.maxiter, callback=self.callback)

        elif self.solver_type == 'bicgstab' and self.solver_type in self.allowed_solvers:
            self.x, info = bicgstab(self.Amat, self.bvec, x0=self.initial_guess, M=self.M,
                                    atol=self.tolerance, maxiter=self.maxiter, callback=self.callback)

        if info > 0:
            raise Exception("Convergence to tolerance not achieved.")
        elif info < 0:
            raise Exception("Solver illegal input or breakdown")

        if self.del_matrices:
            del self.Amat, self.bvec, self.initial_guess, self.M
        print(" ... Done")


class SolverDisplay(object):
    def __init__(self):
        self.niter = 0

    def __call__(self, rk=None):
        self.niter += 1
        try:
            frame = inspect.currentframe().f_back
            sys.stdout.write(f"\rIteration: {self.niter}, driving modified residual = {frame.f_locals['resid']:0.10f} --> target = {frame.f_locals['atol']:0.10f}")
            # resid = np.linalg.norm(frame.f_locals.get('r', 'unknown'))  # scipy 1.12 onwards
            # atol = frame.f_locals.get('atol', 'unknown')
            # sys.stdout.write(f"\rIteration: {self.niter}, driving modified residual = {resid:0.10f} --> target = {atol:0.10f}")
        except Exception as e:
            sys.stdout.write(f"\rIteration: {self.niter}, error retrieving frame data: {e}")


class MinResSolverDisplay(object):
    def __init__(self):
        self.niter = 0

    def __call__(self, rk=None):
        self.niter += 1
        try:
            frame = inspect.currentframe().f_back
            sys.stdout.write(f"\rIteration: {self.niter}, driving either residual ({frame.f_locals['test1']:0.10f}, {frame.f_locals['test2']:0.10f}) --> target = {frame.f_locals['tol']:0.10f}")
            # test1 = frame.f_locals.get('test1', 'unknown')  # scipy 1.12 onwards
            # test2 = frame.f_locals.get('test2', 'unknown')
            # tol = frame.f_locals.get('tol', 'unknown')
            # sys.stdout.write(f"\rIteration: {self.niter}, driving either residual ({test1:0.10f}, {test2:0.10f}) --> target = {tol:0.10f}")
        except Exception as e:
            sys.stdout.write(f"\rIteration: {self.niter}, error retrieving frame data: {e}")

