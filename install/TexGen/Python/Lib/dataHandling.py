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
dataHandling.py

A repository of user defined functions to be compiled as .pyc.

"""

# Import default model database objects.
from abaqus import *

# Import symbolic constants defined by the ABAQUS scripting interface.
from abaqusConstants import *

# Used for log10(), sqrt() etc.
import math

# # Used for defining the load cases.
# import load

# # Import the step module.
# import step

# # Import the component modules.
# import modelSetUp

# Import viewports etc.
import visualization

		
def CreateReportFile(modelName, matPropRVE):
	
	# Create a filename.
	reportFilename = modelName + '.rpt'
		
	fOutputReport = open(reportFilename, 'w')

	sDescription = 'This file provides the equivalent material properties of a simulated unit-cell with appropriate theoretical boundary-conditions set-up.'
	fOutputReport.write(sDescription)

	sUCDisplacements = '\n\n\nOutput results:\n\nEffective compliance matrix:'

	# Displacements at key degrees of freedom.
	strFxe0x = "%1.3e" % matPropRVE[12]
	strFxe0y = "%1.3e" % matPropRVE[13]
	strFxe0z = "%1.3e" % matPropRVE[14]
	strFye0x = "%1.3e" % matPropRVE[15]
	strFye0y = "%1.3e" % matPropRVE[16]
	strFye0z = "%1.3e" % matPropRVE[17]
	strFze0x = "%1.3e" % matPropRVE[18]
	strFze0y = "%1.3e" % matPropRVE[19]
	strFze0z = "%1.3e" % matPropRVE[20]
	strSyz_g0 = "%1.3e" % matPropRVE[21]
	strSzx_g0 = "%1.3e" % matPropRVE[22]
	strSxy_g0 = "%1.3e" % matPropRVE[23]
	sUCDisplacements = sUCDisplacements + '\n'
	sUCDisplacements = sUCDisplacements + strFxe0x + '\t' + strFye0x + '\t' + strFze0x + '\t\t0\t\t' + '\t0\t\t' + '\t0\t\t'
	sUCDisplacements = sUCDisplacements + '\n'
	sUCDisplacements = sUCDisplacements + strFxe0y + '\t' + strFye0y + '\t' + strFze0y + '\t\t0\t\t' + '\t0\t\t' + '\t0\t\t'
	sUCDisplacements = sUCDisplacements + '\n'
	sUCDisplacements = sUCDisplacements + strFxe0z + '\t' + strFye0z + '\t' + strFze0z + '\t\t0\t\t' + '\t0\t\t' + '\t0\t\t'
	sUCDisplacements = sUCDisplacements + '\n'
	sUCDisplacements = sUCDisplacements + '\t0\t\t' + '\t0\t\t' + '\t0\t\t' + strSyz_g0 + '\t\t0\t\t' + '\t0\t\t'
	sUCDisplacements = sUCDisplacements + '\n'
	sUCDisplacements = sUCDisplacements + '\t0\t\t' + '\t0\t\t' + '\t0\t\t' + '\t0\t\t' + strSzx_g0  + '\t\t0\t\t'
	sUCDisplacements = sUCDisplacements + '\n'
	sUCDisplacements = sUCDisplacements + '\t0\t\t' + '\t0\t\t' + '\t0\t\t' + '\t0\t\t' + '\t0\t\t' + strSxy_g0
	fOutputReport.write(sUCDisplacements)

	sUCMaterialProperties = '\n\nEffective material properties:'

	# Effective material properties.
	strE1 = "%1.4e" % matPropRVE[0]
	strv12 = "%1.4f" % matPropRVE[1]
	strv13 = "%1.4f" % matPropRVE[2]
	strE2 = "%1.4e" % matPropRVE[3]
	strv21 = "%1.4f" % matPropRVE[4]
	strv23 = "%1.4f" % matPropRVE[5]
	strE3 = "%1.4e" % matPropRVE[6]
	strv31 = "%1.4f" % matPropRVE[7]
	strv32 = "%1.4f" % matPropRVE[8]
	strG23 = "%1.4e" % matPropRVE[9]
	strG31 = "%1.4e" % matPropRVE[10]
	strG12 = "%1.4e" % matPropRVE[11]
	strAlpha1 = "%1.4e" % matPropRVE[24]
	strAlpha2 = "%1.4e" % matPropRVE[25]
	strAlpha3 = "%1.4e" % matPropRVE[26]
	sUCMaterialProperties = sUCMaterialProperties + '\nE1: '
	sUCMaterialProperties = sUCMaterialProperties + strE1
	sUCMaterialProperties = sUCMaterialProperties + '\nv12: '
	sUCMaterialProperties = sUCMaterialProperties + strv12
	sUCMaterialProperties = sUCMaterialProperties + '\nv13: '
	sUCMaterialProperties = sUCMaterialProperties + strv13
	sUCMaterialProperties = sUCMaterialProperties + '\nE2: '
	sUCMaterialProperties = sUCMaterialProperties + strE2
	sUCMaterialProperties = sUCMaterialProperties + '\nv21: '
	sUCMaterialProperties = sUCMaterialProperties + strv21
	sUCMaterialProperties = sUCMaterialProperties + '\nv23: '
	sUCMaterialProperties = sUCMaterialProperties + strv23
	sUCMaterialProperties = sUCMaterialProperties + '\nE3: '
	sUCMaterialProperties = sUCMaterialProperties + strE3
	sUCMaterialProperties = sUCMaterialProperties + '\nv31: '
	sUCMaterialProperties = sUCMaterialProperties + strv31
	sUCMaterialProperties = sUCMaterialProperties + '\nv32: '
	sUCMaterialProperties = sUCMaterialProperties + strv32
	sUCMaterialProperties = sUCMaterialProperties + '\nG23: '
	sUCMaterialProperties = sUCMaterialProperties + strG23
	sUCMaterialProperties = sUCMaterialProperties + '\nG31: '
	sUCMaterialProperties = sUCMaterialProperties + strG31
	sUCMaterialProperties = sUCMaterialProperties + '\nG12: '
	sUCMaterialProperties = sUCMaterialProperties + strG12
	sUCMaterialProperties = sUCMaterialProperties + '\nCTE_1: '
	sUCMaterialProperties = sUCMaterialProperties + strAlpha1
	sUCMaterialProperties = sUCMaterialProperties + '\nCTE_2: '
	sUCMaterialProperties = sUCMaterialProperties + strAlpha2
	sUCMaterialProperties = sUCMaterialProperties + '\nCTE_3: '
	sUCMaterialProperties = sUCMaterialProperties + strAlpha3 + '\n'
	fOutputReport.write(sUCMaterialProperties)

	fOutputReport.close()
	
def DisplayEquivalentMaterialProperties(materialProperties):
	# Use getInputs for a nicely formatted pop-up box and ignore the returned value.
	strE1 = "%1.3e" % materialProperties[0]
	strv12 = "%1.2f" % materialProperties[1]
	strv13 = "%1.2f" % materialProperties[2]
	strE2 = "%1.3e" % materialProperties[3]
	strv21 = "%1.2f" % materialProperties[4]
	strv23 = "%1.2f" % materialProperties[5]
	strE3 = "%1.3e" % materialProperties[6]
	strv31 = "%1.2f" % materialProperties[7]
	strv32 = "%1.2f" % materialProperties[8]
	strG23 = "%1.3e" % materialProperties[9]
	strG31 = "%1.3e" % materialProperties[10]
	strG12 = "%1.3e" % materialProperties[11]
	strAlpha1 = "%1.3e" % materialProperties[24]
	strAlpha2 = "%1.3e" % materialProperties[25]
	strAlpha3 = "%1.3e" % materialProperties[26]
	
	fieldsMaterialProperties = (
		('E1:', strE1), \
		('v12:', strv12), \
		('v13:', strv13), \
		('E2:', strE2), \
		('v21:', strv21), \
		('v23:', strv23), \
		('E3:', strE3), \
		('v31:', strv31), \
		('v32:', strv32), \
		('G23:', strG23), \
		('G31:', strG31), \
		('G12:', strG12),
		('Alpha1:', strAlpha1),
		('Alpha2:', strAlpha2),
		('Alpha3:', strAlpha3))
	getInputs(fields=fieldsMaterialProperties,
			label='',
			dialogTitle='Effective material properties obtained from the unit cell analysis',)
	
def ComputeEquivalentMaterialProperties(modelName, thermoMechSwitch):

	# Open the relevent .ODB file.
	fileName = modelName + '.odb'
	resultODB = visualization.openOdb(path=fileName, readOnly=True)
	
	# Collect the various steps.
	isothermalStep = list(resultODB.steps.values())[0]
	if (thermoMechSwitch==1):
		thermoMechanicalStep = list(resultODB.steps.values())[1]
			
	# The load-cases are held in frames.
	# Index 0 holds the reference frame.
	# 1 holds the Fx load-case.
	frameFx = resultODB.steps[isothermalStep.name].frames[1]
	# 2 holds the Fy load-case.
	frameFy = resultODB.steps[isothermalStep.name].frames[2]
	# 3 holds the Fx load-case.
	frameFz = resultODB.steps[isothermalStep.name].frames[3]
	# 4 holds the Shear_xy load-case.
	frameShear_xy = resultODB.steps[isothermalStep.name].frames[4]
	# 5 holds the Shear_zx load-case.
	frameShear_zx = resultODB.steps[isothermalStep.name].frames[5]
	# 6 holds the Shear_yz load-case.
	frameShear_yz = resultODB.steps[isothermalStep.name].frames[6]

	if (thermoMechSwitch==1):
		# Index 0 holds the reference frame.
		frameThermomechanical = resultODB.steps[thermoMechanicalStep.name].frames[1]
		
	# Load case Fx.
	Fx_eps0_x = frameFx.fieldOutputs['U'].values[0].data[0]
	Fx_eps0_y = frameFx.fieldOutputs['U'].values[1].data[0]
	Fx_eps0_z = frameFx.fieldOutputs['U'].values[2].data[0]

	# Load case FY.
	Fy_eps0_x = frameFy.fieldOutputs['U'].values[0].data[0]
	Fy_eps0_y = frameFy.fieldOutputs['U'].values[1].data[0]
	Fy_eps0_z = frameFy.fieldOutputs['U'].values[2].data[0]
	
	# Load case Fz.
	Fz_eps0_x = frameFz.fieldOutputs['U'].values[0].data[0]
	Fz_eps0_y = frameFz.fieldOutputs['U'].values[1].data[0]
	Fz_eps0_z = frameFz.fieldOutputs['U'].values[2].data[0]
	
	# Load case Shear_xy.
	Shear_xy_gamma0_xy = frameShear_xy.fieldOutputs['U'].values[3].data[0]
    
    # Load case Shear_zx.
	Shear_zx_gamma0_zx = frameShear_zx.fieldOutputs['U'].values[4].data[0]
    
	# Load case Shear_yz.
	Shear_yz_gamma0_yz = frameShear_yz.fieldOutputs['U'].values[5].data[0]
	
	

	if (thermoMechSwitch==1):
		# Thermomechanical expansion.
		Th_eps0_x = frameThermomechanical.fieldOutputs['U'].values[0].data[0]
		Th_eps0_y = frameThermomechanical.fieldOutputs['U'].values[1].data[0]
		Th_eps0_z = frameThermomechanical.fieldOutputs['U'].values[2].data[0]
	
	# It is assumed that the needed data has been extracted properly.
		
	# Material properties.
	# The loads are now nominal.
	# E0_x = lVect[0]/volUC/Fx_eps0_x
	E0_x = 1.0/Fx_eps0_x
	v0_xy = -Fx_eps0_y/Fx_eps0_x
	v0_xz = -Fx_eps0_z/Fx_eps0_x
	
	E0_y = 1.0/Fy_eps0_y
	v0_yx = -Fy_eps0_x/Fy_eps0_y
	v0_yz = -Fy_eps0_z/Fy_eps0_y
	
	E0_z = 1.0/Fz_eps0_z
	v0_zx = -Fz_eps0_x/Fz_eps0_z
	v0_zy = -Fz_eps0_y/Fz_eps0_z

	G0_xy = 1.0/Shear_xy_gamma0_xy
	
	G0_yz = 1.0/Shear_yz_gamma0_yz
	
	G0_zx = 1.0/Shear_zx_gamma0_zx
	
	
	if (thermoMechSwitch==1):
		alpha0_x = Th_eps0_x
		alpha0_y = Th_eps0_y
		alpha0_z = Th_eps0_z
	else:
		alpha0_x = 0.0
		alpha0_y = 0.0
		alpha0_z = 0.0	
		
	# Now adding the displacements.
	return (E0_x, v0_xy, v0_xz,
		E0_y, v0_yx, v0_yz,
		E0_z, v0_zx, v0_zy,
		G0_yz, G0_zx, G0_xy,
		Fx_eps0_x, Fx_eps0_y, Fx_eps0_z,
		Fy_eps0_x, Fy_eps0_y, Fy_eps0_z,
		Fz_eps0_x, Fz_eps0_y, Fz_eps0_z,
		Shear_yz_gamma0_yz,
		Shear_zx_gamma0_zx,
		Shear_xy_gamma0_xy,
		alpha0_x,
		alpha0_y,
		alpha0_z)

def mainScript(modelName, switchTM):
	# This function loads an .ODB file, extracts its relevant information,
	# then computes the equivalent material properties.
	# A report file is also created.
	equivalentMaterialProperties = ComputeEquivalentMaterialProperties(modelName, switchTM)
	DisplayEquivalentMaterialProperties(equivalentMaterialProperties)
	CreateReportFile(modelName, equivalentMaterialProperties)

		