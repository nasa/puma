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

#include "PrecompiledHeaders.h"
#include "SectionMeshRectangular.h"

using namespace TexGen;
CSectionMeshRectangular::CSectionMeshRectangular(int iNumLayers, bool bTriangleCorners)
: m_iNumLayers(iNumLayers)
, m_bTriangleCorners(bTriangleCorners)
{
	if (iNumLayers != -1 && iNumLayers % 2 != 0)
	{
		assert(false);
		TGERROR("Warning: Rectangular mesh specified with an odd number of layers: " << iNumLayers);
	}
}

CSectionMeshRectangular::~CSectionMeshRectangular(void)
{
}

CSectionMeshRectangular::CSectionMeshRectangular(TiXmlElement &Element)
{
	Element.Attribute("NumLayers", &m_iNumLayers);
	m_bTriangleCorners = valueify<bool>(Element.Attribute("TriangleCorners"));
}

void CSectionMeshRectangular::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CSectionMesh::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("NumLayers", m_iNumLayers);
	Element.SetAttribute("TriangleCorners", stringify(m_bTriangleCorners));
}

bool CSectionMeshRectangular::CreateMesh(const vector<XY> &Section) const
{
	int iNumLayers = m_iNumLayers;
	if (m_iNumLayers == -1)
		iNumLayers = CalculateNumberofLayers(Section);
	else if (m_iNumLayers == 1)
		return CreateSingleLayerMesh(Section);
	if (iNumLayers % 2 != 0)
	{
		assert(false);
		TGERROR("Unable to create section mesh, the number of layers is odd: " << iNumLayers);
		return false;
	}
	if (Section.size() % 2 != 0)
	{
		assert(false);
		TGERROR("Unable to create section mesh, the number of sections points specified is odd: " << Section.size());
		return false;
	}
	int iNumColumns = Section.size()/2-iNumLayers;
	if (m_bTriangleCorners)
		iNumColumns += 2;
	if (iNumColumns < 1)
	{
		assert(false);
		TGERROR("Unable to create section mesh, the number of columns is less than 1: " << iNumColumns);
		return false;
	}
	int i, j;
	int iPointIndex, iNumPoints = (int)Section.size();

	vector<XY> Left;
	vector<XY> Right;
	vector<XY> Top;
	vector<XY> Bottom;

	if (m_bTriangleCorners)
		iNumColumns -= 2;
	for (i=0; i<iNumLayers+1; ++i) // Assemble right and left perimeter points
	{
		if (m_bTriangleCorners && (i == 0 || i == iNumLayers))
		{
			// Insert dummy points at the corners
			Right.push_back(XY());
			Left.push_back(XY());
			continue;
		}
		if (i < iNumLayers/2)
			iPointIndex = iNumPoints-iNumLayers/2+i;
		else
			iPointIndex = i-iNumLayers/2;
		Right.push_back(Section[iPointIndex]);

		iPointIndex = 3*(iNumLayers/2)+iNumColumns-i;
		Left.push_back(Section[iPointIndex]);
	}
	if (m_bTriangleCorners)
		iNumColumns += 2;

	if (m_bTriangleCorners)
		iNumLayers -= 2;
	for (i=0; i<iNumColumns+1; ++i) // Assemble top and bottom perimeter points
	{
		if (m_bTriangleCorners && (i == 0 || i == iNumColumns))
		{
			// Insert dummy points at the corners
			Top.push_back(XY());
			Bottom.push_back(XY());
			continue;
		}
		iPointIndex = iNumLayers/2+iNumColumns-i;
		Top.push_back(Section[iPointIndex]);

		iPointIndex = 3*(iNumLayers/2)+iNumColumns+i;
		Bottom.push_back(Section[iPointIndex]);
	}
	if (m_bTriangleCorners)
		iNumLayers += 2;

	m_Mesh.Clear();
	m_Mesh.SetNumNodes((iNumLayers+1)*(iNumColumns+1));
	double u, v;
	double w1, w2;
	double l1, l2;
	double d1, d2;
	XY P, P1, P2;
	for (i=0; i<iNumLayers+1; ++i)
	{
		u = double(i) / double(iNumLayers);
		for (j=0; j<iNumColumns+1; ++j)
		{
			v = double(j) / double(iNumColumns);
			P1 = Left[i] + (Right[i] - Left[i])*v;  // MS thesis equ 2.56
			P2 = Bottom[j] + (Top[j] - Bottom[j])*u;  // equ 2.57
			d1 = 0.5-abs(v-0.5);  // 2nd half of equ 2.58 corrected to use v not u as in thesis
			d2 = 0.5-abs(u-0.5);  // 2nd half of equ 2.59 corrected to use u not v
			l1 = GetLength(Left[i], Right[i]);
			l2 = GetLength(Bottom[j], Top[j]);
			if (l1)
				d1 *= l1;  // equ 2.58
			if (l2)
				d2 *= l2;  // equ 2.59
			if (d1 + d2 == 0)		// Avoid division by 0
			{
				w1 = w2 = 0.5;
			}
			else
			{
				w1 = d2 / (d1 + d2);  // equ 2.60
				w2 = d1 / (d1 + d2);  // equ 2.61
			}
			P = P1 * w1 + P2 * w2;  // equ 2.62
			m_Mesh.SetNode(j+i*(iNumColumns+1), XYZ(P.x, P.y, 0));
		}
	}
	for (i=0; i<iNumLayers; ++i)
	{
		for (j=0; j<iNumColumns; ++j)
		{
			if (m_bTriangleCorners)
			{
				if ((i==0 || i==iNumLayers-1) && (j==0 || j==iNumColumns-1))
				{
					vector<int> Indices;
					if (i!=0 || j!=0)
						Indices.push_back((j+0)+(i+0)*(iNumColumns+1));
					if (i!=0 || j!=iNumColumns-1)
						Indices.push_back((j+1)+(i+0)*(iNumColumns+1));
					if (i!=iNumLayers-1 || j!=iNumColumns-1)
						Indices.push_back((j+1)+(i+1)*(iNumColumns+1));
					if (i!=iNumLayers-1 || j!=0)
						Indices.push_back((j+0)+(i+1)*(iNumColumns+1));
					m_Mesh.AddElement(CMesh::TRI, Indices);
					continue;
				}
			}
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

	if (m_bTriangleCorners)
	{
		// Remove the four corner dummy nodes...
		int iRemoved = m_Mesh.RemoveUnreferencedNodes();
		assert(iRemoved == 4);
	}

	return true;
}

int CSectionMeshRectangular::CalculateNumberofLayers(const vector<XY> &Section) const
{
	// Section may either be rotated section or not start from point on x axis
	// in which case the points need rotating back otherwise test for number of layers is invalid
	XY p1, p2, dp;
	p1 = Section[0];
	p2 = Section[Section.size()/2];
	dp = p1 - p2;
	double dAngle = atan2( dp.y, dp.x );

	vector<XY> RotatedSection;
	for ( int i = 1; i <= (int)Section.size()/4+1; ++i )
	{
		XY RotPoint;
		RotPoint.x = Section[i].x*cos(dAngle) + Section[i].y*sin(dAngle);
		RotPoint.y = Section[i].y*cos(dAngle) - Section[i].x*sin(dAngle);
		RotatedSection.push_back(RotPoint);
	}

	// Assuming the section is symmetric about all 4 quadrants
	// 25-9-12 Can't assume this. Both hybrid and polygon sections may result in non-symmetrical sections
	int i;
	XY P1, P2, DP;
	vector<XY>::iterator itRotatedSection;
	//for (i=1; i<=(int)RotatedSection.size()/4; ++i)
	for ( i = 0; i < (int)RotatedSection.size()-1; ++i )
	{
		P1 = RotatedSection[i];
		P2 = RotatedSection[i+1];
		DP = P2 - P1;
		if (abs(DP.x) > abs(DP.y))
		{
			return (i+1)*2;  // +1 because RotatedSection starts at 2nd point in Section
		}
	}
	return i*2;
	// Should never reach this point
	//return 0;
}

bool CSectionMeshRectangular::CreateSingleLayerMesh(const vector<XY> &Section) const
{
	if (Section.size() % 2 != 0)
	{
		assert(false);
		TGERROR("Unable to create section mesh, the number of sections points specified is odd: " << Section.size());
		return false;
	}

	int iNumColumns = Section.size()/2;

	m_Mesh.Clear();
	int i, j;
	for (i=0; i<(int)Section.size(); ++i)
	{
		XY pt = Section[i];
		m_Mesh.AddNode(XYZ(pt.x, pt.y, 0));
	}

	// Build up a single row of elements, two triangles at either end and rectangles in the middle
	for (j=0; j<iNumColumns; ++j)
	{
		if (j==0)
		{
			// Triangle at the right edges
			vector<int> Indices;
			Indices.push_back(0);
			Indices.push_back(1);
			Indices.push_back(Section.size()-1);
			m_Mesh.AddElement(CMesh::TRI, Indices);
		}
		else if (j==iNumColumns-1)
		{
			// Triangle at the left edge
			vector<int> Indices;
			Indices.push_back(iNumColumns-1);
			Indices.push_back(iNumColumns);
			Indices.push_back(iNumColumns+1);
			m_Mesh.AddElement(CMesh::TRI, Indices);
		}
		else
		{
			// Rectangles in the middle section
			vector<int> Indices;
			Indices.push_back(j);
			Indices.push_back(j+1);
			Indices.push_back(Section.size()-j-1);
			Indices.push_back(Section.size()-j);
			m_Mesh.AddElement(CMesh::QUAD, Indices);
		}
	}

	bool bSurfaceNodes = true; // Add this to class later
	if ( bSurfaceNodes )
	{
		vector<int> Indices;
		for ( j = 0; j < (int)Section.size(); ++j )
		{
			Indices.push_back(j);
		}
		Indices.push_back(Indices[0]);
		m_Mesh.AddElement(CMesh::POLYGON, Indices);
	}

	return true;
}

void CSectionMeshRectangular::SetNumLayers( int iNum )
{
	m_iNumLayers = iNum;
}




