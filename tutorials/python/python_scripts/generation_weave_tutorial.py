import numpy as np
import pumapy as puma
import os

# ## Tutorial: Weaves
# 
# In order to run this tutorial on Colab, make sure to setup the environment correctly, as explained in the
# "Installation setup and imports" section.
# 
# In this tutorial we demonstrate the creation of weaves using TexGen and how to import them into pumapy.
# TexGen python scrips can be simply obtained by either following TexGen tutorials
# https://github.com/louisepb/TexGenScriptingGuide or by recording the actions in the TexGen GUI (of which there are already
# bundled executables for Windows at https://sourceforge.net/projects/texgen/files/). 
# 
# Let's start by creating a simple LayerToLayer weave using a TexGen python script.

from TexGen.Core import *

NumBinderLayers = 2 
NumXYarns = 3 
NumYYarns = 4 
XSpacing = 1.0
YSpacing = 1.0 
XHeight = 0.2 
YHeight = 0.2 
weave = CTextileLayerToLayer(NumXYarns, NumYYarns, XSpacing, YSpacing, XHeight, YHeight, NumBinderLayers)

#set number of binder / warp yarns
NumBinderYarns = 2
NumWarpYarns = NumXYarns - NumBinderYarns
weave.SetWarpRatio(NumWarpYarns)
weave.SetBinderRatio(NumBinderYarns)

#setup layers: 3 warp, 4 weft
weave.SetupLayers( 3, 4, NumBinderLayers)

#set yarn dimensions: widths / heights
weave.SetYYarnWidths(0.8)
weave.SetYYarnWidths(0.8)
weave.SetBinderYarnWidths(0.4)
weave.SetBinderYarnHeights(0.1)

#define offsets for the two binder yarns
P = [[0, 1, 3, 0],[3, 0, 0, 3]]

#assign the z-positions to the binder yarns
for y in range(NumWarpYarns,NumXYarns): #loop through number of binder yarns
	offset = 0 
	for x in range(NumYYarns): #loop through the node positions	
		weave.SetBinderPosition(x, y, P[y-NumWarpYarns][offset])
		offset += 1


# Next we create the domain, i.e. the lengths of the box containing the weave. In this case we will let TexGen figure
# it out automatically using the AssignDefaultDomain function:

weave.AssignDefaultDomain()
domain = weave.GetDefaultDomain()

# Now we need to specify a folder to export our TexGen weave
export_path = "out"  # CHANGE THIS PATH

if not os.path.exists(export_path):
    os.makedirs(export_path)

# Then, we pass the weave and the domain to the PuMApy exporter, which creates the voxel grid and exports it to the
# specified directory (expensive step ~1.5-2 mins):

puma.export_weave_vtu(os.path.join(export_path, "weavetest"), weave, domain, 200)

# This can be then read back into a pumapy.Workspace:
ws = puma.import_weave_vtu(os.path.join(export_path, "weavetest_200"))


# And we can volume render it:
puma.render_volume(ws, cutoff=(1, ws.matrix.max()), solid_color=None, notebook=False, cmap='jet')

