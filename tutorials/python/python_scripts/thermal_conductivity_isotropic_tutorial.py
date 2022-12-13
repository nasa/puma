import numpy as np
import pumapy as puma
import pyvista as pv
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

# Import an example tomography file of size 200^3 and voxel length 1.3e-6
ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)

# Cropping the 200x200x200 image into a 100x100x100 sample
ws_fiberform.matrix = ws_fiberform.matrix[50:150, 50:150, 50:150]

# Generating a conductivity map. This stores the conductivity values for each phase of the material
cond_map = puma.IsotropicConductivityMap()
# First, we set the conductivity of the void phase to be 0.0257 (air at STP)
cond_map.add_material((0, 89), 0.0257)
# Next we set the conductivity of the solid phase to be 12 (typical value for carbon fiber)
cond_map.add_material((90, 255), 12)

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

k_eff_x, T_x, q_x = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 's', tolerance=1e-4, solver_type='cg', matrix_free=True, method='fv')
k_eff_y, T_y, q_y = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'y', 's', tolerance=1e-4, solver_type='cg', matrix_free=True, method='fv')
k_eff_z, T_z, q_z = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 's', tolerance=1e-4, solver_type='cg', matrix_free=True, method='fv')

# Visualize both the temperature field and the fibers
p = pv.Plotter(notebook=notebook)
p.add_text("Fibers with Temperature field for x simulation")
puma.render_contour(ws_fiberform, cutoff=(90, 255), notebook=notebook, add_to_plot=p, plot_directly=False)
puma.render_volume(T_x[:, :2*T_x.shape[1]//3], solid_color=None, cmap='jet', notebook=notebook, add_to_plot=p, plot_directly=False)
p.show()

# Visualizing the flux magnitude: 
puma.render_volume(np.linalg.norm(q_x, axis=3), notebook=notebook, cmap='jet')

# Below is an example of a thermal conductivity simulation, but now performed on a segmented image in order to show an
# alternative option.

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

# Visualizing the temperature field: 
puma.render_volume(T_y, solid_color=None, notebook=notebook, cmap='jet')


# Visualizing the flux magnitude: 
puma.render_volume(np.linalg.norm(q_y, axis=3), notebook=notebook, cmap='jet')
