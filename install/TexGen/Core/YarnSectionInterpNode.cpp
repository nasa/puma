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
#include "YarnSectionInterpNode.h"
#include "SectionMeshRectangular.h"
//#include "TexGen.h"
using namespace TexGen;

CYarnSectionInterpNode::CYarnSectionInterpNode(bool bRamped, bool bPolar, bool bConstMesh)
: CYarnSectionInterp(bRamped, bPolar, bConstMesh)
{
}

CYarnSectionInterpNode::~CYarnSectionInterpNode(void)
{
}

CYarnSectionInterpNode::CYarnSectionInterpNode(TiXmlElement &Element)
: CYarnSectionInterp(Element)
{
	int i = 0;
	FOR_EACH_TIXMLELEMENT(pNodeSection, Element, "NodeSection")
	{
		TiXmlElement* pSection = pNodeSection->FirstChildElement("Section");
		if (pSection)
		{
			m_Sections.push_back(CSection::CreateSection(*pSection));
		}
		FOR_EACH_TIXMLELEMENT(pMidNodeSection, *pNodeSection, "MidNodeSection")
		{
			if (i >= (int)m_MidNodeSections.size())
				m_MidNodeSections.resize(i+1);
			pSection = pMidNodeSection->FirstChildElement("Section");
			if (pSection)
			{
				m_MidNodeSections[i].push_back(make_pair(
					valueify<double>(pMidNodeSection->Attribute("t")),
					CSection::CreateSection(*pSection)
					));
			}
		}
		++i;
	}
	m_MidNodeSections.resize(m_Sections.size()-1);
}

void CYarnSectionInterpNode::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CYarnSectionInterp::PopulateTiXmlElement(Element, OutputType);

	int i, j;
	for (i=0; i<(int)m_Sections.size(); ++i)
	{
		TiXmlElement NodeSection("NodeSection");
		NodeSection.SetAttribute("nodeindex", i);
		TiXmlElement Section("Section");
		m_Sections[i]->PopulateTiXmlElement(Section, OutputType);
		NodeSection.InsertEndChild(Section);
		if (i < (int)m_MidNodeSections.size())
		{
			for (j=0; j<(int)m_MidNodeSections[i].size(); ++j)
			{
				TiXmlElement MidSideNode("MidNodeSection");
				MidSideNode.SetAttribute("nodeindex", i);
				MidSideNode.SetAttribute("t", stringify(m_MidNodeSections[i][j].first));
				TiXmlElement MidSection("Section");
				m_MidNodeSections[i][j].second->PopulateTiXmlElement(MidSection, OutputType);
				MidSideNode.InsertEndChild(MidSection);
				NodeSection.InsertEndChild(MidSideNode);
			}
		}
		Element.InsertEndChild(NodeSection);
	}
}

vector<XY> CYarnSectionInterpNode::GetSection(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced) const
{
	if (PositionInfo.SectionLengths.size()+1 != m_Sections.size())
	{
		// The number of sections specified is not equal to the number of nodes specified.
		TGERROR("Unable to get section, the number of sections specified (" << m_Sections.size() << ") is not equal to the number of nodes (" << PositionInfo.SectionLengths.size()+1 << ")");
		assert(false);
		return vector<XY>();
	}

	const CSection *pSection1, *pSection2;
	double u;
	GetSectionsToInterp(PositionInfo, pSection1, pSection2, u);

	vector<XY> Points;
	GetInterpedSection(*pSection1, *pSection2, u, iNumPoints, bEquiSpaced, Points);

	return Points;
}

CMesh CYarnSectionInterpNode::GetSectionMesh(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced) const
{
	if (PositionInfo.SectionLengths.size()+1 != m_Sections.size())
	{
		// The number of sections specified is not equal to the number of nodes specified.
		TGERROR("Unable to get section mesh, the number of sections specified (" << m_Sections.size() << ") is not equal to the number of nodes (" << PositionInfo.SectionLengths.size()+1 << ")");
		assert(false);
		return CMesh();
	}

	const CSection *pSection1, *pSection2;
	double u;
	GetSectionsToInterp(PositionInfo, pSection1, pSection2, u);

	CMesh Mesh;
	GetInterpedSectionMesh(*pSection1, *pSection2, u, iNumPoints, bEquiSpaced, Mesh);

	return Mesh;
}

