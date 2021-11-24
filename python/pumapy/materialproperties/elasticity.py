from pumapy.physicsmodels.mpsa_elasticity import Elasticity
from pumapy.physicsmodels.property_maps import ElasticityMap
from scipy.optimize import fsolve
import numpy as np


def compute_elasticity(workspace, elast_map, direction, side_bc='p', prescribed_bc=None, tolerance=1e-4,
                       maxiter=100000, solver_type='gmres', display_iter=True, print_matrices=(0, 0, 0, 0, 0)):
    """ Compute the effective elasticity coefficient
        (N.B. 0 material ID in workspace refers to air unless otherwise specified)

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param elast_map: local elasticity of the constituents
        :type elast_map: pumapy.ElasticityMap
        :param direction: direction for solve ('x','y', 'z', 'yz', 'xz', 'xy')
        :type direction: string
        :param side_bc: side boundary conditions can be symmetric ('s'), periodic ('p'), dirichlet ('d') or free ('f')
        :type side_bc: string
        :param prescribed_bc: 3D array holding dirichlet BC
        :type prescribed_bc: pumapy.ElasticityBC or None
        :param tolerance: tolerance for iterative solver
        :type: tolerance: float
        :param maxiter: maximum Iterations for solver
        :type maxiter: int
        :param solver_type: solver type, options: 'bicgstab', 'cg', 'gmres' (default), 'direct'
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
                            maxiter=100000, solver_type='gmres', display_iter=True, print_matrices=(0, 0, 0, 0, 0)):
    """ Compute stress analysis
        (N.B. 0 material ID in workspace refers to air unless otherwise specified)

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
        :param solver_type: solver type, options: 'bicgstab', 'cg', 'gmres' (default), 'direct'
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
        >>> bc = puma.ElasticityBC(ws)
        >>> bc.dirichlet[0] = 0  # hold x -ve face
        >>> bc.dirichlet[-1, :, :, 0] = 1   # displace x +ve face by 1 in x direction
        >>> bc.dirichlet[-1, :, :, 1:] = 0  # hold x +ve face in y and z directions
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


def get_E_nu_from_elasticity(C11, C12, C13, C22, C23, C33):
    """ Compute Young's moduli E1, E2, E3 and Poisson's ratios nu12, nu23, nu31 from symmetric elastic stiffness tensor
        :param C11: elasticity tensor component
        :type C11: float
        :param C12: elasticity tensor component
        :type C12: float
        :param C13: elasticity tensor component
        :type C13: float
        :param C22: elasticity tensor component
        :type C22: float
        :param C23: elasticity tensor component
        :type C23: float
        :param C33: elasticity tensor component
        :type C33: float
        :return: Young's moduli E1, E2, E3 and Poisson's ratios nu12, nu23, nu31
        :rtype: (float, float, float, float, float, float)
    """
    def eqs(unknowns, *cs):
        E1, E2, E3, nu12, nu23, nu31 = unknowns
        c11, c12, c31, c22, c23, c33 = cs
        nu21 = nu12 * E2 / E1
        nu32 = nu23 * E3 / E2
        nu13 = nu31 * E1 / E3
        c0 = (1 - nu12 * nu21 - nu23 * nu32 - nu13 * nu31 - 2 * nu21 * nu32 * nu13) / (E1 * E2 * E3)
        return (c11 - (1 - nu23 * nu32) / (c0 * E2 * E3),
                c22 - (1 - nu13 * nu31) / (c0 * E1 * E3),
                c33 - (1 - nu21 * nu12) / (c0 * E2 * E1),
                c12 - (nu12 + nu32 * nu13) / (c0 * E1 * E3),
                c23 - (nu23 + nu21 * nu13) / (c0 * E1 * E2),
                c31 - (nu31 + nu21 * nu32) / (c0 * E2 * E3))

    s = fsolve(eqs, np.full(6, 0.1), args=(C11, C12, C13, C22, C23, C33))
    [print(i, j) for i, j in zip(["E1", "E2", "E3", "nu12", "nu23", "nu31"], s)]
    return s
