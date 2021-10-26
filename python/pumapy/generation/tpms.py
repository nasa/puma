from pumapy.utilities.workspace import Workspace
from pumapy.utilities.timer import Timer
from pumapy.generation.tpms_utils import generate
import numpy as np


def generate_tpms(shape, w, q, equation=0, segmented=True):
    """ Generation of triply periodic minimal surface material

        :param shape: shape of 3D domain (x,y,z)
        :type shape: (int, int, int)
        :param w: w parameter for tpms
        :type w: float or (float, float)
        :param q: q parameter for tpms (float or tuple)
        :type q: float or (float, float)
        :param equation: equation 0, 1, or 2 for tpms
        :type equation: int
        :param segmented: return a domain that is already segmented (i.e. 1 for solid and 0 for void) or
            with grayscales 0-255 with threshold=128 for the solid
        :type segmented: bool
        :return: TPMS domain with grayscales from 0-255 with threshold=128 for the normal TPMS geometry
        :rtype: Workspace

        :Example:
        >>> import pumapy as puma
        >>> shape = (200, 200, 200)  # size of the domain, in voxels.
        >>> w = 0.08  # value of w in the equations above
        >>> q = 0.2  # value of q in the equations above
        >>> ws_eq0 = puma.generate_tpms(shape, w, q, equation=0, segmented=False)
        >>> ws_eq1 = puma.generate_tpms(shape, w, q, equation=1, segmented=False)
        >>> ws_eq2 = puma.generate_tpms(shape, w, q, equation=2, segmented=False)
        >>> puma.render_contour(ws_eq0, cutoff=(128, 255)) #visualize the workspace
        >>> puma.render_contour(ws_eq1, cutoff=(128, 255))
        >>> puma.render_contour(ws_eq2, cutoff=(128, 255))
    """

    if isinstance(w, tuple):
        _w = w
    elif isinstance(w, float):
        _w = (w, w)
    else:
        raise Exception("Invalid w, must be float or tuple")

    if isinstance(q, tuple):
        _q = q
    elif isinstance(q, float):
        _q = (q, q)
    else:
        raise Exception("Invalid w, must be float or tuple")

    generator = GeneratorTPMS(shape, _w, _q, equation)

    generator.error_check()

    generator.log_input()
    ws = generator.generate()
    generator.log_output()

    if segmented:
        ws.binarize(128)
    return ws


class GeneratorTPMS:
    def __init__(self, size, w, q, equation):
        self._workspace = Workspace()
        self._size = size
        self._wmin = w[0]
        self._wmax = w[1]
        self._qmin = q[0]
        self._qmax = q[1]
        self._equation = equation

    def generate(self):
        t = Timer()
        self._workspace.resize_new_matrix(self._size)
        self._workspace.matrix = generate(self._workspace.matrix.astype(float), self._size[0], self._size[1],
                                          self._size[2], self._wmin, self._wmax, self._qmin, self._qmax, self._equation)
        self._workspace.matrix = (255. * (self._workspace.matrix - np.min(self._workspace.matrix)) /
                                  np.ptp(self._workspace.matrix)).astype(np.uint16)
        print("Generated in: " + str(t.elapsed()) + " seconds")
        return self._workspace

    def log_input(self):
        self._workspace.log.log_section("Generating TPMS")
        self._workspace.log.log_line("Domain Size: " + str(self._size))
        self._workspace.log.log_line("w: " + str(self._wmin) + ", " + str(self._wmax))
        self._workspace.log.log_line("w: " + str(self._qmin) + ", " + str(self._qmax))
        self._workspace.log.log_line("Equation: " + str(self._equation))
        self._workspace.log.write_log()

    def log_output(self):
        self._workspace.log.log_section("Finished TPMS Generation")
        self._workspace.log.write_log()

    def error_check(self):
        if not isinstance(self._workspace, Workspace):
            raise Exception("Error, workspace must be passed")
        if not (isinstance(self._size, tuple) and len(self._size) == 3):
            raise Exception("Error, invalid size, must be tuple of size 3")
        if self._size[0] <= 0:
            raise Exception("Error, invalid size, must be >= 0")
        if self._size[1] <= 0:
            raise Exception("Error, invalid size, must be >= 0")
        if self._size[2] <= 0:
            raise Exception("Error, invalid size, must be >= 0")
        if self._wmin <= 0:
            raise Exception("Error, invalid w, must be >= 0")
        if self._wmax <= 0:
            raise Exception("Error, invalid w, must be >= 0")
        if self._wmax < self._wmin:
            raise Exception("Error, invalid w, w[1] cannot be < w[0]")
        if self._qmin <= 0:
            raise Exception("Error, invalid q, must be >= 0")
        if self._qmax <= 0:
            raise Exception("Error, invalid q, must be >= 0")
        if self._qmax < self._wmin:
            raise Exception("Error, invalid q, q[1] cannot be < q[0]")
        if self._equation != 0 and self._equation != 1 and self._equation != 2:
            raise Exception("Error, invalid equation number. Must be 0, 1, or 2")
