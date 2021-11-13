from pumapy.utilities.workspace import Workspace
from pumapy.utilities.isosurface import generate_isosurface
from pumapy.utilities.logger import print_warning
import pyvista as pv
import numpy as np
import string
import random


def render_volume(workspace, cutoff=None, solid_color=None, style='surface', origin=(0., 0., 0.),
                  window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=True,
                  show_axes=True, show_outline=True, cmap='gray', add_to_plot=None, notebook=False):
    """ Volume render using Pyvista Threshold filter

        :param workspace: domain
        :type workspace: Workspace or np.ndarray
        :param cutoff: specifying the values to render
        :type cutoff: (int, int) or None
        :param solid_color: if set to None (default), the material is colored by the matrix's values.
                            Otherwise, a solid color can be specified (e.g. for white (1., 1., 1.))
        :type solid_color: (float, float, float) or None
        :param style: specifying the representation style ('surface', 'edges', 'wireframe', 'points')
        :type style: string
        :param origin: origin of the data as
        :type origin: (float, float, float)
        :param window_size: with the popup window size
        :type window_size: (int, int)
        :param opacity: opacity of volume
        :type opacity: float
        :param background: color of the background from (0., 0., 0.) (black) to (1., 1., 1.) (white)
        :type: background: (float, float, float)
        :param show_grid: show the grid with the size of the sides
        :type show_grid: bool
        :param plot_directly: whether to return a Plotter object (to make further changes to it) or show the plot directly
        :type plot_directly: bool
        :param show_axes: show orientation axis in the bottom left corner
        :type show_axes: bool
        :param show_outline: show the bounding box outline of the domain
        :type show_outline: bool
        :param cmap: matplotlib colormap to use (overwritten by solid_color if specified)
        :type cmap: str
        :param add_to_plot: pass an already existing plotter object to add on top of this plot
        :type add_to_plot: pyvista.Plotter or None
        :param notebook: plotting interactively in a jupyter notebook (overwrites show_grid to False)
        :type notebook: bool
        :return: None is plot_directly is True, otherwise a plotter object
        :rtype: pyvista.Plotter or None

        :Example:
        >>> import pumapy as puma
        >>> ws_volume = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
        >>> puma.render_volume(ws_volume)
    """
    r = Renderer(existing_plot=add_to_plot, filter_type="threshold", workspace=workspace, cutoff=cutoff,
                 solid_color=solid_color, style=style, origin=origin, window_size=window_size, opacity=opacity,
                 background=background, show_grid=show_grid, plot_directly=plot_directly, show_axes=show_axes,
                 show_outline=show_outline, cmap=cmap, notebook=notebook)

    return r.render()


def render_contour(workspace, cutoff, solid_color=(1., 1., 1.), style='surface', origin=(0., 0., 0.),
                   window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=True,
                   show_axes=True, show_outline=True, add_to_plot=None, notebook=False):
    """ Contour render using Pyvista Contour filter

        :param workspace: domain
        :type workspace: Workspace or np.ndarray
        :param cutoff: specifying the values at which the isosurface is created
        :type cutoff: (int, int)
        :param solid_color: a solid color to color the surface (e.g. for white (1., 1., 1.))
        :type solid_color: (float, float, float)
        :param style: specifying the representation style ('surface', 'edges', 'wireframe', 'points')
        :type style: string
        :param origin: origin of the data as
        :type origin: (float, float, float)
        :param window_size: with the popup window size
        :type window_size: (int, int)
        :param opacity: opacity of contour
        :type opacity: float
        :param background: color of the background from (0., 0., 0.) (black) to (1., 1., 1.) (white)
        :type: background: (float, float, float)
        :param show_grid: show the grid with the size of the sides
        :type show_grid: bool
        :param plot_directly: whether to return a Plotter object (to make further changes to it) or show the plot directly
        :type plot_directly: bool
        :param show_axes: show orientation axis in the bottom left corner
        :type show_axes: bool
        :param show_outline: show the bounding box outline of the domain
        :type show_outline: bool
        :param add_to_plot: pass an already existing plotter object to add on top of this plot
        :type add_to_plot: pyvista.Plotter or None
        :param notebook: plotting interactively in a jupyter notebook (overwrites show_grid to False)
        :type notebook: bool
        :return: None is plot_directly is True, otherwise a plotter object
        :rtype: pyvista.Plotter or None

        :Example:
        >>> import pumapy as puma
        >>> ws_contour = puma.import_3Dtiff(puma.path_to_example_file("50_artfibers.tif"))
        >>> puma.render_contour(ws_contour, (128,255))
    """
    r = Renderer(existing_plot=add_to_plot, filter_type="contour", workspace=workspace, cutoff=cutoff,
                 solid_color=solid_color, style=style, origin=origin, window_size=window_size, opacity=opacity,
                 background=background, show_grid=show_grid, plot_directly=plot_directly, show_axes=show_axes,
                 show_outline=show_outline, notebook=notebook)

    return r.render()


