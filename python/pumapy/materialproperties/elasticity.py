from pumapy.physicsmodels.mpsa_elasticity import Elasticity
from pumapy.utilities.property_maps import ElasticityMap


def compute_elasticity(workspace, elast_map, direction, side_bc='p', prescribed_bc=None, tolerance=1e-4,
                       maxiter=10000, solver_type='bicgstab', display_iter=True, print_matrices=(0, 0, 0, 0, 0)):
    """ Compute the thermal conductivity (N.B. 0 material ID in workspace refers to air unless otherwise specified)

    :param workspace: domain
    :type workspace: pumapy.Workspace
    :param elast_map: local elasticity of the constituents
    :type elast_map: pumapy.ElasticityMap
    :param direction: direction for solve ('x','y', 'z', 'yz', 'xz', 'xy')
    :type direction: string
    :param side_bc: side boundary conditions can be symmetric ('s'), periodic ('p'), dirichlet ('d') or free ('f')
    :type side_bc: string
    :param prescribed_bc: 3D array holding dirichlet BC
    :type prescribed_bc: pumapy.ElasticityBC
    :param tolerance: tolerance for iterative solver
    :type: tolerance: float
    :param maxiter: maximum Iterations for solver
    :type maxiter: int
    :param solver_type: solver type, options: 'bicgstab', 'cg', 'gmres', 'direct'
    :type solver_type: string
    :param display_iter: display iterations and residual
    :type display_iter: bool
    :param print_matrices: corresponding to b, E, A, u, s decimal places. If 0, they are not printed
    :type print_matrices: (int, int, int, int, int)
    :return: elasticity, displacement field, direct stresses (sigma xx, yy, zz), shear stresses (tau yz, xz, xy)
    :rtype: ((float, float, float, float, float, float), numpy.ndarray, numpy.ndarray, numpy.ndarray)

    :Example:
    >>> import pumapy as puma
    >>> ws = puma.Workspace.from_shape_value((20, 20, 20), 1)
    >>> ws[int(ws.matrix.shape[0] / 2):] = 2
    >>> elast_map = puma.ElasticityMap()
    >>> elast_map.add_isotropic_material((1, 1), 200, 0.3)
    >>> elast_map.add_isotropic_material((2, 2), 400, 0.1)
    >>> C, u, s, t = puma.compute_elasticity(ws, elast_map, direction='x', side_bc='f', solver_type="direct")
    >>> print(C)
    """
    if isinstance(elast_map, ElasticityMap):
        solver = Elasticity(workspace, elast_map, direction, side_bc, prescribed_bc, tolerance, maxiter,
                            solver_type, display_iter, print_matrices)
    else:
        raise Exception("elast_map has to be an ElasticityMap")

    solver.error_check()

    solver.log_input()
    solver.compute()
    solver.log_output()
    return solver.Ceff, solver.u, solver.s, solver.t


def compute_stress_analysis(workspace, elast_map, prescribed_bc, side_bc='p', tolerance=1e-4,
                            maxiter=10000, solver_type='bicgstab', display_iter=True, print_matrices=(0, 0, 0, 0, 0)):
    """ Compute the thermal conductivity (N.B. 0 material ID in workspace refers to air unless otherwise specified)

    :param workspace: domain
    :type workspace: pumapy.Workspace
    :param elast_map: local elasticity of the constituents
    :type elast_map: pumapy.ElasticityMap
    :param prescribed_bc: 3D array holding dirichlet BC
    :type prescribed_bc: pumapy.ElasticityBC
    :param side_bc: side boundary conditions can be symmetric ('s'), periodic ('p'), dirichlet ('d') or free ('f')
    :type side_bc: string
    :param tolerance: tolerance for iterative solver
    :type tolerance: float
    :param maxiter: maximum Iterations for solver
    :type maxiter: int
    :param solver_type: solver type, options: 'bicgstab', 'cg', 'gmres', 'direct'
    :type solver_type: string
    :param display_iter: display iterations and residual
    :type display_iter: bool
    :param print_matrices: corresponding to b, E, A, u, s decimal places. If 0, they are not printed
    :type print_matrices: (int, int, int, int, int)
    :return: displacement field, direct stresses (sigma xx, yy, zz), shear stresses (tau yz, xz, xy)
    :rtype: (numpy.ndarray, numpy.ndarray, numpy.ndarray)

    :Example:
    >>> import pumapy as puma
    >>> ws = puma.Workspace.from_shape_value((20, 20, 20), 1)
    >>> ws[ws.matrix.shape[0]//2:] = 2
    >>> elast_map = puma.ElasticityMap()
    >>> elast_map.add_isotropic_material((1, 1), 200, 0.3)
    >>> elast_map.add_isotropic_material((2, 2), 400, 0.1)
    >>> bc = puma.ElasticityBC.from_workspace(ws)
    >>> bc[0] = 0  # hold x -ve face
    >>> bc[-1, :, :, 0] = 1   # displace x +ve face by 1 in x direction
    >>> bc[-1, :, :, 1:] = 0  # hold x +ve face in y and z directions
    >>> u, s, t = puma.compute_stress_analysis(ws, elast_map, bc, side_bc='f', solver_type="direct")
    >>> puma.render_volume(u[:, :, :, 1], cmap='jet')  # displacement magnitude in y direction
    """
    if isinstance(elast_map, ElasticityMap):
        solver = Elasticity(workspace, elast_map, None, side_bc, prescribed_bc, tolerance, maxiter,
                            solver_type, display_iter, print_matrices)
    else:
        raise Exception("elast_map has to be an ElasticityMap")

    solver.error_check()

    solver.log_input()
    solver.compute()
    solver.log_output()
    return solver.u, solver.s, solver.t
