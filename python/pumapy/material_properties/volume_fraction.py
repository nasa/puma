import numpy as np
from pumapy.utilities.generic_checks import check_ws_cutoff
from pumapy.utilities.workspace import Workspace

def compute_volume_fraction(workspace, cutoff, display=True):
    """ Compute the volume fraction

        :param workspace: domain
        :type workspace: pumapy.Workspace or np.ndarray
        :param cutoff: to binarize domain
        :type cutoff: (int, int)
        :param display: print result
        :type display: bool
        :return: volume fraction
        :rtype: float

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6) # import example file
        Importing ...
        >>> vf = puma.compute_volume_fraction(ws, cutoff=(90, 255)) # compute volume fraction
        Volume Fraction for cutoff ...
    """
    volume_fraction = VolumeFraction(workspace, cutoff, display)

    volume_fraction.error_check()

    volume_fraction.log_input()
    volume_fraction.compute()
    volume_fraction.log_output()

    return volume_fraction.vf


class VolumeFraction:

    def __init__(self, workspace, cutoff, display):
        self.workspace = workspace
        self.cutoff = cutoff
        self.vf = -1.
        self.display = display

    def compute(self):
        if isinstance(self.workspace, Workspace):
            mask = self.workspace.matrix >= self.cutoff[0]
            mask_high = self.workspace.matrix <= self.cutoff[1]
            mask = mask * mask_high
            self.vf = float(np.sum(mask)) / float(self.workspace.get_size())
            if self.display:
                print(f"Volume Fraction for cutoff {self.cutoff}: {self.vf}")
        elif isinstance(self.workspace, np.ndarray):
            mask = self.workspace >= self.cutoff[0]
            mask_high = self.workspace <= self.cutoff[1]
            mask = mask * mask_high
            self.vf = float(np.sum(mask)) / float(self.workspace.size)



    def error_check(self):
        if isinstance(self.workspace, Workspace):
            check_ws_cutoff(self.workspace, self.cutoff)
        elif isinstance(self.workspace, np.ndarray):
            if self.workspace.shape == [1, 1, 1]:
                raise Exception("Empty workspace")
            if len(self.cutoff) != 2:
                raise Exception("Cutoff should be of length 2. Ex: (128,255)")
            if min(self.cutoff[0], self.cutoff[1]) < 0:
                raise Exception("Invalid cutoff. Must be positive")
            if self.cutoff[0] > self.cutoff[1]:
                raise Exception("Invalid cutoff. cutoff[0] should be <= cutoff[1]")
        else:
            raise Exception("Data needs to be either a pumapy.Workspace or Numpy array")


    def log_input(self):
        if isinstance(self.workspace, Workspace):
            self.workspace.log.log_section("Computing Volume Fraction")
            self.workspace.log.log_line("Domain Size: " + str(self.workspace.get_size()))
            self.workspace.log.log_line("Cutoff: " + str(self.cutoff))
            self.workspace.log.write_log()

    def log_output(self):
        if isinstance(self.workspace, Workspace):
            self.workspace.log.log_section("Finished Volume Fraction Calculation")
            self.workspace.log.log_line("Volume Fraction: " + str(self.vf))
