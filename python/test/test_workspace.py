import unittest
import numpy as np
import pumapy as puma


class TestWorkspace(unittest.TestCase):

    def setUp(self) -> None:
        self.ws = puma.Workspace.from_shape((10, 11, 12))

    def test_WScreation(self):
        ws = puma.Workspace.from_shape((10, 11, 12))
        np.testing.assert_equal(ws.matrix, np.zeros((10, 11, 12), dtype=np.uint16))

        ws = puma.Workspace.from_shape_value((10, 11, 12), 34)
        np.testing.assert_equal(ws.matrix, np.full((10, 11, 12), 34, dtype=np.uint16))

        ws = puma.Workspace.from_array(np.full((10, 11, 12), 34))
        np.testing.assert_equal(ws.matrix, np.full((10, 11, 12), 34, dtype=np.uint16))

        ws = puma.Workspace()
        np.testing.assert_equal(ws.matrix, np.zeros((1, 1, 1), dtype=np.uint16))

        ws = puma.Workspace.from_shape((10, 11, 12), orientation=True)
        np.testing.assert_equal(ws.orientation, np.zeros((10, 11, 12, 3), dtype=np.uint16))

        ws = puma.Workspace.from_shape_vector((10, 11, 12), (1, 0, 0))
        test = np.zeros((10, 11, 12, 3), dtype=float)
        test[:] = list((1, 0, 0))
        np.testing.assert_equal(ws.orientation, test)

        ws = puma.Workspace.from_shape_value_vector((10, 11, 12), 4, (1, 0, 0))
        np.testing.assert_equal(ws.matrix, np.full((10, 11, 12), 4, dtype=np.uint16))
        test = np.zeros((10, 11, 12, 3), dtype=float)
        test[:] = list((1, 0, 0))
        np.testing.assert_equal(ws.orientation, test)

        # wrong shape
        try:
            ws = puma.Workspace.from_shape((1, 1))
        except:
            print("Correctly caught exception.")

        # wrong nparray dims
        try:
            ws = puma.Workspace.from_array(np.full((10, 12), 34))
        except:
            print("Correctly caught exception.")

        # wrong nparray type
        try:
            ws = puma.Workspace.from_array([10, 19])
        except:
            print("Correctly caught exception.")

        # wrong keyword
        try:
            ws = puma.Workspace(wrong=[10, 12])
        except:
            print("Correctly caught exception.")

        # wrong keyword
        try:
            ws = puma.Workspace(size=(20, 11, 12))
        except:
            print("Correctly caught exception.")
            return
        self.assertTrue(False)

    def test_set_voxel_length(self):
        self.ws.set_voxel_length(12)
        np.testing.assert_equal(self.ws.voxel_length, 12)

        # wrong voxel_length
        try:
            self.ws.set_voxel_length((10, 12))
        except:
            print("Correctly caught exception.")
            np.testing.assert_equal(self.ws.voxel_length, 12)
            return
        self.assertTrue(False)

    def test_set_matrix(self):
        matrix = np.random.randint(0, 255, size=self.ws.get_shape(), dtype=np.uint16)
        self.ws.set_matrix(matrix)
        np.testing.assert_equal(self.ws.matrix, matrix)

        # wrong numpy array input
        try:
            self.ws.set_matrix([1, 2, 3])
        except:
            print("Correctly caught exception.")
            np.testing.assert_equal(self.ws.matrix, matrix)

        # wrong numpy array dims
        try:
            ws2 = np.random.randint(0, 255, size=(self.ws.get_shape()[0], self.ws.get_shape()[1]), dtype=np.uint16)
            self.ws.set_matrix(ws2)
        except:
            print("Correctly caught exception.")
            np.testing.assert_equal(self.ws.matrix, matrix)
            return
        self.assertTrue(False)

    def test_copy(self):
        ws = puma.Workspace.from_shape_value((10, 11, 12), 34)
        ws2 = ws.copy()
        np.testing.assert_equal(ws.matrix, ws2.matrix)

    def test_getset(self):
        self.ws[-1, -1, -1] = 77
        np.testing.assert_equal(self.ws[-1, -1, -1], 77)

    def test_get_size(self):
        np.testing.assert_equal(self.ws.get_size(),
                                self.ws.matrix.shape[0] *
                                self.ws.matrix.shape[1] *
                                self.ws.matrix.shape[2])

    def test_len_x(self):
        np.testing.assert_equal(self.ws.len_x(), self.ws.matrix.shape[0])

    def test_len_y(self):
        np.testing.assert_equal(self.ws.len_y(), self.ws.matrix.shape[1])

    def test_len_z(self):
        np.testing.assert_equal(self.ws.len_z(), self.ws.matrix.shape[2])

    def test_get_shape(self):
        np.testing.assert_equal(self.ws.get_shape(), self.ws.matrix.shape)

    def test_min(self):
        np.testing.assert_equal(self.ws.min(), self.ws.matrix.min())

    def test_max(self):
        np.testing.assert_equal(self.ws.max(), self.ws.matrix.max())

    def test_average(self):
        np.testing.assert_equal(self.ws.average(), self.ws.matrix.mean())

    def test_resize_matrix(self):
        self.ws.resize_new_matrix((12, 3, 5))
        np.testing.assert_equal(self.ws.matrix, np.zeros((12, 3, 5), dtype=np.uint16))

    def test_resize_matrixwithval(self):
        self.ws.resize_new_matrix((12, 3, 5), 4)
        np.testing.assert_equal(self.ws.matrix, np.full((12, 3, 5), 4, dtype=np.uint16))

    def test_resize_orientation(self):
        self.ws.resize_new_orientation((12, 3, 5))
        np.testing.assert_equal(self.ws.orientation, np.zeros((12, 3, 5, 3), dtype=np.uint16))

    def test_resize_orientationwithval(self):
        self.ws.resize_new_orientation((12, 3, 5), (1, 0, 0))
        np.testing.assert_equal(self.ws.orientation, np.tile([1, 0, 0], (12, 3, 5, 1)).astype(float))

    def test_set(self):
        self.ws.set(matrix_value=36, orientation_value=(1, 0, 0))
        np.testing.assert_equal(self.ws.matrix, np.full((10, 11, 12), 36, dtype=np.uint16))
        np.testing.assert_equal(self.ws.orientation, np.tile([1, 0, 0], (10, 11, 12, 1)).astype(float))

    def test_set_material_id(self):
        matrix = np.random.randint(0, 255, size=self.ws.get_shape())
        self.ws.matrix = matrix.copy()
        self.ws.set_material_id((0, 127), 0)
        self.ws.set_material_id((128, 255), 1)
        matrix[matrix <= 127] = 0
        matrix[matrix >= 128] = 1
        np.testing.assert_equal(self.ws.matrix, matrix)

    def test_binarize(self):
        matrix = np.random.randint(0, 255, size=self.ws.get_shape())
        self.ws.matrix = matrix.copy()
        self.ws.binarize(128)
        matrix[matrix < 128] = 0
        matrix[matrix >= 128] = 1
        np.testing.assert_equal(self.ws.matrix, matrix)

    def test_set_material_id_vector(self):
        test = np.zeros((10, 11, 12, 3), dtype=float)
        test[:] = list((1, 0, 0))
        self.ws = puma.Workspace.from_shape_vector((10, 11, 12), (1, 0, 0))
        self.ws.set_material_id((0, 127), 0)
        np.testing.assert_equal(self.ws.orientation, test)

    def test_binarize_vector(self):
        test = np.zeros((10, 11, 12, 3), dtype=float)
        test[:] = list((1, 0, 0))
        self.ws = puma.Workspace.from_shape_vector((10, 11, 12), (1, 0, 0))
        self.ws.binarize(128)
        np.testing.assert_equal(self.ws.orientation, test)

    def test_porosity(self):
        ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
        np.testing.assert_almost_equal(ws.porosity(cutoff=(0, 89)), 0.83286)

    def test_rescale(self):
        ws = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
        ws_cp = ws.copy()
        ws_cp.rescale(0.5, segmented=False)
        np.testing.assert_almost_equal(ws_cp.matrix.shape, (100, 100, 100))
        porosity_diff = abs(ws_cp.porosity(cutoff=(0, 89)) - ws.porosity(cutoff=(0, 89)))
        self.assertLess(porosity_diff, 0.0011)

    def test_resize(self):
        ws = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
        ws_cp = ws.copy()
        ws_cp.resize((150, 100, 79), segmented=False)
        np.testing.assert_almost_equal(ws_cp.matrix.shape, (150, 100, 79))
        porosity_diff = abs(ws_cp.porosity(cutoff=(0, 89)) - ws.porosity(cutoff=(0, 89)))
        self.assertLess(porosity_diff, 0.001)

    def test_rotate(self):
        ws = puma.Workspace.from_shape((30, 10, 40))
        ws.matrix[0:ws.matrix.shape[0] // 2] = 1

        ws_cp = ws.copy()
        ws_cp.rotate(90, 'z')
        self.assertEqual((10, 30, 40), ws_cp.matrix.shape)
        self.assertEqual(ws.porosity(), ws_cp.porosity())

        ws_cp = ws.copy()
        ws_cp.rotate(90, 'z', reshape=False)
        self.assertEqual((30, 10, 40), ws_cp.matrix.shape)
        self.assertEqual(ws.porosity(), ws_cp.porosity())

        ws_cp = ws.copy()
        ws_cp.rotate(90, 'x')
        self.assertEqual((30, 40, 10), ws_cp.matrix.shape)
        self.assertEqual(ws.porosity(), ws_cp.porosity())

        ws_cp = ws.copy()
        ws_cp.rotate(90, 'y')
        self.assertEqual((40, 10, 30), ws_cp.matrix.shape)
        self.assertEqual(ws.porosity(), ws_cp.porosity())

if __name__ == '__main__':
    unittest.main()
