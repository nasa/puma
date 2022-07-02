import numpy as np
import pumapy as puma
import pyvista as pv
import scipy.ndimage as nd
import os


# ## Tutorial: Elasticity
# In this tutorial we demonstrate the use of the compute_elasticity and compute_stress_analysis functions.
# These functions rely on a stress analysis solver that uses the finite volume Multi-Point Stress Approximation
# (MPSA) method.

# We will run four different verification cases. Change the path of the file outputs:

export_path = "out"  # CHANGE THIS PATH
if not os.path.exists(export_path):
    os.makedirs(export_path)


# ### Example 2: full built-in beam
# The second case is for a fully built-in homogeneous beam with a z displacement in the middle. Because of the
# symmetry of this case, we only model half of it.

export_name = 'builtinbeam'
X = 10
Y = 50
Z = 10
ws = puma.Workspace.from_shape_value((X, Y, Z), 1)
ws.voxel_length = 1

puma.render_volume(ws, cutoff=(0, 255), solid_color=(1,1,1), style='edges', notebook=False)

# We then run set its elasticity as:
elast_map = puma.ElasticityMap()
elast_map.add_isotropic_material((1, 1), 200, 0.3)

# Since we want to set a specific displacement, we need to have more control on the type of boundary conditions we set.
# This can be done by creating an ElasticityBC object as:
bc = puma.ElasticityBC(ws)
bc.dirichlet[:, 0] = 0  # dirichlet displacement to zero on the y -ve face (i.e. hold in place)
bc.dirichlet[:, -1, :, :2] = 0  # dirichlet y and z displacements on y +ve face (i.e. free slip in x)
bc.dirichlet[:, -1, :, 2] = -1  # dirichlet z displacement of -1 on y +ve face
# puma.Workspace.show_orientation(bc)

# Plot the boundary conditions array inside the ElasticityBC object
dir_copy = bc.dirichlet.copy()
# the unset DOF are usually set to Inf, but for plotting purposes we set them to NaN
dir_copy[np.isinf(dir_copy)] = np.NaN
p = pv.Plotter(shape=(1, 3))
p.subplot(0, 0)
p.add_text("Dirichlet displacement in x")
puma.render_volume(dir_copy[:,:,:,0], notebook=False, add_to_plot=p, plot_directly=False, cmap='jet')
p.subplot(0, 1)
p.add_text("Dirichlet displacement in y")
puma.render_volume(dir_copy[:,:,:,1], notebook=False, add_to_plot=p, plot_directly=False, cmap='jet')
p.subplot(0, 2)
p.add_text("Dirichlet displacement in z")
puma.render_volume(dir_copy[:,:,:,2], notebook=False, add_to_plot=p, plot_directly=False, cmap='jet')
p.show()

u, s, t = puma.compute_stress_analysis(ws, elast_map, bc, side_bc='f', solver_type="direct")

results = puma.Workspace()
results.orientation = u[:X//2]
scale_factor = 10

p = pv.Plotter(shape=(2, 3))
p.subplot(0, 0)
p.add_text("Colored by sigma_xx")
results.matrix = s[:X//2, :, :, 0]  # assign direct stresses to matrix
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(0, 1)
p.add_text("Colored by sigma_yy")
results.matrix = s[:X//2, :, :, 1]
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(0, 2)
p.add_text("Colored by sigma_zz")
results.matrix = s[:X//2, :, :, 2]
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(1, 0)
p.add_text("Colored by tau_yz")
results.matrix = t[:X//2, :, :, 0]  # assign shear stresses to matrix
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(1, 1)
p.add_text("Colored by tau_xz")
results.matrix = t[:X//2, :, :, 1]
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(1, 2)
p.add_text("Colored by tau_yz")
results.matrix = t[:X//2, :, :, 2]
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.show()

puma.export_vti(os.path.join(export_path, export_name), {"ws": ws, "disp": u, "sigma": s, "tau": t})
