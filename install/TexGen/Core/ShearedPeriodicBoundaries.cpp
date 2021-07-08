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
#include "ShearedPeriodicBoundaries.h"


using namespace TexGen;
using namespace std;

CShearedPeriodicBoundaries::CShearedPeriodicBoundaries(void)
{
}

CShearedPeriodicBoundaries::~CShearedPeriodicBoundaries(void)
{
}

void CShearedPeriodicBoundaries::SetDomainSize( const CMesh& Mesh )
{
	double Angle1, Angle2;
	XYZ StartPoint = Mesh.GetNode(0);
	XYZ Node4 = Mesh.GetNode(4);
	XYZ Node2 = Mesh.GetNode(2);

	m_DomSize.x = GetLength( StartPoint, Node4 );
	m_DomSize.y = GetLength( StartPoint, Node2 );
	m_DomSize.z = GetLength( StartPoint, Mesh.GetNode(1) );
	Angle1 = atan2( Node4.y - StartPoint.y, Node4.x - StartPoint.x );
	Angle2 = atan2( Node2.y - StartPoint.y, Node2.x - StartPoint.x );

	m_Domyx = m_DomSize.y * cos(Angle2);
	m_Domyy = m_DomSize.y * sin(Angle2);
}

void CShearedPeriodicBoundaries::OutputEquations( ostream& Output, int iBoundaryConditions )
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
	Output << "x=" << m_DomSize.x << endl;
	Output << "***Unit cel sheared Y dimension " << endl;
	Output << "yx=" << m_Domyx << endl;
	Output << "yy=" << m_Domyy << endl;
	Output << "***unit cell Z dimension" << endl;
	Output << "z=" << m_DomSize.z << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "FaceA, 1, 1.0, FaceB, 1, -1.0, ConstraintsDriver0, 1, -<x>" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "FaceA, 2, 1.0, FaceB, 2, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "FaceA, 3, 1.0, FaceB, 3, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "FaceC, 1, 1.0, FaceD, 1, -1.0, ConstraintsDriver3, 1, -<yy>," << endl;
	Output << "ConstraintsDriver0, 1, -<yx>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "FaceC, 2, 1.0, FaceD, 2, -1.0, ConstraintsDriver1, 1, -<yy>" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "FaceC, 3, 1.0, FaceD, 3, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "FaceE, 1, 1.0, FaceF, 1, -1.0, ConstraintsDriver4, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "FaceE, 2, 1.0, FaceF, 2, -1.0, ConstraintsDriver5, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "FaceE, 3, 1.0, FaceF, 3, -1.0, ConstraintsDriver2, 1, -<z>" << endl;
	Output << "************************************************" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge2, 1, 1.0, Edge1, 1, -1.0, ConstraintsDriver0, 1, -<x>" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "Edge2, 2, 1.0, Edge1, 2, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "Edge2, 3, 1.0, Edge1, 3, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "Edge3, 1, 1.0, Edge1, 1, -1.0, ConstraintsDriver0, 1, -<x>, " << endl;
	Output << "ConstraintsDriver3, 1, -<yy>, ConstraintsDriver0, 1, -<yx>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge3, 2, 1.0, Edge1, 2, -1.0, ConstraintsDriver1, 1, -<yy>" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "Edge3, 3, 1.0, Edge1, 3, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge4, 1, 1.0, Edge1, 1, -1.0," << endl; 
	Output << "ConstraintsDriver3, 1, -<yy>, ConstraintsDriver0, 1, -<yx>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge4, 2, 1.0, Edge1, 2, -1.0, ConstraintsDriver1, 1, -<yy>" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "Edge4, 3, 1.0, Edge1, 3, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge6, 1, 1.0, Edge5, 1, -1.0, ConstraintsDriver0, 1, -<x>" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "Edge6, 2, 1.0, Edge5, 2, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "Edge6, 3, 1.0, Edge5, 3, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge7, 1, 1.0, Edge5, 1, -1.0, ConstraintsDriver0, 1, -<x>," << endl; 
	Output << "ConstraintsDriver4, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge7, 2, 1.0, Edge5, 2, -1.0, ConstraintsDriver5, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge7, 3, 1.0, Edge5, 3, -1.0, ConstraintsDriver2, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge8, 1, 1.0, Edge5, 1, -1.0, ConstraintsDriver4, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge8, 2, 1.0, Edge5, 2, -1.0, ConstraintsDriver5, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge8, 3, 1.0, Edge5, 3, -1.0, ConstraintsDriver2, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge10, 1, 1.0, Edge9, 1, -1.0, " << endl;
	Output << "ConstraintsDriver3, 1, -<yy>, ConstraintsDriver0, 1, -<yx>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge10, 2, 1.0, Edge9, 2, -1.0, ConstraintsDriver1, 1, -<yy>" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "Edge10, 3, 1.0, Edge9, 3, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "Edge11, 1, 1.0, Edge9, 1, -1.0,ConstraintsDriver3, 1, -<yy>," << endl; 
	Output << "ConstraintsDriver0, 1, -<yx>,ConstraintsDriver4, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "Edge11, 2, 1.0, Edge9, 2, -1.0, ConstraintsDriver1, 1, -<yy>," << endl; 
	Output << "ConstraintsDriver5, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge11, 3, 1.0, Edge9, 3, -1.0, ConstraintsDriver2, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge12, 1, 1.0, Edge9, 1, -1.0, ConstraintsDriver4, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge12, 2, 1.0, Edge9, 2, -1.0, ConstraintsDriver5, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "Edge12, 3, 1.0, Edge9, 3, -1.0, ConstraintsDriver2, 1, -<z>" << endl;
	Output << "******************************************************" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode2, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, -<x>" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "MasterNode2, 2, 1.0, MasterNode1, 2, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "MasterNode2, 3, 1.0, MasterNode1, 3, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "MasterNode3, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, -<x>, " << endl;
	Output << "ConstraintsDriver3, 1, -<yy>, ConstraintsDriver0, 1, -<yx>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode3, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver1, 1, -<yy>" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "MasterNode3, 3, 1.0, MasterNode1, 3, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "MasterNode4, 1, 1.0, MasterNode1, 1, -1.0," << endl; 
	Output << "ConstraintsDriver3, 1, -<yy>, ConstraintsDriver0, 1, -<yx>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode4, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver1, 1, -<yy>" << endl;
	Output << "*Equation" << endl;
	Output << "2" << endl;
	Output << "MasterNode4, 3, 1.0, MasterNode1, 3, -1.0" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode5, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver4, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode5, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver5, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode5, 3, 1.0, MasterNode1, 3, -1.0, ConstraintsDriver2, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "MasterNode6, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, -<x>, " << endl;
	Output << "ConstraintsDriver4, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode6, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver5, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode6, 3, 1.0, MasterNode1, 3, -1.0, ConstraintsDriver2, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "6" << endl;
	Output << "MasterNode7, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, -<x>," << endl; 
	Output << "ConstraintsDriver3, 1, -<yy>, ConstraintsDriver0, 1, -<yx>," << endl;
	Output << "ConstraintsDriver4, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "MasterNode7, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver1, 1, -<yy>, " << endl;
	Output << "ConstraintsDriver5, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode7, 3, 1.0, MasterNode1, 3, -1.0, ConstraintsDriver2, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "5" << endl;
	Output << "MasterNode8, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver3, 1, -<yy>," << endl; 
	Output << "ConstraintsDriver0, 1, -<yx>, ConstraintsDriver4, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "4" << endl;
	Output << "MasterNode8, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver1, 1, -<yy>, " << endl;
	Output << "ConstraintsDriver5, 1, -<z>" << endl;
	Output << "*Equation" << endl;
	Output << "3" << endl;
	Output << "MasterNode8, 3, 1.0, MasterNode1, 3, -1.0, ConstraintsDriver2, 1, -<z>" << endl;

}