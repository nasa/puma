import numpy as np
import pumapy as puma


# ## Tutorial: Generating Random Fiber Structures
# In this tutorial we demonstrate how to generate a microstructure of random intersecting fibers
# We generate the fiber structure and store it into the PuMA workspace class. 

size = (200, 200, 200)  # size of the domain, in voxels. 
radius = 8  # radius of the fibers to be generated, in voxels
nFibers = None  # Can specify either the number of fibers or the porosity
porosity = 0.8  # porosity of the overall structure
phi = 90    # A value between 0 and 90 that controls the amount that the fibers lie *out of* the XY plane,
            # with 0 meaning all fibers lie in the XY plane, and 90 meaning that cylinders are randomly oriented out
			# of the plane by as much as +/- 90 degrees.
theta = 90  # A value between 0 and 90 that controls the amount of rotation *in the* XY plane,
            # with 0 meaning all fibers point in the X-direction, and 90 meaning they are randomly rotated about the
            # Z axis by as much as +/- 90 degrees.
length = 200 # Length of the fibers to be generated
max_iter = 3  # optional (default=3), iterations to refine the porosity
allow_intersect = True  # optional (default=True), allow intersection betweem the fibers: if equal to False, the
						# function runs considerably more slowly because randomly proposed fibers are rejected if
						# they intersect any other fiber - use with relatively high porosity for reasonable runtimes
segmented = True  # assign unique IDs to each fiber (if set to False, range will be from 0-255)

ws_fibers = puma.generate_random_fibers(size, radius, nFibers, porosity, phi, theta, length, allow_intersect=allow_intersect, segmented=segmented)

# The material is now stored in the puma workspace, ws_fibers. By default, the grayscale value of 0 is assigned to the
# void space and a grayscale value of 1 is assigned to the material.
# Next we will visualize the domain: 

# visualizing the solid domain
puma.render_volume(ws_fibers, cutoff=(1, ws_fibers.max()), solid_color=None, cmap='jet', notebook=False)

# visualizing the solid domain as a contour
puma.render_contour(ws_fibers, cutoff=(1, ws_fibers.max()), notebook=False)

# If you would like to visualize the individual slices, this can be done using the plot_slices function
puma.plot_slices(ws_fibers)

