from pumapy.physicsmodels.fenics_stokes import Permeability


def compute_permeability(workspace, direction, solid_cutoff, side_bc='fs', first_order=True, inf_sup=0.2,
                         pressure_driven=True, rel_tol=1e-8, abs_tol=1e-6, maxiter=10000, solver_type='bicgstab',
                         prec_type=None, display_iter=16, export_path=None):
    """ Compute the permeability

    :param workspace: domain
    :type workspace: Workspace
    :param direction: direction for solve ('x','y', or 'z')
    :type direction: string
    :param solid_cutoff: specify the solid phase
    :type solid_cutoff: tuple(int, int)
    :param side_bc: side boundary conditions can be free slip ('fs'), no slip ('ns') or periodic ('p')
    :type side_bc: string, optional
    :param first_order: specify whether to use Stab. 1st order method or Taylor-Hood method
    :type first_order: bool, optional
    :param inf_sup: inf-sup parameter used in case first_order=True to stabilize the Q1-Q1 method
    :type inf_sup: float, optional
    :param pressure_driven: whether the permeability is to be computed using a pressure driven-flow or body force
    :type pressure_driven: bool, optional
    :param rel_tol: relative tolerance for iterative solver
    :type rel_tol: float, optional
    :param abs_tol: absolute tolerance for iterative solver
    :type abs_tol: float, optional
    :param maxiter: maximum Iterations for solver
    :type maxiter: int, optional
    :param solver_type: solver type, options: 'bicgstab', 'minres', 'gmres', 'direct'
    :type solver_type: string, optional
    :param prec_type: preconditioner type, options: 'amg', 'sor', 'ilu', 'icc' or None
    :type prec_type: string, optional
    :param display_iter: FEniCS progress print: 16 for what's happening (broadly),
        13 for what's happening (in detail), 20 for information of general interest, 0 for printing off
    :type display_iter: int, optional
    :param export_path: export path for intermediary steps (mesh_facets, p, v). If None (default), no export
    :type export_path: string, optional
    :return: permeability, pressure field, velocity field
    :rtype: tuple(ndarray, ndarray, ndarray)
    """
    solver = Permeability(workspace, direction, solid_cutoff, side_bc, first_order, inf_sup, pressure_driven,
                          rel_tol, abs_tol, maxiter, solver_type, prec_type, display_iter, export_path)

    solver.error_check()

    solver.log_input()
    solver.compute()
    solver.log_output()
    return solver.keff, solver.pressure, solver.velocity
