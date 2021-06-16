import paraview.simple as pv
from paraview import numpy_support
import vtk
import numpy as np
import pumapy.utilities.workspace as ws


def render_volume(workspace, cutoff=None, color=False, representation='surface', origin=(0, 0, 0),
                  window_size=(1229, 814), background=(0.1, 0.1, 0.1)):
    """ Volume render using Paraview Threshold filter

    :param workspace: domain
    :type workspace: Workspace or ndarray
    :param cutoff: specifying the values to render
    :type cutoff: tuple(int, int), optional
    :param color: specifying whether to color it
    :type color: bool, optional
    :param representation: specifying the representation ('surface', 'edges', 'wireframe', 'points')
    :type representation: string, optional
    :param origin: origin of the data as
    :type origin: tuple(int, int, int), optional
    :param window_size: with the popup window size
    :type window_size: tuple(int, int), optional
    :param background: color of the background from [0., 0., 0.] (black) to [1., 1., 1.] (white)
    :type: background: tuple(float, float, float)
    :return: None
    """
    renderer = Renderer(workspace, "threshold", None, cutoff, color, representation, origin, window_size,
                        background, None, None, None, None)
    renderer.error_check()
    renderer.render()
    del renderer


def render_contour(workspace, threshold, color=False, origin=(0, 0, 0), window_size=(1229, 814),
                   background=(0.1, 0.1, 0.1)):
    """ Contour render using Paraview Contour filter

    :param workspace: domain
    :type workspace: Workspace or ndarray
    :param threshold: specifying the value(s) to contour
    :type threshold: int or tuple(int,int)
    :param color: specifying whether to color it
    :type color: bool, optional
    :param origin: origin of the data
    :type origin: tuple(int, int, int), optional
    :param window_size: with the popup window size
    :type window_size: tuple(int, int), optional
    :param background: color of the background from [0., 0., 0.] (black) to [1., 1., 1.] (white)
    :type background: tuple(float, float, float), optional
    :return: None
    """
    if isinstance(threshold,tuple):
        ws = workspace.copy()
        ws.matrix = np.copy(workspace.matrix).astype(np.float32)
        average = (threshold[0] + threshold[1]) / 2.0
        mask = ws.matrix > average
        ws.matrix[mask] = 2 * average - ws.matrix[mask]
        cutoff = (threshold[0], None)
        renderer = Renderer(ws, "contour", None, cutoff, color, 'surface', origin, window_size, background,
                            None, None, None, None)

    else:
        cutoff = (threshold, None)
        renderer = Renderer(workspace, "contour", None, cutoff, color, 'surface', origin, window_size, background,
                            None, None, None, None)


    renderer.error_check()
    renderer.render()
    del renderer



def render_orientation(workspace, arrowscale=None, samples=5000, origin=(0, 0, 0), window_size=(1229, 814),
                       background=(0.1, 0.1, 0.1)):
    """ Orientation render using Paraview Glyph filter

    :param workspace: domain
    :type workspace: Workspace or ndarray
    :param arrowscale: specifying the scale of the arrows
    :type arrowscale: int, optional
    :param samples: specifying the number of sampled points to plot arrows
    :type samples: int, optional
    :param origin: origin of the data
    :type origin: tuple(int, int, int), optional
    :param window_size: with the popup window size:
    :type window_size: tuple(int, int), optional
    :param background: color of the background from [0., 0., 0.] (black) to [1., 1., 1.] (white)
    :type background: tuple(float, float, float), optional
    :return: None
    """
    renderer = Renderer(workspace, "glyph", arrowscale, None, False, 'surface', origin, window_size,
                        background, samples, None, None, None)
    renderer.error_check()
    renderer.render()


def render_warp(workspace, displacement, cutoff, scale=None, stresses=None, color_by="magnitude",
                representation='surface', origin=(0, 0, 0), window_size=(1229, 814), background=(0.1, 0.1, 0.1)):
    """ Warping render using Paraview Warp filter

    :param workspace: domain
    :type workspace: Workspace or ndarray
    :param displacement: with the displacement field with dims (x, y, z, 3) (output of compute_elasticity)
    :type displacement: ndarray
    :param cutoff: specifying the values to render
    :type cutoff: tuple(int, int), optional
    :param scale: specifying the scale factor for warping
    :type scale: float, optional
    :param stresses: with the displacement field with dims (x, y, z, 3) (output of compute_elasticity)
        used to color the domain
    :type stresses: ndarray, optional
    :param color_by: specifying how to color it ('magnitude', 'x', 'y', 'z')
    :type color_by: string, optional
    :param representation: specifying the representation ('surface', 'edges', 'wireframe', 'points')
    :type representation: string, optional
    :param origin: origin of the data
    :type origin: tuple(int, int, int), optional
    :param window_size: with the popup window size
    :type window_size: tuple(int, int), optional
    :param background: color of the background from [0., 0., 0.] (black) to [1., 1., 1.] (white)
    :type background: tuple(float, float, float)
    :return: None
    """
    renderer = Renderer(workspace, "warp", scale, cutoff, True, representation, origin, window_size, background, None,
                        displacement, stresses, color_by)
    renderer.error_check()
    renderer.render()



