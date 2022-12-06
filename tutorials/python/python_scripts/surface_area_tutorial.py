#!/usr/bin/env python
# coding: utf-8

# ![puma logo](https://github.com/nasa/puma/raw/main/doc/source/puma_logo.png)

# # Surface Area

# The objective of this notebook is to familiarize new users with the main datastructures that stand at the basis of the
# PuMA project, and outline the functions to compute material properties (please refer to these papers
# ([1](https://www.sciencedirect.com/science/article/pii/S2352711018300281),
# [2](https://www.sciencedirect.com/science/article/pii/S235271102100090X)) for more details on the software).

# ## Installation setup and imports

# The first code block will execute the necessary installation and package import. 
# 
# If you are running this jupyter notebook locally on your machine, assuming you have already installed the software,
# then the installation step will be skipped.

# In[ ]:


# for interactive slicer
get_ipython().run_line_magic('matplotlib', 'widget')
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
# In this tutorial we demonstrate how to calculate the surface area of a microstructure

# We import or generate the microstructure

# In[ ]:


ws_fiberform = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)


# The calculation of the surface area requires the value for the voxel length to be set. The voxel length can be set
# during import, as shown above. The voxel length can also be set for the workspace directly, as shown below:

# In[ ]:


ws_fiberform.voxel_length = 1.3e-6  # setting the voxel length in meters


# For an accurate calculation of the surface area, it is recommended to use an unsegmented domain. In a segmented domain,
# each phase is defined by a single grayscale value (i.e. 0 for void and 1 for solid). In an unsegmented domain, the
# material phases are specified by a range of grayscale value. In the case of the imported tomography, the void range is
# [0,89] and the solid range is [90,255], as shown by the plotted slice below:

# In[ ]:


puma.plot_slices(ws_fiberform)


# An unsegmented domain is recommended because the surface area relies on the marching cubes triangulation of the isosurface,
# which is much more accurate for an unsegmented domain than a segmented domain.
# 
# To calculate the surface area, we use the puma.compute_surface_area function: The function returns both the raw area
# and the specific surface area. The specific surface area is the more often used quantity, and defines the surface area
# divided by the volume of the domain, with units of 1/m.

# In[ ]:


cutoff = (90, 255)  # specifying the grayscale range of the phase of interest

area_us, specific_area_us = puma.compute_surface_area(ws_fiberform, cutoff)
print("Unsegmented:",area_us,specific_area_us)


# If a segmented image must be used, a Gaussian blur can be utilized to help smooth out the triangulation for the surface
# area calculation. Note, however, that this blurring would smear any sharp features present in the microstrucutre.
# It is more appropriate for segmented images of microstrucutres with smooth features. As an example, the above domain is
# segmented and the surface area calculated with and without the Gaussian blur.

# In[ ]:


ws_fiberform.binarize(90)  # segmenting the domain. [0,89] --> 0 and [90,255] --> 1
cutoff = (1, 1)  # updating the cutoff now that the domain is segmented

area, specific_area = puma.compute_surface_area(ws_fiberform, cutoff, flag_gaussian=False)
print("Segmented:",area,specific_area)
print("Percent Error: ", (specific_area - specific_area_us)/specific_area_us * 100.)

area, specific_area = puma.compute_surface_area(ws_fiberform, cutoff, flag_gaussian=True)
print("Segmented Gaussian:",area,specific_area)
print("Percent Error: ", (specific_area - specific_area_us)/specific_area_us * 100.)

