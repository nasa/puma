from pumapy.physics_models.finite_volume.isotropic_conductivity import IsotropicConductivity
from pumapy.physics_models.finite_volume.mpfa_conductivity import AnisotropicConductivity
from pumapy.physics_models.utils.property_maps import IsotropicConductivityMap, AnisotropicConductivityMap
from pumapy.physics_models.finite_element.fe_conductivity import ConductivityFE
from pumapy.io.output import export_vti
from pumapy.utilities.workspace import Workspace
import numpy as np
import pyvista as pv


def compute_thermal_conductivity(workspace, cond_map, direction, side_bc='s', prescribed_bc=None, tolerance=1e-4,
                                 maxiter=10000, solver_type='bicgstab', display_iter=True, print_matrices=(0, 0, 0, 0, 0),
                                 method="fv", matrix_free=True):
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
        Importing .../200_fiberform.tif ... Done
        >>> ws_fiberform.rescale(0.5, segmented=False)
        Rescaled workspace size: (100, 100, 100)
        >>> # Conductivity with Isotropic local phases
        >>> cond_map = puma.IsotropicConductivityMap()
        >>> cond_map.add_material((0, 89), 0.0257)
        >>> cond_map.add_material((90, 255), 12)
        >>> k_eff_x, T_x, q_x = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 's')
        Approximate memory requirement for simulation...
        >>> # Conductivity with Anisotropic local phases
        >>> puma.compute_orientation_st(ws_fiberform, cutoff=(90, 255))
        First gradient computation...
        >>> cond_map = puma.AnisotropicConductivityMap()
        >>> # conductivity of the void phase to be 0.0257 (air at STP)
        >>> cond_map.add_isotropic_material((0, 89), 0.0257)
        >>> # axial fiber conductivity of 12, radial fiber conductivity of 0.7
        >>> cond_map.add_material_to_orient((90, 255), 12., 0.7)
        >>> k_eff_z, T_z, q_z = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 's')
        Approximate memory requirement for simulation...
    """

    if method == "fv":
        if isinstance(cond_map, IsotropicConductivityMap):
            solver = IsotropicConductivity(workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter,
                                           solver_type, display_iter, matrix_free)
        elif isinstance(cond_map, AnisotropicConductivityMap):
            solver = AnisotropicConductivity(workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter,
                                             solver_type, display_iter, print_matrices)
        else:
            raise Exception("cond_map has to be an IsotropicConductivityMap or AnisotropicConductivityMap")
    elif method == "fe":
        solver = ConductivityFE(workspace, cond_map, direction, tolerance, maxiter, solver_type, display_iter, matrix_free)
    else:
        raise Exception("The method can only be set as 'fv' (i.e. finite volume, either isotropic or MPFA) or 'fe' (i.e. Q1-Q1 EBE finite element)")

    solver.error_check()

    solver.log_input()
    solver.compute()
    solver.log_output()

    d = {'x': 'first', 'y': 'second', 'z': 'third'}
    print(f'\nEffective conductivity tensor ({d[solver.direction]} column): \n{solver.keff}\n')
    return solver.keff, solver.T, solver.q


def compute_electrical_conductivity(workspace, cond_map, direction, side_bc='p', prescribed_bc=None, tolerance=1e-4,
                                    maxiter=10000, solver_type='bicgstab', display_iter=True, print_matrices=(0, 0, 0, 0, 0),
                                    method="fv", matrix_free=True):
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
        Importing .../200_fiberform.tif ... Done
        >>> ws_fiberform.matrix = ws_fiberform.matrix[50:150, 50:150, 50:150]
        >>> cond_map = puma.IsotropicConductivityMap()
        >>> cond_map.add_material((0, 89), 0.0257)
        >>> cond_map.add_material((90, 255), 12)
        >>> k_eff_x, P_x, q_x = puma.compute_electrical_conductivity(ws_fiberform, cond_map, 'x', 's', tolerance=1e-2, solver_type='cg')
        Approximate memory requirement for simulation...
    """
    return compute_thermal_conductivity(workspace, cond_map, direction, side_bc, prescribed_bc, tolerance, maxiter,
                                        solver_type, display_iter, print_matrices, method, matrix_free)


def export_conductivity_fields_vti(filepath, ws, T, q):
    export_vti(filepath, {"ws": ws, "T": T, "q": q})


def plot_conductivity_fields(workspace, T, q, show_cbar=True, show_edges=False, xy_view=False, rm_id=None):
    """ Plot the workspace according to the displacement and stress fields output by the elasticity functions
        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param u: displacement field
        :type u: numpy.ndarray
        :param s: direct stress field
        :type s: numpy.ndarray
        :param t: shear stress field
        :type t: numpy.ndarray
        :param show_cbar: show colorbar in each plot
        :type show_cbar: bool
        :param show_edges: show edges in mesh
        :type show_edges: bool
        :param xy_view: show plot aligned with xy plane
        :type xy_view: bool
        :param rm_id: remove a phase of the material from thresholded mesh
        :type rm_id: float or None
    """

    if isinstance(workspace, Workspace):
        ws = workspace.matrix.copy().astype(float)
    else:
        ws = workspace.copy().astype(float)

    T_cp = T.copy()
    q_cp = q.copy()
    if rm_id is not None:
        gas_mask = ws == rm_id
        T_cp[gas_mask] = np.NAN
        q_cp[gas_mask] = np.NAN

    grid = pv.UniformGrid()
    grid.dimensions = np.array(ws.shape) + 1
    grid['ws'] = ws.ravel(order='F')
    grid['T'] = T_cp.ravel(order='F')
    grid['|q|'] = np.linalg.norm(q_cp, axis=3).ravel(order='F')
    grid['qx'] = q_cp[:, :, :, 0].ravel(order='F')
    grid['qy'] = q_cp[:, :, :, 1].ravel(order='F')
    grid['qz'] = q_cp[:, :, :, 2].ravel(order='F')

    p = pv.Plotter(shape=(2, 3))

    plots = [['ws', 'T', '|q|'], ['qx', 'qy', 'qz']]
    for i in range(2):
        for j in range(3):
            p.subplot(i, j)

            f = grid.threshold(scalars=plots[i][j])

            p.add_mesh(f.copy(), scalars=plots[i][j], interpolate_before_map=False, show_edges=show_edges,
                       cmap='jet', show_scalar_bar=False, opacity=1)

            if show_cbar:
                p.add_scalar_bar(plots[i][j], interactive=False, vertical=True, color=(0, 0, 0), height=0.8, title_font_size=40)

            p.show_bounds(grid='front', location='outer', all_edges=True, color=(0, 0, 0))
            p.background_color = (255, 255, 255)
            p.add_axes(line_width=5, color=(0, 0, 0))

            if xy_view:
                p.view_xy()
    p.show()
