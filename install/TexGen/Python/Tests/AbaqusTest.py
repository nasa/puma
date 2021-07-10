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

import unittest
from math import pi
from TestUtils import GetTestWeave
from TexGen.Abaqus import TextileDeformerAbaqus
from TexGen.Core import *

class AbaqusTest(unittest.TestCase):
    def setUp(self):
        AddTextile('Textile', GetTestWeave())

    def tearDown(self):
        DeleteTextiles()

    def testAbaqus(self):
        shear = CLinearTransformation()
        shear.AddSheardYbydX(0.1)
        shear.AddSheardXbydY(0.1)

        tension = CLinearTransformation()
        tension.Scale(1.1)

        rotation = CLinearTransformation()
        rotation.Rotate(WXYZ(pi/8, 0, 0))

        deformer = TextileDeformerAbaqus()
        deformer.AddDeformationStep(shear)
        deformer.AddDeformationStep(shear*tension)
        deformer.AddDeformationStep(rotation)
        deformer.SetFortranProgram('Data/LinearElasticUMAT.for')
        deformer.AddMaterial("ISO_WARP", [1.0, 0.0])
        deformer.AddMaterial("ISO_WEFT", [2.0, 0.0])
        deformer.AssignMaterial('ISO_WARP', [0, 1])
        deformer.AssignMaterial('ISO_WEFT', [2, 3])
        deformer.SetSimulationFilesPrefix('AbaqusTest')

        textile = GetTextile()
        deformer.DeformTextile(textile)


def GetTestSuite():
    suite = unittest.TestLoader().loadTestsFromTestCase(AbaqusTest)
    return suite
