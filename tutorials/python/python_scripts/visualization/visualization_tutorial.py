import numpy as np
import pumapy as puma
import pyvista as pv
import scipy.ndimage as nd
import os
import sys

# ## Tutorial: 3D Visualization
# In this tutorial we demonstrate the use of the 3D Visualization capabilities in pumapy. The 3D visualization is
# based on a coupling with Pyvista. Visualization can be performed on either raw tomography samples (each material
# phase is composed of a range of grayscale values) or segmented images (each material phase is a single grayscale
# value). Pumapy can currently produce volume, contour and orientation renderings. Examples of each visualization
# are shown below.

# Next we must either import or generate a material to visualize. Computational domains are stored in the workspace
# class. Below we show an example of both importing and computationally generating a material:

# Generating a workspace of randomly placed, intersecting spheres: with the inputs:
#  - size of domain in voxels: (200,200,200)
#  - diameter of each sphere in voxels: 20
#  - porosity of generated material: 0.7
ws_generated = puma.generate_random_spheres((200,200,200), 20, 0.7, segmented=False)

# The voxel length (in meters) of the workspace defaults to 1 micron (1e-6 m).
# To change the voxel length, modify the parameter directly: 
ws_generated.voxel_length = 1.3e-6

# Next we will import an example tomography file of size 200^3 and voxel length 1.3e-6
ws_imported = puma.import_3Dtiff(puma.path_to_example_file("200_fiberform.tif"), 1.3e-6)

# The workspaces above have not been segmented yet. In the case of generated workspace, the generated material has
# been stored for a grayscale [128,255] and the void phase is stored between [0,127]. The appropriate grayscale cutoff
# for the imported tomography sample is 90, such that [90,255] is material and [0,89] is the air/void. These values
# will be different for each tomography image.
# 
# First, we will generate both volume and contour renderings of the two materials without segmentation.

# Generating contour (i.e. isosurface) rendering of the computationally generated material
# The grayscale range of the material to be rendered is specified as (128,255) and is inclusive
puma.render_contour(ws_generated, cutoff=(128, 255), notebook=False)

# Generating volume (i.e. voxel) rendering of the computationally generated material
puma.render_volume(ws_generated, cutoff=(128, 255), notebook=False)

# by choosing the "edges" style, we can visualize the triangulated surface
puma.render_contour(ws_imported, cutoff=(90, 255), notebook=False, style="edges")

puma.render_volume(ws_imported, cutoff=(90, 255), notebook=False)


# Next, we will segment the workspaces, such that all void voxels contain an ID of 0 and all solid voxels contain an
# ID of 1. This could be expanded for multi-phase domains.

# Setting all grayscale values between 0 and 127, inclusive, to 0
ws_generated.set_material_id((0, 127), 0)
# Setting all grayscale values between 128 and 255, inclusive, to 1
ws_generated.set_material_id((128, 255),1)
# The above could also be achieved as: ws_generated.binarize_range((128, 255))

# Now on the tomography, we can binarize it using a single threshold as:
ws_imported.binarize(89)  # everything above 89 will be set 1, below to 0

# Now we will generate contour and volume renderings of the segmented files. Note that the grayscale values have changed.

p = pv.Plotter(shape=(1, 2), notebook=False)
p.subplot(0, 0)
p.add_text("Surface")
puma.render_contour(ws_generated, cutoff=(1, 1), notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(0, 1)
p.add_text("Voxels")
puma.render_volume(ws_generated, cutoff=(1, 1), notebook=False, solid_color=(255, 255, 255), add_to_plot=p, plot_directly=False)
p.show()

p = pv.Plotter(shape=(1, 2), notebook=False)
p.subplot(0, 0)
p.add_text("Surface")
puma.render_contour(ws_imported, cutoff=(1, 1), notebook=False, add_to_plot=p, plot_directly=False)
p.subplot(0, 1)
p.add_text("Voxels")
puma.render_volume(ws_imported, cutoff=(1, 1), notebook=False, solid_color=(255, 255, 255), add_to_plot=p, plot_directly=False)
p.show()

# Note that the contour renders for the segmented images are significantly less smooth than for the non-segmented
# images. This is because the triangulation algorithms have significantly less degrees of freedom when assigning
# triangle angles for segmented images, resulting in a rougher surface than for non-segmented images.
