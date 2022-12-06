import numpy as np
import pumapy as puma
import os

notebook = False  # when running locally, actually open pyvista window
export_path = "out"  # CHANGE THIS PATH
if not os.path.exists(export_path):
    os.makedirs(export_path)

# ## Tutorial
# In this tutorial we demonstrate the use of the compute_elasticity and compute_stress_analysis functions.
# These functions rely on a stress analysis solver that uses the finite volume Multi-Point Stress Approximation (MPSA) method.

# ### MPSA Elasticity
# #### Example: harmonic averaging, in series along x with free sides
# We now run a block of material split into two phases with different properties.

export_name = 'halfmat'
X = 20
Y = 22
Z = 22
ws = puma.Workspace.from_shape_value((X, Y, Z), 1)
ws[int(X / 2):] = 2
ws[:, [0, -1]] = 0  # air voxels
ws[:, :, [0, -1]] = 0

puma.render_volume(ws, solid_color=(255,255,255), notebook=notebook, style='edges', cmap='jet')

# We can assign the elasticity of the two materials and compute the resulting overall elasticity of the two phases combined as follows:
elast_map = puma.experimental.ElasticityMap()
elast_map.add_isotropic_material((0, 0), 1e-5, 0.3)  # N.B. air has to be assigned a very low Young modulus, not exactly zero
elast_map.add_isotropic_material((1, 1), 200, 0.3)
elast_map.add_isotropic_material((2, 2), 400, 0.1)

#  In this example, we use the compute_elasticity function, which is useful specifically to compute the homogenized
#  (or effective) elasticity of a multi-phase material. This function imposes a unit displacement along the direction
#  specified by holding the last slice of voxels in place with dirichlet boundary conditions. The side boundary conditions
#  can be set as either 'p'eriodic or 's'ymmetric.

C, u, s, t = puma.experimental.compute_elasticity(ws, elast_map, direction='x', side_bc='p', solver_type="direct")

# Now we can visualize the displacement and stress fields as:

puma.experimental.warp_elasticity_fields(ws[:, 1:-1, 1:-1], u[:, 1:-1, 1:-1], s[:, 1:-1, 1:-1], t[:, 1:-1, 1:-1], 20, show_original=0.,
                            show_edges=False, notebook=notebook)

# Finally, we can export all of this data (domain, displacement, direct stress, shear stress) in a single .vti file as: 

puma.experimental.export_elasticity_fields_vti(os.path.join(export_path, export_name), ws, u, s, t)
