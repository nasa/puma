from pumapy.utilities.workspace import Workspace
from pumapy.utilities.isosurface import generate_isosurface
import pyvista as pv
import numpy as np


def render_volume(workspace, cutoff=None, solid_color=(1., 1., 1.), style='surface', origin=(0., 0., 0.),
                  window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=True,
                  show_axes=True, show_outline=True, cmap='gray', add_to_plot=None, notebook=False):
    """ Volume render using Pyvista Threshold filter

    :param workspace: domain
    :type workspace: Workspace or ndarray
    :param cutoff: specifying the values to render
    :type cutoff: tuple(int, int), optional
    :param solid_color: if set to None, the material is colored by the matrix's values. Otherwise, a solid color can be specified (e.g. for white (1., 1., 1.))
    :type solid_color: tuple(float, float, float), optional
    :param style: specifying the representation style ('surface', 'edges', 'wireframe', 'points')
    :type style: string, optional
    :param origin: origin of the data as
    :type origin: tuple(float, float, float), optional
    :param window_size: with the popup window size
    :type window_size: tuple(int, int), optional
    :param opacity: opacity of volume
    :type opacity: float, optional
    :param background: color of the background from (0., 0., 0.) (black) to (1., 1., 1.) (white)
    :type: background: tuple(float, float, float)
    :param show_grid: show the grid with the size of the sides
    :type show_grid: bool, optional
    :param plot_directly: whether to return a Plotter object (to make further changes to it) or show the plot directly
    :type plot_directly: bool, optional
    :param show_axes: show orientation axis in the bottom left corner
    :type show_axes: bool, optional
    :param show_outline: show the bounding box outline of the domain
    :type show_outline: bool, optional
    :param cmap: matplotlib colormap to use (overwritten by solid_color if specified)
    :type cmap: str, optional
    :param add_to_plot: pass an already existing plotter object to add on top of this plot
    :type add_to_plot: pyvista.Plotter, optional
    :param notebook: plotting interactively in a jupyter notebook (overwrites show_grid to False)
    :type notebook: bool, optional
    :return: None is plot_directly is True, otherwise a plotter object
    :rtype: pyvista.Plotter object or None

    :Example
    >>> import pumapy as puma
    >>> ws_volume = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
    >>> puma.render_volume(ws_volume)
    """
    if cutoff is None:
        solid_color = None
    r = Renderer(add_to_plot, "threshold", workspace, cutoff, solid_color, style, origin, window_size, opacity,
                 background, show_grid, plot_directly, show_axes, show_outline, cmap, None, notebook)
    return r.render()


def render_contour(workspace, cutoff, solid_color=(1., 1., 1.), style='surface', origin=(0., 0., 0.),
                   window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True, plot_directly=True,
                   show_axes=True, show_outline=True, add_to_plot=None, notebook=False):
    """ Contour render using Pyvista Contour filter

    :param workspace: domain
    :type workspace: Workspace or ndarray
    :param cutoff: specifying the values at which the isosurface is created
    :type cutoff: tuple(int, int)
    :param solid_color: a solid color to color the surface (e.g. for white (1., 1., 1.))
    :type solid_color: tuple(float, float, float), optional
    :param style: specifying the representation style ('surface', 'edges', 'wireframe', 'points')
    :type style: string, optional
    :param origin: origin of the data as
    :type origin: tuple(float, float, float), optional
    :param window_size: with the popup window size
    :type window_size: tuple(int, int), optional
    :param opacity: opacity of contour
    :type opacity: float, optional
    :param background: color of the background from (0., 0., 0.) (black) to (1., 1., 1.) (white)
    :type: background: tuple(float, float, float)
    :param show_grid: show the grid with the size of the sides
    :type show_grid: bool, optional
    :param plot_directly: whether to return a Plotter object (to make further changes to it) or show the plot directly
    :type plot_directly: bool, optional
    :param show_axes: show orientation axis in the bottom left corner
    :type show_axes: bool, optional
    :param show_outline: show the bounding box outline of the domain
    :type show_outline: bool, optional
    :param add_to_plot: pass an already existing plotter object to add on top of this plot
    :type add_to_plot: pyvista.Plotter, optional
    :param notebook: plotting interactively in a jupyter notebook (overwrites show_grid to False)
    :type notebook: bool, optional
    :return: None is plot_directly is True, otherwise a plotter object
    :rtype: pyvista.Plotter object or None

    :Example:
    >>> import pumapy as puma
    >>> ws_contour = puma.import_3Dtiff(puma.path_to_example_file("50_artfibers.tif"))
    >>> puma.render_contour(ws_contour, (128,255))
    """
    r = Renderer(add_to_plot, "contour", workspace, cutoff, solid_color, style, origin, window_size, opacity,
                 background, show_grid, plot_directly, show_axes, show_outline, None, None, notebook)
    return r.render()


