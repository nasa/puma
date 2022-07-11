import numpy as np
import pumapy as puma

# ## Tutorial: Thermal Conductivity
# In this tutorial we demonstrate how to compute the effective thermal conductivity of a material based on its
# microstructure and constituent properties. In this example, we compute the thermal conductivity of FiberForm,
# a carbon fiber based material.
#
# Note: The sample size used in this example is very small, well below the size needed for a representative volume of
# the sample.


# ### Anisotropic conductivity
# Next we show how to compute the conductivity if the constituent phases are anisotropic themselves. This solver is
# significantly slower because of the higher complexity of the numerical scheme used, namely the Multi-Point Flux
# Approximation (MPFA) (please refer to [this paper](https://www.sciencedirect.com/science/article/abs/pii/S092702562030447X)
# for more details on the anisotropic conductivity solver in PuMA). For this reason, we scale the domain by half in
# order to keep the runtime reasonable.

# Import an example tomography file of size 100^3 and voxel length 1.3e-6
ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
ws_fiberform.rescale(0.5, False)

# detect the fiber orientation using the Structure Tensor
puma.compute_orientation_st(ws_fiberform, cutoff=(90, 255), sigma=1.4, rho=0.7)

# visualize the detected orientation field
puma.render_orientation(ws_fiberform, notebook=False)

# Generating a conductivity map. This stores the conductivity values for each phase of the material
cond_map = puma.AnisotropicConductivityMap()
# First, we set the conductivity of the void phase to be 0.0257 (air at STP)
cond_map.add_isotropic_material((0, 89), 0.0257)
# Next we set the conductivity of the fiber phase to be 12 along the fiber and 0.7 across it
cond_map.add_material_to_orient((90, 255), 12., 0.7)

# Simulation inputs:
#.  1. workspace - the computational domain for the simulation, containing your material microstructure
#.  2. cond_map - the conductivity values for each material phase
#.  3. direction - the simulation direction, 'x', 'y', or 'z'
#.  4. side_bc - boundary condition in the non-simulation direction. Can be 'p' - periodic, 's' - symmetric, 'd' - dirichlet
#.  5. tolerance (optional) - accuracy of the numerical solver, defaults to 1e-4.
#.  6. maxiter (optional) - maximum number of iterations, defaults to 10,000
#.  7. solver_type (optional) - the iterative solver used. Can be 'bicgstab', 'cg', 'gmres', or 'direct'. Defaults to 'bicgstab'

# When an anisotropic conductivity is fed, the solver automatically uses the MPFA finite volume method
k_eff_x, T_x, q_x = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 's')
k_eff_y, T_y, q_y = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'y', 's')
k_eff_z, T_z, q_z = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 's')

print("Effective thermal conductivity tensor:")
print("[", k_eff_x[0], k_eff_y[0], k_eff_z[0], "]")
print("[", k_eff_x[1], k_eff_y[1], k_eff_z[1], "]")
print("[", k_eff_x[2], k_eff_y[2], k_eff_z[2], "]")


# If the local phases are isotropic, the anisotropic solver can still be used (although it would not be convenient
# because it is slower). As proof that the two solvers are actually giving the same answer, we could run the following
# case, in which we compute the orientation and then set the same conductivity to both the conductivity components
# (i.e. along and across a fiber):

ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
ws_fiberform.rescale(0.5, segmented=False)

cond_map = puma.IsotropicConductivityMap()
cond_map.add_material((0, 89), 0.0257)
cond_map.add_material((90, 255), 12)

print("\nIsotropic solver")
k_eff_x, T_x, q_x = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 's', tolerance=1e-3)
k_eff_y, T_y, q_y = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'y', 's', tolerance=1e-3)
k_eff_z, T_z, q_z = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 's', tolerance=1e-3)

puma.compute_orientation_st(ws_fiberform, cutoff=(90, 255), sigma=1.4, rho=0.7)

cond_map = puma.AnisotropicConductivityMap()
cond_map.add_isotropic_material((0, 89), 0.0257)
cond_map.add_material_to_orient((90, 255), 12., 12)

print("\nAnisotropic solver")
k_eff_x_ani, T_x_ani, q_x_ani = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'x', 's', tolerance=1e-3)
k_eff_y_ani, T_y_ani, q_y_ani = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'y', 's', tolerance=1e-3)
k_eff_z_ani, T_z_ani, q_z_ani = puma.compute_thermal_conductivity(ws_fiberform, cond_map, 'z', 's', tolerance=1e-3)

print("\nEffective conductivity using isotropic solver")
print("[", k_eff_x[0], k_eff_y[0], k_eff_z[0], "]")
print("[", k_eff_x[1], k_eff_y[1], k_eff_z[1], "]")
print("[", k_eff_x[2], k_eff_y[2], k_eff_z[2], "]")

print("\nEffective conductivity using anisotropic solver")
print("[", k_eff_x_ani[0], k_eff_y_ani[0], k_eff_z_ani[0], "]")
print("[", k_eff_x_ani[1], k_eff_y_ani[1], k_eff_z_ani[1], "]")
print("[", k_eff_x_ani[2], k_eff_y_ani[2], k_eff_z_ani[2], "]")


# As you can see, the tensors that have been estimated are very similar. The slight differences are coming from the
# high tolerance that was
