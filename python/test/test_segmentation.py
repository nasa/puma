import unittest
import numpy as np
import pumapy as puma


class TestSegmentation(unittest.TestCase):

    def test_identify_porespace(self):
        ws = puma.generate_sphere((100, 100, 100), (50, 50, 50), 40, segmented=False)
        ws_cp = ws.copy()
        ws_cp.binarize_range((1, 255))

        ws.binarize_range((1, 253))
        pores = puma.identify_porespace(ws, (1, 1))

        np.testing.assert_equal(np.where(pores == 0), np.where(ws.matrix == 1))
        np.testing.assert_equal(np.where(pores == 1, 1, 0).sum(), np.where(ws_cp.matrix == 0, 1, 0).sum())

    def test_fill_closed_pores(self):
        ws = puma.generate_random_spheres((100, 100, 100), diameter=20, porosity=0.8, allow_intersect=True, segmented=False)
        ws_cp = ws.copy()
        ws_cp.binarize_range((1, 250))

        ws.binarize_range((1, 250))
        filled_ws, pores = puma.fill_closed_pores(ws, (1, 1), fill_value=2, return_pores=True)

        np.testing.assert_equal(np.where(pores == 0), np.where(ws.matrix == 1))
        np.testing.assert_equal(np.where(pores >= 1, 1, 0).sum(), np.where(ws_cp.matrix == 0, 1, 0).sum())
        np.testing.assert_equal(np.where(pores >= 2), np.where(filled_ws.matrix == 2))


if __name__ == '__main__':
    unittest.main()
