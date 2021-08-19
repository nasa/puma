import unittest
import pumapy as puma


class TestOrientationST(unittest.TestCase):

    def test_artfib(self):
        ws1 = puma.import_vti("testdata/artifib.vtk")
        puma.compute_orientation_st(ws1, 0.7, 1.1, (128, 255))
        ws2 = puma.import_vti("testdata/artifib_orient.vtk")
        error, mean, std = puma.compute_angular_differences(ws1.matrix, ws1.orientation, ws2.orientation, (128, 255))
        self.assertTrue(mean < 5 and std < 10)

    def test_artfib_edt(self):
        ws1 = puma.import_vti("testdata/artifib.vtk")
        puma.compute_orientation_st(ws1, 0.7, 1.1, (128, 255), edt=True)
        ws2 = puma.import_vti("testdata/artifib_orient.vtk")
        error, mean, std = puma.compute_angular_differences(ws1.matrix, ws1.orientation, ws2.orientation, (128, 255))
        print(mean, std)
        self.assertTrue(mean < 6 and std < 10)


if __name__ == '__main__':
    unittest.main()
