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
#include "Yarn.h"
#include "Section.h"
#include "Plane.h"
#include "Domain.h"
#include "SectionEllipse.h"
#include "Textile.h"

using namespace TexGen;

CYarn::CYarn(void)
: m_iNumSlaveNodes(0)
, m_iNumSectionPoints(0)
, m_iNeedsBuilding(ALL)
, m_bEquiSpacedSectionMesh(true)
//, m_pParent(NULL)
{
	AssignDefaults();
}

CYarn::~CYarn(void)
{
}

CYarn::CYarn(TiXmlElement &Element)
:  CPropertiesYarn(Element)
, m_iNumSlaveNodes(0)
, m_iNumSectionPoints(0)
, m_iNeedsBuilding(ALL)
, m_bEquiSpacedSectionMesh(true)
//, m_pParent(NULL)
{
	AssignDefaults();
	Element.Attribute("NumSlaveNodes", &m_iNumSlaveNodes);
	Element.Attribute("NumSectionPoints", &m_iNumSectionPoints);
	TiXmlElement* pInterpolation = Element.FirstChildElement("Interpolation");
	if (pInterpolation)
	{
		m_pInterpolation = CInterpolation::CreateInterpolation(*pInterpolation);
	}
	TiXmlElement* pYarnSection = Element.FirstChildElement("YarnSection");
	if (pYarnSection)
	{
		m_pYarnSection = CYarnSection::CreateYarnSection(*pYarnSection);
	}
	TiXmlElement* pFibreDistribution = Element.FirstChildElement("FibreDistribution");
	if (pFibreDistribution)
	{
		// TODO: Implement in a similar way to above
		const string* pType = pFibreDistribution->Attribute(string("type"));
		if (pType)
		{
			if (*pType == "CFibreDistribution1DQuad")
				m_pFibreDistribution = CFibreDistribution1DQuad(*pFibreDistribution);
			else if (*pType == "CFibreDistributionConst")
				m_pFibreDistribution = CFibreDistributionConst(*pFibreDistribution);
		}
	}
	FOR_EACH_TIXMLELEMENT(pRepeat, Element, "Repeat")
	{
		m_Repeats.push_back(valueify<XYZ>(pRepeat->Attribute("value")));
	}
	FOR_EACH_TIXMLELEMENT(pMasterNode, Element, "MasterNode")
	{
		m_MasterNodes.push_back(CNode(*pMasterNode));
	}
	FOR_EACH_TIXMLELEMENT(pSlaveNode, Element, "SlaveNode")
	{
		m_SlaveNodes.push_back(CSlaveNode(*pSlaveNode));
	}
	m_AABB.first = valueify<XYZ>(Element.Attribute("AABB.first"));
	m_AABB.second = valueify<XYZ>(Element.Attribute("AABB.second"));
	FOR_EACH_TIXMLELEMENT(pSectionAABB, Element, "SectionAABB")
	{
		m_SectionAABBs.push_back(make_pair(
			valueify<XYZ>(pSectionAABB->Attribute("first")),
			valueify<XYZ>(pSectionAABB->Attribute("second"))));
	}
	FOR_EACH_TIXMLELEMENT(pSectionLength, Element, "SectionLength")
	{
		m_SectionLengths.push_back(valueify<double>(pSectionLength->Attribute("value")));
	}

	Element.Attribute("NeedsBuilding", &m_iNeedsBuilding);
}

void CYarn::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CPropertiesYarn::PopulateTiXmlElement(Element, OutputType);

	Element.SetAttribute("NumSlaveNodes", m_iNumSlaveNodes);
	Element.SetAttribute("NumSectionPoints", m_iNumSectionPoints);
	if (m_pInterpolation)
	{
		TiXmlElement Interpolation("Interpolation");
		m_pInterpolation->PopulateTiXmlElement(Interpolation, OutputType);
		Element.InsertEndChild(Interpolation);
	}
	if (m_pYarnSection)
	{
		TiXmlElement YarnSection("YarnSection");
		m_pYarnSection->PopulateTiXmlElement(YarnSection, OutputType);
		Element.InsertEndChild(YarnSection);
	}
	if (m_pFibreDistribution)
	{
		TiXmlElement FibreDistribution("FibreDistribution");
		m_pFibreDistribution->PopulateTiXmlElement(FibreDistribution, OutputType);
		Element.InsertEndChild(FibreDistribution);
	}
	vector<XYZ>::iterator itXYZ;
	for (itXYZ = m_Repeats.begin(); itXYZ!=m_Repeats.end(); ++itXYZ)
	{
		TiXmlElement Repeat("Repeat");
		Repeat.SetAttribute("value", stringify(*itXYZ));
		Element.InsertEndChild(Repeat);
	}
	int i;
	for (i=0; i<(int)m_MasterNodes.size(); ++i)
	{
		TiXmlElement MasterNode("MasterNode");
		MasterNode.SetAttribute("index", i);
		m_MasterNodes[i].PopulateTiXmlElement(MasterNode, OutputType);
		Element.InsertEndChild(MasterNode);
	}
	if (OutputType == OUTPUT_FULL)
	{
		vector<CSlaveNode>::iterator itSlaveNode;
		for (itSlaveNode = m_SlaveNodes.begin(); itSlaveNode!=m_SlaveNodes.end(); ++itSlaveNode)
		{
			TiXmlElement SlaveNode("SlaveNode");
			itSlaveNode->PopulateTiXmlElement(SlaveNode, OutputType);
			Element.InsertEndChild(SlaveNode);
		}
		Element.SetAttribute("AABB.first", stringify(m_AABB.first));
		Element.SetAttribute("AABB.second", stringify(m_AABB.second));
		for (i=0; i<(int)m_SectionAABBs.size(); ++i)
		{
			TiXmlElement SectionAABB("SectionAABB");
			SectionAABB.SetAttribute("index", i);
			SectionAABB.SetAttribute("first", stringify(m_SectionAABBs[i].first));
			SectionAABB.SetAttribute("second", stringify(m_SectionAABBs[i].second));
			Element.InsertEndChild(SectionAABB);
		}
		for (i=0; i<(int)m_SectionLengths.size(); ++i)
		{
			TiXmlElement SectionLength("SectionLength");
			SectionLength.SetAttribute("index", i);
			SectionLength.SetAttribute("value", stringify(m_SectionLengths[i]));
			Element.InsertEndChild(SectionLength);
		}
		Element.SetAttribute("NeedsBuilding", m_iNeedsBuilding | VOLUME);
	}
	else
	{
		Element.SetAttribute("NeedsBuilding", ALL);
	}
}

void CYarn::AssignDefaults()
{
	// Set the yarn defaults
	AssignInterpolation(CInterpolationBezier());
	AssignFibreDistribution(CFibreDistributionConst());
	AssignSection(CYarnSectionConstant(CSectionEllipse(1, 1)));
	SetResolution(20, 40);  // Changed default numSectionPoints from 20 8-4-13
}

void CYarn::SetNodes(const vector<CNode> &Nodes)
{
	m_MasterNodes = Nodes;
	// When the nodes are set the yarn needs to be rebuilt
	m_iNeedsBuilding = ALL;
}


void CYarn::AddNode(const CNode &Node)
{
	XYZ NodePos = Node.GetPosition();
	vector<CNode>::iterator itNode;
	for ( itNode = m_MasterNodes.begin(); itNode != m_MasterNodes.end(); ++itNode )
	{
		if ( itNode->GetPosition() == NodePos )
		{
			TGERROR( "Trying to add duplicate node: node at " << Node.GetPosition() << " not added" );
			return;
		}
	}
	m_MasterNodes.push_back(Node);
	// When a node is added the yarn needs to be rebuilt
	m_iNeedsBuilding = ALL;
}

bool CYarn::InsertNode(const CNode &Node, const CNode* pBefore)
{
	vector<CNode>::iterator itNode;
	int i;
	for (itNode = m_MasterNodes.begin(), i = 0; itNode != m_MasterNodes.end(); ++itNode, ++i)
	{
		if (&(*itNode) == pBefore)
		{
			m_MasterNodes.insert(itNode, Node);
			if ( m_pYarnSection->GetType() == "CYarnSectionInterpNode" )
			{
				CYarnSectionInterpNode &InterpNode = (CYarnSectionInterpNode&)m_pYarnSection;
				InterpNode.InsertSection( i, InterpNode.GetNodeSection(i) );
			}
			// When a node is added we need to rebuild the yarn
			m_iNeedsBuilding = ALL;
			return true;
		}
	}
	TGERROR("Unable to insert node, given node is not contained within the yarn");
	return false;
}

bool CYarn::InsertNode(const CNode &Node, int iIndex)
{
	if (iIndex < 0 || iIndex > (int)m_MasterNodes.size())
	{
		TGERROR("Unable to insert node, index invalid: " << iIndex);
		return false;
	}
	vector<CNode>::iterator itNode;
	itNode = m_MasterNodes.begin()+iIndex;
	m_MasterNodes.insert(itNode, Node);
	
	if ( m_pYarnSection->GetType() == "CYarnSectionInterpNode" )
	{
		CYarnSectionInterpNode* pInterpNodeSection = (CYarnSectionInterpNode*)m_pYarnSection->Copy();
		const CSection& Section = pInterpNodeSection->GetNodeSection(iIndex);
		pInterpNodeSection->InsertSection( iIndex, Section );
		AssignSection(*pInterpNodeSection);
		delete pInterpNodeSection;
	}

	// When a node is added we need to rebuild the yarn
	m_iNeedsBuilding = ALL;
	return true;
}