void CYarnSectionInterpNode::AddSection(const CSection &Section)
{
	m_Sections.push_back(Section);
	m_MidNodeSections.resize(m_Sections.size()-1);
}

void CYarnSectionInterpNode::InsertSection(int iIndex, const CSection &Section)
{
	if (iIndex < 0 || iIndex >= (int)m_Sections.size())
	{
		TGERROR("Unable to insert section, index out of range " << iIndex);
		return;
	}
	m_Sections.insert(m_Sections.begin()+iIndex, Section);
	m_MidNodeSections.resize(m_Sections.size()-1);
}

void CYarnSectionInterpNode::InsertSection(int iIndex, double u, const CSection &Section)
{
	if (iIndex < 0 || iIndex >= (int)m_MidNodeSections.size())
	{
		TGERROR("Unable to insert section, index out of range " << iIndex);
		return;
	}
	m_MidNodeSections[iIndex].push_back(pair<double, CObjectContainer<CSection> >(u, Section));
	sort(m_MidNodeSections[iIndex].begin(), m_MidNodeSections[iIndex].end(), LessPairDoubleObjectRef<CSection>());
}

void CYarnSectionInterpNode::ReplaceSection(int iIndex, const CSection &Section)
{
	if (iIndex < 0 || iIndex >= (int)m_Sections.size())
	{
		TGERROR("Unable to replace section, index out of range " << iIndex);
		return;
	}
	m_Sections[iIndex] = Section;
}

const CSection &CYarnSectionInterpNode::GetNodeSection(int iIndex) const
{
	assert(iIndex >= 0 && iIndex < (int)m_Sections.size());
	return *m_Sections[iIndex];
}

/*int CYarnSectionInterpNode::ConvertToLocalT( int t )
{
	int iNumSections = MasterNodes.size()-1;
	t *= iNumSections;
	int iIndex = int(t);
	t -= iIndex;
	if (iIndex >= iNumSections)
	{
		iIndex = iNumSections-1;
		t = 1;
	}
}*/

void CYarnSectionInterpNode::GetSectionsToInterp(const YARN_POSITION_INFORMATION PositionInfo, const CSection* &pSection1, const CSection* &pSection2, double &u) const
{
	pSection1 = m_Sections[PositionInfo.iSection];
	pSection2 = m_Sections[PositionInfo.iSection+1];
	u = PositionInfo.dSectionPosition;
	vector<pair<double, CObjectContainer<CSection> > >::const_iterator itSection;
	int iMidSection1 = -1, iMidSection2 = -1;
	int i;
	double u1, u2;
	assert(PositionInfo.iSection < (int)m_MidNodeSections.size());
	for (itSection = m_MidNodeSections[PositionInfo.iSection].begin(), i = 0; 
		itSection != m_MidNodeSections[PositionInfo.iSection].end(); ++itSection, ++i)
	{
		if (itSection->first < PositionInfo.dSectionPosition)
		{
			iMidSection1 = i;
			u1 = itSection->first;
		}
		else
		{
			iMidSection2 = i;
			u2 = itSection->first;
			break;
		}
	}
	if (iMidSection1 != -1 && iMidSection2 != -1)
	{
		pSection1 = m_MidNodeSections[PositionInfo.iSection][iMidSection1].second;
		pSection2 = m_MidNodeSections[PositionInfo.iSection][iMidSection2].second;
		u = (PositionInfo.dSectionPosition - u1)/(u2-u1);
	}
	else if (iMidSection1 != -1)
	{
		pSection1 = m_MidNodeSections[PositionInfo.iSection][iMidSection1].second;
		u = (PositionInfo.dSectionPosition - u1)/(1-u1);
	}
	else if (iMidSection2 != -1)
	{
		pSection2 = m_MidNodeSections[PositionInfo.iSection][iMidSection2].second;
		u = PositionInfo.dSectionPosition/u2;
	}
}

