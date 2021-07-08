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
#include "YarnSectionInterp.h"

using namespace TexGen;
CYarnSectionInterp::CYarnSectionInterp(bool bRamped, bool bPolar, bool bConstMesh)
:CYarnSection(bConstMesh)
, m_bRamped(bRamped)
, m_bPolar(bPolar)
{
}

CYarnSectionInterp::~CYarnSectionInterp(void)
{
}

CYarnSectionInterp::CYarnSectionInterp(TiXmlElement &Element)
: CYarnSection(Element)
{
	m_bRamped = valueify<bool>(Element.Attribute("Ramped"));
	m_bPolar = valueify<bool>(Element.Attribute("Polar"));
}

void CYarnSectionInterp::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CYarnSection::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("Ramped", stringify(m_bRamped));
	Element.SetAttribute("Polar", stringify(m_bPolar));
}

bool CYarnSectionInterp::GetInterpedSection(const CSection &Section1, const CSection &Section2, double u, int iNumPoints, bool bEquiSpaced, vector<XY> &Points) const
{
	Points.clear();

	const vector<XY> &Points1 = Section1.GetPoints(iNumPoints, bEquiSpaced);
	const vector<XY> &Points2 = Section2.GetPoints(iNumPoints, bEquiSpaced);

	int i;

//	XY InterpedPoint, P1, P2;
	for (i=0; i<iNumPoints; ++i)
	{
		Points.push_back(InterpolatePoints(Points1[i], Points2[i], u));
//		P1 = Points1[i];
//		P2 = Points2[i];
//		InterpedPoint = P1+(P2-P1)*u;
//		Points.push_back(InterpedPoint);
	}

	return true;

}

bool CYarnSectionInterp::GetInterpedSectionMesh(const CSection &Section1, const CSection &Section2, double u, int iNumPoints, bool bEquiSpaced, CMesh &Mesh) const
{
	Mesh.Clear();

	const CMesh &Mesh1 = Section1.GetMesh(iNumPoints, bEquiSpaced);
	const CMesh &Mesh2 = Section2.GetMesh(iNumPoints, bEquiSpaced);

	bool bCompatible = Mesh1.GetNumNodes() == Mesh2.GetNumNodes();
	int i;
	for (i = 0; i < CMesh::NUM_ELEMENT_TYPES; ++i)
	{
		if (!bCompatible)
			break;
		if (Mesh1.GetIndices((CMesh::ELEMENT_TYPE)i) != Mesh2.GetIndices((CMesh::ELEMENT_TYPE)i))
			bCompatible = false;
	}

	if (!bCompatible)
	{
		// The two meshes to be interpolated are not compatible
		TGERROR("Unable to interpolate, section meshes are not compatible: " << Section1.GetDefaultName() << ", " << Section2.GetDefaultName());
		//assert(false);
		return false;
	}

	XY InterpedPoint, P1, P2;
	for (i=0; i<(int)Mesh1.GetNumNodes(); ++i)		// Note: Mesh1.m_Nodes.size() == Mesh2.m_Nodes.size()
	{
		P1.x = Mesh1.GetNode(i).x;
		P1.y = Mesh1.GetNode(i).y;
		P2.x = Mesh2.GetNode(i).x;
		P2.y = Mesh2.GetNode(i).y;
		InterpedPoint = InterpolatePoints(P1, P2, u);
		Mesh.AddNode(XYZ(InterpedPoint.x, InterpedPoint.y, 0));
	}

	// Note: Mesh1.m_Indices == Mesh2.m_Indices
	for (i=0; i<CMesh::NUM_ELEMENT_TYPES; ++i)
	{
		Mesh.GetIndices((CMesh::ELEMENT_TYPE)i) = Mesh1.GetIndices((CMesh::ELEMENT_TYPE)i);
	}

	return true;
}

XY CYarnSectionInterp::InterpolatePoints(XY P1, XY P2, double u) const
{
	if (m_bRamped)
		u = 3 * u * u - 2 * u * u * u;
	if (m_bPolar)
	{
		double l1, l2, l;
		double a1, a2, a;
		l1 = GetLength(P1);
		l2 = GetLength(P2);
		l = l1+(l2-l1)*u;
		a1 = atan2(P1.y, P1.x);
		a2 = atan2(P2.y, P2.x);
		if (abs(a1-a2) > PI)
		{
			if (a1 < a2)
				a1+=2*PI;
			else
				a2+=2*PI;
		}
		a = a1+(a2-a1)*u;
		XY P;
		P.x = l*cos(a);
		P.y = l*sin(a);
		return P;
	}
	else
		return P1+(P2-P1)*u;
}

int CYarnSectionInterp::CalculateNumberofLayers(const vector<XY> &Section) const
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

