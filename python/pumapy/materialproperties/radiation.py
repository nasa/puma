from pumapy.physicsmodels.raycasting import RayCasting
from scipy.optimize import curve_fit
from matplotlib import pyplot as plt
import numpy as np
from pumapy.utilities.workspace import Workspace
from pumapy.utilities.logger import print_warning


def compute_radiation(workspace, solid_cutoff, sources_number, degree_accuracy, void_phase=0, boundary_behavior=1,
                      bin_density=10000, exportparticles_filepathname='', export_pathname=None):
    """ Compute the radiative thermal conductivity through ray tracing
        (N.B. 0 material ID in workspace refers to gas phases unless otherwise specified)

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param solid_cutoff: specify the solid phase
        :type solid_cutoff: (int, int)
        :param sources_number: number of light sources spread randomly in the void space (i.e. 0)
        :type sources_number: int
        :param degree_accuracy: angle difference between rays emitted in degrees (has to be an exact divider of 180°)
        :type degree_accuracy: int
        :param void_phase: ID of the void phase, defaulted as 0
        :type void_phase: int
        :param boundary_behavior: how to treat particles exiting the domain: 0=kill, 1=periodic (default)
        :type boundary_behavior: int
        :param bin_density: number of bins used to create histogram of ray distances
        :type bin_density: int
        :param exportparticles_filepathname: path and name of the particle files exported as VTK points
        :type exportparticles_filepathname: string
        :param export_pathname: path to save curve plot of ray distance distribution
        :type export_pathname: str
        :return: extinction coefficient (beta), its standard deviation and of ray distances
        :rtype: (float, float, np.ndarray)

        :Example:
        >>> import pumapy as puma
        >>> ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
        >>> beta, beta_std, rays_distances = puma.compute_radiation(ws_fiberform, (104, 255), 100, 15)
    """
    solver = Radiation(workspace, solid_cutoff, sources_number, degree_accuracy, void_phase, boundary_behavior, bin_density,
                       exportparticles_filepathname, export_pathname)

    solver.error_check()

    solver.log_input()
    rays_distances = solver.compute()
    solver.log_output()
    return solver.beta, solver.beta_std, rays_distances


class Radiation:

    def __init__(self, workspace, cutoff, sources_number, degree_accuracy, void_phase, boundary_behavior, bin_density,
                 rayexport_filepathname, export_plot):
        self.workspace = workspace
        self.cutoff = cutoff
        self.sources_number = sources_number
        self.degree_accuracy = degree_accuracy
        self.void_phase = void_phase
        self.boundary_behavior = boundary_behavior
        self.bin_density = bin_density
        self.rayexport_filepathname = rayexport_filepathname
        self.export_pathname = export_plot
        self.X = None
        self.Y = None
        self.Z = None
        self.beta = [None, None, None]
        self.beta_std = [None, None, None]

    def compute(self):

        simulation = RayCasting(self.workspace, self.degree_accuracy, self.generate_sources(), self.void_phase,
                                self.boundary_behavior, self.rayexport_filepathname)

        simulation.error_check()

        simulation.generate_spherical_walkers()
        simulation.expand_sources()
        if self.export_pathname is not None:
            np.save(self.export_pathname, simulation.rays_distances)

        self.beta, self.beta_std = compute_extinction_coefficients(self.workspace, simulation.rays_distances,
                                                                   self.sources_number, self.degree_accuracy,
                                                                   self.bin_density, self.export_pathname)
        return simulation.rays_distances

    def generate_sources(self):
        # randomly choosing the source locations
        void_voxels_number = np.count_nonzero(self.workspace.matrix == self.void_phase)
        source_locations = np.array(np.where(self.workspace.matrix == self.void_phase)).transpose()
        if void_voxels_number <= self.sources_number:
            print_warning("Too many sources, limiting it to the number of void voxels: {}".format(void_voxels_number))
        else:
            choices = np.random.choice(source_locations.shape[0], size=self.sources_number, replace=False)
            source_locations = source_locations[choices]
        return source_locations

    def error_check(self):
        if not isinstance(self.workspace, Workspace):
            raise Exception("Workspace must be a puma.Workspace.")
        else:
            self.X = self.workspace.matrix.shape[0]
            self.Y = self.workspace.matrix.shape[1]
            self.Z = self.workspace.matrix.shape[2]
            self.workspace.binarize_range(self.cutoff)

        if np.count_nonzero(self.workspace.matrix == self.void_phase) == 0:
            raise Exception("No valid voxels detected (i.e. ID={}), cannot run radiation ray tracing.".format(self.void_phase))

    def log_input(self):
        self.workspace.log.log_section("Computing Radiation")
        self.workspace.log.log_line("Domain Size: " + str(self.workspace.get_shape()))
        self.workspace.log.log_line("Sources: " + str(self.sources_number))
        self.workspace.log.log_line("Degree accuracy: " + str(self.degree_accuracy))
        self.workspace.log.write_log()

    def log_output(self):
        self.workspace.log.log_section("Finished Radiation Simulation")
        self.workspace.log.log_line("Extinction coefficient beta: [" +
                                    str(self.beta[0]) + ' +/- ' + str(self.beta_std[0]) + ', ' +
                                    str(self.beta[1]) + ' +/- ' + str(self.beta_std[1]) + ', ' +
                                    str(self.beta[2]) + ' +/- ' + str(self.beta_std[2]) + ']')
        self.workspace.log.write_log()


