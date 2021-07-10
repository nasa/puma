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
#include "ShearedVoxelMesh.h"
#include "TexGen.h"
#include "ShearedPeriodicBoundaries.h"
#include <iterator>
//#define SHINY_PROFILER TRUE


using namespace TexGen;

CShearedVoxelMesh::CShearedVoxelMesh(string Type)
:CVoxelMesh(Type)
{
	//m_PeriodicBoundaries = new CShearedPeriodicBoundaries;
}

CShearedVoxelMesh::~CShearedVoxelMesh(void)
{
	//delete m_PeriodicBoundaries;
}

bool CShearedVoxelMesh::CalculateVoxelSizes( CTextile &Textile )
{
	XYZ DomSize;
	double Angle1, Angle2;
	CMesh Mesh = Textile.GetDomain()->GetMesh();

	m_StartPoint = Mesh.GetNode(0);
	XYZ Node4 = Mesh.GetNode(4);
	XYZ Node2 = Mesh.GetNode(2);
	DomSize.x = GetLength( m_StartPoint, Node4 );
	DomSize.y = GetLength( m_StartPoint, Node2 );
	DomSize.z = GetLength( m_StartPoint, Mesh.GetNode(1) );
	Angle1 = atan2( Node4.y - m_StartPoint.y, Node4.x - m_StartPoint.x );
	Angle2 = atan2( Node2.y - m_StartPoint.y, Node2.x - m_StartPoint.x );

	m_ShearedVoxSize[0].x = DomSize.x * cos(Angle1) / m_XVoxels;
	m_ShearedVoxSize[0].y = DomSize.x * sin(Angle1) / m_XVoxels;
	m_ShearedVoxSize[1].x = DomSize.y * cos(Angle2) / m_YVoxels;
	m_ShearedVoxSize[1].y = DomSize.y * sin(Angle2) / m_YVoxels;
	m_ShearedVoxSize[2].x = m_ShearedVoxSize[2].y = DomSize.z / m_ZVoxels;
	return true;
}

void CShearedVoxelMesh::OutputNodes(ostream &Output, CTextile &Textile, int Filetype )
{
	int x,y,z;
	int iNodeIndex = 1;
	vector<XYZ> CentrePoints;
	vector<POINT_INFO> RowInfo;
	XYZ StartPoint = m_StartPoint;
	
	for ( z = 0; z <= m_ZVoxels; ++z )
	{
		StartPoint.x = m_StartPoint.x;
		StartPoint.y = m_StartPoint.y;
		StartPoint.z = m_StartPoint.z + m_ShearedVoxSize[2].x * z;
		for ( y = 0; y <= m_YVoxels; ++y )
		{
			StartPoint.x = m_StartPoint.x + m_ShearedVoxSize[1].x * y;
			StartPoint.y = m_StartPoint.y + m_ShearedVoxSize[1].y * y;
			for ( x = 0; x <=m_XVoxels; ++x )
			{
				XYZ Point;
				Point.x = StartPoint.x + m_ShearedVoxSize[0].x * x;
				Point.y = StartPoint.y + m_ShearedVoxSize[0].y * x;
				Point.z = StartPoint.z;

				if (Filetype == INP_EXPORT)
				{
					Output << iNodeIndex << ", ";
					Output << Point << "\n";
				}
				else if (Filetype == VTU_EXPORT)
					m_Mesh.AddNode(Point);

				if ( x < m_XVoxels && y < m_YVoxels && z < m_ZVoxels )
				{
					Point.x += 0.5*m_ShearedVoxSize[0].x;
					Point.x += 0.5*m_ShearedVoxSize[1].x;
					Point.y += 0.5*m_ShearedVoxSize[0].y;
					Point.y += 0.5*m_ShearedVoxSize[1].y;
					Point.z += 0.5*m_ShearedVoxSize[2].x;
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