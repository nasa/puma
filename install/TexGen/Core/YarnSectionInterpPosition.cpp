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
#include "YarnSectionInterpPosition.h"
//#include "Section.h"
using namespace TexGen;

CYarnSectionInterpPosition::CYarnSectionInterpPosition(bool bRamped, bool bPolar, bool bConstMesh)
: CYarnSectionInterp(bRamped, bPolar, bConstMesh)
{
}

CYarnSectionInterpPosition::~CYarnSectionInterpPosition(void)
{
}

CYarnSectionInterpPosition::CYarnSectionInterpPosition(TiXmlElement &Element)
: CYarnSectionInterp(Element)
{
	FOR_EACH_TIXMLELEMENT(pPositionSection, Element, "PositionSection")
	{
		TiXmlElement* pSection = pPositionSection->FirstChildElement("Section");
		if (pSection)
		{
			m_Sections.push_back(make_pair(
				valueify<double>(pPositionSection->Attribute("t")),
				CSection::CreateSection(*pSection)
				));
		}
	}
}

void CYarnSectionInterpPosition::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CYarnSectionInterp::PopulateTiXmlElement(Element, OutputType);

	int i;
	for (i=0; i<(int)m_Sections.size(); ++i)
	{
		TiXmlElement PositionSection("PositionSection");
		PositionSection.SetAttribute("t", stringify(m_Sections[i].first));
		TiXmlElement Section("Section");
		m_Sections[i].second->PopulateTiXmlElement(Section, OutputType);
		PositionSection.InsertEndChild(Section);
		Element.InsertEndChild(PositionSection);
	}
}

vector<XY> CYarnSectionInterpPosition::GetSection(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced) const
{
	int iSection1 = -1;
	int iSection2 = -1;
	double u;
	double dYarnPosition = PositionInfo.GetYarnPosition();

	int i;
	for (i = 0; i<(int)m_Sections.size(); ++i)
	{
		if (m_Sections[i].first <= dYarnPosition)
			iSection1 = i;
		if (m_Sections[i].first >= dYarnPosition && iSection2 == -1)
			iSection2 = i;
	}

	if (iSection1 == -1 && iSection2 == -1)
	{
		TGERROR("Unable to get section, no sections specified");
		assert(false);
		return vector<XY>();
	}

	double du1, du2;
	if (iSection1 != -1 && iSection2 != -1)
	{
		du1 = m_Sections[iSection1].first;
		du2 = m_Sections[iSection2].first;
	}
	else if (iSection1 == -1)
	{
		iSection1 = m_Sections.size()-1;
		du1 = m_Sections[iSection1].first-1;
		du2 = m_Sections[iSection2].first;
	}
	else
	{
		iSection2 = 0;
		du1 = m_Sections[iSection1].first;
		du2 = m_Sections[iSection2].first+1;
	}
	if (du1 == du2)
		u = 0;
	else
		u = (dYarnPosition-du1)/(du2 - du1);

	vector<XY> Points;
	GetInterpedSection(*m_Sections[iSection1].second, *m_Sections[iSection2].second, u, iNumPoints, bEquiSpaced, Points);

	return Points;
}

CMesh CYarnSectionInterpPosition::GetSectionMesh(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced) const
{
	int iSection1 = -1;
	int iSection2 = -1;
	double u;
	double dYarnPosition = PositionInfo.GetYarnPosition();

	int i;
	for (i = 0; i<(int)m_Sections.size(); ++i)
	{
		if (m_Sections[i].first <= dYarnPosition)
			iSection1 = i;
		if (m_Sections[i].first >= dYarnPosition && iSection2 == -1)
			iSection2 = i;
	}

	if (iSection1 == -1 && iSection2 == -1)
	{
		TGERROR("Unable to get section mesh, no sections specified");
		assert(false);
		return CMesh();
	}

	double du1, du2;
	if (iSection1 != -1 && iSection2 != -1)
	{
		du1 = m_Sections[iSection1].first;
		du2 = m_Sections[iSection2].first;
	}
	else if (iSection1 == -1)
	{
		iSection1 = m_Sections.size()-1;
		du1 = m_Sections[iSection1].first-1;
		du2 = m_Sections[iSection2].first;
	}
	else
	{
		iSection2 = 0;
		du1 = m_Sections[iSection1].first;
		du2 = m_Sections[iSection2].first+1;
	}

	CMesh Mesh;
	if ( (iSection1 != -1 && iSection2 != -1) && (du1 == du2) ) // same section
	{
		Mesh = m_Sections[iSection1].second->GetMesh(iNumPoints, bEquiSpaced);
	}
	else
	{
		u = (dYarnPosition-du1)/(du2 - du1);
		GetInterpedSectionMesh(*m_Sections[iSection1].second, *m_Sections[iSection2].second, u, iNumPoints, bEquiSpaced, Mesh);
	}

	return Mesh;
}

void CYarnSectionInterpPosition::AddSection(double dPosition, const CSection &Section)
{
	m_Sections.push_back(pair<double, CObjectContainer<CSection> >(dPosition, Section));
	sort(m_Sections.begin(), m_Sections.end(), LessPairDoubleObjectRef<CSection>());
}

const CSection &CYarnSectionInterpPosition::GetSection(int iIndex) const
{
	assert(iIndex >= 0 && iIndex < (int)m_Sections.size());
	return *m_Sections[iIndex].second;
}

double CYarnSectionInterpPosition::GetSectionPosition(int iIndex) const
{
	assert(iIndex >= 0 && iIndex < (int)m_Sections.size());
	return m_Sections[iIndex].first;
}

void CYarnSectionInterpPosition::SetSectionMeshLayersEqual( int iNumPoints ) const
{
	int iMaxLayers = -1;
	for ( int i = 0; i < (int)m_Sections.size(); ++i )
	{
		int iNumLayers = CalculateNumberofLayers(m_Sections[i].second->GetPoints( iNumPoints, true ));  
		
		if ( iNumLayers > iMaxLayers )
			iMaxLayers = iNumLayers;
	}
	if ( iMaxLayers != -1 )
	{
		for ( int i = 0; i < (int)m_Sections.size(); ++i )
		{
			const CSection* pSection = m_Sections[i].second;  // Convoluted but can't find another way to get around this at the moment
			CSection* pSection2 = const_cast<CSection*> (pSection);
			pSection2->SetSectionMeshLayers(iMaxLayers);
		}
	}
}






