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
#include "SectionMeshRectangleSection.h"

using namespace TexGen;
CSectionMeshRectangleSection::CSectionMeshRectangleSection(int iNumLayers)
: m_iNumLayers(iNumLayers)
{
	/*if (iNumLayers != -1 && iNumLayers % 2 != 0)
	{
		assert(false);
		TGERROR("Warning: Rectangular mesh specified with an odd number of layers: " << iNumLayers);
	}*/
}

CSectionMeshRectangleSection::~CSectionMeshRectangleSection(void)
{
}

CSectionMeshRectangleSection::CSectionMeshRectangleSection(TiXmlElement &Element)
{
	Element.Attribute("NumLayers", &m_iNumLayers);
}

void CSectionMeshRectangleSection::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CSectionMesh::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("NumLayers", m_iNumLayers);
}

bool CSectionMeshRectangleSection::CreateMesh(const vector<XY> &Section) const
{
	if (Section.size() % 2 != 0)
	{
		assert(false);
		TGERROR("Unable to create section mesh, the number of sections points specified is odd: " << Section.size());
		return false;
	}
	
	XY p1, p2, dp;
	p1 = Section[0];
	p2 = Section[Section.size()/2];
	dp = p1 - p2;
	double dAngle = atan2( dp.y, dp.x );
	bool bRotated = false;

	vector<XY> NewSection;
	if ( fabs( dAngle ) > 0.0000001 )
	{
		bRotated = true;
		for ( int i = 0; i < (int)Section.size(); ++i )
		{
			XY RotPoint;
			RotPoint.x = Section[i].x*cos(dAngle) + Section[i].y*sin(dAngle);
			RotPoint.y = Section[i].y*cos(dAngle) - Section[i].x*sin(dAngle);
			NewSection.push_back(RotPoint);
		}
	}
	else
	{
		NewSection.insert(NewSection.begin(), Section.begin(), Section.end() );
	}

	int iNumLayers = m_iNumLayers;
	if (m_iNumLayers == -1)
		iNumLayers = CalculateNumberofLayers(NewSection);
	
	
	int iNumColumns = NewSection.size()/2-iNumLayers;
	if (iNumColumns < 1)
	{
		assert(false);
		TGERROR("Unable to create section mesh, the number of columns is less than 1: " << iNumColumns);
		return false;
	}
	int i, j;

	vector<double> YPoints;
	vector<double> XPoints;

	XY Min, Max;
	GetMinMaxXY( NewSection, Min, Max );

	double dXSpacing = (Max.x - Min.x)/iNumColumns;
	double dYSpacing = (Max.y - Min.y)/iNumLayers;

	double YPoint = Min.y;
	
	for (i=0; i<iNumLayers+1; ++i) // Assemble vertical points
	{
		YPoints.push_back(YPoint);
		YPoint += dYSpacing;
	}
	
	double XPoint = Min.x;

	for (i=0; i<iNumColumns+1; ++i) // Assemble top and bottom perimeter points
	{
		XPoints.push_back(XPoint);
		XPoint += dXSpacing;
	}

	m_Mesh.Clear();
	m_Mesh.SetNumNodes((iNumLayers+1)*(iNumColumns+1));
	
	for (i=0; i<iNumLayers+1; ++i)
	{
		for (j=0; j<iNumColumns+1; ++j)
		{
			XYZ Point( XPoints[j], YPoints[i], 0 );
			if ( bRotated )
			{
				XYZ RotPoint;
				RotPoint.x = Point.x*cos(-dAngle) + Point.y*sin(-dAngle);
				RotPoint.y = Point.y*cos(-dAngle) - Point.x*sin(-dAngle);
				Point = RotPoint;
			}
			//m_Mesh.SetNode(j+i*(iNumColumns+1), XYZ(XPoints[j], YPoints[i], 0));
			m_Mesh.SetNode(j+i*(iNumColumns+1), Point);
		}
	}

	for (i=0; i<iNumLayers; ++i)
	{
		for (j=0; j<iNumColumns; ++j)
		{
			m_Mesh.GetIndices(CMesh::QUAD).push_back((j+0)+(i+0)*(iNumColumns+1));
			m_Mesh.GetIndices(CMesh::QUAD).push_back((j+1)+(i+0)*(iNumColumns+1));
			m_Mesh.GetIndices(CMesh::QUAD).push_back((j+1)+(i+1)*(iNumColumns+1));
			m_Mesh.GetIndices(CMesh::QUAD).push_back((j+0)+(i+1)*(iNumColumns+1));
		}
	}
	bool bSurfaceNodes = true; // Add this to class later
	if ( bSurfaceNodes )
	{
		vector<int> Indices;
		for ( j = 1; j < iNumColumns; ++j )
		{
			Indices.push_back(j);  // Nodes along top
		}
		for ( i = 1; i < iNumLayers; ++i )
		{
			Indices.push_back( iNumColumns + i * (iNumColumns+1) );  // Nodes down right side
		}
		for ( j = iNumColumns-1; j > 0; --j )
		{
			Indices.push_back( j + iNumLayers * (iNumColumns+1) );  // Nodes across bottom (right to left)
		}
		for ( i = iNumLayers-1; i > 0; --i )
		{
			Indices.push_back( i * (iNumColumns+1) );  // Nodes up left side
		}
		Indices.push_back( Indices[0] );
		m_Mesh.AddElement(CMesh::POLYGON, Indices);
	}

	return true;
}

int CSectionMeshRectangleSection::CalculateNumberofLayers(const vector<XY> &Section) const
{
	// Assuming the section is symmetric about all 4 quadrants
	int i;
	XY P1, P2, DP;
	for (i=0; i<(int)Section.size(); ++i)
	{
		P1 = Section[i];
		P2 = Section[i+1];
		DP = P2 - P1;
		if (abs(DP.x) > abs(DP.y))
		{
			if ( i == 0 )
				return 1;  // Single layer mesh
			return i*2;
		}
	}
	// Should never reach this point
	return 0;
}

void CSectionMeshRectangleSection::SetNumLayers( int iNum )
{
	m_iNumLayers = iNum;
}

/*void CSectionMeshRectangleSection::GetMinMaxXY( const vector<XY> &Section, XY &Min, XY &Max ) const
{
	Min = Section[0];
	Max = Section[0];
	for ( int i = 1; i < (int)Section.size(); ++i )
	{
		if ( Section[i].x < Min.x )
			Min.x = Section[i].x;
		else if ( Section[i].x > Max.x )
			Max.x = Section[i].x;

		if ( Section[i].y < Min.y )
			Min.y = Section[i].y;
		else if ( Section[i].y > Max.y )
			Max.y = Section[i].y;
	}
}*/