#! /usr/bin/env python

from TexGen.Core import *

# Create a textile
textile = CTextile()

# Create a yarn
yarn = CYarn()

# Add some nodes
yarn.AddNode(CNode(XYZ(0, 0, 0)))
yarn.AddNode(CNode(XYZ(5, 0, 2)))
yarn.AddNode(CNode(XYZ(10, 0, 0)))

# Set the interpolation function
yarn.AssignInterpolation(CInterpolationCubic())

# Assign a constant cross-section all along the yarn
yarn.AssignSection(CYarnSectionConstant(CSectionEllipse(2, 1)))

# Set the resolution
yarn.SetResolution(20)

# Create some repeats for the yarn
yarn.AddRepeat(XYZ(10, 0, 0))
yarn.AddRepeat(XYZ(0, 10, 0))

# Add the yarn to our textile
textile.AddYarn(yarn)

# Create a new Yarn
yarn = CYarn()

# Add some nodes
yarn.AddNode(CNode(XYZ(0,0,2)))
yarn.AddNode(CNode(XYZ(0,5,0)))
yarn.AddNode(CNode(XYZ(0,10,2)))

# Set the interpolation function
yarn.AssignInterpolation(CInterpolationCubic())

# Assign a cross-section interpolated between the nodes
YarnSection = CYarnSectionInterpNode()
YarnSection.AddSection(CSectionLenticular(2, 0.5))
YarnSection.AddSection(CSectionEllipse(2, 1))
YarnSection.AddSection(CSectionLenticular(2, 0.5))
yarn.AssignSection(YarnSection);

# Set the resolution
yarn.SetResolution(20)

# Create some repeats for the yarn
yarn.AddRepeat(XYZ(10, 0, 0))
yarn.AddRepeat(XYZ(0, 10, 0))

# Add the yarn to our textile
textile.AddYarn(yarn)

# Add the textile to our TexGen singleton
AddTextile("test", textile)








