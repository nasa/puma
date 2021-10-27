from pumapy.utilities.workspace import Workspace
from pumapy.utilities.timer import Timer
import pumapy.utilities.generic_checks as check
from pumapy.generation.single_sphere import get_circle_even, get_circle_odd
import numpy as np


def generate_cylinder_square_array(size, porosity, segmented=True):
    """ Generate a 2D periodic array of circles

        :param size: length of one side of the output domain
        :type size: int
        :param porosity: porosity of the output domain
        :type porosity: float
        :param segmented: return a domain that is already segmented (i.e. each sphere with unique ID) or
            with grayscales 0-255 with threshold=128 for the input diameter
        :type segmented: bool
        :return: array of circles
        :rtype: Workspace

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.generate_cylinder_square_array(100, 0.8, segmented=True)
        >>> puma.render_volume(ws)
    """
    generator = GeneratorSquareArray(size, porosity)

    generator.error_check()

    generator.log_input()
    ws = generator.generate()
    generator.log_output()

    if segmented:
        ws.binarize(128)
    return ws


class GeneratorSquareArray:
    def __init__(self, size, porosity):
        self._workspace = Workspace()
        self._size = (size, size, 1)
        self._center = (np.array([size, size]) / 2) + np.array([size, size])
        self._porosity = porosity

        self._diameter = np.sqrt((1-porosity) * size*size / (2 * np.pi)) * 2
        self._size_padded = (self._size[0] * 3, self._size[1]*3, 1)

    def generate(self):
        t = Timer()
        self._workspace.resize_new_matrix(self._size_padded)
        self._generate_circle()
        print("Generated in: " + str(t.elapsed()) + " seconds")
        return self._workspace

    def _generate_circle(self):

        if self._size[0] % 2 == 0:
            circle, circlesize = get_circle_even(self._diameter)
        else:
            circle, circlesize = get_circle_odd(self._diameter)

        self._circlesize = circlesize
        if self._circlesize % 2 == 0:
            self._size_m = self._circlesize / 2
            self._size_p = self._circlesize / 2
        else:
            self._size_m = int(self._circlesize / 2.)
            self._size_p = int(self._circlesize / 2.) + 1

        [x,y] = self._center
        xm = int(x - self._size_m)
        xp = int(x + self._size_p)
        ym = int(y - self._size_m)
        yp = int(y + self._size_p)
        self._workspace.matrix[xm:xp, ym:yp] += circle[:,:,None]

        circle, circlesize = get_circle_even(self._diameter)
        self._circlesize = circlesize
        if self._circlesize % 2 == 0:
            self._size_m = self._circlesize / 2
            self._size_p = self._circlesize / 2
        else:
            self._size_m = int(self._circlesize / 2.)
            self._size_p = int(self._circlesize / 2.) + 1

        xvec = [self._size[0], self._size[0] * 2]
        yvec = [self._size[1], self._size[1] * 2]

        for x in xvec:
            for y in yvec:
                xm = int(x - self._size_m)
                xp = int(x + self._size_p)
                ym = int(y - self._size_m)
                yp = int(y + self._size_p)
                self._workspace.matrix[xm:xp, ym:yp] += circle[:, :, None]

        self._workspace.matrix = self._workspace.matrix[self._size[0]:2 * self._size[0], self._size[1]:2 * self._size[1]]

    def log_input(self):
        self._workspace.log.log_section("Generating Square Array")
        self._workspace.log.log_value("Domain Size", self._size)
        self._workspace.log.log_value("Porosity", self._porosity)
        self._workspace.log.log_value("Diameter", self._diameter)
        self._workspace.log.write_log()

    def log_output(self):
        self._workspace.log.log_section("Finished Generating Square Array")
        self._workspace.log.write_log()

    def error_check(self):
        if not check.size_check(self._size):
            return False
        if self._porosity < 0.214602:
            raise Exception("Error, invalid porosity. Cylinders cannot fit with a porosity of < 0.214602")
