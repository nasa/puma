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
    
# ### Finite Element Elasticity
# 
# An extra method to compute the elasticity was recently added, which leverages a Finite Element solver based on a Q1-Q1
# Element-by-element implementation. To learn more about this method, please refer to the permeability tutorial.
# In order to use it, a user can simply run similar options as above, adding a flag specifying that the method to be
# used is 'fe'. Some details on the solver:
# 
# - Only periodic boundary conditions are currently available, so the side_bc flag is ignored
# - A matrix-free solution approach, which is slightly slower but very memory efficient, can be run when no local orientation
# is present (i.e. when the local phases are isotropic)
# - The ligher and faster 'minres' iterative solver can be used since the matrices that are assembled are symmetric
# - The simulation imposes a residual strain on the whole domain, rather than a unitary displacement gradient on the boundaries

dim = 50
ws = puma.generate_cylinder_square_array(dim, 0.44)
ws.matrix = np.repeat(ws.matrix, 5, axis=2)
puma.render_volume(ws, notebook=notebook)

mat_elast = puma.experimental.ElasticityMap()
mat_elast.add_isotropic_material((0, 0), 68.3, 0.3)
mat_elast.add_isotropic_material((1, 1), 379.3, 0.1)

solver = "minres"
C = np.zeros((6, 6))
C[:, 0], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='x',  solver_type=solver, method='fe')
C[:, 1], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='y',  solver_type=solver, method='fe')
C[:, 2], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='z',  solver_type=solver, method='fe')
C[:, 3], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='yz', solver_type=solver, method='fe')
C[:, 4], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='xz', solver_type=solver, method='fe')
C[:, 5], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='xy', solver_type=solver, method='fe')

print(C)
coeffs = puma.experimental.get_E_nu_from_elasticity(C)

# #### Example: FiberForm elasticity
# Now let's compute the elasticity of FiberForm

ws = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)  # import example file

# scale down domain to speed up computations
ws.rescale(0.25, segmented=False)

# compute orientation
puma.compute_orientation_st(ws, cutoff=(90, 255), sigma=0.7, rho=1.4)

# threshold it to binary, 0=void 1=fibers
ws.binarize_range((90, 255))

# remove floating voxels, unconnected in a specific direction, since they would create singularities
ws = puma.experimental.remove_rbms(ws, 0, 'x')

# visualize it
puma.render_volume(ws, cutoff=(1, ws.max()), cmap='jet', style='edges', notebook=notebook)

# assign material
mat_elast = puma.experimental.ElasticityMap()
mat_elast.add_isotropic_material((0, 0), 1e-5, 0)
mat_elast.add_material_to_orient((1, 1), 10, 8, 0.3, 0.3, 5)

C = np.zeros((6, 6))
C[:, 0], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='x', solver_type='minres', method='fe')
u[ws.matrix == 0] = 0  # set air to zero, i.e. no displacement
puma.experimental.warp_elasticity_fields(ws, u, s, t, scale_factor=1, notebook=notebook)

C[:, 1], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='y', solver_type='minres', method='fe')
C[:, 2], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='z', solver_type='minres', method='fe')
C[:, 3], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='yz', solver_type='minres', method='fe')
C[:, 4], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='xz', solver_type='minres', method='fe')
C[:, 5], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='xy', solver_type='minres', method='fe')

print(C)
coeffs = puma.experimental.get_E_nu_from_elasticity(C)