def compute_extinction_coefficients(ws, rays_distances, sources_number, degree_accuracy,
                                    bin_density=10000, export_pathname=None):
    """ Compute the extinction coefficient based on the ray casting radiation simulation
    (this is normally a step inside the compute_radiation function)

    :param ws: domain
    :type ws: pumapy.Workspace
    :param rays_distances: rays distances, as output by compute_radiation function
    :type rays_distances: np.ndarray
    :param sources_number: number of light sources spread randomly in the void space (i.e. 0)
    :type sources_number: int
    :param degree_accuracy: angle difference between rays emitted in degrees (has to be an exact divider of 180°)
    :type degree_accuracy: int
    :param bin_density: number of bins used to create histogram of ray distances
    :type bin_density: int
    :param export_pathname: path to save curve plot of ray distance distribution
    :type export_pathname: str
    :return: extinction coefficient (beta), its standard deviation
    :rtype: (float, float)
    """
    print("\nComputing extinction coefficients ... ", end='')

    # splitting use the ray distances into bins (max ray distance in RayCasting is 2x the max cube diagonal)
    bins = np.linspace(0, np.sqrt(ws.len_x()**2 + ws.len_y()**2 + ws.len_z()**2), bin_density, dtype=float)
    bins_midpoints = (bins[:-1] + bins[1:]) / 2.

    if export_pathname is not None:
        dirs = ['x', 'y', 'z']
        fig, ax = plt.subplots(1, 3, figsize=(13, 4))
        fig.tight_layout(pad=4, w_pad=3, h_pad=0)

    beta_out = [None, None, None]
    beta_std_out = [None, None, None]

    # binning the ray distances
    for dim in range(3):

        # probability density function of a ray travelling a certain distance
        pdf, _ = np.histogram(rays_distances[:, dim], bins=bins)
        pdf = pdf.astype(float) / np.sum(pdf)

        # integrating the pdf into a cdf
        cdf = np.cumsum(pdf)
        exp_curve_rays = 1. - cdf

        beta, cov = curve_fit(f=lambda x, b: np.exp(-b * x), xdata=bins_midpoints, ydata=exp_curve_rays)
        beta_std = np.sqrt(np.diag(cov))
        exp_curve_fit = np.exp(-beta * bins)

        beta_out[dim] = beta[0] / ws.voxel_length
        beta_std_out[dim] = beta_std[0] / ws.voxel_length

        if export_pathname is not None:
            ax[dim].plot(bins_midpoints, exp_curve_rays, 'k-', label="Ray lengths " + dirs[dim])
            ax[dim].plot(bins, exp_curve_fit, 'r-', label="Exp. fit " + dirs[dim])
            ax[dim].set_xlim(0, 0.7 * np.sqrt(ws.len_x()**2 + ws.len_y()**2 + ws.len_z()**2))
            ax[dim].set_ylim(0, 1)
            ax[dim].set_aspect(1./ax[dim].get_data_ratio())
            ax[dim].set_xlabel("Ray length (voxels)")
            ax[dim].set_ylabel("I(s)/I_0")
            ax[dim].grid()
            ax[dim].legend(loc="upper right")
            ax[dim].set_title("beta: {:.1f} +/- {:.1f}".format(beta_out[dim], beta_std_out[dim]))

    if export_pathname is not None:
        fig.suptitle("Radiation simulation with {} sources and {}° between rays"
                     .format(sources_number, degree_accuracy))
        fig.savefig(export_pathname)
    print("Done")

    print("Extinction coefficient (beta):")
    print("x: " + str(round(beta_out[0], 1)) + ' +/- ' + str(round(beta_std_out[0], 1)))
    print("y: " + str(round(beta_out[1], 1)) + ' +/- ' + str(round(beta_std_out[1], 1)))
    print("z: " + str(round(beta_out[2], 1)) + ' +/- ' + str(round(beta_std_out[2], 1)))
    return beta_out, beta_std_out
