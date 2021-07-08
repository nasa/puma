#! /usr/bin/env python

# Import the TexGen modules
from TexGen.Core import *
from TexGen.Renderer import *

#####################
# GEOMETRY CREATION #
#####################

# Create a textile
Textile = CTextile()

# Create a list of 4 yarns
Yarns = [CYarn(), CYarn(), CYarn(), CYarn()]

# Add nodes to the yarns to describe their paths
Yarns[0].AddNode(CNode(XYZ(0, 0, 0)))
Yarns[0].AddNode(CNode(XYZ(0.22, 0, 0.05)))
Yarns[0].AddNode(CNode(XYZ(0.44, 0, 0)))

Yarns[1].AddNode(CNode(XYZ(0, 0.22, 0.05)))
Yarns[1].AddNode(CNode(XYZ(0.22, 0.22, 0)))
Yarns[1].AddNode(CNode(XYZ(0.44, 0.22, 0.05)))

Yarns[2].AddNode(CNode(XYZ(0, 0, 0.05)))
Yarns[2].AddNode(CNode(XYZ(0, 0.22, 0)))
Yarns[2].AddNode(CNode(XYZ(0, 0.44, 0.05)))

Yarns[3].AddNode(CNode(XYZ(0.22, 0, 0)))
Yarns[3].AddNode(CNode(XYZ(0.22, 0.22, 0.05)))
Yarns[3].AddNode(CNode(XYZ(0.22, 0.44, 0)))

# Create a lenticular section to be assigned to the yarns
Section = CSectionEllipse(0.18, 0.04)

# Assign the mesh type for this section (Triangular mesh with minimum angle of 20 degrees)
Section.AssignSectionMesh(CSectionMeshTriangulate(20))

# We want the same interpolation and section shape for all the yarns so loop over them all
for Yarn in Yarns:
    # Set the interpolation function
    Yarn.AssignInterpolation(CInterpolationCubic())
    # Assign a constant cross-section all along the yarn
    Yarn.AssignSection(CYarnSectionConstant(Section))
    # Set the resolution
    Yarn.SetResolution(20)
    # Add repeats to the yarn
    Yarn.AddRepeat(XYZ(0.44, 0, 0))
    Yarn.AddRepeat(XYZ(0, 0.44, 0))
    # Add the yarn to our textile
    Textile.AddYarn(Yarn)

# Output the yarn mesh geometry to abaqus
Textile.OutputAbaqus("polyester.inp");

# Create and assign the domain to the yarn
Textile.AssignDomain(CDomainPlanes(XYZ(0, 0, -0.02), XYZ(0.44, 0.44, 0.07)))

# Add the textile to our TexGen singleton with the name "polyester"
AddTextile("polyester", Textile)

#############
# RENDERING #
#############

# Get an instance of the CTexGenRenderer to render the textile
Renderer = CTexGenRenderer()

# Render our textile "polyester" clipped to the domain "box"
Renderer.RenderTextile("polyester")
Renderer.RenderDomain()

Renderer.SetBackgroundColor(COLOR(0, 0, 1))

# Start rendering
Renderer.Start()











