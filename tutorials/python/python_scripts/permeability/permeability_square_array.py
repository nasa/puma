import numpy as np
import pumapy as puma
import pyvista as pv
import scipy.ndimage as nd
import os
import sys

# ## Tutorial: Permeability
# In this tutorial we introduce pumapy's permeability module. The numerical method behind the permeability
# homogenization function relies on a Finite Element method, which approximates both the velocity and pressure
# fields with first-order elements (i.e. Q1-Q1) and imposing a unit body force in each Cartesian direction.
# More details about the specifics of this method can be found in (https://www.mdpi.com/2311-5521/5/1/16),
# which was the starting point of the PuMA implementation.
# In this section of the tutorials, we will run two different cases: the first one simulates the flow through a
# square array of cylinders, for which the permeability can be computed analytically; the second one solves the
# Stokes equation for the flow around artificial fibers domains, whose fiber orientation is varied.

# ### Transverse flow through a square array of cylinders
# The analytical solution for this case can be found in section 11 of [this paper]
# (https://www.sciencedirect.com/science/article/abs/pii/030193228490079X) (equation 29). It describes the homogenized
# permeability of a square domain containing cylinders. In our case, we will consider a domain with equal unit sides,
# discretized with 300 voxels in each direction, and with cylinders with 0.1 radius:
r = 0.1  # cylinder radius
vf = 2. * np.pi * (r ** 2.)  # solid volume fraction

# The analytical solution can now be computed as:
keff_analytical = ((r ** 2) / (8 * vf)) * (-np.log(vf) - 1.47633597 + 2 * vf - 1.77428264 * vf ** 2 + 4.07770444 * vf ** 3 - 4.84227402 * vf ** 4)
print(f"Analytical diagonal permeability: {keff_analytical}")

# We can create the square array of cylinders by running the following cell:
ws = puma.generate_cylinder_square_array(100, 1. - vf)  # 100x100x1 domain with porosity = 1 - vf
ws.voxel_length = 1./ws.matrix.shape[0]  # i.e. side length = 1
print(f"Domain solid VF: {puma.compute_volume_fraction(ws, (1, 1))}")

puma.render_volume(ws, (1, 1), notebook=False)

# Finally, we can compute the exact numerical permeability in the three directions using a sparse direct solver by running:
keff, u_x, p_x, u_y, p_y, u_z, p_z = puma.compute_permeability(ws, (1, 1), solver_type='direct')

print(f"\nNumerical diagonal permeability: {keff[0, 0]}\n")

# We can also visualize the output velocity fields as:
arrows_scale_factor = 250
u_x[::2] = 0  # lower sampling for plotting arrows
u_x[:, ::2] = 0
u_y[::2] = 0
u_y[:, ::2] = 0
p_x[ws.matrix == 1] = np.NAN
p_y[ws.matrix == 1] = np.NAN
p = pv.Plotter(shape=(2, 2))
p.subplot(0, 0)
p.add_text("Body force in x, velocity")
puma.render_orientation(u_x, scale_factor=arrows_scale_factor, solid_color=None, add_to_plot=p, plot_directly=False, notebook=False)
puma.render_volume(ws, (1, 1), solid_color=(1,1,1), style='surface', add_to_plot=p, plot_directly=False, notebook=False)
p.subplot(0, 1)
p.add_text("Body force in y, velocity")
puma.render_orientation(u_y, scale_factor=arrows_scale_factor, solid_color=None, add_to_plot=p, plot_directly=False, notebook=False)
puma.render_volume(ws, (1, 1), solid_color=(1,1,1), style='surface', add_to_plot=p, plot_directly=False, notebook=False)

p.subplot(1, 0)
p.add_text("pressure")
puma.render_volume(p_x, solid_color=None, add_to_plot=p, plot_directly=False, cmap='jet', notebook=False)
p.subplot(1, 1)
p.add_text("pressure")
puma.render_volume(p_y, solid_color=None, add_to_plot=p, plot_directly=False, cmap='jet', notebook=False)
p.show()

