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
# 
# Although all of the required information for performing simulations on a multi-phase material are already present in the
# other tutorials, this tutorial is designed to help understand how to perform basic visualizations and simulations on a
# material with more than one solid phase.
# 
# For the example here, we are using an artifically generated material with three phases: two random fibrous phases, and
# randomly populated spheres

# ### Material Generation
# 
# First, we will generate a material with 2 different types of fibers, and one type of sphere. 
# 
# The two fibers will be stored with material ID 1, and 2, and the sphere will be stored with material ID 3. 
# 
# Specify the output directory for the files to be generated:

size = (200, 200, 200)  # size of the domain, in voxels. 
radius = 8  # radius of the fibers to be generated, in voxels
nFibers = None # Can specify either the number of fibers or the porosity
porosity = 0.9  # porosity of the overall structure
phi = 90 # A value between 0 and 90 that controls the amount that the fibers lie *out of* the XY plane,
         # with 0 meaning all fibers lie in the XY plane, and 90 meaning that cylinders are randomly oriented out of the
         # plane by as much as +/- 90 degrees.
theta = 90 # A value between 0 and 90 that controls the amount of rotation *in the* XY plane,
           # with 0 meaning all fibers point in the X-direction, and 90 meaning they are randomly rotated about the
           # Z axis by as much as +/- 90 degrees.
length = 200 # Length of the fibers to be generated

ws_fibers1 = puma.generate_random_fibers(size, radius, nFibers, porosity, phi, theta, length)
ws_fibers1.binarize((1))  # turn all the fibers into ID=1

radius = 5  # creating smaller fibers for material #2
length = 50

ws_fibers2 = puma.generate_random_fibers(size, radius, nFibers, porosity, phi, theta, length)
ws_fibers2.set_material_id((1, ws_fibers2.max()), 2)  # turn all the fibers into ID=2

# combining fibers #1 and fibers #2 into a single domain, and setting the overlap to default to fibers #1
ws_fibers1.matrix = ws_fibers1.matrix + ws_fibers2.matrix
ws_fibers1.set_material_id((3, 3), 1) # setting the overlap, which would be 3, equal to 1

# Generating the spheres
diameter = 20  # diameter of the spheres to be generated, in voxels
porosity = 0.8  # porosity of the overall structure
allow_intersections = True # flag on whether to allow intersections between spheres. 
# Note: If allow_intersections is set to false, it will be significantly slower to generate,
#.      and will usually require a fairly high porosity value to be generated

ws_spheres = puma.generate_random_spheres(size, diameter, porosity, allow_intersections, segmented=False)
ws_spheres.set_material_id((0, 127), 0)
ws_spheres.set_material_id((128, 255), 3)

# combining all three materials, with overlap defaulting to the sphere material
ws_fibers1.matrix = ws_fibers1.matrix + ws_spheres.matrix
ws_fibers1.set_material_id((4, 5), 3) # setting the overlap, which would be either 4 or 5, equal to 3

puma.export_3Dtiff(os.path.join(export_path, "multiphase.tif"), ws_fibers1)

ws_multiphase = ws_fibers1


# ### Material Visualization
# 
# Now we will plot a slice of the material and visualize each of the three phases:

slices = puma.plot_slices(ws_multiphase, index=100)

puma.render_contour(ws_multiphase, cutoff=(1, 1), notebook=notebook)
puma.render_contour(ws_multiphase, cutoff=(2, 2), notebook=notebook)
puma.render_contour(ws_multiphase, cutoff=(3, 3), notebook=notebook)

# We can also visualize the three-phase material using either the volume_render for showing a voxel representation or the
# puma.render_contour_multiphase function for a smooth triangulated surface representation:

puma.render_volume(ws_multiphase, cutoff=(1, 3), solid_color=None, notebook=notebook, cmap='gray')

cutoffs = [(1, 1)]  # material phase 1
cutoffs.append((2, 2)) # material phase 2
cutoffs.append((3, 3)) # material phase 3

# if solid_colors is not provided, the color of the phases is randomized
puma.render_contour_multiphase(ws_multiphase, cutoffs, notebook=notebook, 
                               solid_colors=((0., 0., 0.), (0.5, 0.5, 0.5), (1., 1., 1.)))

# ### Volume Fractions
# 
# To calculate the volume fractions of each material phase, we will use the puma.compute_volume_fraction function and
# specify the grayscale range of each material:

vf_void = puma.compute_volume_fraction(ws_multiphase, (0,0))
vf_phase1 = puma.compute_volume_fraction(ws_multiphase, (1,1))
vf_phase2 = puma.compute_volume_fraction(ws_multiphase, (2,2))
vf_phase3 = puma.compute_volume_fraction(ws_multiphase, (3,3))
vf_solid = puma.compute_volume_fraction(ws_multiphase, (1,3))

