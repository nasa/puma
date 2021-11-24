from pumapy.physicsmodels.isotropic_conductivity import IsotropicConductivity
from pumapy.physicsmodels.mpfa_conductivity import AnisotropicConductivity
from pumapy.physicsmodels.property_maps import IsotropicConductivityMap, AnisotropicConductivityMap


def compute_thermal_conductivity(workspace, cond_map, direction, side_bc='s', prescribed_bc=None, tolerance=1e-4,
                                 maxiter=10000, solver_type='bicgstab', display_iter=True, print_matrices=(0, 0, 0, 0, 0)):
    """ Compute the thermal conductivity

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param cond_map: local constituents themal conductivities
        :type cond_map: IsotropicConductivityMap or AnisotropicConductivityMap
        :param direction: direction for solve ('x','y', or 'z')
        :type direction: string
        :param side_bc: side boundary conditions can be symmetric ('s'), periodic ('p') or dirichlet ('d')
        :type side_bc: string
        :param prescribed_bc: 3D array holding dirichlet BC.
        :type prescribed_bc: pumapy.ConductivityBC or None
        :param tolerance: tolerance for iterative solver
        :type tolerance: float
        :param maxiter: maximum Iterations for solver
        :type maxiter: int
        :param solver_type: solver type, options: 'bicgstab' (default), 'cg', 'gmres', 'direct'
        :type solver_type: string
        :param display_iter: display iterations and residual
        :type display_iter: bool
        :param print_matrices: corresponding to b, E, A, T, q decimal places. If 0, they are not printed
        :type print_matrices: (bool, bool, bool, bool, bool)
        :return: thermal conductivity, temperature field, flux
        :rtype: ((float, float, float), numpy.ndarray, numpy.ndarray)

        :Example:
        >>> import pumapy as puma
        >>> ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
        >>> ws_fiberform.rescale(0.5, segmented=False)
        >>>
        >>> # Conductivity with Isotropic local phases
        >>> cond_map = puma.IsotropicConductivityMap()
        >>> cond_map.add_material((0, 89), 0.0257)
        >>> cond_map.add_material((90, 255), 12)
        >>> k_eff_x, T_x, q_x = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 's')
        >>>
        >>> # Conductivity with Anisotropic local phases
        >>> puma.compute_orientation_st(ws_fiberform, cutoff=(90, 255))
        >>> cond_map = puma.AnisotropicConductivityMap()
        >>> # conductivity of the void phase to be 0.0257 (air at STP)
        >>> cond_map.add_isotropic_material((0, 89), 0.0257)
        >>> # axial fiber conductivity of 12, radial fiber conductivity of 0.7
        >>> cond_map.add_material_to_orient((90, 255), 12., 0.7)
        >>> k_eff_z, T_z, q_z = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 's')
    """
    if isinstance(cond_map, IsotropicConductivityMap):
        solver = IsotropicConductivity(workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter,
                                       solver_type, display_iter)
    elif isinstance(cond_map, AnisotropicConductivityMap):
        solver = AnisotropicConductivity(workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter,
                                         solver_type, display_iter, print_matrices)
    else:
        raise Exception("cond_map has to be an IsotropicConductivityMap or AnisotropicConductivityMap")

    solver.error_check()

    solver.log_input()
    solver.compute()
    solver.log_output()
    return solver.keff, solver.T, solver.q


def compute_electrical_conductivity(workspace, cond_map, direction, side_bc='p', prescribed_bc=None, tolerance=1e-4,
                                    maxiter=10000, solver_type='bicgstab', display_iter=True, print_matrices=(0, 0, 0, 0, 0)):
    """ Compute the electrical conductivity

    :param workspace: domain
    :type workspace: pumapy.Workspace
    :param cond_map: local constituents electrical conductivities
    :type cond_map: IsotropicConductivityMap or AnisotropicConductivityMap
    :param direction: direction for solve ('x','y', or 'z')
    :type direction: string
    :param side_bc: side boundary conditions can be symmetric ('s'), periodic ('p') or dirichlet ('d')
    :type side_bc: string
    :param prescribed_bc: 3D array holding dirichlet BC
    :type prescribed_bc: pumapy.ConductivityBC or None
    :param tolerance: tolerance for iterative solver
    :type tolerance: float
    :param maxiter: maximum Iterations for solver
    :type maxiter: int
    :param solver_type: solver type, options: 'bicgstab' (default), 'cg', 'gmres', 'direct'
    :type solver_type: string
    :param display_iter: display iterations and residual
    :type display_iter: bool
    :param print_matrices: corresponding to E, A, b, T, q decimal places. If 0, they are not printed
    :type print_matrices: (bool, bool, bool, bool, bool)
    :return: electrical conductivity, potential field, flux
    :rtype: ((float, float, float), numpy.ndarray, numpy.ndarray)

    :Example:
    >>> import pumapy as puma
    >>> ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
    >>> ws_fiberform.matrix = ws_fiberform.matrix[50:150, 50:150, 50:150]
    >>> cond_map = puma.IsotropicConductivityMap()
    >>> cond_map.add_material((0, 89), 0.0257)
    >>> cond_map.add_material((90, 255), 12)
    >>> k_eff_x, P_x, q_x = puma.compute_electrical_conductivity(ws_fiberform, cond_map, 'x', 's', tolerance=1e-2, solver_type='cg')
    """
    return compute_thermal_conductivity(workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter,
                                        solver_type, display_iter, print_matrices)
