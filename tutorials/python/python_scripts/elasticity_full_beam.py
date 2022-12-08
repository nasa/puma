import numpy as np
import pumapy as puma
import os

# The objective of this notebook is to familiarize new users with the main datastructures that stand at the basis of the
# PuMA project, and outline the functions to compute material properties (please refer to these papers
# ([1](https://www.sciencedirect.com/science/article/pii/S2352711018300281),
# [2](https://www.sciencedirect.com/science/article/pii/S235271102100090X)) for more details on the software).

notebook = False  # when running locally, actually open pyvista window
export_path = "out"  # CHANGE THIS PATH
if not os.path.exists(export_path):
    os.makedirs(export_path)

# ## Tutorial
# In this tutorial we demonstrate the use of the compute_elasticity and compute_stress_analysis functions.
# These functions rely on a stress analysis solver that uses the finite volume Multi-Point Stress Approximation (MPSA) method.

# ### MPSA Elasticity
# #### Example: full built-in beam
# This case is for a fully built-in homogeneous beam with a y displacement in the middle. Because of the symmetry of this
# case, we only model half of it.

export_name = 'builtinbeam'
X = 100
Y = 12
Z = 12
ws = puma.Workspace.from_shape_value((X, Y, Z), 1)
ws.voxel_length = 1

ws[:, [0, -1]] = 0
ws[:, :, [0, -1]] = 0

puma.render_volume(ws, cutoff=(0, 255), solid_color=(255,255,255), style='edges', notebook=notebook)

elast_map = puma.experimental.ElasticityMap()
elast_map.add_isotropic_material((0, 0), 1e-5, 0.3)
elast_map.add_isotropic_material((1, 1), 200, 0.3)

bc = puma.experimental.ElasticityBC(ws)
bc.xfaces[0, :, :, 0] = 0  # dirichlet displacement to zero on the x -ve face
bc.xfaces[0, :, :, 1] = 0
bc.xfaces[0, :, :, 2] = 0
bc.xfaces[1, :, :, 0] = 0   # dirichlet x and z displacements to zero on x +ve face
bc.xfaces[1, :, :, 1] = -1  # dirichlet y displacement of -1 on x +ve face
bc.xfaces[1, :, :, 2] = 0

u, s, t = puma.experimental.compute_stress_analysis(ws, elast_map, bc, side_bc='s', solver_type="bicgstab")
puma.experimental.warp_elasticity_fields(ws[:, 1:-1, 1:-1], u[:, 1:-1, 1:-1], s[:, 1:-1, 1:-1], t[:, 1:-1, 1:-1], 20, show_original=0., show_edges=False, xy_view=True, notebook=notebook)

puma.experimental.export_elasticity_fields_vti(os.path.join(export_path, export_name), ws, u, s, t)

