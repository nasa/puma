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

#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TextileFactory.h"
#include <string>
using namespace std;

class CXMLTests : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(CXMLTests);
	CPPUNIT_TEST(TestTextile2DWeaveMin);
	CPPUNIT_TEST(TestTextile2DWeaveStandard);
	CPPUNIT_TEST(TestTextile2DWeaveFull);
	CPPUNIT_TEST(TestTextile3DWeaveMin);
	CPPUNIT_TEST(TestTextile3DWeaveStandard);
//	CPPUNIT_TEST(TestTextile3DWeaveFull);	This Takes a long time so it has been removed
	CPPUNIT_TEST(TestYarnMin);
	CPPUNIT_TEST(TestYarnStandard);
	CPPUNIT_TEST(TestYarnFull);
	CPPUNIT_TEST(TestSectionsMin);
	CPPUNIT_TEST(TestSectionsStandard);
	CPPUNIT_TEST(TestSectionsFull);
	CPPUNIT_TEST(TestDomain);
//	CPPUNIT_TEST(TestMeshing);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

protected:
	void TestTextile2DWeaveMin();
	void TestTextile2DWeaveStandard();
	void TestTextile2DWeaveFull();
	void TestTextile3DWeaveMin();
	void TestTextile3DWeaveStandard();
	void TestTextile3DWeaveFull();
	void TestYarnMin();
	void TestYarnStandard();
	void TestYarnFull();
	void TestSectionsMin();
	void TestSectionsStandard();
	void TestSectionsFull();
	void TestDomain();
//	void TestMeshing();

	bool TestOutput(string Prefix, OUTPUT_TYPE Type);
	
	CTextileFactory m_TextileFactory;
};