def render_orientation(workspace, scale_factor=1., solid_color=None, style='surface', origin=(0., 0., 0.),
                       window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, cmap=None,
                       plot_directly=True, show_axes=True, show_outline=True, add_to_plot=None, notebook=False,
                       sampling=None):
    """ Orientation render using Pyvista Glyph filter

        :param workspace: domain
        :type workspace: Workspace or np.ndarray
        :param scale_factor: scale the arrows by a factor
        :type scale_factor: float
        :param solid_color: a solid color for the arrows. Deafult is None, which colors the vectors by their magnitude.
                            To color white, input set solid_color=(1., 1., 1.)
        :type solid_color: None or (float, float, float)
        :param style: specifying the representation style ('surface', 'edges', 'wireframe', 'points')
        :type style: string
        :param origin: origin of the data as
        :type origin: (float, float, float)
        :param window_size: with the popup window size
        :type window_size: (int, int)
        :param opacity: opacity of arrows
        :type opacity: float
        :param background: color of the background from (0., 0., 0.) (black) to (1., 1., 1.) (white)
        :type: background: (float, float, float)
        :param show_grid: show the grid with the size of the sides
        :type show_grid: bool
        :param cmap: matplotlib colormap to use (overwritten by solid_color if specified)
        :type cmap: str or None
        :param plot_directly: whether to return a Plotter object (to make further changes to it) or show the plot directly
        :type plot_directly: bool
        :param show_axes: show orientation axis in the bottom left corner
        :type show_axes: bool
        :param show_outline: show the bounding box outline of the domain
        :type show_outline: bool
        :param add_to_plot: pass an already existing plotter object to add on top of this plot
        :type add_to_plot: pyvista.Plotter or None
        :param notebook: plotting interactively in a jupyter notebook (overwrites show_grid to False)
        :type notebook: bool
        :param sampling: number of arrows to sample (None means plot an arrow at each voxel)
        :type sampling: int or None
        :return: None is plot_directly is True, otherwise a plotter object
        :rtype: pyvista.Plotter or None

        :Example:
        >>> import pumapy as puma
        >>> ws_orientation = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
        >>> puma.compute_orientation_st(ws_orientation, (90, 255))
        >>> puma.render_orientation(ws_orientation)
    """
    r = Renderer(existing_plot=add_to_plot, filter_type="glyph", workspace=workspace, solid_color=solid_color, style=style,
                 origin=origin, window_size=window_size, opacity=opacity, background=background, show_grid=show_grid,
                 plot_directly=plot_directly, show_axes=show_axes, show_outline=show_outline, cmap=cmap,
                 scale_factor=scale_factor, notebook=notebook, sampling=sampling)

    return r.render()