class Renderer:

    def __init__(self, workspace, render_type, scale, cutoff, color, representation, origin, window_size, background,
                 sampled_points, displacement, stresses, color_by):
        if isinstance(workspace, ws.Workspace):
            self.matrix = workspace.matrix
            self.orientation = workspace.orientation
            self.voxel_length = workspace.voxel_length
        else:
            if render_type == "glyph":
                self.orientation = workspace
                self.matrix = None
            else:
                self.matrix = workspace
            self.voxel_length = 1
        self.shape = None
        self.render_type = render_type
        self.cutoff = cutoff
        self.origin = origin
        self.color = color
        self.representation = representation
        self.window_size = window_size
        self.background = background
        self.sampled_points = sampled_points
        self.disp = displacement
        if scale is None:
            if render_type == "glyph":
                self.scale = 1./(self.orientation.max()/4.)
            else:
                self.scale = 10
        else:
            self.scale = scale
        if self.render_type == "warp":
            self.scale *= self.voxel_length
        self.stresses = stresses
        self.color_by = color_by

        self.vtkimage = vtk.vtkImageData()
        self.producer = pv.TrivialProducer()
        self.view = pv.GetActiveViewOrCreate('RenderView')
        self.filter = None
        self.display = None

    def render(self):
        self.convert_numpy_to_vtk()
        self.setup_paraview()
        pv.Interact()
        pv.Hide(self.producer, self.view)
        pv.Hide(self.filter, self.view)

        pv.RemoveViewsAndLayouts()


    def convert_numpy_to_vtk(self):

        if self.render_type == "glyph":
            tmpdata = np.zeros((self.orientation[:, :, :, 0].size, 3), dtype=float)
            for i in range(3):
                tmpdata[:, i] = self.orientation[:, :, :, i].ravel(order='F')
            vtktmp = numpy_support.numpy_to_vtk(np.ascontiguousarray(tmpdata), array_type=vtk.VTK_FLOAT)
            vtktmp.SetNumberOfComponents(3)
            self.vtkimage.GetCellData().SetVectors(vtktmp)
            self.vtkimage.SetDimensions(self.shape)

        elif self.render_type == "contour":
            matrix_buf = np.full([self.matrix.shape[0] + 2, self.matrix.shape[1] + 2, self.matrix.shape[2] + 2], -1e6)
            matrix_buf[1:-1, 1:-1, 1:-1] = self.matrix[:, :, :]
            vtktmp = numpy_support.numpy_to_vtk(matrix_buf.ravel(order='F'))
            self.vtkimage.GetCellData().SetScalars(vtktmp)
            self.vtkimage.SetDimensions((matrix_buf.shape[0]+1, matrix_buf.shape[1]+1, matrix_buf.shape[2]+1))

        else:  # threshold or warp
            vtktmp = numpy_support.numpy_to_vtk(self.matrix.ravel(order='F'))
            self.vtkimage.GetCellData().SetScalars(vtktmp)
            self.vtkimage.SetDimensions(self.shape)

        if self.render_type == "warp":
            tmpdata = np.zeros((self.disp[:, :, :, 0].size, 3), dtype=float)
            for i in range(3):
                tmpdata[:, i] = self.disp[:, :, :, i].ravel(order='F')
            vtktmp_disp = numpy_support.numpy_to_vtk(np.ascontiguousarray(tmpdata), array_type=vtk.VTK_FLOAT)
            vtktmp_disp.SetNumberOfComponents(3)
            vtktmp_disp.SetName('disp')
            self.vtkimage.GetCellData().SetVectors(vtktmp_disp)

            if self.stresses is not None:
                for i in range(3):
                    tmpdata[:, i] = self.stresses[:, :, :, i].ravel(order='F')
                vtktmp_sigma = numpy_support.numpy_to_vtk(np.ascontiguousarray(tmpdata), array_type=vtk.VTK_FLOAT)
                vtktmp_sigma.SetNumberOfComponents(3)
                vtktmp_sigma.SetName('stress')
                self.vtkimage.GetCellData().AddArray(vtktmp_sigma)

        vtktmp.SetName('pumapy_data')
        self.vtkimage.SetSpacing([self.voxel_length] * 3)
        self.vtkimage.SetOrigin(self.origin)

    def setup_paraview(self):

        self.producer.GetClientSideObject().SetOutput(self.vtkimage)

        outline = pv.Show(self.producer, self.view)
        outline.Representation = 'Outline'
        outline.AmbientColor = [1. - self.background[0], 1. - self.background[0], 1. - self.background[0]]

        if self.render_type == "contour":
            self.filter = pv.Contour(Input=self.producer)
            self.filter.ContourBy = 'pumapy_data'
            self.filter.Isosurfaces = self.cutoff[0]

        elif self.render_type == "threshold":
            self.filter = pv.Threshold(Input=self.producer)
            if self.cutoff is not None:
                self.filter.ThresholdRange = self.cutoff
            else:
                self.filter.ThresholdRange = (self.matrix.min(), self.matrix.max())

        elif self.render_type == "glyph":
            self.filter = pv.Glyph(Input=self.producer, GlyphType='Arrow')
            self.filter.OrientationArray = ['CELLS', 'pumapy_data']
            self.filter.ScaleArray = ['CELLS', 'pumapy_data']
            self.filter.ScaleFactor = self.scale * self.voxel_length
            self.filter.VectorScaleMode = 'Scale by Magnitude'
            self.filter.MaximumNumberOfSamplePoints = self.sampled_points

        elif self.render_type == "warp":
            threshold = pv.Threshold(Input=self.producer)
            threshold.ThresholdRange = self.cutoff
            resample = pv.ResampleWithDataset(registrationName='ResampleWithDataset', SourceDataArrays=self.producer,
                                              DestinationMesh=threshold)
            self.filter = pv.WarpByVector(registrationName='WarpByVector1', Input=resample)
            self.filter.Vectors = ['POINTS', 'disp']
            self.filter.ScaleFactor = self.scale

        self.display = pv.Show(self.filter, self.view)
        self.display.Representation = self.representation

        if self.color:
            if self.render_type == "warp":
                if self.stresses is None:
                    pv.ColorBy(self.display, ('POINTS', 'disp', self.color_by))
                else:
                    pv.ColorBy(self.display, ('POINTS', 'stress', self.color_by))
            else:
                self.display.LookupTable = pv.MakeBlueToRedLT(self.matrix.min(), self.matrix.max())
                self.display.ColorArrayName = ("CELL_DATA", "pumapy_data")
            self.display.SetScalarBarVisibility(self.view, True)
        else:
            if self.render_type == "glyph":
                pass
            else:
                pv.ColorBy(self.display, None)

        # Window settings
        self.view.ViewSize = self.window_size
        pv.ResetCamera()
        self.view.CenterOfRotation = pv.GetActiveCamera().GetFocalPoint()
        self.view.Background = self.background

    def error_check(self):
        if self.matrix is not None:
            if self.matrix.ndim == 2:
                self.matrix = self.matrix[..., np.newaxis]
            if self.matrix.ndim == 3:
                self.shape = self.matrix.shape[0] + 1, self.matrix.shape[1] + 1, self.matrix.shape[2] + 1
            else:
                raise Exception("Input array has to be a Workspace or 3D Numpy array.")
        else:
            self.shape = self.orientation.shape[0] + 1, self.orientation.shape[1] + 1, self.orientation.shape[2] + 1

        if self.representation is not None:
            if self.representation == "surface":
                self.representation = 'Surface'
            elif self.representation == "edges":
                self.representation = 'Surface With Edges'
            elif self.representation == "wireframe":
                self.representation = 'Wireframe'
            elif self.representation == "points":
                self.representation = 'Points'
            else:
                raise Exception("Unrecognized representation, it can only be 'surface', 'edges', 'wireframe', 'points'")

        if self.color_by is not None:
            if self.color_by == "magnitude":
                self.color_by = 'Magnitude'
            elif self.color_by == "x":
                self.color_by = 'X'
            elif self.color_by == "y":
                self.color_by = 'Y'
            elif self.color_by == "z":
                self.color_by = 'Z'
            else:
                raise Exception("Unrecognized color_by, it can only be 'magnitude', 'x', 'y', 'z'")
