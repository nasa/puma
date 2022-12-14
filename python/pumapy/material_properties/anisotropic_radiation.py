import pumapy
from scipy.optimize import curve_fit
from matplotlib import pyplot as plt
import numpy as np
from pumapy.utilities.workspace import Workspace


def compute_radiation_anisotropic(workspace, void_cutoff, sources_number, bin_density=10000, export_pathname=None):
    """ Compute the anisotropic radiative thermal conductivity through ray tracing

        :param workspace: domain
        :type workspace: pumapy.Workspace
        :param void_cutoff: specify the void phase
        :type void_cutoff: (int, int)
        :param sources_number: number of light sources spread randomly in the void space (i.e. 0)
        :type sources_number: int
        :param bin_density: number of bins used to create histogram of ray distances
        :type bin_density: int
        :param export_pathname: path to save curve plot of ray distance distribution
        :type export_pathname: str
        :return: extinction coefficient (beta), its standard deviation and of ray distances
        :rtype: (float, float, np.ndarray)

    """

    rays_distances = np.zeros((sources_number, 3))

    solver = AnisotropicRadiation(workspace, void_cutoff, sources_number, 'x', bin_density, export_pathname)
    solver.error_check()
    solver.log_input()
    rays_distances[:,0] = solver.compute()
    solver.log_output()

    solver = AnisotropicRadiation(workspace, void_cutoff, sources_number, 'y', bin_density, export_pathname)
    solver.error_check()
    solver.log_input()
    rays_distances[:,1] = solver.compute()
    solver.log_output()

    solver = AnisotropicRadiation(workspace, void_cutoff, sources_number, 'z', bin_density, export_pathname)
    solver.error_check()
    solver.log_input()
    rays_distances[:,2] = solver.compute()
    solver.log_output()

    beta, beta_std = compute_extinction_coefficients_anisotropic(workspace, rays_distances, sources_number, bin_density, export_pathname)

    return beta, beta_std, rays_distances


class AnisotropicRadiation:

    def __init__(self, workspace, void_cutoff, sources_number, direction, bin_density, export_plot):
        self.workspace = workspace
        self.matrix = np.copy(self.workspace.matrix)
        self.matrix[self.matrix < void_cutoff[0]] = 1e4
        self.matrix[self.matrix <= void_cutoff[1]] = 0
        self.matrix[self.matrix > void_cutoff[1]] = 1
        self.indices_matrix = None
        self.boundary_indices = None

        self.void_cutoff = void_cutoff
        self.sources_number = sources_number
        self.bin_density = bin_density
        self.direction = direction
        self.export_pathname = export_plot
        self.X = None
        self.Y = None
        self.Z = None
        self.beta = [None, None, None]
        self.beta_std = [None, None, None]
        self.ray_distances = np.zeros(sources_number)
        self.ray_indices = None

    def compute(self):

        if self.direction == 'x':
            self.matrix = np.swapaxes(self.matrix, 0, 2)
        elif self.direction == 'y':
            self.matrix = np.swapaxes(self.matrix, 1, 2)

        self.boundary_indices = np.array(np.where(self.matrix[:,:,0] == 0))
        self.indices_matrix = np.array(np.where(self.matrix == 0))

        self.ray_indices = np.random.randint(self.indices_matrix.shape[1], size=self.sources_number)

        for i in range(self.sources_number):
            start_pos = self.indices_matrix[:, self.ray_indices[i]]
            array_1d = self.matrix[start_pos[0], start_pos[1], start_pos[2]:]
            length = np.argmax(array_1d > 0)

            self.ray_distances[i] += length

            while length == 0:
                self.ray_distances[i] += array_1d.shape[0]
                ray_index = np.random.randint(self.boundary_indices.shape[1])
                start_pos = self.boundary_indices[:, ray_index]
                array_1d = self.matrix[start_pos[0], start_pos[1], :]
                length = np.argmax(array_1d > 0)
                self.ray_distances[i] += length

        return self.ray_distances


    def error_check(self):
        if not isinstance(self.workspace, Workspace):
            raise Exception("Workspace must be a puma.Workspace.")
        else:
            self.X = self.workspace.matrix.shape[0]
            self.Y = self.workspace.matrix.shape[1]
            self.Z = self.workspace.matrix.shape[2]

        if np.count_nonzero(self.workspace.matrix == 0) == 0:
            raise Exception("All voxels are solid, cannot run radiation ray tracing.")

        if np.count_nonzero(self.workspace.matrix == 1) == 0:
            raise Exception("All voxels are void, cannot run radiation ray tracing.")

    def log_input(self):
        self.workspace.log.log_section("Computing Radiation")
        self.workspace.log.log_line("Domain Size: " + str(self.workspace.get_shape()))
        self.workspace.log.log_line("Sources: " + str(self.sources_number))
        self.workspace.log.write_log()

    def log_output(self):
        self.workspace.log.log_section("Finished Radiation Simulation")
        self.workspace.log.log_line("Extinction coefficient beta: [" +
                                    str(self.beta[0]) + ' +/- ' + str(self.beta_std[0]) + ', ' +
                                    str(self.beta[1]) + ' +/- ' + str(self.beta_std[1]) + ', ' +
                                    str(self.beta[2]) + ' +/- ' + str(self.beta_std[2]) + ']')
        self.workspace.log.write_log()


def compute_extinction_coefficients_anisotropic(ws, rays_distances, sources_number, bin_density=10000, export_pathname=None):
    """ Compute the extinction coefficient based on the ray casting radiation simulation
    (this is normally a step inside the compute_radiation function)

    :param ws: domain
    :type ws: pumapy.Workspace
    :param rays_distances: rays distances, as output by compute_radiation function
    :type rays_distances: np.ndarray
    :param sources_number: number of light sources spread randomly in the void space (i.e. 0)
    :type sources_number: int
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
        fig.suptitle("Radiation simulation with {} sources"
                     .format(sources_number))
        fig.savefig(export_pathname)
    print("Done")

    print("Extinction coefficient (beta):")
    print("x: " + str(round(beta_out[0], 1)) + ' +/- ' + str(round(beta_std_out[0], 1)))
    print("y: " + str(round(beta_out[1], 1)) + ' +/- ' + str(round(beta_std_out[1], 1)))
    print("z: " + str(round(beta_out[2], 1)) + ' +/- ' + str(round(beta_std_out[2], 1)))
    return beta_out, beta_std_out