def render_warp(workspace, scale_factor=1., color_by='magnitude', style='surface', origin=(0., 0., 0.),
                window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, cmap='jet',
                plot_directly=True, show_axes=True, show_outline=True, add_to_plot=None, notebook=False):
    """ Warp a domain by an orientation field

        :param workspace: domain with both matrix and orientation defined
        :type workspace: Workspace
        :param scale_factor: scale the orientation by a factor
        :type scale_factor: float
        :param color_by: it can be 'magnitude', which colors warped domain by magnitude of vectors, or 'x', 'y' or 'z',
                         which colors it by the orientation component, or 'matrix', which colors it by the matrix phases
        :type color_by: str
        :param style: specifying the representation style ('surface', 'edges', 'wireframe', 'points')
        :type style: string
        :param origin: origin of the data as
        :type origin: (float, float, float)
        :param window_size: with the popup window size
        :type window_size: (int, int)
        :param opacity: opacity of arrows
        :type opacity: float
        :param background: color of the background from (0., 0., 0.) (black) to (1., 1., 1.) (white)
        :type: background: (float, float, float)
        :param show_grid: show the grid with the size of the sides
        :type show_grid: bool
        :param cmap: matplotlib colormap to use (overwritten by solid_color if specified)
        :type cmap: str
        :param plot_directly: whether to return a Plotter object (to make further changes to it) or show the plot directly
        :type plot_directly: bool
        :param show_axes: show orientation axis in the bottom left corner
        :type show_axes: bool
        :param show_outline: show the bounding box outline of the domain
        :type show_outline: bool
        :param add_to_plot: pass an already existing plotter object to add on top of this plot
        :type add_to_plot: pyvista.Plotter or None
        :param notebook: plotting interactively in a jupyter notebook (overwrites show_grid to False)
        :type notebook: bool
        :return: None is plot_directly is True, otherwise a plotter object
        :rtype: pyvista.Plotter or None

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.Workspace.from_shape_value((20, 25, 18), 1)
        >>> ws[ws.matrix.shape[0]//2:] = 2
        >>> elast_map = puma.ElasticityMap()
        >>> elast_map.add_isotropic_material((1, 1), 200, 0.3)
        >>> elast_map.add_isotropic_material((2, 2), 400, 0.1)
        >>> bc = puma.ElasticityBC(ws)
        >>> bc.dirichlet[0] = 0  # hold x -ve face
        >>> bc.dirichlet[-1, :, :, 0] = 10   # displace x +ve face by 1 in x direction
        >>> bc.dirichlet[-1, :, :, 1:] = 0  # hold x +ve face in y and z directions
        >>> ws.orientation, _, _ = puma.compute_stress_analysis(ws, elast_map, bc, side_bc='f', solver_type="direct")
        >>> puma.render_warp(ws, color_by='y', style='edges')
    """
    if not isinstance(workspace, Workspace):
        raise Exception('Input is not a pumapy.Workspace.')
    if color_by == "matrix":
        if (workspace.matrix.shape[0] != workspace.orientation.shape[0] or
                workspace.matrix.shape[1] != workspace.orientation.shape[1] or
                workspace.matrix.shape[2] != workspace.orientation.shape[2]):
            raise Exception('Workspace has to have matrix and orientation variables of the same shape.')

    r = Renderer(existing_plot=add_to_plot, filter_type="warp", workspace=workspace, solid_color=color_by, style=style,
                 origin=origin, window_size=window_size, opacity=opacity, background=background, show_grid=show_grid,
                 plot_directly=plot_directly, show_axes=show_axes, show_outline=show_outline, cmap=cmap,
                 scale_factor=scale_factor, notebook=notebook)

    return r.render()


