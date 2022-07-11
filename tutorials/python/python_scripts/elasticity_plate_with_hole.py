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


# ### Example 3: plate with a hole
# In this example, we model a plate with a hole in the middle pulled in the x direction by a certain displacement.

export_name = 'platehole'
X = 50
Y = 50
Z = 3
ws = puma.Workspace.from_shape_value((X, Y, Z), 1)
ws.voxel_length = 1

# creating circular hole
mask = np.ones((X, Y, Z), dtype=bool)
mask[X//2, Y//2] = 0
distance_mask = nd.morphology.distance_transform_edt(mask)
max_distance = np.max(distance_mask)
distance_mask_display = (distance_mask*255./max_distance).astype(dtype=np.uint8)
in_range = distance_mask <= 17  # this sets how big is the hole with a threshold
ws[in_range] = 0

# setting material
elast_map = puma.ElasticityMap()
elast_map.add_isotropic_material((1, 1), 200, 0.3)

# setting dirichlet boundary conditions
bc = puma.ElasticityBC(ws)
bc.dirichlet[0, :, :, 0] = 0
bc.dirichlet[-1, :, :, 0] = 1

u, s, t = puma.compute_stress_analysis(ws, elast_map, bc, side_bc='f', solver_type="direct")

results = puma.Workspace()
u[ws.matrix == 0] = np.NAN  # set air displacement to NAN to avoid plotting it
results.orientation = u[:, :, :Z//2]
scale_factor = notebook=False

p = pv.Plotter(shape=(2, 3))
p.subplot(0, 0)
p.add_text("Colored by sigma_xx")
results.matrix = s[:, :, :Z//2, 0]  # assign direct stresses to matrix
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(0, 1)
p.add_text("Colored by sigma_yy")
results.matrix = s[:, :, :Z//2, 1]
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(0, 2)
p.add_text("Colored by sigma_zz")
results.matrix = s[:, :, :Z//2, 2]
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(1, 0)
p.add_text("Colored by tau_yz")
results.matrix = t[:, :, :Z//2, 0]  # assign shear stresses to matrix
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(1, 1)
p.add_text("Colored by tau_xz")
results.matrix = t[:, :, :Z//2, 1]
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(1, 2)
p.add_text("Colored by tau_yz")
results.matrix = t[:, :, :Z//2, 2]
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.show(cpos="xy")

puma.export_vti(os.path.join(export_path, export_name), {"ws": ws, "disp": u, "sigma": s, "tau": t})


