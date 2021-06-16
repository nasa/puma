from paraview.simple import *
from paraview import numpy_support
import vtk
import numpy as np

def render_contour_multiphase(workspace, cutoffs, background = (1.,1.,1.)):
    """ Contour render for multi-phase materials using Paraview

    :param workspace: domain
    :type workspace: Workspace or ndarray
    :param cutoffs: array of size (n,2) where n is the number of materials. specifies the low and high cutoff ranges
    :type cutoffs: ndarray
    :param background: color of the background from [0., 0., 0.] (black) to [1., 1., 1.] (white)
    :type background: tuple(float, float, float), optional
    :return: None
    """
    background = (1,1,1)
    paraview.simple._DisableFirstRenderCameraReset()
    origin = (0, 0, 0)
    window_size = (1229, 814)

    for index in range(cutoffs.shape[0]):
        matrix = np.copy(workspace.matrix).astype(np.float32)
        average = (cutoffs[index,0] + cutoffs[index,1]) / 2.0
        mask = matrix > average
        matrix[mask] = 2 * average - matrix[mask]
        matrix[matrix < 0] = 0
        cutoff = cutoffs[index,0]

        vtkimage = vtk.vtkImageData()
        producer = TrivialProducer()
        view = GetActiveViewOrCreate('RenderView')

        matrix_buf = np.full([matrix.shape[0] + 2, matrix.shape[1] + 2, matrix.shape[2] + 2], -1e6)
        matrix_buf[1:-1, 1:-1, 1:-1] = matrix[:, :, :]
        vtktmp = numpy_support.numpy_to_vtk(matrix_buf.ravel(order='F'))
        vtkimage.GetCellData().SetScalars(vtktmp)
        vtkimage.SetDimensions((matrix_buf.shape[0] + 1, matrix_buf.shape[1] + 1, matrix_buf.shape[2] + 1))

        vtktmp.SetName('pumapy_data')
        vtkimage.SetSpacing([workspace.voxel_length] * 3)
        vtkimage.SetOrigin(origin)

        producer.GetClientSideObject().SetOutput(vtkimage)

        outline = Show(producer, view)
        outline.Representation = 'Outline'
        outline.AmbientColor = [1. - background[0], 1. - background[0], 1. - background[0]]

        filter = Contour(Input=producer)
        filter.ContourBy = 'pumapy_data'
        filter.Isosurfaces = cutoff

        display = Show(filter, view)
        display.Representation = "Surface"

    # Window settings
    view.ViewSize = window_size
    ResetCamera()
    view.CenterOfRotation = GetActiveCamera().GetFocalPoint()
    view.Background = background

    Interact()
    Hide(producer, view)
    Hide(filter, view)

    RemoveViewsAndLayouts()
