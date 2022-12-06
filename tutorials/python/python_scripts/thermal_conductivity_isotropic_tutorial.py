#!/usr/bin/env python
# coding: utf-8

# ![puma logo](https://github.com/nasa/puma/raw/main/doc/source/puma_logo.png)

# # Thermal Conductivity

# The objective of this notebook is to familiarize new users with the main datastructures that stand at the basis of the
# PuMA project, and outline the functions to compute material properties (please refer to these papers
# ([1](https://www.sciencedirect.com/science/article/pii/S2352711018300281),
# [2](https://www.sciencedirect.com/science/article/pii/S235271102100090X)) for more details on the software).

# ## Installation setup and imports

# The first code block will execute the necessary installation and package import. 
# 
# If you are running this jupyter notebook locally on your machine, assuming you have already installed the software,
# then the installation step will be skipped.

# In[ ]:


# for interactive slicer
get_ipython().run_line_magic('matplotlib', 'widget')
import numpy as np
import pumapy as puma
import pyvista as pv
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
# In this tutorial we demonstrate how to compute the effective thermal conductivity of a material based on its
# microstructure and constituent properties. In this example, we compute the thermal conductivity of FiberForm, a
# carbon fiber based material.
# 
# Note: The sample size used in this example is very small, well below the size needed for a representative volume of the sample. 

# ### Isotropic conductivity

# We will show a thermal conductivity simulation based on a non-segmented representation of the material. In the example
# material used, the void phase is contained in grayscale range [0,89] and the solid phase is contained in the grayscale
# range of [90,255]. This range varies for each tomography sample.
# 
# The outputs of the thermal conductivity simulation are the effective thermal conductivity tensor, the steady state
# temperature profile, and the steady state heat flux. Note that we are solving this problem using an iterative solver
# (Scipy's conjugate gradient) and a matrix-free approach, in which we don't statically allocate any large sparse matrix,
# but instead we compute the residual dynamically.

# In[ ]:


# Import an example tomography file of size 200^3 and voxel length 1.3e-6
ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)

# Cropping the 200x200x200 image into a 100x100x100 sample
ws_fiberform.matrix = ws_fiberform.matrix[50:150, 50:150, 50:150]

# Generating a conductivity map. This stores the conductivity values for each phase of the material
cond_map = puma.IsotropicConductivityMap()
# First, we set the conductivity of the void phase to be 0.0257 (air at STP)
cond_map.add_material((0, 89), 1)
# Next we set the conductivity of the solid phase to be 12 (typical value for carbon fiber)
cond_map.add_material((90, 255), 0)

# The thermal conductivity calculation needs to be run for each of the three simulation directions. 
# For each simulation, a temperature gradient is forced in the simulation direction, and converged to steady state

# Important simulation inputs: 
#.  1. workspace - the computational domain for the simulation, containing your material microstructure
#.  2. cond_map - the conductivity values for each material phase. To use the isotropic solver, it has to be of type: IsotropicConductivityMap
#.  3. direction - the simulation direction, 'x','y','z', or '' for prescribed_bc
#.  4. side_bc - boundary condition in the non-simulation direction. Can be 'p' - periodic, 's' - symmetric
#.  5. tolerance - accuracy of the numerical solver, defaults to 1e-5. 
#.  6. maxiter - maximum number of iterations, defaults to 10,000
#.  7. solver_type - the iterative solver used. Can be 'bicgstab', 'cg', 'gmres', or 'direct'. Defaults to 'bicgstab'
#.  8. matrix_free - whether to use the slower but lighter matrix-free approach (only available for the isotropic solver when solver_type!="direct"), or instantiating the big sparse A matrix
#.  9. method - whether to use finite volume ("fv") or finite element ("fe") solver
#.  10. prescribed_bc - has to be of type IsotropicConductivityBC, and it provides a more flexible way of specifying dirichlet BC, which can be imposed on any voxel of the domain

k_eff_x, T_x, q_x = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 'p', tolerance=1e-3, solver_type='cg', matrix_free=True, method='fv')
k_eff_y, T_y, q_y = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'y', 'p', tolerance=1e-3, solver_type='cg', matrix_free=True, method='fv')
k_eff_z, T_z, q_z = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 'p', tolerance=1e-3, solver_type='cg', matrix_free=True, method='fv')

print("Effective thermal conductivity tensor:")
print("[", k_eff_x[0], k_eff_y[0], k_eff_z[0], "]")
print("[", k_eff_x[1], k_eff_y[1], k_eff_z[1], "]")
print("[", k_eff_x[2], k_eff_y[2], k_eff_z[2], "]")


# In[ ]:


