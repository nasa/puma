import unittest
import pumapy as puma
import numpy as np


class TestFilters(unittest.TestCase):
    
    def setUp(self):
        self.ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
        self.shape = self.ws.get_shape()
        
    def test_median(self):
        puma.filter_median(self.ws, size=10)
        
        np.testing.assert_equal(self.ws.matrix.shape, self.shape)

    def test_gaussian(self):
        puma.compute_orientation_st(self.ws, cutoff=(90, 255))
        puma.filter_gaussian(self.ws, sigma=2, apply_on_orientation=True)
        
        np.testing.assert_equal(self.ws.matrix.shape, self.shape)
    
    def test_edt(self):
        puma.filter_edt(self.ws, cutoff=(90, 255))
        
        np.testing.assert_equal(self.ws.matrix.shape, self.shape)
    
    def test_mean(self):
        puma.filter_mean(self.ws, size=10)

        np.testing.assert_equal(self.ws.matrix.shape, self.shape)
    
    def test_erode(self):
        puma.filter_erode(self.ws, (90, 255), size=6)

        np.testing.assert_equal(self.ws.matrix.shape, self.shape)
    
    def test_dilate(self):
        puma.filter_dilate(self.ws, cutoff=(90, 255), size=5)

        np.testing.assert_equal(self.ws.matrix.shape, self.shape)
    
    def test_opening(self):
        puma.filter_opening(self.ws, cutoff=(90, 255), size=3)

        np.testing.assert_equal(self.ws.matrix.shape, self.shape)
    
    def test_closing(self):
        puma.filter_closing(self.ws, cutoff=(90, 255), size=3)

        np.testing.assert_equal(self.ws.matrix.shape, self.shape)
    

if __name__ == '__main__':
    unittest.main()
