#!/usr/bin/env python
# coding: utf-8

# ![puma logo](https://github.com/nasa/puma/raw/main/doc/source/puma_logo.png)

# # Elasticity

# The objective of this notebook is to familiarize new users with the main datastructures that stand at the basis of the
# PuMA project, and outline the functions to compute material properties (please refer to these papers
# ([1](https://www.sciencedirect.com/science/article/pii/S2352711018300281),
# [2](https://www.sciencedirect.com/science/article/pii/S235271102100090X)) for more details on the software).

# ## Installation setup and imports

# The first code block will execute the necessary installation and package import.
# 
# If you are running this jupyter notebook locally on your machine, assuming you have already installed the software,
# then the installation step will be skipped

# In[ ]:


# for interactive slicer
get_ipython().run_line_magic('matplotlib', 'widget')
import numpy as np
import pumapy as puma
import os

if 'BINDER_SERVICE_HOST' in os.environ:  # ONLINE JUPYTER ON BINDER
    from pyvirtualdisplay import Display
    display = Display(visible=0, size=(600, 400))
    display.start()  # necessary for pyvista interactive plots
    notebook = True

else:  # LOCAL JUPYTER NOTEBOOK
    notebook = False  # when running locally, actually open pyvista window


# ## Tutorial
# 
# In this tutorial we demonstrate the use of the compute_elasticity and compute_stress_analysis functions.
# These functions rely on a stress analysis solver that uses the finite volume Multi-Point Stress Approximation (MPSA) method.

# We will run four different verification cases. Change the path of the file outputs:

# In[ ]:


export_path = "out"  # CHANGE THIS PATH

if not os.path.exists(export_path):
    os.makedirs(export_path)


# ### MPSA Elasticity

# #### Example: characterizing periodic square cylinder array
# 
# In this example we compute the 6x6 C tensor for a square array of cylinders and show how to convert this tensor into its
# mechanical properties (Young's moduli, Poison's ratios, Shear moduli). The results match with what is reported in
# [this paper](https://link.springer.com/article/10.1007/s00366-018-0616-4).

# In[ ]:


dim = 50
ws = puma.generate_cylinder_square_array(dim, 0.44)
ws.matrix = np.repeat(ws.matrix, 5, axis=2)
puma.render_volume(ws, notebook=notebook)

mat_elast = puma.experimental.ElasticityMap()
mat_elast.add_isotropic_material((0, 0), 68.3, 0.3)
mat_elast.add_isotropic_material((1, 1), 379.3, 0.1)


# In[ ]:


solver = "bicgstab"

C = np.zeros((6, 6))
C[:, 0], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='x', side_bc='p', solver_type=solver)
puma.experimental.warp_elasticity_fields(ws, u, s, t, scale_factor=10, xy_view=True, notebook=notebook)


# In[ ]:


C[:, 1], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='y',  side_bc='p', solver_type=solver)
C[:, 2], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='z',  side_bc='p', solver_type=solver)
C[:, 3], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='yz', side_bc='p', solver_type=solver)
C[:, 4], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='xz', side_bc='p', solver_type=solver)
C[:, 5], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='xy', side_bc='p', solver_type=solver)
puma.experimental.warp_elasticity_fields(ws, u, s, t, scale_factor=10, xy_view=True, notebook=notebook)


# In[ ]:


print(C)
coeffs = puma.experimental.get_E_nu_from_elasticity(C)


# #### Example: harmonic averaging, in series along x with free sides
# 
# We now run a block of material split into two phases with different properties.

# In[ ]:


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

# In[ ]:


elast_map = puma.experimental.ElasticityMap()
elast_map.add_isotropic_material((0, 0), 1e-5, 0.3)  # N.B. air has to be assigned a very low Young modulus, not exactly zero
elast_map.add_isotropic_material((1, 1), 200, 0.3)
elast_map.add_isotropic_material((2, 2), 400, 0.1)


#  In this example, we use the compute_elasticity function, which is useful specifically to compute the homogenized
#  (or effective) elasticity of a multi-phase material. This function imposes a unit displacement along the direction
#  specified by holding the last slice of voxels in place with dirichlet boundary conditions. The side boundary conditions
#  can be set as either 'p'eriodic or 's'ymmetric.

# In[ ]:


C, u, s, t = puma.experimental.compute_elasticity(ws, elast_map, direction='x', side_bc='p', solver_type="direct")


# Now we can visualize the displacement and stress fields as:

# In[ ]:


puma.experimental.warp_elasticity_fields(ws[:, 1:-1, 1:-1], u[:, 1:-1, 1:-1], s[:, 1:-1, 1:-1], t[:, 1:-1, 1:-1], 20, show_original=0.,
                            show_edges=False, notebook=notebook)


# Finally, we can export all of this data (domain, displacement, direct stress, shear stress) in a single .vti file as: 

# In[ ]:


puma.experimental.export_elasticity_fields_vti(os.path.join(export_path, export_name), ws, u, s, t)


# #### Example: full built-in beam
# This case is for a fully built-in homogeneous beam with a y displacement in the middle. Because of the symmetry of this
# case, we only model half of it.

# In[ ]:


export_name = 'builtinbeam'
X = 100
Y = 12
Z = 12
ws = puma.Workspace.from_shape_value((X, Y, Z), 1)
ws.voxel_length = 1

ws[:, [0, -1]] = 0
ws[:, :, [0, -1]] = 0

puma.render_volume(ws, cutoff=(0, 255), solid_color=(255,255,255), style='edges', notebook=notebook)


# We then run set its elasticity as:

# In[ ]:


elast_map = puma.experimental.ElasticityMap()
elast_map.add_isotropic_material((0, 0), 1e-5, 0.3)
elast_map.add_isotropic_material((1, 1), 200, 0.3)


# Since we want to set a specific displacement, we need to have more control on the type of boundary conditions we set.
# This can be done by creating an ElasticityBC object as:

# In[ ]:


bc = puma.experimental.ElasticityBC(ws)
bc.xfaces[0, :, :, 0] = 0  # dirichlet displacement to zero on the x -ve face
bc.xfaces[0, :, :, 1] = 0
bc.xfaces[0, :, :, 2] = 0
bc.xfaces[1, :, :, 0] = 0   # dirichlet x and z displacements to zero on x +ve face
bc.xfaces[1, :, :, 1] = -1  # dirichlet y displacement of -1 on x +ve face
bc.xfaces[1, :, :, 2] = 0


# In[ ]:


u, s, t = puma.experimental.compute_stress_analysis(ws, elast_map, bc, side_bc='s', solver_type="bicgstab")


# In[ ]:


puma.experimental.warp_elasticity_fields(ws[:, 1:-1, 1:-1], u[:, 1:-1, 1:-1], s[:, 1:-1, 1:-1], t[:, 1:-1, 1:-1], 20, show_original=0., show_edges=False, xy_view=True, notebook=notebook)


# In[ ]:


puma.experimental.export_elasticity_fields_vti(os.path.join(export_path, export_name), ws, u, s, t)


# #### Example: plate with a hole
# 
# In this example, we model a plate with a hole in the middle pulled in the x direction by an input displacement.

# In[ ]:


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


# In[ ]:


u[ws.matrix == 0] = 0  # set air to zero
puma.experimental.warp_elasticity_fields(ws[:, 1:-1, 1:2], u[:, 1:-1, 1:2], s[:, 1:-1, 1:2], t[:, 1:-1, 1:2], 5, show_original=0., show_edges=False, xy_view=True, rm_id=0, notebook=notebook)


# In[ ]:


puma.experimental.export_elasticity_fields_vti(os.path.join(export_path, export_name), ws, u, s, t)


# #### Example: cracked plate
# 
# In this example, we model a plate with a single row of voxels removed, mimicking a crack. 

# In[ ]:


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


# In[ ]:


u[ws.matrix == 0] = 0  # set air to zero
puma.experimental.warp_elasticity_fields(ws[:, 1:-1, 2:3], u[:, 1:-1, 2:3], s[:, 1:-1, 2:3], t[:, 1:-1, 2:3], 20, show_original=0., show_edges=False, xy_view=True, rm_id=0)


# In[ ]:


puma.experimental.export_elasticity_fields_vti(os.path.join(export_path, export_name), ws, u, s, t)


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

# In[ ]:


dim = 50
ws = puma.generate_cylinder_square_array(dim, 0.44)
ws.matrix = np.repeat(ws.matrix, 5, axis=2)
puma.render_volume(ws, notebook=notebook)

mat_elast = puma.experimental.ElasticityMap()
mat_elast.add_isotropic_material((0, 0), 68.3, 0.3)
mat_elast.add_isotropic_material((1, 1), 379.3, 0.1)


# In[ ]:


solver = "minres"
C = np.zeros((6, 6))
C[:, 0], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='x',  solver_type=solver, method='fe')
C[:, 1], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='y',  solver_type=solver, method='fe')
C[:, 2], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='z',  solver_type=solver, method='fe')
C[:, 3], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='yz', solver_type=solver, method='fe')
C[:, 4], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='xz', solver_type=solver, method='fe')
C[:, 5], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='xy', solver_type=solver, method='fe')


# In[ ]:


print(C)
coeffs = puma.experimental.get_E_nu_from_elasticity(C)


# #### Example: FiberForm elasticity
# 
# Now let's compute the elasticity of FiberForm

# In[ ]:


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


# In[ ]:


C = np.zeros((6, 6))
C[:, 0], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='x', solver_type='minres', method='fe')
u[ws.matrix == 0] = 0  # set air to zero, i.e. no displacement
puma.experimental.warp_elasticity_fields(ws, u, s, t, scale_factor=1, notebook=notebook)


# In[ ]:


C[:, 1], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='y', solver_type='minres', method='fe')
C[:, 2], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='z', solver_type='minres', method='fe')
C[:, 3], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='yz', solver_type='minres', method='fe')
C[:, 4], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='xz', solver_type='minres', method='fe')
C[:, 5], u, s, t = puma.experimental.compute_elasticity(ws, mat_elast, direction='xy', solver_type='minres', method='fe')


# In[ ]:


print(C)
coeffs = puma.experimental.get_E_nu_from_elasticity(C)


# In[ ]:




