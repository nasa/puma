# Create a 4x4 satin weave with yarn spacing of 1 and thickness of 0.2
weave = CTextileWeave2D(4, 4, 1, 0.2, 1, False)
weave.SetGapSize(0)

# Set the weave pattern
weave.SwapPosition(0, 3)
weave.SwapPosition(1, 2)
weave.SwapPosition(2, 1)
weave.SwapPosition(3, 0)
# Adjust the yarn widths and heights
weave.SetYarnWidths(0.8)
weave.SetYarnHeights(0.1)

# Assign the domain and get its limits
weave.AssignDefaultDomain()
Domain = weave.GetDefaultDomain()
Min = XYZ()
Max = XYZ()
Domain.GetBoxLimits( Min, Max )

# Create a new textile to contain the multiple layers
LayeredTextile = CTextile()

# Get the yarns from the first textile and add to the layered textile
Yarns = weave.GetYarns()
for Yarn in Yarns:
    LayeredTextile.AddYarn(Yarn)

# Create 2nd textile: Plain weave, spacing of 1 and thickness 0.25
weave1 = CTextileWeave2D(2, 2, 1, 0.25, 1, False)
weave1.SetGapSize(0)
weave1.SetYarnWidths(0.8)
weave1.SwapPosition(0, 1)
weave1.SwapPosition(1, 0)
weave1.SetXYarnWidths(0, 1.0)
weave1.SetXYarnHeights(0, 0.15)
weave1.SetXYarnSpacings(0, 1)
weave1.SetXYarnWidths(1, 0.8)
weave1.SetXYarnHeights(1, 0.1)
weave1.SetXYarnSpacings(1, 1)
weave1.SetYYarnWidths(0, 0.8)
weave1.SetYYarnHeights(0, 0.1)
weave1.SetYYarnSpacings(0, 1)
weave1.SetYYarnWidths(1, 1.0)
weave1.SetYYarnHeights(1, 0.15)
weave1.SetYYarnSpacings(1, 1)

# The yarns will be offset by the height of the first textile
Offset = XYZ(0.0,0.0,0.2)

# Get the yarns from the second textile
Yarns1 = weave1.GetYarns()
# For each yarn in the second textile..
for Yarn1 in Yarns1:
    # Get the nodes in the yarn
    Nodes = Yarn1.GetMasterNodes()
    i = 0
    # Offset each node
    for Node in Nodes:
        Node.SetPosition( Node.GetPosition() - Offset )
        Yarn1.ReplaceNode( i, Node )
        i += 1
    # and add to the layered textile
    LayeredTextile.AddYarn(Yarn1)

# Offset the domain to include the second textile
Min -= Offset
NewDomain = CDomainPlanes( Min, Max )
LayeredTextile.AssignDomain( NewDomain )

# Add the textile with the name "LayeredTextile"
AddTextile("LayeredTextile", LayeredTextile)


