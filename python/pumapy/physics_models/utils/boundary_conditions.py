from pumapy.utilities.workspace import Workspace
import numpy as np


class IsotropicConductivityBC:
    def __init__(self, workspace):
        """ Conductivity prescribed dirichlet boundary conditions class.

            N.B. IsotropicConductivityBC and AnisotropicConductivityBC classes have a different way of providing
            dirichlet BC due to the underlying numerical method:
            IsotropicConductivityBC are enforced in the voxel centers, whereas AnisotropicConductivityBC and ElasticityBC
            (based on the MPFA and MPSA methods), apply the dirichlet bc on the voxel sides.

            :param workspace: domain
            :type workspace: Workspace

            :var dirichlet: of size (X, Y, Z), it describes voxels to keep fixed to a certain temperature
            :vartype dirichlet: np.ndarray
        """
        self.dirichlet = np.full((workspace.len_x(), workspace.len_y(), workspace.len_z()), np.Inf, dtype=float)

    def show_dirichlet(self):
        Workspace.show_matrix(self.dirichlet)


class AnisotropicConductivityBC:
    def __init__(self, workspace):
        """ Conductivity prescribed dirichlet boundary conditions class.

            :param workspace: domain
            :type workspace: Workspace

            :var xfaces: of size (2, Y, Z), it describes the temperature to impose on the X faces (-ve and +ve), initialized as np.NAN
            :vartype xfaces: np.ndarray
            :var yfaces: of size (2, X, Z), it describes the temperature to impose on the Y faces (-ve and +ve), initialized as np.NAN
            :vartype yfaces: np.ndarray
            :var zfaces: of size (2, X, Y), it describes the temperature to impose on the Z faces (-ve and +ve), initialized as np.NAN
            :vartype zfaces: np.ndarray
        """
        self.xfaces = np.full((2, workspace.matrix.shape[1] + 1, workspace.matrix.shape[2] + 1), np.NAN, dtype=float)
        self.yfaces = np.full((2, workspace.matrix.shape[0] + 1, workspace.matrix.shape[2] + 1), np.NAN, dtype=float)
        self.zfaces = np.full((2, workspace.matrix.shape[0] + 1, workspace.matrix.shape[1] + 1), np.NAN, dtype=float)


class ElasticityBC:
    def __init__(self, workspace):
        """ Elasticity prescribed dirichlet boundary conditions class.

            :param workspace: domain
            :type workspace: Workspace

            :var xfaces: of size (2, Y, Z, 3), it describes the displacements to impose on the X faces (-ve and +ve), initialized as np.NAN
            :vartype xfaces: np.ndarray
            :var yfaces: of size (2, X, Z, 3), it describes the displacements to impose on the Y faces (-ve and +ve), initialized as np.NAN
            :vartype yfaces: np.ndarray
            :var zfaces: of size (2, X, Y, 3), it describes the displacements to impose on the Z faces (-ve and +ve), initialized as np.NAN
            :vartype zfaces: np.ndarray
        """
        self.xfaces = np.full((2, workspace.matrix.shape[1] + 1, workspace.matrix.shape[2] + 1, 3), np.NAN, dtype=float)
        self.yfaces = np.full((2, workspace.matrix.shape[0] + 1, workspace.matrix.shape[2] + 1, 3), np.NAN, dtype=float)
        self.zfaces = np.full((2, workspace.matrix.shape[0] + 1, workspace.matrix.shape[1] + 1, 3), np.NAN, dtype=float)
