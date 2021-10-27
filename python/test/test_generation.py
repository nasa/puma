import unittest
import pumapy as puma
import numpy as np
np.random.seed(0)


class TestCylinderSquareArray(unittest.TestCase):

    def test_2d_square_array(self):
        ws = puma.generate_cylinder_square_array(300, 0.8, segmented=True)
        np.testing.assert_equal(ws.matrix.shape, (300, 300, 1))
        np.testing.assert_almost_equal(ws.porosity(), 0.8, 3)

        ws = puma.generate_cylinder_square_array(300, 0.8, segmented=False)
        np.testing.assert_equal(ws.matrix.shape, (300, 300, 1))
        np.testing.assert_almost_equal(ws.porosity(cutoff=(0, 127)), 0.8, 3)


class TestSphere(unittest.TestCase):

    def test_sphere(self):
        ws = puma.generate_sphere((100, 100, 100), (50, 50, 50), 80, segmented=True)
        analytical_porosity = (100. ** 3 - (4./3.) * np.pi * (40. ** 3)) / (100. ** 3)
        np.testing.assert_equal(ws.matrix.shape, (100, 100, 100))
        np.testing.assert_almost_equal(ws.porosity(), analytical_porosity, 4)

        ws = puma.generate_sphere((100, 100, 100), (50, 50, 50), 80, segmented=False)
        np.testing.assert_equal(ws.matrix.shape, (100, 100, 100))
        np.testing.assert_almost_equal(ws.porosity(cutoff=(0, 127)), analytical_porosity, 4)


class TestRandomSpheres(unittest.TestCase):

    def test_random_spheres(self):
        ws = puma.generate_random_spheres(shape=(200, 200, 200), diameter=4, porosity=0.75,
                                          allow_intersect=True, segmented=True)
        np.testing.assert_equal(ws.matrix.shape, (200, 200, 200))
        self.assertLess(abs(ws.porosity() - 0.75), 0.0001)

        ws = puma.generate_random_spheres(shape=(100, 100, 100), diameter=8, porosity=0.8,
                                          allow_intersect=True, segmented=False)
        np.testing.assert_equal(ws.matrix.shape, (100, 100, 100))
        self.assertLess(abs(ws.porosity(cutoff=(0, 127)) - 0.8), 0.00013)

        ws = puma.generate_random_spheres(shape=(50, 50, 50), diameter=4, porosity=0.8,
                                          allow_intersect=False, segmented=False)
        np.testing.assert_equal(ws.matrix.shape, (50, 50, 50))
        self.assertLess(abs(ws.porosity(cutoff=(0, 127)) - 0.8), 0.00023)


class TestRandomFibers(unittest.TestCase):

    def test_random_fibers(self):
        ws = puma.generate_random_fibers(shape=(100, 100, 100), radius=2, porosity=0.8, length=50, max_iter=6,
                                         allow_intersect=True, segmented=True)
        np.testing.assert_equal(ws.matrix.shape, (100, 100, 100))
        self.assertLess(abs(ws.porosity() - 0.8), 0.001)

        ws = puma.generate_random_fibers(shape=(100, 100, 100), radius=2, nfibers=100, phi=20, theta=90, length=100,
                                         max_iter=6, allow_intersect=True, segmented=True)
        np.testing.assert_equal(ws.matrix.shape, (100, 100, 100))
        self.assertEqual(ws.max(), 100)

        ws = puma.generate_random_fibers(shape=(50, 50, 50), radius=2, porosity=0.8, length=50, max_iter=6,
                                         allow_intersect=False, segmented=True)
        np.testing.assert_equal(ws.matrix.shape, (50, 50, 50))
        self.assertLess(abs(ws.porosity() - 0.8), 0.01)

        ws = puma.generate_random_fibers(shape=(100, 100, 100), radius=2, porosity=0.8, length=50, max_iter=6,
                                         allow_intersect=True, segmented=False)
        np.testing.assert_equal(ws.matrix.shape, (100, 100, 100))
        self.assertLess(abs(ws.porosity(cutoff=(0, 127)) - 0.8), 0.001)

    def test_51x44x87fibers_nfibers(self):
        nfibers = 100
        shape = (51, 44, 87)
        ws = puma.generate_random_fibers(shape=shape, radius=3, nfibers=nfibers, phi=20, theta=90, length=200,
                                         allow_intersect=True)

        self.assertEqual(ws.get_shape(), shape)
        self.assertEqual(ws.unique_values().shape[0], nfibers + 1)

    def test_fibers_porosity(self):
        porosity = 0.8
        ws = puma.generate_random_fibers(shape=(100, 100, 100), radius=3, porosity=porosity, phi=20, theta=90, length=200,
                                         allow_intersect=True, max_iter=20)

        self.assertTrue(abs(puma.compute_volume_fraction(ws, (0, 0)) - porosity) < 1e-2, 3)

    def test_fibers_nfibers_nointersection(self):
        nfibers = 100
        ws = puma.generate_random_fibers(shape=(100, 100, 100), radius=3, nfibers=nfibers, phi=20, theta=90, length=200,
                                         allow_intersect=False)

        self.assertEqual(ws.unique_values().shape[0], nfibers + 1)

    def test_fibers_porosity_nointersection(self):
        porosity = 0.9
        ws = puma.generate_random_fibers(shape=(100, 100, 100), radius=3, porosity=porosity, phi=20, theta=90, length=200,
                                         allow_intersect=False, max_iter=20)

        self.assertTrue(abs(puma.compute_volume_fraction(ws, (0, 0)) - porosity) < 1e-2, 3)


class TestTPMS(unittest.TestCase):

    def test_tpms(self):
        shape = (100, 100, 100)  # size of the domain, in voxels.
        w = 0.08  # value of w in the equations above
        q = 0.2  # value of q in the equations above
        ws = puma.generate_tpms(shape, w, q, equation=0)
        np.testing.assert_equal(ws.matrix.shape, (100, 100, 100))
        print(ws.porosity())

        ws = puma.generate_tpms(shape, w, q, equation=1)
        np.testing.assert_equal(ws.matrix.shape, (100, 100, 100))
        print(ws.porosity())

        ws = puma.generate_tpms(shape, w, q, equation=2)
        np.testing.assert_equal(ws.matrix.shape, (100, 100, 100))
        print(ws.porosity())


if __name__ == '__main__':
    unittest.main()
