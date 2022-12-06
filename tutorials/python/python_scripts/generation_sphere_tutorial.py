import numpy as np
import pumapy as puma
import os

notebook = False  # when running locally, actually open pyvista window
export_path = "out"  # CHANGE THIS PATH
if not os.path.exists(export_path):
    os.makedirs(export_path)

# ## Tutorial: Generating Random Sphere Structures
# In this tutorial we demonstrate how to generate a microstructure of random intersecting spheres
# We generate the sphere structure and store it into the PuMA workspace class. 

size = (400, 400, 400)  # size of the domain, in voxels. 
diameter = 20  # diameter of the spheres to be generated, in voxels
porosity = 0.8  # porosity of the overall structure
allow_intersections = True # flag on whether to allow intersections between spheres. 
segmented = True  # assign unique IDs to each sphere (if set to False, range will be from 0-255)
# Note: If allow_intersections is set to false, it will be significantly slower to generate,
#       and will usually require a fairly high porosity value to be generated

ws_spheres = puma.generate_random_spheres(size, diameter, porosity, allow_intersections, segmented=segmented)

# The material is now stored in the puma workspace, ws_spheres. By default, the grayscale range of [0,127] corresponds to
# the void space, and [128,255] contains the material.
# 
# Next we will visualize the domain:

# visualizing the solid domain, contained in [128,255] grayscale range. 
puma.render_volume(ws_spheres, cutoff=(1, ws_spheres.max()), cmap='jet', notebook=notebook)