bool CYarn::ReplaceNode(int iIndex, CNode NewNode)
{
	if (iIndex < 0 || iIndex >= (int)m_MasterNodes.size())
		return false;
	XYZ NodePos = NewNode.GetPosition();
	vector<CNode>::iterator itNode;
	int i = 0;
	for ( itNode = m_MasterNodes.begin(); itNode != m_MasterNodes.end(); ++itNode, ++i )
	{
		if ( itNode->GetPosition() == NodePos && i != iIndex )  // If i == iIndex position might be same but tangent or Up might have changed so still want to replace
		{
			TGERROR( "Trying to replace duplicate node: node at " << NodePos << " not added" );
			return false;
		}
	}
	m_MasterNodes[iIndex] = NewNode;
	// When a node is replaced the yarn needs to be rebuilt
	m_iNeedsBuilding = ALL;
	return true;
}

bool CYarn::DeleteNode(int iIndex)
{
	if (iIndex < 0 || iIndex >= (int)m_MasterNodes.size())
		return false;
	m_MasterNodes.erase(m_MasterNodes.begin()+iIndex);
	if ( m_pYarnSection->GetType() == "CYarnSectionInterpNode" )
	{
		CYarnSectionInterpNode* pInterpNodeSection = (CYarnSectionInterpNode*)m_pYarnSection->Copy();
		pInterpNodeSection->DeleteSection(iIndex);
		AssignSection(*pInterpNodeSection);
	}

	// When a node is deleted the yarn needs to be rebuilt
	m_iNeedsBuilding = ALL;
	return true;
}

const CNode* CYarn::GetNode(int iIndex) const
{
	if (iIndex < 0 || iIndex >= (int)m_MasterNodes.size())
		return NULL;
	return &m_MasterNodes[iIndex];
}

void CYarn::SetResolution(int iNumSlaveNodes, int iNumSectionPoints)
{
	m_iNumSlaveNodes = iNumSlaveNodes;
	m_iNumSectionPoints = iNumSectionPoints;

	// When the resolution is changed the yarn needs to be rebuilt
	m_iNeedsBuilding = ALL;
}

void CYarn::SetEquiSpacedSectionMesh(bool bEquiSpacedSectionMesh)
{
	m_bEquiSpacedSectionMesh = bEquiSpacedSectionMesh;

	// When the resolution is changed the yarn needs to be rebuilt
	m_iNeedsBuilding = ALL;
}

bool CYarn::SetResolution(int iNumSectionPoints)
{
	TGLOGINDENT("Calculating slave node resolution given " << iNumSectionPoints << " section points");
	double dNodeDistance;
	double dYarnLength;
	double dNumNodes;
	int iNumNodes = 10;		// Set the an initial resolution, and refine from here
	int i, iMaxIterations = 100;
	for (i=0; i<iMaxIterations; ++i)
	{
		// Set the new resolution
		SetResolution(iNumNodes, iNumSectionPoints);
		// Build the yarn with given resolution
		if (!BuildYarnIfNeeded(SURFACE))
		{
			TGERROR("Unable to calculate slave node resolution");
			return false;
		}
		// Loop over all the slave nodes and sum the circumference of each
		vector<CSlaveNode>::iterator itSlaveNode;
		dNodeDistance = 0;
		for (itSlaveNode = m_SlaveNodes.begin(); itSlaveNode != m_SlaveNodes.end(); ++itSlaveNode)
		{
			dNodeDistance += CSection::GetCircumference(itSlaveNode->Get2DSectionPoints());
		}
		if (dNodeDistance == 0)
		{
			TGERROR("Unable to calculate slave node resolution");
			return false;
		}
		// Dividing the sum of circumferences by the number of nodes and section points gives
		// the average distance between section points
		dNodeDistance /= m_SlaveNodes.size() * iNumSectionPoints;
		dYarnLength = GetRawYarnLength();
		// Number of nodes can then be calculated with the equation below
		dNumNodes = (dYarnLength / dNodeDistance)+1;
		// We then need to round the number from a double to an int
		iNumNodes = (int)dNumNodes;
		if (dNumNodes-iNumNodes > 0.5)
			++iNumNodes;
		// If the number of nodes is the same as in the previous iteration or different by 1
		// then the operation was successful, otherwise do another iteration
		if (abs(iNumNodes - m_iNumSlaveNodes) <= 1)
		{
			TGLOG("Slave node resolution set to " << m_iNumSlaveNodes << " after " << i+1 << " iterations");
			return true;
		}
	};
	TGERROR("Unable to calculate slave node resolution");
	return false;
}

bool CYarn::BuildYarnIfNeeded(int iBuildType) const
{
	// If a volume build is required then a surface build is also required
	if (iBuildType & VOLUME)
		iBuildType |= SURFACE;

	// If a surface build is required then a line build is also required
	if (iBuildType & SURFACE)
		iBuildType |= LINE;

	// If the build type is LINE and it needs building, then build the slave nodes
	if (iBuildType & m_iNeedsBuilding & LINE)
	{
		if (!BuildSlaveNodes())
			return false;
	}
	// If the build type is SURFACE and it needs building, then build the slave nodes
	if (iBuildType & m_iNeedsBuilding & SURFACE)
	{
		if (!BuildSections())
			return false;
	}
	// If the build type is VOLUME and it needs building, then build the slave nodes
	if (iBuildType & m_iNeedsBuilding & VOLUME)
	{
		if (!BuildSectionMeshes())
			return false;
	}

	return true;
}

bool CYarn::BuildSlaveNodes() const
{
	TGLOG("Building yarn slave nodes");
	if (m_MasterNodes.size() <= 1)
	{
		TGERROR("Unable to build slave nodes, not enough master nodes specified");
		assert(false);
		return false;
	}
	if (!m_pInterpolation)
	{
		TGERROR("Unable to build slave nodes, no interpolation function specified");
		assert(false);
		return false;
	}

	// Set a default resolution of 20 if no resolution is specified
/*	if (m_iNumSlaveNodes == 0)
		SetResolution(20);*/
	m_SlaveNodes = m_pInterpolation->GetSlaveNodes(m_MasterNodes, m_iNumSlaveNodes);

	// Populate m_SectionLengths
	int i, j;
	XYZ PrevPos;
	m_SectionLengths.resize(m_MasterNodes.size()-1);

	// Calculate the section length by summing the distances between the two master nodes
	// on each end and all the slave nodes in between. 
	for (i=0; i<int(m_MasterNodes.size()-1); ++i)
	{
		m_SectionLengths[i] = 0;
		PrevPos = m_MasterNodes[i].GetPosition();
		for (j=0; j<int(m_SlaveNodes.size()); ++j)
		{
			if (m_SlaveNodes[j].GetIndex() == i)
			{
				m_SectionLengths[i] += GetLength(PrevPos, m_SlaveNodes[j].GetPosition());
				PrevPos = m_SlaveNodes[j].GetPosition();
			}
		}
		m_SectionLengths[i] += GetLength(PrevPos, m_MasterNodes[i+1].GetPosition());
	}

	// Center-line is built, but everything else needs rebuilding
	m_iNeedsBuilding = ALL^LINE;

	return true;
}

bool CYarn::BuildSections() const
{
	TGLOG("Building yarn sections");
	if (m_SlaveNodes.empty())
	{
		TGERROR("Unable to build sections, no slave nodes created");
		assert(false);
		return false;
	}
	if (!m_pYarnSection)
	{
		// Assign default circular section with diameter 1 tenth of the length of the yarn
/*		double dDiameter = GetRawYarnLength()/10;
		AssignSection(CYarnSectionConstant(CSectionEllipse(dDiameter, dDiameter)));*/
		TGERROR("Unable to build sections, no yarn section specified");
		assert(false);
		return false;
	}

	YARN_POSITION_INFORMATION YarnPositionInfo;
	YarnPositionInfo.SectionLengths = m_SectionLengths;

	bool bFirst = true;
	m_AABB = pair<XYZ, XYZ>(XYZ(), XYZ());
//	vector<bool> SectionFirst;
//	SectionFirst.resize(m_MasterNodes.size()-1, true);
//	int iSectionMin, iSectionMax;

	vector<XY> Section;
	vector<XYZ>::const_iterator itPoint;
	vector<CSlaveNode>::iterator itSlaveNode;
	XYZ PrevPos = m_SlaveNodes[0].GetPosition();
	for (itSlaveNode = m_SlaveNodes.begin(); itSlaveNode != m_SlaveNodes.end(); ++itSlaveNode)
	{
/*		iSectionMin = iSectionMax = itSlaveNode->GetIndex();

		if (itSlaveNode!=m_SlaveNodes.begin() && (itSlaveNode-1)->GetIndex() != iSectionMin)
			--iSectionMin;
		if ((itSlaveNode+1)!=m_SlaveNodes.end() && (itSlaveNode+1)->GetIndex() != iSectionMax)
			++iSectionMax;*/

		YarnPositionInfo.dSectionPosition = itSlaveNode->GetT();
		YarnPositionInfo.iSection = itSlaveNode->GetIndex();

		Section = m_pYarnSection->GetSection(YarnPositionInfo, m_iNumSectionPoints);
		itSlaveNode->UpdateSectionPoints(&Section);

		PrevPos = itSlaveNode->GetPosition();

		for (itPoint = itSlaveNode->GetSectionPoints().begin(); itPoint != itSlaveNode->GetSectionPoints().end(); ++itPoint)
		{
			if (bFirst)
			{
				m_AABB.first = m_AABB.second = *itPoint;
				bFirst = false;
			}
			m_AABB.first = Min(m_AABB.first, *itPoint);
			m_AABB.second = Max(m_AABB.second, *itPoint);
/*			for (iIndex = iSectionMin; iIndex <= iSectionMax; ++iIndex)
			{
				if (SectionFirst[iIndex])
				{
					m_SectionAABBs[iIndex].first = m_SectionAABBs[iIndex].second = *itPoint;
					SectionFirst[iIndex] = false;
				}
				m_SectionAABBs[iIndex].first = Min(m_SectionAABBs[iIndex].first, *itPoint);
				m_SectionAABBs[iIndex].second = Max(m_SectionAABBs[iIndex].second, *itPoint);
			}*/
		}
	}

	CreateSectionAABBs();
/*	const double TOL = 1e-9;
	m_AABB.first.x -= TOL;
	m_AABB.first.y -= TOL;
	m_AABB.first.z -= TOL;
	m_AABB.second.x += TOL;
	m_AABB.second.y += TOL;
	m_AABB.second.z += TOL;

	vector<pair<XYZ, XYZ> >::iterator itSectionAABB;
	for (itSectionAABB = m_SectionAABBs.begin(); itSectionAABB != m_SectionAABBs.end(); ++itSectionAABB)
	{
		itSectionAABB->first.x -= TOL;
		itSectionAABB->first.y -= TOL;
		itSectionAABB->first.z -= TOL;
		itSectionAABB->second.x += TOL;
		itSectionAABB->second.y += TOL;
		itSectionAABB->second.z += TOL;
	}*/

	// Surface points are built
	m_iNeedsBuilding &= ALL^SURFACE;

	return true;
}

