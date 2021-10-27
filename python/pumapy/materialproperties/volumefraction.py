import numpy as np
from pumapy.utilities.generic_checks import check_ws_cutoff


def compute_volume_fraction(workspace, cutoff):
    """ Compute the volume fraction

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param cutoff: to binarize domain
        :type cutoff: (int, int)
        :return: volume fraction
        :rtype: float

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6) # import example file
        >>> vf = puma.compute_volume_fraction(ws, cutoff=(90, 255)) # compute volume fraction
    """
    volume_fraction = VolumeFraction(workspace, cutoff)

    volume_fraction.error_check()

    volume_fraction.log_input()
    volume_fraction.compute()
    volume_fraction.log_output()

    return volume_fraction.vf


class VolumeFraction:

    def __init__(self, workspace, cutoff):
        self.workspace = workspace
        self.cutoff = cutoff
        self.vf = -1.

    def compute(self):
        mask = self.workspace.matrix >= self.cutoff[0]
        mask_high = self.workspace.matrix <= self.cutoff[1]
        mask = mask * mask_high
        self.vf = float(np.sum(mask)) / float(self.workspace.get_size())
        print(f"Volume Fraction for cutoff {self.cutoff}: {self.vf}")

    def error_check(self):
        check_ws_cutoff(self.workspace, self.cutoff)

    def log_input(self):
        self.workspace.log.log_section("Computing Volume Fraction")
        self.workspace.log.log_line("Domain Size: " + str(self.workspace.get_shape()))
        self.workspace.log.log_line("Cutoff: " + str(self.cutoff))
        self.workspace.log.write_log()

    def log_output(self):
        self.workspace.log.log_section("Finished Volume Fraction Calculation")
        self.workspace.log.log_line("Volume Fraction: " + str(self.vf))
