import unittest
import os
import pumapy as puma
from pumapy.utilities.logger import print_warning
pumadir = os.path.abspath('..')

try:
    from TexGen.Core import *

    if os.path.exists(pumadir + "/python/TexGen/install/lib"):
        class TestWeaveIO(unittest.TestCase):

            def test_weave_io(self):
                # Create a textile
                Textile = CTextile()

                # Create a python list containing 4 yarns
                Yarns = [CYarn(), CYarn(), CYarn(), CYarn()]

                # Add nodes to the yarns to describe their paths
                Yarns[0].AddNode(CNode(XYZ(0, 0, 0)))
                Yarns[0].AddNode(CNode(XYZ(0.22, 0, 0.05)))
                Yarns[0].AddNode(CNode(XYZ(0.44, 0, 0)))

                Yarns[1].AddNode(CNode(XYZ(0, 0.22, 0.05)))
                Yarns[1].AddNode(CNode(XYZ(0.22, 0.22, 0)))
                Yarns[1].AddNode(CNode(XYZ(0.44, 0.22, 0.05)))

                Yarns[2].AddNode(CNode(XYZ(0, 0, 0.05)))
                Yarns[2].AddNode(CNode(XYZ(0, 0.22, 0)))
                Yarns[2].AddNode(CNode(XYZ(0, 0.44, 0.05)))

                Yarns[3].AddNode(CNode(XYZ(0.22, 0, 0)))
                Yarns[3].AddNode(CNode(XYZ(0.22, 0.22, 0.05)))
                Yarns[3].AddNode(CNode(XYZ(0.22, 0.44, 0)))

                # Loop over all the yarns in the list
                for Yarn in Yarns:
                    # Set the interpolation function
                    Yarn.AssignInterpolation(CInterpolationCubic())

                    # Assign a constant cross-section all along the yarn of elliptical shape
                    Yarn.AssignSection(CYarnSectionConstant(CSectionEllipse(0.18, 0.04)))

                    # Set the resolution of the surface mesh created
                    Yarn.SetResolution(20)

                    # Add repeat vectors to the yarn
                    Yarn.AddRepeat(XYZ(0.44, 0, 0))
                    Yarn.AddRepeat(XYZ(0, 0.44, 0))

                    # Add the yarn to our textile
                    Textile.AddYarn(Yarn)

                self.assertEqual(puma.export_weave_vtu(os.path.join(pumadir, "test", "out", "weavetest"), Textile,
                                                       CDomainPlanes(XYZ(0, 0, -0.02), XYZ(0.44, 0.44, 0.07)), 100),
                                 os.path.join(pumadir, "test", "out", "weavetest_100_100_21"))

                ws = puma.import_weave_vtu(os.path.join(pumadir, "test", "out", "weavetest_100_100_21.vtu"))
                self.assertEqual(max(ws.matrix.shape), 100)

except:
    print_warning("Could not run test_weave_io because of failed TexGen import.")

if __name__ == '__main__':
    unittest.main()
