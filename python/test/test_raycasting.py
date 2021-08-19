import unittest
import numpy as np
import pumapy as puma
from pumapy.utilities.raycasting import RayCasting


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

    def test_distancetravelled2(self):
        ws = puma.Workspace.from_shape_value((9, 9, 9), 1)
        ws.voxel_length = 1
        simulation = RayCasting(ws, 45, np.array([[4, 4, 4]]), 1)
        self.assertFalse(simulation.error_check())
        simulation.generate_spherical_walkers()
        simulation.expand_sources()
        np.testing.assert_almost_equal(np.sort(np.sqrt(simulation.rays_distances[:, 0]**2 +
                                                       simulation.rays_distances[:, 1]**2 +
                                                       simulation.rays_distances[:, 2]**2)),
                                       np.concatenate((np.repeat(4.5, 6), np.repeat(np.sqrt(2) * 4.5, 20))), 7)

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
