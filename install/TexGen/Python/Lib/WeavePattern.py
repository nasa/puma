# TexGen: Geometric textile modeller.
# Copyright (C) 2014 Louise Brown
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

from TexGen.Core import *
from math import *

def SpacedStringToIntVector( Str ):
	Vector = IntVector()
	for s in Str.split():
		if s.isdigit():
			Vector.push_back(int(s))
	return Vector
	
def StringToIntVector( Str ):
	Vector = IntVector()
	for a in Str:
		if a.isdigit():
			Vector.push_back(int(a))
	return Vector
	
def SetupBinders( Str ):
    # Sets up vector indicating which warp yarns are binders
	Vector = BoolVector()
	NumBinders = 0
	Str = Str.replace(' ','')
	for i in range( len( Str )-1 ):
		if Str[i] == '1' and ( Str[i+1] == '1' or Str[i+1] == '\n' ):
			Vector.insert( Vector.begin(), True )
			NumBinders += 1
		elif Str[i] == '1':
			#Vector.push_back( False )
			Vector.insert( Vector.begin(), False )
	#for s in Str.split('1'):
	#	if s == '' or s == '\n':
			#Vector.push_back( True )
	#		Vector.insert( Vector.begin(), True )
	#		NumBinders += 1
	#	else:
	#		#Vector.push_back( False )
	#		Vector.insert( Vector.begin(), False )
	return NumBinders, Vector

