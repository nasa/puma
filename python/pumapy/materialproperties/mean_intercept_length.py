from pumapy.utilities.generic_checks import check_ws_cutoff
from pumapy.utilities.logger import print_warning
import numpy as np


def compute_mean_intercept_length(workspace, void_cutoff):
    """ Computation of the mean intercept length

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param void_cutoff: specify the void or gaseous phase of the domain
        :type void_cutoff: (int, int)
        :return: mean intercept length in x,y,z
        :rtype: (float, float, float)

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
        >>> mil = puma.compute_mean_intercept_length(ws, (0, 89))
        >>> print(mil)
    """

    check_ws_cutoff(workspace, void_cutoff)
    workspace.log.log_section("Computing mean intercept length")
    workspace.log.log_line("Domain Size: " + str(workspace.get_shape()))
    workspace.log.log_line("Void cutoff: " + str(void_cutoff))

    ws = workspace.copy()
    ws.binarize_range(void_cutoff)
    void_voxs = ws.matrix.sum() * ws.voxel_length

    ws.matrix = ws.matrix.astype(int)

    if void_voxs > 0:
        collisions_x = np.count_nonzero(ws.matrix[:-1] - ws.matrix[1:] > 0)
        collisions_y = np.count_nonzero(ws.matrix[:, :-1] - ws.matrix[:, 1:] > 0)
        collisions_z = np.count_nonzero(ws.matrix[:, :, :-1] - ws.matrix[:, :, 1:] > 0)

        mil = []

        if collisions_x == 0:
            print_warning("Infinite Mean Intercept Length in x direction")
            mil.append(float('Inf'))
        else:
            mil.append(void_voxs / collisions_x)
        if collisions_y == 0:
            print_warning("Infinite Mean Intercept Length in y direction")
            mil.append(float('Inf'))
        else:
            mil.append(void_voxs / collisions_y)
        if collisions_z == 0:
            print_warning("Infinite Mean Intercept Length in z direction")
            mil.append(float('Inf'))
        else:
            mil.append(void_voxs / collisions_z)
    else:
        print_warning("Domain is fully dense.")
        mil = 0, 0, 0

    workspace.log.log_line("Mean Intercept Length: " + str(mil))
    workspace.log.write_log()
    return tuple(mil)
