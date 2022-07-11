import numpy as np
import pumapy as puma
import pyvista as pv


# ## Tutorial: Thermal Conductivity
# In this tutorial we demonstrate how to compute the effective thermal conductivity of a material based on its
# microstructure and constituent properties. In this example, we compute the thermal conductivity of FiberForm,
# a carbon fiber based material.
#
# Note: The sample size used in this example is very small, well below the size needed for a representative volume of
# the sample.

# ### Isotropic conductivity

# We will show a thermal conductivity simulation based on a non-segmented representation of the material. In the
# example material used, the void phase is contained in grayscale range [0,89] and the solid phase is contained in
# the grayscale range of [90,255]. This range varies for each tomography sample.
#
# The outputs of the thermal conductivity simulation are the effective thermal conductivity tensor, the steady state
# temperature profile, and the steady state heat flux.

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

# Simulation inputs:
#.  1. workspace - the computational domain for the simulation, containing your material microstructure
#.  2. cond_map - the conductivity values for each material phase
#.  3. direction - the simulation direction, 'x', 'y', or 'z'
#.  4. side_bc - boundary condition in the non-simulation direction. Can be 'p' - periodic, 's' - symmetric, 'd' - dirichlet
#.  5. tolerance - accuracy of the numerical solver, defaults to 1e-4.
#.  6. maxiter - maximum number of iterations, defaults to 10,000
#.  7. solver_type - the iterative solver used. Can be 'bicgstab', 'cg', 'gmres', or 'direct'. Defaults to 'bicgstab'

k_eff_x, T_x, q_x = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 's', tolerance=1e-3, solver_type='cg')
k_eff_y, T_y, q_y = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'y', 's', tolerance=1e-3, solver_type='cg')
k_eff_z, T_z, q_z = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 's', tolerance=1e-3, solver_type='cg')

print("Effective thermal conductivity tensor:")
print("[", k_eff_x[0], k_eff_y[0], k_eff_z[0], "]")
print("[", k_eff_x[1], k_eff_y[1], k_eff_z[1], "]")
print("[", k_eff_x[2], k_eff_y[2], k_eff_z[2], "]")

# Visualize both the temperature field and the fibers
p = pv.Plotter(notebook=False)
p.add_text("Fibers with Temperature field for x simulation")
puma.render_contour(ws_fiberform, cutoff=(90, 255), notebook=False, add_to_plot=p, plot_directly=False)
puma.render_volume(T_x[:, :2*T_x.shape[1]//3], solid_color=None, cmap='jet', notebook=False, add_to_plot=p, plot_directly=False)
p.show()

# Visualizing the flux magnitude:
puma.render_volume(np.linalg.norm(q_x, axis=3), notebook=False, cmap='jet')

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

# Simulation inputs:
#.  1. workspace - the computational domain for the simulation, containing your material microstructure
#.  2. cond_map - the conductivity values for each material phase
#.  3. direction - the simulation direction, 'x', 'y', or 'z'
#.  4. side_bc - boundary condition in the non-simulation direction. Can be 'p' - periodic, 's' - symmetric, 'd' - dirichlet
#.  5. tolerance (optional) - accuracy of the numerical solver, defaults to 1e-4.
#.  6. maxiter (optional) - maximum number of iterations, defaults to 10,000
#.  7. solver_type (optional) - the iterative solver used. Can be 'bicgstab', 'cg', 'gmres', or 'direct'. Defaults to 'bicgstab'

k_eff_x, T_x, q_x = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 's', tolerance=1e-3, solver_type='cg')
k_eff_y, T_y, q_y = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'y', 's', tolerance=1e-3, solver_type='cg')
k_eff_z, T_z, q_z = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 's', tolerance=1e-3, solver_type='cg')

print("Effective thermal conductivity tensor:")
print("[", k_eff_x[0], k_eff_y[0], k_eff_z[0], "]")
print("[", k_eff_x[1], k_eff_y[1], k_eff_z[1], "]")
print("[", k_eff_x[2], k_eff_y[2], k_eff_z[2], "]")

# Visualizing the temperature field:
puma.render_volume(T_y, solid_color=None, notebook=False, cmap='jet')

# Visualizing the flux magnitude:
puma.render_volume(np.linalg.norm(q_y, axis=3), notebook=False, cmap='jet')
