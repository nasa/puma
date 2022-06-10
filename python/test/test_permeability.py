import unittest
import numpy as np
import pumapy as puma


class TestPermeability(unittest.TestCase):

    def test_analytical_direct(self):
        ws = puma.generate_cylinder_square_array(100, 1. - 2. * np.pi * (0.1 ** 2.))
        ws.voxel_length = 1. / ws.matrix.shape[0]
        keff, _ = puma.compute_permeability(ws, (1, 1), solver_type='direct')
        np.testing.assert_array_almost_equal(keff, np.array([[2.71223274e-02, 0., 0.], [0., 2.71223274e-02, 0.], [0., 0., 5.48040140e-02]]))

    def test_analytical_cg(self):
        ws = puma.generate_cylinder_square_array(100, 1. - 2. * np.pi * (0.1 ** 2.))
        ws.voxel_length = 1. / ws.matrix.shape[0]
        keff, _ = puma.compute_permeability(ws, (1, 1), solver_type='cg', tol=1e-7, maxiter=10000)
        np.testing.assert_array_almost_equal(keff, np.array([[2.71223274e-02, 0., 0.], [0., 2.71223274e-02, 0.], [0., 0., 5.48040140e-02]]), decimal=4)

    def test_analytical_cg_nomf(self):
        ws = puma.generate_cylinder_square_array(100, 1. - 2. * np.pi * (0.1 ** 2.))
        ws.voxel_length = 1. / ws.matrix.shape[0]
        keff, _ = puma.compute_permeability(ws, (1, 1), solver_type='cg', tol=1e-7, maxiter=10000, matrix_free=False)
        np.testing.assert_array_almost_equal(keff, np.array([[2.71223274e-02, 0., 0.], [0., 2.71223274e-02, 0.], [0., 0., 5.48040140e-02]]), decimal=4)

    def test_analytical_cg_noprec(self):
        ws = puma.generate_cylinder_square_array(100, 1. - 2. * np.pi * (0.1 ** 2.))
        ws.voxel_length = 1. / ws.matrix.shape[0]
        keff, _ = puma.compute_permeability(ws, (1, 1), solver_type='cg', tol=1e-7, maxiter=10000, precondition=False)
        np.testing.assert_array_almost_equal(keff, np.array([[2.71223274e-02, 0., 0.], [0., 2.71223274e-02, 0.], [0., 0., 5.48040140e-02]]), decimal=4)

    def test_analytical_cg_nomf_noprec(self):
        ws = puma.generate_cylinder_square_array(100, 1. - 2. * np.pi * (0.1 ** 2.))
        ws.voxel_length = 1. / ws.matrix.shape[0]
        keff, _ = puma.compute_permeability(ws, (1, 1), solver_type='cg', tol=1e-7, maxiter=10000, matrix_free=False, precondition=False)
        np.testing.assert_array_almost_equal(keff, np.array([[2.71223274e-02, 0., 0.], [0., 2.71223274e-02, 0.], [0., 0., 5.48040140e-02]]), decimal=4)

    def test_analytical_minres_noprec(self):
        ws = puma.generate_cylinder_square_array(100, 1. - 2. * np.pi * (0.1 ** 2.))
        ws.voxel_length = 1. / ws.matrix.shape[0]
        keff, _ = puma.compute_permeability(ws, (1, 1), solver_type='minres', tol=1e-8, maxiter=10000, precondition=False)
        np.testing.assert_array_almost_equal(keff, np.array([[2.71223274e-02, 0., 0.], [0., 2.71223274e-02, 0.], [0., 0., 5.48040140e-02]]), decimal=4)

    def test_analytical_minres_nomf_noprec(self):
        ws = puma.generate_cylinder_square_array(100, 1. - 2. * np.pi * (0.1 ** 2.))
        ws.voxel_length = 1. / ws.matrix.shape[0]
        keff, _ = puma.compute_permeability(ws, (1, 1), solver_type='minres', tol=1e-8, maxiter=10000, matrix_free=False, precondition=False)
        np.testing.assert_array_almost_equal(keff, np.array([[2.71223274e-02, 0., 0.], [0., 2.71223274e-02, 0.], [0., 0., 5.48134246e-02]]), decimal=4)

if __name__ == '__main__':
    unittest.main()
