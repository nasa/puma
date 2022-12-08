import unittest
import numpy as np
import pumapy as puma


class TestTortuosity(unittest.TestCase):

    def setUp(self):
        self.X = 30
        self.Y = 46
        self.Z = 35

        self.ws_channel_x = puma.Workspace.from_array(np.zeros((self.X, self.Y, self.Z)))
        self.ws_channel_x.matrix[:, [0, -1]] = 1
        self.ws_channel_x.matrix[:, :, [0, -1]] = 1

        self.ws_channel_y = puma.Workspace.from_array(np.zeros((self.X, self.Y, self.Z)))
        self.ws_channel_y.matrix[[0, -1]] = 1
        self.ws_channel_y.matrix[:, :, [0, -1]] = 1

        self.ws_channel_z = puma.Workspace.from_array(np.zeros((self.X, self.Y, self.Z)))
        self.ws_channel_z.matrix[[0, -1]] = 1
        self.ws_channel_z.matrix[:, [0, -1]] = 1

    def test_channel_x(self):
        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_x, (0, 0), 'x', side_bc='s',
                                                           solver_type='direct', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff[0], 1, decimal=8)

        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_x, (0, 0), 'y', side_bc='s',
                                                           solver_type='direct', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff, [0, 0, 0], decimal=8)

        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_x, (0, 0), 'z', side_bc='s',
                                                           solver_type='direct', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff, [0, 0, 0], decimal=8)

    def test_channel_y(self):
        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_y, (0, 0), 'x', side_bc='s',
                                                           solver_type='direct', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff, [0, 0, 0], decimal=8)

        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_y, (0, 0), 'y', side_bc='s',
                                                           solver_type='direct', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff[1], 1, decimal=8)

        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_y, (0, 0), 'z', side_bc='s',
                                                           solver_type='direct', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff, [0, 0, 0], decimal=8)

    def test_channel_z(self):
        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_z, (0, 0), 'x', side_bc='s',
                                                           solver_type='direct', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff, [0, 0, 0], decimal=8)

        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_z, (0, 0), 'y', side_bc='s',
                                                           solver_type='direct', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff, [0, 0, 0], decimal=8)

        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_z, (0, 0), 'z', side_bc='s',
                                                           solver_type='direct', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff[2], 1, decimal=8)

    def test_channel_x_bicgstab(self):
        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_x, (0, 0), 'x', side_bc='s', tolerance=1e-5,
                                                           solver_type='bicgstab', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff[0], 1, decimal=5)

    def test_channel_y_bicgstab(self):
        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_y, (0, 0), 'y', side_bc='s', tolerance=1e-5,
                                                           solver_type='bicgstab', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff[1], 1, decimal=5)

    def test_channel_z_bicgstab(self):
        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_z, (0, 0), 'z', side_bc='s', tolerance=1e-5,
                                                           solver_type='bicgstab', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff[2], 1, decimal=5)

    def test_channel_x_bicgstab_mf(self):
        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_x, (0, 0), 'x', side_bc='s', tolerance=1e-5,
                                                           solver_type='bicgstab', matrix_free=True)
        np.testing.assert_array_almost_equal(n_eff[0], 1, decimal=5)

    def test_channel_y_bicgstab_mf(self):
        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_y, (0, 0), 'y', side_bc='s', tolerance=1e-5,
                                                           solver_type='bicgstab', matrix_free=True)
        np.testing.assert_array_almost_equal(n_eff[1], 1, decimal=5)

    def test_channel_z_bicgstab_mf(self):
        n_eff, _, _, _ = puma.compute_continuum_tortuosity(self.ws_channel_z, (0, 0), 'z', side_bc='s', tolerance=1e-5,
                                                           solver_type='bicgstab', matrix_free=True)
        np.testing.assert_array_almost_equal(n_eff[2], 1, decimal=5)

    def test_fiberform(self):
        ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
        n_eff_x, Deff_x, poro, C_x = puma.compute_continuum_tortuosity(ws, (0, 89), 'x', side_bc='s', tolerance=1e-5,
                                                                       solver_type='cg', matrix_free=False)
        np.testing.assert_array_almost_equal(n_eff_x, [1.3133487164843647, 51.124052632447885, -19.286810954243887], decimal=4)
        np.testing.assert_array_almost_equal(Deff_x, [0.6341499325704143, 0.0162909620250135, -0.043182877769470586], decimal=4)
