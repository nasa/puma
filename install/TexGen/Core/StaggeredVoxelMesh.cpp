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
#include "StaggeredVoxelMesh.h"
#include "TexGen.h"
#include "StaggeredPeriodicBoundaries.h"
#include "RectangularVoxelMesh.h"
#include <iterator>
//#define SHINY_PROFILER TRUE

#define TOL		1e-9

using namespace TexGen;

CStaggeredVoxelMesh::CStaggeredVoxelMesh(string Type)
:CRectangularVoxelMesh(Type)
{
	//m_PeriodicBoundaries = new CStaggeredPeriodicBoundaries;
}

CStaggeredVoxelMesh::~CStaggeredVoxelMesh(void)
{
	//delete m_PeriodicBoundaries;
}

void CStaggeredVoxelMesh::SetOffset( double Offset )
{
	m_Offset = Offset;
	m_PeriodicBoundaries->SetOffset( Offset );
}

bool CStaggeredVoxelMesh::CalculateVoxelSizes(CTextile &Textile)
{
	if ( m_XVoxels % 2 )
	{
		TGERROR("Number of voxels in x direction must be even for staggered boundary conditions");
		return false;
	}
	if ( fabs( fmod( (double)m_XVoxels, 1.0/m_Offset ) ) > TOL )
	{
		TGERROR("Number of x voxels must satisfy  N mod (1/offset) = 0 condition");
		return false;
	}

	CRectangularVoxelMesh::CalculateVoxelSizes( Textile );

	return true;
}

