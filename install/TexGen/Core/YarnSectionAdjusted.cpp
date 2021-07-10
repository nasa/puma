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
#include "YarnSectionAdjusted.h"

using namespace TexGen;

CYarnSectionAdjusted::CYarnSectionAdjusted(const CYarnSection &YarnSection)
: m_pYarnSection(YarnSection)
{
}

CYarnSectionAdjusted::~CYarnSectionAdjusted(void)
{
}

CYarnSectionAdjusted::CYarnSectionAdjusted(TiXmlElement &Element)
: CYarnSection(Element)
{
	FOR_EACH_TIXMLELEMENT(pAdjustment, Element, "Adjustment")
	{
		vector<pair<double, SECTION_ADJUST> > Adjustments;
		FOR_EACH_TIXMLELEMENT(pAdjustmentSection, *pAdjustment, "AdjustmentSection")
		{
			pair<double, SECTION_ADJUST> Adjustment(0.0, SECTION_ADJUST());
			pAdjustmentSection->Attribute("T", &Adjustment.first);
			FOR_EACH_TIXMLELEMENT(pAdjustmentPerimeter, *pAdjustmentSection, "AdjustmentPerimeter")
			{
				pair<double, XY> AdjustmentPerimeter(0.0, XY());
				pAdjustmentPerimeter->Attribute("T", &AdjustmentPerimeter.first);
				AdjustmentPerimeter.second = valueify<XY>(pAdjustmentPerimeter->Attribute("Vector"));
				Adjustment.second.push_back(AdjustmentPerimeter);
			}			
			Adjustments.push_back(Adjustment);
		}
		m_Adjustments.push_back(Adjustments);
	}
	TiXmlElement* pYarnSection = Element.FirstChildElement("YarnSection");
	if (pYarnSection)
		m_pYarnSection = CreateYarnSection(*pYarnSection);
}

void CYarnSectionAdjusted::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CYarnSection::PopulateTiXmlElement(Element, OutputType);
	int i, j, k;
	for (i=0; i<(int)m_Adjustments.size(); ++i)
	{
		TiXmlElement Adjustment("Adjustment");
		Adjustment.SetAttribute("index", i);
		for (j=0; j<(int)m_Adjustments[i].size(); ++j)
		{
			TiXmlElement AdjustmentSection("AdjustmentSection");
			AdjustmentSection.SetAttribute("T", stringify(m_Adjustments[i][j].first));
			for (k=0; k<(int)m_Adjustments[i][j].second.size(); ++k)
			{
				TiXmlElement AdjustmentPerimeter("AdjustmentPerimeter");
				AdjustmentPerimeter.SetAttribute("T", stringify(m_Adjustments[i][j].second[k].first));
				AdjustmentPerimeter.SetAttribute("Vector", stringify(m_Adjustments[i][j].second[k].second));
				AdjustmentSection.InsertEndChild(AdjustmentPerimeter);
			}
			Adjustment.InsertEndChild(AdjustmentSection);
		}
		Element.InsertEndChild(Adjustment);
	}
	TiXmlElement YarnSection("YarnSection");
	m_pYarnSection->PopulateTiXmlElement(YarnSection, OutputType);
	Element.InsertEndChild(YarnSection);
}

vector<XY> CYarnSectionAdjusted::GetSection(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced) const
{
	vector<XY> SectionPoints = m_pYarnSection->GetSection(PositionInfo, iNumPoints, bEquiSpaced);

	if (PositionInfo.iSection < 0 || PositionInfo.iSection >= (int)m_Adjustments.size())
	{
		TGERROR("Unable to adjust section. Index out of range: " << PositionInfo.iSection);
		return SectionPoints;
	}

	pair<double, pair<SECTION_ADJUST, SECTION_ADJUST> > ClosestVals = 
		GetClosestValues(m_Adjustments[PositionInfo.iSection], PositionInfo.dSectionPosition);

	XY Disp, Disp1, Disp2;
	vector<XY>::iterator itSecPt;
	assert((int)SectionPoints.size() == iNumPoints);
	int i;
	for (itSecPt = SectionPoints.begin(), i=0; itSecPt != SectionPoints.end(); ++itSecPt, ++i)
	{
		double dU = i/double(iNumPoints);
		Disp1 = GetInterpedValue(ClosestVals.second.first, dU);
		Disp2 = GetInterpedValue(ClosestVals.second.second, dU);
		Disp = Disp1 + ClosestVals.first * (Disp2-Disp1);
		*itSecPt += Disp;
	}

	return SectionPoints;
}

CMesh CYarnSectionAdjusted::GetSectionMesh(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced) const
{
	return m_pYarnSection->GetSectionMesh(PositionInfo, iNumPoints, bEquiSpaced);
}

void CYarnSectionAdjusted::AddAdjustment(int iIndex, double t, const vector<pair<double, XY> > &SectionAdjust)
{
	if (iIndex < 0)
	{
		TGERROR("Unable to add adjustment. Index out of range: " << iIndex);
		return;
	}
	if (iIndex >= (int)m_Adjustments.size())
	{
		m_Adjustments.resize(iIndex+1);
	}
	m_Adjustments[iIndex].push_back(make_pair(t, SectionAdjust));
	// Sort the vector to make sure t values are ordered from lowest to highest
	sort(m_Adjustments[iIndex].begin(), m_Adjustments[iIndex].end());
}




