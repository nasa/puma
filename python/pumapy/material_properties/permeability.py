from pumapy.physics_models.finite_element.fe_permeability import Permeability


def compute_permeability(workspace, solid_cutoff, direction='xyz', tol=1e-8, maxiter=10000, solver_type='minres',
                         display_iter=True, matrix_free=True, precondition=False, output_fields=True):
    """ Compute the permeability using first-order Q1-Q1 Finite Element solver and periodic BC on the sides.

        Note: the iterative solvers have been observed to struggle converging for some cases. This is often
        connected to having the preconditioner option as True.

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param solid_cutoff: specify the solid phase
        :type solid_cutoff: (int, int)
        :param direction: direction for solve ('xyz','x','y','z'). Note that if solver_type="direct", then the direction
        is considered as "xyz" automatically because there is no need to invert the A sparse matrix multiple times
        :type direction: str
        :param tol: tolerance for iterative solver
        :type tol: float
        :param maxiter: maximum Iterations for solver
        :type maxiter: int
        :param solver_type: solver type, options: 'minres' (default, only works if precondition=False), 'cg', 'direct'
        :type solver_type: string
        :param display_iter: display iteration in iterative solver
        :type display_iter: bool
        :param matrix_free: solve system using matrix-free method (True, recommended) or building the sparse A matrix (False)
        :type matrix_free: bool
        :param precondition: solve system with Jacobi preconditioner (True) or without (False, default because
        it reduces memory, but more iterations. The default minres iterative solver does not support this kind of preconditioner)
        :type precondition: bool
        :param output_fields: export velocity and pressure fields (True, default) or not (False, lower memory)
        :type output_fields: bool
        :return: effective permeability (3x3 matrix) and, if output_fields=True, the normalized velocity field for the corresponding
        direction (arranged as tuple of numpy.ndarrays, i.e. (ux, uy, uz). If output_fields=False, then (None, None, None) is output
        :rtype: numpy.ndarray, (numpy.ndarray, numpy.ndarray, numpy.ndarray)

        :Example:
        >>> import pumapy as puma
        >>> import pyvista as pv
        >>> ws = puma.generate_random_fibers_transverseisotropic(shape=(50, 50, 50), radius=2, porosity=0.7, direction='x', variation=15, length=200, allow_intersect=True, segmented=True)
         Fibers created...
        >>> keff, (ux, _, _) = puma.compute_permeability(ws, (1, ws.max()), direction='x', tol=1e-7)
        Approximate memory requirement for simulation: ...
        >>> # p = pv.Plotter()  # to visualize it
        >>> # puma.render_orientation(ux, add_to_plot=p, scale_factor=2e12, plot_directly=False)
        >>> # ws.voxel_length = 1  # the voxel_length is converted to 1 for plotting the workspace together with the velocity
        >>> # puma.render_volume(ws, cutoff=(1, ws.max()), add_to_plot=p, plot_directly=False, cmap='jet')
        >>> # p.show()
    """
    solver = Permeability(workspace, solid_cutoff, direction, tol, maxiter, solver_type, display_iter,
                          matrix_free, precondition, output_fields)

    solver.error_check()

    solver.log_input()
    solver.compute()
    solver.log_output()
    return solver.keff, (solver.ux, solver.uy, solver.uz)
