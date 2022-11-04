import unittest
import numpy as np
import pumapy as puma


class TestRadiation(unittest.TestCase):

    def test_novoid(self):
        ws = puma.Workspace.from_shape_value((10, 10, 10), 1)
        try:
            puma.compute_radiation(ws, (0, 0), 1, 50)
        except:
            print("Correctly caught exception.")
            return
        self.assertTrue(False)


    def test_nosolid(self):
        ws = puma.Workspace.from_shape_value((10, 10, 10), 0)
        try:
            puma.compute_radiation(ws, (0, 0), 1, 50)
        except:
            print("Correctly caught exception.")
            return
        self.assertTrue(False)

    def test_basic(self):
        ws = puma.Workspace.from_shape_value((10, 10, 10), 1)
        ws[4, 4, 4] = 0
        self.assertTrue(puma.compute_radiation(ws, (0, 0), 1, 100) is not False)

    def test_sphere(self):
        ws_sphere = puma.import_3Dtiff(puma.path_to_example_file("100_sphere_r40.tif"), 1e-6)
        beta, beta_std, rays_distances = puma.compute_radiation(ws_sphere, (128, 255), 1000, 100)
        print(beta)
        np.testing.assert_approx_equal(beta[0], beta[1], 2)
        np.testing.assert_approx_equal(beta[0], beta[2], 2)
        np.testing.assert_approx_equal(beta[1], beta[2], 2)


class TestAnisotropicRadiation(unittest.TestCase):

    def test_novoid(self):
        ws = puma.Workspace.from_shape_value((10, 10, 10), 1)
        try:
            puma.compute_radiation_anisotropic(ws, (0, 0), 10)
        except:
            print("Correctly caught exception.")
            return
        self.assertTrue(False)


    def test_nosolid(self):
        ws = puma.Workspace.from_shape_value((10, 10, 10), 0)
        try:
            puma.compute_radiation_anisotropic(ws, (0, 0), 10)
        except:
            print("Correctly caught exception.")
            return
        self.assertTrue(False)

    def test_basic(self):
        ws = puma.Workspace.from_shape_value((10, 10, 10), 1)
        ws[4, 4, 4] = 0
        try:
            puma.compute_radiation_anisotropic(ws, (1, 1), 1000)
        except:
            self.assertTrue(False)
            return

    def test_sphere(self):
        ws_sphere = puma.import_3Dtiff(puma.path_to_example_file("100_sphere_r40.tif"), 1e-6)
        beta, beta_std, rays_distances = puma.compute_radiation_anisotropic(ws_sphere, (128, 255), 10000)
        print(beta)
        np.testing.assert_approx_equal(beta[0], beta[1], 2)
        np.testing.assert_approx_equal(beta[0], beta[2], 2)
        np.testing.assert_approx_equal(beta[1], beta[2], 2)


if __name__ == '__main__':
    unittest.main()
