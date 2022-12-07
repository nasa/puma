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
# In this tutorial we demonstrate how to compute the continuum tortuosity factors of a material based on its microstructure
# and constituent properties. In this example, we compute the continuum tortuosity of FiberForm, a carbon fiber based material.
# 
# Note: the rarified tortuosity factors are not available in pumapy, but are available in the PuMA C++ library.
# 
# Note: the sample size used in this example is very small, well below the size needed for a representative volume of the sample. 

# We will show a continuum tortuosity simulation based on a non-segmented representation of the material. In the example
# material used, the void phase is contained in grayscale range [0,89] and the solid phase is contained in the grayscale
# range of [90,255]. This range varies for each tomography sample.
# 
# The outputs of the continuum tortuosity simulation are the continuum tortuosity factors, the effective diffusivity,
# the porosity, and the steady state concentration profile


# Import an example tomography file of size 200^3 and voxel length 1.3e-6
ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)

# The tortuosity calculation needs to be run for each of the three simulation directions. 
# For each simulation, a concentration gradient is forced in the simulation direction, and converged to steady state

# Simulation inputs: 
#.  1. workspace - the computational domain for the simulation, containing your material microstructure
#.  2. cutoff - the grayscale values for the void phase. [0,89] for this tomography sample
#.  3. direction - the simulation direction, 'x', 'y', or 'z'
#.  4. side_bc - boundary condition in the non-simulation direction. Can be 'p' - periodic, 's' - symmetric, 'd' - dirichlet
#.  5. tolerance - accuracy of the numerical solver, defaults to 1e-4. 
#.  6. maxiter - maximum number of iterations, defaults to 10,000
#.  7. solver_type - the iterative solver used. Can be 'bicgstab', 'cg', 'gmres', or 'direct'. Defaults to 'bicgstab'

n_eff_x, Deff_x, poro, C_x = puma.compute_continuum_tortuosity(ws_fiberform, (0,89), 'x', side_bc='s', tolerance=1e-3, solver_type='cg')
n_eff_y, Deff_y, poro, C_y = puma.compute_continuum_tortuosity(ws_fiberform, (0,89), 'y', side_bc='s', tolerance=1e-3, solver_type='cg')
n_eff_z, Deff_z, poro, C_z = puma.compute_continuum_tortuosity(ws_fiberform, (0,89), 'z', side_bc='s', tolerance=1e-3, solver_type='cg')

print("\nEffective tortuosity factors:")
print("[", n_eff_x[0], n_eff_y[0], n_eff_z[0], "]")
print("[", n_eff_x[1], n_eff_y[1], n_eff_z[1], "]")
print("[", n_eff_x[2], n_eff_y[2], n_eff_z[2], "]")

print("Porosity of the material:", poro)

# Visualizing the Concentration field: 
puma.render_volume(C_x, notebook=notebook, cmap='jet')

# Below is an example of the exact same continuum tortuosity simulation, but now performed on a segmented image.
# If done correctly, both should produce identical results.

# Segments the image. All values >= 90 are set to 1, and all values <90 are set to 0
ws_fiberform.binarize(90)

# Simulation inputs: 
#.  1. workspace - the computational domain for the simulation, containing your material microstructure
#.  2. cutoff - the grayscale values for the void phase. [0,89] for this tomography sample
#.  3. direction - the simulation direction, 'x', 'y', or 'z'
#.  4. side_bc - boundary condition in the non-simulation direction. Can be 'p' - periodic, 's' - symmetric, 'd' - dirichlet
#.  5. tolerance - accuracy of the numerical solver, defaults to 1e-4. 
#.  6. maxiter - maximum number of iterations, defaults to 10,000
#.  7. solver_type - the iterative solver used. Can be 'bicgstab', 'cg', 'gmres', or 'direct'. Defaults to 'bicgstab'

n_eff_x, Deff_x, poro, C_x = puma.compute_continuum_tortuosity(ws_fiberform, (0,0), 'x', side_bc='s', tolerance=1e-3, solver_type='cg')
n_eff_y, Deff_y, poro, C_y = puma.compute_continuum_tortuosity(ws_fiberform, (0,0), 'y', side_bc='s', tolerance=1e-3, solver_type='cg')
n_eff_z, Deff_z, poro, C_z = puma.compute_continuum_tortuosity(ws_fiberform, (0,0), 'z', side_bc='s', tolerance=1e-3, solver_type='cg')

print("\nEffective tortuosity factors:")
print("[", n_eff_x[0], n_eff_y[0], n_eff_z[0], "]")
print("[", n_eff_x[1], n_eff_y[1], n_eff_z[1], "]")
print("[", n_eff_x[2], n_eff_y[2], n_eff_z[2], "]")

print("Porosity of the material:", poro)

# Visualizing the Concentration field: 
puma.render_volume(C_x, notebook=notebook, cmap='jet')

