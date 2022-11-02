from pumapy.physicsmodels.mpsa_elasticity import Elasticity
from pumapy.physicsmodels.fe_elasticity import ElasticityFE
from pumapy.physicsmodels.property_maps import ElasticityMap
from pumapy.utilities.workspace import Workspace
from pumapy.segmentation.ccl import identify_porespace
import numpy as np
import pyvista as pv


def compute_elasticity(workspace, elast_map, direction, side_bc='p', tolerance=1e-5,
                       maxiter=100000, solver_type='bicgstab', display_iter=True,
                       method="fv", matrix_free=True):
    """ Compute the effective elasticity coefficient

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param elast_map: local elasticity of the constituents
        :type elast_map: pumapy.ElasticityMap
        :param direction: direction for solve ('x','y', 'z', 'yz', 'xz', 'xy')
        :type direction: string
        :param side_bc: side boundary conditions can be symmetric ('s') or periodic ('p')
        :type side_bc: string
        :param tolerance: tolerance for iterative solver
        :type: tolerance: float
        :param maxiter: maximum Iterations for solver
        :type maxiter: int
        :param solver_type: solver type, options: 'gmres' (default), 'bicgstab', 'direct'
        :type solver_type: string
        :param display_iter: display iterations and residual
        :type display_iter: bool
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
        Initializing and padding domains ...
    """
    if not isinstance(workspace, Workspace):
        raise Exception("Workspace must be a puma.Workspace.")
    if not isinstance(elast_map, ElasticityMap):
        raise Exception("elast_map has to be an ElasticityMap")

    if method == "fv":
        solver = Elasticity(workspace, elast_map, direction, side_bc, tolerance, maxiter, solver_type, display_iter)
    elif method == "fe":
        solver = ElasticityFE(workspace, elast_map, direction, tolerance, maxiter, solver_type, display_iter, matrix_free)
    else:
        raise Exception("method can only be set as 'fv' (i.e. MPSA finite volume) or 'fe' (i.e. Q1-Q1 EBE finite element)")

    solver.error_check()

    solver.log_input()
    solver.compute()
    solver.log_output()

    d = {'x': 'first', 'y': 'second', 'z': 'third', 'yz': 'fourth', 'xz': 'fifth', 'xy': 'sixth'}
    print(f'\nEffective elasticity tensor ({d[solver.direction]} column): \n{solver.Ceff}\n')
    return solver.Ceff, solver.u, solver.s, solver.t


def compute_stress_analysis(workspace, elast_map, prescribed_bc, side_bc='p', tolerance=1e-5,
                            maxiter=100000, solver_type='bicgstab', display_iter=True):
    """ Compute stress analysis

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param elast_map: local elasticity of the constituents
        :type elast_map: pumapy.ElasticityMap
        :param direction: direction for solve ('x','y', 'z', 'yz', 'xz', 'xy')
        :type direction: string
        :param side_bc: side boundary conditions can be symmetric ('s') or periodic ('p')
        :type side_bc: string
        :param tolerance: tolerance for iterative solver
        :type: tolerance: float
        :param maxiter: maximum Iterations for solver
        :type maxiter: int
        :param solver_type: solver type, options: 'gmres' (default), 'bicgstab', 'direct'
        :type solver_type: string
        :param display_iter: display iterations and residual
        :type display_iter: bool
        :return: elasticity, displacement field, direct stresses (sigma xx, yy, zz), shear stresses (tau yz, xz, xy)
        :rtype: (numpy.ndarray, numpy.ndarray, numpy.ndarray)

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.Workspace.from_shape_value((20, 22, 22), 1)
        >>> ws[ws.matrix.shape[0]//2:] = 2
        >>> ws[:, [0, -1]] = 0
        >>> ws[:, :, [0, -1]] = 0
        >>> elast_map = puma.ElasticityMap()
        >>> elast_map.add_isotropic_material((0, 0), 1e-5, 0.3)
        >>> elast_map.add_isotropic_material((1, 1), 200, 0.3)
        >>> elast_map.add_isotropic_material((2, 2), 400, 0.1)
        >>> bc = puma.ElasticityBC(ws)
        >>> bc.xfaces[0, :, :, 0] = 0
        >>> bc.xfaces[0, :, :, 1] = 0
        >>> bc.xfaces[0, :, :, 2] = 0
        >>> bc.xfaces[1, :, :, 0] = 0
        >>> bc.xfaces[1, :, :, 1] = 1
        >>> bc.xfaces[1, :, :, 2] = 0
        >>> u, s, t = puma.compute_stress_analysis(ws, elast_map, bc)
        >>> # puma.warp_elasticity_fields(ws[:, 1:-1, 1:-1], u[:, 1:-1, 1:-1], s[:, 1:-1, 1:-1], t[:, 1:-1, 1:-1], 20, show_original=0., show_edges=True)  # to visualize
    """
    if not isinstance(workspace, Workspace):
        raise Exception("Workspace must be a puma.Workspace.")
    if not isinstance(elast_map, ElasticityMap):
        raise Exception("elast_map has to be an ElasticityMap")

    solver = Elasticity(workspace, elast_map, '', side_bc, tolerance, maxiter, solver_type, display_iter, prescribed_bc)
    solver.error_check()

    solver.log_input()
    solver.compute()
    solver.log_output()
    return solver.u, solver.s, solver.t


