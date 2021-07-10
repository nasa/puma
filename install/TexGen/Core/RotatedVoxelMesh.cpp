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
#include "RotatedVoxelMesh.h"
#include "TexGen.h"
//#include "ShearedPeriodicBoundaries.h"
#include <iterator>
//#define SHINY_PROFILER TRUE


using namespace TexGen;

CRotatedVoxelMesh::CRotatedVoxelMesh(string Type)
:CVoxelMesh(Type)
{
	
}

CRotatedVoxelMesh::~CRotatedVoxelMesh(void)
{
	
}

bool CRotatedVoxelMesh::CalculateVoxelSizes( CTextile &Textile )
{
	XYZ DomSize;
	CMesh Mesh = Textile.GetDomain()->GetMesh();

	m_StartPoint = Mesh.GetNode(0);
	XYZ Node4 = Mesh.GetNode(4);
	XYZ Node2 = Mesh.GetNode(2);
	XYZ Node1 = Mesh.GetNode(1);
	DomSize.x = GetLength( m_StartPoint, Node4 );
	DomSize.y = GetLength( m_StartPoint, Node2 );
	DomSize.z = GetLength( m_StartPoint, Node1 );

	XYZ XVec = Node4 - m_StartPoint;
	XYZ YVec = Node2 - m_StartPoint;
	XYZ ZVec = Node1 - m_StartPoint;
	m_RotatedVoxSize[0] = XVec / m_XVoxels;
	m_RotatedVoxSize[1] = YVec / m_YVoxels;
	m_RotatedVoxSize[2] = ZVec / m_ZVoxels;

	return true;
}

void CRotatedVoxelMesh::OutputNodes(ostream &Output, CTextile &Textile, int Filetype )
{
	int x,y,z;
	int iNodeIndex = 1;
	vector<XYZ> CentrePoints;
	vector<POINT_INFO> RowInfo;
	XYZ StartPoint = m_StartPoint;
	
	for ( z = 0; z <= m_ZVoxels; ++z )
	{
		StartPoint = m_StartPoint + m_RotatedVoxSize[2] * z;
		
		for ( y = 0; y <= m_YVoxels; ++y )
		{
			XYZ YStartPoint;
			YStartPoint = StartPoint + m_RotatedVoxSize[1] * y;
			
			for ( x = 0; x <=m_XVoxels; ++x )
			{
				XYZ Point;
				Point = YStartPoint + m_RotatedVoxSize[0] * x;
				
				if (Filetype == INP_EXPORT)
				{
					Output << iNodeIndex << ", ";
					Output << Point << "\n";
				}
				else if (Filetype == VTU_EXPORT)
					m_Mesh.AddNode(Point);

				if ( x < m_XVoxels && y < m_YVoxels && z < m_ZVoxels )
				{
					Point.x += 0.5*m_RotatedVoxSize[0].x;
					Point.x += 0.5*m_RotatedVoxSize[1].x;
					Point.x += 0.5*m_RotatedVoxSize[2].x;
					Point.y += 0.5*m_RotatedVoxSize[0].y;
					Point.y += 0.5*m_RotatedVoxSize[1].y;
					Point.y += 0.5*m_RotatedVoxSize[2].y;
					Point.z += 0.5*m_RotatedVoxSize[0].z;
					Point.z += 0.5*m_RotatedVoxSize[1].z;
					Point.z += 0.5*m_RotatedVoxSize[2].z;
					CentrePoints.push_back(Point);
				}
				++iNodeIndex;
			}
			
		}
		RowInfo.clear();   // Changed to do layer at a time instead of row to optimise
			Textile.GetPointInformation( CentrePoints, RowInfo );
			m_ElementsInfo.insert(m_ElementsInfo.end(), RowInfo.begin(), RowInfo.end() );
			CentrePoints.clear();
	}
}