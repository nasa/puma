#! /usr/bin/env python

from TexGen.Core import *
from TexGen.Renderer import *

# Create a textile
Textile = CTextile()

# Create a lenticular section and add it to our TexGen singleton

Section = CSectionLenticular(0.3, 0.14)
Section.AssignSectionMesh(CSectionMeshTriangulate(30))

# Create 4 yarns
Yarns = (CYarn(), CYarn(), CYarn(), CYarn())

# Add nodes to the yarns to describe their paths
Yarns[0].AddNode(CNode(XYZ(0, 0, 0)))
Yarns[0].AddNode(CNode(XYZ(0.35, 0, 0.15)))
Yarns[0].AddNode(CNode(XYZ(0.7, 0, 0)))

Yarns[1].AddNode(CNode(XYZ(0, 0.35, 0.15)))
Yarns[1].AddNode(CNode(XYZ(0.35, 0.35, 0)))
Yarns[1].AddNode(CNode(XYZ(0.7, 0.35, 0.15)))

Yarns[2].AddNode(CNode(XYZ(0, 0, 0.15)))
Yarns[2].AddNode(CNode(XYZ(0, 0.35, 0)))
Yarns[2].AddNode(CNode(XYZ(0, 0.7, 0.15)))

Yarns[3].AddNode(CNode(XYZ(0.35, 0, 0)))
Yarns[3].AddNode(CNode(XYZ(0.35, 0.35, 0.15)))
Yarns[3].AddNode(CNode(XYZ(0.35, 0.7, 0)))

# We want the same interpolation and section shape for all the yarns so loop over them all
for Yarn in Yarns:
    # Set the interpolation function
    Yarn.AssignInterpolation(CInterpolationCubic())
    # Assign a constant cross-section all along the yarn
    Yarn.AssignSection(CYarnSectionConstant(Section))
    # Set the resolution
    Yarn.SetResolution(8)
    # Add repeats to the yarn
    Yarn.AddRepeat(XYZ(0.7, 0, 0))
    Yarn.AddRepeat(XYZ(0, 0.7, 0))
    # Add the yarn to our textile
    Textile.AddYarn(Yarn)

Textile.AssignDomain(CDomainPlanes(XYZ(0, 0, -1), XYZ(0.7, 0.7, 2)))

# Add the textile to our TexGen singleton with the name "cotton"
AddTextile("cotton", Textile)

Textile.OutputAbaqus("cotton.inp", 1);

# Get an instance of the CTexGenRenderer to render the textile
Renderer = CTexGenRenderer()

# Render our textile "cotton" clipped to the domain "box"
Renderer.RenderTextile("cotton")

#Renderer.SetBackgroundColor(0, 0, 1)

# Start rendering
Renderer.Start()











