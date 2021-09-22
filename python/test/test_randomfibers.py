import unittest
import pumapy as puma
import numpy as np
np.random.seed(1)


class TestRandomFibers(unittest.TestCase):

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


if __name__ == '__main__':
    unittest.main()
