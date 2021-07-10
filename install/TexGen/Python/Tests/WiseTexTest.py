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
from TestUtils import GetTestTextile
from TexGen.WiseTex import ImportWiseTex
from TexGen.Core import *

class ImportTest(unittest.TestCase):
    def tearDown(self):
        DeleteTextiles()

    def testImport(self):
        ImportWiseTex('Data/test.cfl')

class VerifyData(unittest.TestCase):
    def setUp(self):
        ImportWiseTex('Data/test.cfl')

    def tearDown(self):
        DeleteTextiles()

    def testNumYarns(self):
        self.assertEqual(GetTextile().GetNumYarns(), 4)

    def testVolume(self):
        textile = GetTextile()
        mesh = CMesh()
        textile.AddVolumeToMesh(mesh)
        self.assertAlmostEqual(mesh.CalculateVolume(), 7.6144607823548363)

def GetTestSuite():
    tests = []
    tests.append(unittest.TestLoader().loadTestsFromTestCase(ImportTest))
    tests.append(unittest.TestLoader().loadTestsFromTestCase(VerifyData))
    return unittest.TestSuite(tests)