void CYarn::CreateSectionAABBs() const
{
	vector<XYZ>::const_iterator itPoint;
	vector<CSlaveNode>::iterator itSlaveNode;
	int iIndex;
	int i, j;
	int iSlaveNodeMin;
	int iSlaveNodeMax;
	m_SectionAABBs.resize(m_MasterNodes.size()-1);
	for (i=0; i<(int)m_SectionAABBs.size(); ++i)
	{
		iSlaveNodeMin = 0;
		iSlaveNodeMax = (int)m_SlaveNodes.size()-1;
		for (itSlaveNode = m_SlaveNodes.begin(), j=0; itSlaveNode != m_SlaveNodes.end(); ++itSlaveNode, ++j)
		{
			iIndex = itSlaveNode->GetIndex();
			if (iIndex < i)
				iSlaveNodeMin = j;
			if (iIndex > i)
			{
				iSlaveNodeMax = j;
				break;
			}
		}
		bool bFirst = true;
		for (j=iSlaveNodeMin; j<=iSlaveNodeMax; ++j)
		{
			for (itPoint = m_SlaveNodes[j].GetSectionPoints().begin(); itPoint != m_SlaveNodes[j].GetSectionPoints().end(); ++itPoint)
			{
				if (bFirst)
				{
					m_SectionAABBs[i].first = m_SectionAABBs[i].second = *itPoint;
					bFirst = false;
				}
				m_SectionAABBs[i].first = Min(m_SectionAABBs[i].first, *itPoint);
				m_SectionAABBs[i].second = Max(m_SectionAABBs[i].second, *itPoint);
			}
		}
	}
}

bool CYarn::BuildSectionMeshes() const
{
	TGLOG("Building yarn section meshes");
	if (m_SlaveNodes.empty())
	{
		TGERROR("Unable to build section meshes, no slave nodes created");
		assert(false);
		return false;
	}
	if (!m_pYarnSection)
	{
		TGERROR("Unable to build section meshes, no yarn section specified");
		assert(false);
		return false;
	}
	
	if ( m_pYarnSection->GetForceMeshLayers() )
	{	
		m_pYarnSection->SetSectionMeshLayersEqual(m_iNumSectionPoints);
	}

	YARN_POSITION_INFORMATION YarnPositionInfo;
	YarnPositionInfo.SectionLengths = m_SectionLengths;

	vector<XY> Section;
	CMesh Mesh;
	vector<CSlaveNode>::iterator itSlaveNode;
	XYZ PrevPos = m_SlaveNodes[0].GetPosition();
	
	for (itSlaveNode = m_SlaveNodes.begin(); itSlaveNode != m_SlaveNodes.end(); ++itSlaveNode)
	{
		YarnPositionInfo.dSectionPosition = itSlaveNode->GetT();
		YarnPositionInfo.iSection = itSlaveNode->GetIndex();

		Mesh = m_pYarnSection->GetSectionMesh(YarnPositionInfo, m_iNumSectionPoints, m_bEquiSpacedSectionMesh);
		if ( Mesh.GetNumNodes() == 0 )
			return false;
		itSlaveNode->UpdateSectionMesh(&Mesh);

		PrevPos = itSlaveNode->GetPosition();
	}

	// Volume mesh points are built
	m_iNeedsBuilding &= ALL^VOLUME;
	return true;
}

void CYarn::AssignInterpolation(const CInterpolation &Interpolation)
{
	m_pInterpolation = Interpolation;

	// When a new interpolation is assigned the yarn needs to be rebuilt
	m_iNeedsBuilding = ALL;
}

void CYarn::AssignSection(const CYarnSection &YarnSection)
{
	if ( m_pYarnSection != &YarnSection )
		m_pYarnSection = YarnSection;

	// When a new section is assigned the yarn needs to be rebuilt
	m_iNeedsBuilding = ALL;
}

void CYarn::AssignFibreDistribution(const CFibreDistribution &Distribution)
{
	m_pFibreDistribution = Distribution;
}

void CYarn::Rotate(WXYZ Rotation, XYZ Origin)
{
	vector<CNode>::iterator itNode;
	for (itNode = m_MasterNodes.begin(); itNode != m_MasterNodes.end(); ++itNode)
	{
		itNode->Rotate(Rotation, Origin);
	}
	vector<XYZ>::iterator itRepeat;
	for (itRepeat = m_Repeats.begin(); itRepeat != m_Repeats.end(); ++itRepeat)
	{
		*itRepeat = Rotation * (*itRepeat);
	}
/*	vector<CSlaveNode>::iterator itSlaveNode;
	for (itSlaveNode = m_SlaveNodes.begin(); itSlaveNode != m_SlaveNodes.end(); ++itSlaveNode)
	{
		itSlaveNode->Rotate(Rotation, Origin);
	}*/
	// Will need to rebuild for the AABBs to be re-created properly
	m_iNeedsBuilding = ALL;
}

void CYarn::Translate(XYZ Vector)
{
	vector<CNode>::iterator itNode;
	for (itNode = m_MasterNodes.begin(); itNode != m_MasterNodes.end(); ++itNode)
	{
		itNode->Translate(Vector);
	}
/*	vector<CSlaveNode>::iterator itSlaveNode;
	for (itSlaveNode = m_SlaveNodes.begin(); itSlaveNode != m_SlaveNodes.end(); ++itSlaveNode)
	{
		itSlaveNode->Translate(Vector);
	}*/

	// Will need to rebuild slave nodes, AABBs, etc... to be translated (could easily be translated as well
	// but this method is more robust, if this becomes a performance issue recode)
	m_iNeedsBuilding = ALL;
}

bool CYarn::AddAABBToMesh(CMesh &Mesh) const
{
	// If the yarn needs building then build it before creating the mesh
	if (!BuildYarnIfNeeded(SURFACE))
		return false;

	XYZ Min, Max;

	CMesh AABB;

	// Find AABB
	vector<CSlaveNode>::iterator itNode;
	vector<XYZ>::const_iterator itSectionPoint;
	
	for (itNode = m_SlaveNodes.begin(); itNode != m_SlaveNodes.end(); ++itNode)
	{
		const vector<XYZ> NodeSectionPoints = itNode->GetSectionPoints();
		
		for (itSectionPoint = NodeSectionPoints.begin(); itSectionPoint != NodeSectionPoints.end(); ++itSectionPoint)
		{
			if (itNode == m_SlaveNodes.begin() && itSectionPoint == NodeSectionPoints.begin())
				Min = Max = *itSectionPoint;
			else
			{
				Min = ::Min(Min, *itSectionPoint);
				Max = ::Max(Max, *itSectionPoint);
			}
		}
	}

	/*for (itNode = m_SlaveNodes.begin(); itNode != m_SlaveNodes.end(); ++itNode)
	{
		for (itSectionPoint = itNode->GetSectionPoints().begin(); itSectionPoint != itNode->GetSectionPoints().end(); ++itSectionPoint)
		{
			if (itNode == m_SlaveNodes.begin() && itSectionPoint == itNode->GetSectionPoints().begin())
				Min = Max = *itSectionPoint;
			else
			{
				Min = ::Min(Min, *itSectionPoint);
				Max = ::Max(Max, *itSectionPoint);
			}
		} 
	}*/

	// Add nodes to the mesh
	// Adds bounding box 
	AABB.AddNode(Min);
	AABB.AddNode(XYZ(Max.x, Min.y, Min.z));
	AABB.AddNode(XYZ(Max.x, Max.y, Min.z));
	AABB.AddNode(XYZ(Min.x, Max.y, Min.z));
	AABB.AddNode(XYZ(Min.x, Min.y, Max.z));
	AABB.AddNode(XYZ(Max.x, Min.y, Max.z));
	AABB.AddNode(Max);
	AABB.AddNode(XYZ(Min.x, Max.y, Max.z));

	// Add indices to the mesh
	vector<int> Indices;
	Indices.push_back(0);
	Indices.push_back(1);
	Indices.push_back(2);
	Indices.push_back(3);
	Indices.push_back(4);
	Indices.push_back(5);
	Indices.push_back(6);
	Indices.push_back(7);

	AABB.AddElement(CMesh::HEX, Indices);

//	AABB.ConvertHextoQuad();

	Mesh.InsertMesh(AABB);

	return true;
}

