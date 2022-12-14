"""
Functions to relying on Connected Component Labeling (CCL)
"""
import numpy as np
from skimage import measure
from pumapy.utilities.generic_checks import check_ws_cutoff


def identify_porespace(workspace, solid_cutoff, connectivity=None):
    """ Identify the porespace

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param solid_cutoff: specify the solid range to discard from pores identification
        :type solid_cutoff: (int, int)
        :param connectivity: Maximum number of orthogonal hops to consider a pixel/voxel as a neighbor
        (None automatically gives full connectivity of ``input.ndim``, which for 3D domain is 3, for 2D is 2)
        :type connectivity: int or None
        :return: porespace marked as: 0 solid, 1 largest pore (likely open porosity), >1 other pores
        :rtype: np.ndarray

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.generate_sphere((100, 100, 100), (50, 50, 50), 40, segmented=False)
        Generated in...
        >>> ws.binarize_range((1, 253))
        >>> # puma.render_volume(ws[:ws.matrix.shape[0] // 2], cutoff=(0, 255))  # to visualize workspace
        >>> pores = puma.identify_porespace(ws, (1, 1))
        >>> # puma.render_volume(pores[:pores.shape[0] // 2], cutoff=(0, pores.max()))  # to visualize pores
    """

    # error check
    check_ws_cutoff(workspace, solid_cutoff)
    ws = workspace.copy()

    # logging
    workspace.log.log_section("Identifying Porespace")
    workspace.log.log_value("Solid Cutoff: ", solid_cutoff)
    workspace.log.write_log()

    ws.binarize_range(solid_cutoff)

    # padding with solid to detect pores along the borders
    array = np.ones((ws.matrix.shape[0] + 2, ws.matrix.shape[1] + 2, ws.matrix.shape[2] + 2))
    array[1:-1, 1:-1, 1:-1] = ws.matrix

    pore_labels = measure.label(array, background=1, connectivity=connectivity)
    pore_labels = pore_labels[1:-1, 1:-1, 1:-1]
    unique_pore_ids, unique_id_counts = np.unique(pore_labels[pore_labels != 0], return_counts=True)
    sorted_unique_pore_ids = unique_pore_ids[np.argsort(unique_id_counts)[::-1]]
    keyarray = np.arange(np.max(pore_labels) + 1)
    keyarray[sorted_unique_pore_ids] = unique_pore_ids
    return keyarray[pore_labels]


def fill_closed_pores(workspace, solid_cutoff, fill_value, return_pores=False):
    """ Identify the porespace and fill closed porosity

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param solid_cutoff: specify the solid range to discard from pores identification
        :type solid_cutoff: (int, int)
        :param fill_value: value to fill closed porosity with
        :type fill_value: int
        :param return_pores: specifies whether to return identified pores
        :type return_pores: bool
        :return: filled workspace with IDs set as: 0 for void, 1 for solid, fill_value for 
            added filler material. In addition, if return_pores==True, then it also returns 
            the porespace marked as: 0 solid, 1 largest pore (likely open porosity), >1 other pores
        :rtype: pumapy.Workspace

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.generate_random_spheres((100, 100, 100), diameter=20, porosity=0.8, allow_intersect=True, segmented=False)
        Approximately ... spheres to be generated...
        >>> # puma.render_volume(ws[:ws.matrix.shape[0] // 2])  # to visualize workspace
        >>> ws.binarize_range((1, 250))
        >>> # puma.render_volume(ws[:ws.matrix.shape[0] // 2])
        >>> filled_ws, pores = puma.fill_closed_pores(ws, (1, 1), fill_value=2, return_pores=True)
        >>> # puma.render_volume(pores, cutoff=(0, pores.max()), cmap='jet')  # to visualize pores
    """

    pores = identify_porespace(workspace, solid_cutoff)
    if isinstance(pores, bool):
        return False

    ws = workspace.copy()
    ws.binarize_range(solid_cutoff)
    ws[np.where(pores > 1)] = np.uint16(fill_value)

    if return_pores:
        return ws, pores
    else:
        return ws
