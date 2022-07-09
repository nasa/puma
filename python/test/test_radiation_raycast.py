import unittest
import numpy as np
import pumapy as puma
from pumapy.physicsmodels.raycasting import RayCasting
import platform


class TestRadiation(unittest.TestCase):

    def test_nophase(self):
        ws = puma.Workspace.from_shape_value((10, 10, 10), 1)
        try:
            puma.compute_radiation(ws, (1, 1), 1, 45)
        except:
            print("Correctly caught exception.")
            return
        self.assertTrue(False)

    def test_basic(self):
        ws = puma.Workspace.from_shape_value((10, 10, 10), 1)
        ws[4, 4, 4] = 0
        self.assertTrue(puma.compute_radiation(ws, (1, 1), 1, 100) is not False)

    # if platform.system() != 'Windows':  # not same answer on windows
    #     def test_artfib(self):
    #         ws = puma.import_vti("testdata/artifib.vtk")
    #         ws.set_material_id((0, 128), 0)
    #         ws.set_material_id((129, 255), 1)

    #         np.random.seed(0)
    #         beta, beta_std, _ = puma.compute_radiation(ws, (1, 1), 100, 15, boundary_behavior=0)
    #         print(beta)
    #         np.testing.assert_almost_equal(beta, [0.18048920415267472, 0.17922697533941973, 0.1241739185326434], decimal=4)


class TestRayCasting(unittest.TestCase):

    def test_nophase(self):
        ws = puma.Workspace.from_shape_value((10, 10, 10), 1)
        simulation = RayCasting(ws, 45, np.array([[4, 4, 4]]), 0)
        try:
            simulation.error_check()
        except:
            print("Correctly caught exception.")
            return
        self.assertTrue(False)


    def test_sphere(self):
        ws_sphere = puma.import_3Dtiff(puma.path_to_example_file("100_sphere_r40.tif"), 1e-6)
        beta, beta_std, rays_distances = puma.compute_radiation(ws_sphere, (0, 127), 1000, 100)
        print(beta)
        np.testing.assert_approx_equal(beta[0], beta[1], 2)
        np.testing.assert_approx_equal(beta[0], beta[2], 2)
        np.testing.assert_approx_equal(beta[1], beta[2], 2)



if __name__ == '__main__':
    unittest.main()
