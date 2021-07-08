/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2006 Martin Sherburn

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

#include "MesherTests.h"
#include "TestUtilities.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CMesherTests);

void CMesherTests::setUp()
{
}

void CMesherTests::tearDown()
{
	TEXGEN.DeleteTextiles();
}

void CMesherTests::TestSimpleMesh()
{
	CTextileWeave2D Textile = m_TextileFactory.CottonWeave();
	Textile.SetFibreArea(0.01, "mm^2");

	CMesher Mesher;
	Mesher.SetPeriodic(true);
	Mesher.SetSeed(0.05);
	Mesher.SetMergeTolerance(0.02);
	CPPUNIT_ASSERT(Mesher.CreateMesh(Textile));
	Mesher.SaveVolumeMeshToVTK("vmesh.vtu");
	CPPUNIT_ASSERT(CompareFiles("vmesh.vtu","..\\..\\UnitTests\\vmesh.vtu"));
	Mesher.SaveVolumeMeshToABAQUS("vmesh.inp", Textile);
	CPPUNIT_ASSERT(CompareFiles("vmesh.inp","..\\..\\UnitTests\\vmesh.inp"));
}

void CMesherTests::TestInvertedElements()
{
	CTextileWeave2D Textile = m_TextileFactory.PlainWeave();

	CMesher Mesher;
	Mesher.SetHybrid(true);
	Mesher.SetSeed(0.1);
	Mesher.SetMergeTolerance(0.1);
	Mesher.CreateMesh(Textile);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Some tetrahedral elements are inverted", 0, Mesher.GetMesh().CountInvertedElements(CMesh::TET));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Some pyramid elements are inverted", 0, Mesher.GetMesh().CountInvertedElements(CMesh::PYRAMID));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Some wedge elements are inverted", 0, Mesher.GetMesh().CountInvertedElements(CMesh::WEDGE));
}

/*void CMesherTests::TestMatchingFaces()
{
	CTextileWeave2D Textile = m_TextileFactory.PlainWeave();
	CMesher Mesher;
	Mesher.SetHybrid(true);
	Mesher.SetSeed(0.1);
	Mesher.SetMergeTolerance(0.1);
	Mesher.CreateMesh(Textile);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Some faces are not matched up correctly", 0, Mesher.GetMesh().CountMismatchedFaces());
}*/













