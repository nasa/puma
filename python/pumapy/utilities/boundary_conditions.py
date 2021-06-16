from pumapy import Workspace
import numpy as np


class ConductivityBC:
    def __init__(self, x, y, z):
        self.dirichlet = np.full((x, y, z), np.Inf, dtype=float)
        # self.__neumann = np.full((x, y, z, 3), np.Inf, dtype=float)

    def __getitem__(self, key):
        return self.dirichlet[key]

    def __setitem__(self, key, value):
        self.dirichlet[key] = value

    @classmethod
    def from_workspace(cls, workspace):
        """ Generate ConductivityBC from a Workspace.

        :param workspace: domain
        :type workspace: Workspace
        """
        if isinstance(workspace, Workspace):
            return cls(workspace.len_x(), workspace.len_y(), workspace.len_z())
        else:
            raise Exception("Cannot create ConductivityBC, the input to the from_workspace method has to be a Workspace.")

    def get_shape(self):
        return self.dirichlet.shape

    def show(self):
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
    def __init__(self, x, y, z):
        self.dirichlet = np.full((x, y, z, 3), np.Inf, dtype=float)
        # self.__neumann = np.full((x, y, z, 6), np.Inf, dtype=float)

    def __getitem__(self, key):
        return self.dirichlet[key]

    def __setitem__(self, key, value):
        self.dirichlet[key] = value

    @classmethod
    def from_workspace(cls, workspace):
        """ Generate ElasticityBC from a Workspace.

        :param workspace: domain
        :type workspace: Workspace
        """
        if isinstance(workspace, Workspace):
            return cls(workspace.len_x(), workspace.len_y(), workspace.len_z())
        else:
            raise Exception("Cannot create ElasticityBC, the input to the from_workspace method has to be a Workspace.")

    def get_shape(self):
        return self.dirichlet.shape

    def show(self):
        Workspace.show_orientation(self.dirichlet)
