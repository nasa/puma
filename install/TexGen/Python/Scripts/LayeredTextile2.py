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

# Create a 4x4 satin weave with yarn spacing of 1 and thickness of 0.2
weave = CTextileWeave2D(4, 4, 1, 0.2, 0, False)
weave.SetGapSize(0)

# Set the weave pattern
weave.SwapPosition(0, 3)
weave.SwapPosition(1, 2)
weave.SwapPosition(2, 1)
weave.SwapPosition(3, 0)

# Adjust the yarn widths and heights
weave.SetYarnWidths(0.8)
weave.SetYarnHeights(0.1)

# Assign the domain 
weave.AssignDefaultDomain()

# Create a layered textile
LayeredTextile = CTextileLayered()

# Add the first layer with specified offset
Offset = XYZ(0.25, 0.5, 0)
LayeredTextile.AddLayer( weave, Offset );

# Create 2nd textile: Plain weave, spacing of 1 and thickness 0.2
weave1 = CTextileWeave2D(2, 2, 1, 0.25, 0, False)
weave1.SetGapSize(0)
weave1.SetYarnWidths(0.8)
weave1.SwapPosition(0, 1)
weave1.SwapPosition(1, 0)
weave1.SetXYarnWidths(0, 0.9)
weave1.SetXYarnHeights(0, 0.12)
weave1.SetXYarnSpacings(0, 1)
weave1.SetXYarnWidths(1, 0.8)
weave1.SetXYarnHeights(1, 0.1)
weave1.SetXYarnSpacings(1, 1)
weave1.SetYYarnWidths(0, 0.8)
weave1.SetYYarnHeights(0, 0.1)
weave1.SetYYarnSpacings(0, 1)
weave1.SetYYarnWidths(1, 0.9)
weave1.SetYYarnHeights(1, 0.12)
weave1.SetYYarnSpacings(1, 1)

weave1.AssignDefaultDomain()

# Offsets for second layer.  z offset is height of first textile
Offset = XYZ(0.4,0.2,0.2)

# Add the second textile to the layered textile
LayeredTextile.AddLayer( weave1, Offset )

# Get the size of the domain for the second textile
Domain1 = weave1.GetDefaultDomain()
Min = XYZ()
Max = XYZ()
Domain1.GetBoxLimits(Min, Max)

# Get the domain of the first textile
Domain = weave.GetDefaultDomain()
Plane = PLANE()
# Get the domain upper surface
index = Domain.GetPlane( XYZ(0,0,-1), Plane )
# Offset the domain to include the second textile
Plane.d -= Max.z - Min.z
Domain.SetPlane( index, Plane )

LayeredTextile.AssignDomain( Domain )

# Add the textile with the name "LayeredTextile"
AddTextile('LayeredTextile', LayeredTextile)



