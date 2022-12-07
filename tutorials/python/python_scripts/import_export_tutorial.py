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
# In this tutorial we demonstrate how to input and output a computational domain in PuMA

# ### Explanation of data structures
# First, we give a brief explanation of the PuMA data structures. For a more detailed description, refer to the tutorial
# on Workspace Manipulations.
# 
# Microstructures in PuMA are stored in a data structure called a "workspace". The workspace class includes all of the
# data about the material microstructure, including a 3D matrix of grayscale values, which can be either raw data from
# tomographic images, or segmented values.
# 
# Examples of unsegmented and segmented workspaces are demonstrated below: 

ws_unsegmented = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)
ws_segmented = ws_unsegmented.copy()
ws_segmented.binarize(90)

puma.compare_slices(ws_unsegmented, ws_segmented,'z', index=1)


# The PuMA workspace class also contains other important information about the workspace. This includes the voxel length,
# which is the physical length (in meters) of each voxel of the image. Typical tomography data ranges from 1e-4 to 1e-7
# meters in voxel length. If no value is provided to PuMA, the workspace class defaults to 1e-6 meters, i.e. one micron,
# as a voxel length. This value can either be set during input-output or it can be set directly for the workspace class.
# 
# An example is shown below, where the voxel length of a workspace class is manually changed

ws_unsegmented.voxel_length = 1.5e-6


# The workspace class can also store the material orientation for each voxel in the domain. This data can either be imported,
# or can be computed using "compute_orientation_st" function, which applies the structure-tensor method to estimate the
# material orientation.

# ### Workspace import and export
# Now we will give examples of input and output for the workspace class. 
# 
# PuMA import and export uses three different file types: 3D tiff, vti, and binary (extension is .pumapy)
# 
# The 3D tiff images only include the 3D matrix data for the grayscale values. vti and binary, however, includes all data
# associated with the puma workspace class, including the matrix data, the orientation data (if used) and the voxel length.
# 
# 3D tiff images are often the exported data format for tomography imaging, so they are included in PuMA. If you have a
# tomographic image that uses a different file format, it is recommended to use an external image processing software
# ([FIJI](https://imagej.net/software/fiji/) is recommended) and convert the image to a 3D tiff before importing into pumapy.
# If tomographic reconstruction also needs to be performed, the python package called
# [Tomopy](https://anaconda.org/conda-forge/tomopy) is recommended.
#
# ### Loading and exporting a .vti file

# Now, we will import an example file from the example data inside the folder pumapy.data:

ws_vtk = puma.import_vti(puma.path_to_example_file("fibers_with_orientation.vti"))

print("Voxel Length: ", ws_vtk.voxel_length)
print("Domain shape: ", ws_vtk.get_shape())

# We can now visualize both the fibrous microstructure and the fiber orientation side by side (for more visualization tips,
# follow the visualization tutorial):

p = pv.Plotter(shape=(1, 2), notebook=notebook)
p.subplot(0, 0)
p.add_text("Microstructure")
puma.render_contour(ws_vtk, (128, 255), notebook=notebook, add_to_plot=p, plot_directly=False)
p.subplot(0, 1)
p.add_text("Fiber orientation")
puma.render_orientation(ws_vtk, notebook=notebook, add_to_plot=p, plot_directly=False)
p.show()

# Finally, we will export it again to our specified directory:

puma.export_vti(os.path.join(export_path, "fibers_with_orientation.vti"), ws_vtk)

# And that's it for exporting to vti!  Let's repeat the same steps for .pumapy and 3D tiffs.

# ### Loading and exporting a .pumapy binary file

ws_binary = puma.import_bin(puma.path_to_example_file("fibers_with_orientation.pumapy"))

print("Voxel Length: ", ws_binary.voxel_length)
print("Domain shape: ", ws_binary.get_shape())

p = pv.Plotter(shape=(1, 2), notebook=notebook)
p.subplot(0, 0)
p.add_text("Microstructure")
puma.render_contour(ws_binary, (128, 255), notebook=notebook, add_to_plot=p, plot_directly=False)
p.subplot(0, 1)
p.add_text("Fiber orientation")
puma.render_orientation(ws_binary, notebook=notebook, add_to_plot=p, plot_directly=False)
p.show()

puma.export_bin(os.path.join(export_path, "fibers_with_orientation.vti"), ws_binary)


# ### Loading and exporting a .tif stack

ws_tiff = puma.import_3Dtiff(puma.path_to_example_file("50_artfibers.tif"))

print("Voxel Length: ", ws_tiff.voxel_length)
print("Domain shape: ", ws_vtk.get_shape())

puma.render_contour(ws_tiff, (128,255), notebook=notebook)

puma.export_3Dtiff(os.path.join(export_path, "50_artfibers.tif"), ws_tiff)


# As you can see, with the 3D tiff import, the voxel length of the original workspace and the orientation is not preserved.
# The voxel length can be set by either adding it as an option to the 3D tiff import call, or by setting it directly:

ws_tiff = puma.import_3Dtiff(puma.path_to_example_file("50_artfibers.tif"), 1.3e-6)
print("Voxel Length - passed to input function: ", ws_tiff.voxel_length)

ws_tiff = puma.import_3Dtiff(puma.path_to_example_file("50_artfibers.tif"))
print("Voxel Length - no input set: ", ws_tiff.voxel_length)
ws_tiff.voxel_length = 1.3e-6
print("Voxel Length - manually changed: ", ws_tiff.voxel_length)



