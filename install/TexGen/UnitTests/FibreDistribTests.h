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

class CFibreDistribTests : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(CFibreDistribTests);
	CPPUNIT_TEST(TestInside);
	CPPUNIT_TEST(TestOutside);
	CPPUNIT_TEST(TestConst);
	CPPUNIT_TEST(TestQuad);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

protected:
	void TestInside();
	void TestOutside();
	void TestConst();
	void TestQuad();

	double GetAverageVF(CFibreDistribution &FibreDistrib, CSection &Section);
	bool PointInsideSection(const vector<XY> &Points, XY Point);
	void GetBoundingBox(const vector<XY> &Points, XY &Min, XY &Max);

	CTextileFactory m_TextileFactory;
};
