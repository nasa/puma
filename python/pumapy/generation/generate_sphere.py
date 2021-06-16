from pumapy import Workspace
from pumapy.utilities.timer import Timer
import pumapy.utilities.generic_checks as check
from pumapy.generation.sphere import get_sphere
import numpy as np


def generate_sphere(size, center, diameter):
    """ Generation of a sphere at a given point and diameter

    :param size: size of 3D domain (x,y,z)
    :type size: tuple(int, int, int)
    :param center: centerpoint of sphere (x,y,z)
    :type center: tuple(int, int, int)
    :param diameter: diameter of the random spheres in voxels
    :rtype: float
    """

    generator = GeneratorSphere(size, center, diameter)

    generator.error_check()

    generator.log_input()
    generator.log_output()

    return generator.generate()


class GeneratorSphere:
    def __init__(self, size, center, diameter):
        self._workspace = Workspace()
        self._size = size
        self._center = center
        self._diameter = diameter

        self._size_padded = (self._size[0]+diameter, self._size[1]+diameter, self._size[2]+diameter)
        self._start = int(0.5 * diameter)
        self._endX = size[0] + self._start
        self._endY = size[1] + self._start
        self._endZ = size[2] + self._start

        # size of the created sphere block
        self._spheresize = int(self._diameter+4)
        if self._spheresize % 2 == 0:
            self._size_m = self._spheresize / 2
            self._size_p = self._spheresize / 2
        else:
            self._size_m = int(self._spheresize / 2.)+1
            self._size_p = int(self._spheresize / 2.)

    def generate(self):
        t = Timer()
        self._workspace.resize_new_matrix(self._size_padded)
        self._generate_sphere()
        print("Generated in: " + str(t.elapsed()) + " seconds")
        return self._workspace

    def _generate_sphere(self):
        sphere = get_sphere(self._diameter)
        [x,y,z] = self._center + np.array([self._start, self._start, self._start])
        xm = int(x - self._size_m)
        xp = int(x + self._size_p)
        ym = int(y - self._size_m)
        yp = int(y + self._size_p)
        zm = int(z - self._size_m)
        zp = int(z + self._size_p)

        self._workspace.matrix[xm:xp,ym:yp,zm:zp] += sphere[:,:,:]
        self._workspace.matrix = self._workspace.matrix[self._start:self._endX,self._start:self._endY,self._start:self._endZ]

    def log_input(self):
        self._workspace.log.log_section("Generating Sphere")
        self._workspace.log.log_value("Domain Size", self._size)
        self._workspace.log.log_value("Sphere Center", self._center)
        self._workspace.log.log_value("Diameter", self._diameter)
        self._workspace.log.write_log()

    def log_output(self):
        self._workspace.log.log_section("Finished Sphere Generation")
        self._workspace.log.write_log()

    def error_check(self):
        check.size_check(self._size)
        if self._center[0] < 0 or self._center[0] >= self._size[0]:
            raise Exception("Error, invalid center. Must be contained within domain size")
        if self._center[1] < 0 or self._center[1] >= self._size[1]:
            raise Exception("Error, invalid center. Must be contained within domain size")
        if self._center[2] < 0 or self._center[2] >= self._size[2]:
            raise Exception("Error, invalid center. Must be contained within domain size")
