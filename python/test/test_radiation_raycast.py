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
        self.assertTrue(puma.compute_radiation(ws, (1, 1), 1, 45) is not False)

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

    def test_non180divider(self):
        ws = puma.Workspace.from_shape_value((10, 10, 10), 1)
        simulation = RayCasting(ws, 70, np.array([[4, 4, 4]]), 1)
        try:
            simulation.error_check()
        except:
            print("Correctly caught exception.")
            return
        self.assertTrue(False)

    def test_numberofparticles(self):
        ws = puma.Workspace.from_shape_value((10, 10, 10), 1)
        simulation = RayCasting(ws, 45, np.array([[4, 4, 4]]), 1)
        self.assertFalse(simulation.error_check())
        simulation.generate_spherical_walkers()
        self.assertEqual(simulation.spherical_walkers.shape[0], 26)

    def test_distancetravelled1(self):
        ws = puma.Workspace.from_shape_value((9, 9, 9), 1)
        ws.voxel_length = 1
        simulation = RayCasting(ws, 90, np.array([[4, 4, 4]]), 1)
        self.assertFalse(simulation.error_check())
        simulation.generate_spherical_walkers()
        simulation.expand_sources()
        np.testing.assert_equal(np.sqrt(simulation.rays_distances[:, 0]**2 +
                                        simulation.rays_distances[:, 1]**2 +
                                        simulation.rays_distances[:, 2]**2), np.repeat(4.5, 6))

    # if platform.system() != 'Windows':  # not same answer on windows
    #     def test_distancetravelled2(self):
    #         ws = puma.Workspace.from_shape_value((9, 9, 9), 1)
    #         ws.voxel_length = 1
    #         simulation = RayCasting(ws, 45, np.array([[4, 4, 4]]), 1)
    #         self.assertFalse(simulation.error_check())
    #         simulation.generate_spherical_walkers()
    #         simulation.expand_sources()
    #         np.testing.assert_almost_equal(np.sort(np.sqrt(simulation.rays_distances[:, 0]**2 +
    #                                                        simulation.rays_distances[:, 1]**2 +
    #                                                        simulation.rays_distances[:, 2]**2)),
    #                                        np.concatenate((np.repeat(4.5, 6), np.repeat(np.sqrt(2) * 4.5, 20))), 7)

    def test_distancetravelled_withcollision(self):
        ws = puma.Workspace.from_shape_value((9, 9, 9), 1)
        ws[4, 4, 0] = 0
        ws[4, 1, 4] = 0
        ws.voxel_length = 1
        source = np.array([[4, 4, 4]])
        simulation = RayCasting(ws, 90, source, 1)
        self.assertFalse(simulation.error_check())
        simulation.generate_spherical_walkers()
        simulation.expand_sources()
        np.testing.assert_almost_equal(np.sort(np.sqrt(simulation.rays_distances[:, 0]**2 +
                                                       simulation.rays_distances[:, 1]**2 +
                                                       simulation.rays_distances[:, 2]**2)),
                                       np.concatenate((np.array([2.5, 3.5]), np.repeat(4.5, 4))), 7)


if __name__ == '__main__':
    unittest.main()
