"""
Further explained in publication:
Semeraro, F., Ferguson, J.C., Panerai, F., King, R.J. and Mansour, N.N., 2020.
Anisotropic analysis of fibrous and woven materials part 1: Estimation of local orientation.
Computational Materials Science, 178, p.109631.
(https://www.sciencedirect.com/science/article/abs/pii/S0927025620301221)

Please cite using this BibTex:
@article{semeraro2020anisotropic,
  title={Anisotropic analysis of fibrous and woven materials part 1: Estimation of local orientation},
  author={Semeraro, Federico and Ferguson, Joseph C and Panerai, Francesco and King, Robert J and Mansour, Nagi N},
  journal={Computational Materials Science},
  volume={178},
  pages={109631},
  year={2020},
  publisher={Elsevier}
}
"""

import sys
import numpy as np
from scipy.ndimage.filters import gaussian_filter
from scipy.ndimage import distance_transform_edt
from pumapy.utilities.generic_checks import check_ws_cutoff


def compute_angular_differences(matrix, orientation1, orientation2, cutoff):
    """ Compute angular difference between two orientation ndarrays

        :param matrix: domain matrix
        :type matrix: np.ndarray
        :param orientation1: orientation as (x, y, z, 3)
        :type orientation1: np.ndarray
        :param orientation2: orientation as (x, y, z, 3)
        :type orientation2: np.ndarray
        :param cutoff: to binarize domain
        :type cutoff: (int, int)
        :return: angle_errors in degrees, mean, std
        :rtype: (np.ndarray, float, float)
    """

    if not isinstance(matrix, np.ndarray) or not isinstance(orientation1, np.ndarray) or not isinstance(orientation2, np.ndarray):
        raise Exception("Inputs must be ndarrays.")
    if not isinstance(cutoff, tuple) or not len(cutoff) == 2:
        raise Exception("Cutoff must be a tuple(int, int).")
    if not (orientation1.ndim == 4 and orientation2.ndim == 4 and matrix.ndim == 3 and orientation1.shape[3] == 3 and
            orientation1.shape == orientation2.shape and orientation1.shape[0] == matrix.shape[0] and
            orientation1.shape[1] == matrix.shape[1] and orientation1.shape[2] == matrix.shape[2]):
        raise Exception("Incorrect dimensions in input ndarrays.")

    mask = np.logical_and(matrix >= cutoff[0], matrix <= cutoff[1])
    unit_vectors_1 = orientation1[mask]
    unit_vectors_2 = orientation2[mask]

    radians_diff = np.einsum('ij,ij->i', unit_vectors_1, unit_vectors_2)
    diff = np.zeros((unit_vectors_1.shape[0], 2), dtype=float)
    diff[:, 0] = np.degrees(np.arccos(np.clip(radians_diff, -1, 1)))
    diff[:, 1] = 180 - diff[:, 0]
    diff = np.min(diff, axis=1)

    angle_diff = np.zeros_like(matrix, dtype=float)
    angle_diff[mask] = diff

    return angle_diff, diff.mean(), diff.std()


def compute_orientation_st(ws, cutoff, sigma=1.4, rho=0.7, edt=False):
    """ Compute orientation of the material by the structure tensor algorithm

        :param ws: domain
        :type ws: pumapy.Workspace
        :param cutoff: which grayscales to consider
        :type cutoff: (int, int)
        :param sigma: kernel size parameter for Gaussian derivatives (should be smaller than rho)
        :type sigma: float
        :param rho: kernel size parameter for Gaussian filter (should be bigger than sigma)
        :type rho: float
        :param edt: indicating if we need to apply Euclidean Distance Transform before computing ST
        :type edt: bool
        :return: True if successful, False otherwise.
        :rtype: bool

        :Example:
        >>> import pumapy as puma
        >>> import pyvista as pv
        >>> ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6) # import example file
        >>> puma.compute_orientation_st(ws, cutoff=(90, 255), sigma=1.4, rho=0.7) # compute orientation
        >>> p = pv.Plotter(shape=(1, 2))
        >>> p.subplot(0, 0)
        >>> p.add_text("Microstructure")
        >>> puma.render_contour(ws, (90, 255), add_to_plot=p, plot_directly=False) # visualize the workspace
        >>> p.subplot(0, 1)
        >>> p.add_text("Detected fiber orientation")
        >>> puma.render_orientation(ws, add_to_plot=p, plot_directly=False)
        >>> p.show()
    """
    solver = OrientationST(ws, sigma, rho, cutoff, edt)

    solver.error_check()

    solver.log_input()
    check = solver.compute()
    solver.log_output()
    return check