bool CYarn::AddAABBToMesh(CMesh &Mesh, const vector<pair<int, int> > &RepeatLimits) const
{
	if (m_Repeats.size() != RepeatLimits.size())
	{
		TGERROR("Unable to create repeated AABB mesh, number of repeats (" << m_Repeats.size() << ") doesn't match the number of limits (" << RepeatLimits.size() << ")");
		assert(false);
		return false;
	}

	CMesh AABB;

	if (!AddAABBToMesh(AABB))
		return false;

	vector<XYZ>::const_iterator itRepeat;
	vector<pair<int, int> >::const_iterator itLimits;
	for (itRepeat = m_Repeats.begin(), itLimits = RepeatLimits.begin(); itRepeat != m_Repeats.end() && itLimits != RepeatLimits.end(); ++itRepeat, ++itLimits)
	{
		AABB.CopySelfToRange(*itRepeat, itLimits->first, itLimits->second);
	}

	Mesh.InsertMesh(AABB);

	return true;
}

bool CYarn::AddNodesToMesh(CMesh &Mesh) const
{
	vector<CNode>::const_iterator itNode;
	for (itNode = m_MasterNodes.begin(); itNode != m_MasterNodes.end(); ++itNode)
	{
		Mesh.AddNode(itNode->GetPosition());
	}
	return true;
}

bool CYarn::AddPathToMesh(CMesh &Mesh) const
{
	if (!BuildYarnIfNeeded(LINE))
		return false;
	int iStartIndex = Mesh.GetNumNodes();
	vector<CSlaveNode>::iterator itSlaveNode;
	for (itSlaveNode = m_SlaveNodes.begin(); itSlaveNode != m_SlaveNodes.end(); ++itSlaveNode)
	{
		Mesh.AddNode(itSlaveNode->GetPosition());
	}
	int iEndIndex = Mesh.GetNumNodes();

	vector<int> Indices;
	Indices.push_back(iStartIndex);
	Indices.push_back(iEndIndex);
	Mesh.AddElement(CMesh::POLYLINE, Indices);

	return true;
}

bool CYarn::AddSurfaceToMesh(CMesh &Mesh, const CDomain &Domain, bool bAddEndCaps, bool bFillGaps) const
{
	CMesh YarnMesh;
	if (AddSurfaceToMesh(YarnMesh, Domain.GetTranslations(*this), bAddEndCaps))
	{
		Domain.ClipMeshToDomain(YarnMesh, bFillGaps );
		Mesh.InsertMesh(YarnMesh);
		return true;
	}
	else
		return false;
}

bool CYarn::AddSurfaceToMesh( CMesh &Mesh, const CDomain &Domain, vector<CMesh> &DomainMeshes ) const
{
	CMesh YarnMesh;
	if (AddSurfaceToMesh(YarnMesh, Domain.GetTranslations(*this)))
	{
		if ( !Domain.ClipMeshToDomain(YarnMesh, DomainMeshes) )
			return false;
		Mesh.InsertMesh(YarnMesh);
		return true;
	}
	else
		return false;
}

bool CYarn::AddSurfaceToMesh(CMesh &Mesh, bool bAddEndCaps) const
{
	TGLOG("Adding yarn surface to mesh");
	// If the yarn needs building then build it before creating the mesh
	if (!BuildYarnIfNeeded(SURFACE))
		return false;

	// Check the number of section points on all the nodes is the same
	int iNumPoints = -1;
	int iNumNodes = (int)m_SlaveNodes.size();
	vector<CSlaveNode>::iterator itNode;
	for (itNode = m_SlaveNodes.begin(); itNode != m_SlaveNodes.end(); ++itNode)
	{
		if (iNumPoints == -1)
		{
			iNumPoints = (int)itNode->GetSectionPoints().size();
		}
		else if (iNumPoints != (int)itNode->GetSectionPoints().size())
		{
			// Cannot create surface mesh if the number of section points is not the same for all nodes
			TGERROR("Unable to create surface mesh, number of section points is not the same for all slave nodes");
			assert(false);
			return false;
		}
	}

	// Add nodes to the mesh
	vector<XYZ>::const_iterator itSectionPoint;
	int iFirstNode = (int)Mesh.GetNumNodes();
	for (itNode = m_SlaveNodes.begin(); itNode != m_SlaveNodes.end(); ++itNode)
	{
		for (itSectionPoint = itNode->GetSectionPoints().begin(); itSectionPoint != itNode->GetSectionPoints().end(); ++itSectionPoint)
		{
			Mesh.AddNode(*itSectionPoint);
		}
	}

	// Create the surface mesh elements
	int i, j, u, v;
	for (i=0; i<iNumNodes-1; ++i)
	{
		for (j=0; j<iNumPoints; ++j)
		{
			vector<int> Indices;
			// Add quad (if triangles are needed they can be converted to triangles later)
			u = i;   v = j;   if (v == iNumPoints) v = 0; Indices.push_back(iFirstNode+v+u*iNumPoints);
			u = i+1; v = j;   if (v == iNumPoints) v = 0; Indices.push_back(iFirstNode+v+u*iNumPoints);
			u = i+1; v = j+1; if (v == iNumPoints) v = 0; Indices.push_back(iFirstNode+v+u*iNumPoints);
			u = i;   v = j+1; if (v == iNumPoints) v = 0; Indices.push_back(iFirstNode+v+u*iNumPoints);
			Mesh.AddElement(CMesh::QUAD, Indices);
		}
	}

	if (bAddEndCaps)
		AddEndCapsToMesh(Mesh);

	return true;
}

void CYarn::AddEndCapsToMesh(CMesh &Mesh) const
{
	if (m_SlaveNodes.empty())
		return;
	
	const CSlaveNode &StartNode = m_SlaveNodes[0];
	const CSlaveNode &EndNode = m_SlaveNodes[m_SlaveNodes.size()-1];

	if (StartNode.GetPosition() == EndNode.GetPosition())
		return;

	CMesh StartMesh, EndMesh;

	StartMesh = CSectionMeshTriangulate::GetSimpleMesh(StartNode.Get2DSectionPoints());
	EndMesh = CSectionMeshTriangulate::GetSimpleMesh(EndNode.Get2DSectionPoints());

	XYZ StartSide = CrossProduct(StartNode.GetTangent(), StartNode.GetUp());
	XYZ EndSide = CrossProduct(EndNode.GetTangent(), EndNode.GetUp());

	StartMesh.Rotate(WXYZ(StartSide, StartNode.GetUp(), -StartNode.GetTangent()));
	EndMesh.Rotate(WXYZ(EndSide, EndNode.GetUp(), -EndNode.GetTangent()));

	EndMesh.FlipNormals();

	Mesh.InsertMesh(StartMesh, StartNode.GetPosition());
	Mesh.InsertMesh(EndMesh, EndNode.GetPosition());
}

bool CYarn::AddSurfaceToMesh(CMesh &Mesh, const vector<pair<int, int> > &RepeatLimits) const
{
	if (m_Repeats.size() != RepeatLimits.size())
	{
		TGERROR("Unable to create repeated surface mesh, number of repeats (" << m_Repeats.size() << ") doesn't match the number of limits (" << RepeatLimits.size() << ")");
		assert(false);
		return false;
	}

	CMesh YarnMesh;

	if (!AddSurfaceToMesh(YarnMesh))
		return false;

	vector<XYZ>::const_iterator itRepeat;
	vector<pair<int, int> >::const_iterator itLimits;
	for (itRepeat = m_Repeats.begin(), itLimits = RepeatLimits.begin(); itRepeat != m_Repeats.end() && itLimits != RepeatLimits.end(); ++itRepeat, ++itLimits)
	{
		YarnMesh.CopySelfToRange(*itRepeat, itLimits->first, itLimits->second);
	}

	YarnMesh.MergeNodes();
	YarnMesh.RemoveOpposingTriangles();

	Mesh.InsertMesh(YarnMesh);

	return true;
}

bool CYarn::AddSurfaceToMesh(CMesh &Mesh, const vector<XYZ> &Translations, bool bAddEndCaps) const
{
	CMesh SingleYarnMesh, FinalYarnMesh;

	if (!AddSurfaceToMesh(SingleYarnMesh, bAddEndCaps))
		return false;

	vector<XYZ>::const_iterator itTranslation;
	for (itTranslation = Translations.begin(); itTranslation != Translations.end(); ++itTranslation)
	{
		FinalYarnMesh.InsertMesh(SingleYarnMesh, *itTranslation);
	}

	FinalYarnMesh.MergeNodes();
	FinalYarnMesh.RemoveOpposingTriangles();

	Mesh.InsertMesh(FinalYarnMesh);

	return true;
}

bool CYarn::AddVolumeToMesh(CMesh &Mesh, const CDomain &Domain) const
{
	CMesh YarnMesh;
	if (AddVolumeToMesh(YarnMesh, Domain.GetTranslations(*this)))
	{
		Domain.ClipMeshToDomain(YarnMesh);
		Mesh.InsertMesh(YarnMesh);
		return true;
	}
	else
		return false;
}

