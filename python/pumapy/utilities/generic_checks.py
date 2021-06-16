from pumapy.utilities.workspace import Workspace


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
