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


# ### Example 4: cracked plate
#
# In this final example, we model a plate with a single row of voxels removed, mimicking a crack.


export_name = 'crackedplate'
X = 25
Y = 100
Z = 3
ws = puma.Workspace.from_shape_value((X, Y, Z), 1)
ws.voxel_length = 1

ws[:10, Y//2-1:Y//2+1] = 0

elast_map = puma.ElasticityMap()
elast_map.add_isotropic_material((1, 1), 200, 0.3)

bc = puma.ElasticityBC(ws)
bc.dirichlet[:, 0, :, 1] = 0
bc.dirichlet[:, -1, :, 1] = 1

u, s, t = puma.compute_stress_analysis(ws, elast_map, bc, side_bc='f', solver_type="direct")

results = puma.Workspace()
u[ws.matrix == 0] = np.NAN  # set air displacement to NAN to avoid plotting it
results.orientation = u[:, :, :Z//2]
scale_factor = 10

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
