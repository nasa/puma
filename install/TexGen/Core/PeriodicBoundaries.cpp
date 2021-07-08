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
#include "PeriodicBoundaries.h"
#include "Materials.h"

#define NUM_FACES		6
#define NUM_EDGES		12
#define NUM_VERTICES	8

using namespace TexGen;
using namespace std;

CPeriodicBoundaries::CPeriodicBoundaries(int NumEdges, int NumVertices)
:m_NumEdges(NumEdges),
m_NumVertices(NumVertices)
{
}

CPeriodicBoundaries::~CPeriodicBoundaries(void)
{
}

void CPeriodicBoundaries::SetDomainSize( const CMesh& Mesh )
{
	pair<XYZ, XYZ> DomainAABB = Mesh.GetAABB();
	m_DomSize = DomainAABB.second - DomainAABB.first;
}

// Faces with constant x
void CPeriodicBoundaries::SetFaceA( vector<int>& A1, vector<int>& A2 )
{
	m_FaceA = make_pair( A1, A2 );
}

// Faces with consant y
void CPeriodicBoundaries::SetFaceB( vector<int>& B1, vector<int>& B2 )
{
	m_FaceB = make_pair( B1, B2 );
}

// Faces with constant z
void CPeriodicBoundaries::SetFaceC( vector<int>& C1, vector<int>& C2 )
{
	m_FaceC = make_pair( C1, C2 );
}

void CPeriodicBoundaries::SetVertex( int Vertex )
{
	m_Vertices.push_back( Vertex );
}

void CPeriodicBoundaries::SetEdges( vector<int>& Edge )
{
	m_Edges.push_back( Edge );
}

void CPeriodicBoundaries::CreatePeriodicBoundaries( ostream& Output, int iDummyNodeNum, CTextile& Textile, int iBoundaryConditions, bool bMatrixOnly )
{
	Output << "************************************" << endl;
	Output << "*** PERIODIC BOUNDARY CONDITIONS ***" << endl;
	Output << "************************************" << endl;

	OutputDummyNodeSets( Output, iDummyNodeNum );
	OutputFaceSets( Output );
	OutputEdgeSets( Output );
	OutputVertexSets( Output ); 

	OutputEquations( Output, iBoundaryConditions );

	OutputStep( Output, iBoundaryConditions );
}

void CPeriodicBoundaries::OutputSets( ostream& Output, vector<int>& Set, string SetName)
{
	Output << "*NSet, NSet=" + SetName;
	Output << ", Unsorted" << endl;
	WriteValues( Output, Set, 16 ); 
}

void CPeriodicBoundaries::OutputDummyNodeSets( ostream& Output, int iDummyNodeNum )
{
	// Dummy nodes/sets: x = 0, y = 1, z = 2, xy = 3, xz = 4, yz = 5
	Output << "*** ConstraintsDriver0 = e_x" << endl;
	Output << "*** ConstraintsDriver1 = e_y" << endl;
	Output << "*** ConstraintsDriver2 = e_z" << endl;
	Output << "*** ConstraintsDriver3 = e_xy" << endl;
	Output << "*** ConstraintsDriver4 = e_xz" << endl;
	Output << "*** ConstraintsDriver5 = e_yz" << endl;

	for ( int i = 0; i < 6; ++i )
	{
		Output << "*Node" << endl;
		Output << iDummyNodeNum + i << ", 0, 0, 0" << endl;
		Output << "*NSet, NSet=ConstraintsDriver" << i << endl;
		Output << iDummyNodeNum + i << endl;
	}
}

void CPeriodicBoundaries::OutputDummyNodeSets( string Filename, int iDummyNodeNum )
{
	AddExtensionIfMissing(Filename, ".inp");

	ofstream Output(Filename.c_str(), ofstream::app);
	OutputDummyNodeSets( Output, iDummyNodeNum );
}

void CPeriodicBoundaries::OutputFaceSets( ostream& Output )
{
	OutputSets( Output, m_FaceA.first, "FaceA" );
	OutputSets( Output, m_FaceA.second, "FaceB" );
	OutputSets( Output, m_FaceB.first, "FaceC" );
	OutputSets( Output, m_FaceB.second, "FaceD" );
	OutputSets( Output, m_FaceC.first, "FaceE" );
	OutputSets( Output, m_FaceC.second, "FaceF" );
}

