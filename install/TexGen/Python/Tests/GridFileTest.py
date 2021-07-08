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
from TexGen.GridFile import ExportGridFile
from TexGen.Core import *

class GridFileTest(unittest.TestCase):
    def setUp(self):
        AddTextile('Textile', GetTestTextile())

    def tearDown(self):
        DeleteTextiles()

    def testGridFile(self):
        ExportGridFile('test.grd', 'Textile', (40, 40))

def GetTestSuite():
    suite = unittest.TestLoader().loadTestsFromTestCase(GridFileTest)
    return suite








