import unittest
import numpy as np
import pumapy as puma


class TestFiniteVolumeIsotropicTC(unittest.TestCase):

    def setUp(self):
        self.X = 10
        self.Y = 8
        self.Z = 14

        self.ws_homoiso = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.cond_map_homoiso = puma.IsotropicConductivityMap()
        self.cond_map_homoiso.add_material((1, 1), 10)
        # 10   0   0
        #  0  10   0
        #  0   0  10

        self.solution_homoiso_x = np.zeros((self.X, self.Y, self.Z), dtype=float)
        for i in range(self.X):
            self.solution_homoiso_x[i] = i / (self.X - 1.)

        self.solution_homoiso_y = np.zeros((self.X, self.Y, self.Z), dtype=float)
        for j in range(self.Y):
            self.solution_homoiso_y[:, j] = j / (self.Y - 1.)

        self.solution_homoiso_z = np.zeros((self.X, self.Y, self.Z), dtype=float)
        for k in range(self.Z):
            self.solution_homoiso_z[:, :, k] = k / (self.Z - 1.)

        self.ws_matSeriesInx = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.ws_matSeriesInx[int(self.ws_matSeriesInx.matrix.shape[0] / 2.):, :] = 2  # in series in x

        self.ws_matSeriesIny = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.ws_matSeriesIny[:, int(self.ws_matSeriesIny.matrix.shape[1] / 2.):] = 2  # in series in y

        self.ws_matSeriesInz = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.ws_matSeriesInz[:, :, int(self.ws_matSeriesInz.matrix.shape[2] / 2.):] = 2  # in series in z

        self.cond_map_matSeries = puma.IsotropicConductivityMap()
        self.cond_map_matSeries.add_material((1, 1), 10)
        self.cond_map_matSeries.add_material((2, 2), 1)
        # conductivity solution tensor for mat in series along x
        # 1.8181   0   0
        #      0 5.5   0
        #      0   0 5.5

        # conductivity solution tensor for mat in series along y
        # 5.5      0    0
        #   0 1.8181    0
        #   0      0  5.5

        # conductivity solution tensor for mat in series along z
        # 5.5   0      0
        #   0 5.5      0
        #   0   0 1.8181

    def test_homoiso_x(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_homoiso, self.cond_map_homoiso, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [10, 0., 0.])
        np.testing.assert_array_almost_equal(T, self.solution_homoiso_x)

    def test_homoiso_y(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_homoiso, self.cond_map_homoiso, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0., 10, 0.])
        np.testing.assert_array_almost_equal(T, self.solution_homoiso_y)

    def test_homoiso_z(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_homoiso, self.cond_map_homoiso, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0., 0., 10])
        np.testing.assert_array_almost_equal(T, self.solution_homoiso_z)

    def test_matSeriesInx_x_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [1.818181818, 0., 0.])

    def test_matSeriesInx_y_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0., 5.5, 0.])

    def test_matSeriesInx_z_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0., 0., 5.5])

    def test_matSeriesIny_x_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesIny, self.cond_map_matSeries, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [5.5, 0., 0.])

    def test_matSeriesIny_y_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesIny, self.cond_map_matSeries, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0., 1.818181818, 0.])

    def test_matSeriesIny_z_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesIny, self.cond_map_matSeries, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0., 0., 5.5])

    def test_matSeriesInz_x_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInz, self.cond_map_matSeries, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [5.5, 0., 0.])

    def test_matSeriesInz_y_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInz, self.cond_map_matSeries, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0., 5.5, 0.])

    def test_matSeriesInz_z_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInz, self.cond_map_matSeries, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0., 0., 1.818181818])

    def test_matSeriesInx_x_sym(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'x', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [1.818181818, 0., 0.])

    def test_matSeriesInx_y_sym(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'y', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0., 5.5, 0.])

    def test_matSeriesInx_z_sym(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'z', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0., 0., 5.5])

    def test_matSeriesInx_x_sym_bicgstab(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'x', 's', solver_type='bicgstab', matrix_free=False)
        np.testing.assert_array_almost_equal(keff, [1.818181818, 0., 0.], decimal=4)

    def test_matSeriesInx_y_sym_bicgstab(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'y', 's', solver_type='bicgstab', matrix_free=False)
        np.testing.assert_array_almost_equal(keff, [0., 5.5, 0.], decimal=4)

    def test_matSeriesInx_z_sym_bicgstab(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'z', 's', solver_type='bicgstab', matrix_free=False)
        np.testing.assert_array_almost_equal(keff, [0., 0., 5.5], decimal=4)

    def test_matSeriesInx_x_sym_bicgstab_mf(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'x', 's', solver_type='bicgstab', matrix_free=True)
        np.testing.assert_array_almost_equal(keff, [1.818181818, 0., 0.], decimal=4)

    def test_matSeriesInx_y_sym_bicgstab_mf(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'y', 's', solver_type='bicgstab', matrix_free=True)
        np.testing.assert_array_almost_equal(keff, [0., 5.5, 0.], decimal=4)

    def test_matSeriesInx_z_sym_bicgstab_mf(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'z', 's', solver_type='bicgstab', matrix_free=True)
        np.testing.assert_array_almost_equal(keff, [0., 0., 5.5], decimal=4)

    # def test_artfib50_prescribed_bc(self):
    #     ws = puma.import_vti("testdata/artifib.vtk")
    #     cond_map = puma.IsotropicConductivityMap()
    #     cond_map.add_material((0, 128), 0.0257)
    #     cond_map.add_material((129, 255), 10)

        # bc = puma.IsotropicConductivityBC(ws)
        # bc.dirichlet[0] = 0
        # bc.dirichlet[-1] = 1
        # keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'x', 'p', tolerance=1e-5, matrix_free=False)
        # puma.plot_conductivity_fields(ws, T, q)
        # keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, '', 'p', prescribed_bc=bc, tolerance=1e-5, matrix_free=False)
        # puma.plot_conductivity_fields(ws, T, q)
        # print(keff)
        # np.testing.assert_array_almost_equal(keff, [0.08925413601635017, 0.0007061955182692809, 0.0034755766404363747], decimal=4)

        # bc = puma.IsotropicConductivityBC(ws)
        # bc.dirichlet[:, 0] = 0
        # bc.dirichlet[:, -1] = 1
        # keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'y', 'p', prescribed_bc=bc, tolerance=1e-5, matrix_free=False)
        # puma.plot_conductivity_fields(ws, T, q)
        # keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, '', 'p', prescribed_bc=bc, tolerance=1e-5, matrix_free=False)
        # print(keff)

        # puma.plot_conductivity_fields(ws, T, q)
        # np.testing.assert_array_almost_equal(keff, [0.0001529471806702979, 0.1507727041868121, 0.0011563593733770895], decimal=4)
        # bc = puma.IsotropicConductivityBC(ws)
        # bc.dirichlet[:, :, 0] = 0
        # bc.dirichlet[:, :, -1] = 1
        # keff, _, _ = puma.compute_thermal_conductivity(ws, cond_map, '', 's', prescribed_bc=bc, tolerance=1e-5, matrix_free=False)
        # np.testing.assert_array_almost_equal(keff, [0.0009825604753153855, -0.006340682953106483, 0.07495772275364905], decimal=4)