class OrientationST:
    def __init__(self, ws, sigma, rho, cutoff, edt):
        self.ws = ws.copy()
        self.ws_in = ws

        self.sigma = sigma
        self.rho = rho
        self.cutoff = cutoff
        self.edt = edt

        self.mask = None
        self.st = None

    def compute(self):
        self.__compute_structuretensor()
        self.__eigenvalue_analysis()
        return True

    def __compute_structuretensor(self):
        self.ws.matrix = self.ws.matrix.astype(int)
        self.mask = np.logical_and(self.ws.matrix >= self.cutoff[0], self.ws.matrix <= self.cutoff[1])

        sys.stdout.write("First gradient computation ... ")
        if self.edt:
            self.st = np.empty((self.ws.matrix.shape[0], self.ws.matrix.shape[1], self.ws.matrix.shape[2], 3, 3), dtype=float)
            tmp = np.empty(self.ws.matrix.shape, dtype=float)
            distance = distance_transform_edt(self.mask)
            Vx = gaussian_filter(distance, self.sigma, order=[1, 0, 0], mode='nearest')
            Vy = gaussian_filter(distance, self.sigma, order=[0, 1, 0], mode='nearest')
            Vz = gaussian_filter(distance, self.sigma, order=[0, 0, 1], mode='nearest')
        else:
            self.st = np.empty((self.ws.matrix.shape[0], self.ws.matrix.shape[1], self.ws.matrix.shape[2], 3, 3), dtype=int)
            tmp = np.empty(self.ws.matrix.shape, dtype=int)
            Vx = gaussian_filter(self.ws.matrix, self.sigma, order=[1, 0, 0], mode='nearest')
            Vy = gaussian_filter(self.ws.matrix, self.sigma, order=[0, 1, 0], mode='nearest')
            Vz = gaussian_filter(self.ws.matrix, self.sigma, order=[0, 0, 1], mode='nearest')
        print("Done")

        # blur the gradient products with a Gaussian filter and assemble structure tensor
        sys.stdout.write("Blurring of gradients ... ")
        np.multiply(Vx, Vx, out=tmp)
        self.st[:, :, :, 0, 0] = gaussian_filter(tmp, self.rho, mode='nearest')
        np.multiply(Vy, Vy, out=tmp)
        self.st[:, :, :, 1, 1] = gaussian_filter(tmp, self.rho, mode='nearest')
        np.multiply(Vz, Vz, out=tmp)
        self.st[:, :, :, 2, 2] = gaussian_filter(tmp, self.rho, mode='nearest')
        np.multiply(Vx, Vy, out=tmp)
        self.st[:, :, :, 0, 1] = gaussian_filter(tmp, self.rho, mode='nearest')
        self.st[:, :, :, 1, 0] = self.st[:, :, :, 0, 1]
        np.multiply(Vx, Vz, out=tmp)
        self.st[:, :, :, 0, 2] = gaussian_filter(tmp, self.rho, mode='nearest')
        self.st[:, :, :, 2, 0] = self.st[:, :, :, 0, 2]
        np.multiply(Vy, Vz, out=tmp)
        self.st[:, :, :, 1, 2] = gaussian_filter(tmp, self.rho, mode='nearest')
        self.st[:, :, :, 2, 1] = self.st[:, :, :, 1, 2]
        print("Done")

    def __eigenvalue_analysis(self):
        sys.stdout.write("Computing eigenvalue analysis ... ")
        self.ws_in.set_orientation(np.zeros((self.ws.matrix.shape[0], self.ws.matrix.shape[1], self.ws.matrix.shape[2], 3), dtype=float))
        self.ws_in.orientation[self.mask] = np.linalg.eigh(self.st[self.mask])[1][:, :, 0]
        print("Done")

    def log_input(self):
        self.ws.log.log_section("Detecting Orientation using the Structure Tensor (ST) method")
        self.ws.log.log_value("Sigma", self.sigma)
        self.ws.log.log_value("Rho", self.rho)
        self.ws.log.log_value("Cutoff", self.cutoff)
        self.ws.log.log_value("Apply EDT before computation", self.edt)
        self.ws.log.write_log()

    def log_output(self):
        self.ws.log.log_section("Finished Orientation ST computation")
        self.ws.log.write_log()

    def error_check(self):
        check_ws_cutoff(self.ws, self.cutoff)