bool CYarn::AddVolumeToMesh(CMesh &Mesh) const
{
	TGLOG("Adding yarn volume to mesh");
	// Build the yarn with section meshes if needed
	if (!BuildYarnIfNeeded(VOLUME))
		return false;

	// Check the section meshes are compatible
	bool bFirst = true;
	CMesh ReferenceMesh;
//    int iNumNodes = (int)m_SlaveNodes.size();
	vector<CSlaveNode>::iterator itNode;
	for (itNode = m_SlaveNodes.begin(); itNode != m_SlaveNodes.end(); ++itNode)
	{
		if (bFirst)
		{
			ReferenceMesh = itNode->GetSectionMesh();
			bFirst = false;
		}
		else
		{
			// Check mesh is compatible
			bool bCompatible = ReferenceMesh.GetNumNodes() == itNode->GetSectionMesh().GetNumNodes();
			int i;
			for (i = 0; i < CMesh::NUM_ELEMENT_TYPES; ++i)
			{
				if (!bCompatible)
					break;
				if (ReferenceMesh.GetIndices((CMesh::ELEMENT_TYPE)i) != itNode->GetSectionMesh().GetIndices((CMesh::ELEMENT_TYPE)i))
					bCompatible = false;
			}
			if (!bCompatible)
			{
				// Cannot create surface mesh if the section meshes are not compatible
				TGERROR("Unable to create volume mesh, not all section meshes are compatible");
				assert(false);
				return false;
			}
		}
	}

	// Add nodes and elements to the mesh
	list<int>::const_iterator itIndex;
	int iPrevIndex = -1;
	int iIndex;
	int aiTriangles[3];
	int aiQuads[4];
	for (itNode = m_SlaveNodes.begin(); itNode != m_SlaveNodes.end(); ++itNode)
	{
		const CMesh &SectionMesh = itNode->GetSectionMesh();
		iIndex = Mesh.InsertNodes(SectionMesh);
		const list<int> &PolygonIndices = SectionMesh.GetIndices(CMesh::POLYGON);
		for ( itIndex = PolygonIndices.begin(); itIndex != PolygonIndices.end(); ++itIndex )
		{
			Mesh.GetIndices(CMesh::POLYGON).push_back(*(itIndex) + iIndex);
		}

		if (iPrevIndex != -1)
		{
			const list<int> &TriIndices = SectionMesh.GetIndices(CMesh::TRI);
			for (itIndex = TriIndices.begin(); itIndex != TriIndices.end(); )
			{
				aiTriangles[0] = *(itIndex++);
				aiTriangles[1] = *(itIndex++);
				aiTriangles[2] = *(itIndex++);

				// Element node ordering as defined in VTK
				Mesh.GetIndices(CMesh::WEDGE).push_back(aiTriangles[0] + iPrevIndex);
				Mesh.GetIndices(CMesh::WEDGE).push_back(aiTriangles[1] + iPrevIndex);
				Mesh.GetIndices(CMesh::WEDGE).push_back(aiTriangles[2] + iPrevIndex);

				Mesh.GetIndices(CMesh::WEDGE).push_back(aiTriangles[0] + iIndex);
				Mesh.GetIndices(CMesh::WEDGE).push_back(aiTriangles[1] + iIndex);
				Mesh.GetIndices(CMesh::WEDGE).push_back(aiTriangles[2] + iIndex);
			}
			const list<int> &QuadIndices = SectionMesh.GetIndices(CMesh::QUAD);
			for (itIndex = QuadIndices.begin(); itIndex != QuadIndices.end(); )
			{
				aiQuads[0] = *(itIndex++);
				aiQuads[1] = *(itIndex++);
				aiQuads[2] = *(itIndex++);
				aiQuads[3] = *(itIndex++);

				// Element node ordering as defined in VTK
				Mesh.GetIndices(CMesh::HEX).push_back(aiQuads[0] + iIndex);
				Mesh.GetIndices(CMesh::HEX).push_back(aiQuads[1] + iIndex);
				Mesh.GetIndices(CMesh::HEX).push_back(aiQuads[2] + iIndex);
				Mesh.GetIndices(CMesh::HEX).push_back(aiQuads[3] + iIndex);

				Mesh.GetIndices(CMesh::HEX).push_back(aiQuads[0] + iPrevIndex);
				Mesh.GetIndices(CMesh::HEX).push_back(aiQuads[1] + iPrevIndex);
				Mesh.GetIndices(CMesh::HEX).push_back(aiQuads[2] + iPrevIndex);
				Mesh.GetIndices(CMesh::HEX).push_back(aiQuads[3] + iPrevIndex);
			}
		}
		iPrevIndex = iIndex;
	}
	return true;
}

bool CYarn::AddVolumeToMesh(CMesh &Mesh, const vector<pair<int, int> > &RepeatLimits) const
{
	if (m_Repeats.size() != RepeatLimits.size())
	{
		TGERROR("Unable to create repeated volume mesh, number of repeats (" << m_Repeats.size() << ") doesn't match the number of limits (" << RepeatLimits.size() << ")");
		assert(false);
		return false;
	}

	CMesh YarnMesh;

	if (!AddVolumeToMesh(YarnMesh))
		return false;

	vector<XYZ>::const_iterator itRepeat;
	vector<pair<int, int> >::const_iterator itLimits;
	for (itRepeat = m_Repeats.begin(), itLimits = RepeatLimits.begin(); itRepeat != m_Repeats.end() && itLimits != RepeatLimits.end(); ++itRepeat, ++itLimits)
	{
		YarnMesh.CopySelfToRange(*itRepeat, itLimits->first, itLimits->second);
	}

	YarnMesh.MergeNodes();

	Mesh.InsertMesh(YarnMesh);

	return true;
}

bool CYarn::AddVolumeToMesh(CMesh &Mesh, const vector<XYZ> &Translations) const
{
	CMesh SingleYarnMesh, FinalYarnMesh;

	if (!AddVolumeToMesh(SingleYarnMesh))
		return false;

	vector<XYZ>::const_iterator itTranslation;
	for (itTranslation = Translations.begin(); itTranslation != Translations.end(); ++itTranslation)
	{
		FinalYarnMesh.InsertMesh(SingleYarnMesh, *itTranslation);
	}

	FinalYarnMesh.MergeNodes();
	FinalYarnMesh.RemoveOpposingTriangles();

	Mesh.InsertMesh(FinalYarnMesh);

	return true;
}

bool CYarn::AddCentrePlaneToMesh(CMesh &Mesh) const
{
	TGLOG("Adding centre plane to mesh");
	// Build the yarn with section meshes if needed
	if (!BuildYarnIfNeeded(SURFACE))
		return false;

	YARN_POSITION_INFORMATION YarnPositionInfo;
	YarnPositionInfo.SectionLengths = m_SectionLengths;

	vector<CSlaveNode>::iterator itSlaveNode;
	vector<XY> Points2D;
	int iNumPoints = m_iNumSectionPoints / 2;

	CMesh YarnMesh;

	// Add nodes to mesh
	for (itSlaveNode = m_SlaveNodes.begin(); itSlaveNode != m_SlaveNodes.end(); ++itSlaveNode)
	{
		vector<XY> StartEnd;
		double xSpacing, ySpacing;

		YarnPositionInfo.dSectionPosition = itSlaveNode->GetT();
		YarnPositionInfo.iSection = itSlaveNode->GetIndex();

		// GetSection called with two points will return the points at 0 and 180 degrees 
		// (ie the outer points of the horizontal axis of the section)
		// Then interpolate between these points to generate the mesh
		StartEnd = m_pYarnSection->GetSection(YarnPositionInfo, 2);  
		xSpacing = (StartEnd[1].x - StartEnd[0].x)/(iNumPoints-1);
		ySpacing = (StartEnd[1].y - StartEnd[0].y)/(iNumPoints-1);

		for ( int i = 0; i < iNumPoints; ++i )
		{
			XY Point(StartEnd[0].x + i*xSpacing, StartEnd[0].y + i*ySpacing);
			XYZ Point3D = itSlaveNode->GetPointOnSection(Point);
			YarnMesh.AddNode(Point3D);
		}
		
	}
	int iOffset = Mesh.InsertNodes(YarnMesh);

	// Add elements to the mesh
	list<int>::const_iterator itIndex;
	
	int iIndex = 0, iSlaveIndex = 0;

	for (iSlaveIndex = 0; iSlaveIndex < m_SlaveNodes.size()-1; ++iSlaveIndex)
	{
		
		for (int i = 0; i < iNumPoints-1; ++i )
		{
			Mesh.GetIndices(CMesh::QUAD).push_back(i+(iSlaveIndex)*iNumPoints + iOffset);
			Mesh.GetIndices(CMesh::QUAD).push_back(i+(iSlaveIndex+1)*iNumPoints + iOffset);
			Mesh.GetIndices(CMesh::QUAD).push_back(i+1+(iSlaveIndex+1)*iNumPoints + iOffset);
			Mesh.GetIndices(CMesh::QUAD).push_back(i+1+(iSlaveIndex)*iNumPoints + iOffset);
		}
	}
	return true;
}

bool CYarn::AddCentrePlaneToMesh(CMesh &Mesh, const CDomain &Domain) const
{
	CMesh YarnMesh;
	if (AddCentrePlaneToMesh(YarnMesh, Domain.GetTranslations(*this)))
	{
		Domain.ClipMeshToDomain(YarnMesh);
		Mesh.InsertMesh(YarnMesh);
		return true;
	}
	else
		return false;
}