def get_E_nu_from_elasticity(C):
    """ Compute Young's moduli E1, E2, E3 and Poisson's ratios nu12, nu23, nu31 from symmetric elastic stiffness tensor
        :param C: 6x6 elasticity tensor
        :type C: np.ndarray
        :return: Young's moduli E1, E2, E3 and Poisson's ratios nu12, nu23, nu31
        :rtype: (float, float, float, float, float, float)
    """
    compliance = np.linalg.inv(C)
    E1 = 1. / compliance[0, 0]
    E2 = 1. / compliance[1, 1]
    E3 = 1. / compliance[2, 2]
    G23 = 1. / compliance[3, 3]
    G13 = 1. / compliance[4, 4]
    G12 = 1. / compliance[5, 5]
    nu23 = - E2 * compliance[2, 1]
    nu13 = - E1 * compliance[2, 0]
    nu12 = - E1 * compliance[1, 0]
    coeffs = [E1, E2, E3, G23, G13, G12, nu23, nu13, nu12]
    [print(i, j) for i, j in zip(["E1", "E2", "E3", "G23", "G13", "G12", "nu23", "nu13", "nu12"], coeffs)]
    return coeffs


def warp_elasticity_fields(workspace, u, s, t, scale_factor=1, show_original=0., show_cbar=True, show_edges=False,
                           xy_view=False, rm_id=None, show_axes=True):
    """ Warp the workspace according to the displacement field output by the elasticity functions,
        and color by displacement and stress components
        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param u: displacement field
        :type u: numpy.ndarray
        :param s: direct stress field
        :type s: numpy.ndarray
        :param t: shear stress field
        :type t: numpy.ndarray
        :param scale_factor: scaling factor for warp
        :type scale_factor: float
        :param show_original: opacity of the original workspace before warp
        :type show_original: float
        :param show_cbar: show colorbar in each plot
        :type show_cbar: bool
        :param show_edges: show edges in mesh
        :type show_edges: bool
        :param xy_view: show plot aligned with xy plane
        :type xy_view: bool
        :param rm_id: remove a phase of the material from warped mesh (only works for 2D slice)
        :type rm_id: float or None
        :param show_axes: show the axes and side dimensions
        :type show_axes: float
    """

    if isinstance(workspace, Workspace):
        ws = workspace.matrix.copy().astype(float)
    else:
        ws = workspace.copy().astype(float)

    orientation = u.copy()

    if rm_id is not None:
        gas_mask = ws == rm_id
        orientation[gas_mask] = np.NAN

    x = np.linspace(0, orientation.shape[0] - 1, orientation.shape[0])
    y = np.linspace(0, orientation.shape[1] - 1, orientation.shape[1])
    z = np.linspace(0, orientation.shape[2] - 1, orientation.shape[2])
    x, y, z = np.meshgrid(x, y, z, indexing='ij')
    grid = pv.StructuredGrid(x, y, z)

    tmp = np.zeros((orientation[:, :, :, 0].size, 3), dtype=float)
    for i in [0, 1, 2]:
        tmp[:, i] = orientation[:, :, :, i].ravel(order='F')

    grid['vectors'] = tmp
    grid['ux'] = u[:, :, :, 0].ravel(order='F')
    grid['uy'] = u[:, :, :, 1].ravel(order='F')
    grid['uz'] = u[:, :, :, 2].ravel(order='F')
    grid['sx'] = s[:, :, :, 0].ravel(order='F')
    grid['sy'] = s[:, :, :, 1].ravel(order='F')
    grid['sz'] = s[:, :, :, 2].ravel(order='F')
    grid['tyz'] = t[:, :, :, 0].ravel(order='F')
    grid['txz'] = t[:, :, :, 1].ravel(order='F')
    grid['txy'] = t[:, :, :, 2].ravel(order='F')

    f = grid.warp_by_vector('vectors', factor=scale_factor)

    p = pv.Plotter(shape=(3, 3))

    plots = [['ux', 'uy', 'uz'], ['sx', 'sy', 'sz'], ['tyz', 'txz', 'txy']]
    for i in range(3):
        for j in range(3):
            p.subplot(i, j)
            p.add_mesh(f.copy(), scalars=plots[i][j], interpolate_before_map=False, show_edges=show_edges,
                       cmap='jet', show_scalar_bar=False, opacity=1)

            if show_cbar:
                p.add_scalar_bar(plots[i][j], interactive=False, vertical=True, color=(0, 0, 0), height=0.8)

            if show_original > 0:
                grid2 = pv.UniformGrid()
                grid2.origin = (0.5, 0.5, 0.5)
                grid2.dimensions = np.array(ws.shape) + 1
                grid2["values"] = ws.flatten(order="F")
                f2 = grid2.threshold((0, ws.max()))
                p.add_mesh(f2, opacity=show_original, cmap='jet')

            if show_axes:
                p.show_bounds(grid='front', location='outer', all_edges=True, color=(0, 0, 0))
            p.background_color = (1, 1, 1)
            p.add_axes(line_width=5, color=(0, 0, 0))

            if xy_view:
                p.view_xy()
    p.show()


