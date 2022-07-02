import numpy as np
import pumapy as puma
import pyvista as pv

# ## Tutorial: Permeability
# In this tutorial we introduce pumapy's permeability module. The numerical method behind the permeability
# homogenization function relies on a Finite Element method, which approximates both the velocity and pressure
# fields with first-order elements (i.e. Q1-Q1) and imposing a unit body force in each Cartesian direction.
# More details about the specifics of this method can be found in (https://www.mdpi.com/2311-5521/5/1/16),
# which was the starting point of the PuMA implementation.
# In this section of the tutorials, we will run two different cases: the first one simulates the flow through a
# square array of cylinders, for which the permeability can be computed analytically; the second one solves the
# Stokes equation for the flow around artificial fibers domains, whose fiber orientation is varied.


# ### Flow through artificial fibers with variable orientation
# In this section, we are going to showcase how to compute the permeability of a couple of artificial fiber samples.
# We set the orientation of the first one to be completely random, whereas we align the fibers of the second one with
# the XZ plane.

# This is how we can generate them:
ws1 = puma.generate_random_fibers(shape=(100, 100, 100), radius=3, porosity=0.7, phi=90, theta=90, length=200, max_iter=6)
ws2 = puma.generate_random_fibers(shape=(100, 100, 100), radius=3, porosity=0.7, phi=0, theta=90, length=200, max_iter=6)

p = pv.Plotter(shape=(1, 2))
p.subplot(0, 0)
p.add_text("Random orientation")
puma.render_volume(ws1, (1, ws1.max()), solid_color=None, cmap='jet', style='surface', add_to_plot=p, plot_directly=False, notebook=False)
p.subplot(0, 1)
p.add_text("Preferentially aligned with XZ plane")
puma.render_volume(ws2, (1, ws2.max()), solid_color=None, cmap='jet', style='surface', add_to_plot=p, plot_directly=False, notebook=False)
p.show()

# Now, we compute their permeabilities using the minres iterative solver (due to the significantly larger sparse system to solve):

keff1, u_x1, p_x1, u_y1, p_y1, u_z1, p_z1 = puma.compute_permeability(ws1, (1, ws1.max()), tol=1e-7, maxiter=10000, solver_type='minres')
keff2, u_x2, p_x2, u_y2, p_y2, u_z2, p_z2 = puma.compute_permeability(ws2, (1, ws2.max()), tol=1e-7, maxiter=10000, solver_type='minres')

# As you can see, the diagonal permeabilities in the first case are similar to each other, whereas the permeability in
# z direction for the second sample is significatly lower than in the other two directions, which is a direct cause of
# the fiber orientation.
# 
# We can now visualize their output velocity fields in Z as:

arrows_scale_factor = 5e11
u_z1[::2] = 0  # lower sampling for plotting arrows
u_z1[:, ::2] = 0
u_z2[::2] = 0  # lower sampling for plotting arrows
u_z2[:, ::2] = 0
p_z1[ws1.matrix > 0] = np.NAN
p_z2[ws2.matrix > 0] = np.NAN
p = pv.Plotter(shape=(2, 2))
p.subplot(0, 0)
p.add_text("Z velocity for fibers with random orientation")
puma.render_orientation(u_z1, scale_factor=arrows_scale_factor, solid_color=None, add_to_plot=p, plot_directly=False, notebook=False)
puma.render_volume(ws1, (1, ws1.max()), solid_color=(1,1,1), style='surface', add_to_plot=p, plot_directly=False, notebook=False)
p.subplot(0, 1)
p.add_text("Z velocity for fibers with preferential orientation on XZ plane")
puma.render_orientation(u_z2, scale_factor=arrows_scale_factor, solid_color=None, add_to_plot=p, plot_directly=False, notebook=False)
puma.render_volume(ws2, (1, ws2.max()), solid_color=(1,1,1), style='surface', add_to_plot=p, plot_directly=False, notebook=False)
p.subplot(1, 0)
p.add_text("pressure")
puma.render_volume(p_z1, solid_color=None, add_to_plot=p, plot_directly=False, cmap='jet', notebook=False)
p.subplot(1, 1)
p.add_text("pressure")
puma.render_volume(p_z2, solid_color=None, add_to_plot=p, plot_directly=False, cmap='jet', notebook=False)
p.show()

