import unittest
import numpy as np
from pumapy.utilities.isosurface import generate_isosurface
import pumapy as puma
from skimage import io


class TestIsosurface(unittest.TestCase):

    def test_fiberform(self):
        ws = puma.Workspace.from_array(io.imread(puma.path_to_example_file("200_fiberform.tif")))
        mesh = generate_isosurface(ws, (90, 255), True, False)

        np.testing.assert_equal(len(mesh.values), 665453)
        np.testing.assert_equal(mesh.faces.shape, (1330866, 3))


class TestSurfacearea(unittest.TestCase):

    def test_fiberform(self):
        ws = puma.Workspace.from_array(io.imread(puma.path_to_example_file("200_fiberform.tif")))
        area, specific_area = puma.compute_surface_area(ws, (90, 255), False)

        np.testing.assert_almost_equal(area, 4.12090e-7, decimal=5)
        np.testing.assert_almost_equal(specific_area, 52291.746, decimal=3)


class TestMeanInterceptLength(unittest.TestCase):

    def test_fiberform(self):
        ws = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.)
        mil = puma.compute_mean_intercept_length(ws, (0, 89))
        np.testing.assert_almost_equal(mil, (63.26930970616208, 70.25351564671224, 54.3159003008165))


class TestVolumeFraction(unittest.TestCase):

    def test_fiberform(self):
        ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
        vf = puma.compute_volume_fraction(ws, cutoff=(90, 255))
        np.testing.assert_almost_equal(vf, 0.16714)


class TestOrientationST(unittest.TestCase):

    def test_artfib(self):
        ws1 = puma.import_vti("testdata/artifib.vtk")
        puma.compute_orientation_st(ws1, (128, 255), 0.7, 1.1)
        ws2 = puma.import_vti("testdata/artifib_orient.vtk")
        error, mean, std = puma.compute_angular_differences(ws1.matrix, ws1.orientation, ws2.orientation, (128, 255))
        self.assertTrue(mean < 5 and std < 10)

    def test_artfib_edt(self):
        ws1 = puma.import_vti("testdata/artifib.vtk")
        puma.compute_orientation_st(ws1, (128, 255), 0.7, 1.1, edt=True)
        ws2 = puma.import_vti("testdata/artifib_orient.vtk")
        error, mean, std = puma.compute_angular_differences(ws1.matrix, ws1.orientation, ws2.orientation, (128, 255))
        print(mean, std)
        self.assertTrue(mean < 6 and std < 10)


if __name__ == '__main__':
    unittest.main()
