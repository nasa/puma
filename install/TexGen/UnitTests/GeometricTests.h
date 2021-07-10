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

class CGeometricTests : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(CGeometricTests);
	CPPUNIT_TEST(TestPointInsideYarn);
	CPPUNIT_TEST(TestLenticularSection);
	CPPUNIT_TEST(TestHybridQuarterSection);
	CPPUNIT_TEST(TestHybridHalfSection);
	CPPUNIT_TEST(TestQuaternionRotation);
	CPPUNIT_TEST(TestConvertRotation);
	CPPUNIT_TEST(TestGetClosestPointFunctions);
	CPPUNIT_TEST(Test3DGetClosestPointFunctions);
	CPPUNIT_TEST(TestFindClosestSurfacePoint);
	CPPUNIT_TEST(TestRotateYarn);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

protected:
	void TestPointInsideYarn();
	void TestLenticularSection();
	void TestHybridQuarterSection();
	void TestHybridHalfSection();
	void TestQuaternionRotation();
	void TestConvertRotation();

	double GetDistanceFromEdge(XYZ Point);

	void TestGetClosestPointFunctions();
	void Test3DGetClosestPointFunctions();
	void TestFindClosestSurfacePoint();

	void TestRotateYarn();

	CTextileFactory m_TextileFactory;
};
