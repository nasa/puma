import numpy as np
import pumapy as puma
import os

notebook = False  # when running locally, actually open pyvista window
export_path = "out"  # CHANGE THIS PATH
if not os.path.exists(export_path):
    os.makedirs(export_path)

# ## Tutorial: Generating Fiber Structures
# In this part of the tutorial we demonstrate how to generate a microstructure of random intersecting fibers

# There are three types of random fiber structures that can be generated: 
# 1. fully isotropic fibers
# 2. transverse isotropic fibers
# 3. 1D fiber structures
# 
# Each of these fiber structures has a separate function call. Below we will demonstrate the generation of each of these fiber types:

size = (200, 200, 200)  # size of the domain, in voxels. 
radius = 8  # radius of the fibers to be generated, in voxels
nFibers = None  # Can specify either the number of fibers or the porosity
porosity = 0.8  # porosity of the overall structure
length = 200 # Length of the fibers to be generated
max_iter = 3  # optional (default=3), iterations to refine the porosity
allow_intersect = True  # optional (default=True), allow intersection betweem the fibers: if equal to False, the function runs considerably more slowly because    
                        # randomly proposed fibers are rejected if they intersect any other fiber - use with relatively high porosity for reasonable runtimes 
segmented = True  # assign unique IDs to each fiber (if set to False, range will be from 0-255)

ws_fibers_isotropic = puma.generate_random_fibers_isotropic(size, radius, nFibers, porosity, length, allow_intersect=allow_intersect, segmented=segmented)


direction = 'z' # Direction orthogonal to the fibers. For example, 'z' means fibers are oriented in the XY plane. 
variation = 0  # Variability for the angle, relative to the XY plane. Should be between 0 and 90.  

ws_fibers_trans_iso = puma.generate_random_fibers_transverseisotropic(size, radius, nFibers, porosity, direction, variation, length,
                                                                      allow_intersect=allow_intersect, segmented=segmented)

direction = 'z' # Direction of the 1D fibers. 
ws_fibers_1D = puma.generate_random_fibers_1D(size, radius, nFibers, porosity, direction, length, allow_intersect=allow_intersect, segmented=segmented)

# The material is now stored in the puma workspace, ws_fibers. By default, the grayscale value of 0 is assigned to the
# void space and a grayscale value of 1 is assigned to the material.
# 
# Next we will visualize the domain: 

# visualizing the solid domain
puma.render_volume(ws_fibers_isotropic, cutoff=(1, ws_fibers_isotropic.max()), solid_color=None, cmap='jet', notebook=notebook)
puma.render_volume(ws_fibers_trans_iso, cutoff=(1, ws_fibers_trans_iso.max()), solid_color=None, cmap='jet', notebook=notebook)
puma.render_volume(ws_fibers_1D, cutoff=(1, ws_fibers_1D.max()), solid_color=None, cmap='jet', notebook=notebook)

# If you would like to visualize the individual slices, this can be done using the plot_slices function

puma.plot_slices(ws_fibers_isotropic)
puma.plot_slices(ws_fibers_trans_iso)
puma.plot_slices(ws_fibers_1D)
