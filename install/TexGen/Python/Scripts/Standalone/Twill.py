#! /usr/bin/env python

from TexGen.Core import *
from random import *

# Create a textile
Textile = CTextileWeave2D(4, 4, 3, 0.75);

Textile.SwapPosition(0, 0);
Textile.SwapPosition(1, 0);
Textile.SwapPosition(1, 1);
Textile.SwapPosition(2, 1);
Textile.SwapPosition(2, 2);
Textile.SwapPosition(3, 2);
Textile.SwapPosition(3, 3);
Textile.SwapPosition(0, 3);

Textile.SetYarnWidths(2.99);
Textile.SetYarnHeights(0.37);

# Add default domain to textile
Textile.AssignDefaultDomain()

# Add the textile to our TexGen singleton
AddTextile(Textile)

for TextileName, Textile in CTexGen.GetInstance().GetTextiles().iteritems():
    print TextileName
    for Yarn in Textile.GetYarns():
        # Get the Section Points
        for Node in Yarn.GetSlaveNodes(CYarn.SURFACE):
            for SectionPoint in Node.GetSectionPoints():
                print SectionPoint.x, SectionPoint.y, SectionPoint.z
        # Get the repeat limits for the yarns
        RepeatLimits = Textile.GetDomain().GetRepeatLimits(Yarn)
        Repeats = Yarn.GetRepeats()
        for Repeat, Limits in zip(Repeats, RepeatLimits):
            print "Repeat:", Repeat.x, Repeat.y, Repeat.z
            print "Limits:", Limits[0], Limits[1]
    # Get the domain size
    Min, Max = Textile.GetDomain().GetMesh().GetAABB()
    print "Domain Limits:"
    print "Minimum:", Min.x, Min.y, Min.z
    print "Maximum:", Max.x, Max.y, Max.z


# Get element orientations
Points = []  # Create a list (will be used to add points to)
seed()  # Seed the random number generator
for i in range(1000):
    # Generate some random points within the domain
    Points.append(XYZ(random()*12, random()*12, random()*4-1))

for TextileName, Textile in CTexGen.GetInstance().GetTextiles().iteritems():
    print TextileName
    PointsInfo = PointInfoVector()
    Textile.GetPointInformation(Points, PointsInfo)

    for Point, PointInfo in zip(Points, PointsInfo):
        print "Point:", Point.x, Point.y, Point.z
        print "Yarn ID:", PointInfo.iYarnIndex
        print "Tangent:", PointInfo.YarnTangent.x, PointInfo.YarnTangent.y, PointInfo.YarnTangent.z
    












