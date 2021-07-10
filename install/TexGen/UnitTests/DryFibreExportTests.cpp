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

#include "DryFibreExportTests.h"
#include "TestUtilities.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CDryFibreExportTests);

void CDryFibreExportTests::setUp()
{
}

void CDryFibreExportTests::tearDown()
{
	TEXGEN.DeleteTextiles();
}

void CDryFibreExportTests::TestOrthogonalWholeSurface()
{
	CTextileOrthogonal Textile = m_TextileFactory.OrthogonalWeave();

	CSimulationAbaqus Deformer = CSimulationAbaqus();
	CLinearTransformation Tension = CLinearTransformation();
	Tension.AddScale(1.0, 1.0, 1.0);
	Deformer.AddDeformationStep(Tension);
	
	Deformer.SetWholeSurfaces( true );
	Deformer.CreateAbaqusInputFile(Textile, "DryFibreWholeSurfaceTest", false );
	
	// Compare to template file
	CPPUNIT_ASSERT(CompareFiles("DryFibreWholeSurfaceTest.inp","..\\..\\UnitTests\\DryFibreWholeSurfaceTest.inp"));
}

void CDryFibreExportTests::TestOrthogonalULSurface()
{
	CTextileOrthogonal Textile = m_TextileFactory.OrthogonalWeave();

	CSimulationAbaqus Deformer = CSimulationAbaqus();
	CLinearTransformation Tension = CLinearTransformation();
	Tension.AddScale(1.0, 1.0, 1.0);
	Deformer.AddDeformationStep(Tension);
	
	Deformer.CreateAbaqusInputFile(Textile, "DryFibreULSurfaceTest", false );
	
	// Compare to template file
	CPPUNIT_ASSERT(CompareFiles("DryFibreULSurfaceTest.inp","..\\..\\UnitTests\\DryFibreULSurfaceTest.inp"));
}