void CStaggeredVoxelMesh::OutputPeriodicBoundaries(ostream &Output, CTextile& Textile, int iBoundaryConditions, bool bMatrixOnly )
{
	m_PeriodicBoundaries->SetDomainSize( Textile.GetDomain()->GetMesh() );

	vector<int> GroupA;
	vector<int> GroupB;
	pair< vector<int>, vector<int> > Faces;

	int numx = m_XVoxels + 1;
	int numy = m_YVoxels + 1;
	int numz = m_ZVoxels + 1;
	int xOffsetInd1 = (int)(numx * (1.0 - m_Offset));
	int xOffsetInd2 = (int)(numx * m_Offset);
	//int iDummyNodeNum = numx*numy*numz + 1;
	
	// Create a set of nodes for opposite faces of the domain, then output
	for ( int z = 1; z < numz-1; ++z )
	{
		for ( int x = 1; x < xOffsetInd1; ++x )
		{
			GroupA.push_back( 1 + x + z*numx*numy );	
		}
		for ( int x = xOffsetInd1+1; x < numx-1; ++x )
		{
			GroupB.push_back( 1 + x + z*numx*numy );
		}
	}
	m_PeriodicBoundaries->SetFaceA( GroupA, GroupB );

	GroupA.clear();
	GroupB.clear();
	for ( int z = 1; z < numz-1; ++z )
	{
		for ( int x = 1; x < xOffsetInd2; ++x )
		{
			GroupA.push_back( 1 + x + z*numx*numy + numx*(numy-1) );
		}
		for ( int x = xOffsetInd2+1; x < numx-1; ++x )
		{
			GroupB.push_back( 1 + x + z*numx*numy + numx*(numy-1) );
		}
	}
	m_PeriodicBoundaries->SetFaceB( GroupA, GroupB );

	GroupA.clear();
	GroupB.clear();
	for ( int z = 1; z < numz-1; ++z )
	{
		for ( int y = 1; y < numy-1; ++y )
		{
			GroupA.push_back( y*numx + z*numx*numy + 1);
			GroupB.push_back( numx-1 + y*numx + z*numx*numy +1 );
			
			
		}
	}
	m_PeriodicBoundaries->SetFaceC( GroupA, GroupB );

	GroupA.clear();
	GroupB.clear();
	for ( int y = 1; y < numy-1; ++y )
	{
		for ( int x = 1; x < numx-1; ++x )
		{
			GroupA.push_back( x + y*numx + 1 );
			GroupB.push_back( x + y*numx + (numz-1)*numx*numy + 1);
		}
	}
	m_PeriodicBoundaries->SetFaceD( GroupA, GroupB );

	// Create edge node sets
	// Note indices are 1 less than in Mikhail's script
	vector<int> Edges[18];

	for ( int z = 1; z < numz-1; ++z )
	{
		Edges[17].push_back( z*numx*numy + 1 );
		Edges[15].push_back( numx*(numy-1) + z*numx*numy +1 );
		Edges[14].push_back( (z+1)*numx*numy );
		Edges[16].push_back( numx + z*numx*numy );
	}
	
	for ( int y = 1; y < numy-1; ++y )
	{
		Edges[7].push_back( y*numx + (numz-1)*numx*numy + 1 ); 
		Edges[6].push_back( (y+1)*numx +(numz-1)*numx*numy );
		Edges[0].push_back( (y+1)*numx );
		Edges[1].push_back( y*numx + 1 );
	}

	for ( int x = 1; x < xOffsetInd1; ++x )
	{
		Edges[9].push_back( x + (numz-1)*numx*numy + 1 );
		Edges[3].push_back( x + 1 );
	}
	for ( int x = xOffsetInd1+1; x < numx-1; ++x )
	{
		Edges[11].push_back( x + (numz-1)*numx*numy + 1 );
		Edges[5].push_back( x + 1 );
	}
	//Vertical edges at offset
	for ( int z = 1; z < numz-1; ++z)
	{
		Edges[12].push_back( 1 + xOffsetInd1 + z*numx*numy);
		Edges[13].push_back( 1 + xOffsetInd2 + z*numx*numy + numx*(numy-1) );
	}

	for ( int x = 1; x < xOffsetInd2; ++x )
	{
		Edges[10].push_back( x + numx*(numy-1) + (numz-1)*numx*numy +1 );
		Edges[4].push_back( x + numx*(numy-1) + 1 );
	}
	for ( int x = xOffsetInd2+1; x < numx-1; ++x )
	{
		Edges[8].push_back( x + numx*(numy-1) + (numz-1)*numx*numy +1 );
		Edges[2].push_back( x + numx*(numy-1) + 1 );
	}

	for ( int i = 0; i < 18; ++i )
	{
		m_PeriodicBoundaries->SetEdges( Edges[i] );
		Edges[i].clear();
	}

	// Create vertex sets
	// Corresponding vertex name in Mikhail's script in comments
	m_PeriodicBoundaries->SetVertex( 1 );// A
	m_PeriodicBoundaries->SetVertex( (numz-1)*numx*numy + 1 );// A1
	m_PeriodicBoundaries->SetVertex( numx );// B
	m_PeriodicBoundaries->SetVertex( numx + (numz-1)*numx*numy );// B1
	m_PeriodicBoundaries->SetVertex( numx + numx*(numy-1) ); // C
	m_PeriodicBoundaries->SetVertex( numx + (numz-1)*numx*numy + numx*(numy-1) ); // C1
	m_PeriodicBoundaries->SetVertex( (numy-1)*numx + 1 ); // D
	m_PeriodicBoundaries->SetVertex( numx*(numy-1) + (numz-1)*numx*numy + 1 ); // D1
	m_PeriodicBoundaries->SetVertex( 1 + xOffsetInd1 ); // E
	m_PeriodicBoundaries->SetVertex( 1 +(numz-1)*numx*numy + xOffsetInd1); // E1
	m_PeriodicBoundaries->SetVertex( 1 + numx*(numy-1) + xOffsetInd2 ); // F
	m_PeriodicBoundaries->SetVertex( 1 + (numz-1)*numx*numy + numx*(numy-1) + xOffsetInd2 );  // F1

	
	
	m_PeriodicBoundaries->CreatePeriodicBoundaries( Output, numx*numy*numz + 1, Textile, iBoundaryConditions, bMatrixOnly );
}