# Visualize both the temperature field and the fibers
p = pv.Plotter(notebook=notebook)
p.add_text("Fibers with Temperature field for x simulation")
puma.render_contour(ws_fiberform, cutoff=(90, 255), notebook=notebook, add_to_plot=p, plot_directly=False)
puma.render_volume(T_x[:, :2*T_x.shape[1]//3], solid_color=None, cmap='jet', notebook=notebook, add_to_plot=p, plot_directly=False)
p.show()


# In[ ]:


# Visualizing the flux magnitude: 
puma.render_volume(np.linalg.norm(q_x, axis=3), notebook=notebook, cmap='jet')


# Below is an example of a thermal conductivity simulation, but now performed on a segmented image in order to show an
# alternative option.

# In[ ]:


# Segments the image. All values >= 90 are set to 1, and all values <90 are set to 0.
ws_fiberform.binarize(90)

# Generating a conductivity map. This stores the conductivity values for each phase of the material
cond_map = puma.IsotropicConductivityMap()
# First, we set the conductivity of the void phase to be 0.0257 (air at STP)
cond_map.add_material((0, 0), 0.0257)
# Next we set the conductivity of the solid phase to be 12 (typical value for carbon fiber)
cond_map.add_material((1, 1), 12)
# Note that the grayscale ranges have changed relative to the last example, since this image is already segmented

# The thermal conductivity calculation needs to be run for each of the three simulation directions. 
# For each simulation, a temperature gradient is forced in the simulation direction, and converged to steady state
k_eff_x, T_x, q_x = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 'p', tolerance=1e-3, solver_type='cg')
k_eff_y, T_y, q_y = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'y', 'p', tolerance=1e-3, solver_type='cg')
k_eff_z, T_z, q_z = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 'p', tolerance=1e-3, solver_type='cg')

print("Effective thermal conductivity tensor:")
print("[", k_eff_x[0], k_eff_y[0], k_eff_z[0], "]")
print("[", k_eff_x[1], k_eff_y[1], k_eff_z[1], "]")
print("[", k_eff_x[2], k_eff_y[2], k_eff_z[2], "]")


# In[ ]:


# Visualizing the temperature field: 
puma.render_volume(T_y, solid_color=None, notebook=notebook, cmap='jet')


# In[ ]:


# Visualizing the flux magnitude: 
puma.render_volume(np.linalg.norm(q_y, axis=3), notebook=notebook, cmap='jet')


# ### Anisotropic conductivity
# Next we show how to compute the conductivity if the constituent phases are anisotropic themselves. This solver is
# significantly slower because of the higher complexity of the numerical scheme used, namely the Multi-Point Flux
# Approximation (MPFA) (please refer to [this paper](https://www.sciencedirect.com/science/article/abs/pii/S092702562030447X)
# for more details on the anisotropic conductivity solver in PuMA). For this reason, we scale the domain by half in order
# to keep the runtime reasonable.

# In[ ]:


# Import an example tomography file of size 100^3 and voxel length 1.3e-6
ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
ws_fiberform.rescale(0.5, False)

# detect the fiber orientation using the Structure Tensor
puma.compute_orientation_st(ws_fiberform, cutoff=(90, 255), sigma=1.4, rho=0.7)


# In[ ]:


# visualize the detected orientation field
puma.render_orientation(ws_fiberform, notebook=notebook)


# In[ ]:


# Generating a conductivity map. This stores the conductivity values for each phase of the material
cond_map = puma.AnisotropicConductivityMap()
# First, we set the conductivity of the void phase to be 0.0257 (air at STP)
cond_map.add_isotropic_material((0, 89), 0.0257)
# Next we set the conductivity of the fiber phase to be 12 along the fiber and 0.7 across it
cond_map.add_material_to_orient((90, 255), 12., 0.7)

# Simulation inputs: 
#.  1. workspace - the computational domain for the simulation, containing your material microstructure
#.  2. cond_map - the conductivity values for each material phase. To use the isotropic solver, it has to be of type: AnisotropicConductivityMap
#.  3. direction - the simulation direction, 'x','y','z', or '' for prescribed_bc
#.  4. side_bc - boundary condition in the non-simulation direction. Can be 'p' - periodic, 's' - symmetric, 'd' - dirichlet
#.  5. tolerance (optional) - accuracy of the numerical solver, defaults to 1e-4. 
#.  6. maxiter (optional) - maximum number of iterations, defaults to 10,000
#.  7. solver_type (optional) - the iterative solver used. Can be 'bicgstab', 'cg', 'gmres', or 'direct'. Defaults to 'bicgstab'
#.  8. method - whether to use finite volume ("fv") or finite element ("fe") solver
#.  9. prescribed_bc - has to be of type AnisotropicConductivityBC, and it provides a more flexible way of specifying dirichlet BC, which can be imposed on any voxel of the domain

# When an anisotropic conductivity map is fed, the solver automatically uses the MPFA finite volume method
k_eff_x, T_x, q_x = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 'p', tolerance=1e-3, solver_type='bicgstab', method='fv')
k_eff_y, T_y, q_y = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'y', 'p', tolerance=1e-3, solver_type='bicgstab', method='fv')
k_eff_z, T_z, q_z = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 'p', tolerance=1e-3, solver_type='bicgstab', method='fv')

