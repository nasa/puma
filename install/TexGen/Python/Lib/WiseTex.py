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
from copy import copy

def ImportWiseTex(filename):
    """ Import a file from WiseTex. All WiseTex section points are
    defined as master nodes in TexGen to ensure that the geometry is
    exactly the same.
    Returns the name of the textile imported."""
    # Open file
    File = open(filename, 'r')

    # Get cell size
    CellSize = XYZ(*[float(item) for item in File.readline().split()])
    
    # Get the domain angle
    Alpha = float(File.readline())
    
    # Store the cell size in terms of X and Y vectors
    X = XYZ(CellSize.x, 0, 0)
    # Initially create the Y vector along the X axis
    Y = XYZ(CellSize.y, 0, 0)
    # Then rotate the vector by Alpha
    Rotation = WXYZ(Alpha, 0, 0)
    Y = Rotation * Y

    # Get number of yarns
    NumYarnTypes = int(File.readline())

    # Create dictionary that will store yarn types
    YarnTypes = {}

    # Read yarn types
    for i in range(NumYarnTypes):
        # Get Yarn number
        YarnTypeNum = int(File.readline())
        # Use yarn number as index to YarnTypes dictionary and create new entry
        YarnTypes[YarnTypeNum] = {}
        # Store the information read from the file in a subdictionary
        YarnTypes[YarnTypeNum]['YarnName'] = File.readline()
        YarnTypes[YarnTypeNum]['Shape'] = File.readline()
        YarnTypes[YarnTypeNum]['FibreName'] = File.readline()
        YarnTypes[YarnTypeNum]['FibreData'] = File.readline()
        YarnTypes[YarnTypeNum]['EFF'] = File.readline()

    # Get the total number of sections
    TotalNumSections = int(File.readline())

    # Store the information about the yarns
    YarnInfo = []

    # Continue while all yarns are read
    while 1:
        # Get the yarn type
        Line = File.readline()
        # If the Line is empty then must be the end of the file
        if Line == '':
            break;
        # Otherwise its the yarn type number
        YarnTypeNum = int(Line)
        # Save the yarn type number
        CurrentYarnInfo = {'YarnType':YarnTypeNum}
        # Create a list to store the points in
        CurrentYarnInfo['Points'] = []
        # Get the number of sections in the yarn
        NumSegments = File.readline()
        # Continue while all points in a yarn are read
        while 1:
            Line = File.readline()
            # If a star is found this indicates the end of the yarn
            if Line == '*\n':
                # Add the current yarn information to the list of yarn information
                YarnInfo.append(CurrentYarnInfo)
                break;
            # Read in information about the point on a yarn centerline
            Coords = Line.split()
            Ls = File.readline()
            A1, A2 = File.readline().split()
            Ks = File.readline()
            CS13 = File.readline().split()
            CS46 = File.readline().split()
            CS79 = File.readline().split()
            Vf = File.readline()
            vy = File.readline()

            # Store the information that is needed for TexGen in
            # PointInfo and ignore the rest
            PointInfo = {}
            PointInfo['Dimensions'] = (float(A1), float(A2)) 
            PointInfo['Pos'] = XYZ(*[float(item) for item in Coords])
            PointInfo['Tangent'] = XYZ(*[float(item) for item in CS79])
            PointInfo['Up'] = XYZ(*[float(item) for item in CS46])
            CurrentYarnInfo['Points'].append(PointInfo)

    # All necessary information has been read
    # Reconstruct the final point on each yarn because it is missing
#    for CurrentYarnInfo in YarnInfo:
        # First find out if the yarn is repeated in the X direction
        # or the Y direction
#        StartPoint = CurrentYarnInfo['Points'][0]['Pos']
#        EndPoint = CurrentYarnInfo['Points'][-1]['Pos']
        # Add a copy of the point at the start to the end
#        CurrentYarnInfo['Points'].append(CurrentYarnInfo['Points'][0].copy())
#        Position = CurrentYarnInfo['Points'][-1]['Pos']
        # Now we will adjust its position
        # Note that it is necessary to create a new XYZ because the original
        # XYZ is shared with the first point despite the fact that the copy
        # function was called
#        if (DotProduct(EndPoint-StartPoint, X)/GetLength(X) > DotProduct(EndPoint-StartPoint, Y)/GetLength(Y)):
            # Direction is along the X axis
#            CurrentYarnInfo['Points'][-1]['Pos'] = XYZ(Position) + X
#        else:
            # Direction is along the Y axis
#            CurrentYarnInfo['Points'][-1]['Pos'] = XYZ(Position) + Y

    # Now Create the model...

    # Create textile
    Textile = CTextile()
    
    # Create a square section
    SquareSection = CSectionPolygon((XY(0.5, 0), XY(0.5, 0.5), XY(0, 0.5)), True) 

    for CurrentYarnInfo in YarnInfo:
        # Create a yarn
        Yarn = CYarn()
        # And a yarn section where section is specified at each node
        YarnSection = CYarnSectionInterpNode()
        # Get yarn shape
        Shape = YarnTypes[CurrentYarnInfo['YarnType']]['Shape']
        for Point in CurrentYarnInfo['Points']:
            # Add the nodes to the yarn
            Yarn.AddNode(CNode(Point['Pos'], Point['Tangent'], Point['Up']))
            if Shape[0] == 'L':
                # Lenticular section
                Section = CSectionLenticular(*Point['Dimensions'])
            elif Shape[0] == 'E':
                # Elliptical section
                Section = CSectionEllipse(*Point['Dimensions'])
            elif Shape[0] == 'R':
                # Rectangular section
                Section = CSectionScaled(SquareSection, XY(*Point['Dimensions']))
            else:
                print('Unknown shape found: ', Shape)
                print('Using ellipse instead...')
                Section = CSectionEllipse(*Point['Dimensions'])
            # Add section to the yarn section
            YarnSection.AddSection(Section)

        # Bezier interpolation allows for the master node tangent to be specified
        Yarn.AssignInterpolation(CInterpolationBezier())
        # Temporarily assign ellipse
        Yarn.AssignSection(YarnSection)
        # Set the resolution of the surface mesh
        Yarn.SetResolution(20)
        # Add the repeats to the yarn
        Yarn.AddRepeat(X)
        Yarn.AddRepeat(Y)
        # Add the yarn to the textile
        Textile.AddYarn(Yarn)

    # Create domain
#    Domain = CDomainPlanes(XYZ(0,0,-0.5*CellSize.z), XYZ(CellSize.x,CellSize.y,0.5*CellSize.z))
    Domain = CDomainPlanes();
    NormalX = CrossProduct(Y, XYZ(0,0,1))
    Normalise(NormalX)
    NormalY = XYZ(0, 1, 0)
    Domain.AddPlane(PLANE(NormalX, -0.5*DotProduct(NormalX, X)))
    Domain.AddPlane(PLANE(-NormalX, -0.5*DotProduct(NormalX, X)))
    Domain.AddPlane(PLANE(NormalY, -0.5*DotProduct(NormalY, Y)))
    Domain.AddPlane(PLANE(-NormalY, -0.5*DotProduct(NormalY, Y)))
    Domain.AddPlane(PLANE(XYZ(0, 0, 1), -0.5*CellSize.z))
    Domain.AddPlane(PLANE(XYZ(0, 0, -1), -0.5*CellSize.z))

    # Add domain to textile
    Textile.AssignDomain(Domain)

    return AddTextile(Textile)
