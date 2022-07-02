import numpy as np
import pumapy as puma
import pyvista as pv
import scipy.ndimage as nd
import os
import sys
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


### Transverse flow through a square array of cylinders
#
# The analytical solution for this case can be found in section 11 of
# [this paper](https://www.sciencedirect.com/science/article/abs/pii/030193228490079X) (equation 29).
# It describes the homogenized permeability of a square domain containing cylinders. In our case, we will consider
# a domain with equal unit sides, discretized with 100 voxels in each direction, and with cylinders with 0.1 radius:

r = 0.1  # cylinder radius
vf = 2. * np.pi * (r ** 2.)  # solid volume fraction

# The analytical solution can now be computed as:
keff_analytical = ((r ** 2) / (8 * vf)) * (-np.log(vf) - 1.47633597 + 2 * vf - 1.77428264 * vf ** 2 + 4.07770444 * vf ** 3 - 4.84227402 * vf ** 4)
print(f"Analytical diagonal permeability: {keff_analytical}")

# We can create the square array of cylinders by running the following cell:
ws = puma.generate_cylinder_square_array(100, 1. - vf)  # 100x100x1 domain with porosity = 1 - vf
ws.voxel_length = 1./ws.matrix.shape[0]  # i.e. side length = 1
print(f"Domain solid VF: {puma.compute_volume_fraction(ws, (1, 1))}")

puma.render_volume(ws, (1, 1), solid_color=(255,255,255), style='edges', notebook=False)

keff, (u_x, u_y, u_z) = puma.compute_permeability(ws, (1, 1), solver_type='direct')


p = pv.Plotter(shape=(1, 2))
ws.voxel_length = 1  # the voxel_length is converted to 1 for plotting the workspace together with the velocity
p.subplot(0, 0)
p.add_text("Body force in x, velocity")
puma.render_orientation(u_x, scale_factor=1e2, add_to_plot=p, plot_directly=False, notebook=False)
puma.render_volume(ws, (1, 1), solid_color=(255,255,255), style='surface', add_to_plot=p, plot_directly=False, notebook=False)
p.subplot(0, 1)
p.add_text("Body force in y, velocity")
puma.render_orientation(u_y, scale_factor=1e2, add_to_plot=p, plot_directly=False, notebook=False)
puma.render_volume(ws, (1, 1), solid_color=(255,255,255), style='surface', add_to_plot=p, plot_directly=False, notebook=False)
p.show()

