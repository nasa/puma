from pumapy.physicsmodels.mpsa_elasticity import Elasticity
from pumapy.utilities.property_maps import ElasticityMap


def compute_elasticity(workspace, elast_map, direction, side_bc='p', prescribed_bc=None, tolerance=1e-4,
                       maxiter=10000, solver_type='bicgstab', display_iter=True, print_matrices=(0, 0, 0, 0, 0)):
    """ Compute the thermal conductivity (N.B. 0 material ID in workspace refers to air unless otherwise specified)

    :param workspace: domain
    :type workspace: Workspace
    :param elast_map: local elasticity of the constituents
    :type elast_map: ElasticityMap
    :param direction: direction for solve ('x','y', 'z', 'yz', 'xz', 'xy')
    :type direction: string
    :param side_bc: side boundary conditions can be symmetric ('s'), periodic ('p'), dirichlet ('d') or free ('f')
    :type side_bc: string, optional
    :param prescribed_bc: 3D array holding dirichlet BC
    :type prescribed_bc: ElasticityBC, optional
    :param tolerance: tolerance for iterative solver
    :type: tolerance: float, optional
    :param maxiter: maximum Iterations for solver
    :type maxiter: int, optional
    :param solver_type: solver type, options: 'bicgstab', 'cg', 'gmres', 'direct'
    :type solver_type: string, optional
    :param display_iter: display iterations and residual
    :type display_iter: bool, optional
    :param print_matrices: corresponding to b, E, A, u, s decimal places. If 0, they are not printed
    :type print_matrices: tuple(5 ints), optional
    :return: elasticity, displacement field, direct stresses, shear stresses
    :rtype: tuple(tuple(6 floats), ndarray, ndarray, ndarray)

    :Example:
    >>> import pumapy as puma
    >>> export_name = 'halfmat'
    >>> X = 20
    >>> Y = 20
    >>> Z = 20
    >>> ws = puma.Workspace.from_shape_value((X, Y, Z), 1)
    >>> ws[int(X / 2):] = 2
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


def compute_stress_analysis(workspace, elast_map, prescribed_bc=None, side_bc='p', tolerance=1e-4,
                            maxiter=10000, solver_type='bicgstab', display_iter=True, print_matrices=(0, 0, 0, 0, 0)):
    """ Compute the thermal conductivity (N.B. 0 material ID in workspace refers to air unless otherwise specified)

    :param workspace: domain
    :type workspace: Workspace
    :param elast_map: local elasticity of the constituents
    :type elast_map: ElasticityMap
    :param prescribed_bc: 3D array holding dirichlet BC
    :type prescribed_bc: ElasticityBC, optional
    :param side_bc: side boundary conditions can be symmetric ('s'), periodic ('p'), dirichlet ('d') or free ('f')
    :type side_bc: string, optional
    :param tolerance: tolerance for iterative solver
    :type tolerance: float, optional
    :param maxiter: maximum Iterations for solver
    :type maxiter: int, optional
    :param solver_type: solver type, options: 'bicgstab', 'cg', 'gmres', 'direct'
    :type solver_type: string, optional
    :param display_iter: display iterations and residual
    :type display_iter: bool, optional
    :param print_matrices: corresponding to b, E, A, u, s decimal places. If 0, they are not printed
    :type print_matrices: tuple(5 ints), optional
    :return: displacement field, direct stresses, shear stresses 'yz', 'xz', 'xy'
    :rtype: tuple(ndarray, ndarray, ndarray)
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
