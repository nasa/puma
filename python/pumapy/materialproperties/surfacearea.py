from pumapy.utilities.isosurface import generate_isosurface, TriMesh
from pumapy.utilities.generic_checks import check_ws_cutoff
from skimage import measure


def compute_surface_area(workspace, cutoff, flag_gaussian=False):
    """ Computation of the surface area based on isosurface

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param cutoff: specify the solid phase
        :type cutoff: (int, int)
        :param flag_gaussian: apply Gaussian filter before generating surface
        :type flag_gaussian: bool
        :return: area, specific_area
        :rtype: (float, float)

        :Example:
        >>> import pumapy as puma
        >>> ws = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6) # import workspace
        >>> area, specific_area = puma.compute_surface_area(ws, cutoff=(90, 255)) # computing surface area
        >>> print("Surface area: ", area)
        >>> print("Specific surface area: ", specific_area)
    """
    check_ws_cutoff(workspace, cutoff)

    surf_area = SurfaceArea(workspace, cutoff, flag_gaussian)

    surf_area.log_input()
    surf_area.compute()
    surf_area.log_output()

    return surf_area.area, surf_area.specific_area


class SurfaceArea:
    def __init__(self, workspace, cutoff, flag_gaussian):
        self.workspace = workspace
        self.cutoff = cutoff
        self.flag_gaussian = flag_gaussian
        self.mesh = TriMesh()
        self.area = -1.
        self.specific_area = -1.

    def compute(self):
        self.mesh = generate_isosurface(self.workspace, self.cutoff, False, self.flag_gaussian)
        self.area = self.workspace.voxel_length ** 2 * measure.mesh_surface_area(self.mesh.verts, self.mesh.faces)
        volume = self.workspace.voxel_length ** 3 * (self.workspace.len_x() - 1) * \
                 (self.workspace.len_y() - 1) * (self.workspace.len_z() - 1)
        self.specific_area = self.area / volume

    def log_input(self):
        self.workspace.log.log_section("Computing Surface Area")
        self.workspace.log.log_line("Domain Size: " + str(self.workspace.get_shape()))
        self.workspace.log.log_line("Cutoff: " + str(self.cutoff))

        if self.flag_gaussian:
            self.workspace.log.log_line("Gaussian: True")
        else:
            self.workspace.log.log_line("Gaussian: False")

        self.workspace.log.write_log()

    def log_output(self):
        self.workspace.log.log_section("Finished Surface Area Calculation")
        self.workspace.log.log_line("Surface Area: " + str(self.area))
        self.workspace.log.log_line("Specific Surface Area: " + str(self.specific_area))
