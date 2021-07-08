# Create a textile
Textile = CTextile()

# Create a lenticular section
Section = CSectionLenticular(0.8, 0.2)

# Create 4 yarns
Yarns = (CYarn(), CYarn(), CYarn(), CYarn())

# Add nodes to the yarns to describe their paths
Yarns[0].AddNode(CNode(XYZ(0, 0, 0)))
Yarns[0].AddNode(CNode(XYZ(1, 0.1, 0.2)))
Yarns[0].AddNode(CNode(XYZ(2, 0.2, 0)))

Yarns[1].AddNode(CNode(XYZ(0.1, 1, 0.2)))
Yarns[1].AddNode(CNode(XYZ(1.1, 1.1, 0)))
Yarns[1].AddNode(CNode(XYZ(2.1, 1.2, 0.2)))

Yarns[2].AddNode(CNode(XYZ(0, 0, 0.2)))
Yarns[2].AddNode(CNode(XYZ(0.1, 1, 0)))
Yarns[2].AddNode(CNode(XYZ(0.2, 2, 0.2)))

Yarns[3].AddNode(CNode(XYZ(1, 0.1, 0)))
Yarns[3].AddNode(CNode(XYZ(1.1, 1.1, 0.2)))
Yarns[3].AddNode(CNode(XYZ(1.2, 2.1, 0)))

# We want the same interpolation and section shape for all the yarns so loop over them all
for Yarn in Yarns:
    # Set the interpolation function
    Yarn.AssignInterpolation(CInterpolationCubic())
    # Assign a constant cross-section all along the yarn
    Yarn.AssignSection(CYarnSectionConstant(Section))
    # Set the resolution
    Yarn.SetResolution(20)
    # Add repeats to the yarn
    Yarn.AddRepeat(XYZ(2, 0.2, 0))
    Yarn.AddRepeat(XYZ(0.2, 2, 0))
    # Add the yarn to our textile
    Textile.AddYarn(Yarn)

# Create a domain and assign it to the textile
Domain = CDomainPlanes()
Repeat = XYZ(0.2, 2, 0)
PlaneNormal = XYZ(-0.1, 1, 0)
d = DotProduct(Repeat, PlaneNormal)/GetLength(PlaneNormal)
Domain.AddPlane(PLANE(XYZ(1, -0.1, 0), 0))
Domain.AddPlane(PLANE(XYZ(-1, 0.1, 0), -d))
Domain.AddPlane(PLANE(XYZ(-0.1, 1, 0), 0))
Domain.AddPlane(PLANE(XYZ(0.1, -1, 0), -d))
Domain.AddPlane(PLANE(XYZ(0, 0, 1), -0.1))
Domain.AddPlane(PLANE(XYZ(0, 0, -1), -0.3))
Textile.AssignDomain(Domain);

# Add the textile
AddTextile(Textile)