class TestFiniteVolumeAnisotropicTC(unittest.TestCase):

    def setUp(self):
        self.X = 10
        self.Y = 8
        self.Z = 14

        self.ws_homoiso = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.cond_map_homoiso = puma.AnisotropicConductivityMap()
        self.cond_map_homoiso.add_material((1, 1), 10, 11, 12, 0.5, 0.2, 0.4)
        # conductivity solution tensor for homogeneous
        # 10  0.5 0.2
        # 0.5  11 0.4
        # 0.2 0.4, 12

        self.solution_homoiso_x = np.zeros((self.X, self.Y, self.Z), dtype=float)
        step = (1. / self.X) / 2
        acc = step
        for ind in range(self.X):
            self.solution_homoiso_x[ind] = acc
            acc += 2 * step

        self.solution_homoiso_y = np.zeros((self.X, self.Y, self.Z), dtype=float)
        step = (1. / self.Y) / 2
        acc = step
        for ind in range(self.Y):
            self.solution_homoiso_y[:, ind] = acc
            acc += 2 * step

        self.solution_homoiso_z = np.zeros((self.X, self.Y, self.Z), dtype=float)
        step = (1. / self.Z) / 2
        acc = step
        for ind in range(self.Z):
            self.solution_homoiso_z[:, :, ind] = acc
            acc += 2 * step

        self.ws_matSeriesInx = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.ws_matSeriesInx[int(self.ws_matSeriesInx.matrix.shape[0] / 2.):, :] = 2  # in series in x

        self.ws_matSeriesIny = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.ws_matSeriesIny[:, int(self.ws_matSeriesIny.matrix.shape[1] / 2.):] = 2  # in series in y

        self.ws_matSeriesInz = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.ws_matSeriesInz[:, :, int(self.ws_matSeriesInz.matrix.shape[2] / 2.):] = 2  # in series in z

        self.cond_map_matSeries = puma.AnisotropicConductivityMap()
        self.cond_map_matSeries.add_material((1, 1), 10, 10, 10, 0.2, 0.3, 0.5)
        self.cond_map_matSeries.add_material((2, 2), 1, 1, 1, 0.2, 0.3, 0.5)
        # conductivity solution tensor for mat in series along x
        # 1.8181 0.2 0.3
        #    0.2 5.5 0.5
        #    0.3 0.5 5.5

        # conductivity solution tensor for mat in series along y
        # 5.5    0.2 0.3
        # 0.2 1.8181 0.5
        # 0.3    0.5 5.5

        # conductivity solution tensor for mat in series along z
        # 5.5 0.2    0.3
        # 0.2 5.5    0.5
        # 0.3 0.5 1.8181

    def test_homoiso_x(self):
        keff, T, q = puma.compute_thermal_conductivity(self.ws_homoiso, self.cond_map_homoiso, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [10, 0.5, 0.2])
        np.testing.assert_array_almost_equal(T, self.solution_homoiso_x)

    def test_homoiso_y(self):
        keff, T, q = puma.compute_thermal_conductivity(self.ws_homoiso, self.cond_map_homoiso, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0.5, 11, 0.4])
        np.testing.assert_array_almost_equal(T, self.solution_homoiso_y)

    def test_homoiso_z(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_homoiso, self.cond_map_homoiso, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0.2, 0.4, 12])
        np.testing.assert_array_almost_equal(T, self.solution_homoiso_z)

    def test_matSeriesInx_x_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [1.818181818, 0.2, 0.3])

    def test_matSeriesInx_y_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0.2, 5.5, 0.5])

    def test_matSeriesInx_z_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0.3, 0.5, 5.5])

    def test_matSeriesIny_x_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesIny, self.cond_map_matSeries, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [5.5, 0.2, 0.3])

    def test_matSeriesIny_y_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesIny, self.cond_map_matSeries, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0.2, 1.818181818, 0.5])

    def test_matSeriesIny_z_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesIny, self.cond_map_matSeries, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0.3, 0.5, 5.5])

    def test_matSeriesInz_x_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInz, self.cond_map_matSeries, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [5.5, 0.2, 0.3])

    def test_matSeriesInz_y_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInz, self.cond_map_matSeries, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0.2, 5.5, 0.5])

    def test_matSeriesInz_z_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInz, self.cond_map_matSeries, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0.3, 0.5, 1.818181818])

    def test_matSeriesInx_x_sym(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'x', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [1.818181818, 0.2, 0.3])

    def test_matSeriesInx_y_sym(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'y', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0.2, 5.5, 0.5])

    def test_matSeriesInx_z_sym(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'z', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0.3, 0.5, 5.5])

    def test_matSeriesInx_x_sym_bicgstab(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'x', 's', solver_type='bicgstab')
        np.testing.assert_array_almost_equal(keff, [1.818181818, 0.2, 0.3], decimal=4)

    def test_matSeriesInx_y_sym_bicgstab(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'y', 's', solver_type='bicgstab', tolerance=1e-6)
        np.testing.assert_array_almost_equal(keff, [0.2, 5.5, 0.5], decimal=4)

    def test_matSeriesInx_z_sym_bicgstab(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'z', 's', solver_type='bicgstab', tolerance=1e-6)
        np.testing.assert_array_almost_equal(keff, [0.3, 0.5, 5.5], decimal=4)

    def test_tensor_rotation_x(self):
        ws = puma.Workspace.from_array(np.zeros((self.X, self.Y, self.Z)))
        ws.set(1, (1, 0, 0))
        cond_map = puma.AnisotropicConductivityMap()
        cond_map.add_material_to_orient((1, 1), 10, 1)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'x', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [10, 0, 0], decimal=6)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'y', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0, 1, 0], decimal=6)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'z', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0, 0, 1], decimal=6)

    def test_tensor_rotation_y(self):
        ws = puma.Workspace.from_array(np.zeros((self.X, self.Y, self.Z)))
        ws.set(1, (0, 1, 0))
        cond_map = puma.AnisotropicConductivityMap()
        cond_map.add_material_to_orient((1, 1), 10, 1)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'x', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [1, 0, 0], decimal=6)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'y', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0, 10, 0], decimal=6)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'z', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0, 0, 1], decimal=6)

    def test_tensor_rotation_z(self):
        ws = puma.Workspace.from_array(np.zeros((self.X, self.Y, self.Z)))
        ws.set(1, (0, 0, 1))
        cond_map = puma.AnisotropicConductivityMap()
        cond_map.add_material_to_orient((1, 1), 10, 1)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'x', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [1, 0, 0], decimal=6)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'y', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0, 1, 0], decimal=6)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'z', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(keff, [0, 0, 10], decimal=6)

    def test_artfib50_prescribed_bc(self):
        ws = puma.import_vti("testdata/artifib.vtk")
        cond_map = puma.AnisotropicConductivityMap()
        cond_map.add_isotropic_material((0, 128), 0.0257)
        cond_map.add_isotropic_material((129, 255), 10)
        bc = puma.AnisotropicConductivityBC(ws)
        bc.xfaces[0] = 0
        bc.xfaces[1] = 1
        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, '', 's', prescribed_bc=bc, tolerance=1e-5, matrix_free=False)
        print(keff)
        np.testing.assert_array_almost_equal(keff, [0.08943850091779587, 0.0009302391577982926, 0.003266055217241104], decimal=3)
        bc = puma.AnisotropicConductivityBC(ws)
        bc.yfaces[0] = 0
        bc.yfaces[1] = 1
        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, '', 's', prescribed_bc=bc, tolerance=1e-5, matrix_free=False)
        print(keff)
        np.testing.assert_array_almost_equal(keff, [0.0006042199183454983, 0.1524122632672828, 0.0017681983127303493], decimal=3)
        bc = puma.AnisotropicConductivityBC(ws)
        bc.zfaces[0] = 0
        bc.zfaces[1] = 1
        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, '', 'p', prescribed_bc=bc, tolerance=1e-5, matrix_free=False)
        np.testing.assert_array_almost_equal(keff, [-0.004294621647451377, 0.009307678810456408, 0.13749729264962243], decimal=3)
        print(keff)

    def test_artfib50_orient_prescribed_bc(self):
        ws = puma.import_vti("testdata/artifib.vtk")
        ws.orientation = puma.import_vti("testdata/artifib_orient.vtk", import_ws=False)["PuMA_Vec3"]

        cond_map = puma.AnisotropicConductivityMap()
        cond_map.add_isotropic_material((0, 128), 0.0257)
        cond_map.add_material_to_orient((129, 255), 10, 1)

        bc = puma.AnisotropicConductivityBC(ws)
        bc.xfaces[0] = 0
        bc.xfaces[1] = 1
        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, '', 's', tolerance=1e-5, prescribed_bc=bc)
        # puma.plot_conductivity_fields(ws, T, q)
        np.testing.assert_array_almost_equal(keff, [0.06712707273637297, -0.0007017851098093986, 0.0017924686637479958], decimal=4)
        bc = puma.AnisotropicConductivityBC(ws)
        bc.yfaces[0] = 0
        bc.yfaces[1] = 1
        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, '', 's', tolerance=1e-5, prescribed_bc=bc)
        # puma.plot_conductivity_fields(ws, T, q)
        np.testing.assert_array_almost_equal(keff, [0.003760128942078807, 0.07922692884170093, 0.003888392944562686], decimal=4)
        bc = puma.AnisotropicConductivityBC(ws)
        bc.zfaces[0] = 0
        bc.zfaces[1] = 1
        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, '', 's', tolerance=1e-5, prescribed_bc=bc)
        # puma.plot_conductivity_fields(ws, T, q)
        np.testing.assert_array_almost_equal(keff, [0.0015291100308031456, -0.0016663003168615753, 0.044952464988095735], decimal=4)


