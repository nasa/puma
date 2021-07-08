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
#include "SectionRectangle.h"
using namespace TexGen;

CSectionRectangle::CSectionRectangle(double dWidth, double dHeight)
: m_dWidth(dWidth)
, m_dHeight(dHeight)
{
	m_pSectionMesh = CSectionMeshRectangleSection();
}

CSectionRectangle::~CSectionRectangle(void)
{
}

bool CSectionRectangle::operator == (const CSection &CompareMe) const
{
	if (CompareMe.GetType() != GetType())
		return false;
	return m_dWidth == ((CSectionRectangle*)&CompareMe)->m_dWidth &&
		m_dHeight == ((CSectionRectangle*)&CompareMe)->m_dHeight;
}

CSectionRectangle::CSectionRectangle(TiXmlElement &Element)
: CSection(Element)
{
	Element.Attribute("Width", &m_dWidth);
	Element.Attribute("Height", &m_dHeight);
}

void CSectionRectangle::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CSection::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("Width", stringify(m_dWidth));
	Element.SetAttribute("Height", stringify(m_dHeight));
}

XY CSectionRectangle::GetPoint(double t) const
{	
	double dTotalLength = 2 * m_dWidth + 2 * m_dHeight;
	double dLength = t * dTotalLength;
	double dPartLength = m_dHeight/2.0;
	if ( dLength <= dPartLength )
	{
		return XY( m_dWidth/2.0, dLength );
	}
	dPartLength += m_dWidth;
	if ( dLength <= dPartLength )
	{
		return XY( m_dWidth/2.0 - (dLength - m_dHeight/2.0), m_dHeight/2.0 );
	}
	dPartLength += m_dHeight;
	if ( dLength <= dPartLength )
	{
		return XY( -m_dWidth/2.0, m_dHeight/2.0 - (dLength - m_dWidth - m_dHeight/2.0) ); 
	}
	dPartLength += m_dWidth;
	if ( dLength <= dPartLength )
	{
		return XY( -m_dWidth/2.0 + (dLength - m_dWidth - 1.5*m_dHeight), -m_dHeight/2.0 );
	}
	return XY( m_dWidth/2.0, -(dTotalLength - dLength) );
}

string CSectionRectangle::GetDefaultName() const
{
	return "Rectangle(W:" + stringify(m_dWidth) + ",H:" + stringify(m_dHeight) + ")";
}

const vector<XY> &CSectionRectangle::GetPoints(int iNumPoints, bool bEquiSpaced) const
{
	if (iNumPoints != (int)m_EdgePoints.size() ) // Doesn't do anything with equispaced at the moment || bEquiSpaced != m_bEquiSpaced)
	{
		m_EdgePoints.resize(iNumPoints);
		if ( iNumPoints == 2 )  // If only 2 points requested want to force them to be on centre plane
		{
			m_EdgePoints[0].x = m_dWidth/2.0;
			m_EdgePoints[0].y = 0.0;

			m_EdgePoints[1].x = -m_dWidth/2.0;
			m_EdgePoints[1].y = 0.0;
			return m_EdgePoints;
		}

		double dTotalLength = 2 * m_dWidth + 2 * m_dHeight;
		double dAveSpacing = dTotalLength / (iNumPoints-1);

		int iNumYSpaces = (int)(m_dHeight / dAveSpacing);
		if ( ( iNumYSpaces % 2 ) || iNumYSpaces == 0 )
		{
			iNumYSpaces++;
		}
		double dYSpacing = m_dHeight/iNumYSpaces;

		int iNumXSpaces = iNumPoints/2 - iNumYSpaces;
		double dXSpacing = m_dWidth/iNumXSpaces;

		int j = 0;
		XY SectionPoint( m_dWidth/2.0, 0.0 );
		m_EdgePoints[j] = SectionPoint;
		++j;

		for ( int i=0; i < iNumYSpaces/2; ++i )
		{
			SectionPoint.y += dYSpacing;
			m_EdgePoints[j] = SectionPoint;
			++j;
		}
		for ( int i = 0; i < iNumXSpaces; ++i )
		{
			SectionPoint.x -= dXSpacing;
			m_EdgePoints[j] = SectionPoint;
			++j;
		}
		for ( int i = 0; i < iNumYSpaces; ++i )
		{
			SectionPoint.y -= dYSpacing;
			m_EdgePoints[j] = SectionPoint;
			++j;
		}
		for ( int i = 0; i < iNumXSpaces; ++i )
		{
			SectionPoint.x += dXSpacing;
			m_EdgePoints[j] = SectionPoint;
			++j;
		}
		for ( int i = 0; i < (iNumYSpaces/2)-1; ++i )
		{
			SectionPoint.y += dYSpacing;
			m_EdgePoints[j] = SectionPoint;
			++j;
		}
	}
	return m_EdgePoints;
}