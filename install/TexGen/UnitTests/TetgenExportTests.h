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

#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TextileFactory.h"
#include <string>
using namespace std;

class CTetgenExportTests : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(CTetgenExportTests);
	CPPUNIT_TEST(TestPeriodicExport);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

protected:
	void TestPeriodicExport();

	CTextileFactory m_TextileFactory;
};