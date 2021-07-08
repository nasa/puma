/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2017 Louise Brown

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

#include "PatternDraftTests.h"
#include "TestUtilities.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CPatternDraftTests);

void CPatternDraftTests::setUp()
{
}

void CPatternDraftTests::tearDown()
{
	TEXGEN.DeleteTextiles();
}

void CPatternDraftTests::Test2DTextileWeavePattern()
{
	CTextileWeave2D Textile = m_TextileFactory.SatinWeave();

	Textile.ConvertToPatternDraft();

	CPatternDraft PatternDraft = Textile.GetPatternDraft();
	vector<string> WeavePattern = PatternDraft.GetWeavePattern();

	std::vector<string> TestPattern;
	TestPattern.push_back("10");
	TestPattern.push_back("01");
	TestPattern.push_back("01");
	TestPattern.push_back("10");

	// Compare to template file
	CPPUNIT_ASSERT(WeavePattern == TestPattern);
}

void CPatternDraftTests::TestLayerToLayerWeavePatterns()
{
	CTextileLayerToLayer Textile = m_TextileFactory.LayerToLayerWeave();

	TestLayerToLayerWeavePattern( Textile, BOTTOM_TO_TOP );
	TestLayerToLayerWeavePattern( Textile, TOP_TO_BOTTOM );
	TestLayerToLayerWeavePattern( Textile, ALTERNATE_WEFT_STACK );
}

void CPatternDraftTests::TestLayerToLayerWeavePattern( CTextileLayerToLayer& Textile, int WeftOrder )
{
	Textile.ConvertToPatternDraft( WeftOrder );

	CPatternDraft PatternDraft = Textile.GetPatternDraft();
	vector<string> WeavePattern = PatternDraft.GetWeavePattern();

	// Compare to template file
	std::vector<string> TestPattern;
	ReadPatternFromFile( TestPattern, WeftOrder );
	CPPUNIT_ASSERT(WeavePattern == TestPattern);
}

void CPatternDraftTests::ReadPatternFromFile( std::vector<string>& TestPattern, int WeftOrder )
{
	ifstream File;
	switch ( WeftOrder )
	{
		case BOTTOM_TO_TOP:
			File.open("..\\..\\UnitTests\\Test3dLayerToLayerPatternBToT.txt");
			break;
		case TOP_TO_BOTTOM:
			File.open("..\\..\\UnitTests\\Test3dLayerToLayerPatternTToB.txt");
			break;
		case ALTERNATE_WEFT_STACK:
			File.open("..\\..\\UnitTests\\Test3dLayerToLayerPatternAlternating.txt");
			break;
	}

	string Line;
	if ( File.is_open() )
	{
		while( getline( File, Line ) )
		{
			TestPattern.push_back( Line );
		}
	}
}