class TestFiniteElementAnisotropicTC(unittest.TestCase):

    def setUp(self):
        self.X = 10
        self.Y = 8
        self.Z = 14

        self.ws_homoiso = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.cond_map_homoiso = puma.AnisotropicConductivityMap()
        self.cond_map_homoiso.add_material((1, 1), 10, 11, 12, 0.5, 0.2, 0.4)
        # conductivity solution tensor for homogeneous
        # 10  0.5 0.2
        # 0.5  11 0.4
        # 0.2 0.4, 12

        self.ws_matSeriesInx = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.ws_matSeriesInx[int(self.ws_matSeriesInx.matrix.shape[0] / 2.):, :] = 2  # in series in x

        self.ws_matSeriesIny = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.ws_matSeriesIny[:, int(self.ws_matSeriesIny.matrix.shape[1] / 2.):] = 2  # in series in y

        self.ws_matSeriesInz = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.ws_matSeriesInz[:, :, int(self.ws_matSeriesInz.matrix.shape[2] / 2.):] = 2  # in series in z

        self.cond_map_matSeries = puma.AnisotropicConductivityMap()
        self.cond_map_matSeries.add_material((1, 1), 10, 10, 10, 0.2, 0.3, 0.5)
        self.cond_map_matSeries.add_material((2, 2), 1, 1, 1, 0.2, 0.3, 0.5)
        # conductivity solution tensor for mat in series along x
        # 1.8181 0.2 0.3
        #    0.2 5.5 0.5
        #    0.3 0.5 5.5

        # conductivity solution tensor for mat in series along y
        # 5.5    0.2 0.3
        # 0.2 1.8181 0.5
        # 0.3    0.5 5.5

        # conductivity solution tensor for mat in series along z
        # 5.5 0.2    0.3
        # 0.2 5.5    0.5
        # 0.3 0.5 1.8181

    def test_homoiso_x(self):
        keff, T, q = puma.compute_thermal_conductivity(self.ws_homoiso, self.cond_map_homoiso, 'x', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [10, 0.5, 0.2])

    def test_homoiso_y(self):
        keff, T, q = puma.compute_thermal_conductivity(self.ws_homoiso, self.cond_map_homoiso, 'y', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0.5, 11, 0.4])

    def test_homoiso_z(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_homoiso, self.cond_map_homoiso, 'z', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0.2, 0.4, 12])

    def test_matSeriesInx_x_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'x', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [1.818181818, 0.2, 0.3])

    def test_matSeriesInx_y_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'y', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0.2, 5.5, 0.5])

    def test_matSeriesInx_z_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'z', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0.3, 0.5, 5.5])

    def test_matSeriesIny_x_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesIny, self.cond_map_matSeries, 'x', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [5.5, 0.2, 0.3])

    def test_matSeriesIny_y_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesIny, self.cond_map_matSeries, 'y', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0.2, 1.818181818, 0.5])

    def test_matSeriesIny_z_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesIny, self.cond_map_matSeries, 'z', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0.3, 0.5, 5.5])

    def test_matSeriesInz_x_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInz, self.cond_map_matSeries, 'x', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [5.5, 0.2, 0.3])

    def test_matSeriesInz_y_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInz, self.cond_map_matSeries, 'y', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0.2, 5.5, 0.5])

    def test_matSeriesInz_z_per(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInz, self.cond_map_matSeries, 'z', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0.3, 0.5, 1.818181818])

    def test_matSeriesInx_x_sym(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'x', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [1.818181818, 0.2, 0.3])

    def test_matSeriesInx_x_sym_bicgstab(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'x', solver_type='bicgstab', tolerance=1e-6, method='fe', matrix_free=False)
        np.testing.assert_array_almost_equal(keff, [1.818181818, 0.2, 0.3], decimal=4)

    def test_matSeriesInx_y_sym_bicgstab(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'y', solver_type='bicgstab', tolerance=1e-6, method='fe', matrix_free=False)
        np.testing.assert_array_almost_equal(keff, [0.2, 5.5, 0.5], decimal=4)

    def test_matSeriesInx_z_sym_bicgstab(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'z', solver_type='bicgstab', tolerance=1e-6, method='fe', matrix_free=False)
        np.testing.assert_array_almost_equal(keff, [0.3, 0.5, 5.5], decimal=4)

    def test_matSeriesInx_y_sym_minres(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'y', solver_type='minres', tolerance=1e-6, method='fe', matrix_free=True)
        np.testing.assert_array_almost_equal(keff, [0.2, 5.5, 0.5], decimal=4)

    def test_matSeriesInx_z_sym_gmres(self):
        keff, T, _ = puma.compute_thermal_conductivity(self.ws_matSeriesInx, self.cond_map_matSeries, 'z', solver_type='gmres', tolerance=1e-6, method='fe', matrix_free=True)
        np.testing.assert_array_almost_equal(keff, [0.3, 0.5, 5.5], decimal=4)

    def test_tensor_rotation_x(self):
        ws = puma.Workspace.from_array(np.zeros((self.X, self.Y, self.Z)))
        ws.set(1, (1, 0, 0))
        cond_map = puma.AnisotropicConductivityMap()
        cond_map.add_material_to_orient((1, 1), 10, 1)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'x', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [10, 0, 0], decimal=6)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'y', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0, 1, 0], decimal=6)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'z', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0, 0, 1], decimal=6)

    def test_tensor_rotation_y(self):
        ws = puma.Workspace.from_array(np.zeros((self.X, self.Y, self.Z)))
        ws.set(1, (0, 1, 0))
        cond_map = puma.AnisotropicConductivityMap()
        cond_map.add_material_to_orient((1, 1), 10, 1)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'x', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [1, 0, 0], decimal=6)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'y', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0, 10, 0], decimal=6)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'z', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0, 0, 1], decimal=6)

    def test_tensor_rotation_z(self):
        ws = puma.Workspace.from_array(np.zeros((self.X, self.Y, self.Z)))
        ws.set(1, (0, 0, 1))
        cond_map = puma.AnisotropicConductivityMap()
        cond_map.add_material_to_orient((1, 1), 10, 1)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'x', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [1, 0, 0], decimal=6)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'y', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0, 1, 0], decimal=6)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'z', solver_type='direct', method='fe')
        np.testing.assert_array_almost_equal(keff, [0, 0, 10], decimal=6)

    def test_artfib50_orient_prescribed_bc(self):
        ws = puma.import_vti("testdata/artifib.vtk")
        ws.orientation = puma.import_vti("testdata/artifib_orient.vtk", import_ws=False)["PuMA_Vec3"]

        cond_map = puma.AnisotropicConductivityMap()
        cond_map.add_isotropic_material((0, 128), 0.0257)
        cond_map.add_material_to_orient((129, 255), 10, 1)

        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'x', solver_type='minres', tolerance=1e-8, matrix_free=False, method='fe')
        np.testing.assert_array_almost_equal(keff, [0.06745428716343704, -0.004375283607634153, 0.0011587203619874528], decimal=4)
        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'y', solver_type='minres', tolerance=1e-8, matrix_free=False, method='fe')
        np.testing.assert_array_almost_equal(keff, [-0.004394957645331049, 0.07009870980864435, -0.00521480227586068], decimal=4)
        keff, T, q = puma.compute_thermal_conductivity(ws, cond_map, 'z', solver_type='minres', tolerance=1e-8, matrix_free=False, method='fe')
        np.testing.assert_array_almost_equal(keff, [0.0011873151479020543, -0.005200248935462725, 0.052539850175818394], decimal=4)


if __name__ == '__main__':
    unittest.main()
