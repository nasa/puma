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
# #### Example: cracked plate
# In this example, we model a plate with a single row of voxels removed, mimicking a crack. 

export_name = 'crackedplate'
X = 200
Y = 52
Z = 5
crack = 10
ws = puma.Workspace.from_shape_value((X, Y, Z), 1)
ws.voxel_length = 1
ws[X//2-1:X//2+1, -crack + 1:] = 0
ws[:, [0, -1]] = 0
ws[:, :, [0, -1]] = 0

elast_map = puma.experimental.ElasticityMap()
elast_map.add_isotropic_material((0, 0), 1e-5, 0.3)
elast_map.add_isotropic_material((1, 1), 200, 0.3)

bc = puma.experimental.ElasticityBC(ws)
bc.xfaces[0, :, :, 0] = 0
bc.xfaces[0, :, :, 1] = 0
bc.xfaces[0, :, :, 2] = 0
bc.xfaces[1, :, :, 0] = 1
bc.xfaces[1, :, :, 1] = 0
bc.xfaces[1, :, :, 2] = 0

u, s, t = puma.experimental.compute_stress_analysis(ws, elast_map, bc, side_bc='s', solver_type="bicgstab")

u[ws.matrix == 0] = 0  # set air to zero
puma.experimental.warp_elasticity_fields(ws[:, 1:-1, 2:3], u[:, 1:-1, 2:3], s[:, 1:-1, 2:3], t[:, 1:-1, 2:3], 20, show_original=0., show_edges=False, xy_view=True, rm_id=0)

puma.experimental.export_elasticity_fields_vti(os.path.join(export_path, export_name), ws, u, s, t)


