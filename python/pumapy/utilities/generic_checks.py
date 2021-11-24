from pumapy.utilities.workspace import Workspace
import math
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


def estimate_max_memory(material_property, workspace_shape, solver_type='iterative', need_to_orient=False,
                        permeability_fluid_vf=1.):
    """ Compute an estimate of the extra maximum memory required to run a specified material property on a domain

        :param material_property: property to estimate, options:
        'anisotropic_conductivity', 'isotropic_conductivity', 'tortuosity', 'elasticity', 'permeability'
        :type material_property: string
        :param workspace_shape: size of the domain to compute
        :type workspace_shape: (int, int, int)
        :param solver_type: type of solver, options: 'direct', 'iterative'
        :type solver_type: string
        :param need_to_orient: domain with orientation (needed for anisotropic conductivity and elasticity)
        :type need_to_orient: bool
        :param permeability_fluid_vf: volume fraction of the fluid (needed for permeability)
        :type permeability_fluid_vf: float
        :return: number of Bytes
        :rtype: int
    """

    # missing properties: 'orientation', 'radiation'
    mat_properties = ['anisotropic_conductivity', 'isotropic_conductivity', 'tortuosity', 'elasticity', 'permeability']

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

    # General idea: peak memory should be reached at Amat sparse matrix creation
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
        A_rows = dof * values_in_Amat_rows * len_xyz
        total_bytes += 2 * (A_rows * float_size + 2 * A_rows * uint32_size)

        # bvec size
        bvec = dof * len_y * len_z * float_size + 2 * len_xyz * uint32_size
        total_bytes += bvec

        # ws_pad_size
        total_bytes += len_xyz * uint16_size

        # case dependent variables
        if solver_type != "direct":
            total_bytes += dof * len_xyz * float_size  # initial guess size

        if need_to_orient:
            total_bytes += 3 * len_xyz * float_size  # orient_pad_size

    elif material_property in ["isotropic_conductivity", "tortuosity"]:

        # Amat size (V, I, J)
        A_rows = (len_x - 2) * len_y * len_z * 7 + 2 * len_y * len_z
        total_bytes += 2 * (A_rows * float_size + 2 * A_rows * uint32_size)

        len_xyz = len_x * len_y * len_z

        # bvec size
        total_bytes += len_xyz * float_size

        # ws_pad_size
        total_bytes += len_xyz * uint16_size

        if solver_type != "direct":
            total_bytes += len_xyz * float_size  # initial guess size

    elif material_property == "permeability":

        # Amat size (V, I, J)
        ndofs = permeability_fluid_vf * len_x * len_y * len_z
        A_rows = ndofs * 24 * 24 + ndofs * 24 * 8 + ndofs * 192 + ndofs * 8 * 8
        total_bytes += 2 * (A_rows * float_size + 2 * A_rows * uint32_size)

        # bvec
        total_bytes += 3 * (4 * ndofs * float_size + 2 * 4 * ndofs * uint32_size)

        # mgdlF and resolveF
        total_bytes += 32 * ndofs
        total_bytes += 3.5 * ndofs  # rough

    # elif material_property == "orientation":
    #     pass
    #
    # elif material_property == "radiation":
    #     pass

    print(f"Memory requirement for conductivity simulation: {convert_bytes_size(total_bytes)}")
    return total_bytes


def set_random_seed(seed):
    """ Set random seed for scipy and numpy to make results reproducible.
        NB: if you want to generate the same material twice in the same process, you need to call it twice (see example)

        :param seed: random seed
        :type seed: int

        :Example:
        >>> import pumapy as puma
        >>> puma.set_random_seed(1)
        >>> ws = puma.generate_random_spheres((100, 100, 100), 20, 0.5, allow_intersect=True, segmented=False)
        >>> puma.render_volume(ws)
        >>> puma.set_random_seed(1)  # need to call it again to get the same domain!
        >>> ws = puma.generate_random_spheres((100, 100, 100), 20, 0.5, allow_intersect=True, segmented=False)
        >>> puma.render_volume(ws)
    """
    np.random.seed(seed)
