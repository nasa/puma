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
#include "SectionRotated.h"

using namespace TexGen;
CSectionRotated::CSectionRotated(const CSection &Section, double dAngle)
: m_dAngle(dAngle),m_pSection(Section)
{
	if ( m_pSection->GetType() == "CSectionRectangle" )
		m_pSectionMesh = CSectionMeshRectangleSection();
}

CSectionRotated::~CSectionRotated(void)
{
}

bool CSectionRotated::operator == (const CSection &CompareMe) const
{
	if (CompareMe.GetType() != GetType())
		return false;
	return m_dAngle == ((CSectionRotated*)&CompareMe)->m_dAngle &&
		*m_pSection == *((CSectionRotated*)&CompareMe)->m_pSection;
}

CSectionRotated::CSectionRotated(TiXmlElement &Element)
: CSection(Element)
{
	Element.Attribute("Angle", &m_dAngle);
	TiXmlElement* pSection = Element.FirstChildElement("Section");
	if (pSection)
		m_pSection = CreateSection(*pSection);
}

void CSectionRotated::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CSection::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("Angle", stringify(m_dAngle));
	TiXmlElement Section("Section");
	m_pSection->PopulateTiXmlElement(Section, OutputType);
	Element.InsertEndChild(Section);
}

string CSectionRotated::GetDefaultName() const
{
	return "Rotated(" + m_pSection->GetDefaultName() + ",R:" + stringify(m_dAngle) + ")";
}

string CSectionRotated::GetBaseType() const
{
	return m_pSection->GetType();
}

XY CSectionRotated::GetPoint(double t) const
{
	XY Point = m_pSection->GetPoint(t);
	XY RotatedPoint;
	RotatedPoint.x = Point.x*cos(m_dAngle) - Point.y*sin(m_dAngle);
	RotatedPoint.y = Point.x*sin(m_dAngle) + Point.y*cos(m_dAngle);
	return RotatedPoint;
}






