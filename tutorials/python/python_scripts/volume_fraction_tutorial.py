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
# In this tutorial we demonstrate how to calculate the volume fraction of individual phases of a microstructure

# We import or generate the microstructure

ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)

# The volume fraction does not require the voxel length to set, however it is still good practice to set it regardless
# The voxel length can be set during import, as shown above. The voxel length can also be set for the workspace
# directly, as shown below:

ws_fiberform.voxel_length = 1.3e-6  # setting the voxel length in meters

#  In a segmented domain, each phase is defined by a single grayscale value (i.e. 0 for void and 1 for solid). In an
#  unsegmented domain, the material phases are specified by a range of grayscale value. In the case of the imported
#  tomography, the void range is  [0,89] and the solid range is [90,255], as shown by the plotted slice below:

puma.plot_slices(ws_fiberform)

cutoff_solid = (90, 255)  # specifying the grayscale range of the phase of interest
cutoff_void = (0, 89)  # specifying the grayscale range of the phase of interest

# To calculate the volume fractions of each material phase, we will use the puma.compute_volume_fraction function and
# specify the grayscale range of each material:

vf_void = puma.compute_volume_fraction(ws_fiberform, cutoff_void)
vf_solid = puma.compute_volume_fraction(ws_fiberform, cutoff_solid)

print("Volume Fraction of Void Phase (Same as porosity): ", vf_void)
print("Volume Fraction of the Sold Phase: ", vf_solid)


