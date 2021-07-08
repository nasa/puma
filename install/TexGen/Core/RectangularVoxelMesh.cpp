/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2012 Louise Brown

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
#include "RectangularVoxelMesh.h"
#include "TexGen.h"
#include "PeriodicBoundaries.h"
#include <iterator>
//#define SHINY_PROFILER TRUE

using namespace TexGen;

CRectangularVoxelMesh::CRectangularVoxelMesh(string Type)
:CVoxelMesh(Type)
{
	//m_PeriodicBoundaries = new CPeriodicBoundaries;
}

CRectangularVoxelMesh::~CRectangularVoxelMesh(void)
{
	//delete m_PeriodicBoundaries;
}

bool CRectangularVoxelMesh::CalculateVoxelSizes(CTextile &Textile)
{
	XYZ DomSize;

	m_DomainAABB = Textile.GetDomain()->GetMesh().GetAABB();
	DomSize = m_DomainAABB.second - m_DomainAABB.first;
	
	m_VoxSize[0] = DomSize.x / m_XVoxels;
	m_VoxSize[1] = DomSize.y / m_YVoxels;
	m_VoxSize[2] = DomSize.z / m_ZVoxels;
	return true;
}

void CRectangularVoxelMesh::OutputNodes(ostream &Output, CTextile &Textile, int Filetype )
{
	int x,y,z;
	int iNodeIndex = 1;
	vector<XYZ> CentrePoints;
	vector<POINT_INFO> RowInfo;

	if ( Filetype == SCIRUN_EXPORT )  // if outputting in SCIRun format need to output number of voxels
		Output << (m_XVoxels+1)*(m_YVoxels+1)*(m_ZVoxels+1) << "\n";
	
	for ( z = 0; z <= m_ZVoxels; ++z )
	{
		for ( y = 0; y <= m_YVoxels; ++y )
		{
			for ( x = 0; x <=m_XVoxels; ++x )
			{
				XYZ Point;
				Point.x = m_DomainAABB.first.x + m_VoxSize[0] * x;
				Point.y = m_DomainAABB.first.y + m_VoxSize[1] * y;
				Point.z = m_DomainAABB.first.z + m_VoxSize[2] * z;
				if ( Filetype == INP_EXPORT )
					Output << iNodeIndex << ", ";

				if (Filetype == VTU_EXPORT)
					m_Mesh.AddNode(Point);
				else
					Output << Point << "\n";

				if ( x < m_XVoxels && y < m_YVoxels && z < m_ZVoxels )
				{
					Point.x += 0.5*m_VoxSize[0];
					Point.y += 0.5*m_VoxSize[1];
					Point.z += 0.5*m_VoxSize[2];
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
	//Textile.GetPointInformation( CentrePoints, m_ElementsInfo );
}