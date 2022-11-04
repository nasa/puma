from pumapy.utilities.workspace import Workspace
import numpy as np


class ConductivityBC:
    def __init__(self, workspace):
        """ Conductivity prescribed dirichlet boundary conditions class.

            :param workspace: domain
            :type workspace: Workspace

            :var dirichlet: of size (X, Y, Z), it describes voxels to keep fixed to a certain temperature
            :vartype dirichlet: np.ndarray
        """
        self.dirichlet = np.full((workspace.len_x(), workspace.len_y(), workspace.len_z()), np.Inf, dtype=float)

    def show_dirichlet(self):
        Workspace.show_matrix(self.dirichlet)


class Isotropic_symmetricBC:
    def __init__(self, len_x, len_y, len_z):
        self.len_x = len_x
        self.len_y = len_y
        self.len_z = len_z
        self.len_xy = self.len_x * self.len_y

    def indexAt_3D(self, i, j, k):
        if i == -1:
            i = 0
        if i == self.len_x:
            i = self.len_x - 1
        if j == -1:
            j = 0
        if j == self.len_y:
            j = self.len_y - 1
        if k == -1:
            k = 0
        if k == self.len_z:
            k = self.len_z - 1
        return self.len_xy * k + self.len_x * j + i


class Isotropic_periodicBC:
    def __init__(self, len_x, len_y, len_z):
        self.len_x = len_x
        self.len_y = len_y
        self.len_z = len_z
        self.len_xy = self.len_x * self.len_y

    def indexAt_3D(self, i, j, k):
        if i == -1:
            i = self.len_x - 1
        if i == self.len_x:
            i = 0
        if j == -1:
            j = self.len_y - 1
        if j == self.len_y:
            j = 0
        if k == -1:
            k = self.len_z - 1
        if k == self.len_z:
            k = 0
        return self.len_xy * k + self.len_x * j + i


class ElasticityBC:
    def __init__(self, workspace):
        """ Elasticity prescribed dirichlet boundary conditions class.

            :param workspace: domain
            :type workspace: Workspace

            :var xfaces: of size (2, Y, Z), it describes the displacements to impose on the X faces (-ve and +ve), initialized as np.NAN
            :vartype xfaces: np.ndarray
            :var yfaces: of size (2, X, Z), it describes the displacements to impose on the Y faces (-ve and +ve), initialized as np.NAN
            :vartype yfaces: np.ndarray
            :var zfaces: of size (2, X, Y), it describes the displacements to impose on the Z faces (-ve and +ve), initialized as np.NAN
            :vartype zfaces: np.ndarray
        """
        self.xfaces = np.full((2, workspace.matrix.shape[1] + 1, workspace.matrix.shape[2] + 1, 3), np.NAN, dtype=float)
        self.yfaces = np.full((2, workspace.matrix.shape[0] + 1, workspace.matrix.shape[2] + 1, 3), np.NAN, dtype=float)
        self.zfaces = np.full((2, workspace.matrix.shape[0] + 1, workspace.matrix.shape[1] + 1, 3), np.NAN, dtype=float)