def ImportWeavePattern( Filename ):
	file = open(Filename,'r')
	Pattern = CPatternDraft()

	firstLine = True
	WeftMatrix = []   # Vector of IntVectors for storing rows of weave pattern
	WeftDensity = 0.0
	Width = 0.0
	TowArea = 0.0
	LinearDensity = 0.0
	FibreDiameter = 0.0
	FibreDensity = 0.0
	FibreCount = 0
	TowWidth = 0.0
	TowHeight = 0.0
	BinderWidth = 0.0
	BinderHeight = 0.0
	tol = 1e-6
	WeftRepeat = False
	Orthogonal = False
	
	for line in file:
		# First line contains layer data
		if firstLine:
			firstLine = False
			Layers = SpacedStringToIntVector( line )
			NumWarps = line.count('1')  # Sum of number of binders and warp stacks
			
			NumBinders, Binders = SetupBinders( line )
			print(NumBinders)
			
			NumWarpLayers = int(max(line))  # Number of warp layers set to size of largest warp stack
		else:  # Read in row matrix
			parts = line.split()
			if not parts:  # empty line
				continue
			print(parts)
			if parts[0] == 'FIBRE_COUNT':
				FibreCount = int(parts[1])
			elif parts[0] == 'LINEAR_DENSITY':  
				LinearDensity = float(parts[1])
				LinearDensityUnits = parts[2]
			elif parts[0] == 'FIBRE_DIAMETER':
				FibreDiameter = float(parts[1])
				FibreDiameterUnits = parts[2]
			elif parts[0] == 'FIBRE_DENSITY':
				FibreDensity = float(parts[1])
				FibreDensityUnits = parts[2]
			elif parts[0] == 'WIDTH':
				Width = float(parts[1])
				WidthUnits = parts[2]
			elif parts[0] == 'WEFT_DENSITY':
				WeftDensity = float(parts[1])
				WeftDensityUnits = parts[2]
			elif parts[0] == 'TOW_AREA':
				TowArea = float(parts[1])
				TowAreaUnits = parts[2]
			elif parts[0] == 'WEFT_REPEAT':
				WeftRepeat = True
			elif parts[0] == 'ORTHOGONAL':
				Orthogonal = True
			elif parts[0] == 'TOW_WIDTH':
				TowWidth = float(parts[1])
				TowWidthUnits = parts[2]
			elif parts[0] == 'TOW_HEIGHT':
				TowHeight = float(parts[1])
				TowHeightUnits = parts[2]
			elif parts[0] == 'BINDER_WIDTH':
				BinderWidth = float(parts[1])
				BinderWidthUnits = parts[2]
			elif parts[0] == 'BINDER_HEIGHT':
				BinderHeight = float(parts[1])
				BinderHeightUnits = parts[2]
			else:
				allNumeric = True
				for part in parts:
					if not part.isdigit():
						allNumeric = False
						print('Error in weave pattern line, not all numeric')
						break
				
				if allNumeric == True:	
                    # Save row as string in CPatternDraft and vector of IntVectors in WeftMatrix
					Pattern.AddRow( line )
					LineVector = StringToIntVector( line )
					WeftMatrix.append(LineVector) 

	file.close()

	NumWefts = len(WeftMatrix)
	
	# Check that number of items in layer info matches the number of warps
	if (len(Layers) != len( WeftMatrix[0])):
		return ''
	
	if ( (fabs(Width) < tol) or (fabs(WeftDensity) < tol) or ((fabs(TowArea) < tol) and ((fabs(TowWidth) < tol) or (fabs(TowHeight) < tol))) ):
		print('Width, weft density or tow area missing in input file.  Using default values')
		WarpSpacing = 1.0
		WeftSpacing = 1.0
		TowWidth = 0.8
		TowHeight = 0.2
	else:
		WarpSpacing = ConvertUnits( Width, WidthUnits, 'mm' )/NumWarps
		print(str(WarpSpacing))
	
		WeftSpacing = 1/ConvertUnits( WeftDensity, WeftDensityUnits, '/mm' )
		print(str(WeftSpacing))
		
		if ( (fabs(TowWidth) > tol) and (fabs(TowHeight) > tol) ):
			TowWidth = ConvertUnits( TowWidth, TowWidthUnits, 'mm')
			TowHeight = ConvertUnits( TowHeight, TowHeightUnits, 'mm')
		else:
			TowArea = ConvertUnits( TowArea, TowAreaUnits, 'mm^2')
			# Work on 6:1 ratio for width to height of yarn in weave
			TowHeight = 2.0 * sqrt( TowArea / ( 6 * pi ) )
			TowWidth = 6 * TowHeight
			
		if ( (fabs(BinderWidth) > tol) and (fabs(BinderHeight) > tol) ):
			print('Convert binder')
			BinderWidth = ConvertUnits( BinderWidth, BinderWidthUnits, 'mm')
			BinderHeight = ConvertUnits( BinderHeight, BinderHeightUnits, 'mm')
			
	# Create textile
	if Orthogonal:
		Textile = CTextileOrthogonal(NumWarps, NumWefts, WarpSpacing, WeftSpacing, TowHeight, TowHeight, False, True )
	else:
		Textile = CTextile3DWeave( NumWarps,NumWefts, WarpSpacing, WeftSpacing, TowHeight, TowHeight )
	if Textile == 0:
		return ''
		
	if Orthogonal:
		Textile.SetBinderPattern( Binders )
		NumLayers = (NumWarpLayers * 2) + 3 # Num weft layers = num warp layers + 1, then layer at top and bottom for binders  
	else:
		Textile.SetWarpRatio( NumWarps )
		Textile.SetBinderRatio( 1 )
		# Need to find max no of layers
		NumLayers = NumWarpLayers + 1  # Just have warps and one binder in stack
		
    # Set up cells for storing yarn configuration, all initialised to PATTERN3D_NOYARN
	for i in range(NumLayers):  
		Textile.AddNoYarnLayer()
		
	Textile.SetXYarnWidths( TowWidth )
	Textile.SetYYarnWidths( TowWidth )
	
	if ( (fabs(BinderWidth) > tol) and (fabs(BinderHeight) > tol) ):
		print('Setting binder widths')
		Textile.SetBinderYarnWidths( BinderWidth )
		Textile.SetBinderYarnHeights( BinderHeight )
		Textile.SetBinderYarnPower( 0.2 )
	else:
		Textile.SetBinderYarnPower(1.0)
		
	Textile.SetWarpYarnPower(1.0)
	Textile.SetWeftYarnPower(1.0)
	
    # Set whether or not textile will repeat in weft direction
	Textile.SetWeftRepeat( WeftRepeat )

	# Set up weave pattern in textile
	for i in range(NumWefts-1,-1,-1):
		Textile.SetupWeftRow( Layers, WeftMatrix[i], NumWarps, NumWefts-1-i )
		
    # If orthogonal textile move wefts into stacks where possible
	if Orthogonal:
		Textile.ConsolidateCells()
		
	Textile.AssignDefaultDomain()
	
	if fabs(LinearDensity) > tol:
		Textile.SetYarnLinearDensity( WARP, LinearDensity, LinearDensityUnits )
		Textile.SetYarnLinearDensity( WEFT, LinearDensity, LinearDensityUnits )
	if fabs(FibreDiameter) > tol:
		Textile.SetFibreDiameter( WARP, FibreDiameter, FibreDiameterUnits )
		Textile.SetFibreDiameter( WEFT, FibreDiameter, FibreDiameterUnits )
	if fabs(FibreDensity) > tol:
		Textile.SetFibreDensity( WARP, FibreDensity, FibreDensityUnits )
		Textile.SetFibreDensity( WEFT, FibreDensity, FibreDensityUnits )
	if FibreCount != 0:
		Textile.SetFibresPerYarn( WARP, FibreCount )
		Textile.SetFibresPerYarn( WEFT, FibreCount )
	
	TextileName = AddTextile(Textile)
	#return Textile
	return TextileName