def render_orientation(workspace, scale_factor=1., solid_color=(1., 1., 1.), style='surface', origin=(0., 0., 0.),
                       window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True,
                       plot_directly=True, show_axes=True, show_outline=True, add_to_plot=None, notebook=False):
    """ Orientation render using Pyvista Glyph filter

    :param workspace: domain
    :type workspace: Workspace or ndarray
    :param scale_factor: scale the arrows by a factor
    :type scale_factor: float
    :param solid_color: a solid color to color the surface (e.g. for white (1., 1., 1.))
    :type solid_color: tuple(float, float, float), optional
    :param style: specifying the representation style ('surface', 'edges', 'wireframe', 'points')
    :type style: string, optional
    :param origin: origin of the data as
    :type origin: tuple(float, float, float), optional
    :param window_size: with the popup window size
    :type window_size: tuple(int, int), optional
    :param opacity: opacity of arrows
    :type opacity: float, optional
    :param background: color of the background from (0., 0., 0.) (black) to (1., 1., 1.) (white)
    :type: background: tuple(float, float, float)
    :param show_grid: show the grid with the size of the sides
    :type show_grid: bool, optional
    :param plot_directly: whether to return a Plotter object (to make further changes to it) or show the plot directly
    :type plot_directly: bool, optional
    :param show_axes: show orientation axis in the bottom left corner
    :type show_axes: bool, optional
    :param show_outline: show the bounding box outline of the domain
    :type show_outline: bool, optional
    :param add_to_plot: pass an already existing plotter object to add on top of this plot
    :type add_to_plot: pyvista.Plotter, optional
    :param notebook: plotting interactively in a jupyter notebook (overwrites show_grid to False)
    :type notebook: bool, optional
    :return: None is plot_directly is True, otherwise a plotter object
    :rtype: pyvista.Plotter object or None

    :Example:
    >>> import pumapy as puma
    >>> ws_orientation = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
    >>> puma.compute_orientation_st(ws_orientation, 0.7, 1.4, (90, 255))
    >>> puma.render_orientation(ws_orientation)
    """
    r = Renderer(add_to_plot, "glyph", workspace, None, solid_color, style, origin, window_size, opacity,
                 background, show_grid, plot_directly, show_axes, show_outline, None, scale_factor, notebook)
    return r.render()


