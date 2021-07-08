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

#include "SolverTests.h"
#include "../Core/Elements.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CSolverTests);

void CSolverTests::setUp()
{
}

void CSolverTests::tearDown()
{
	TEXGEN.DeleteTextiles();
}

void CSolverTests::TestGeometrySolver()
{
	CGeometrySolver Solver;
	Solver.SetSeed(0.5);
//	Solver.SetPeriodic(true);
//	Solver.CreateDebugSystem();
//	CPPUNIT_ASSERT(Solver.CreateSystem(m_TextileFactory.PlainWeave()));
//	CPPUNIT_ASSERT(Solver.CreateSystem(m_TextileFactory.Weave3D8x4()));
//	CPPUNIT_ASSERT(Solver.CreateSystem(m_TextileFactory.SatinWeave()));
    CTextileWeave2D Weave = m_TextileFactory.SatinWeave();
	CPPUNIT_ASSERT(Solver.CreateSystem(Weave));
	CPPUNIT_ASSERT(Solver.SolveSystem() != 0);
	Solver.SaveToVTK("GeomSolve");
}

void CSolverTests::TestFineGridSolve()
{
	CGeometrySolver Solver;
	Solver.SetSeed(0.2);
//	Solver.SetPeriodic(true);
//	Solver.CreateDebugSystem();
//	CPPUNIT_ASSERT(Solver.CreateSystem(m_TextileFactory.PlainWeave()));
//	CPPUNIT_ASSERT(Solver.CreateSystem(m_TextileFactory.Weave3D8x4()));
    CTextileWeave2D Weave = m_TextileFactory.SatinWeave();
	CPPUNIT_ASSERT(Solver.CreateSystem(Weave));
	CPPUNIT_ASSERT(Solver.SolveSystem() != 0);
	Solver.SaveToVTK("GeomSolve2");
}

void CSolverTests::TestPlateElementArea()
{
	CMatrix P(3, 2);
	P(0,0) = 1;
	P(0,1) = 1;
	P(1,0) = 2;
	P(1,1) = 1;
	P(2,0) = 0;
	P(2,1) = 3;
	CElementTriBending PlateElement(P, 3);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, PlateElement.GetArea(), 1e-12);
}

