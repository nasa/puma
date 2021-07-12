import numpy as np
from skimage import measure
import scipy.ndimage as ndimage
import stl
from pumapy.utilities.workspace import Workspace
from pumapy.utilities.generic_checks import check_ws_cutoff


def generate_isosurface(workspace, cutoff, flag_closed_edges=True, flag_gaussian=False):
    """ Generation of isosurface based on cutoff provided

    :param workspace: domain
    :type workspace: Workspace
    :param cutoff: specify cutoff to binarize material
    :type cutoff: tuple(int, int)
    :param flag_closed_edges: close edges of surfaces on the boundaries
    :type flag_closed_edges: bool, optional
    :param flag_gaussian: apply Gaussian filter before generating surface
    :type flag_gaussian: bool, optional
    :return: triangulated surface
    :rtype: TriMesh
    """
    iso = Isosurface(workspace, cutoff, flag_closed_edges, flag_gaussian)

    iso.error_check()

    iso.log_input()
    iso.compute()
    iso.log_output()

    return iso.mesh


class TriMesh:
    verts = []
    faces = []
    normals = []
    values = []

    def copy(self, other):
        self.verts = np.copy(other.verts)
        self.faces = np.copy(other.faces)
        self.normals = np.copy(other.normals)
        self.values = np.copy(other.values)

    def save(self, filename, binary=True):
        if not filename.lower().endswith('.stl'):
            filename += ".stl"

        m = stl.mesh.Mesh(np.zeros(self.faces.shape[0], dtype=stl.mesh.Mesh.dtype))
        for i, f in enumerate(self.faces):
            m.vectors[i] = self.verts[f]
        if binary:
            return m.save(filename)
        else:
            return m.save(filename, mode=stl.Mode.ASCII)

    def get_properties(self):
        m = stl.mesh.Mesh(np.zeros(self.faces.shape[0], dtype=stl.mesh.Mesh.dtype))
        for i, f in enumerate(self.faces):
            m.vectors[i] = self.verts[f]
        volume, cog, inertia = m.get_mass_properties()
        print("Volume                                  = {0}".format(abs(volume)))
        print("Position of the center of gravity (COG) = {0}".format(cog))
        print("Inertia matrix                          = {0}".format(inertia[0, :]))
        print("                                          {0}".format(inertia[1, :]))
        print("                                          {0}".format(inertia[2, :]))


class Isosurface:

    def __init__(self, workspace, cutoff, flag_closed_edges, flag_gaussian):
        if isinstance(workspace, np.ndarray):
            self.workspace = Workspace.from_array(workspace)
        else:
            self.workspace = workspace
        self.cutoff = list(cutoff)
        self.flag_closed_edges = flag_closed_edges
        self.flag_gaussian = flag_gaussian
        self.mesh = TriMesh()
        self.matrix = np.empty(self.workspace.matrix.shape)

    def compute(self):
        if float(self.cutoff[0]).is_integer():
            self.cutoff[0] -= 0.5
        if float(self.cutoff[1]).is_integer():
            self.cutoff[1] += 0.5

        self.flip_matrix()
        self.apply_gaussian()
        self.buffer_matrix()
        import warnings
        warnings.simplefilter(action='ignore', category=FutureWarning)
        self.mesh.verts, self.mesh.faces, self.mesh.normals, self.mesh.values \
            = measure.marching_cubes_lewiner(self.matrix, self.iso_value)

    def flip_matrix(self):
        self.matrix = np.copy(self.workspace.matrix).astype(np.float32)
        average = (self.cutoff[0] + self.cutoff[1]) / 2.0
        mask = self.matrix > average
        self.matrix[mask] = 2 * average - self.matrix[mask]
        self.iso_value = self.cutoff[0]

    def buffer_matrix(self):
        if self.flag_closed_edges:
            matrix_buf = np.full([self.matrix.shape[0] + 2, self.matrix.shape[1] + 2, self.matrix.shape[2] + 2], 1e-3)
            matrix_buf[1:-1, 1:-1, 1:-1] = self.matrix[:, :, :]
            self.matrix = np.copy(matrix_buf)

    def apply_gaussian(self):
        if self.flag_gaussian:
            self.matrix = ndimage.gaussian_filter(self.matrix, sigma=1)

    def error_check(self):
        check_ws_cutoff(self.workspace, self.cutoff)

    def log_input(self):
        self.workspace.log.log_section("Computing Isosurface")
        self.workspace.log.log_line("Domain Size: " + str(self.workspace.get_shape))
        self.workspace.log.log_line("Cutoff: " + str(self.cutoff))

        if self.flag_closed_edges:
            self.workspace.log.log_line("Closed Edges: True")
        else:
            self.workspace.log.log_line("Closed Edges: False")

        if self.flag_gaussian:
            self.workspace.log.log_line("Gaussian: True")
        else:
            self.workspace.log.log_line("Gaussian: False")

        self.workspace.log.write_log()

    def log_output(self):
        self.workspace.log.log_section("Finished Isosurface")
        self.workspace.log.log_line("Number of Triangles: " + str(self.mesh.values.shape[0] / 3.))
