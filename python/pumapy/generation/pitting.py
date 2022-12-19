from pumapy.utilities.workspace import Workspace
from pumapy.utilities.timer import Timer
from pumapy.material_properties.volume_fraction import compute_volume_fraction
import numpy as np
from pumapy.utilities.logger import print_warning
import random
import sys


def generate_pitting(workspace, surface_threshold, mean_radius, deviation_radius, volume_fraction_removed, max_pits=10000):
    """ Generation of pitting into existing workspace

        :param workspace: pumapy workspace to add pitting to
        :type workspace: pumapy.Workspace
        :param surface_threshold: threshold value at which the solid starts. Void phase is contained in (0,surface_threshold-1)
        :type surface_threshold: int
        :param mean_radius: mean radius of the pits
        :type mean_radius: float
        :param deviation_radius: deviation in the radius of the pits (plus or minus)
        :type deviation_radius: float
        :param volume_fraction_removed: volume fraction to be removed by pits
        :type volume_fraction_removed: float
        :param max_pits: maximum number of pits. To ensure while(true) condition doesn't occur if specified volume_fraction can't be reached
        :type max_pits: int
        :return: None
        :rtype: None

        :Example:
        >>> import pumapy as puma
        >>> shape = (200, 200, 200)  # size of the domain, in voxels.
        >>> w = 0.08  # value of w in the equations above
        >>> q = 0.2  # value of q in the equations above
        >>> ws_eq0 = puma.generate_tpms(shape, w, q, equation=0, segmented=False)
        Generating TPMS ...
        >>> surface_threshold = 128 # threshold value at which the solid starts. Void phase is contained in (0,surface_threshold-1)
        >>> mean_radius = 4 # Mean radius of the pits, in voxels
        >>> deviation_radius = 3 # Deviation of the pits, in voxels
        >>> volume_fraction_removed = 0.01 # Volume fraction of the pits to be removed
        >>> puma.experimental.generate_pitting(ws_eq0, surface_threshold, mean_radius, deviation_radius, volume_fraction_removed)
        Volume Fraction for cutoff ...
        >>> # puma.render_contour(ws_eq0, (128,255))  # to visualize it
    """

    pitter = GeneratorPits(workspace, surface_threshold, mean_radius, deviation_radius, volume_fraction_removed, max_pits)

    pitter.error_check()

    pitter.log_input()
    pitter.generate()
    pitter.log_output()