void CSolverTests::TestPlateElement()
{
	CMatrix P(3, 2);
	P(0,0) = 0.1;
	P(0,1) = 0.2;
	P(1,0) = 1.3;
	P(1,1) = 0.4;
	P(2,0) = 0.5;
	P(2,1) = 1.6;
	CElementTriBending PlateElement(P, 3);
	CMatrix Ke;
	PlateElement.GetKeMatrix(Ke);
	// This solution obtained from Python element test using scipy
	CMatrix Solution(10, 10);
	Solution(0, 0) = 53.849121093749694;
	Solution(0, 1) = 11.441927083333258;
	Solution(0, 2) = 13.556510416666603;
	Solution(0, 3) = 46.286132812499744;
	Solution(0, 4) = -11.28066406249993;
	Solution(0, 5) = 9.5303710937499595;
	Solution(0, 6) = 41.364257812499787;
	Solution(0, 7) = 3.2160807291666571;
	Solution(0, 8) = -14.787662760416584;
	Solution(0, 9) = -141.49951171874926;
	Solution(1, 0) = 11.441927083333258;
	Solution(1, 1) = 3.3141666666666487;
	Solution(1, 2) = 2.5047916666666508;
	Solution(1, 3) = 7.7778645833332725;
	Solution(1, 4) = -1.8931249999999835;
	Solution(1, 5) = 1.7657291666666568;
	Solution(1, 6) = 8.6169270833332785;
	Solution(1, 7) = 0.64499999999999735;
	Solution(1, 8) = -3.0435416666666457;
	Solution(1, 9) = -27.836718749999815;
	Solution(2, 0) = 13.556510416666603;
	Solution(2, 1) = 2.5047916666666508;
	Solution(2, 2) = 4.3855208333333202;
	Solution(2, 3) = 12.847526041666613;
	Solution(2, 4) = -3.1965104166666514;
	Solution(2, 5) = 2.3432031249999921;
	Solution(2, 6) = 9.5889322916666213;
	Solution(2, 7) = 0.6353645833333309;
	Solution(2, 8) = -3.5264843749999821;
	Solution(2, 9) = -35.992968749999847;
	Solution(3, 0) = 46.286132812499744;
	Solution(3, 1) = 7.7778645833332725;
	Solution(3, 2) = 12.847526041666613;
	Solution(3, 3) = 54.874511718749787;
	Solution(3, 4) = -15.268554687499941;
	Solution(3, 5) = 9.2133789062499698;
	Solution(3, 6) = 41.723144531249829;
	Solution(3, 7) = 1.1567057291666605;
	Solution(3, 8) = -15.243522135416601;
	Solution(3, 9) = -142.8837890624994;
	Solution(4, 0) = -11.28066406249993;
	Solution(4, 1) = -1.8931249999999835;
	Solution(4, 2) = -3.1965104166666514;
	Solution(4, 3) = -15.268554687499941;
	Solution(4, 4) = 4.9321874999999835;
	Solution(4, 5) = -2.2803124999999902;
	Solution(4, 6) = -11.182226562499949;
	Solution(4, 7) = -0.36734374999999775;
	Solution(4, 8) = 4.0622135416666474;
	Solution(4, 9) = 37.731445312499822;
	Solution(5, 0) = 9.5303710937499595;
	Solution(5, 1) = 1.7657291666666568;
	Solution(5, 2) = 2.3432031249999921;
	Solution(5, 3) = 9.2133789062499698;
	Solution(5, 4) = -2.2803124999999902;
	Solution(5, 5) = 2.7669531249999952;
	Solution(5, 6) = 6.2127929687499757;
	Solution(5, 7) = 0.28356770833333295;
	Solution(5, 8) = -2.3405859374999904;
	Solution(5, 9) = -24.956542968749908;
	Solution(6, 0) = 41.364257812499787;
	Solution(6, 1) = 8.6169270833332785;
	Solution(6, 2) = 9.5889322916666213;
	Solution(6, 3) = 41.723144531249829;
	Solution(6, 4) = -11.182226562499949;
	Solution(6, 5) = 6.2127929687499757;
	Solution(6, 6) = 40.812011718749851;
	Solution(6, 7) = 2.2524088541666587;
	Solution(6, 8) = -15.203483072916608;
	Solution(6, 9) = -123.89941406249946;
	Solution(7, 0) = 3.2160807291666571;
	Solution(7, 1) = 0.64499999999999735;
	Solution(7, 2) = 0.6353645833333309;
	Solution(7, 3) = 1.1567057291666605;
	Solution(7, 4) = -0.36734374999999775;
	Solution(7, 5) = 0.28356770833333295;
	Solution(7, 6) = 2.2524088541666587;
	Solution(7, 7) = 0.96677083333333325;
	Solution(7, 8) = -0.77020833333333072;
	Solution(7, 9) = -6.6251953124999776;
	Solution(8, 0) = -14.787662760416584;
	Solution(8, 1) = -3.0435416666666457;
	Solution(8, 2) = -3.5264843749999821;
	Solution(8, 3) = -15.243522135416601;
	Solution(8, 4) = 4.0622135416666474;
	Solution(8, 5) = -2.3405859374999904;
	Solution(8, 6) = -15.203483072916608;
	Solution(8, 7) = -0.77020833333333072;
	Solution(8, 8) = 6.0754947916666442;
	Solution(8, 9) = 45.234667968749797;
	Solution(9, 0) = -141.49951171874926;
	Solution(9, 1) = -27.836718749999815;
	Solution(9, 2) = -35.992968749999847;
	Solution(9, 3) = -142.8837890624994;
	Solution(9, 4) = 37.731445312499822;
	Solution(9, 5) = -24.956542968749908;
	Solution(9, 6) = -123.89941406249946;
	Solution(9, 7) = -6.6251953124999776;
	Solution(9, 8) = 45.234667968749797;
	Solution(9, 9) = 408.28271484374818;
	int i, j;
	stringstream ErrorMessage;
	for (i=0; i<10; ++i)
	{
		for (j=0; j<10; ++j)
		{
			ErrorMessage.clear();
			ErrorMessage << "Error in element " << i << ", " << j << " of stiffness matrix" << endl;
			CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(ErrorMessage.str(), Solution(i, j), Ke(i, j), 1e-10);
		}
	}
}














