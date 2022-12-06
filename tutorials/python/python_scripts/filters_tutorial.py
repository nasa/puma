import numpy as np
import pumapy as puma
import os

notebook = False  # when running locally, actually open pyvista window
export_path = "out"  # CHANGE THIS PATH
if not os.path.exists(export_path):
    os.makedirs(export_path)

# ## Tutorial
# In this tutorial we show the use of the image filters implemented in pumapy. Let's start by importing an image:

ws = puma.import_3Dtiff(puma.path_to_example_file("100_fiberform.tif"), 1.3e-6)

# In succession, we can now run the different image filters and show their output:
# 3D Median filter (edge preserving):

ws_median = ws.copy()
# the size refers to the side of the cubical kernel to be applied
puma.filter_median(ws_median, size=5)

puma.compare_slices(ws, ws_median, 'z', index=1)

# 3D Gaussian filter:
ws_gaussian = ws.copy()

puma.filter_gaussian(ws_gaussian, sigma=2, apply_on_orientation=False)

puma.compare_slices(ws, ws_gaussian, 'z', index=1)

# 3D Exact euclidean distance transform:

ws_edt = ws.copy()

puma.filter_edt(ws_edt, cutoff=(90, 255))

puma.compare_slices(ws, ws_edt, 'z', index=1)

# 3D Mean filter:

ws_mean = ws.copy()

# the size refers to the side of the cubical kernel to be applied
puma.filter_mean(ws_mean, size=10)

puma.compare_slices(ws, ws_mean, 'z', index=1)

# 3D morphological erosion filter:

ws_erode = ws.copy()

# the size refers to the side of the spherical kernel to be applied
puma.filter_erode(ws_erode, cutoff=(90, 255), size=3)

ws_binary = ws.copy()
ws_binary.binarize_range((90, 255))

puma.compare_slices(ws_binary, ws_erode, 'z', index=1)

# 3D morphological dilation filter:

ws_dilate = ws.copy()

# the size refers to the side of the spherical kernel to be applied
puma.filter_dilate(ws_dilate, cutoff=(90, 255), size=3)

ws_binary = ws.copy()
ws_binary.binarize_range((90, 255))

puma.compare_slices(ws_binary, ws_dilate, 'z', index=1)

# 3D morphological opening filter (i.e. dilation first and then erosion):

ws_opening = ws.copy()

# the size refers to the side of the spherical kernel to be applied
puma.filter_opening(ws_opening, cutoff=(90, 255), size=3)

ws_binary = ws.copy()
ws_binary.binarize_range((90, 255))

puma.compare_slices(ws_binary, ws_opening, 'z', index=1)

# 3D morphological closing filter (i.e. erosion first and then dilation)

ws_closing = ws.copy()

# the size refers to the side of the spherical kernel to be applied
puma.filter_closing(ws_closing, cutoff=(90, 255), size=3)

ws_binary = ws.copy()
ws_binary.binarize_range((90, 255))

puma.compare_slices(ws_binary, ws_closing, 'z', index=1)

