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
#include "SectionPolygon.h"
using namespace TexGen;

CSectionPolygon::CSectionPolygon(const vector<XY> &PolygonPoints, bool bSingleQuadrant)
: m_PolygonPoints(PolygonPoints)
{
	
	if (bSingleQuadrant)
	{
		vector<XY>::const_iterator itPoint;
		XY P;
		// Reflect to top left quadrant
		for (itPoint = PolygonPoints.end()-2; itPoint != PolygonPoints.begin(); --itPoint)
		{
			P = *itPoint;
			P.x *= -1;
			m_PolygonPoints.push_back(P);
		}
		// Reflect to bottom left quadrant
		for (itPoint = PolygonPoints.begin(); itPoint != PolygonPoints.end(); ++itPoint)
		{
			P = *itPoint;
			P.y *= -1;
			P.x *= -1;
			m_PolygonPoints.push_back(P);
		}
		// Reflect to bottom right quadrant
		for (itPoint = PolygonPoints.end()-2; itPoint != PolygonPoints.begin(); --itPoint)
		{
			P = *itPoint;
			P.y *= -1;
			m_PolygonPoints.push_back(P);
		}
	}

	CalcTValues();
}

CSectionPolygon::~CSectionPolygon(void)
{
}

bool CSectionPolygon::operator == (const CSection &CompareMe) const
{
	if (CompareMe.GetType() != GetType())
		return false;
	return m_PolygonPoints == ((CSectionPolygon*)&CompareMe)->m_PolygonPoints;
}

CSectionPolygon::CSectionPolygon(TiXmlElement &Element)
: CSection(Element)
{
	FOR_EACH_TIXMLELEMENT(pPoint, Element, "PolygonPoint")
	{
		m_PolygonPoints.push_back(valueify<XY>(pPoint->Attribute("value")));
	}
	CalcTValues();
}

void CSectionPolygon::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CSection::PopulateTiXmlElement(Element, OutputType);
	vector<XY>::const_iterator itPoint;
	for (itPoint = m_PolygonPoints.begin(); itPoint != m_PolygonPoints.end(); ++itPoint)
	{
		TiXmlElement Point("PolygonPoint");
		Point.SetAttribute("value", stringify(*itPoint));
		Element.InsertEndChild(Point);
	}
}

/*XY CSectionPolygon::GetPoint(double t) const
{
	t *= m_PolygonPoints.size();
	int iIndex = int(t);
	t -= iIndex;
	XY P1, P2;
	P1 = m_PolygonPoints[iIndex];
	P2 = m_PolygonPoints[(iIndex+1)%m_PolygonPoints.size()];
	return P1 + (P2-P1) * t;
}*/

XY CSectionPolygon::GetPoint(double t) const
{
	vector<double>::const_iterator itT;
	vector<XY>::const_iterator itPoint;

	// Find which points given t value lies between
	int i;
	for( itT = m_t.begin(), i = 0; itT != m_t.end(); ++itT, ++i )
	{
		if ( fabs(*itT - t) < 1e-10 )
			return( m_PolygonPoints[i] );
		if ( *itT > t )
			break;
	}
	XY P;
	double t2 = itT == m_t.end() ? 1.0 : m_t[i]; // If last point t = 1.0
	// Interpolate between points to find point for given t value	
	P = m_PolygonPoints[i-1] + ( (t - m_t[i-1]) / (t2 - m_t[i-1]) * (m_PolygonPoints[i%m_PolygonPoints.size()]-m_PolygonPoints[i-1]));
	return P;
}

string CSectionPolygon::GetDefaultName() const
{
	return "Polygon(N:" + stringify(m_PolygonPoints.size()) + ")";
}

void CSectionPolygon::Scale(XY Scale)
{
	vector<XY>::iterator itPoint;
	for (itPoint = m_PolygonPoints.begin(); itPoint != m_PolygonPoints.end(); ++itPoint)
	{
		itPoint->x *= Scale.x;
		itPoint->y *= Scale.y;
	}
}

void CSectionPolygon::Scale(double dScale)
{
	vector<XY>::iterator itPoint;
	for (itPoint = m_PolygonPoints.begin(); itPoint != m_PolygonPoints.end(); ++itPoint)
	{
		*itPoint *= dScale;
	}
}

void CSectionPolygon::CalcTValues()
{
// Find length of perimeter of section
	vector<XY>::const_iterator itPoint;
	double dTotalLength = 0.0;
	itPoint = m_PolygonPoints.begin();
	XY StartPoint = *itPoint;
	XY LastPoint = *itPoint;
	
	m_t.push_back( 0.0 );
	while( ++itPoint != m_PolygonPoints.end() )
	{
		dTotalLength += GetLength( *itPoint, LastPoint );
		m_t.push_back( dTotalLength );
		LastPoint = *itPoint;
	}
	dTotalLength += GetLength( LastPoint, StartPoint );

	// Assign t for each section point as proportion of distance around perimeter from start point
	vector<double>::iterator itT;
	for ( itT = m_t.begin(); itT != m_t.end(); ++itT )
	{
		*itT /= dTotalLength;
	}
}







