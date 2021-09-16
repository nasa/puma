from pumapy.physicsmodels.fe_permeability import Permeability


def compute_permeability(workspace, solid_cutoff, tol=1e-8, maxiter=10000, solver_type='minres', display_iter=True):
    """ Compute the permeability using first order Q1-Q1 Finite Element solver and periodic BC on the sides

    :param workspace: domain
    :type workspace: Workspace
    :param solid_cutoff: specify the solid phase
    :type solid_cutoff: tuple(int, int)
    :param tol: tolerance for iterative solver
    :type tol: float, optional
    :param maxiter: maximum Iterations for solver
    :type maxiter: int, optional
    :param solver_type: solver type, options: 'minres' (default), 'direct', 'cg', 'bicgstab'
    :type solver_type: string, optional
    :type display_iter: bool, optional
    :return: permeability, velocity field
    :rtype: tuple(ndarray, ndarray)
    """
    solver = Permeability(workspace, solid_cutoff, tol, maxiter, solver_type, display_iter)

    solver.error_check()

    solver.log_input()
    solver.compute()
    solver.log_output()
    return solver.keff, solver.u_x, solver.u_y, solver.u_z
