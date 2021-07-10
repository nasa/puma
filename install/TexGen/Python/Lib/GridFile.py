# TexGen: Geometric textile modeller.
# Copyright (C) 2006 Martin Sherburn
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

def AverageZComparison(x, y):
    """ Take the average of second and third element in an array and compare
    which is bigger. To be used in conjunction with the sort function. """
    xsum = x[1]+x[2]
    ysum = y[1]+y[2]
    if xsum < ysum:
        return -1
    if xsum > ysum:
        return 1
    return 0

def ExportGridFile(Filename, TextileName, NumPoints):
    """ Export a textile as a grid file. """
    # Open file for writing
    File = open(Filename, 'w')

    # Get the textile
    Textile = GetTextile(TextileName)

    # Get the domain
    Domain = Textile.GetDomain()

    # Get the limits of the domain
    Min, Max = Domain.GetMesh().GetAABB()

    # Write out the grid size
    File.write('** GRID SIZE: MinX, MinY, MinZ, MaxX, MaxY, MaxZ\n')
    File.write('%g,\t%g,\t%g,\t%g,\t%g,\t%g\n' % (Min.x, Min.y, Min.z, Max.x, Max.y, Max.z))

    # Write out the divisions
    File.write('** DIVISIONS: X, Y\n')
    File.write('%d,\t%d\n' % NumPoints)

    # Calculate spacing based on domain size and number of points
    Spacing = [0,0]
    Spacing[0] = (Max.x-Min.x)/(NumPoints[0])
    Spacing[1] = (Max.y-Min.y)/(NumPoints[1])

    # Create a 2D grid of points that will be used to look up information
    # from TexGen
    Points = []
    for i in range(NumPoints[0]):
        for j in range(NumPoints[1]):
            Point = XY((i+0.5)*Spacing[0]+Min.x, (j+0.5)*Spacing[1]+Min.y)
            Points.append(Point)

    # Create a list of lists the same size as the number of points
    IntersectionsList = [[] for item in Points]

    # Store the repeat limits for the yarns
    TranslationsList = []

    # Get a surface mesh of each yarn
    for i, Yarn in enumerate(Textile.GetYarns()):
        # Create an empty mesh
        Mesh = CMesh()
        # Get the repeat limits for the yarn
        Translations = Domain.GetTranslations(Yarn)
        # Create the surface mesh
        Yarn.AddSurfaceToMesh(Mesh, Translations)
        # Convert to triangles because it is needed for line intersection
        Mesh.ConvertQuadstoTriangles()
        # Store the repeat limits for use later
        TranslationsList.append(Translations)
        # For each XY point calculate intersections between a vertical line
        # and the yarn mesh
        for Point, Intersections in zip(Points, IntersectionsList):
            # The intersection line is defined by two points
            P1 = XYZ(Point.x, Point.y, 0.0)
            P2 = XYZ(Point.x, Point.y, 1.0)
            # Results will store the results of the call to Mesh.IntersectLine
            Results = DoubleXYZPairVector()
            Mesh.IntersectLine(P1, P2, Results)
            # Extract just the Z coordinates from the results
            ZCoords = [item[0] for item in Results]
            try:
                ZMin = min(ZCoords)
                ZMax = max(ZCoords)
            except ValueError:
                pass
            else:
                Intersections.append((i, ZMin, ZMax))

    # Write out the points to file now
    File.write('** GRID POINTS: NUMVOLUMES { VOLUME NUMBER, BOTTOM Z, TOP Z, DIRECTION (X, Y, Z) }\n')
    for Intersections, Point in zip(IntersectionsList, Points):
        # Sort the intersections in ascending order of the average of ZMin and ZMax
        Intersections.sort(cmp=AverageZComparison)
        # Write out the number of volumes
        File.write(str(1+2*len(Intersections)) + '\n')
        # Store the Z coordinate of the previous volume
        PrevZ = Min.z
        # Output all the volumes
        for Intersection in Intersections:
            # Unpack tuple
            YarnID, ZMin, ZMax = Intersection
            # Write a domain volume
            File.write('0,\t%g,\t%g,\t0,\t0,\t0\n' % (PrevZ, ZMin))
            # Get the yarn associated with the yarn volume
            Yarn = Textile.GetYarn(YarnID)
            # Get the midpoint of the yarn volume
            MidPt = XYZ(Point.x, Point.y, (ZMin+ZMax)/2)
            # Get the tangent for this point, allow a certain tolerance because we already
            # know the point inside the yarn we just want to get its tangent
            Tangent = XYZ()
            bInside = Yarn.PointInsideYarn(MidPt, TranslationsList[YarnID], Tangent, None, None, None, 0.01)
            assert bInside
            # Write a yarn volume with yarn tangent
            File.write('%d,\t%g,\t%g,\t%g,\t%g,\t%g\n' % (YarnID+1, ZMin, ZMax, Tangent.x, Tangent.y, Tangent.z))
            # Updated previous Z coordinate
            PrevZ = ZMax
        # Write the top domain volume
        File.write('0,\t%g,\t%g,\t0,\t0,\t0\n' % (PrevZ, Max.z))