bool CYarn::AddCentrePlaneToMesh(CMesh &Mesh, const vector<XYZ> &Translations) const
{
	CMesh SingleYarnMesh, FinalYarnMesh;

	if (!AddCentrePlaneToMesh(SingleYarnMesh))
		return false;

	vector<XYZ>::const_iterator itTranslation;
	for (itTranslation = Translations.begin(); itTranslation != Translations.end(); ++itTranslation)
	{
		FinalYarnMesh.InsertMesh(SingleYarnMesh, *itTranslation);
	}

	FinalYarnMesh.MergeNodes();

	Mesh.InsertMesh(FinalYarnMesh);

	return true;
}

void CYarn::AddRepeat(XYZ Repeat)
{
	m_Repeats.push_back(Repeat);
}

void CYarn::ClearRepeats()
{
	m_Repeats.clear();
}

void CYarn::SetRepeats(const vector<XYZ> &Repeats)
{
	m_Repeats = Repeats;
}

bool CYarn::RepeatMatchesEnds(XYZ Repeat) const
{
	if (m_MasterNodes.empty())
		return false;
	XYZ Origin = m_MasterNodes.front().GetPosition();
	XYZ End = m_MasterNodes.back().GetPosition();
	XYZ YarnVec = End - Origin;
	const double dTolerance = 1e-6;
	if (GetLength(YarnVec, Repeat) < dTolerance)
		return true;
	if (GetLength(YarnVec, -Repeat) < dTolerance)
		return true;
	return false;
}

void CYarn::ClearMasterNodeOrientations()
{
	vector<CNode>::iterator itNode;
	for (itNode = m_MasterNodes.begin(); itNode != m_MasterNodes.end(); ++itNode)
	{
		itNode->SetTangent(XYZ());	// Set to 0
		itNode->SetUp(XYZ());	// Set to 0
	}
}

void CYarn::StraightenYarn(double dStraightness)
{
	TGLOG("Straightening yarn " << dStraightness);
	int i, j;
	double dAngle;
	XYZ DeltaPos1, DeltaPos2, Pos;
	XYZ Axis;
	WXYZ Rotation;
	for (i=1; i<(int)m_MasterNodes.size()-1; ++i)
	{
		Pos = m_MasterNodes[i].GetPosition();
		DeltaPos1 = Pos - m_MasterNodes[i-1].GetPosition();
		DeltaPos2 = m_MasterNodes[i+1].GetPosition() - Pos;

		Normalise(DeltaPos1);
		Normalise(DeltaPos2);

		dAngle = DotProduct(DeltaPos1, DeltaPos2);

		if (dAngle > 1)
			dAngle = 1;
		if (dAngle < -1)
			dAngle = -1;

		dAngle = acos(dAngle);

		if (dAngle > 0)
		{
			Axis = CrossProduct(DeltaPos2, DeltaPos1);
			Normalise(Axis);
			dAngle = dAngle * dStraightness;
			Rotation = WXYZ(Axis, dAngle);

			for (j=i+1; j<(int)m_MasterNodes.size(); ++j)
			{
				m_MasterNodes[j].SetPosition(Pos + Rotation * (m_MasterNodes[j].GetPosition()-Pos));
			}
		}
	}

	m_iNeedsBuilding = ALL;
}

bool CYarn::PointInsideYarn(const XYZ &Point, XYZ *pTangent, XY *pLoc, double* pVolumeFraction, double* pDistanceToSurface, double dTolerance, XYZ *pOrientation, XYZ *pUp, bool bSurface) const
{
	//PROFILE_FUNC()
	if (!BuildYarnIfNeeded(SURFACE))
		return false;

	if (!PointInsideBox(Point, m_AABB.first, m_AABB.second, dTolerance))
		return false;

	bool bSectionConstant = m_pYarnSection->GetType() == "CYarnSectionConstant";
	if ( pOrientation && !bSectionConstant )
	{
		if ( !BuildYarnIfNeeded(VOLUME) )  // Only need to build the volume mesh if pOrientation == true and varying sections
			return false;
	}

	int i;
	CSlaveNode N;
	double u;
	m_pInterpolation->Initialise(m_MasterNodes);

	YARN_POSITION_INFORMATION YarnPositionInfo;
	YarnPositionInfo.SectionLengths = m_SectionLengths;
	vector<XY> Section;

	bool bIsInside = false;

	XY Loc;
	XYZ Relative, Up, Side;
	int iNumSegments = (int)m_MasterNodes.size()-1;

	for (i=0; i<iNumSegments; ++i)
	{
		if (!PointInsideBox(Point, m_SectionAABBs[i].first, m_SectionAABBs[i].second, dTolerance))
			continue;
		if (FindPlaneContainingPoint(Point, u, dTolerance, i))
		{
			//TGLOG("Converged point inside yarn after " << iIterations << " iterations");
	//		cout << "Num iterations: " << iIterations << endl;

			N = m_pInterpolation->GetNode(m_MasterNodes, i, u);

			YarnPositionInfo.dSectionPosition = N.GetT();
			YarnPositionInfo.iSection = N.GetIndex();
			Section = m_pYarnSection->GetSection(YarnPositionInfo, m_iNumSectionPoints);
			N.UpdateSectionPoints(&Section);

			const vector<XY> &SectionPoints = N.Get2DSectionPoints();

			// Calculate the location of the point projected on to the cross section plane
			Relative = Point - N.GetPosition();
			Up = N.GetUp();
			Side = N.GetSide();
			Loc.x = DotProduct(Side, Relative);
			Loc.y = DotProduct(Up, Relative);

			// Check if point is inside min/max box of section. 
			XY Min, Max;
			GetMinMaxXY( SectionPoints, Min, Max );
			if ( Loc.x < Min.x || Loc.x > Max.x || Loc.y < Min.y || Loc.y > Max.y )
				continue;
			
			{
				//PROFILE_BLOCK( PointInside )
				if (!bSurface )
					bIsInside = PointInside( Loc, SectionPoints );
				else
					bIsInside = true;   // If surface assume that only exporting yarns and that been sent centre point
										// of element which must be inside (or on surface)
										// If this assumption changes will need to look at how PointInside function
										// works for point on or very close to surface
			}

			if ( bIsInside )
			{
				if (pTangent)
				{
					*pTangent = N.GetTangent();
				}
				if (pLoc)
				{
					*pLoc = Loc;
				}
				if (pVolumeFraction)
				{
					if (m_pFibreDistribution && m_pParent)
					{
						double dFibreArea = GetFibreArea(m_pParent->GetGeometryScale()+"^2");
						if (dFibreArea == 0)
							dFibreArea = m_pParent->GetFibreArea(m_pParent->GetGeometryScale()+"^2");
						vector<XY> SectionPoints = N.Get2DSectionPoints();
						if( N.GetAngle() != 0.0 )
						{
							double CosAng = cos( N.GetAngle() );
							vector<XY>::iterator itSectionPoints;
							for ( itSectionPoints = SectionPoints.begin(); itSectionPoints != SectionPoints.end(); ++itSectionPoints )
							{
								XY Point = *itSectionPoints;
								Point.x = Point.x * CosAng;
								*itSectionPoints = Point;
							}
						}
						*pVolumeFraction = m_pFibreDistribution->GetVolumeFraction(SectionPoints, dFibreArea, Loc);
						//*pVolumeFraction = m_pFibreDistribution->GetVolumeFraction(N.Get2DSectionPoints(), dFibreArea, Loc);
					}
					else
					{
						*pVolumeFraction = -1;
					}
				}
				if (pDistanceToSurface)
				{
					//PROFILE_BLOCK( DistanceToSurface )
					double dClosestEdgeDistance = FindClosestEdgeDistance( Loc, SectionPoints, dTolerance );
					if ( dClosestEdgeDistance < dTolerance )
						*pDistanceToSurface = dClosestEdgeDistance;
				}

				if ( pOrientation )
				{
					//PROFILE_BLOCK(Orientation)
					if ( bSectionConstant && N.GetAngle() == 0.0 )
					{
						*pOrientation = N.GetTangent();  // Don't need to calculate orientation if constant section
					}
					else
					{
						// Find which element of section mesh point lies in. Get section mesh for section either side and calculate orientation from elements
						CMesh SectionMesh = m_pYarnSection->GetSectionMesh(YarnPositionInfo, m_iNumSectionPoints, false);
						
						int Index;
						CMesh::ELEMENT_TYPE ElementType = GetMeshPoint( SectionMesh, Loc, Index );
						if ( ElementType != CMesh::NUM_ELEMENT_TYPES )
						{
							XYZ Ori;
							double u1, u2;
							CSlaveNode N1,N2;
							
							{
								//PROFILE_BLOCK(Orientation1)
							u1 = u > 0.1 ? u-0.1 : 0;
							//if ( u > 0.01 )  // Is 1/100th length of section suitable offset?
							//{
								N1 = m_pInterpolation->GetNode(m_MasterNodes, i, u1 );
								//N1 = m_pInterpolation->GetNode(m_MasterNodes, i, u - 0.01 );
								YarnPositionInfo.dSectionPosition = N1.GetT();
								YarnPositionInfo.iSection = N1.GetIndex();
								SectionMesh = m_pYarnSection->GetSectionMesh(YarnPositionInfo, m_iNumSectionPoints, false);  // Gets 2D section
								N1.UpdateSectionMesh( &SectionMesh );  // Converts back to 3D section
							}
							//}
							// else use N1 at master node 
							
							{
								//PROFILE_BLOCK(Orientation2)
								u2 = u < 0.99 ? u+0.1 : 1;
							//if ( u < 0.99 )
							//{
								N2 = m_pInterpolation->GetNode(m_MasterNodes, i, u2 );
								//N2 = m_pInterpolation->GetNode(m_MasterNodes, i, u + 0.01 );
								YarnPositionInfo.dSectionPosition = N2.GetT();
								YarnPositionInfo.iSection = N2.GetIndex();
								SectionMesh = m_pYarnSection->GetSectionMesh(YarnPositionInfo, m_iNumSectionPoints, false);
								N2.UpdateSectionMesh( &SectionMesh );
							//}
							}
							
							{
								//PROFILE_BLOCK(Orientation3)
							CMesh End1Mesh = N1.GetSectionMesh();
							CMesh End2Mesh = N2.GetSectionMesh();
							int iNumNodes = CMesh::GetNumNodes(ElementType);
							vector<int> ElementIndices1;
							vector<int> ElementIndices2;
							End1Mesh.ConvertElementListToVector( ElementType, ElementIndices1 );
							End2Mesh.ConvertElementListToVector( ElementType, ElementIndices2 );
							for ( int j = 0; j < iNumNodes; ++j )
							{
								Ori += End2Mesh.GetNode( ElementIndices2[Index] ) - End1Mesh.GetNode( ElementIndices1[Index] );
								Index++;
							}
							Ori /= iNumNodes;
							Normalise( Ori );
							*pOrientation = Ori;
							assert( fabs(Ori.x) > 1e-14 || fabs(Ori.y) > 1e-14 || fabs(Ori.z) > 1e-14 );
							}
						}
					}
				}
				if ( pUp )
				{
					*pUp = N.GetUp();
				}
			
				return true;
			}
		}
	}
	return false;
}