def render_contour_multiphase(workspace, cutoffs, solid_colors=None, style='surface', origin=(0., 0., 0.),
                              window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True,
                              plot_directly=True, show_axes=True, show_outline=True, add_to_plot=None, notebook=False):
    """ Contour render for multi-phase materials using Pyvista

        :param workspace: domain
        :type workspace: Workspace or ndarray
        :param cutoffs: n cutoffs is the number of materials. specifies the low and high cutoff ranges
        :type cutoffs: tuple
        :param solid_colors: solid colors to color the different phases' surface e.g. for white ((1., 1., 1.), (0., 0., 0.), ...)
        :type solid_colors: tuple or None
        :param style: specifying the representation style ('surface', 'edges', 'wireframe', 'points')
        :type style: string
        :param origin: origin of the data as
        :type origin: (float, float, float)
        :param window_size: with the popup window size
        :type window_size: (int, int)
        :param opacity: opacity of contour
        :type opacity: float
        :param background: color of the background from (0., 0., 0.) (black) to (1., 1., 1.) (white)
        :type: background: (float, float, float)
        :param show_grid: show the grid with the size of the sides
        :type show_grid: bool
        :param plot_directly: whether to return a Plotter object (to make further changes to it) or show the plot directly
        :type plot_directly: bool
        :param show_axes: show orientation axis in the bottom left corner
        :type show_axes: bool
        :param show_outline: show the bounding box outline of the domain
        :type show_outline: bool
        :param add_to_plot: pass an already existing plotter object to add on top of this plot
        :type add_to_plot: pyvista.Plotter or None
        :param notebook: plotting interactively in a jupyter notebook (overwrites show_grid to False)
        :type notebook: bool
        :return: None is plot_directly is True, otherwise a plotter object
        :rtype: pyvista.Plotter or None

        :Example:
        >>> import pumapy as puma
        >>> ws_multiphase = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
        >>> puma.render_contour_multiphase(ws_multiphase, ((100, 150), (150, 255)))
    """

    if add_to_plot is None:
        p = pv.Plotter(notebook=notebook)
    else:
        p = add_to_plot
    if notebook:  # observed problems in rendering with grid in jupyter
        show_grid = False
    for i, cutoff in enumerate(cutoffs):
        if solid_colors is None or len(solid_colors) != len(cutoffs):
            solid_color = tuple(np.random.rand(3))
        else:
            solid_color = solid_colors[i]
        r = Renderer(existing_plot=p, filter_type="contour", workspace=workspace, cutoff=cutoff, solid_color=solid_color,
                     style=style, origin=origin, window_size=window_size, opacity=opacity, background=background,
                     show_grid=show_grid, plot_directly=False, show_axes=show_axes, show_outline=show_outline)

        p = r.render()
    if plot_directly:
        p.show()
    else:
        return p


