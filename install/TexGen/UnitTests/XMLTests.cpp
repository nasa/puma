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

#include "XMLTests.h"
#include "TestUtilities.h"


CPPUNIT_TEST_SUITE_REGISTRATION(CXMLTests);

void CXMLTests::setUp()
{
}

void CXMLTests::tearDown()
{
	TEXGEN.DeleteTextiles();
}

bool CXMLTests::TestOutput(string Prefix, OUTPUT_TYPE Type)
{
	// Set up some file names
	string OriginalFile = Prefix + "1.tg3";
	string CompareFile = Prefix + "2.tg3";

	// Save to xml file
	TEXGEN.SaveToXML(OriginalFile, "", Type);
	TEXGEN.DeleteTextiles();

	// Read it back in
	TEXGEN.ReadFromXML(OriginalFile);

	// Save it back to disk again
	TEXGEN.SaveToXML(CompareFile, "", Type);

	// Check the two files are the same
	return CompareFiles(OriginalFile, CompareFile);
}



void CXMLTests::TestTextile2DWeaveMin()
{
	TEXGEN.AddTextile(m_TextileFactory.SatinWeave());
	CPPUNIT_ASSERT(TestOutput("2dweavemin", OUTPUT_MINIMAL));
}
void CXMLTests::TestTextile2DWeaveStandard()
{
	TEXGEN.AddTextile(m_TextileFactory.SatinWeave());
	CPPUNIT_ASSERT(TestOutput("2dweavestan", OUTPUT_STANDARD));
}
void CXMLTests::TestTextile2DWeaveFull()
{
	TEXGEN.AddTextile(m_TextileFactory.SatinWeave());
	CPPUNIT_ASSERT(TestOutput("2dweavefull", OUTPUT_FULL));
}

void CXMLTests::TestTextile3DWeaveMin()
{
	TEXGEN.AddTextile(m_TextileFactory.Weave3D8x4());
	CPPUNIT_ASSERT(TestOutput("3dweavemin", OUTPUT_MINIMAL));
}
void CXMLTests::TestTextile3DWeaveStandard()
{
	TEXGEN.AddTextile(m_TextileFactory.Weave3D8x4());
	CPPUNIT_ASSERT(TestOutput("3dweavestan", OUTPUT_STANDARD));
}
void CXMLTests::TestTextile3DWeaveFull()
{
	TEXGEN.AddTextile(m_TextileFactory.Weave3D8x4());
	CPPUNIT_ASSERT(TestOutput("3dweavefull", OUTPUT_FULL));
}

void CXMLTests::TestYarnMin()
{
	TEXGEN.AddTextile(m_TextileFactory.InterpTest());
	CPPUNIT_ASSERT(TestOutput("yarnmin", OUTPUT_MINIMAL));
}
void CXMLTests::TestYarnStandard()
{
	TEXGEN.AddTextile(m_TextileFactory.InterpTest());
	CPPUNIT_ASSERT(TestOutput("yarnstan", OUTPUT_STANDARD));
}
void CXMLTests::TestYarnFull()
{
	TEXGEN.AddTextile(m_TextileFactory.InterpTest());
	CPPUNIT_ASSERT(TestOutput("yarnfull", OUTPUT_FULL));
}

void CXMLTests::TestSectionsMin()
{
	TEXGEN.AddTextile(m_TextileFactory.SectionsTest());
	CPPUNIT_ASSERT(TestOutput("sectionsmin", OUTPUT_MINIMAL));
}
void CXMLTests::TestSectionsStandard()
{
	TEXGEN.AddTextile(m_TextileFactory.SectionsTest());
	CPPUNIT_ASSERT(TestOutput("sectionsstan", OUTPUT_STANDARD));
}
void CXMLTests::TestSectionsFull()
{
	TEXGEN.AddTextile(m_TextileFactory.SectionsTest());
	CPPUNIT_ASSERT(TestOutput("sectionsfull", OUTPUT_FULL));
}

void CXMLTests::TestDomain()
{
	CTextile Textile;
	CDomainPlanes Domain(XYZ(0,0,0), XYZ(1,2,3));
	Textile.AssignDomain(Domain);
	TEXGEN.AddTextile(Textile);
	CPPUNIT_ASSERT(TestOutput("domain", OUTPUT_FULL));
}
/*
void CXMLTests::TestMeshing()
{
	CTextileWeave2D Text = m_TextileFactory.MeshedWeave();
	Text.SetResolution(8);
//	Text.OutputAbaqus("test.inp");
//	TEXGEN.AddTextile(m_TextileFactory.MeshedWeave());
//	CPPUNIT_ASSERT(TestOutput("meshedweave", OUTPUT_FULL));
}*/






















