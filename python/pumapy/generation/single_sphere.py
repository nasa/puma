from pumapy.utilities.workspace import Workspace
from pumapy.utilities.timer import Timer
import pumapy.utilities.generic_checks as check
import numpy as np


def generate_sphere(shape, center, diameter, segmented=True):
    """ Generation of a sphere at a given point and diameter

        :param shape: size of 3D domain (x,y,z)
        :type shape: (int, int, int)
        :param center: centerpoint of sphere (x,y,z)
        :type center: (int, int, int)
        :param diameter: diameter of the random spheres in voxels
        :type diameter: float
        :param segmented: return a domain that is already segmented (i.e. each sphere with unique ID) or
            with grayscales 0-255 with threshold=128 for the input diameter
        :type segmented: bool
        :return: domain with sphere with input diameter
        :rtype: Workspace

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.generate_sphere((100, 100, 100), (50, 50, 50), 80)
        >>> puma.render_volume(ws.matrix[:ws.matrix.shape[0]//2])
    """

    generator = GeneratorSphere(shape, center, diameter)

    generator.error_check()

    generator.log_input()
    ws = generator.generate()
    generator.log_output()

    if segmented:
        ws.binarize(128)
    return ws


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


def get_sphere(diameter):
    radius = diameter / 2.
    shape = (int(diameter+4), int(diameter+4), int(diameter+4))
    center_point = (diameter+4.) / 2. - 0.5
    point = np.array([center_point, center_point, center_point])
    dist = np.linalg.norm(np.indices(shape)-point[:, None, None, None], axis=0)
    sphere = (255.-127.5*(1+np.tanh((dist-radius))))
    sphere[sphere < 0] = 0
    sphere = sphere.astype(np.uint16)
    return sphere


def get_circle(diameter):
    radius = diameter / 2.
    shape = (int(diameter+4), int(diameter+4))
    center_point = (int(diameter+4)) / 2.
    point = np.array([center_point, center_point])
    dist = np.linalg.norm(np.indices(shape)+0.5-point[:, None, None], axis=0)
    circle = (255.-128*(1+np.tanh(2*(dist-radius))))
    circle[circle < 0] = 0
    circle = circle.astype(np.uint16)
    return circle


def get_circle_odd(diameter):
    radius = diameter / 2.
    if int(diameter) % 2 == 0:
        shape = (int(diameter+5), int(diameter+5))
        center_point = (int(diameter + 5)) / 2.
    else:
        shape = (int(diameter+4), int(diameter+4))
        center_point = (int(diameter + 4)) / 2.
    point = np.array([center_point, center_point])
    dist = np.linalg.norm(np.indices(shape)+0.5-point[:, None, None], axis=0)
    circle = (255.-128*(1+np.tanh(2*(dist-radius))))
    circle[circle < 0] = 0
    circle = circle.astype(np.uint16)
    return circle, shape[0]


def get_circle_even(diameter):
    radius = diameter / 2.
    if int(diameter) % 2 == 0:
        shape = (int(diameter+4), int(diameter+4))
        center_point = (int(diameter + 4)) / 2.
    else:
        shape = (int(diameter+5), int(diameter+5))
        center_point = (int(diameter + 5)) / 2.
    point = np.array([center_point, center_point])
    dist = np.linalg.norm(np.indices(shape)+0.5-point[:, None, None], axis=0)
    circle = (255.-128*(1+np.tanh(2*(dist-radius))))
    circle[circle < 0] = 0
    circle = circle.astype(np.uint16)
    return circle, shape[0]