bool CYarn::PointInsideYarn(const XYZ &Point, const vector<XYZ> &Translations, XYZ *pTangent, XY* pLoc, double *pVolumeFraction, double* pDistanceToSurface, double dTolerance, XYZ *pOrientation, XYZ *pUp, bool bSurface) const
{
	// TODO OPTIMISE THIS
	vector<XYZ>::const_iterator itXYZ;
	for (itXYZ = Translations.begin(); itXYZ != Translations.end(); ++itXYZ)
	{
		if (PointInsideYarn(Point - *itXYZ, pTangent, pLoc, pVolumeFraction, pDistanceToSurface, dTolerance, pOrientation, pUp, bSurface))
			return true;
	}
	return false;
}

bool CYarn::FindPlaneContainingPoint(const XYZ &Point, double &u, double dTolerance, int iSeg) const
{
	const double dConvergenceTolerance = 1e-6;
	CSlaveNode N1, N2, N;
	double du;
	PLANE Plane1, Plane2, SearchPlane;
	double d1, d2, d, dprev;

	N1 = m_pInterpolation->GetNode(m_MasterNodes, iSeg, 0);
	N2 = m_pInterpolation->GetNode(m_MasterNodes, iSeg, 1);
		
	Plane1.Normal = N1.GetNormal();
	Plane2.Normal = -N2.GetNormal();
		

	Plane1.d = DotProduct(Plane1.Normal, N1.GetPosition());
	Plane2.d = DotProduct(Plane2.Normal, N2.GetPosition());

	d1 = DotProduct(Plane1.Normal, Point) - Plane1.d;
	d2 = DotProduct(Plane2.Normal, Point) - Plane2.d;
		
	// If true then the point lies between the two planes
	if (d1 >= -dConvergenceTolerance && d2 >= -dConvergenceTolerance)
	{
		if ( d1 == 0.0 )  // Point lies on one of end planes if d1 or d2 == 0.0
		{
			u = 0.0;
		}
		else if ( d2 == 0.0 )
		{
			u = 1.0;
		}
		else
		{
			u = d1/(d1+d2);  
			du = u;
			dprev = d1 < d2 ? d1 : d2;
			//dprev = d1;
			SearchPlane = Plane1;
			int iIterations = 0;
			while(abs(du) > dConvergenceTolerance)
			{
				N = m_pInterpolation->GetNode(m_MasterNodes, iSeg, u);

				SearchPlane.Normal = N.GetNormal();
				SearchPlane.d = DotProduct(SearchPlane.Normal, N.GetPosition());

				d = DotProduct(SearchPlane.Normal, Point) - SearchPlane.d;

				du = d * (du/(dprev-d));
				if ( du > (1-u + dConvergenceTolerance) )
					du = 1 - u + dConvergenceTolerance;
				else if ( du < (-u - dConvergenceTolerance) )
					du = -u - dConvergenceTolerance;

				u += du;
				dprev = d;
				++iIterations;
				if ( iIterations > 100 )
				{
					TGLOG("Exceeded number of iterations. PointInsideYarn failed");
					return false;  // TODO Very rare that doesn't converge but still need to find correct solution
				}
			}
		}
		return true;
	}
	return false;
}

double CYarn::FindClosestSurfacePoint(const XYZ &Point, XYZ &SurfacePoint, int iNumSectionPoints, double dTolerance )
{
	if (!BuildYarnIfNeeded(SURFACE))
		return false;

	int i;
	CSlaveNode N;
	double u;
	m_pInterpolation->Initialise(m_MasterNodes);

	YARN_POSITION_INFORMATION YarnPositionInfo;
	YarnPositionInfo.SectionLengths = m_SectionLengths;
	vector<XY> Section;

	bool bFound = false;
	XY Loc;
	XYZ Relative, Up, Side;
	int iNumSegments = (int)m_MasterNodes.size()-1;
	int index;
	double dist;

	double minDist = -1.0;
	for (i=0; i<iNumSegments; ++i)
	{
		if (FindPlaneContainingPoint(Point, u, dTolerance, i))
		{

			N = m_pInterpolation->GetNode(m_MasterNodes, i, u);

			YarnPositionInfo.dSectionPosition = N.GetT();
			YarnPositionInfo.iSection = N.GetIndex();
			Section = m_pYarnSection->GetSection(YarnPositionInfo, iNumSectionPoints);
			N.UpdateSectionPoints(&Section);
			vector<XYZ> SectionPoints = N.GetSectionPoints();
			index = GetClosestPointIndex( SectionPoints, Point);
			dist = GetLength( SectionPoints[index], Point);
			if ( dist < minDist || minDist == -1 )
			{
				minDist = dist;
				SurfacePoint = SectionPoints[index];
				bFound = true;
			}
		}
	}
	return minDist;
}

bool CYarn::FindClosestSurfacePoint(const XYZ &Point, const vector<XYZ> &Translations, XYZ &SurfacePoint, int iNumSectionPoints, double dTolerance)
{
	vector<XYZ>::const_iterator itXYZ;
	double minDist = -1, dist = -1.0;
	XYZ SPoint;
	for (itXYZ = Translations.begin(); itXYZ != Translations.end(); ++itXYZ)
	{
		dist = FindClosestSurfacePoint(Point - *itXYZ, SPoint, iNumSectionPoints, dTolerance);
		if ( fabs(dist + 1.0) > 1e-9 )  // Add 1 because FindClosestSurfacePoint returns -1 if unsuccessful
		{
			if ( dist < minDist || minDist == -1 )
			{
				minDist = dist;
				SPoint += *itXYZ;
				SurfacePoint = SPoint;
			}
		}
	}
	if ( minDist == -1 )
		return false;
	return true;
}

double CYarn::GetRawYarnLength() const
{
	if (!BuildYarnIfNeeded(LINE))
		return 0;
	return accumulate(m_SectionLengths.begin(), m_SectionLengths.end(), 0.0);
}

double CYarn::GetRawYarnVolume() const
{
	CMesh Mesh;
	AddSurfaceToMesh(Mesh);
	return Mesh.CalculateVolume();
}

double CYarn::GetRawYarnSectionLength(int iIndex) const
{
	if (!BuildYarnIfNeeded(LINE))
		return 0;
	if (iIndex < 0 || iIndex >= (int)m_SectionLengths.size())
	{
		TGERROR("Unable to get yarn section length, invalid index: " << iIndex);
		return 0;
	}
	return m_SectionLengths[iIndex];
}

vector<double> CYarn::GetYarnSectionLengths() const
{
	if (!BuildYarnIfNeeded(LINE))
		return vector<double>();
	return m_SectionLengths;
}

double CYarn::GetRawRepeatArea() const
{
	if (m_Repeats.size() != 2)
	{
		TGERROR("Yarns must contain 2 repeat vectors in order to calculate yarn length per unit area");
		return 0;
	}
	return GetLength(CrossProduct(m_Repeats[0], m_Repeats[1]));
}


pair<XYZ, XYZ> CYarn::GetAABB() const
{
	if (!BuildYarnIfNeeded(SURFACE))
		return pair<XYZ, XYZ>(XYZ(), XYZ());
	return m_AABB;
}

pair<XYZ, XYZ> CYarn::GetSectionAABB(int iIndex) const
{
	if (!BuildYarnIfNeeded(SURFACE))
		return pair<XYZ, XYZ>(XYZ(), XYZ());
	if (iIndex < 0 || iIndex >= (int)m_SectionAABBs.size())
		return pair<XYZ, XYZ>(XYZ(), XYZ());
	return m_SectionAABBs[iIndex];
}

const vector<CSlaveNode> &CYarn::GetSlaveNodes(BUILD_TYPE Usage) const
{
	BuildYarnIfNeeded(Usage);
	return m_SlaveNodes;
}

