/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2016 Louise Brown

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
=============================================================================*/

#include "VoxelExportTests.h"
#include "TestUtilities.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CVoxelExportTests);

void CVoxelExportTests::setUp()
{
}

void CVoxelExportTests::tearDown()
{
	TEXGEN.DeleteTextiles();
}

void CVoxelExportTests::TestContinuumExport()
{
	CTextile Textile = m_TextileFactory.GetSingleYarn(3, 20);
	CRectangularVoxelMesh Vox("CPeriodicBoundaries");
	// Save yarns and matrix
	Vox.SaveVoxelMesh(Textile,"VoxelContinuumTest",10,10,10,true,true, MATERIAL_CONTINUUM );
	// Compare to template file
	CPPUNIT_ASSERT(CompareFiles("VoxelContinuumTest.inp","..\\..\\UnitTests\\VoxelContinuumTest.inp"));
}

void CVoxelExportTests::TestRotatedExport()
{
	CTextile Textile = m_TextileFactory.GetSingleYarn(3, 20);
	Textile.Rotate(WXYZ(XYZ(0,0,1), PI/4.0));
	CDomain* Domain = Textile.GetDomain();
	Domain->Rotate(WXYZ(XYZ(0,0,1), PI/4.0));

	CRotatedVoxelMesh Vox("CRotatedPeriodicBoundaries");
	// Save yarns and matrix
	Vox.SaveVoxelMesh(Textile,"RotatedVoxelMeshTest",10,10,10,true,true, ROTATED_BC );
	// Compare to template file
	CPPUNIT_ASSERT(CompareFiles("RotatedVoxelMeshTest.inp","..\\..\\UnitTests\\RotatedVoxelMeshTest.inp"));
}

void CVoxelExportTests::TestOctreeExport()
{
	CTextile Textile = m_TextileFactory.GetSingleYarn(3, 20);

	COctreeVoxelMesh Vox("CPeriodicBoundaries");
	// Save yarns and matrix
	Vox.SaveVoxelMesh(Textile,"OctreeVoxelMeshTest", 5, 6, true, 10, 0.3, 0.3, false, false );
	// Compare to template file
	CPPUNIT_ASSERT(CompareFiles("OctreeVoxelMeshTest.inp","..\\..\\UnitTests\\OctreeVoxelMeshTest.inp"));
}