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

class CPropertyTests : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(CPropertyTests);
	CPPUNIT_TEST(TestArealDensityUnits);
	CPPUNIT_TEST(TestGeometryScale);
	CPPUNIT_TEST(TestLinearDensityUnits);
	CPPUNIT_TEST(TestVolumeFraction);
	CPPUNIT_TEST(TestVolumeFraction2);
	CPPUNIT_TEST(TestVolumeFraction3);
	CPPUNIT_TEST(TestVolumeFraction4);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

protected:
	void TestArealDensityUnits();
	void TestGeometryScale();
	void TestLinearDensityUnits();
	void TestVolumeFraction();
	void TestVolumeFraction2();
	void TestVolumeFraction3();
	void TestVolumeFraction4();

	CTextileFactory m_TextileFactory;
};













