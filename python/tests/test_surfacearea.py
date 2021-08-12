import unittest
import numpy as np
import pumapy as puma
from skimage import io


class TestSurfacearea(unittest.TestCase):

    def test_fiberform(self):
        ws = puma.Workspace.from_array(io.imread(puma.path_to_example_file("200_fiberform.tif")))
        area, specific_area = puma.compute_surface_area(ws, (90, 255), False)

        np.testing.assert_almost_equal(area, 4.12090e-7, decimal=5)
        np.testing.assert_almost_equal(specific_area, 52291.746, decimal=3)


if __name__ == '__main__':
    unittest.main()
