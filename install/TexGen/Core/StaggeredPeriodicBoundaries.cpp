/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2010 Louise Brown

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

#include "PrecompiledHeaders.h"
#include "TexGen.h"
#include "StaggeredPeriodicBoundaries.h"


using namespace TexGen;
using namespace std;

CStaggeredPeriodicBoundaries::CStaggeredPeriodicBoundaries(int NumEdges, int NumVertices)
:CPeriodicBoundaries(NumEdges, NumVertices)
{
}

CStaggeredPeriodicBoundaries::~CStaggeredPeriodicBoundaries(void)
{
}

void CStaggeredPeriodicBoundaries::SetFaceD( vector<int>& D1, vector<int>& D2 )
{
	m_FaceD = make_pair( D1, D2 );
}

void CStaggeredPeriodicBoundaries::OutputFaceSets( ostream& Output )
{
	CPeriodicBoundaries::OutputFaceSets( Output );
	OutputSets( Output, m_FaceD.first, "FaceG" );
	OutputSets( Output, m_FaceD.second, "FaceH" );
}

void CStaggeredPeriodicBoundaries::OutputEquations( ostream& Output, int iBoundaryConditions )
{
	Output << "***************************" << endl;
	Output << "*** BOUNDARY CONDITIONS ***" << endl;
	Output << "***************************" << endl;

	Output << "*** Name: Translation stop Vertex 1 Type: Displacement/Rotation" << endl;
	Output << "*Boundary" << endl;
	Output << "MasterNode1, 1, 1" << endl;
	Output << "MasterNode1, 2, 2" << endl;
	Output << "MasterNode1, 3, 3" << endl;
	Output << endl;

	Output << "*****************" << endl;
	Output << "*** EQUATIONS ***" << endl;
	Output << "*****************" << endl;
	Output << "*parameter" << endl;
	Output << "***unit cell X dimension" << endl;
	Output << "BX=" << m_DomSize.x << endl;
	Output << "***Unit cel Y dimension " << endl;
	Output << "BY=" << m_DomSize.y << endl;
	Output << "***unit cell X offset1" << endl;
	Output << "OFF1=" << m_DomSize.x*m_Offset << endl;
	Output << "***unit cell X length - offset1" << endl;
	Output << "OFF2=" << m_DomSize.x*(1.0 - m_Offset) << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "FaceF,1,1.0,FaceE,1,-1.0,ConstraintsDriver0,1,-<BX>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "FaceF,2,1.0,FaceE,2,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "FaceF,3,1.0,FaceE,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "FaceD,1,1.0,FaceA,1,-1.0,ConstraintsDriver0,1,-<OFF1>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "FaceD,2,1.0,FaceA,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "FaceD,3,1.0,FaceA,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "FaceC,1,1.0,FaceB,1,-1.0,ConstraintsDriver0,1,<OFF2>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "FaceC,2,1.0,FaceB,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "FaceC,3,1.0,FaceB,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "Edge1,1,1.0,Edge2,1,-1.0,ConstraintsDriver0,1,-<BX>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge1,2,1.0,Edge2,2,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge1,3,1.0,Edge2,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "Edge3,1,1.0,Edge4,1,-1.0,ConstraintsDriver0,1,-<OFF1>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "Edge3,2,1.0,Edge4,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge3,3,1.0,Edge4,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "Edge5,1,1.0,Edge6,1,-1.0,ConstraintsDriver0,1,<OFF2>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "Edge5,2,1.0,Edge6,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge5,3,1.0,Edge6,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "Edge7,1,1.0,Edge8,1,-1.0,ConstraintsDriver0,1,-<BX>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge7,2,1.0,Edge8,2,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge7,3,1.0,Edge8,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "Edge9,1,1.0,Edge10,1,-1.0,ConstraintsDriver0,1,-<OFF1>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "Edge9,2,1.0,Edge10,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge9,3,1.0,Edge10,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "Edge11,1,1.0,Edge12,1,-1.0,ConstraintsDriver0,1,<OFF2>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "Edge11,2,1.0,Edge12,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge11,3,1.0,Edge12,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "Edge17,1,1.0,Edge18,1,-1.0,ConstraintsDriver0,1,-<BX>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge17,2,1.0,Edge18,2,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge17,3,1.0,Edge18,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "Edge14,1,1.0,Edge18,1,-1.0,ConstraintsDriver0,1,-<OFF1>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "Edge14,2,1.0,Edge18,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge14,3,1.0,Edge18,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "Edge15,1,1.0,Edge13,1,-1.0,ConstraintsDriver0,1,-<OFF1>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "Edge15,2,1.0,Edge13,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge15,3,1.0,Edge13,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "Edge16,1,1.0,Edge13,1,-1.0,ConstraintsDriver0,1,<OFF2>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "Edge16,2,1.0,Edge13,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "Edge16,3,1.0,Edge13,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "MasterNode3,1,1.0,MasterNode1,1,-1.0,ConstraintsDriver0,1,-<BX>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "MasterNode3,2,1.0,MasterNode1,2,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "MasterNode3,3,1.0,MasterNode1,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "MasterNode11,1,1.0,MasterNode1,1,-1.0,ConstraintsDriver0,1,-<OFF1>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "MasterNode11,2,1.0,MasterNode1,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "MasterNode11,3,1.0,MasterNode1,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "MasterNode5,1,1.0,MasterNode9,1,-1.0,ConstraintsDriver0,1,-<OFF1>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "MasterNode5,2,1.0,MasterNode9,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "MasterNode5,3,1.0,MasterNode9,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "MasterNode7,1,1.0,MasterNode9,1,-1.0,ConstraintsDriver0,1,<OFF2>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "MasterNode7,2,1.0,MasterNode9,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "MasterNode7,3,1.0,MasterNode9,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "MasterNode4,1,1.0,MasterNode2,1,-1.0,ConstraintsDriver0,1,-<BX>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "MasterNode4,2,1.0,MasterNode2,2,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "MasterNode4,3,1.0,MasterNode2,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "MasterNode12,1,1.0,MasterNode2,1,-1.0,ConstraintsDriver0,1,-<OFF1>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "MasterNode12,2,1.0,MasterNode2,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "MasterNode12,3,1.0,MasterNode2,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "MasterNode6,1,1.0,MasterNode10,1,-1.0,ConstraintsDriver0,1,-<OFF1>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "MasterNode6,2,1.0,MasterNode10,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "MasterNode6,3,1.0,MasterNode10,3,-1.0" << endl;
	Output << "*EQUATION" << endl;
	Output << "4" << endl;
	Output << "MasterNode8,1,1.0,MasterNode10,1,-1.0,ConstraintsDriver0,1,<OFF2>,ConstraintsDriver3,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "3" << endl;
	Output << "MasterNode8,2,1.0,MasterNode10,2,-1.0,ConstraintsDriver1,1,-<BY>" << endl;
	Output << "*EQUATION" << endl;
	Output << "2" << endl;
	Output << "MasterNode8,3,1.0,MasterNode10,3,-1.0" << endl;
}