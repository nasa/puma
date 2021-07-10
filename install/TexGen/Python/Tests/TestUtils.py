from TexGen.Core import *

def GetTestTextile():
    Textile = CTextile()
    Yarn = CYarn()
    Yarn.AddNode(CNode(XYZ(0, 0, 0)))
    Yarn.AddNode(CNode(XYZ(0, 1, 0)))
    Yarn.AssignInterpolation(CInterpolationBezier())
    Yarn.AssignSection(CYarnSectionConstant(CSectionEllipse(1, 1)))
    Yarn.SetResolution(20)
    Yarn.AddRepeat(XYZ(0, 1, 0))
    Textile.AddYarn(Yarn)
    Domain = CDomainPlanes(XYZ(-1,-1,-1), XYZ(1,1,1))
    Textile.AssignDomain(Domain)
    return Textile

def GetTestWeave():
    weave = CTextileWeave2D(2, 2, 1, 0.2, True)
    weave.SwapPosition(0, 0)
    weave.SwapPosition(1, 1)
    weave.AssignDefaultDomain()
    return weave
