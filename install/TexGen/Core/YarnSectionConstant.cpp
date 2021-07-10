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
#include "YarnSectionConstant.h"
#include "SectionEllipse.h"
//#include "TexGen.h"
using namespace TexGen;

CYarnSectionConstant::CYarnSectionConstant(const CSection &Section)
: m_pSection(Section)
{
	
}

CYarnSectionConstant::~CYarnSectionConstant(void)
{
}

CYarnSectionConstant::CYarnSectionConstant(TiXmlElement &Element)
: CYarnSection(Element)
{
	TiXmlElement* pSection = Element.FirstChildElement("Section");
	if (pSection)
		m_pSection = CSection::CreateSection(*pSection);
	else
		m_pSection = CSectionEllipse(1, 1);
}

void CYarnSectionConstant::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CYarnSection::PopulateTiXmlElement(Element, OutputType);

	TiXmlElement Section("Section");
	m_pSection->PopulateTiXmlElement(Section, OutputType);
	Element.InsertEndChild(Section);
}

vector<XY> CYarnSectionConstant::GetSection(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced) const
{
	// return the same section ignoring the information given in PositionInfo
	return m_pSection->GetPoints(iNumPoints, bEquiSpaced);
}

CMesh CYarnSectionConstant::GetSectionMesh(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced) const
{
	// return the same section ignoring the information given in PositionInfo
	return m_pSection->GetMesh(iNumPoints, bEquiSpaced);
}


