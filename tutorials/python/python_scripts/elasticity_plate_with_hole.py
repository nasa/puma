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

# #### Example: plate with a hole
# In this example, we model a plate with a hole in the middle pulled in the x direction by an input displacement.

export_name = 'platehole'
X = 100
Y = 102
Z = 3
ws = puma.Workspace.from_shape_value((X, Y, Z), 1)
ws.voxel_length = 1

ws[:, [0, -1]] = 0
ws[:, :, [0, -1]] = 0

# creating circular hole
mask = np.ones((X, Y, Z), dtype=bool)
mask[X//2, Y//2] = 0
from scipy import ndimage as nd
distance_mask = nd.morphology.distance_transform_edt(mask)
max_distance = np.max(distance_mask)
distance_mask_display = (distance_mask*255./max_distance).astype(dtype=np.uint8)
in_range = distance_mask <= 35  # this sets how big is the hole with a threshold
ws[in_range] = 0

puma.render_volume(ws.matrix[:, :, 1:2])

# setting material
elast_map = puma.experimental.ElasticityMap()
elast_map.add_isotropic_material((0, 0), 1e-5, 0.3)
elast_map.add_isotropic_material((1, 1), 200, 0.3)

# setting dirichlet boundary conditions
bc = puma.experimental.ElasticityBC(ws)
bc.xfaces[0, :, :, 0] = 0
bc.xfaces[0, :, :, 1] = 0
bc.xfaces[0, :, :, 2] = 0
bc.xfaces[1, :, :, 0] = 1
bc.xfaces[1, :, :, 1] = 0
bc.xfaces[1, :, :, 2] = 0

u, s, t = puma.experimental.compute_stress_analysis(ws, elast_map, bc, side_bc='s', solver_type="bicgstab")

u[ws.matrix == 0] = 0  # set air to zero
puma.experimental.warp_elasticity_fields(ws[:, 1:-1, 1:2], u[:, 1:-1, 1:2], s[:, 1:-1, 1:2], t[:, 1:-1, 1:2], 5, show_original=0., show_edges=False, xy_view=True, rm_id=0, notebook=notebook)

puma.experimental.export_elasticity_fields_vti(os.path.join(export_path, export_name), ws, u, s, t)
