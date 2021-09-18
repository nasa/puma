import numpy as np
from skimage import measure
from pumapy.utilities.generic_checks import check_ws_cutoff


def identify_porespace(workspace, solid_cutoff):
    """ Identify the porespace

        :param workspace: domain
        :type workspace: Workspace
        :param solid_cutoff: specify the solid range to discard from pores identification
        :type solid_cutoff: tuple(int, int)
        :return: porespace marked as: 0 solid, 1 largest pore (likely open porosity), >1 other pores
        :rtype: ndarray

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.generate_random_spheres((200, 200, 200), diameter=20, porosity=0.8, allow_intersect=True)
        >>> ws.binarize_range((0, 128))  # invert material, i.e. consider spheres as pores
        >>> pores = puma.identify_porespace(ws, (1, 1))
        >>> puma.render_volume(pores, (1, pores.max()), solid_color=None, cmap='jet')
    """

    # error check
    check_ws_cutoff(workspace, solid_cutoff)
    ws = workspace.copy()

    ws.binarize_range(solid_cutoff)

    pore_labels = measure.label(ws.matrix, background=1)

    unique_pore_ids, unique_id_counts = np.unique(pore_labels[pore_labels != 0], return_counts=True)

    sorted_unique_pore_ids = unique_pore_ids[np.argsort(unique_id_counts)[::-1]]

    keyarray = np.arange(np.max(pore_labels) + 1)
    keyarray[sorted_unique_pore_ids] = unique_pore_ids

    return keyarray[pore_labels]


def fill_closed_pores(workspace, solid_cutoff, fill_value, return_pores=False):
    """ Identify the porespace and fill closed porosity

        :param workspace: domain
        :type workspace: Workspace
        :param solid_cutoff: specify the solid range to discard from pores identification
        :type solid_cutoff: tuple(int, int)
        :param fill_value: value to fill closed porosity with
        :type fill_value: int
        :param return_pores: specifies whether to return identified pores
        :type return_pores: bool, optional
        :return: filled workspace with IDs set as: 0 for void, 1 for solid, fill_value for 
            added filler material. In addition, if return_pores==True, then it also returns 
            the porespace marked as: 0 solid, 1 largest pore (likely open porosity), >1 other pores
        :rtype: Workspace

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.generate_random_spheres((200, 200, 200), diameter=20, porosity=0.8, allow_intersect=True)
        >>> ws.binarize_range((0, 128))  # invert material, i.e. consider spheres as pores
        >>> filled_ws, pores = puma.fill_closed_pores(ws, solid_cutoff=(1, 1), fill_value=2, return_pores=True)
        >>> puma.render_volume(pores, (1, pores.max()), solid_color=None, cmap='jet')
        >>> puma.render_volume(filled_ws)
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
