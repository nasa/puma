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


# ### Example 1: harmonic averaging, in series along x with free sides
# The first example that we run is for a block of material split into two phases with different properties.
export_name = 'halfmat'
X = 20
Y = 20
Z = 20
ws = puma.Workspace.from_shape_value((X, Y, Z), 1)
ws[int(X / 2):] = 2
# ws.show_matrix()

puma.render_volume(ws, solid_color=(255,255,255), notebook=False, style='edges', cmap='jet')



# We can now assign the elasticity of the two materials and compute the resulting overall elasticity of the
# two phases combined as follows:
elast_map = puma.ElasticityMap()
elast_map.add_isotropic_material((1, 1), 200, 0.3)
elast_map.add_isotropic_material((2, 2), 400, 0.1)


#  In this example, we use the compute_elasticity function, which is useful specifically to compute the homogenized
#  (or effective) elasticity of a multi-phase material. This function imposes a unit displacement along the direction
#  specified by holding the last slice of voxels in place with dirichlet boundary conditions. The side boundary
#  conditions can be set as either 'p'eriodic, 's'ymmetric or 'f'ree. In this case we set them as free with 'f'.
C, u, s, t = puma.compute_elasticity(ws, elast_map, direction='x', side_bc='f', solver_type="direct")

# Now we can visualize the displacement and stress fields as:
results = puma.Workspace()
results.orientation = u[:, :Y//2]  # cut domain in half to show internal stresses
scale_factor = 10

p = pv.Plotter(shape=(2, 3))
p.subplot(0, 0)
p.add_text("Colored by sigma_xx")
results.matrix = s[:, :Y//2, :, 0]  # assign direct stresses to matrix
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(0, 1)
p.add_text("Colored by sigma_yy")
results.matrix = s[:, :Y//2, :, 1]
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(0, 2)
p.add_text("Colored by sigma_zz")
results.matrix = s[:, :Y//2, :, 2]
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(1, 0)
p.add_text("Colored by tau_yz")
results.matrix = t[:, :Y//2, :, 0]  # assign shear stresses to matrix
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(1, 1)
p.add_text("Colored by tau_xz")
results.matrix = t[:, :Y//2, :, 1]
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(1, 2)
p.add_text("Colored by tau_yz")
results.matrix = t[:, :Y//2, :, 2]
puma.render_warp(results, color_by='matrix', scale_factor=scale_factor, style='edges', notebook=False, add_to_plot=p, plot_directly=False)
p.show()



# Finally, we can export all of this data (domain, displacement, direct stress, shear stress) in a single .vti file as:
puma.export_vti(os.path.join(export_path, export_name), {"ws": ws, "disp": u, "sigma": s, "tau": t})
