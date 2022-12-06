#!/usr/bin/env python
# coding: utf-8

# ![puma logo](https://github.com/nasa/puma/raw/main/doc/source/puma_logo.png)

# # Workspace Manipulations

# The objective of this notebook is to familiarize new users with the main datastructures that stand at the basis of the
# PuMA project, and outline the functions to compute material properties (please refer to these papers
# ([1](https://www.sciencedirect.com/science/article/pii/S2352711018300281),
# [2](https://www.sciencedirect.com/science/article/pii/S235271102100090X)) for more details on the software).

# ## Installation setup and imports

# The first code block will execute the necessary installation and package import. 
# 
# If you are running this jupyter notebook locally on your machine, assuming you have already installed the software,
# then the installation step will be skipped

# In[ ]:


# for interactive slicer
get_ipython().run_line_magic('matplotlib', 'widget')
import numpy as np
import pumapy as puma
import os

if 'BINDER_SERVICE_HOST' in os.environ:  # ONLINE JUPYTER ON BINDER
    from pyvirtualdisplay import Display
    display = Display(visible=0, size=(600, 400))
    display.start()  # necessary for pyvista interactive plots
    notebook = True

else:  # LOCAL JUPYTER NOTEBOOK
    notebook = False  # when running locally, actually open pyvista window


# ## Tutorial
# 
# In this tutorial we demonstrate how to create a workspace and perform basic operations on it, including cropping, rotation, thresholding.

# A workspace is the datastructure at the basis of both PuMA and pumapy and it is basically a container for the material
# sample that is being analyzed. A workspace is made of little cubes, or voxels (i.e. 3D pixels), holding a value.
# This simple element definition (formally called Cartesian grid) allows for very fast operations.
# Inside a workspace object, two different arrays are defined: one called "matrix" and the other called "orientation".
# Both of these are nothing but a 3D Numpy array for the matrix (X,Y,Z dimensions of the domain) and a 4D Numpy array
# for the orientation (dimensions of X,Y,Z,3 for vectors throughout the domain).
# 
# Next we show the different ways we have implemented to define a workspace class. You can check how to use the methods
# by running the following commands:

# In[ ]:


help(puma.Workspace)  # all class methods


# In[ ]:


help(puma.Workspace.rescale)  # specific class method


# In[ ]:


# defines a workspace full of zeros of shape 10x11x12
ws = puma.Workspace.from_shape((10, 11, 12))
print(f"Shape of workspace: {ws.matrix.shape}")
print(f"Unique values in matrix: {ws.unique_values()}")


# In[ ]:


# defines a workspace of shape 20x31x212, full of a custom value (in this case ones)
ws = puma.Workspace.from_shape_value((20, 31, 212), 1)
print(f"Shape of workspace: {ws.matrix.shape}")
print(f"Unique values in matrix: {ws.unique_values()}")


# In[ ]:


# defines a workspace of shape 5x6x2, full of a custom value (in this case ones) for the matrix array
# and vectors for the orientation array
ws_with_orientation = puma.Workspace.from_shape_value_vector((5, 6, 2), 1, (0.4, 2, 5))
print(f"Matrix shape of workspace: {ws_with_orientation.matrix.shape}")
print(f"Orientation shape of workspace: {ws_with_orientation.orientation.shape}")
print("Display Workspace matrix")
ws_with_orientation.show_matrix()


# In[ ]:


print("Display Workspace orientation")
ws_with_orientation.show_orientation()


# In[ ]:


# we can also convert a Numpy array into a Workspace directly by running:
array = np.random.randint(5, size=(10, 10, 10))
ws = puma.Workspace.from_array(array)
print(f"Matrix shape of workspace: {ws.get_shape()}")


# In[ ]:


# finally, we can create an empty workspace object and assign its matrix directly (not recommended):
ws = puma.Workspace()
ws.matrix = np.random.randint(5, size=(10, 10, 3))
print("Display Workspace")
ws.show_matrix()


# In[ ]:


# N.B. the following commands are equivalent
print("Different ways to index the matrix array:")
print(ws[0, 0, 0])
print(ws.matrix[0, 0, 0])


# It is important to keep the first three dimensions (X,Y,Z) of the matrix and orientation class variables the same.
# This is automatically enforced by using the class methods, but it is not when assigning them directly as in the last example.

# Let's now import a tomography image directly into a workspace (PuMA comes with some example files that can be imported
# using the path_to_example_file function as shown below):

# In[ ]:


ws_raw = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)
print(f"Shape of workspace: {ws_raw.matrix.shape}")


# This specific tiff stack is 8 bit, so the grayscale values will range from 0 to 255. PuMA can also import 16bit images,
# which allow a much larger range from 0 to 65535 (i.e. 2^16).

# The voxel length (in meters) of the workspace can either be set during import of a 3D tiff, or manually afterwards, as shown below: 

# In[ ]:


ws_raw.voxel_length = 1.3e-6


# We can visualize its slices by running the command below. By scrolling on top of the plot, you can slice through the
# material along the z axis. You can also use the left/right arrows on the keyboard to skip +/-10 slices or the up/down
# arrows to skip +/-100 slices (if this does not work automatically, try clicking on the plot first). In addition, on the
# bottom of the plot, the (x,y) coordinates are shown along with the corresponding grayscale value.

# In[ ]:


slices = puma.plot_slices(ws_raw, slice_direction='z', crange=None, cmap='gray', index=1)


# Otherwise, we can also 3D render it as (see the visualization tutorial for more tips on this):

# In[ ]:


puma.render_volume(ws_raw, notebook=notebook)


# Next, we show how to manipulate the domain, e.g. crop, rescale, resize and rotate it. 
# 
# An approach to crop a domain is the following:

# In[ ]:


ws_copy = ws_raw.copy()  # make a copy of the domain first
ws_copy.matrix = ws_copy[10:40, 35:, -20:]  # crop the domain by selecting ranges
print(f"Shape of original workspace: {ws_raw.get_shape()}")
print(f"Shape of cropped workspace: {ws_copy.get_shape()}")


# However, it is important to not fall in the trap of referencing the same Numpy array. Here is an example of how you
# SHOULD NOT perform cropping:

# In[ ]:


ws_bad = puma.Workspace()
ws_bad.matrix = ws_raw[10:40, 35:, -20:]  # WRONG: always make a copy first!
ws_bad[0, 0, 0] = np.random.randint(0, 255)  # otherwise, this line also changes ws_raw
print(ws_raw[10, 35, -20])
print(ws_bad[0, 0, 0])


# As you can see from the output, both the original Workspace and the newly created one share the same Numpy array for
# the matrix class variable (the second one is only a section of it, also called a Numpy view). This way, when one is
# changed, the other one will be changed as well. It is important to make a copy of a domain if the original workspace
# needs to be kept.
# 
# Next, we show how we can rescale a domain by a factor or resize it to a specified size. 

# In[ ]:


ws_copy = ws_raw.copy()
ws_copy.rescale(scale=0.5, segmented=False)

# Notice that now the axis have different limits
puma.compare_slices(ws_raw, ws_copy, index=50)

