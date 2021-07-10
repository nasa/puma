#=============================================================================
#TexGen: Geometric textile modeller.
#Copyright (C) 2012 Laurent Jeanmeure

#This program is free software; you can redistribute it and/or
#modify it under the terms of the GNU General Public License
#as published by the Free Software Foundation; either version 2
#of the License, or (at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#=============================================================================

"""
effectiveMatPropRVE.py

A repository of user defined functions to be compiled as .pyc.

"""

# Import the functions.
import dataHandling
import dataHandlingInPlane

# Copy the execfile() order on the Abaqus Command Line Interface (CLI)
# or use the "Run script" option from the File menu.

# execfile('effectiveMatPropRVE.py')
# import dataHandling; reload(dataHandling); execfile('effectiveMatPropRVE.py')


# CHANGE THE MODEL NAME HERE.
# DO NOT ADD THE '.ODB' EXTENSION.
# This is added automatically by the script when needed.

modelName = 'VoxelContinuumTest'
# Set the thermo-mechanical switch to 1 (CTE evaluation) or 0 (no CTE evaluation)
TM=1
# Set SingleLayerRVE switch to True for single layer voxel export and staggered boundary conditions
# False for all others (single layer will only calculate in-plane properties)
SingleLayerRVE = False
# Perform the operations.
if ( SingleLayerRVE):
    dataHandlingInPlane.mainScript(modelName, TM)
else:
    dataHandling.mainScript(modelName, TM)