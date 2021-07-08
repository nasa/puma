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
import re

def GetNextLine(File):
    """Get the next line in a file ignoring lines beginning with *
    a space or empty lines. If the end of file is reached None is returned
    otherwise a string with the line will be returned."""
    while 1:
        Line = File.readline()
        if len(Line) == 0:
            return None
        elif Line[0] == '*' or Line[0] == '\n' or Line[0] == ' ':
            continue
        else:
            return Line

def ImportTexGenv2(filename):
    """ Import a file from TexGen version 2, note that some assumptions are
    made when importing from this version. The cross section is assumed constant
    all along the yarn length. It is also assumed that no scaling, rotation
    or squashing has been imposed. If this is not the case the imported
    model will not conform to the one in TexGen version 2.
    Returns the name of the textile imported."""

    # Open the file
    File = open(filename, 'r')
    # Read the first line
    NumVectors = int(GetNextLine(File))
    # Read the second line
    FabricType = GetNextLine(File)
    # Read the third line
    Line = GetNextLine(File)
    L1, L2, D1x, D1y, D2x, D2y = [float(item) for item in Line.split(',')]
    
    # Variable to keep track of which section we are reading
    section = 'VECTORS'

    # Create the variables that will be used to save data read from the file
    Vectors = []
    SectionPoints = {}
    DomainSize = None

    # Read the vectors
    while 1:
        Line = GetNextLine(File)
        # Check if the end of the file has been reached
        if Line == None:
            break
        # Check if this is a section string i.e. for the form [XXXX]
        m = re.match(r'\[([A-Z]+)\]', Line)
        if m != None:
            # If it is then change the section variable to reflect this
            section = m.group(1)
        elif section == 'VECTORS':
            # We are reading the vectors section (the vectors section always comes first)
            VectorInfo = {}
            # Extract the relevent information from the file
            SplitLine = Line.split(',')
            ID, Type, X1, Y1, Z1, X2, Y2, Z2 = SplitLine[0:8]
            U1 = XYZ(0,0,1)
            U2 = XYZ(0,0,1)
            if len(SplitLine) >= 14:
                U1 = XYZ(*[float(x) for x in SplitLine[8:11]])
                U2 = XYZ(*[float(x) for x in SplitLine[11:14]])
            Line = GetNextLine(File)
            Chain, ChainOrder, Origin, End = [int(item) for item in Line.split(',')]
            Line = GetNextLine(File)
            NumNodes, Curv, IAngle, OAngle, PAngle = Line.split(',')
            while 1:
                # Current implementation just ignores the slave nodes
                Line = File.readline()
                if Line[0] == '*':
                    break
                # Take the ID of the slave node, assume it is the same for all nodes
                ID = Line.split(',')[5]
            # Save the vector information in the VectorInfo dictionary
            VectorInfo['Origin'] = XYZ(float(X1), float(Y1), float(Z1))
            VectorInfo['End'] = XYZ(float(X2), float(Y2), float(Z2))
            VectorInfo['Chain'] = int(Chain)
            VectorInfo['ChainOrder'] = int(ChainOrder)
            VectorInfo['ID'] = int(ID)
            VectorInfo['U1'] = U1
            VectorInfo['U2'] = U2
            # Add the information to the vectors array
            Vectors.append(VectorInfo)
        elif section == 'SECTIONS':
            # Read the cross section information
            # Get section index and number of points contained in the section
            SectionIndex = int(Line.split('\t', 1)[1].split(',')[0])
            NumPoints = int(Line.split('\t', 1)[1].split(',')[1])
            # Create an array of XY points
            SectionPoints[SectionIndex] = []
            for i in range(NumPoints):
                Line = GetNextLine(File).split(',')
                SectionPoints[SectionIndex].append(XY(float(Line[1]), float(Line[2])))
        elif section == 'DOMAIN':
            # Read the domain size information
            Min = XYZ(*[float(item) for item in Line.split(',')[0:3]])
            Max = XYZ(*[float(item) for item in Line.split(',')[3:6]])
            # Save it as a tuple
            DomainSize = (Min, Max)

    # Reorganise the data for easier processing, create a list of chains
    # each chain contains the vectors within it
    Chains = {}
    for Vector in Vectors:
        if Vector['Chain'] not in Chains:
            Chains[Vector['Chain']] = []
        Chains[Vector['Chain']].append(Vector)

    # Create polygon sections out of the list of points
    Sections = {}
    for Key, Section in list(SectionPoints.items()):
        Sections[Key] = CSectionPolygon(Section)

    # All necessary information has been read and re-organised
    # Now Create the model...

    # Create textile
    Textile = CTextile()

    # For each of the chains
    for Chain in list(Chains.values()):
        # Create a yarn
        Yarn = CYarn()
        # Add the master nodes
        Yarn.AddNode(CNode(Chain[0]['Origin'], XYZ(), Chain[0]['U1']))
        for Vector in Chain:
            # Add the nodes to the yarn
            Yarn.AddNode(CNode(Vector['End'], XYZ(), Vector['U2']))

        # TexGen 2 was based on Bezier interpolation so keep it that way
        Yarn.AssignInterpolation(CInterpolationBezier())
        # Assign section to the yarn assuming the section is constant throughout
        Yarn.AssignSection(CYarnSectionConstant(Sections[Chain[0]['ID']]))
        # Set the resolution of the surface mesh
        Yarn.SetResolution(20)
        # Add the repeats to the yarn
        Yarn.AddRepeat(XYZ(L1*D1x, L1*D1y, 0))
        Yarn.AddRepeat(XYZ(L2*D2x, L2*D2y, 0))
        # Add the yarn to the textile
        Textile.AddYarn(Yarn)

    # Create domain if it exists
    if DomainSize:
        # Create domain
        Domain = CDomainPlanes(*DomainSize)
        Textile.AssignDomain(Domain)

    return AddTextile(Textile)