print("Volume Fraction of Void (Porosity):", vf_void)
print("Volume Fraction of Phase 1:", vf_phase1)
print("Volume Fraction of Phase 2:", vf_phase2)
print("Volume Fraction of Phase 3:", vf_phase3)
print("Volume Fraction of All Three Phases:", vf_solid)


# ### Surface Area
# 
# To calculate the total surface area of the entire material phase, we can use the puma.compute_surface_area function with
# the material cutoff of (1,3) which includes all 3 material phases

area, specific_area = puma.compute_surface_area(ws_multiphase, (1, 3))
print("Areas:", area, specific_area)


# Computing the surface area of each individual phase is a little bit more tricky. To demonstrate, refer to the simple 2D
# schematic below of a 2-phase material.
# 
# ![image info](https://github.com/nasa/puma/raw/main/tutorial/pictures/multiphase.png)
# 
# The materials are each labeled, 1, and 2, and the edge lengths are labeled a, b, and c. The total surface area of both
# materials is defined as A<sub>total</sub> = a + b. Assuming that your materials are stored with grayscale values 1 and 2,
# this total surface area is calculated as before in the 3-material example:
# 
#     puma.compute_surface_area(ws_multiphase, (1,2))
# 
# However, if you want to know the surface area of an individual phase, there are two options. You can compute the total
# surface area of the individual phase, including the surface area contact with other material phases, or you can compute
# only the exposed surface area to the void phase. The latter would be the relevant quantity when computing, for example,
# the effective reactive surface area for chemical reactions. In the first case, the surface area can be calculated as
# 
# Area = A<sub>1</sub> = a + c = puma.compute_surface_area(ws_multiphase, (1,2))
# 
# Area = 0.5 ( A<sub>1</sub> + A<sub>1-2</sub> - A<sub>2</sub> ) = 0.5 * (a + b + a + c - b - c) = a
# 
# which, written in puma commands, becomes:
# 
#     Area = puma.compute_surface_area(ws_multiphase, (1,1)) + puma.compute_surface_area(ws_multiphase, (1,2)) - puma.compute_surface_area(ws_multiphase, (2,2))
# 
# For our 3-phase material, it is quite similar:
# 
# To compute the surface area of material 1 exposed to the void, 
# 
# Area = 0.5 ( A<sub>1</sub> + A<sub>1-2-3</sub> - A<sub>2-3</sub> ) = 0.5 * (a + d + f + a + b + c - d - b - c - f) = a

# Below, we compute the exposed to void surface area of each of the three phases: note that computing the surface area of
# the union between 1 and 3 requires extra steps, since using the surface area calculation on the domain with cutoffs (1,3)
# would include phase 2. Instead we copy the domain, set material 3 to an ID of 1, and then compute the surface area of
# material 1, which now includes material 3

# Raw and specific surface area calculations
Area_1, SSA_1 = puma.compute_surface_area(ws_multiphase, (1,1))  # a + d + f
Area_2, SSA_2 = puma.compute_surface_area(ws_multiphase, (2,2))  # d + b + e
Area_3, SSA_3 = puma.compute_surface_area(ws_multiphase, (3,3))  # f + e + c

Area_12, SSA_12 = puma.compute_surface_area(ws_multiphase, (1,2))  # a + b + e + f
Area_23, SSA_23 = puma.compute_surface_area(ws_multiphase, (2,3))  # d + b + c + f
Area_123, SSA_123 = puma.compute_surface_area(ws_multiphase, (1,3)) # a 

ws_copy = ws_multiphase.copy()
ws_copy.set_material_id((3,3),1)  # setting all of phase 3 to be equal to ID 1
Area_13, SSA_13 = puma.compute_surface_area(ws_copy, (1,1))  # a + d + e + c

# Now to compute the exposed surface areas for each phase: 
Exposed_Area_1 = 0.5 * (Area_1 + Area_123 - Area_23)
Exposed_SSA_1 = 0.5 * (SSA_1 + SSA_123 - SSA_23)

Exposed_Area_2 = 0.5 * (Area_2 + Area_123 - Area_13)
Exposed_SSA_2 = 0.5 * (SSA_2 + SSA_123 - SSA_13)

Exposed_Area_3 = 0.5 * (Area_3 + Area_123 - Area_12)
Exposed_SSA_3 = 0.5 * (SSA_3 + SSA_123 - SSA_12)

print("Exposed Areas for Phase 1:", Exposed_Area_1, Exposed_SSA_1)
print("Exposed Areas for Phase 2:", Exposed_Area_2, Exposed_SSA_2)
print("Exposed Areas for Phase 3:", Exposed_Area_3, Exposed_SSA_3)

# As a check, we will test to make sure that the total exposed areas of each phase sum up to the total surface area of
# all of the material phases:

print("Sum of Exposed Phase Area:", Exposed_SSA_1 + Exposed_SSA_2 + Exposed_SSA_3)
print("Total Area:", SSA_123)
print("Percent Error: ", np.abs((Exposed_SSA_1 + Exposed_SSA_2 + Exposed_SSA_3 - SSA_123)) / SSA_123 * 100.)


# We can see that the two values are very close but not exactly the same. This slight difference is simply numerical error
# from the triangulations and accounts for a small error in this case, which should not be too significant.

# ### Tortuosity Factors
# 
# Computing the tortuosity factors for a multi-phase material is not different than for a single phase material, since
# it is only the void phase that is specified for the calculation.
# 
# To speed up the simulation, we will take a 100<sup>3</sup> subsection of the domain in order to perform the tortuosity
# simulation. Please note that this domain size is almost certainly not a representative volume, and a far larger size
# should be used when performing production simulations

# The tortuosity calculation needs to be run for each of the three simulation directions. 
# For each simulation, a concentration gradient is forced in the simulation direction, and converged to steady state

# Simulation inputs: 
#.  1. workspace - the computational domain for the simulation, containing your material microstructure
#.  2. cutoff - the grayscale values for the void phase. [0,0] for this sample
#.  3. direction - the simulation direction, 'x', 'y', or 'z'
#.  4. side_bc - boundary condition in the non-simulation direction. Can be 'p' - periodic, 's' - symmetric, 'd' - dirichlet
#.  5. tolerance - accuracy of the numerical solver, defaults to 1e-4. 
#.  6. maxiter - maximum number of iterations, defaults to 10,000
#.  7. solver_type - the iterative solver used. Can be 'bicgstab', 'cg', 'gmres', or 'direct'. Defaults to 'bicgstab'

ws_cropped = ws_multiphase.copy()  # creating a copy of the workspace to crop
ws_cropped.matrix = ws_cropped.matrix[50:150,50:150,50:150]  # cropping the sample to 100^3

n_eff_x, Deff_x, poro, C_x = puma.compute_continuum_tortuosity(ws_cropped, (0,0), 'x', side_bc='s', tolerance=1e-4, solver_type='cg')
n_eff_y, Deff_y, poro, C_y = puma.compute_continuum_tortuosity(ws_cropped, (0,0), 'y', side_bc='s', tolerance=1e-4, solver_type='cg')
n_eff_z, Deff_z, poro, C_z = puma.compute_continuum_tortuosity(ws_cropped, (0,0), 'z', side_bc='s', tolerance=1e-4, solver_type='cg')

print("\nEffective Tortuosity Factors")
print("[", n_eff_x[0], n_eff_y[0], n_eff_z[0], "]")
print("[", n_eff_x[1], n_eff_y[1], n_eff_z[1], "]")
print("[", n_eff_x[2], n_eff_y[2], n_eff_z[2], "]")

print("Porosity of the material:", poro)

# Visualizing the Concentration field for the simulation along the x-axis:  
puma.render_volume(C_x, solid_color=None, notebook=notebook, cmap='jet')


# ### Effective Thermal Conductivity
# 
# Computing the effective thermal conductivity is also very similar to in a single-phase case. The only difference is that
# rather than two materials being specified (void and solid) in the conductivity map, an entry must be made for each
# material phase, and the corresponding constituent thermal conductivity must be set.

# Generating a conductivity map. This stores the conductivity values for each phase of the material
cond_map = puma.IsotropicConductivityMap()
# First, we set the conductivity of the void phase to be 0.0257 (air at STP)
cond_map.add_material((0, 0), 0.0257)
# Next we set the conductivity of each of the three material phases
cond_map.add_material((1, 1), 10)
cond_map.add_material((2, 2), 20)
cond_map.add_material((3, 3), 100)

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

k_eff_x, T_x, q_x = puma.compute_thermal_conductivity(ws_cropped,cond_map, 'x', 's', tolerance=1e-4, solver_type='bicgstab')
k_eff_y, T_y, q_y = puma.compute_thermal_conductivity(ws_cropped,cond_map, 'y', 's', tolerance=1e-4, solver_type='bicgstab')
k_eff_z, T_z, q_z = puma.compute_thermal_conductivity(ws_cropped,cond_map, 'z', 's', tolerance=1e-4, solver_type='bicgstab')

print("\nEffective Thermal Conductivity")
print("[", k_eff_x[0], k_eff_y[0], k_eff_z[0], "]")
print("[", k_eff_x[1], k_eff_y[1], k_eff_z[1], "]")
print("[", k_eff_x[2], k_eff_y[2], k_eff_z[2], "]")


# Visualizing the temperature field for the simulation along the y-axis: 
puma.render_volume(T_y, solid_color=None, notebook=notebook, cmap='jet')