void CYarnSectionInterpNode::SetSectionMeshLayersEqual( int iNumPoints ) const
{
	
	int iNumSections = (int)m_Sections.size();

	CSection* Section = m_Sections[0]->Copy();
	int iNumLayers = Section->GetSectionMeshLayers();
	int iMaxLayers = iNumLayers;
	delete Section;

	if ( iNumLayers != -1 )  // Num layers has been set.  Check that all set to same value
	{
		int i;
		for ( i = 1; i < iNumSections; ++i )
		{
			
			CSection* Section = m_Sections[i]->Copy();

			if ( Section->GetSectionMeshLayers() != iNumLayers )
			{
				delete Section;
				break;
			}
			delete Section;
		}
		if ( i == iNumSections )  // Return if all same
			return;
	}

	// Not all set the same so calculate
	for ( int i = 0; i < iNumSections; ++i )
	{
		int iNumLayers = CalculateNumberofLayers(m_Sections[i]->GetPoints( iNumPoints, true ));  
		
		if ( iNumLayers > iMaxLayers )
			iMaxLayers = iNumLayers;
		
		if ( i < iNumSections-1 )
		{
			for ( int j = 0; j < (int)m_MidNodeSections[i].size(); ++j )
			{
				iNumLayers = CalculateNumberofLayers(m_MidNodeSections[i][j].second->GetPoints( iNumPoints, true )); 
				if ( iNumLayers > iMaxLayers )
					iMaxLayers = iNumLayers;
			}
		}
	}
	if ( iMaxLayers != -1 )
	{
		for ( int i = 0; i < (int)m_Sections.size(); ++i )
		{
			const CSection* pSection = m_Sections[i];  // Convoluted but can't find another way to get around this at the moment
			CSection* pSection2 = const_cast<CSection*> (pSection);
			pSection2->SetSectionMeshLayers(iMaxLayers);

			if ( i < iNumSections-1 )
			{
				for ( int j = 0; j < (int)m_MidNodeSections[i].size(); ++j )
				{
					const CSection* pSection = m_MidNodeSections[i][j].second;  // Convoluted but can't find another way to get around this at the moment
					CSection* pSection2 = const_cast<CSection*> (pSection);
					pSection2->SetSectionMeshLayers(iMaxLayers);
				}
			}
		}
		
	}
}

void CYarnSectionInterpNode::DeleteSection(int iIndex)
{
	if (iIndex < 0 || iIndex >= (int)m_Sections.size())
	{
		TGERROR("Unable to delete section, index out of range " << iIndex);
		return;
	}
	m_Sections.erase(m_Sections.begin()+iIndex);
	m_MidNodeSections.resize(m_Sections.size()-1);
}

int CYarnSectionInterpNode::GetNumMidNodeSections(int iIndex) const
{
	if ( iIndex < 0 || iIndex >= (int)m_Sections.size())
	{
		TGERROR("Unable to get number of mid node sections, index out of range " << iIndex );
		return(0);
	}
	return( (int)m_MidNodeSections[iIndex].size() );
}

double CYarnSectionInterpNode::GetMidNodeSectionPos( int iNodeIndex, int iIndex ) const
{
	assert(iNodeIndex >= 0 && iNodeIndex < (int)m_Sections.size());
	assert(iIndex >= 0 && iIndex < (int)m_MidNodeSections[iNodeIndex].size() );

	return( m_MidNodeSections[iNodeIndex][iIndex].first );
}

void CYarnSectionInterpNode::ReplaceMidSection(int iNodeIndex, int iIndex, const CSection &Section)
{
	if (iNodeIndex < 0 || iNodeIndex >= (int)m_Sections.size())
	{
		TGERROR("Unable to replace mid section, index out of range " << iNodeIndex);
		return;
	}
	if ( iIndex < 0 || iIndex >= (int)m_MidNodeSections[iNodeIndex].size() )
	{
		TGERROR("Unable to replace mid section, mid index out of range " << iIndex);
		return;
	}
	m_MidNodeSections[iNodeIndex][iIndex].second = Section;
}