def plot_elasticity_fields(workspace, u, s, t, show_cbar=True, show_edges=False,
                           xy_view=False, rm_id=None):
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

    u_cp = u.copy()
    s_cp = s.copy()
    t_cp = t.copy()
    if rm_id is not None:
        gas_mask = ws == rm_id
        u_cp[gas_mask] = np.NAN
        s_cp[gas_mask] = np.NAN
        t_cp[gas_mask] = np.NAN

    grid = pv.UniformGrid()
    grid.dimensions = np.array(ws.shape) + 1
    grid['ux'] = u_cp[:, :, :, 0].ravel(order='F')
    grid['uy'] = u_cp[:, :, :, 1].ravel(order='F')
    grid['uz'] = u_cp[:, :, :, 2].ravel(order='F')
    grid['sx'] = s_cp[:, :, :, 0].ravel(order='F')
    grid['sy'] = s_cp[:, :, :, 1].ravel(order='F')
    grid['sz'] = s_cp[:, :, :, 2].ravel(order='F')
    grid['tyz'] = t_cp[:, :, :, 0].ravel(order='F')
    grid['txz'] = t_cp[:, :, :, 1].ravel(order='F')
    grid['txy'] = t_cp[:, :, :, 2].ravel(order='F')

    p = pv.Plotter(shape=(3, 3))

    plots = [['ux', 'uy', 'uz'], ['sx', 'sy', 'sz'], ['tyz', 'txz', 'txy']]
    for i in range(3):
        for j in range(3):
            p.subplot(i, j)

            f = grid.threshold(scalars=plots[i][j])

            p.add_mesh(f.copy(), scalars=plots[i][j], interpolate_before_map=False, show_edges=show_edges,
                       cmap='jet', show_scalar_bar=False, opacity=1)

            if show_cbar:
                p.add_scalar_bar(plots[i][j], interactive=False, vertical=True, color=(0, 0, 0), height=0.8)

            p.show_bounds(grid='front', location='outer', all_edges=True, color=(0, 0, 0))
            p.background_color = (255, 255, 255)
            p.add_axes(line_width=5, color=(0, 0, 0))

            if xy_view:
                p.view_xy()
    p.show()


def remove_rbms(workspace, void_id, direction):
    """ Rigid Body Movements (RBMs) removal in segmented domain

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param void_id: specify the void ID to discard from RBMs identification
        :type void_id: int
        :param direction: Cartesian direction that has to be connected, options: 'x', 'y', 'z'
        :type direction: str
        :return: workspace without the possible RBMs determined by not being connected from side to side
                 N.B. The output workspace is segmented into 0=void, 1=solid
        :rtype: pumapy.Workspace

        :Example:
        >>> import pumapy as puma
        >>> workspace = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"))
        Importing ...
        >>> workspace.binarize(103)
        >>> new_ws = puma.remove_rbms(workspace, void_id=0, direction='y')
        >>> # puma.render_volume(workspace, (1, 1), solid_color=(255, 255, 255))  # to visualize before and after
        >>> # puma.render_volume(new_ws, (1, new_ws.max()), solid_color=(255, 255, 255))
    """

    if direction not in ['x', 'y', 'z']:
        raise Exception("direction input can only be 'x', 'y', 'z'")

    solid = identify_porespace(workspace, (void_id, void_id), connectivity=1)
    uniques = np.unique(solid)
    if uniques[0] == 0:
        uniques = uniques[1:]

    supported_solid = Workspace.from_array(np.full_like(solid, void_id))
    supported_solid.voxel_length = workspace.voxel_length

    if workspace.orientation.shape[:-1] == workspace.matrix.shape:
        has_orientation = True
        supported_solid.create_orientation()
    else:
        has_orientation = False

    # only pass the phases connected from side to side
    for unique in uniques:
        mask = solid == unique
        if ((direction == 'x' and np.any(mask[0]) and np.any(mask[-1])) or
                (direction == 'y' and np.any(mask[:, 0]) and np.any(mask[:, -1])) or
                (direction == 'z' and np.any(mask[:, :, 0]) and np.any(mask[:, :, -1]))):
            supported_solid[mask] = unique
            if has_orientation:
                supported_solid.orientation[mask] = workspace.orientation[mask]

    supported_solid[supported_solid.matrix > 0] = 1
    return supported_solid
