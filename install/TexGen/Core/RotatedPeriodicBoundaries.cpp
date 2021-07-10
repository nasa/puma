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

#include "PrecompiledHeaders.h"
#include "TexGen.h"
#include "RotatedPeriodicBoundaries.h"


using namespace TexGen;
using namespace std;

CRotatedPeriodicBoundaries::CRotatedPeriodicBoundaries(void)
{
}

CRotatedPeriodicBoundaries::~CRotatedPeriodicBoundaries(void)
{
}

void CRotatedPeriodicBoundaries::SetDomainSize( const CMesh& Mesh )
{
	XYZ DomSize;

	XYZ StartPoint = Mesh.GetNode(0);
	XYZ Node1 = Mesh.GetNode(1);
	XYZ Node2 = Mesh.GetNode(2);
	XYZ Node4 = Mesh.GetNode(4);

	m_DomSize.x = GetLength( StartPoint, Node4 );
	m_DomSize.y = GetLength( StartPoint, Node2 );
	m_DomSize.z = GetLength( StartPoint, Node1 );

	m_DispVec[0] = Node4 - StartPoint;
	m_DispVec[1] = Node2 - StartPoint;
	m_DispVec[2] = Node1 - StartPoint;
}

void CRotatedPeriodicBoundaries::OutputEquations( ostream& Output, int iBoundaryConditions )
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
	// Multiply all parameters by -1 to avoid getting -- in equation after abaqus has parsed it
	// Then put eg <xx> in equations rather than -<xx>
	Output << "***unit cell X axis displacements" << endl;
	Output << "xx=" << -m_DispVec[0].x << endl;
	Output << "xy=" << -m_DispVec[0].y << endl;
	Output << "xz=" << -m_DispVec[0].z << endl;

	Output << "***Unit cell Y axis displacements" << endl;
	Output << "yx=" << -m_DispVec[1].x << endl;
	Output << "yy=" << -m_DispVec[1].y << endl;
	Output << "yz=" << -m_DispVec[1].z << endl;
	
	Output << "***Unit cell Z axis displacements" << endl;
	Output << "zx=" << -m_DispVec[2].x << endl;
	Output << "zy=" << -m_DispVec[2].y << endl;
	Output << "zz=" << -m_DispVec[2].z << endl;

	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "FaceA, 1, 1.0, FaceB, 1, -1.0, ConstraintsDriver0, 1, <xx>" << endl;
	Output << "ConstraintsDriver3, 1, <xy>, ConstraintsDriver4, 1, <xz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "FaceA, 2, 1.0, FaceB, 2, -1.0" << endl;
	Output << "ConstraintsDriver1, 1, <xy>, ConstraintsDriver5, 1, <xz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "FaceA, 3, 1.0, FaceB, 3, -1.0" << endl;
	Output << "ConstraintsDriver2, 1, <xz>" << endl;

	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "FaceC, 1, 1.0, FaceD, 1, -1.0, ConstraintsDriver0, 1, <yx>" << endl;
	Output << "ConstraintsDriver3, 1, <yy>, ConstraintsDriver4, 1, <yz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "FaceC, 2, 1.0, FaceD, 2, -1.0" << endl;
	Output << "ConstraintsDriver1, 1, <yy>, ConstraintsDriver5, 1, <yz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "FaceC, 3, 1.0, FaceD, 3, -1.0" << endl;
	Output << "ConstraintsDriver2, 1, <yz>" << endl;

	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "FaceE, 1, 1.0, FaceF, 1, -1.0, ConstraintsDriver0, 1, <zx>" << endl;
	Output << "ConstraintsDriver3, 1, <zy>, ConstraintsDriver4, 1, <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "FaceE, 2, 1.0, FaceF, 2, -1.0" << endl;
	Output << "ConstraintsDriver1, 1, <zy>, ConstraintsDriver5, 1, <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "FaceE, 3, 1.0, FaceF, 3, -1.0" << endl;
	Output << "ConstraintsDriver2, 1, <zz>" << endl;

	Output << "************************************************" << endl;
	Output << "*** There seems to be an issue with using parameterised variables for repeated constraints " << endl;
	Output << "*** ABAQUS should be able to combine these but it generates a system error, reported in the .dat file " << endl;
	Output << "*** Only solution for this found is to add the values before exporting to the .inp file "  << endl;
	Output << "*** Comments in following sections show which parameters have been added " << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "Edge2, 1, 1.0, Edge1, 1, -1.0, ConstraintsDriver0, 1, <xx>, ConstraintsDriver3, 1, <xy>" << endl;
	Output << "ConstraintsDriver4, 1, <xz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge2, 2, 1.0, Edge1, 2, -1.0, ConstraintsDriver1, 1, <xy>, ConstraintsDriver5, 1, <xz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge2, 3, 1.0, Edge1, 3, -1.0, ConstraintsDriver2, 1, <xz>" << endl;

	Output << "*** ConstraintsDriver0 <xx> + <yx>" << endl;
	Output << "*** ConstraintsDriver3 <xy> + <yy>" << endl;
	Output << "*** ConstraintsDriver4 <xz> + <yz>" << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "Edge3, 1, 1.0, Edge1, 1, -1.0, ConstraintsDriver0, 1, " << -1.0*(m_DispVec[0].x + m_DispVec[1].x) << ", ConstraintsDriver3, 1, " << -1.0*(m_DispVec[0].y + m_DispVec[1].y) << endl;
	Output << "ConstraintsDriver4, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[1].z) << endl;
	
	Output << "*** ConstraintsDriver1 <xy> + <yy>" << endl;
	Output << "*** ConstraintsDriver5 <xz> + <yz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge3, 2, 1.0, Edge1, 2, -1.0, ConstraintsDriver1, 1, " << -1.0*(m_DispVec[0].y + m_DispVec[1].y) << ", ConstraintsDriver5, 1, "<< -1.0*(m_DispVec[0].z + m_DispVec[1].z)  << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge3, 3, 1.0, Edge1, 3, -1.0, ConstraintsDriver2, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[1].z) << endl;

	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "Edge4, 1, 1.0, Edge1, 1, -1.0, ConstraintsDriver0, 1, <yx>, ConstraintsDriver3, 1, <yy>" << endl;
	Output << "ConstraintsDriver4, 1, <yz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge4, 2, 1.0, Edge1, 2, -1.0, ConstraintsDriver1, 1, <yy>, ConstraintsDriver5, 1, <yz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge4, 3, 1.0, Edge1, 3, -1.0, ConstraintsDriver2, 1, <yz>" << endl;

	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "Edge6, 1, 1.0, Edge5, 1, -1.0, ConstraintsDriver0, 1, <xx>" << endl;
	Output << "ConstraintsDriver3, 1, <xy>, ConstraintsDriver4, 1, <xz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge6, 2, 1.0, Edge5, 2, -1.0" << endl;
	Output << "ConstraintsDriver1, 1, <xy>, ConstraintsDriver5, 1, <xz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge6, 3, 1.0, Edge5, 3, -1.0" << endl;
	Output << "ConstraintsDriver2, 1, <xz>" << endl;

	Output << "*** ConstraintsDriver0 <xx> + <zx>" << endl;
	Output << "*** ConstraintsDriver3 <xy> + <zy>" << endl;
	Output << "*** ConstraintsDriver4 <xz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "Edge7, 1, 1.0, Edge5, 1, -1.0, ConstraintsDriver0, 1, " << -1.0*(m_DispVec[0].x + m_DispVec[2].x) << endl;
	Output << "ConstraintsDriver3, 1, " << -1.0*(m_DispVec[0].y + m_DispVec[2].y) << ", ConstraintsDriver4, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[2].z) << endl;
	
	Output << "*** ConstraintsDriver1 <xy> + <zy>" << endl;
	Output << "*** ConstraintsDriver5 <xz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge7, 2, 1.0, Edge5, 2, -1.0" << endl;
	Output << "ConstraintsDriver1, 1, " << -1.0*(m_DispVec[0].y + m_DispVec[2].y) << ", ConstraintsDriver5, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[2].z) << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge7, 3, 1.0, Edge5, 3, -1.0" << endl;
	Output << "ConstraintsDriver2, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[2].z) << endl;

	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "Edge8, 1, 1.0, Edge5, 1, -1.0, ConstraintsDriver0, 1, <zx>" << endl;
	Output << "ConstraintsDriver3, 1, <zy>, ConstraintsDriver4, 1, <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge8, 2, 1.0, Edge5, 2, -1.0" << endl;
	Output << "ConstraintsDriver1, 1, <zy>, ConstraintsDriver5, 1, <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge8, 3, 1.0, Edge5, 3, -1.0" << endl;
	Output << "ConstraintsDriver2, 1, <zz>" << endl;

	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "Edge10, 1, 1.0, Edge9, 1, -1.0, ConstraintsDriver0, 1, <yx>" << endl;
	Output << "ConstraintsDriver3, 1, <yy>, ConstraintsDriver4, 1, <yz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge10, 2, 1.0, Edge9, 2, -1.0" << endl;
	Output << "ConstraintsDriver1, 1, <yy>, ConstraintsDriver5, 1, <yz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge10, 3, 1.0, Edge9, 3, -1.0" << endl;
	Output << "ConstraintsDriver2, 1, <yz>" << endl;

	Output << "*** ConstraintsDriver0 <yx> + <zx>" << endl;
	Output << "*** ConstraintsDriver3 <yy> + <zy>" << endl;
	Output << "*** ConstraintsDriver4 <yz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "Edge11, 1, 1.0, Edge9, 1, -1.0, ConstraintsDriver0, 1, " << -1.0*(m_DispVec[1].x + m_DispVec[2].x) << endl;
	Output << "ConstraintsDriver3, 1, " << -1.0*(m_DispVec[1].y + m_DispVec[2].y) << ", ConstraintsDriver4, 1, " << -1.0*(m_DispVec[1].z + m_DispVec[2].z) << endl;
	
	Output << "*** ConstraintsDriver1 <yy> + <zy>" << endl;
	Output << "*** ConstraintsDriver5 <yz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge11, 2, 1.0, Edge9, 2, -1.0" << endl;
	Output << "ConstraintsDriver1, 1, " << -1.0*(m_DispVec[1].y + m_DispVec[2].y) << ", ConstraintsDriver5, 1, " << -1.0*(m_DispVec[1].z + m_DispVec[2].z) << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge11, 3, 1.0, Edge9, 3, -1.0" << endl;
	Output << "ConstraintsDriver2, 1, " << -1.0*(m_DispVec[1].z + m_DispVec[2].z) << endl;

	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "Edge12, 1, 1.0, Edge9, 1, -1.0, ConstraintsDriver0, 1, <zx>" << endl;
	Output << "ConstraintsDriver3, 1, <zy>, ConstraintsDriver4, 1, <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge12, 2, 1.0, Edge9, 2, -1.0" << endl;
	Output << "ConstraintsDriver1, 1, <zy>, ConstraintsDriver5, 1, <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge12, 3, 1.0, Edge9, 3, -1.0" << endl;
	Output << "ConstraintsDriver2, 1, <zz>" << endl;
	
	Output << "******************************************************" << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "MasterNode2, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, <xx>, ConstraintsDriver3, 1, <xy>" << endl;
	Output << "ConstraintsDriver4, 1, <xz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "MasterNode2, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver1, 1, <xy>, ConstraintsDriver5, 1, <xz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode2, 3, 1.0, MasterNode1, 3, -1.0, ConstraintsDriver2, 1, <xz>" << endl;

	Output << "*** ConstraintsDriver0 <xx> + <yx>" << endl;
	Output << "*** ConstraintsDriver3 <xy> + <yy>" << endl;
	Output << "*** ConstraintsDriver4 <xz> + <yz>" << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "MasterNode3, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, " << -1.0*(m_DispVec[0].x + m_DispVec[1].x) << ", ConstraintsDriver3, 1, " << -1.0*(m_DispVec[0].y + m_DispVec[1].y) << endl;
	Output << "ConstraintsDriver4, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[1].z) << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "MasterNode3, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver1, 1, " << -1.0*(m_DispVec[0].y + m_DispVec[1].y) << ", ConstraintsDriver5, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[1].z) << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "MasterNode3, 3, 1.0, MasterNode1, 3, -1.0, ConstraintsDriver2, 1, <xz>" << endl;
	Output << "ConstraintsDriver2, 1, <yz>" << endl;

	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "MasterNode4, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, <yx>, ConstraintsDriver3, 1, <yy>" << endl;
	Output << "ConstraintsDriver4, 1, <yz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "MasterNode4, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver1, 1, <yy>, ConstraintsDriver5, 1, <yz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode4, 3, 1.0, MasterNode1, 3, -1.0, ConstraintsDriver2, 1, <yz>" << endl;

	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "MasterNode5, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, <zx>" << endl;
	Output << "ConstraintsDriver3, 1, <zy>, ConstraintsDriver4, 1, <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "MasterNode5, 2, 1.0, MasterNode1, 2, -1.0" << endl;
	Output << "ConstraintsDriver1, 1, <zy>, ConstraintsDriver5, 1, <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode5, 3, 1.0, MasterNode1, 3, -1.0" << endl;
	Output << "ConstraintsDriver2, 1, <zz>" << endl;

	Output << "*** ConstraintsDriver0 <xx> + <zx>" << endl;
	Output << "*** ConstraintsDriver3 <xy> + <zy>" << endl;
	Output << "*** ConstraintsDriver4 <xz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "MasterNode6, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, " << -1.0*(m_DispVec[0].x + m_DispVec[2].x) << endl;
	Output << "ConstraintsDriver3, 1, " << -1.0*(m_DispVec[0].y + m_DispVec[2].y) << ", ConstraintsDriver4, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[2].z) << endl;
	
	Output << "*** ConstraintsDriver1 <xy> + <zy>" << endl;
	Output << "*** ConstraintsDriver5 <xz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "MasterNode6, 2, 1.0, MasterNode1, 2, -1.0" << endl;
	Output << "ConstraintsDriver1, 1, " << -1.0*(m_DispVec[0].y + m_DispVec[2].y) << ", ConstraintsDriver5, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[2].z) << endl;
	
	Output << "*** ConstraintsDriver2 <xz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode6, 3, 1.0, MasterNode1, 3, -1.0" << endl;
	Output << "ConstraintsDriver2, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[2].z) << endl;
	
	Output << "*** ConstraintsDriver0 <xx> + <yx> +<zx>" << endl;
	Output << "*** ConstraintsDriver3 <xy> + <yy> + <zy>" << endl;
	Output << "*** ConstraintsDriver4 <xz> + <yz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "MasterNode7, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, " << -1.0*(m_DispVec[0].x + m_DispVec[1].x + m_DispVec[2].x) << " , ConstraintsDriver3, 1, " << -1.0*(m_DispVec[0].y + m_DispVec[1].y + m_DispVec[2].y) << endl;
	Output << "ConstraintsDriver4, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[1].z + m_DispVec[2].z)<< endl;
	
	Output << "*** ConstraintsDriver1 <xy> + <yy> + <zy>" << endl;
	Output << "*** ConstraintsDriver5 <xz> + <yz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "MasterNode7, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver1, 1, " <<  -1.0*(m_DispVec[0].y + m_DispVec[1].y + m_DispVec[2].y) << ", ConstraintsDriver5, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[1].z + m_DispVec[2].z)<< endl;
	
	Output << "*** ConstraintsDriver2 <xz> + <yz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode7, 3, 1.0, MasterNode1, 3, -1.0, ConstraintsDriver2, 1, " << -1.0*(m_DispVec[0].z + m_DispVec[1].z + m_DispVec[2].z) << endl;

	Output << "*** ConstraintsDriver0 <yx> + <zx>" << endl;
	Output << "*** ConstraintsDriver3 <yy> + <zy>" << endl;
	Output << "*** ConstraintsDriver4 <yz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "MasterNode8, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, " << -1.0*(m_DispVec[1].x + m_DispVec[2].x) << endl;
	Output << "ConstraintsDriver3, 1, " << -1.0*(m_DispVec[1].y + m_DispVec[2].y) << ", ConstraintsDriver4, 1, " << -1.0*(m_DispVec[1].z + m_DispVec[2].z) << endl;
	
	Output << "*** ConstraintsDriver1 <yy> + <zy>" << endl;
	Output << "*** ConstraintsDriver5 <yz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "MasterNode8, 2, 1.0, MasterNode1, 2, -1.0" << endl;
	Output << "ConstraintsDriver1, 1, " << -1.0*(m_DispVec[1].y + m_DispVec[2].y) << " , ConstraintsDriver5, 1, " << -1.0*(m_DispVec[1].z + m_DispVec[2].z) << endl;
	
	Output << "*** ConstraintsDriver2 <yz> + <zz>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode8, 3, 1.0, MasterNode1, 3, -1.0" << endl;
	Output << "ConstraintsDriver2, 1, " << -1.0*(m_DispVec[1].z + m_DispVec[2].z) << endl;
}