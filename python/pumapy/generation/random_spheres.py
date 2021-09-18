from pumapy import Workspace
import pumapy.utilities.generic_checks as check
from pumapy.generation.sphere import get_sphere
import numpy as np
import sys


def generate_random_spheres(size, diameter, porosity, allow_intersect=True):
    """ Generation of triply periodic minimal surface material

    :param size: size of 3D domain (x,y,z)
    :type size: tuple(int, int, int)
    :param diameter: diameter of the random spheres in voxels
    :type diameter: float
    :param porosity: target porosity of the generated structure
    :type porosity: float
    :param allow_intersect: allow the spheres to intersect or not
    :type allow_intersect: bool
    :return: domain with random spheres with input diameter
    :rtype: Workspace

    :Example:
    >>> import pumapy as puma
    >>> ws_generated = puma.generate_random_spheres(size=(200,200,200), diameter=20, porosity=0.7) # Generating a workspace of randomly placed, intersecting spheres
    >>> puma.render_contour(ws_generated, cutoff=(128, 255))
    """

    generator = GeneratorSpheres(size, diameter, porosity, allow_intersect)

    generator.error_check()

    generator.log_input()
    generator.log_output()

    return generator.generate()


class GeneratorSpheres:
    def __init__(self, size, diameter, porosity, allow_intersect):
        self._workspace = Workspace()
        self._size = size
        self._diameter = diameter
        self._porosity = porosity
        self._allow_intersect = allow_intersect

        self._size_padded = (self._size[0]+4*diameter, self._size[1]+4*diameter, self._size[2]+4*diameter)
        self._start = 2 * diameter
        self._endX = size[0] + self._start
        self._endY = size[1] + self._start
        self._endZ = size[2] + self._start

        self._placed_sphere_list = [(-1e6, -1e6, -1e6)]

        # size of the created sphere block
        self._spheresize = int(self._diameter+4)
        if self._spheresize % 2 == 0:
            self._size_m = self._spheresize / 2
            self._size_p = self._spheresize / 2
        else:
            self._size_m = int(self._spheresize / 2.)+1
            self._size_p = int(self._spheresize / 2.)

    def generate(self):
        self._workspace.resize_new_matrix(self._size_padded)
        self._generate_spheres()

        return self._workspace

    def _generate_spheres(self):
        sphere = get_sphere(self._diameter)
        radius = self._diameter / 2.
        volume_per_sphere = 4./3. * np.pi * radius**3
        volume_all_spheres = self._size[0] * self._size[1] * self._size[2] * (1-self._porosity)

        approx_num_spheres = volume_all_spheres / volume_per_sphere / self._porosity
        print("Approximately " + str(approx_num_spheres) + " spheres to be generated")

        current_volume = 0
        approx_volume_left = volume_all_spheres - current_volume
        approx_spheres_left = approx_volume_left / volume_per_sphere

        current_porosity = 1
        localCount = 0
        totalCount = 0

        while current_porosity > self._porosity:
            x = np.random.randint(self._size[0] + 2 * self._diameter) + self._diameter
            y = np.random.randint(self._size[1] + 2 * self._diameter) + self._diameter
            z = np.random.randint(self._size[2] + 2 * self._diameter) + self._diameter
            xm = int(x - self._size_m)
            xp = int(x + self._size_p)
            ym = int(y - self._size_m)
            yp = int(y + self._size_p)
            zm = int(z - self._size_m)
            zp = int(z + self._size_p)

            if self._allow_intersect:
                self._workspace.matrix[xm:xp, ym:yp, zm:zp] += sphere
            else:
                intersection = False
                for location in self._placed_sphere_list:
                    if np.sqrt( (x-location[0])*(x-location[0]) +
                                (y-location[1])*(y-location[1]) +
                                (z-location[2])*(z-location[2]) ) < self._diameter:
                        intersection = True
                        break
                if intersection:
                    continue
                else:
                    self._workspace.matrix[xm:xp, ym:yp, zm:zp] += sphere
                    self._placed_sphere_list.append((x, y, z))

            localCount += 1
            totalCount += 1

            if localCount > 0.5 * approx_spheres_left:
                localCount = 0

                mask = self._workspace.matrix[self._start:self._endX, self._start:self._endY, self._start:self._endZ] >= 0
                mask_high = self._workspace.matrix[self._start:self._endX, self._start:self._endY, self._start:self._endZ] <= 127
                mask = mask * mask_high
                current_porosity = float(np.sum(mask)) / float(self._size[0] * self._size[1] * self._size[2])

                current_volume = self._size[0] * self._size[1] * self._size[2] * (1 - current_porosity)
                approx_volume_left = volume_all_spheres - current_volume
                approx_spheres_left = approx_volume_left / volume_per_sphere
                sys.stdout.write("\rSpheres Generated {}  Porosity = {} ".format(totalCount, current_porosity))

        self._workspace.matrix = self._workspace.matrix[self._start:self._endX, self._start:self._endY, self._start:self._endZ]
        self._workspace.matrix[self._workspace.matrix > 255] = 255
        self._workspace.matrix = self._workspace.matrix.astype(np.uint16)

    def log_input(self):
        self._workspace.log.log_section("Generating Random Spheres")
        self._workspace.log.log_value("Domain Size", self._size)
        self._workspace.log.log_value("Diameter", self._diameter)
        self._workspace.log.log_value("Porosity", self._porosity)
        self._workspace.log.write_log()

    def log_output(self):
        self._workspace.log.log_section("Finished Random Sphere Generation")
        self._workspace.log.write_log()

    def error_check(self):
        check.size_check(self._size)
        check.greater_than_exc(self._diameter, 0, "diameter")
        check.range_exc(self._porosity, (0, 1), "porosity")
