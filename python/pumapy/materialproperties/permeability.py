from pumapy.physicsmodels.fe_permeability import Permeability


def compute_permeability(workspace, solid_cutoff, direction='all', tol=1e-6, maxiter=10000, solver_type='bicgstab',
                         display_iter=True, matrix_free=True, preconditioner=True):
    """ Compute the permeability using first order Q1-Q1 Finite Element solver and periodic BC on the sides

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param solid_cutoff: specify the solid phase
        :type solid_cutoff: (int, int)
        :param direction: direction for solve ('all','x','y','z'). Note that if solver_type='direct', then ignored
        :type direction: str
        :param tol: tolerance for iterative solver
        :type tol: float
        :param maxiter: maximum Iterations for solver
        :type maxiter: int
        :param solver_type: solver type, options: 'minres' (default), 'direct', 'cg', 'bicgstab'
        :type solver_type: string
        :param display_iter: display iteration in iterative solver
        :type display_iter: bool
        :param matrix_free: solve system using matrix-free method (True, recommended) or building the sparse A matrix (False)
        :type matrix_free: bool
        :param preconditioner: solve system with Jacobi preconditioner (True, recommended) or without (False, reduces memory, but more iterations)
        :type preconditioner: bool
        :return: effective permeability (3x3 matrix), velocity and pressure fields for x, y, z directions (u_x, p_x, etc)
        :rtype: numpy.ndarray, numpy.ndarray, numpy.ndarray, numpy.ndarray, numpy.ndarray, numpy.ndarray, numpy.ndarray

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.generate_random_fibers(shape=(100, 100, 100), radius=3, porosity=0.7, phi=90, theta=90, length=200)
        >>> keff, u_x, p_x, u_y, p_y, u_z, p_z = puma.compute_permeability(ws, (1, ws.max()))
        >>> puma.render_orientation(u_x, scale_factor=5e11, solid_color=None)
    """
    solver = Permeability(workspace, solid_cutoff, direction, tol, maxiter, solver_type, display_iter, matrix_free, preconditioner)

    solver.error_check()

    solver.log_input()
    solver.compute()
    solver.log_output()
    return solver.keff, solver.u_x, solver.p_x, solver.u_y, solver.p_y, solver.u_z, solver.p_z