def render_contour_multiphase(workspace, cutoffs, solid_colors=None, style='surface', origin=(0., 0., 0.),
                              window_size=(1920, 1200), opacity=1., background=(0.3, 0.3, 0.3), show_grid=True,
                              plot_directly=True, show_axes=True, show_outline=True, add_to_plot=None, notebook=False):
    """ Contour render for multi-phase materials using Pyvista

    :param workspace: domain
    :type workspace: Workspace or ndarray
    :param cutoffs: n cutoffs is the number of materials. specifies the low and high cutoff ranges
    :type cutoffs: tuple(tuple(int, int), tuple(int, int) ...)
    :param solid_colors: solid colors to color the different phases' surface e.g. for white ((1., 1., 1.), (0., 0., 0.), ...)
    :type solid_colors: tuple(tuple(float, float, float), tuple(float, float, float) ...), optional
    :param style: specifying the representation style ('surface', 'edges', 'wireframe', 'points')
    :type style: string, optional
    :param origin: origin of the data as
    :type origin: tuple(float, float, float), optional
    :param window_size: with the popup window size
    :type window_size: tuple(int, int), optional
    :param opacity: opacity of contour
    :type opacity: float, optional
    :param background: color of the background from (0., 0., 0.) (black) to (1., 1., 1.) (white)
    :type: background: tuple(float, float, float)
    :param show_grid: show the grid with the size of the sides
    :type show_grid: bool, optional
    :param plot_directly: whether to return a Plotter object (to make further changes to it) or show the plot directly
    :type plot_directly: bool, optional
    :param show_axes: show orientation axis in the bottom left corner
    :type show_axes: bool, optional
    :param show_outline: show the bounding box outline of the domain
    :type show_outline: bool, optional
    :param add_to_plot: pass an already existing plotter object to add on top of this plot
    :type add_to_plot: pyvista.Plotter, optional
    :param notebook: plotting interactively in a jupyter notebook (overwrites show_grid to False)
    :type notebook: bool, optional
    :return: None is plot_directly is True, otherwise a plotter object
    :rtype: pyvista.Plotter object or None

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
        r = Renderer(p, "contour", workspace, cutoff, solid_color, style, origin, window_size, opacity,
                     background, show_grid, False, show_axes, show_outline, None, None, None)
        p = r.render()
    if plot_directly:
        p.show()
    else:
        return p


class Renderer:
    def __init__(self, existing_plot, filter_type, workspace, cutoff, solid_color, style, origin, window_size, opacity,
                 background, show_grid, plot_directly, show_axes, show_outline, cmap, scale_factor, notebook):
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
        self.scale_factor = scale_factor

        if isinstance(workspace, Workspace):
            if self.filter_type == "glyph":
                self.array = workspace.orientation
            else:
                self.array = workspace.matrix
        elif isinstance(workspace, np.ndarray):
            if self.filter_type == "glyph":
                if not (workspace.ndim == 4 and workspace.shape[3] == 3):
                    raise Exception("For rendering orientation, need a numpy.ndarray of shape (X, Y, Z, 3)")
            self.array = workspace
        else:
            raise Exception("Need to input either pumapy.Workspace or a numpy.ndarray")

        if self.filter_type != "contour":
            self.grid = pv.UniformGrid()
            self.grid.origin = self.origin

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
            self.grid.cell_data["values"] = self.array.flatten(order="F")
            self.filter = self.grid.threshold(self.cutoff)

        elif self.filter_type == "contour":
            tri_mesh = generate_isosurface(self.array, self.cutoff)
            tri_mesh.verts += self.origin
            tri_mesh.create_mesh()
            self.filter = tri_mesh.mesh

        elif self.filter_type == "glyph":
            self.grid.dimensions = np.array(self.array.shape[:3]) + 1
            tmp = np.zeros((self.array[:, :, :, 0].size, 3), dtype=float)
            for i in range(3):
                tmp[:, i] = self.array[:, :, :, i].ravel(order='F')
            self.grid.cell_data["scalars"] = np.linalg.norm(self.array, axis=3).ravel(order='F')
            self.grid.cell_data["vectors"] = tmp
            self.filter = self.grid.glyph(orient="vectors", scale="scalars", factor=self.scale_factor, geom=pv.Arrow())

        self.p.add_mesh(self.filter, color=self.solid_color, show_edges=show_edges, style=self.style,
                        cmap=self.cmap, show_scalar_bar=False, opacity=self.opacity)

        if self.cmap is not None and self.solid_color is None:
            self.p.add_scalar_bar(title='', height=0.5, vertical=True, position_x=0.05, position_y=0.3)

        self.p.background_color = self.background
        self.p.window_size = self.window_size
        if self.show_outline:
            self.p.add_bounding_box(color="k")
        if self.show_axes:
            self.p.show_axes()
        if self.show_grid:
            self.p.show_grid()
