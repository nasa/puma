import unittest
import numpy as np
import pumapy as puma
from pumapy.physicsmodels.mpsa_elasticity import Elasticity
import scipy.sparse
import os


class TestElasticity(unittest.TestCase):

    def setUp(self):
        self.X = 6
        self.Y = 4
        self.Z = 8

        self.ws_homoiso = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        self.elast_map_homoiso = puma.ElasticityMap()
        self.elast_map_homoiso.add_material((1, 1),
                                            1, 0.1, 0.2, 0.3, 0.4, 0.5,
                                            2, 0.6, 0.7, 0.8, 0.9,
                                            3, 0.11, 0.12, 0.13,
                                            4, 0.14, 0.15,
                                            5, 0.16,
                                            6)
        # elasticity solution tensor for homogeneous
        #    1       0.1     0.2     0.3     0.4     0.5
        #    0.1       2     0.6     0.7     0.8     0.9
        #    0.2     0.6     3       0.11    0.12    0.13
        #    0.3     0.7     0.11    4       0.14    0.15
        #    0.4     0.8     0.12    0.14    5       0.16
        #    0.5     0.9     0.13    0.15    0.16    6

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

        self.elast_matSeries = puma.ElasticityMap()
        self.elast_matSeries.add_material((1, 1),
                                          10, 0.2, 0.3, 0, 0, 0,
                                          10, 0.5, 0, 0, 0,
                                          10, 0, 0, 0,
                                          0.5, 0, 0,
                                          0.5, 0,
                                          0.5)
        self.elast_matSeries.add_material((2, 2),
                                          1, 0.2, 0.3, 0, 0, 0,
                                          1, 0.5, 0, 0, 0,
                                          1, 0, 0, 0,
                                          0.5, 0, 0,
                                          0.5, 0,
                                          0.5)
        # elasticity solution tensor for mat in series along x
        # 1.8181 0.2 0.3
        #    0.2 5.5 0.5
        #    0.3 0.5 5.5

        # elasticity solution tensor for mat in series along y
        # 5.5    0.2 0.3
        # 0.2 1.8181 0.5
        # 0.3    0.5 5.5

        # elasticity solution tensor for mat in series along z
        # 5.5 0.2    0.3
        # 0.2 5.5    0.5
        # 0.3 0.5 1.8181

    def test_homoiso_x(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_homoiso, self.elast_map_homoiso, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [1, 0.1, 0.2, 0.3, 0.4, 0.5])
        np.testing.assert_array_almost_equal(u[:, :, :, 0], self.solution_homoiso_x)

    def test_homoiso_y(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_homoiso, self.elast_map_homoiso, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [0.1, 2, 0.6, 0.7, 0.8, 0.9])
        np.testing.assert_array_almost_equal(u[:, :, :, 1], self.solution_homoiso_y)

    def test_homoiso_z(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_homoiso, self.elast_map_homoiso, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [0.2, 0.6, 3, 0.11, 0.12, 0.13])
        np.testing.assert_array_almost_equal(u[:, :, :, 2], self.solution_homoiso_z)

    def test_matSeriesInx_x_per(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesInx, self.elast_matSeries, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [1.818181818, 0.2, 0.3, 0, 0, 0])

    def test_matSeriesInx_y_per(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesInx, self.elast_matSeries, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [0.2, 5.5, 0.5, 0, 0, 0])

    def test_matSeriesInx_z_per(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesInx, self.elast_matSeries, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [0.3, 0.5, 5.5, 0, 0, 0])

    def test_matSeriesIny_x_per(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesIny, self.elast_matSeries, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [5.5, 0.2, 0.3, 0, 0, 0])

    def test_matSeriesIny_y_per(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesIny, self.elast_matSeries, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [0.2, 1.818181818, 0.5, 0, 0, 0])

    def test_matSeriesIny_z_per(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesIny, self.elast_matSeries, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [0.3, 0.5, 5.5, 0, 0, 0])

    def test_matSeriesInz_x_per(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesInz, self.elast_matSeries, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [5.5, 0.2, 0.3, 0, 0, 0])

    def test_matSeriesInz_y_per(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesInz, self.elast_matSeries, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [0.2, 5.5, 0.5, 0, 0, 0])

    def test_matSeriesInz_z_per(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesInz, self.elast_matSeries, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [0.3, 0.5, 1.818181818, 0, 0, 0])

    def test_matSeriesInx_x_sym(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesInx, self.elast_matSeries, 'x', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [1.818181818, 0.2, 0.3, 0, 0, 0])

    def test_matSeriesInx_y_sym(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesInx, self.elast_matSeries, 'y', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [0.2, 5.5, 0.5, 0, 0, 0])

    def test_matSeriesInx_z_sym(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesInx, self.elast_matSeries, 'z', 's', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [0.3, 0.5, 5.5, 0, 0, 0])

    def test_matSeriesInx_x_sym_bicgstab(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesInx, self.elast_matSeries, 'x', 's', solver_type='bicgstab')
        np.testing.assert_array_almost_equal(Ceff, [1.818181818, 0.2, 0.3, 0, 0, 0], decimal=4)

    def test_matSeriesInx_y_sym_bicgstab(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesInx, self.elast_matSeries, 'y', 's', solver_type='bicgstab')
        np.testing.assert_array_almost_equal(Ceff, [0.2, 5.5, 0.5, 0, 0, 0], decimal=4)

    def test_matSeriesInx_z_sym_bicgstab(self):
        Ceff, u, _, _ = puma.compute_elasticity(self.ws_matSeriesInx, self.elast_matSeries, 'z', 's', solver_type='bicgstab')
        np.testing.assert_array_almost_equal(Ceff, [0.3, 0.5, 5.5, 0, 0, 0], decimal=4)

    def test_symmetry(self):
        X, Y, Z = (8, 6, 4)
        ws = puma.Workspace.from_array(np.ones((X, Y, Z)))
        bc = puma.ElasticityBC(ws)

        elast_map = puma.ElasticityMap()
        elast_map.add_isotropic_material((1, 1), 10, 0.3)
        elast_map.add_isotropic_material((2, 2), 7.3, 0.23)

        # Along x
        bc.dirichlet[0, :, :, 0] = -1.
        bc.dirichlet[-1, :, :, 0] = 1.
        bc.dirichlet[0, :, :, [1, 2]] = 0.
        bc.dirichlet[-1, :, :, [1, 2]] = 0.
        # puma.Workspace.show_orientation(bc)

        ws[:, :int(Y/2)] = 2
        # ws.show_matrix()

        u, _, _ = puma.compute_stress_analysis(ws, elast_map, bc, side_bc='p', solver_type='direct')
        # puma.Workspace.show_orientation(u, 5)

        np.testing.assert_array_almost_equal(u[:int(X / 2), :, :, 0], -u[int(X / 2):, :, :, 0][::-1], decimal=4)
        np.testing.assert_array_almost_equal(u[:int(X / 2), :, :, 1],  u[int(X / 2):, :, :, 1][::-1], decimal=4)
        np.testing.assert_array_almost_equal(u[:int(X / 2), :, :, 2],  u[int(X / 2):, :, :, 2][::-1], decimal=4)

        np.testing.assert_array_almost_equal(np.array([[0.14197716, 0.04368197, 0.],
                                                       [0.42584737, 0.04142283, 0.],
                                                       [0.71028350, 0.03215080, 0.],
                                                       [1.00000000, 0.00000000, 0.]], dtype=float), u[int(X / 2):, 2, 2], decimal=7)

        # Along y
        bc = puma.ElasticityBC(ws)
        bc.dirichlet[:, 0, :, 1] = -1.
        bc.dirichlet[:, -1, :, 1] = 1.
        bc.dirichlet[:, 0, :, [0, 2]] = 0.
        bc.dirichlet[:, -1, :, [0, 2]] = 0.
        # puma.Workspace.show_orientation(bc)

        ws = puma.Workspace.from_array(np.ones((X, Y, Z)))
        ws[:int(X/2)] = 2
        # ws.show_matrix()

        u, _, _ = puma.compute_stress_analysis(ws, elast_map, bc, side_bc='p', solver_type='direct')
        # puma.Workspace.show_orientation(u, 5)

        np.testing.assert_array_almost_equal(u[:, :int(Y / 2), :, 0],  u[:, int(Y / 2):, :, 0][:, ::-1], decimal=4)
        np.testing.assert_array_almost_equal(u[:, :int(Y / 2), :, 1], -u[:, int(Y / 2):, :, 1][:, ::-1], decimal=4)
        np.testing.assert_array_almost_equal(u[:, :int(Y / 2), :, 2],  u[:, int(Y / 2):, :, 2][:, ::-1], decimal=4)

        # Along z
        bc = puma.ElasticityBC(ws)
        bc.dirichlet[:, :, 0, 2] = -1.
        bc.dirichlet[:, :, -1, 2] = 1.
        bc.dirichlet[:, :, 0, [0, 1]] = 0.
        bc.dirichlet[:, :, -1, [0, 1]] = 0.
        # puma.Workspace.show_orientation(bc)

        u, _, _ = puma.compute_stress_analysis(ws, elast_map, bc, side_bc='p', solver_type='direct')
        # puma.Workspace.show_orientation(u, 5)

        np.testing.assert_array_almost_equal(u[:, :, :int(Z / 2), 0],  u[:, :, int(Z / 2):, 0][:, :, ::-1], decimal=4)
        np.testing.assert_array_almost_equal(u[:, :, :int(Z / 2), 1],  u[:, :, int(Z / 2):, 1][:, :, ::-1], decimal=4)
        np.testing.assert_array_almost_equal(u[:, :, :int(Z / 2), 2], -u[:, :, int(Z / 2):, 2][:, :, ::-1], decimal=4)

    def test_tensor_rotation_x(self):
        ws = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        ws.set(orientation_value=(1, 0, 0))
        elast_map = puma.ElasticityMap()
        elast_map.add_material_to_orient((1, 1), 10, 20, 0.23, 0.3, 50)

        Ceff, u, _, _ = puma.compute_elasticity(ws, elast_map, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [14.33251433, 9.41850942, 9.41850942, 0, 0, 0], decimal=7)

        Ceff, u, _, _ = puma.compute_elasticity(ws, elast_map, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [9.41850942, 28.16732817, 12.78271278, 0, 0, 0], decimal=7)

        Ceff, u, _, _ = puma.compute_elasticity(ws, elast_map, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [9.41850942, 12.78271278, 28.16732817, 0, 0, 0], decimal=7)

    def test_tensor_rotation_y(self):
        ws = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        ws.set(orientation_value=(0, 1, 0))
        elast_map = puma.ElasticityMap()
        elast_map.add_material_to_orient((1, 1), 10, 20, 0.23, 0.3, 50)

        Ceff, u, _, _ = puma.compute_elasticity(ws, elast_map, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [28.167328167328126, 9.418509418509416, 12.782712782712776, 0, 0, 0], decimal=7)

        Ceff, u, _, _ = puma.compute_elasticity(ws, elast_map, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [9.418509418509423, 14.33251433251433, 9.418509418509421, 0, 0, 0], decimal=7)

        Ceff, u, _, _ = puma.compute_elasticity(ws, elast_map, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [12.78271278271278, 9.418509418509414, 28.16732816732822, 0, 0, 0], decimal=7)

    def test_tensor_rotation_z(self):
        ws = puma.Workspace.from_array(np.ones((self.X, self.Y, self.Z)))
        ws.set(orientation_value=(0, 0, 1))
        elast_map = puma.ElasticityMap()
        elast_map.add_material_to_orient((1, 1), 10, 20, 0.23, 0.3, 50)

        Ceff, u, _, _ = puma.compute_elasticity(ws, elast_map, 'x', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [28.167328167328208, 12.782712782712784, 9.418509418509416, 0, 0, 0], decimal=7)

        Ceff, u, _, _ = puma.compute_elasticity(ws, elast_map, 'y', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [12.782712782712778, 28.167328167328137, 9.418509418509416, 0, 0, 0], decimal=7)

        Ceff, u, _, _ = puma.compute_elasticity(ws, elast_map, 'z', 'p', solver_type='direct')
        np.testing.assert_array_almost_equal(Ceff, [9.418509418509425, 9.418509418509425, 14.33251433251433, 0, 0, 0], decimal=7)

    # def test_Amat_builtinbeam596(self):
    #     ws = puma.Workspace.from_shape_value((5, 9, 6), 1)
    #
    #     elast_map = puma.ElasticityMap()
    #     elast_map.add_isotropic_material((1, 1), 200, 0.3)
    #
    #     bc = puma.ElasticityBC(ws)
    #     bc.dirichlet[0] = 0
    #     bc.dirichlet[-1] = [0, 1, 0]
    #
    #     solver = Elasticity(ws, elast_map, None, 'f', bc, None, None, "direct", True, (0, 0, 0, 0, 0))
    #     solver.error_check()
    #     solver.initialize()
    #     solver.assemble_Amatrix()
    #     Amat_correct = scipy.sparse.load_npz(os.path.join("testdata", "mpsa_Amat", "Amat_builtinbeam596_xf.npz"))
    #     test_Amat = np.abs(solver.Amat.toarray() - Amat_correct.toarray())
    #     self.assertAlmostEqual(test_Amat.max(), 0, 10)
    #
    #     solver = Elasticity(ws, elast_map, None, 'p', bc, None, None, "direct", True, (0, 0, 0, 0, 0))
    #     solver.error_check()
    #     solver.initialize()
    #     solver.assemble_Amatrix()
    #     Amat_correct = scipy.sparse.load_npz(os.path.join("testdata", "mpsa_Amat", "Amat_builtinbeam596_xp.npz"))
    #     test_Amat = np.abs(solver.Amat.toarray() - Amat_correct.toarray())
    #     self.assertAlmostEqual(test_Amat.max(), 0, 10)
    #
    #     solver = Elasticity(ws, elast_map, None, 's', bc, None, None, "direct", True, (0, 0, 0, 0, 0))
    #     solver.error_check()
    #     solver.initialize()
    #     solver.assemble_Amatrix()
    #     Amat_correct = scipy.sparse.load_npz(os.path.join("testdata", "mpsa_Amat", "Amat_builtinbeam596_xs.npz"))
    #     test_Amat = np.abs(solver.Amat.toarray() - Amat_correct.toarray())
    #     self.assertAlmostEqual(test_Amat.max(), 0, 10)
    #
    #     bc = puma.ElasticityBC(ws)
    #     bc.dirichlet[:, 0] = 0
    #     bc.dirichlet[:, -1] = [0, 0, 1]
    #
    #     solver = Elasticity(ws, elast_map, None, 'f', bc, None, None, "direct", True, (0, 0, 0, 0, 0))
    #     solver.error_check()
    #     solver.initialize()
    #     solver.assemble_Amatrix()
    #     Amat_correct = scipy.sparse.load_npz(os.path.join("testdata", "mpsa_Amat", "Amat_builtinbeam596_yf.npz"))
    #     test_Amat = np.abs(solver.Amat.toarray() - Amat_correct.toarray())
    #     self.assertAlmostEqual(test_Amat.max(), 0, 10)
    #
    #     solver = Elasticity(ws, elast_map, None, 'p', bc, None, None, "direct", True, (0, 0, 0, 0, 0))
    #     solver.error_check()
    #     solver.initialize()
    #     solver.assemble_Amatrix()
    #     Amat_correct = scipy.sparse.load_npz(os.path.join("testdata", "mpsa_Amat", "Amat_builtinbeam596_yp.npz"))
    #     test_Amat = np.abs(solver.Amat.toarray() - Amat_correct.toarray())
    #     self.assertAlmostEqual(test_Amat.max(), 0, 10)
    #
    #     solver = Elasticity(ws, elast_map, None, 's', bc, None, None, "direct", True, (0, 0, 0, 0, 0))
    #     solver.error_check()
    #     solver.initialize()
    #     solver.assemble_Amatrix()
    #     Amat_correct = scipy.sparse.load_npz(os.path.join("testdata", "mpsa_Amat", "Amat_builtinbeam596_ys.npz"))
    #     test_Amat = np.abs(solver.Amat.toarray() - Amat_correct.toarray())
    #     self.assertAlmostEqual(test_Amat.max(), 0, 10)
    #
    #     bc = puma.ElasticityBC(ws)
    #     bc.dirichlet[:, :, 0] = 0
    #     bc.dirichlet[:, :, -1] = [1, 0, 0]
    #
    #     solver = Elasticity(ws, elast_map, None, 'f', bc, None, None, "direct", True, (0, 0, 0, 0, 0))
    #     solver.error_check()
    #     solver.initialize()
    #     solver.assemble_Amatrix()
    #     Amat_correct = scipy.sparse.load_npz(os.path.join("testdata", "mpsa_Amat", "Amat_builtinbeam596_zf.npz"))
    #     test_Amat = np.abs(solver.Amat.toarray() - Amat_correct.toarray())
    #     self.assertAlmostEqual(test_Amat.max(), 0, 10)
    #
    #     solver = Elasticity(ws, elast_map, None, 'p', bc, None, None, "direct", True, (0, 0, 0, 0, 0))
    #     solver.error_check()
    #     solver.initialize()
    #     solver.assemble_Amatrix()
    #     Amat_correct = scipy.sparse.load_npz(os.path.join("testdata", "mpsa_Amat", "Amat_builtinbeam596_zp.npz"))
    #     test_Amat = np.abs(solver.Amat.toarray() - Amat_correct.toarray())
    #     self.assertAlmostEqual(test_Amat.max(), 0, 10)
    #
    #     solver = Elasticity(ws, elast_map, None, 's', bc, None, None, "direct", True, (0, 0, 0, 0, 0))
    #     solver.error_check()
    #     solver.initialize()
    #     solver.assemble_Amatrix()
    #     Amat_correct = scipy.sparse.load_npz(os.path.join("testdata", "mpsa_Amat", "Amat_builtinbeam596_zs.npz"))
    #     test_Amat = np.abs(solver.Amat.toarray() - Amat_correct.toarray())
    #     self.assertAlmostEqual(test_Amat.max(), 0, 10)

    # def test_Amat_fiberform(self):
    #     ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"))
    #     ws.rotate(90, 'z')
    #     ws.rescale(0.5, False)
    #     ws.binarize_range((104, 255))
    #     ws.voxel_length = 1
    #     elast_map = puma.ElasticityMap()
    #     elast_map.add_isotropic_material((1, 1), 200.e9, 0.3)
    #     solver = Elasticity(ws, elast_map, 'x', 'p', None, None, None, "direct", True, (0, 0, 0, 0, 0))
    #     solver.error_check()
    #     solver.initialize()
    #     solver.assemble_Amatrix()
    #     Amat_correct = scipy.sparse.load_npz(os.path.join("testdata", "mpsa_Amat", "Amat_correct_ff100_rot_05.npz"))
    #     np.testing.assert_equal(Amat_correct.data,  solver.Amat.data)


if __name__ == '__main__':
    unittest.main()
