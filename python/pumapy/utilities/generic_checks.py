from pumapy.utilities.workspace import Workspace
import math
import sys
import numpy as np


def size_check(size):
    if not (isinstance(size, tuple) and len(size) == 3):
        raise Exception("Error, invalid size, must be tuple of size 3")
    if size[0] <= 0:
        raise Exception("Error, invalid size, must be >= 0")
    if size[1] <= 0:
        raise Exception("Error, invalid size, must be >= 0")
    if size[2] <= 0:
        raise Exception("Error, invalid size, must be >= 0")


def range_inc(var, var_range, var_name):
    if var < var_range[0] or var > var_range[1]:
        raise Exception("Error, invalid " + str(var_name) + ", must be between " + str(var_range[0]) + " and " + str(
            var_range[1]) + ", inclusive")


def range_exc(var, var_range, var_name):
    if var <= var_range[0] or var >= var_range[1]:
        raise Exception("Error, invalid " + str(var_name) + ", must be between " + str(var_range[0]) + " and " + str(
            var_range[1]) + ", exclusive")


def greater_than_inc(var, lim, var_name):
    if var < lim:
        raise Exception("Error, invalid " + str(var_name) + ", must be greater than " + str(lim) + ", inclusive")


def greater_than_exc(var, lim, var_name):
    if var <= lim:
        raise Exception("Error, invalid " + str(var_name) + ", must be greater than " + str(lim) + ", exclusive")


def less_than_inc(var, lim, var_name):
    if var > lim:
        raise Exception("Error, invalid " + str(var_name) + ", must be less than " + str(lim) + ", inclusive")


def less_than_exc(var, lim, var_name):
    if var >= lim:
        raise Exception("Error, invalid " + str(var_name) + ", must be less than " + str(lim) + ", exclusive")


def check_ws_cutoff(workspace, cutoff):
    if not isinstance(workspace, Workspace):
        raise Exception("Need to pass pumapy.Workspace.")
    if workspace.matrix.shape == [1, 1, 1]:
        raise Exception("Empty workspace")
    if len(cutoff) != 2:
        raise Exception("Cutoff should be of length 2. Ex: (128,255)")
    if min(cutoff[0], cutoff[1]) < 0:
        raise Exception("Invalid cutoff. Must be positive")
    if cutoff[0] > cutoff[1]:
        raise Exception("Invalid cutoff. cutoff[0] should be <= cutoff[1]")


def estimate_max_memory(material_property, workspace_shape, solver_type='bicgstab', need_to_orient=False,
                        permeability_solid_cutoff=(1,1)):
    """ Compute a rough estimate of the extra maximum memory required to run a specified material property

    """

    mat_properties = ['anisotropic_conductivity', 'isotropic_conductivity', 'tortuosity', 'elasticity', 'permeability',
                      'orientation', 'radiation']

    if material_property not in mat_properties:
        raise Exception(f"material_property input can only be one of the following types: {mat_properties}")

    def convert_bytes_size(size_bytes):
        if size_bytes == 0:
            return "0B"
        size_name = ("B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB")
        i = int(math.floor(math.log(size_bytes, 1024)))
        p = math.pow(1024, i)
        s = round(size_bytes / p, 2)
        return "%s %s" % (s, size_name[i])

    # sizes in bytes
    float_size = np.zeros(1, dtype=float).itemsize
    uint32_size = np.zeros(1, dtype=np.uint32).itemsize
    uint16_size = np.zeros(1, dtype=np.uint16).itemsize

    len_x, len_y, len_z = workspace_shape
    total_bytes = 0

    if material_property in mat_properties[:5]:

        if material_property in ["anisotropic_conductivity", "elasticity"]:

            # because of the use of padded domains
            len_x += 2
            len_y += 2
            len_z += 2

            if material_property == "anisotropic_conductivity":
                values_in_Amat_rows = 27
                dof = 1.  # degrees of freedom
            else:
                values_in_Amat_rows = 81
                dof = 3.

            len_xyz = len_x * len_y * len_z

            # Amat size (V, I, J)
            total_bytes += (dof * values_in_Amat_rows * len_xyz * float_size +
                           2 * dof * values_in_Amat_rows * len_xyz * uint32_size)

            # bvec size
            total_bytes += 2 * (dof * len_y * len_z * float_size + 2 * len_xyz * uint32_size)

            # ws_pad_size
            total_bytes += len_xyz * uint16_size

            # case dependent variables
            if solver_type != "direct":
                total_bytes += dof * len_xyz * float_size  # initial guess size
                total_bytes +=  dof * len_xyz * float_size + 2 * len_xyz * uint32_size  # preconditioner M size

            if need_to_orient:
                total_bytes += 3 * len_xyz * float_size  # orient_pad_size

            # empirical factor observed from memory used (possibly due to iterative solver)
            if material_property == "anisotropic_conductivity":
                total_bytes *= 3.
            else:
                total_bytes *= 6.

        elif material_property in ["isotropic_conductivity", "tortuosity"]:
            # Amat size (V, I, J)
            total_bytes += (((len_x - 2) * len_y * len_z * 7 + 2 * len_y * len_z) * float_size +
                            2 * ((len_x - 2) * len_y * len_z * 7 + 2 * len_y * len_z) * uint32_size)

            len_xyz = len_x * len_y * len_z

            # bvec size
            total_bytes += len_xyz * float_size

            # ws_pad_size
            total_bytes += len_xyz * uint16_size

            if solver_type != "direct":
                total_bytes += len_xyz * float_size  # initial guess size
                total_bytes += len_xyz * float_size + 2 * len_xyz * uint32_size  # preconditioner M size

            # empirical factor observed from memory used (possibly due to iterative solver)
            total_bytes *= 2.5

        elif material_property == "permeability":
            pass

    elif material_property == "orientation":
        pass

    elif material_property == "radiation":
        pass

    print(f"Memory requirement for conductivity simulation: {convert_bytes_size(total_bytes)}")
