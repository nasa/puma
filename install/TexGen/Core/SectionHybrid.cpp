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
#include "SectionHybrid.h"
#include "SectionEllipse.h"
#include "SectionRotated.h"

using namespace TexGen;
CSectionHybrid::CSectionHybrid(void)
{
	// m_Sections should never be emtpy
	m_Sections.resize(1, CSectionEllipse(1, 1));
}

CSectionHybrid::CSectionHybrid(const CSection &TopHalf, const CSection &BottomHalf)
{
	AddDivision(0);
	AddDivision(0.5);
	AssignSection(0, TopHalf);
	AssignSection(1, BottomHalf);
}

CSectionHybrid::CSectionHybrid(const CSection &TopRight, const CSection &TopLeft, const CSection &BottomLeft, const CSection &BottomRight)
{
	AddDivision(0);
	AddDivision(0.25);
	AddDivision(0.5);
	AddDivision(0.75);
	AssignSection(0, TopRight);
	AssignSection(1, TopLeft);
	AssignSection(2, BottomLeft);
	AssignSection(3, BottomRight);
}

CSectionHybrid::~CSectionHybrid(void)
{
}

bool CSectionHybrid::operator == (const CSection &CompareMe) const
{
	if (CompareMe.GetType() != GetType())
		return false;
	return (m_Divisions == ((CSectionHybrid*)&CompareMe)->m_Divisions &&
		m_Sections == ((CSectionHybrid*)&CompareMe)->m_Sections);
}

CSectionHybrid::CSectionHybrid(TiXmlElement &Element)
: CSection(Element)
{
	FOR_EACH_TIXMLELEMENT(pDivision, Element, "Division")
	{
		double dVal = 0;
		pDivision->Attribute("Value", &dVal);
		AddDivision(dVal);
	}
	int i = 0;
	FOR_EACH_TIXMLELEMENT(pSection, Element, "Section")
	{
		AssignSection(i++, *CreateSection(*pSection));
	}
}

void CSectionHybrid::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CSection::PopulateTiXmlElement(Element, OutputType);
	int i;
	for (i=0; i<(int)m_Divisions.size(); ++i)
	{
		TiXmlElement Division("Division");
		Division.SetAttribute("Value", stringify(m_Divisions[i]));
		Element.InsertEndChild(Division);
	}
	for (i=0; i<(int)m_Sections.size(); ++i)
	{
		TiXmlElement Section("Section");
		m_Sections[i]->PopulateTiXmlElement(Section, OutputType);
		Element.InsertEndChild(Section);
	}
}

void CSectionHybrid::AddDivision(double dFraction)
{
	m_Divisions.push_back(dFraction);
	sort(m_Divisions.begin(), m_Divisions.end());
	m_Sections.resize(max((int)m_Divisions.size(), 1), CSectionEllipse(1, 1));
}

bool CSectionHybrid::AssignSection(int iIndex, const CSection &Section)
{
	if (iIndex < 0 || iIndex >= (int)m_Sections.size())
	{
		TGERROR("Unable to assign section, index invalid: " << iIndex);
		return false;
	}
	m_Sections[iIndex] = Section;
	return true;
}

XY CSectionHybrid::GetPoint(double t) const
{
	vector<double>::const_iterator itDiv;
	int i;
	for (itDiv = m_Divisions.begin(), i=0; itDiv != m_Divisions.end(); ++itDiv, ++i)
	{
		if (t <= *itDiv)
			break;
	}
	--i;
	if (i < 0)
		i = max((int)m_Divisions.size()-1, 0);

	assert(i >= 0 && i < (int)m_Sections.size());

	if ( m_Sections[i]->GetType() == "CSectionRotated" )
	{
		CSectionRotated* rotSection = (CSectionRotated*)m_Sections[i]->Copy();
		double angle = rotSection->GetAngle();
		double t_angle = t*2*PI;
		double new_angle = t_angle - angle;
		if ( new_angle < 0.0 )
			new_angle += (2*PI);
		t = new_angle / (2*PI);
		delete rotSection;
	}

	return m_Sections[i]->GetPoint(t);
}

string CSectionHybrid::GetDefaultName() const
{
	 return "Hybrid(N:" + stringify(m_Sections.size()) + ")";
}

double CSectionHybrid::GetDivision(int iIndex) const
{
	assert(iIndex>=0 && iIndex<(int)m_Divisions.size());
	return m_Divisions[iIndex];
}

const CSection &CSectionHybrid::GetSection(int iIndex) const
{
	assert(iIndex>=0 && iIndex<(int)m_Sections.size());
	return *m_Sections[iIndex];
}






