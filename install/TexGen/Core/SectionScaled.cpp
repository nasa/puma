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
#include "SectionScaled.h"

using namespace TexGen;
CSectionScaled::CSectionScaled(const CSection &Section, XY Scale)
:  m_Scale(Scale), m_pSection(Section)
{
}

CSectionScaled::~CSectionScaled(void)
{
}

bool CSectionScaled::operator == (const CSection &CompareMe) const
{
	if (CompareMe.GetType() != GetType())
		return false;
	return m_Scale == ((CSectionScaled*)&CompareMe)->m_Scale &&
		*m_pSection == *((CSectionScaled*)&CompareMe)->m_pSection;
}

CSectionScaled::CSectionScaled(TiXmlElement &Element)
: CSection(Element)
{
	m_Scale = valueify<XY>(Element.Attribute("Scale"));
	TiXmlElement* pSection = Element.FirstChildElement("Section");
	if (pSection)
		m_pSection = CreateSection(*pSection);
}

void CSectionScaled::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CSection::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("Scale", stringify(m_Scale));
	TiXmlElement Section("Section");
	m_pSection->PopulateTiXmlElement(Section, OutputType);
	Element.InsertEndChild(Section);
}


string CSectionScaled::GetDefaultName() const
{
	return "Scaled(" + m_pSection->GetDefaultName() + ",S:" + stringify(m_Scale) + ")";
}

XY CSectionScaled::GetPoint(double t) const
{
	XY Point = m_pSection->GetPoint(t);
	XY ScalePoint;
	ScalePoint.x = Point.x * m_Scale.x;
	ScalePoint.y = Point.y * m_Scale.y;
	return ScalePoint;
}