void CPeriodicBoundaries::OutputEdgeSets( ostream& Output )
{
	for ( int i = 0; i < m_NumEdges; ++i )
	{
		OutputSets( Output, m_Edges[i], "Edge" + stringify(i+1) );
	}
}

void CPeriodicBoundaries::OutputVertexSets( ostream& Output )
{
	vector<int> Vertex;
	Vertex.push_back(0);
	for ( int i = 0; i < m_NumVertices; ++i )
	{
		Vertex[0] = m_Vertices[i];
		OutputSets( Output, Vertex, "MasterNode" + stringify(i+1) );
	}
}

void CPeriodicBoundaries::OutputEquations( ostream& Output, int iBoundaryConditions )
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
	Output << "*Equation\n3\n";
	Output << "FaceA, 1, 1.0, FaceB, 1, -1.0, ConstraintsDriver0, 1, -" << m_DomSize.x << endl;

	Output << "*Equation\n2\n";
	Output << "FaceA, 2, 1.0, FaceB, 2, -1.0" << endl;

	Output << "*Equation\n2\n";
	Output << "FaceA, 3, 1.0, FaceB, 3, -1.0" << endl;

	Output << "*Equation\n3\n";
	Output << "FaceC, 1, 1.0, FaceD, 1, -1.0, ConstraintsDriver3, 1, -" << m_DomSize.y << endl;

	Output << "*Equation\n3\n";
	Output << "FaceC, 2, 1.0, FaceD, 2, -1.0, ConstraintsDriver1, 1, -" << m_DomSize.y << endl;

	Output << "*Equation\n2\n";
	Output << "FaceC, 3, 1.0, FaceD, 3, -1.0" << endl;

	if ( iBoundaryConditions == MATERIAL_CONTINUUM )
	{
		Output << "*Equation\n3\n";
		Output << "FaceE, 1, 1.0, FaceF, 1, -1.0, ConstraintsDriver4, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "FaceE, 2, 1.0, FaceF, 2, -1.0, ConstraintsDriver5, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "FaceE, 3, 1.0, FaceF, 3, -1.0, ConstraintsDriver2, 1, -" << m_DomSize.z << endl;
	}

	Output << "*Equation\n3\n";
	Output << "Edge2, 1, 1.0, Edge1, 1, -1.0, ConstraintsDriver0, 1, -" << m_DomSize.x << endl;

	Output << "*Equation\n2\n";
	Output << "Edge2, 2, 1.0, Edge1, 2, -1.0" << endl;

	Output << "*Equation\n2\n";
	Output << "Edge2, 3, 1.0, Edge1, 3, -1.0" << endl;

	Output << "*Equation\n4\n";
	Output << "Edge3, 1, 1.0, Edge1, 1, -1.0, ConstraintsDriver0, 1, -" << m_DomSize.x << ", ConstraintsDriver3, 1, -" << m_DomSize.y << endl;

	Output << "*Equation\n3\n";
	Output << "Edge3, 2, 1.0, Edge1, 2, -1.0, ConstraintsDriver1, 1, -" << m_DomSize.y << endl;

	Output << "*Equation\n2\n";
	Output << "Edge3, 3, 1.0, Edge1, 3, -1.0" << endl;

	Output << "*Equation\n3\n";
	Output << "Edge4, 1, 1.0, Edge1, 1, -1.0, ConstraintsDriver3, 1, -" << m_DomSize.y << endl;

	Output << "*Equation\n3\n";
	Output << "Edge4, 2, 1.0, Edge1, 2, -1.0, ConstraintsDriver1, 1, -" << m_DomSize.y << endl;

	Output << "*Equation\n2\n";
	Output << "Edge4, 3, 1.0, Edge1, 3, -1.0" << endl;

	Output << "*Equation\n3\n";
	Output << "Edge6, 1, 1.0, Edge5, 1, -1.0, ConstraintsDriver0, 1, -" << m_DomSize.x << endl;

	Output << "*Equation\n2\n";
	Output << "Edge6, 2, 1.0, Edge5, 2, -1.0" << endl;

	Output << "*Equation\n2\n";
	Output << "Edge6, 3, 1.0, Edge5, 3, -1.0" << endl;

	if ( iBoundaryConditions == SINGLE_LAYER_RVE )
	{
		Output << "*Equation\n3\n";
		Output << "Edge7, 1, 1.0, Edge8, 1, -1.0, ConstraintsDriver0, 1, -" << m_DomSize.x << endl;

		Output << "*Equation\n2\n";
		Output << "Edge7, 2, 1.0, Edge8, 2, -1.0" << endl;

		Output << "*Equation\n2\n";
		Output << "Edge7, 3, 1.0, Edge8, 3, -1.0" << endl;

	}
	else
	{
		Output << "*Equation\n4\n";
		Output << "Edge7, 1, 1.0, Edge5, 1, -1.0, ConstraintsDriver4, 1, -" << m_DomSize.z << ", ConstraintsDriver0, 1, -" << m_DomSize.x << endl;

		Output << "*Equation\n3\n";
		Output << "Edge7, 2, 1.0, Edge5, 2, -1.0, ConstraintsDriver5, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "Edge7, 3, 1.0, Edge5, 3, -1.0, ConstraintsDriver2, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "Edge8, 1, 1.0, Edge5, 1, -1.0, ConstraintsDriver4, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "Edge8, 2, 1.0, Edge5, 2, -1.0, ConstraintsDriver5, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "Edge8, 3, 1.0, Edge5, 3, -1.0, ConstraintsDriver2, 1, -" << m_DomSize.z << endl;
	}

	Output << "*Equation\n3\n";
	Output << "Edge10, 1, 1.0, Edge9, 1, -1.0, ConstraintsDriver3, 1, -" << m_DomSize.y << endl;

	Output << "*Equation\n3\n";
	Output << "Edge10, 2, 1.0, Edge9, 2, -1.0, ConstraintsDriver1, 1, -" << m_DomSize.y << endl;

	Output << "*Equation\n2\n";
	Output << "Edge10, 3, 1.0, Edge9, 3, -1.0" << endl;

	if ( iBoundaryConditions == SINGLE_LAYER_RVE )
	{
		Output << "*Equation\n3\n";
		Output << "Edge11, 1, 1.0, Edge12, 1, -1.0, ConstraintsDriver3, 1, -" << m_DomSize.y << endl;

		Output << "*Equation\n3\n";
		Output << "Edge11, 2, 1.0, Edge12, 2, -1.0, ConstraintsDriver1, 1, -" << m_DomSize.y << endl;

		Output << "*Equation\n2\n";
		Output << "Edge11, 3, 1.0, Edge12, 3, -1.0" << endl;

	}
	else
	{
		Output << "*Equation\n4\n";
		Output << "Edge11, 1, 1.0, Edge9, 1, -1.0, ConstraintsDriver3, 1, -" << m_DomSize.y << ", ConstraintsDriver4, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n4\n";
		Output << "Edge11, 2, 1.0, Edge9, 2, -1.0, ConstraintsDriver1, 1, -" << m_DomSize.y << ", ConstraintsDriver5, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "Edge11, 3, 1.0, Edge9, 3, -1.0, ConstraintsDriver2, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "Edge12, 1, 1.0, Edge9, 1, -1.0, ConstraintsDriver4, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "Edge12, 2, 1.0, Edge9, 2, -1.0, ConstraintsDriver5, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "Edge12, 3, 1.0, Edge9, 3, -1.0, ConstraintsDriver2, 1, -" << m_DomSize.z << endl;
	}

	Output << "*Equation\n3\n";
	Output << "MasterNode2, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, -" << m_DomSize.x << endl;

	Output << "*Equation\n2\n";
	Output << "MasterNode2, 2, 1.0, MasterNode1, 2, -1.0" << endl;
	
	Output << "*Equation\n2\n";
	Output << "MasterNode2, 3, 1.0, MasterNode1, 3, -1.0" << endl;

	Output << "*Equation\n4\n";
	Output << "MasterNode3, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, -" << m_DomSize.x << ", ConstraintsDriver3, 1, -" << m_DomSize.y << endl;

	Output << "*Equation\n3\n";
	Output << "MasterNode3, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver1, 1, -" << m_DomSize.y << endl;

	Output << "*Equation\n2\n";
	Output << "MasterNode3, 3, 1.0, MasterNode1, 3, -1.0" << endl;

	Output << "*Equation\n3\n";
	Output << "MasterNode4, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver3, 1, -" << m_DomSize.y << endl;

	Output << "*Equation\n3\n";
	Output << "MasterNode4, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver1, 1, -" << m_DomSize.y << endl;

	Output << "*Equation\n2\n";
	Output << "MasterNode4, 3, 1.0, MasterNode1, 3, -1.0" << endl;

	if ( iBoundaryConditions == SINGLE_LAYER_RVE )
	{
		Output << "*Equation\n3\n";
		Output << "MasterNode6, 1, 1.0, MasterNode5, 1, -1.0, ConstraintsDriver0, 1, -" << m_DomSize.x << endl;

		Output << "*Equation\n2\n";
		Output << "MasterNode6, 2, 1.0, MasterNode5, 2, -1.0" << endl;

		Output << "*Equation\n2\n";
		Output << "MasterNode6, 3, 1.0, MasterNode5, 3, -1.0" << endl;

		Output << "*Equation\n4\n";
		Output << "MasterNode7, 1, 1.0, MasterNode5, 1, -1.0, ConstraintsDriver0, 1, -" << m_DomSize.x << "," << endl;
		Output << "ConstraintsDriver3, 1, -" << m_DomSize.y << endl;

		Output << "*Equation\n3\n";
		Output << "MasterNode7, 2, 1.0, MasterNode5, 2, -1.0, ConstraintsDriver1, 1, -" << m_DomSize.y << endl; 

		Output << "*Equation\n2\n";
		Output << "MasterNode7, 3, 1.0, MasterNode5, 3, -1.0" << endl;

		Output << "*Equation\n3\n";
		Output << "MasterNode8, 1, 1.0, MasterNode5, 1, -1.0, ConstraintsDriver3, 1, -" << m_DomSize.y << endl; 

		Output << "*Equation\n3\n";
		Output << "MasterNode8, 2, 1.0, MasterNode5, 2, -1.0, ConstraintsDriver1, 1, -" << m_DomSize.y << endl; 

		Output << "*Equation\n2\n";
		Output << "MasterNode8, 3, 1.0, MasterNode5, 3, -1.0" << endl;

	}
	else
	{
		Output << "*Equation\n3\n";
		Output << "MasterNode5, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver4, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "MasterNode5, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver5, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "MasterNode5, 3, 1.0, MasterNode1, 3, -1.0, ConstraintsDriver2, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n4\n";
		Output << "MasterNode6, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver4, 1, -" << m_DomSize.z << ", ConstraintsDriver0, 1, -" << m_DomSize.x << endl;

		Output << "*Equation\n3\n";
		Output << "MasterNode6, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver5, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "MasterNode6, 3, 1.0, MasterNode1, 3, -1.0, ConstraintsDriver2, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n5\n";
		Output << "MasterNode7, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver0, 1, -" << m_DomSize.x << ", ConstraintsDriver4, 1, -" << m_DomSize.z << "," << endl;
		Output << "ConstraintsDriver3, 1, -" << m_DomSize.y << endl;

		Output << "*Equation\n4\n";
		Output << "MasterNode7, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver1, 1, -" << m_DomSize.y << ", ConstraintsDriver5, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "MasterNode7, 3, 1.0, MasterNode1, 3, -1.0, ConstraintsDriver2, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n4\n";
		Output << "MasterNode8, 1, 1.0, MasterNode1, 1, -1.0, ConstraintsDriver3, 1, -" << m_DomSize.y << ", ConstraintsDriver4, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n4\n";
		Output << "MasterNode8, 2, 1.0, MasterNode1, 2, -1.0, ConstraintsDriver1, 1, -" << m_DomSize.y << ", ConstraintsDriver5, 1, -" << m_DomSize.z << endl;

		Output << "*Equation\n3\n";
		Output << "MasterNode8, 3, 1.0, MasterNode1, 3, -1.0, ConstraintsDriver2, 1, -" << m_DomSize.z << endl;
	}
	
}