print("Effective thermal conductivity tensor:")
print("[", k_eff_x[0], k_eff_y[0], k_eff_z[0], "]")
print("[", k_eff_x[1], k_eff_y[1], k_eff_z[1], "]")
print("[", k_eff_x[2], k_eff_y[2], k_eff_z[2], "]")


# If the local phases are isotropic, the anisotropic solver can still be used (although it would not be convenient
# because it is slower). As proof that the two solvers are actually giving the same answer, we could run the following case,
# in which we compute the orientation and then set the same conductivity to both the conductivity components (i.e. along
# and across a fiber):

# In[ ]:


ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
ws_fiberform.rescale(0.5, segmented=False)

cond_map = puma.IsotropicConductivityMap()
cond_map.add_material((0, 89), 0.0257)
cond_map.add_material((90, 255), 12)

print("\nIsotropic solver")
k_eff_x, T_x, q_x = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 'p', tolerance=1e-3)
k_eff_y, T_y, q_y = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'y', 'p', tolerance=1e-3)
k_eff_z, T_z, q_z = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 'p', tolerance=1e-3)

puma.compute_orientation_st(ws_fiberform, cutoff=(90, 255), sigma=1.4, rho=0.7)

cond_map = puma.AnisotropicConductivityMap()
cond_map.add_isotropic_material((0, 89), 0.0257)
cond_map.add_material_to_orient((90, 255), 12., 12)

print("\nAnisotropic solver")
k_eff_x_ani, T_x_ani, q_x_ani = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 'p', tolerance=1e-3)
k_eff_y_ani, T_y_ani, q_y_ani = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'y', 'p', tolerance=1e-3)
k_eff_z_ani, T_z_ani, q_z_ani = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 'p', tolerance=1e-3)

print("\nEffective conductivity using isotropic solver")
print("[", k_eff_x[0], k_eff_y[0], k_eff_z[0], "]")
print("[", k_eff_x[1], k_eff_y[1], k_eff_z[1], "]")
print("[", k_eff_x[2], k_eff_y[2], k_eff_z[2], "]")

print("\nEffective conductivity using anisotropic solver")
print("[", k_eff_x_ani[0], k_eff_y_ani[0], k_eff_z_ani[0], "]")
print("[", k_eff_x_ani[1], k_eff_y_ani[1], k_eff_z_ani[1], "]")
print("[", k_eff_x_ani[2], k_eff_y_ani[2], k_eff_z_ani[2], "]")


# As you can see, the tensors that have been estimated are very similar. The slight differences are coming from the high
# tolerance that was

# ### Finite Element Conductivity
# 
# An extra method to compute the thermal conductivity was recently added, which leverages a Finite Element solver based
# on a Q1-Q1 Element-by-element implementation. To learn more about this method, please refer to the permeability tutorial.
# In order to use it, a user can simply run similar options as for the anisotropic case above, adding a flag specifying
# that the method to be used is 'fe'. Some details on the solver:
# 
# - Only periodic boundary conditions are currently available, so the side_bc flag is ignored
# - A matrix-free solution approach (slightly slower but very memory efficient) can be run when no local orientation is
# present, i.e. when the local phases are isotropic
# - The ligher and faster 'minres' iterative solver can be used since the matrices that are assembled are symmetric
# - The simulation imposes a residual flux on the whole domain, rather than a unitary temperature gradient on the boundaries

# In[ ]:


ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
# ws_fiberform.rescale(0.5, segmented=False)

puma.compute_orientation_st(ws_fiberform, cutoff=(90, 255), sigma=1.4, rho=0.7)

cond_map = puma.AnisotropicConductivityMap()
cond_map.add_isotropic_material((0, 89), 0.0257)
cond_map.add_material_to_orient((90, 255), 12., 12)

print("\nAnisotropic solver")
k_eff_x_ani_fe, T_x_ani, q_x_ani = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', tolerance=1e-7, solver_type='minres', method='fe')
k_eff_y_ani_fe, T_y_ani, q_y_ani = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'y', tolerance=1e-7, solver_type='minres', method='fe')
k_eff_z_ani_fe, T_z_ani, q_z_ani = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', tolerance=1e-7, solver_type='minres', method='fe')

print("\nEffective conductivity using anisotropic solver")
print("[", k_eff_x_ani_fe[0], k_eff_y_ani_fe[0], k_eff_z_ani_fe[0], "]")
print("[", k_eff_x_ani_fe[1], k_eff_y_ani_fe[1], k_eff_z_ani_fe[1], "]")
print("[", k_eff_x_ani_fe[2], k_eff_y_ani_fe[2], k_eff_z_ani_fe[2], "]")


# In[ ]:




