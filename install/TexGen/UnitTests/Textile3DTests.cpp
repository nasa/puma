/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2018 Louise Brown

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

#include "Textile3DTests.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CTextile3DTests);

void CTextile3DTests::setUp()
{
}

void CTextile3DTests::tearDown()
{
	TEXGEN.DeleteTextiles();
}

void CTextile3DTests::TestGetXYarnIndex()
{
	CTextileLayerToLayer Textile = m_TextileFactory.LayerToLayerWeave();

	CTextile3DWeave* Weave = Textile.Get3DWeave();

	CPPUNIT_ASSERT( Weave->GetXYarnIndex(0) == 0 );
	CPPUNIT_ASSERT( Weave->GetXYarnIndex(3) == 1 );
	CPPUNIT_ASSERT( Weave->GetXYarnIndex(6) == -1 );
}

