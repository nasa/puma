# =============================================================================
# TexGen: Geometric textile modeller.
# Copyright (C) 2015 Louise Brown

# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
# =============================================================================

# Create a textile
Textile = CTextile()

# Create a lenticular section
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

# Create a domain and assign it to the textile
Textile.AssignDomain(CDomainPlanes(XYZ(0, 0, -0.1), XYZ(0.7, 0.7, 0.25)));

# Add the textile with the name "cotton"
AddTextile("cotton", Textile)











