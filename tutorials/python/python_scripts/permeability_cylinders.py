import numpy as np
import pumapy as puma
import pyvista as pv

# ## Tutorial: Permeability
# In this tutorial we introduce pumapy's permeability module. The numerical method behind the permeability
# homogenization function relies on a Finite Element method, which approximates both the velocity and pressure fields
# with first-order elements (i.e. Q1-Q1) and imposing a unit body force in each Cartesian direction. More details
# about the specifics of this method can be found in [this publication](https://www.mdpi.com/2311-5521/5/1/16), which
# was the starting point of the PuMA implementation.
#
# In this section of the tutorials, we will run two different cases: the first one simulates the flow through a square
# array of cylinders, for which the permeability can be computed analytically; the second one solves the Stokes
# equation for the flow around artificial fiber domains, whose fiber orientation is varied.
#
# **Important notes on the use of the compute_permeability function:**
#
#
# The permeability is one of the most advanced material property computations in PuMA and it requires significant
# resources in terms of memory and runtime. For this reason, several solver types and options are available to the
# user in order to tailor the use of this function to the specific requirement. Three different solver types can be
# used: a direct sparse solver (solver_type='direct'), which requires the assembly of a big sparse A matrix; and two
# iterative solver options (MINimum RESidual, solver_type='minres', and the Conjugate Gradient, solver_type='cg').
# The direct solver is more accurate, since it finds a correct solution to the inversion of the sparse A matrix, but
# it requires the most memory, so it can only be run on small domains (e.g. up to $100^3$ voxels). The benefit of
# this solver is the non-approximate solution it gives, as well as the fact that the solver does not need to be called
# three times for the three directions, since it finds the correct $A^{-1}$ that can be multiplied with the three
# $b$ forcing vectors to find the velocity solution directly. On the other hand, the iterative solvers are what needs
# to be used for production runs on larger domains. For the iterative solvers, a crucial flag is the matrix_free
# parameter, which tells the solver whether to assemble a large A sparse matrix (matrix_free=False) or to create a
# LinearOperator instead, which computes the matrix-vector operation directly. The former is faster, but it requires
# significantly more memory, whereas the second one is much lighter in the memory requirement, but it needs to compute
# more operations at each iteration. Finally, a Jacobi preconditioner is also available only for the cg solver, which
# requires slightly more memory, but it significantly reduces the number of iterations. The preconditioner is not
# available to the minres solver because it leads to asymmetries in the A sparse matrix. For very large runs where
# only the effective permeability coefficient is needed, it is recommended to use the following set of options to
# minimize the memory: solver_type='minres', matrix_free=True, precondition=False, output_fields=False.


# ### Flow through artificial fibers with variable orientation
#
# In this section, we are going to showcase how to compute the permeability of a couple of artificial fiber samples.
# We set the orientation of the first one to be completely random, whereas we align the fibers of the second one with
# the XZ plane.
#
# This is how we can generate them:

ws1 = puma.generate_random_fibers(shape=(50, 50, 50), radius=2, porosity=0.7, phi=90, theta=90, length=200, max_iter=6)
ws2 = puma.generate_random_fibers(shape=(50, 50, 50), radius=2, porosity=0.7, phi=0, theta=90, length=200, max_iter=6)

p = pv.Plotter(shape=(1, 2))
p.subplot(0, 0)
p.add_text("Random orientation")
puma.render_volume(ws1, (1, ws1.max()), solid_color=None, cmap='jet', style='surface', add_to_plot=p, plot_directly=False, notebook=False)
p.subplot(0, 1)
p.add_text("Preferentially aligned with XZ plane")
puma.render_volume(ws2, (1, ws2.max()), solid_color=None, cmap='jet', style='surface', add_to_plot=p, plot_directly=False, notebook=False)
p.show()


# Now, we compute their permeabilities using the minres iterative solver due to the significantly larger sparse system
# to solve (refer to the first permeability section or to the documentation for more specs on the solver types and options):
keff1, (u_x1, u_y1, u_z1) = puma.compute_permeability(ws1, (1, ws1.max()), tol=1e-8, maxiter=10000, solver_type='minres', matrix_free=False)
keff2, (u_x2, u_y2, u_z2) = puma.compute_permeability(ws2, (1, ws2.max()), tol=1e-8, maxiter=10000, solver_type='minres', matrix_free=False)


# As you can see, the diagonal permeabilities in the first case are similar to each other, whereas the permeability in
# z direction for the second sample is significatly lower than in the other two directions, which is a direct cause of
# the fiber orientation.
#
# We can now visualize their output velocity fields (normalized by the voxel_length) in Z as:

p = pv.Plotter(shape=(2, 3), off_screen=False)
ws1.voxel_length = 1  # the voxel_length is converted to 1 for plotting the workspace together with the velocity
ws2.voxel_length = 1
p.subplot(0, 0)
p.add_text("u_x for fibers with random orientation")
puma.render_orientation(u_x1, add_to_plot=p, scale_factor=1e12, plot_directly=False)
puma.render_volume(ws1, cutoff=(1, ws1.max()), cmap='jet', add_to_plot=p, plot_directly=False, notebook=False)
p.subplot(0, 1)
p.add_text("u_y")
puma.render_orientation(u_y1, add_to_plot=p, scale_factor=1e12, plot_directly=False)
puma.render_volume(ws1, cutoff=(1, ws1.max()), cmap='jet', add_to_plot=p, plot_directly=False, notebook=False)
p.subplot(0, 2)
p.add_text("u_z")
puma.render_orientation(u_z1, add_to_plot=p, scale_factor=1e12, plot_directly=False)
puma.render_volume(ws1, cutoff=(1, ws1.max()), cmap='jet', add_to_plot=p, plot_directly=False, notebook=False)
p.subplot(1, 0)
p.add_text("u_x for fibers with preferential orientation on XZ plane")
puma.render_orientation(u_x2, add_to_plot=p, scale_factor=1e12, plot_directly=False)
puma.render_volume(ws2, cutoff=(1, ws2.max()), cmap='jet', add_to_plot=p, plot_directly=False, notebook=False)
p.subplot(1, 1)
p.add_text("u_y")
puma.render_orientation(u_y2, add_to_plot=p, scale_factor=1e12, plot_directly=False)
puma.render_volume(ws2, cutoff=(1, ws2.max()), cmap='jet', add_to_plot=p, plot_directly=False, notebook=False)
p.subplot(1, 2)
p.add_text("u_z")
puma.render_orientation(u_z2, add_to_plot=p, scale_factor=1e12, plot_directly=False)
puma.render_volume(ws2, cutoff=(1, ws2.max()), cmap='jet', add_to_plot=p, plot_directly=False, notebook=False)
p.show()