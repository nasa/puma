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

# Create a 4x4 2d woven textile with yarn spacing of 5 and thickness 2
# The fifth parameter indicates whether to refine the textile to avoid intersections
Textile = CTextileWeave2D(4, 4, 5, 2, False);

# Set the weave pattern
Textile.SwapPosition(3, 0);
Textile.SwapPosition(2, 1);
Textile.SwapPosition(1, 2);
Textile.SwapPosition(0, 3);

# Adjust the yarn width and height
Textile.SetYarnWidths(4);
Textile.SetYarnHeights(0.8);

# Setup a domain
Textile.AssignDefaultDomain()

# Add the textile
AddTextile(Textile)