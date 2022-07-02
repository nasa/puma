import numpy as np
import pumapy as puma
import pyvista as pv
import scipy.ndimage as nd
import os
import sys


# ## Porespace identification
# When dealing with porous materials, it is often useful to distinguish between closed and open porosity.
# This is particularly relevant when particle or ray casting simulations need to be performed. In these cases,
# closed porosity throughout a domain becomes a problem, because particles get trapped inside these pockets,
# inevitably altering the output of a simulation. For this reason, a method to identify and fill the closed
# porosity was implemented in both C++ PuMA and pumapy.
# Let's first create a 50% dense sample full of hollowed out spheres and visualize it:

ws = puma.generate_random_spheres((200, 200, 200), diameter=20, porosity=0.5, allow_intersect=True, segmented=False)

# void is 0, solid is 255, but the material does not jump from 0-255, it gradually does so
# we therefore use this gradient to select the shell of each sphere
ws.binarize_range((1, 254))
puma.render_contour(ws, cutoff=(1, 1), notebook=False)


# Now we can run the pore filling algorithm, which runs a connected component analysis. The fill_closed_pores function
# needs to be given the solid range (in this case only 1 since we have already binarized the workspace) and the ID
# value to fill the closed porosity throughout the workspace (in this case we set 2).
filled_ws, pores = puma.fill_closed_pores(ws, (1, 1), 2, return_pores=True)

# When the return_pores flag is True, then it returns both the already filled workspace and a Numpy array of pores
# with IDs: solid == 0, largest pore (likely open porosity) == 1, smaller pores (likely closed porosity) == 2 - N.
# We can visualize both these outputs as:

puma.render_volume(pores[:pores.shape[0]//2], cutoff=(1, 10), solid_color=None, notebook=False, cmap='jet')
puma.render_volume(filled_ws, notebook=False, cmap='jet')

# Note that the identify_porespace function performs the same operation as the fill_closed_pores, except that it
# does not fill the closed pores throughout the workspace, it only returns the labelled pores identified.
