import numpy as np
import pumapy as puma
import pyvista as pv
import scipy.ndimage as nd
import os
import sys


# ## Tutorial: Generating Triply Periodic Minimal Surfaces
# In this tutorial we demonstrate how to generate a TPMS microstructure
# We generate the TPMS structure. There are three different equations that can be used in the TPMS generator: 
# 
# Equation 0: grayscale value = q + sin(wx)*sin(wy)*sin(wz) + sin(wx)*cos(wy)*cos(wz) + cos(wx)*sin(wy)*cos(wz) + cos(wx)*cos(wy)*sin(wz)
# 
# Equation 1: grayscale value = q + cos(wx)*sin(wy) + cos(wy)*sin(wz) + cos(wi)*cos(wz)
# 
# Equation 2: grayscale value = q + cos(wx) + cos(wy) + cos(wz)
# 
# The values of w and q can either be provided as single values, or as a tuple. If a tuple is used, then the first and
# second values will be the minimum and maximum values for q and w, and the value will vary linearly between the two
# along the z-axis.
# 
# The value of w controls the pore diameter and the value of q controls the porosity

size = (400, 400, 400)  # size of the domain, in voxels. 
w = 0.08  # value of w in the equations above
q = 0.2  # value of q in the equations above

ws_eq0 = puma.generate_tpms(size, w, q, equation=0, segmented=False)
ws_eq1 = puma.generate_tpms(size, w, q, equation=1, segmented=False)
ws_eq2 = puma.generate_tpms(size, w, q, equation=2, segmented=False)

# The material is now stored in the puma workspace. By default, the grayscale range of [0,127] corresponds to the void
# space, and [128,255] contains the material.
# 
# Next we will visualize slices of the domains:
puma.plot_slices(ws_eq0)
puma.plot_slices(ws_eq1)
puma.plot_slices(ws_eq2)

# Next, we will visualize the 3D domains. To render the domain, the grayscale range corresponding to the material must be specified. In this case, the range of [128,255] corresponds to the material. 
puma.render_contour(ws_eq0, cutoff=(128, 255), notebook=False)
puma.render_contour(ws_eq1, cutoff=(128, 255), notebook=False)
puma.render_contour(ws_eq2, cutoff=(128, 255), notebook=False)


# If you would like to segment the domain, that can be done using the binarize function, or the set_material_id function.
# binarize(128) sets all grayscale values below 128 to 0, and all grayscale values above and equal to 128 to 1
ws_eq0.binarize(128)
ws_eq1.binarize(128)
ws_eq2.binarize(128)

puma.plot_slices(ws_eq0)
puma.plot_slices(ws_eq1)
puma.plot_slices(ws_eq2)