class GeneratorPits:
    def __init__(self, workspace, surface_threshold, mean_radius, deviation_radius, volume_fraction_removed, max_pits):
        self._workspace = workspace
        self._surface_threshold = surface_threshold
        self._mean_radius = mean_radius
        self._deviation_radius = deviation_radius
        self._volume_fraction_removed = volume_fraction_removed
        self._max_pits = max_pits

    def generate(self):
        t = Timer()

        # creating a buffer around the domain
        buflength = (self._mean_radius + self._deviation_radius) * 2 + 1 # size of buffer around the domain
        new_x_length = self._workspace.matrix.shape[0] + 2 * buflength
        new_y_length = self._workspace.matrix.shape[1] + 2 * buflength
        new_z_length = self._workspace.matrix.shape[2] + 2 * buflength
        mat_copy = np.zeros((new_x_length,new_y_length,new_z_length))
        mat_copy[buflength:new_x_length-buflength,buflength:new_y_length-buflength,buflength:new_z_length-buflength] = self._workspace.matrix

        ## Determining all of the surface voxels
        tmp1 = np.where(mat_copy < self._surface_threshold, 0, 1)
        sums = (np.roll(tmp1, shift=1, axis=0) + np.roll(tmp1, shift=-1, axis=0) +
                np.roll(tmp1, shift=1, axis=1) + np.roll(tmp1, shift=-1, axis=1) +
                np.roll(tmp1, shift=1, axis=2) + np.roll(tmp1, shift=-1, axis=2))
        mask=np.zeros(sums.shape)
        mask[sums < 6] = 1
        mask[tmp1 == 0] = 0

        mask[[buflength, new_x_length - buflength - 1], :, :] = 0
        mask[:, [buflength, new_y_length - buflength - 1], :] = 0
        mask[:, :, [buflength, new_z_length - buflength - 1]] = 0

        indices = np.argwhere(mask == 1)

        print(indices.shape)

        if indices.size == 0:
            print_warning("No internal surfaces found. Please check threshold value")
            return

        # Determining what volume fraction we want to target in the while loop
        vf_previous = compute_volume_fraction(self._workspace, (0, self._surface_threshold-1))
        vf = vf_previous
        vf_target = vf_previous + self._volume_fraction_removed

        my_iteration = 0

        while vf < vf_target: # Each iteration here is one pit

            # choosing a random surface voxel
            random_index = indices[random.randint(0,indices.shape[0]-1)]

            # calculate the gradient - outward facing normal
            # grad phi / | grad phi | - outward facing normal

            # choose a random radius based on the deviation given
            # change this to a pdf sampling if you want
            radius = self._mean_radius + random.randint(-self._deviation_radius, self._deviation_radius)

            # creating a sphere
            shape_val = int(radius * 2 + 2)
            if shape_val % 2 == 0:
                shape_val += 1

            half_shape_val = int(shape_val/2)

            shape = (shape_val,shape_val,shape_val)
            point = np.array([shape_val/2., shape_val/2., shape_val/2.])
            dist = np.linalg.norm(np.indices(shape) - point[:, None, None, None], axis=0)
            sphere = (255. - 127.5 * (1 + np.tanh((dist - radius))))
            sphere[sphere < 127.5] = 1
            sphere[sphere > 127.5] = 0

            # putting the sphere into the buffered matrix
            mat_copy[random_index[0] - half_shape_val : random_index[0] + half_shape_val + 1,
            random_index[1] - half_shape_val: random_index[1] + half_shape_val + 1,
            random_index[2] - half_shape_val: random_index[2] + half_shape_val + 1,] *= sphere

            # calculating how much material we removed
            vf = compute_volume_fraction(mat_copy[buflength:new_x_length-buflength,buflength:new_y_length-buflength,buflength:new_z_length-buflength], (0, self._surface_threshold-1))

            my_iteration += 1

            sys.stdout.write("\r{0}".format("Iteration: " + str(my_iteration) + "  Volume Fraction: " + str(vf) + "  Volume Fraction Target: " + str(vf_target)))
            sys.stdout.flush()

            if my_iteration > self._max_pits :
                print_warning("Volume Fraction was not achieved. Maximum iterations reached. Check input parameters")
                break

        # Put the updated matrix back into the workspace removing the buffer
        self._workspace.matrix = mat_copy[buflength:new_x_length-buflength,buflength:new_y_length-buflength,buflength:new_z_length-buflength]

        print("\nPits generated in: " + str(t.elapsed()) + " seconds")

    def log_input(self):
        self._workspace.log.log_section("Generating Pits")
        self._workspace.log.log_line("Domain Size: " + str(self._workspace.matrix.shape))
        self._workspace.log.log_line("Surface Threshold: " + str(self._surface_threshold))
        self._workspace.log.log_line("Mean Radius: " + str(self._mean_radius))
        self._workspace.log.log_line("Deviation Radius: " + str(self._deviation_radius))
        self._workspace.log.log_line("Volume Fraction: " + str(self._volume_fraction_removed))
        self._workspace.log.write_log()

    def log_output(self):
        self._workspace.log.log_section("Finished Generating Pits")
        self._workspace.log.write_log()

    def error_check(self):
        if not isinstance(self._workspace, Workspace):
            raise Exception("Error, workspace must be passed")
        if not (isinstance(self._surface_threshold, float) or isinstance(self._surface_threshold, int)):
            raise Exception("Error, surface_threshold must be a float/int")
        if not (isinstance(self._mean_radius, float) or isinstance(self._mean_radius, int)):
            raise Exception("Error, mean_radius must be a float/int")
        if not (isinstance(self._deviation_radius, float) or isinstance(self._deviation_radius, int)):
            raise Exception("Error, deviation_radius must be a float/int")
        if not (isinstance(self._volume_fraction_removed, float)):
            raise Exception("Error, volume_fraction must be a float")
        if self._surface_threshold <= 0:
            raise Exception("Error, invalid surface_threshold, must be a positive value")
        if self._volume_fraction_removed < 0 or self._volume_fraction_removed > 1:
            raise Exception("Error, invalid volume_fraction, must be between 0 and 1 (likely << 1)")
        if self._mean_radius < 0:
            raise Exception("Error, invalid mean_radius, must be >0")
        if self._deviation_radius < 0 or self._deviation_radius >= self._mean_radius:
            raise Exception("Error, invalid deviation_radius, must be > 0 and < mean_radius")
        if self._volume_fraction_removed > compute_volume_fraction(self._workspace, (self._surface_threshold,int(1e5))):
            raise Exception("Error, volume fraction to be removed is larger that solid volume fraction")
