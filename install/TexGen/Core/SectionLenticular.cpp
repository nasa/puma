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
#include "SectionLenticular.h"
using namespace TexGen;

CSectionLenticular::CSectionLenticular(double dWidth, double dHeight, double dDistortion)
: m_dWidth(dWidth)
, m_dHeight(dHeight)
, m_dDistortion(dDistortion)
{
	if (dHeight > dWidth)
		TGERROR("Warning: Lenticular section has greater height than width");
	if (abs(dDistortion) > 0.5*dHeight+1e-9)
		TGERROR("Warning: Lenticular section distortion is greater than half the height");
}

CSectionLenticular::~CSectionLenticular(void)
{
}

bool CSectionLenticular::operator == (const CSection &CompareMe) const
{
	if (CompareMe.GetType() != GetType())
		return false;
	return m_dWidth == ((CSectionLenticular*)&CompareMe)->m_dWidth &&
		m_dHeight == ((CSectionLenticular*)&CompareMe)->m_dHeight &&
		m_dDistortion == ((CSectionLenticular*)&CompareMe)->m_dDistortion;
}

CSectionLenticular::CSectionLenticular(TiXmlElement &Element)
: CSection(Element)
, m_dWidth(0)
, m_dHeight(0)
, m_dDistortion(0)
{
	// For backwards compatibility, previously lenticular shapes could
	// not be distorted
	if (Element.Attribute("Radius") && Element.Attribute("Offset"))
	{
		double dRadius, dOffset;
		Element.Attribute("Radius", &dRadius);
		Element.Attribute("Offset", &dOffset);
		m_dWidth = 2*sqrt(dRadius*dRadius-dOffset*dOffset);
		m_dHeight = 2*(dRadius-dOffset);
		m_dDistortion = 0;
	}
	else
	{
		Element.Attribute("Width", &m_dWidth);
		Element.Attribute("Height", &m_dHeight);
		Element.Attribute("Distortion", &m_dDistortion);
	}
}

void CSectionLenticular::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CSection::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("Width", stringify(m_dWidth));
	Element.SetAttribute("Height", stringify(m_dHeight));
	Element.SetAttribute("Distortion", stringify(m_dDistortion));
}

XY CSectionLenticular::GetPoint(double t) const
{
	XY Point;
	double dH;
	double dRadius;
	double dO;
	double dMaxAngle;
	double dAngle;

	if (t < 0.5)
		dH = m_dHeight-2*m_dDistortion;
	else
		dH = m_dHeight+2*m_dDistortion;

	if (abs(dH) < 1e-9)
	{
		if (t < 0.5)
		{
			Point.x = 0.5*m_dWidth-2*t*m_dWidth;
			Point.y = 0.5*m_dHeight;
		}
		else
		{
			Point.x = 0.5*m_dWidth-2*(1-t)*m_dWidth;
			Point.y = -0.5*m_dHeight;
		}
	}
	else
	{
		dRadius = (m_dWidth*m_dWidth + dH*dH) / (4*dH);
		dO = dRadius-0.5*m_dHeight;
		dMaxAngle = asin(0.5*m_dWidth/dRadius);

		if (t < 0.5)
		{
			dAngle = (1-4*t)*dMaxAngle;
			Point.x = dRadius*sin(dAngle);
			Point.y = dRadius*cos(dAngle)-dO;
		}
		else
		{
			dAngle = (-3+4*t)*dMaxAngle;
			Point.x = dRadius*sin(dAngle);
			Point.y = -(dRadius*cos(dAngle)-dO);
		}
	}

	return Point;
}

string CSectionLenticular::GetDefaultName() const
{
	 return "Lenticular(W:" + stringify(m_dWidth) + ",H:" + stringify(m_dHeight) + ",D:" + stringify(m_dDistortion) + ")";
}