void CPeriodicBoundaries::OutputStep( ostream& Output, int iBoundaryConditions )
{
	Output << "*******************" << endl;
	Output << "*** CREATE STEP ***" << endl;
	Output << "*******************" << endl;

	Output << "*** PREDEFINED FIELDS ***" << endl;
	Output << "*** Name: Initial temperature 0ºC all cells   Type: Temperature ***" << endl;
	Output << "*Initial Conditions, type=TEMPERATURE" << endl;
	Output << "AllNodes, 0." << endl;
	Output << endl;
	Output << "*Step, Name=Isothermal linear perturbation step, perturbation" << endl;
	Output << "Elastic material property computation" << endl;
	Output << "*Static" << endl;
	Output << endl;
	Output << "***********************" << endl;
	Output << "*** OUTPUT REQUESTS ***" << endl;
	Output << "***********************" << endl;
	Output << "*Output, field" << endl;

	bool bOutputTransverse = iBoundaryConditions == MATERIAL_CONTINUUM || iBoundaryConditions == SHEARED_BC || iBoundaryConditions == ROTATED_BC;

	if ( iBoundaryConditions == SINGLE_LAYER_RVE || iBoundaryConditions == STAGGERED_BC )
	{
		Output << "*Element Output, directions=YES" << endl << "S,E," << endl;
	}
	else
	{
		Output << "*Element Output, directions=YES" << endl << "S," << endl;
	}
	Output << "*** FIELD OUTPUT: Output Request Fx ***" << endl;
	Output << "*Node Output, nset=ConstraintsDriver0" << endl << "U," << endl;
	Output << "*** FIELD OUTPUT: Output Request Fy ***" << endl;
	Output << "*Node Output, nset=ConstraintsDriver1" << endl << "U," << endl;
	if ( bOutputTransverse )
	{
		Output << "*** FIELD OUTPUT: Ouput Request Fz ***" << endl;
		Output << "*Node Output, nset=ConstraintsDriver2" << endl << "U," << endl;
	}
	Output << "*** FIELD OUTPUT: Output Request Shear_xy ***" << endl;
	Output << "*Node Output, nset=ConstraintsDriver3" << endl << "U," << endl;
	if ( bOutputTransverse )
	{
		Output << "*** FIELD OUTPUT: Output Request Shear_zx ***" << endl;
		Output << "*Node Output, nset=ConstraintsDriver4" << endl << "U," << endl;
		Output << "*** FIELD OUTPUT: Output Request Shear_yz ***" << endl;
		Output << "*Node Output, nset=ConstraintsDriver5" << endl << "U," << endl; 
	}
	
	Output << endl;

	Output << "******************" << endl;
	Output << "*** LOAD CASES ***" << endl;
	Output << "******************" << endl;
	for ( int i = 0; i < 6; ++i )
	{
		if ( bOutputTransverse || !i || i == 1 || i==3 )
			OutputLoadCase( Output, i );
	}
	
	Output << endl;

	Output << "*End Step" << endl;
	Output << endl;
 
	Output << "*** STEP: Thermomechanical step ***" << endl;
	Output << "*Step, name=Thermomechanical step, perturbation" << endl;
	Output << "Coefficient of Thermal Expansion computation" << endl;
	Output << "*Static" << endl;
	 
	Output << "*** PREDEFINED FIELDS ***" << endl;
	Output << "*** Name: Temperature steady 1ºC all cells   Type: Temperature ***" << endl;
	Output << "*Temperature" << endl;
	Output << "AllNodes, 1." << endl;   // Make sure no blank line after this line or analysis fails
	Output << "***********************" << endl;
	Output << "*** OUTPUT REQUESTS ***" << endl;
	Output << "***********************" << endl;
	Output << "*Output, field" << endl;
	Output << "*Element Output, directions=YES" << endl << "S," << endl;
	Output << "*** FIELD OUTPUT: Output Request Fx ***" << endl;
	Output << "*Node Output, nset=ConstraintsDriver0" << endl << "U," << endl; 
	Output << "*** FIELD OUTPUT: Output Request Fy ***" << endl;
	Output << "*Node Output, nset=ConstraintsDriver1" << endl << "U," << endl;
	if ( bOutputTransverse )
	{
		Output << "*** FIELD OUTPUT: Output Request Fz ***" << endl;
		Output << "*Node Output, nset=ConstraintsDriver2" << endl << "U," << endl; 
	}
	Output << "*End Step" << endl;
}

void CPeriodicBoundaries::OutputStep( string Filename, int iBoundaryConditions )
{
	AddExtensionIfMissing(Filename, ".inp");

	ofstream Output(Filename.c_str(), ofstream::app);
	OutputStep( Output, iBoundaryConditions );
}

void CPeriodicBoundaries::OutputLoadCase(std::ostream &Output, int iCase )
{
	double dLoadPerVol = m_DomSize.x * m_DomSize.y * m_DomSize.z;
	Output << "*Load Case, name=Load" << iCase << endl;
	Output << "*Boundary, op=NEW" << endl;
	Output << "MasterNode1, 1, 1" << endl;
	Output << "MasterNode1, 2, 2" << endl;
	Output << "MasterNode1, 3, 3" << endl;
	Output << "*Cload" << endl;
	Output << "ConstraintsDriver" << iCase << ", 1, " << dLoadPerVol << endl;
	Output << "*End Load Case" << endl;
	Output << endl;
}