class Renderer:
    def __init__(self, existing_plot=None, filter_type=None, workspace=None, cutoff=None, solid_color=None, style=None,
                 origin=None, window_size=None, opacity=None, background=None, show_grid=None, plot_directly=None,
                 show_axes=None, show_outline=None, cmap=None, scale_factor=None, notebook=None, sampling=None):
        self.filter_type = filter_type
        self.cutoff = cutoff
        self.solid_color = solid_color
        self.style = style
        self.origin = origin
        self.window_size = window_size
        self.opacity = opacity
        self.background = background
        self.show_grid = show_grid
        self.plot_directly = plot_directly
        self.show_axes = show_axes
        self.show_outline = show_outline
        self.cmap = cmap
        self.voxel_length = 1
        self.scale_factor = scale_factor

        if isinstance(workspace, Workspace):
            self.voxel_length = workspace.voxel_length

            if self.filter_type == "glyph":
                self.scale_factor *= self.voxel_length
                # reducing number of arrows by sampling
                if sampling is not None and sampling >= np.prod(workspace.orientation.shape[:3]):
                    print_warning("Sampling has to be less than the number of voxels. Setting it to None.")
                    sampling = None
                if sampling is not None:
                    mag = workspace.orientation_magnitude()
                    inds = np.where(mag > 0.5)
                    rand_choice = np.random.choice(inds[0].shape[0], sampling, replace=False)
                    inds = (inds[0][rand_choice], inds[1][rand_choice], inds[2][rand_choice])
                    self.array = np.zeros_like(workspace.orientation)
                    self.array[inds[0], inds[1], inds[2]] = workspace.orientation[inds[0], inds[1], inds[2]]
                else:
                    self.array = workspace.orientation

            elif self.filter_type == "warp":
                self.array = workspace.matrix
                self.orientation = workspace.orientation
                # self.scale_factor *= self.voxel_length
            else:
                self.array = workspace.matrix

        elif isinstance(workspace, np.ndarray):
            if self.filter_type == "glyph":
                if not (workspace.ndim == 4 and workspace.shape[3] == 3):
                    raise Exception("For rendering orientation, need a numpy.ndarray of shape (X, Y, Z, 3)")
            self.array = workspace
        else:
            raise Exception("Need to input either pumapy.Workspace or a numpy.ndarray")

        if self.filter_type != "contour" and self.filter_type != "warp":
            self.grid = pv.UniformGrid()
            self.grid.origin = self.origin
        elif self.filter_type == "warp":
            x = np.linspace(0, self.orientation.shape[0] - 1, self.orientation.shape[0])
            y = np.linspace(0, self.orientation.shape[1] - 1, self.orientation.shape[1])
            z = np.linspace(0, self.orientation.shape[2] - 1, self.orientation.shape[2])
            x, y, z = np.meshgrid(x, y, z, indexing='ij')
            self.grid = pv.StructuredGrid(x, y, z)

        self.filter = None
        if existing_plot is None:
            self.p = pv.Plotter(notebook=notebook)
        else:
            self.p = existing_plot

        if notebook:  # observed problems in rendering with grid in jupyter
            self.show_grid = False

    def render(self):
        self.build_plotter()

        if self.plot_directly:
            self.p.show()
        else:
            return self.p

    def build_plotter(self):
        if self.style == 'edges':
            show_edges = True
            self.style = 'surface'
        else:
            show_edges = False

        if self.filter_type == "threshold":
            self.grid.dimensions = np.array(self.array.shape) + 1
            self.grid.spacing = (self.voxel_length, self.voxel_length, self.voxel_length)
            self.grid.cell_data["values"] = self.array.flatten(order="F")
            self.filter = self.grid.threshold(self.cutoff)

        elif self.filter_type == "contour":
            tri_mesh = generate_isosurface(self.array, self.cutoff)
            tri_mesh.verts += self.origin
            tri_mesh.create_mesh()
            self.filter = tri_mesh.mesh

        elif self.filter_type == "glyph":
            self.grid.dimensions = np.array(self.array.shape[:3]) + 1
            self.grid.spacing = (self.voxel_length, self.voxel_length, self.voxel_length)
            tmp = np.zeros((self.array[:, :, :, 0].size, 3), dtype=float)
            for i in range(3):
                tmp[:, i] = self.array[:, :, :, i].ravel(order='F')
            self.grid.cell_data["scalars"] = np.linalg.norm(self.array, axis=3).ravel(order='F')
            self.grid.cell_data["vectors"] = tmp
            self.filter = self.grid.glyph(orient="vectors", scale="scalars", factor=self.scale_factor, geom=pv.Arrow())

        elif self.filter_type == "warp":
            tmp = np.zeros((self.orientation[:, :, :, 0].size, 3), dtype=float)
            for i in [0, 1, 2]:
                tmp[:, i] = self.orientation[:, :, :, i].ravel(order='F')
            self.grid['vectors'] = tmp
            if self.solid_color == 'magnitude':
                self.grid['scalars'] = np.linalg.norm(self.orientation, axis=3).ravel(order='F')
            elif self.solid_color == 'x':
                self.grid['scalars'] = self.orientation[:, :, :, 0].flatten(order="F")
            elif self.solid_color == 'y':
                self.grid['scalars'] = self.orientation[:, :, :, 1].flatten(order="F")
            elif self.solid_color == 'z':
                self.grid['scalars'] = self.orientation[:, :, :, 2].flatten(order="F")
            else:  # defaulting to matrix, even if not correct
                if self.solid_color != 'matrix':
                    print_warning("Options for color_by input are: 'magnitude', 'x', 'y', 'z', 'matrix'")
                self.grid['scalars'] = self.array.flatten(order="F")
            self.filter = self.grid.warp_by_vector('vectors', factor=self.scale_factor)

        if self.filter_type != "warp":
            self.p.add_mesh(self.filter, color=self.solid_color, show_edges=show_edges, style=self.style,
                            cmap=self.cmap, show_scalar_bar=False, opacity=self.opacity)
        else:
            self.p.add_mesh(self.filter, scalars='scalars', interpolate_before_map=False, show_edges=show_edges,
                            style=self.style, cmap=self.cmap, show_scalar_bar=False, opacity=self.opacity)

        # this is because warp uses solid_color to select what to color the domain by
        if self.solid_color is None or isinstance(self.solid_color, str):
            # the title has to be a unique string for each subplot
            self.p.add_scalar_bar(title="".join([random.choice(string.ascii_letters) for _ in range(10)]),
                                  title_font_size=1, height=0.5, vertical=True, position_x=0.05, position_y=0.3)

        self.p.background_color = self.background
        self.p.window_size = self.window_size
        if self.show_outline:
            self.p.add_bounding_box(color="k")
        if self.show_axes:
            self.p.show_axes()
        if self.show_grid:
            self.p.show_grid()