double CYarn::GetRealYarnLength(string Units) const
{
	if (!m_pParent)
		return 0;
	return ConvertUnits(GetRawYarnLength(), m_pParent->GetGeometryScale(), Units);
}

double CYarn::GetYarnLengthPerUnitArea(string Units) const
{
	if (!m_pParent)
		return 0;
	double dRepeatArea = GetRawRepeatArea();
	if (dRepeatArea == 0)
		return 0;
	double dLengthPerUnitArea = GetRawYarnLength() / dRepeatArea;
	return ConvertUnits(dLengthPerUnitArea, "/"+m_pParent->GetGeometryScale(), Units);
}

double CYarn::GetRealYarnVolume(string Units) const
{
	if (!m_pParent)
		return 0;
	double dVolume = GetRawYarnVolume();
	return ConvertUnits(dVolume, m_pParent->GetGeometryScale()+"^3", Units);
}

double CYarn::GetYarnVolumePerUnitArea(string Units) const
{
	if (!m_pParent)
		return 0;
	double dRepeatArea = GetRawRepeatArea();
	if (dRepeatArea == 0)
		return 0;
	double dVolPerUnitArea = GetRawYarnVolume() / dRepeatArea;
	return ConvertUnits(dVolPerUnitArea, m_pParent->GetGeometryScale(), Units);
}

double CYarn::GetFibreVolume(string Units) const
{
	if (!m_pParent)
		return 0;
	double dLength = GetRealYarnLength();

	// Calculate volume based on fibre area
	double dFibreArea = GetFibreArea();
	if ( dFibreArea != 0 )
	{
		double dVolume = dLength * dFibreArea;
		return ConvertUnits(dVolume, "m^3", Units);
	}

	// Otherwise calculate based on linear density
	double dYarnLinearDensity = GetYarnLinearDensity();
	// If yarn linear density is not specified for the yarn use textile value
	if (dYarnLinearDensity == 0)
		dYarnLinearDensity = m_pParent->GetYarnLinearDensity();
	if ( dYarnLinearDensity == 0 )
	{
		TGERROR( "There is no yarn linear density specified" );
		return 0;
	}
	double dMass = dYarnLinearDensity*dLength;
	double dFibreDensity = GetFibreDensity();
	// If fibre density is not specified for the yarn use textile value
	if (dFibreDensity == 0)
		dFibreDensity = m_pParent->GetFibreDensity();
	if (dFibreDensity == 0)
	{
		TGERROR( "There is no fibre density specified" );
		return 0;
	}
	double dVolume = dMass/dFibreDensity;
	return ConvertUnits(dVolume, "m^3", Units);
}

double CYarn::GetFibreVolumePerUnitArea(string Units) const
{
	if (!m_pParent)
		return 0;
	double dRepeatArea = GetRawRepeatArea();
	if (dRepeatArea == 0)
		return 0;
	double dLengthPerUnitArea = GetYarnLengthPerUnitArea();
	double dYarnLinearDensity = GetYarnLinearDensity();
	// If yarn linear density is not specified for the yarn use textile value
	if (dYarnLinearDensity == 0)
		dYarnLinearDensity = m_pParent->GetYarnLinearDensity();
	if ( dYarnLinearDensity == 0 )
	{
		TGERROR( "There is no yarn linear density specified" );
		return 0;
	}
	double dMassPerUnitArea = dYarnLinearDensity*dLengthPerUnitArea;
	double dFibreDensity = GetFibreDensity();
	// If fibre density is not specified for the yarn use textile value
	if (dFibreDensity == 0)
		dFibreDensity = m_pParent->GetFibreDensity();
	if (dFibreDensity == 0)
	{
		TGERROR( "There is no fibre density specified" );
		return 0;
	}
	double dVolPerUnitArea = dMassPerUnitArea/dFibreDensity;
	return ConvertUnits(dVolPerUnitArea, "m", Units);
}

double CYarn::GetFibreYarnVolumeFraction() const
{
	if (!m_pParent)
		return 0;
	double dYarnVolume = GetRealYarnVolume();
	if (dYarnVolume)
		return GetFibreVolume()/dYarnVolume;
	else
		return 0;
}

void CYarn::SetParent(const CTextile *pParent)
{
	m_pParent = pParent;
}


CMesh::ELEMENT_TYPE CYarn::GetMeshPoint( CMesh &Mesh, const XY &Point, int& Index ) const
{
	//PROFILE_FUNC()
	list<int>::iterator itIndex;
	list<int> &TriIndices = Mesh.GetIndices( CMesh::TRI );
	
	for ( itIndex = TriIndices.begin(), Index = 0; itIndex != TriIndices.end(); Index += 3 )
	{
		vector<XYZ> Nodes; 
		for ( int j = 0; j < 3; j++ )
		{
			Nodes.push_back( Mesh.GetNode( *itIndex++ ));
		}
		if ( PointInside( Point, Nodes ) )
		{
			return( CMesh::TRI );
		}
	}

	list<int> &QuadIndices = Mesh.GetIndices( CMesh::QUAD );
	
	for ( itIndex = QuadIndices.begin(), Index = 0; itIndex != QuadIndices.end(); Index += 4 )
	{
		vector<XYZ> Nodes; 
		for ( int j = 0; j < 4; j++ )
		{
			Nodes.push_back( Mesh.GetNode( *itIndex++ ));
		}
		if ( PointInside( Point, Nodes ) )
		{
			return( CMesh::QUAD );
		}
	}
	return CMesh::NUM_ELEMENT_TYPES;
}

bool CYarn::PointInside( const XY &Point, vector<XYZ> &Nodes ) const
{
	// Algorithm borrowed from http://paulbourke.net/geometry/polygonmesh/

	int counter = 0;
	int i, N = (int)Nodes.size();
	double xinters;
	XYZ p1, p2;
	p1 = Nodes[0];
	for (i=1;i<=N;i++)
	{
		p2 = Nodes[i % N];
		if (Point.y > min(p1.y,p2.y))
		{
			if (Point.y <= max(p1.y,p2.y))
			{
				if (Point.x <= max(p1.x,p2.x))
				{
					if (p1.y != p2.y)
					{
						xinters = (Point.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
						if (p1.x == p2.x || Point.x <= xinters)
							counter++;
					}
				}
			}
		}
		p1 = p2;
	}

	if (counter % 2 == 1)
		return true;
	else
		return false;
}

bool CYarn::PointInside( const XY &Point, const vector<XY> &Nodes ) const
{
	// Algorithm borrowed from http://paulbourke.net/geometry/polygonmesh/

	int counter = 0;
	int i, N = (int)Nodes.size();
	double xinters;
	XY p1, p2;
	p1 = Nodes[0];
	for (i=1;i<=N;i++)
	{
		p2 = Nodes[i % N];
		if (Point.y > min(p1.y,p2.y))
		{
			if (Point.y <= max(p1.y,p2.y))
			{
				if (Point.x <= max(p1.x,p2.x))
				{
					if (p1.y != p2.y)
					{
						xinters = (Point.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
						if (p1.x == p2.x || Point.x <= xinters )
							counter++;
					}
				}
			}
		}
		p1 = p2;
	}

	if (counter % 2 == 1)
		return true;
	else
		return false;
}

double CYarn::FindClosestEdgeDistance( XY &Loc, const vector<XY> &SectionPoints, double dTolerance ) const
{
	vector<XY>::const_iterator itP1, itP2;
	XY Edge, EdgeNormal;
	double dEdgeDistance;
	double dClosestEdgeDistance = 1.0;
	bool bStart = true;

	itP1 = itP2 = SectionPoints.begin();
	++itP2;
	while (itP1 != SectionPoints.end())
	{
		Edge = *itP2 - *itP1;
		// Calculate the edge normal as the vector pointing away from the section
		// This is done by rotating the edge line by 90 degrees clockwise
		EdgeNormal.x = Edge.y;
		EdgeNormal.y = -Edge.x;
		Normalise(EdgeNormal);

		dEdgeDistance = DotProduct(EdgeNormal, Loc - *itP1);

		// dClosestEdgeDistance keeps track of the closest distance from the point
		// to the section. This value may be negative or positive. Positive distance
		// represents a point outside the section, negative represents a point inside.
		// Note that this distance is not exact for points lying outside the section.
		// The distance may be less than the true value for sections with sharp edges.
		if ( dEdgeDistance < dTolerance )  // Point on innner side so check distance. 
		{
			if ( bStart )
			{
				dClosestEdgeDistance = dEdgeDistance;
				bStart = false;
			}
			else
				dClosestEdgeDistance = max(dClosestEdgeDistance, dEdgeDistance);
		}
		
		++itP1;
		++itP2;
		if (itP2 == SectionPoints.end())
			itP2 = SectionPoints.begin();
	}
	return dClosestEdgeDistance;
}

bool CYarn::ConvertToInterpNodes()
{
	// Get a copy of the yarn sections that are applied to the nodes
	string str = GetYarnSection()->GetType();
	if ( str == "CYarnSectionInterpNode" )
		return true;
	if (str != "CYarnSectionConstant")
		return false;

	CYarnSectionConstant* pYarnSection = (CYarnSectionConstant*)GetYarnSection()->Copy();
		
	CYarnSectionInterpNode NewYarnSection(false, true);
	// Add section at each node
	for ( int i = 0; i < GetNumNodes(); ++i )
	{
		NewYarnSection.AddSection( pYarnSection->GetSection() );
	}
		
	delete pYarnSection;
	AssignSection( NewYarnSection );
	return true;
}





