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
#include "TextileWeave.h"
#include "SectionEllipse.h"
#include "SectionPolygon.h"

using namespace TexGen;

CTextileWeave::CTextileWeave(int iNumYYarns, int iNumXYarns, double dSpacing, double dThickness)
: m_iNumXYarns(iNumXYarns) 
, m_iNumYYarns(iNumYYarns)
, m_dGapSize(0)
, m_dFabricThickness(dThickness)
, m_iResolution(40)
{
	m_Pattern.resize(iNumXYarns*iNumYYarns);
	YARNDATA YarnData;
	YarnData.dSpacing = dSpacing;
	YarnData.dWidth = YarnData.dSpacing;
	YarnData.dHeight = m_dFabricThickness/2;
	m_XYarnData.resize(iNumXYarns, YarnData);
	m_YYarnData.resize(iNumYYarns, YarnData);
}

CTextileWeave::~CTextileWeave(void)
{
}

CTextileWeave::CTextileWeave(TiXmlElement &Element)
: CTextile(Element)
{
	Element.Attribute("NumXYarns", &m_iNumXYarns);
	Element.Attribute("NumYYarns", &m_iNumYYarns);
	Element.Attribute("GapSize", &m_dGapSize);
	Element.Attribute("FabricThickness", &m_dFabricThickness);
	Element.Attribute("Resolution", &m_iResolution);
	TiXmlElement *pSectionMesh = Element.FirstChildElement("SectionMesh");
	if (pSectionMesh)
	{
		m_pSectionMesh = CSectionMesh::CreateSectionMesh(*pSectionMesh);
	}
	FOR_EACH_TIXMLELEMENT(pPatternCell, Element, "PatternCell")
	{
		vector<PATTERN2D> Cell;
		FOR_EACH_TIXMLELEMENT(pCell, *pPatternCell, "PatternElement")
		{
			Cell.push_back(valueify<bool>(pCell->Attribute("value")));
		}
		m_Pattern.push_back(Cell);
	}
	YARNDATA YarnData;
	FOR_EACH_TIXMLELEMENT(pYarnData, Element, "XYarnData")
	{
		pYarnData->Attribute("Width", &YarnData.dWidth);
		pYarnData->Attribute("Height", &YarnData.dHeight);
		pYarnData->Attribute("Spacing", &YarnData.dSpacing);
		m_XYarnData.push_back(YarnData);
	}
	FOR_EACH_TIXMLELEMENT(pYarnData, Element, "YYarnData")
	{
		pYarnData->Attribute("Width", &YarnData.dWidth);
		pYarnData->Attribute("Height", &YarnData.dHeight);
		pYarnData->Attribute("Spacing", &YarnData.dSpacing);
		m_YYarnData.push_back(YarnData);
	}
	FOR_EACH_TIXMLELEMENT(pYarns, Element, "XYarns")
	{
		vector<int> Indices;
		FOR_EACH_TIXMLELEMENT(pYarn, *pYarns, "XYarn")
		{
			Indices.push_back(valueify<int>(pYarn->Attribute("yarnindex")));
		}
		m_XYarns.push_back(Indices);
	}
	FOR_EACH_TIXMLELEMENT(pYarns, Element, "YYarns")
	{
		vector<int> Indices;
		FOR_EACH_TIXMLELEMENT(pYarn, *pYarns, "YYarn")
		{
			Indices.push_back(valueify<int>(pYarn->Attribute("yarnindex")));
		}
		m_YYarns.push_back(Indices);
	}
}

void CTextileWeave::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextile::PopulateTiXmlElement(Element, OutputType);

	Element.SetAttribute("NumXYarns", m_iNumXYarns);
	Element.SetAttribute("NumYYarns", m_iNumYYarns);
	Element.SetAttribute("GapSize", stringify(m_dGapSize));
	Element.SetAttribute("FabricThickness", stringify(m_dFabricThickness));
	Element.SetAttribute("Resolution", m_iResolution);
	if (m_pSectionMesh)
	{
		TiXmlElement SectionMesh("SectionMesh");
		m_pSectionMesh->PopulateTiXmlElement(SectionMesh, OutputType);
		Element.InsertEndChild(SectionMesh);
	}
	int i, j;
	for (i=0; i<(int)m_Pattern.size(); ++i)
	{
		TiXmlElement PatternCell("PatternCell");
		pair<int, int> Coords = GetCellCoordinates(i);
		PatternCell.SetAttribute("x", Coords.first);
		PatternCell.SetAttribute("y", Coords.second);
//		PatternCell.SetAttribute("index", i);
		for (j=0; j<(int)m_Pattern[i].size(); ++j)
		{
			TiXmlElement PatternElement("PatternElement");
//			PatternElement.SetAttribute("index", j);
			PatternElement.SetAttribute("value", stringify(m_Pattern[i][j]));
			PatternCell.InsertEndChild(PatternElement);
		}
		Element.InsertEndChild(PatternCell);
	}
	for (i=0; i<(int)m_XYarnData.size(); ++i)
	{
		TiXmlElement XYarnData("XYarnData");
		XYarnData.SetAttribute("index", i);
		XYarnData.SetAttribute("Width", stringify(m_XYarnData[i].dWidth));
		XYarnData.SetAttribute("Height", stringify(m_XYarnData[i].dHeight));
		XYarnData.SetAttribute("Spacing", stringify(m_XYarnData[i].dSpacing));
		Element.InsertEndChild(XYarnData);
	}
	for (i=0; i<(int)m_YYarnData.size(); ++i)
	{
		TiXmlElement YYarnData("YYarnData");
		YYarnData.SetAttribute("index", i);
		YYarnData.SetAttribute("Width", stringify(m_YYarnData[i].dWidth));
		YYarnData.SetAttribute("Height", stringify(m_YYarnData[i].dHeight));
		YYarnData.SetAttribute("Spacing", stringify(m_YYarnData[i].dSpacing));
		Element.InsertEndChild(YYarnData);
	}
	if (OutputType == OUTPUT_STANDARD ||
		OutputType == OUTPUT_FULL)
	{
		for (i=0; i<(int)m_XYarns.size(); ++i)
		{
			TiXmlElement XYarns("XYarns");
			XYarns.SetAttribute("index", i);
			for (j=0; j<(int)m_XYarns[i].size(); ++j)
			{
				TiXmlElement XYarn("XYarn");
	//			XYarn.SetAttribute("index", j);
				XYarn.SetAttribute("yarnindex", m_XYarns[i][j]);
				XYarns.InsertEndChild(XYarn);
			}
			Element.InsertEndChild(XYarns);
		}
		for (i=0; i<(int)m_YYarns.size(); ++i)
		{
			TiXmlElement YYarns("YYarns");
			YYarns.SetAttribute("index", i);
			for (j=0; j<(int)m_YYarns[i].size(); ++j)
			{
				TiXmlElement YYarn("YYarn");
	//			YYarn.SetAttribute("index", j);
				YYarn.SetAttribute("yarnindex", m_YYarns[i][j]);
				YYarns.InsertEndChild(YYarn);
			}
			Element.InsertEndChild(YYarns);
		}
	}
}

pair<int, int> CTextileWeave::GetCellCoordinates(int iIndex) const
{
	return make_pair(iIndex % m_iNumYYarns, iIndex / m_iNumYYarns);
}

const vector<PATTERN2D> &CTextileWeave::GetCell(int x, int y) const
{
	return m_Pattern[x + m_iNumYYarns*y];
}

vector<PATTERN2D> &CTextileWeave::GetCell(int x, int y)
{
	return m_Pattern[x + m_iNumYYarns*y];
}

bool CTextileWeave::Valid() const
{
	// Check number of yarns is the same in the x direction
	int i, j, k, iNumYarns, iFirstNumYarns;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		for (j=0; j<m_iNumYYarns; ++j)
		{
			const vector<PATTERN2D> &Cell = GetCell(j, i);
			iNumYarns = 0;
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN_XYARN)
					++iNumYarns;
			}
			if (j==0)
			{
				iFirstNumYarns = iNumYarns;
			}
			else
			{
				if (iFirstNumYarns != iNumYarns)
					return false;
			}
		}
	}
	// Check number of yarns is the same in the y direction
	for (j=0; j<m_iNumYYarns; ++j)
	{
		for (i=0; i<m_iNumXYarns; ++i)
		{
			const vector<PATTERN2D> &Cell = GetCell(j, i);
			iNumYarns = 0;
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN_YYARN)
					++iNumYarns;
			}
			if (i==0)
			{
				iFirstNumYarns = iNumYarns;
			}
			else
			{
				if (iFirstNumYarns != iNumYarns)
					return false;
			}
		}
	}
	return true;
}

bool CTextileWeave::BuildTextile() const
{
	m_Yarns.clear();
	m_YYarns.clear();
	m_XYarns.clear();

	m_YYarns.resize(m_iNumYYarns);
	m_XYarns.resize(m_iNumXYarns);

	if (!Valid())
		return false;

	TGLOGINDENT("Building textile weave \"" << GetName() << "\"");

	vector<int> Yarns;

	double x, y, z;

	// Add x yarns (yarns parallel to the x axis)
	int i, j, k, iYarn;
	y = 0;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		x = 0;
		Yarns.clear();
		for (j=0; j<=m_iNumYYarns; ++j)
		{
			const vector<PATTERN2D> &Cell = GetCell(j%m_iNumYYarns, i);
			if (j==0)
			{
				for (k=0; k<(int)Cell.size(); ++k)
				{
					if (Cell[k] == PATTERN_XYARN)
					{
						Yarns.push_back(AddYarn(CYarn()));
					}
				}
			}
			m_XYarns[i] = Yarns;
			iYarn = 0;
			z = m_dFabricThickness/(2*Cell.size());
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN_XYARN)
				{
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(1, 0, 0)));
					++iYarn;
				}
				z += m_dFabricThickness/Cell.size();
			}
			if (j<m_iNumYYarns)
				x += m_YYarnData[j].dSpacing;
		}
		y += m_XYarnData[i].dSpacing;
	}

	// Add y yarns (yarns parallel to the y axis)
	x = 0;
	for (j=0; j<m_iNumYYarns; ++j)
	{
		y = 0;
		Yarns.clear();
		for (i=0; i<=m_iNumXYarns; ++i)
		{
			const vector<PATTERN2D> &Cell = GetCell(j, i%m_iNumXYarns);
			if (i==0)
			{
				for (k=0; k<(int)Cell.size(); ++k)
				{
					if (Cell[k] == PATTERN_YYARN)
					{
						Yarns.push_back(AddYarn(CYarn()));
					}
				}
			}
			m_YYarns[j] = Yarns;
			iYarn = 0;
			z = m_dFabricThickness/(2*Cell.size());
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN_YYARN)
				{
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(0, 1, 0)));
					++iYarn;
				}
				z += m_dFabricThickness/Cell.size();
			}
			if (i<m_iNumXYarns)
				y += m_XYarnData[i].dSpacing;
		}
		x += m_YYarnData[j].dSpacing;
	}


	// Assign sections to the yarns
	vector<int>::iterator itpYarn;
	double dWidth, dHeight;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		dWidth = m_XYarnData[i].dWidth;
		dHeight = m_XYarnData[i].dHeight;
		CSectionEllipse Section(dWidth, dHeight);
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		for (itpYarn = m_XYarns[i].begin(); itpYarn != m_XYarns[i].end(); ++itpYarn)
		{
			m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
		}
	}
	for (i=0; i<m_iNumYYarns; ++i)
	{
		dWidth = m_YYarnData[i].dWidth;
		dHeight = m_YYarnData[i].dHeight;
		CSectionEllipse Section(dWidth, dHeight);
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		for (itpYarn = m_YYarns[i].begin(); itpYarn != m_YYarns[i].end(); ++itpYarn)
		{
			m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
		}
	}

	// Add repeats and set interpolation
	dWidth = GetWidth();
	dHeight = GetHeight();
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->AssignInterpolation(CInterpolationBezier());
		itYarn->SetResolution(m_iResolution);
		itYarn->AddRepeat(XYZ(dWidth, 0, 0));
		itYarn->AddRepeat(XYZ(0, dHeight, 0));
	}

	return true;
}

void CTextileWeave::SetThickness(double dThickness, bool bAdjustYarnHeights)
{
	m_dFabricThickness = dThickness;
	if (bAdjustYarnHeights)
	{
		SetYarnHeights(m_dFabricThickness/2);
	}
	m_bNeedsBuilding = true;
}

void CTextileWeave::SetGapSize(double dGapSize)
{
	m_dGapSize = dGapSize;
	m_bNeedsBuilding = true;
}

void CTextileWeave::SetResolution(int iResolution)
{
	m_iResolution = iResolution;
	m_bNeedsBuilding = true;
}

void CTextileWeave::AssignSectionMesh(const CSectionMesh &SectionMesh)
{
	m_pSectionMesh = SectionMesh;
	m_bNeedsBuilding = true;
}

void CTextileWeave::CorrectYarnWidths() const
{
	TGLOGINDENT("Adjusting yarn widths for \"" << GetName() << "\" with gap size of " << m_dGapSize);

	vector<vector<int> > *pTransverseYarns;
	vector<vector<int> > *pLongitudinalYarns;
	int iTransverseNum;
	int iLongitudinalNum;
	int iDir;
	int i, j;
	CMesh TransverseYarnsMesh;
	vector<int>::iterator itpYarn;
	vector<pair<int, int> > RepeatLimits;
	vector<pair<double, XYZ> > Intersections;
	XYZ Center, P;
	const CYarnSection* pYarnSection;
	const CInterpolation* pInterpolation;
	CSlaveNode Node;
	XYZ Side, Up;
	YARN_POSITION_INFORMATION YarnPosInfo;

	RepeatLimits.resize(2, pair<int, int>(-1, 0));
	vector<double> YarnMaxWidth;
	YarnMaxWidth.resize(m_Yarns.size(), -1);

	// Find the max width of the yarns...
	for (iDir=0; iDir<2; ++iDir)
	{
		switch (iDir)
		{
		case 0:
			pTransverseYarns = &m_YYarns;
			pLongitudinalYarns = &m_XYarns;
			iTransverseNum = m_iNumYYarns;
			iLongitudinalNum = m_iNumXYarns;
			break;
		case 1:
			pTransverseYarns = &m_XYarns;
			pLongitudinalYarns = &m_YYarns;
			iTransverseNum = m_iNumXYarns;
			iLongitudinalNum = m_iNumYYarns;
			break;
		}
		for (i=0; i<iTransverseNum; ++i)
		{
			TransverseYarnsMesh.Clear();
			for (itpYarn = (*pTransverseYarns)[i].begin(); itpYarn != (*pTransverseYarns)[i].end(); ++itpYarn)
			{
				m_Yarns[*itpYarn].AddSurfaceToMesh(TransverseYarnsMesh, RepeatLimits);
			}
			TransverseYarnsMesh.Convert3Dto2D();
			TransverseYarnsMesh.ConvertQuadstoTriangles();
//			CElementsOctree Octree;
//			TransverseYarnsMesh.BuildElementOctree(CMesh::TRI, Octree);
			for (j=0; j<iLongitudinalNum; ++j)
			{
				for (itpYarn = (*pLongitudinalYarns)[j].begin(); itpYarn != (*pLongitudinalYarns)[j].end(); ++itpYarn)
				{
					YarnPosInfo.iSection = i;
					YarnPosInfo.dSectionPosition = 0;
					YarnPosInfo.SectionLengths = m_Yarns[*itpYarn].GetYarnSectionLengths();

					pInterpolation = m_Yarns[*itpYarn].GetInterpolation();
					Node = pInterpolation->GetNode(m_Yarns[*itpYarn].GetMasterNodes(), i, 0);
					Up = Node.GetUp();
					Side = CrossProduct(Node.GetTangent(), Up);

					pYarnSection = m_Yarns[*itpYarn].GetYarnSection();
					// Get 2 section points, one at either side of the cross-section 
					vector<XY> Points = pYarnSection->GetSection(YarnPosInfo, 2);
					Center = m_Yarns[*itpYarn].GetMasterNodes()[i].GetPosition();
					vector<XY>::iterator itPoint;
					for (itPoint = Points.begin(); itPoint != Points.end(); ++itPoint)
					{
						P = itPoint->x * Side + itPoint->y * Up + Center;
						// Find intersection of side points of longitudinal yarn with transvese yarn meshes
						if (TransverseYarnsMesh.IntersectLine(Center, P, Intersections, make_pair(true, false)))
						{
							double dU = Intersections[0].first;
							XYZ Normal = Intersections[0].second;
							double dProjectedGap = m_dGapSize / DotProduct(Normal, Center-P);
							dU -= 0.5 * dProjectedGap;
							if (dU < 0)
								dU = 0;
							if (dU < 1)
							{
								double dWidth = 2 * GetLength(*itPoint) * dU;
								if (YarnMaxWidth[*itpYarn] < 0 || dWidth < YarnMaxWidth[*itpYarn])
								{
									YarnMaxWidth[*itpYarn] = dWidth;
								}
							}
						}
					}
				}
			}
		}
	}

	// Adjust yarn widths
	double dWidth, dHeight;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		for (itpYarn = m_XYarns[i].begin(); itpYarn != m_XYarns[i].end(); ++itpYarn)
		{
			if (YarnMaxWidth[*itpYarn] >= 0)
			{
				if (m_XYarnData[i].dWidth > YarnMaxWidth[*itpYarn])
				{
					TGLOG("Changing X yarn width " << i << " from " << m_XYarnData[i].dWidth << " to " << YarnMaxWidth[*itpYarn]);
					m_XYarnData[i].dWidth = YarnMaxWidth[*itpYarn];
				}
			}
		}
	}
	for (i=0; i<m_iNumYYarns; ++i)
	{
		for (itpYarn = m_YYarns[i].begin(); itpYarn != m_YYarns[i].end(); ++itpYarn)
		{
			if (YarnMaxWidth[*itpYarn] >= 0)
			{
				if (m_YYarnData[i].dWidth > YarnMaxWidth[*itpYarn])
				{
					TGLOG("Changing Y yarn width " << i << " from " << m_YYarnData[i].dWidth << " to " << YarnMaxWidth[*itpYarn]);
					m_YYarnData[i].dWidth = YarnMaxWidth[*itpYarn];
				}
			}
		}
	}

	// Assign sections to the yarns
	for (i=0; i<m_iNumXYarns; ++i)
	{
		dWidth = m_XYarnData[i].dWidth;
		dHeight = m_XYarnData[i].dHeight;
		CSectionEllipse Section(dWidth, dHeight);
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		for (itpYarn = m_XYarns[i].begin(); itpYarn != m_XYarns[i].end(); ++itpYarn)
		{
			m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
		}
	}
	for (i=0; i<m_iNumYYarns; ++i)
	{
		dWidth = m_YYarnData[i].dWidth;
		dHeight = m_YYarnData[i].dHeight;
		CSectionEllipse Section(dWidth, dHeight);
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		for (itpYarn = m_YYarns[i].begin(); itpYarn != m_YYarns[i].end(); ++itpYarn)
		{
			m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
		}
	}
}

bool CTextileWeave::FlattenYarns(double dFlatLevel, int iUpDown)
{
	YARN_POSITION_INFORMATION YarnPosInfo;
	// for all yarns
	for(unsigned int i=0;i<m_Yarns.size();++i)
	{
		// get the Current section and cast 
		const CYarnSection* ysection = m_Yarns[i].GetYarnSection();
		if (ysection->GetType()!="CYarnSectionInterpNode")
		{
			TGERROR("Cannot flatten a yarn with section type: " << ysection->GetType())
			return false;
		}
		CYarnSectionInterpNode NewYarnSection((CYarnSectionInterpNode&)(*ysection));
		const std::vector<CNode>& m_nodes = m_Yarns[i].GetMasterNodes();
		const int n_section_points = m_Yarns[i].GetNumSectionPoints();
		YarnPosInfo.SectionLengths = m_Yarns[i].GetYarnSectionLengths();
		YarnPosInfo.dSectionPosition = 0;
		double z_mid = 0.0;
		XYZ npos;
		for(unsigned int j=0;j<m_nodes.size();++j)
		{
			npos = m_nodes[j].GetPosition();
			z_mid += npos.z/m_nodes.size();
		} 
		// for all master nodes on the yarn		
		for(unsigned int j=0;j<m_nodes.size();++j)
		{
			YarnPosInfo.iSection = j%(m_nodes.size()-1);
			npos  = m_nodes[j].GetPosition();
			std::vector<XY> section = NewYarnSection.GetSection(YarnPosInfo, n_section_points);
			for(unsigned int k=0;k<section.size();++k)
			{
				if(npos.z<z_mid)
				{
					if(section[k].y<-dFlatLevel	&&	iUpDown<=0)
						section[k].y = - dFlatLevel;
				}
				else if (npos.z>z_mid	&&	iUpDown>=0)
				{
					if (section[k].y>dFlatLevel)
						section[k].y = dFlatLevel;
				}
			}
			NewYarnSection.ReplaceSection(j,CSectionPolygon(section));
		}
		m_Yarns[i].AssignSection(NewYarnSection);
	}
	return true;
}

void CTextileWeave::CorrectInterference() const
{
	TGLOGINDENT("Correcting interference for \"" << GetName() << 
		"\" with gap size of " << m_dGapSize);

	vector<vector<int> > *pTransverseYarns;
	vector<vector<int> > *pLongitudinalYarns;
	int iTransverseNum;
	int iLongitudinalNum;
	int iDir;
	int i, j, k;
	CMesh TransverseYarnsMesh;
	vector<int>::iterator itpYarn;
	vector<pair<int, int> > RepeatLimits;
	vector<pair<double, XYZ> > Intersections;
	XYZ Center, P;
	const CYarnSection* pYarnSection;
	const CInterpolation* pInterpolation;
	CSlaveNode Node;
	XYZ Side, Up;
	YARN_POSITION_INFORMATION YarnPosInfo;

	RepeatLimits.resize(2, pair<int, int>(-1, 0));
	vector<double> Modifiers;
	vector<vector<vector<double> > > YarnSectionModifiers;
	YarnSectionModifiers.resize(m_Yarns.size());

	// Find at how much the cross sections need to be compressed to leave given gap size
	for (iDir=0; iDir<2; ++iDir)
	{
		switch (iDir)
		{
		case 0:
			pTransverseYarns = &m_YYarns;
			pLongitudinalYarns = &m_XYarns;
			iTransverseNum = m_iNumYYarns;
			iLongitudinalNum = m_iNumXYarns;
			break;
		case 1:
			pTransverseYarns = &m_XYarns;
			pLongitudinalYarns = &m_YYarns;
			iTransverseNum = m_iNumXYarns;
			iLongitudinalNum = m_iNumYYarns;
			break;
		}
		for (i=0; i<iTransverseNum; ++i)
		{
			TransverseYarnsMesh.Clear();
			for (itpYarn = (*pTransverseYarns)[i].begin(); itpYarn != (*pTransverseYarns)[i].end(); ++itpYarn)
			{
				m_Yarns[*itpYarn].AddSurfaceToMesh(TransverseYarnsMesh, RepeatLimits);
			}
			TransverseYarnsMesh.Convert3Dto2D();
			TransverseYarnsMesh.ConvertQuadstoTriangles();
//			CElementsOctree Octree;
//			TransverseYarnsMesh.BuildElementOctree(CMesh::TRI, Octree);
			for (j=0; j<iLongitudinalNum; ++j)
			{
				for (itpYarn = (*pLongitudinalYarns)[j].begin(); itpYarn != (*pLongitudinalYarns)[j].end(); ++itpYarn)
				{
					YarnPosInfo.iSection = i;
					YarnPosInfo.dSectionPosition = 0;
					YarnPosInfo.SectionLengths = m_Yarns[*itpYarn].GetYarnSectionLengths();

					pInterpolation = m_Yarns[*itpYarn].GetInterpolation();
					Node = pInterpolation->GetNode(m_Yarns[*itpYarn].GetMasterNodes(), i, 0);
					Up = Node.GetUp();
					Side = CrossProduct(Node.GetTangent(), Up);

					pYarnSection = m_Yarns[*itpYarn].GetYarnSection();
					vector<XY> Points = pYarnSection->GetSection(YarnPosInfo, m_Yarns[*itpYarn].GetNumSectionPoints());
					Center = m_Yarns[*itpYarn].GetMasterNodes()[i].GetPosition();
					vector<XY>::iterator itPoint;
					Modifiers.clear();
					for (itPoint = Points.begin(); itPoint != Points.end(); ++itPoint)
					{
						P = itPoint->x * Side + itPoint->y * Up + Center;
						if (TransverseYarnsMesh.IntersectLine(Center, P, Intersections, make_pair(true, false)))
						{
							double dU = Intersections[0].first;
							XYZ Normal = Intersections[0].second;
							double dProjectedGap = m_dGapSize / DotProduct(Normal, Center-P);
							dU -= 0.5 * dProjectedGap;
							if (dU > 1)
								dU = 1;
							if (dU < 0)
								dU = 0;
							Modifiers.push_back(dU);
						}
						else
							Modifiers.push_back(1);
					}
					YarnSectionModifiers[*itpYarn].push_back(Modifiers);
				}
			}
		}
	}
	vector<XY> Points;
	for (i=0; i<(int)m_Yarns.size(); ++i)
	{
		CYarnSectionInterpNode NewYarnSection(false, true);
		pYarnSection = m_Yarns[i].GetYarnSection();
		YarnPosInfo.SectionLengths = m_Yarns[i].GetYarnSectionLengths();
		// Add sections at the nodes
		YarnPosInfo.dSectionPosition = 0;
		for (j=0; j<(int)YarnSectionModifiers[i].size(); ++j)
		{
			YarnPosInfo.iSection = j;

			Points = pYarnSection->GetSection(YarnPosInfo, YarnSectionModifiers[i][j].size());
			for (k=0; k<(int)Points.size(); ++k)
			{
				Points[k] *= YarnSectionModifiers[i][j][k];
			}
			CSectionPolygon Section(Points);
			if (m_pSectionMesh)
				Section.AssignSectionMesh(*m_pSectionMesh);
			NewYarnSection.AddSection(Section);
		}
		NewYarnSection.AddSection(NewYarnSection.GetNodeSection(0));

		// Add sections between the nodes (not necessary for sections that don't cross)
		YarnPosInfo.dSectionPosition = 0.5;
		for (j=0; j<(int)YarnSectionModifiers[i].size(); ++j)
		{
			if (NeedsMidSection(i, j))
			{
				YarnPosInfo.iSection = j;

				Points = pYarnSection->GetSection(YarnPosInfo, YarnSectionModifiers[i][j].size());
				for (k=0; k<(int)Points.size(); ++k)
				{
					Points[k] *= min(YarnSectionModifiers[i][j][k], YarnSectionModifiers[i][(j+1)%YarnSectionModifiers[i].size()][k]);
				}
				CSectionPolygon Section(Points);
				if (m_pSectionMesh)
					Section.AssignSectionMesh(*m_pSectionMesh);
				NewYarnSection.InsertSection(j, 0.5, Section);
			}
		}
		m_Yarns[i].AssignSection(NewYarnSection);
	}
}

bool CTextileWeave::NeedsMidSection(int iYarn, int iSection) const
{
	int i, j;
	for (i=0; i<(int)m_XYarns.size(); ++i)
	{
		for (j=0; j<(int)m_XYarns[i].size(); ++j)  // Steps through layers - why does it need to do this if only used for 2D weaves?
		{
			if (m_XYarns[i][j] == iYarn)
			{
				int iNextSection = (iSection+1) % m_YYarns.size();
				return !(GetCell(iSection, i) == GetCell(iNextSection, i));
			}
		}
	}
	for (i=0; i<(int)m_YYarns.size(); ++i)
	{
		for (j=0; j<(int)m_YYarns[i].size(); ++j)
		{
			if (m_YYarns[i][j] == iYarn)  
			{
				int iNextSection = (iSection+1) % m_XYarns.size();
				return !(GetCell(i, iSection) == GetCell(i, iNextSection));
			}
		}
	}
	return true;
}

void CTextileWeave::SetXYarnWidths(int iIndex, double dWidth)
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to set yarn width, index out of range: " << iIndex);
		return;
	}
	m_XYarnData[iIndex].dWidth = dWidth;
	m_bNeedsBuilding = true;
}

void CTextileWeave::SetYYarnWidths(int iIndex, double dWidth)
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to set yarn width, index out of range: " << iIndex);
		return;
	}
	m_YYarnData[iIndex].dWidth = dWidth;
	m_bNeedsBuilding = true;
}

void CTextileWeave::SetXYarnHeights(int iIndex, double dHeight)
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to set yarn height, index out of range: " << iIndex);
		return;
	}
	m_XYarnData[iIndex].dHeight = dHeight;
	m_bNeedsBuilding = true;
}

void CTextileWeave::SetYYarnHeights(int iIndex, double dHeight)
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to set yarn height, index out of range: " << iIndex);
		return;
	}
	m_YYarnData[iIndex].dHeight = dHeight;
	m_bNeedsBuilding = true;
}

void CTextileWeave::SetXYarnSpacings(int iIndex, double dSpacing)
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to set yarn spacing, index out of range: " << iIndex);
		return;
	}
	m_XYarnData[iIndex].dSpacing = dSpacing;
	m_bNeedsBuilding = true;
}

void CTextileWeave::SetYYarnSpacings(int iIndex, double dSpacing)
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to set yarn spacing, index out of range: " << iIndex);
		return;
	}
	m_YYarnData[iIndex].dSpacing = dSpacing;
	m_bNeedsBuilding = true;
}


double CTextileWeave::GetXYarnWidths(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to set yarn width, index out of range: " << iIndex);
		return 0;
	}
	return m_XYarnData[iIndex].dWidth;
}

double CTextileWeave::GetYYarnWidths(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to get yarn width, index out of range: " << iIndex);
		return 0;
	}
	return m_YYarnData[iIndex].dWidth;
}

double CTextileWeave::GetXYarnHeights(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to get yarn height, index out of range: " << iIndex);
		return 0;
	}
	return m_XYarnData[iIndex].dHeight;
}

double CTextileWeave::GetYYarnHeights(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to get yarn height, index out of range: " << iIndex);
		return 0;
	}
	return m_YYarnData[iIndex].dHeight;
}

double CTextileWeave::GetXYarnSpacings(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to get yarn spacing, index out of range: " << iIndex);
		return 0;
	}
	return m_XYarnData[iIndex].dSpacing;
}

double CTextileWeave::GetYYarnSpacings(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to get yarn spacing, index out of range: " << iIndex);
		return 0;
	}
	return m_YYarnData[iIndex].dSpacing;
}

double CTextileWeave::GetXYarnGapSize(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to get yarn spacing, index out of range: " << iIndex);
		return 0;
	}
	double dGapSize = m_XYarnData[iIndex].dSpacing;
	dGapSize -= 0.5*m_XYarnData[iIndex].dWidth;
	if (iIndex == m_iNumXYarns-1)
		dGapSize -= 0.5*m_XYarnData[0].dWidth;
	else
		dGapSize -= 0.5*m_XYarnData[iIndex+1].dWidth;
	return dGapSize;
}

double CTextileWeave::GetYYarnGapSize(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to get yarn spacing, index out of range: " << iIndex);
		return 0;
	}
	double dGapSize = m_YYarnData[iIndex].dSpacing;
	dGapSize -= 0.5*m_YYarnData[iIndex].dWidth;
	if (iIndex == m_iNumYYarns-1)
		dGapSize -= 0.5*m_YYarnData[0].dWidth;
	else
		dGapSize -= 0.5*m_YYarnData[iIndex+1].dWidth;
	return dGapSize;
}


void CTextileWeave::SetXYarnWidths(double dWidth)
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		SetXYarnWidths(i, dWidth);
	}
}

void CTextileWeave::SetYYarnWidths(double dWidth)
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		SetYYarnWidths(i, dWidth);
	}
}

void CTextileWeave::SetXYarnHeights(double dHeight)
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		SetXYarnHeights(i, dHeight);
	}
}

void CTextileWeave::SetYYarnHeights(double dHeight)
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		SetYYarnHeights(i, dHeight);
	}
}

void CTextileWeave::SetXYarnSpacings(double dSpacing)
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		SetXYarnSpacings(i, dSpacing);
	}
}

void CTextileWeave::SetYYarnSpacings(double dSpacing)
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		SetYYarnSpacings(i, dSpacing);
	}
}


void CTextileWeave::SetYarnWidths(double dWidth)
{
	SetXYarnWidths(dWidth);
	SetYYarnWidths(dWidth);
}

void CTextileWeave::SetYarnHeights(double dHeight)
{
	SetXYarnHeights(dHeight);
	SetYYarnHeights(dHeight);
}

void CTextileWeave::SetYarnSpacings(double dSpacing)
{
	SetXYarnSpacings(dSpacing);
	SetYYarnSpacings(dSpacing);
}

double CTextileWeave::GetWidth() const
{
	double dWidth = 0;

	int i;

	for (i=0; i<m_iNumYYarns; ++i)
	{
		dWidth += m_YYarnData[i].dSpacing;
	}

	return dWidth;
}

double CTextileWeave::GetHeight() const
{
	double dHeight = 0;

	int j;

	for (j=0; j<m_iNumXYarns; ++j)
	{
		dHeight += m_XYarnData[j].dSpacing;
	}

	return dHeight;
}

CDomainPlanes CTextileWeave::GetDefaultDomain( bool bSheared, bool bAddedHeight )
{
	XYZ Min, Max;
	double dGap = 0.0;
	if ( bAddedHeight )
		dGap = 0.05*m_dFabricThickness;
	Min.x = -0.5*m_YYarnData[m_iNumYYarns-1].dSpacing;
	Min.y = -0.5*m_XYarnData[m_iNumXYarns-1].dSpacing;
//	Min.x = m_YYarnData[0].dSpacing;
//	Min.y = m_XYarnData[0].dSpacing;
	Min.z = -dGap;
	Max.x = Min.x + GetWidth();
	Max.y = Min.y + GetHeight();
	Max.z = m_dFabricThickness+dGap;
	return CDomainPlanes(Min, Max);
}

void CTextileWeave::AssignDefaultDomain( bool bSheared, bool bAddedHeight )
{
	CDomainPlanes Domain = GetDefaultDomain( bSheared, bAddedHeight );
	AssignDomain(Domain);
}

string CTextileWeave::GetDefaultName() const
{
	 return "Weave(W:" + stringify(m_iNumYYarns) + ",H:" + stringify(m_iNumXYarns) + ")";
}

double CTextileWeave::GetAverageYarnWidth() const
{
	int i;

	double dWidth = 0;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		dWidth += m_XYarnData[i].dWidth;
	}
	for (i=0; i<m_iNumYYarns; ++i)
	{
		dWidth += m_YYarnData[i].dWidth;
	}

	return dWidth/(m_iNumXYarns+m_iNumYYarns);
}

vector<CYarn*> CTextileWeave::GetXYarns(int iIndex)
{
	vector<CYarn*> Yarns;
	if (!BuildTextileIfNeeded())
		return Yarns;
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to get yarn, index out of range: " << iIndex);
		return Yarns;
	}
	vector<int>::iterator itIndex;
	for (itIndex = m_XYarns[iIndex].begin(); itIndex != m_XYarns[iIndex].end(); ++itIndex)
	{
		Yarns.push_back(&m_Yarns[*itIndex]);
	}
	return Yarns;
}

vector<CYarn*> CTextileWeave::GetYYarns(int iIndex)
{
	vector<CYarn*> Yarns;
	if (!BuildTextileIfNeeded())
		return Yarns;
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to get yarn, index out of range: " << iIndex);
	}
	vector<int>::iterator itIndex;
	for (itIndex = m_YYarns[iIndex].begin(); itIndex != m_YYarns[iIndex].end(); ++itIndex)
	{
		Yarns.push_back(&m_Yarns[*itIndex]);
	}
	return Yarns;
}

int CTextileWeave::GetYarnIndex(int x, int y, int z) const
{
	int iXYarnCount = 0;
	int iYYarnCount = 0;
	vector<PATTERN2D> Pattern = GetCell(x, y);
	int k;
	for (k=0; k<z; ++k)
	{
		if (Pattern[k] == PATTERN_XYARN)
			++iXYarnCount;
		else
			++iYYarnCount;
	}
	if (Pattern[z] == PATTERN_XYARN)
		return m_XYarns[y][iXYarnCount];
	else
		return m_YYarns[x][iYYarnCount];
}

void CTextileWeave::CorrectEdgeInterference()
{
	vector<vector<int> > *pTransverseYarns;
	vector<vector<int> > *pLongitudinalYarns;
	int iTransverseNum;
	int iLongitudinalNum;
	int iDir;

	for (iDir=0; iDir<2; ++iDir)  
	{
		switch (iDir)
		{
		case 0:
			pTransverseYarns = &m_YYarns;
			pLongitudinalYarns = &m_XYarns;
			iTransverseNum = m_iNumYYarns;
			iLongitudinalNum = m_iNumXYarns;
			break;
		case 1:
			pTransverseYarns = &m_XYarns;
			pLongitudinalYarns = &m_YYarns;
			iTransverseNum = m_iNumXYarns;
			iLongitudinalNum = m_iNumYYarns;
			break;
		}
		
	//	if (pYarn->GetYarnSection()->GetType() != "CYarnSectionInterpNode")
	//			return false;
	
		for ( int i = 0; i < iTransverseNum-1; ++i )
		{
			CYarn TransverseYarn1 = m_Yarns[(*pTransverseYarns)[i][0]];
			CYarn TransverseYarn2 = m_Yarns[(*pTransverseYarns)[i+1][0]];
			vector<CNode> TransverseNodes1, TransverseNodes2;
			TransverseNodes1 = TransverseYarn1.GetMasterNodes();
			TransverseNodes2 = TransverseYarn2.GetMasterNodes();
			
			CYarnSectionInterpNode* pYarnSections1 = (CYarnSectionInterpNode*)TransverseYarn1.GetYarnSection()->Copy();
			CYarnSectionInterpNode* pYarnSections2 = (CYarnSectionInterpNode*)TransverseYarn2.GetYarnSection()->Copy();
			
			CYarnSectionInterpNode NewYarnSections1, NewYarnSections2;
			
			for (int j = 0; j < iLongitudinalNum; ++j )
			{
			CSectionEllipse* YarnSection1 = NULL;
			CSectionEllipse* YarnSection2 = NULL;
			XYZ Point1 = TransverseNodes1[j].GetPosition();
			XYZ Point2 = TransverseNodes2[j].GetPosition();
			
			double dist = GetLength(Point1, Point2);
			if ( pYarnSections1->GetNodeSection(j).GetType() != "CSectionEllipse" || pYarnSections2->GetNodeSection(j).GetType() != "CSectionEllipse" )
			continue;
			YarnSection1 = (CSectionEllipse*)pYarnSections1->GetNodeSection(j).Copy();
			YarnSection2 = (CSectionEllipse*)pYarnSections2->GetNodeSection(j).Copy();
			double Width1 = YarnSection1->GetWidth();
			double Width2 = YarnSection2->GetWidth();
			
			double dist2 = 0.5*(Width1+Width2);
			dist -=  dist2;
			if ( dist < 0.0 )
			{
			CNode Node1 = TransverseNodes1[j];
			CNode Node2 = TransverseNodes2[j];
			//double Offset = -0.5 * dist;
			if ( iDir == 0 )
			{
			Point1.x += 0.25 * dist;
			Point2.x -= 0.25 * dist;  // Keep the opposite edge from the overlap at the same place
			}
			else
			{
			Point1.y += 0.25 * dist;
			Point2.y -= 0.25 * dist;  // Want to add whole dist or just y component?
			}
			
			Node1.SetPosition( Point1 );
			Node2.SetPosition( Point2 );  
			//pYarnSections2->ReplaceSection( j, Node );
			m_Yarns[(*pTransverseYarns)[i][0]].ReplaceNode( j, Node1 );
			m_Yarns[(*pTransverseYarns)[i+1][0]].ReplaceNode(j, Node2 );
			YarnSection2->SetWidth( Width2 + 0.5*dist );  // dist is -ve so reduces width by overlap
			YarnSection1->SetWidth( Width1 + 0.5*dist );
			}
			NewYarnSections1.AddSection( *YarnSection1 );
			NewYarnSections2.AddSection( *YarnSection2 );
			
			delete YarnSection1;
			delete YarnSection2;
			}	
			NewYarnSections1.AddSection( pYarnSections1->GetNodeSection(iLongitudinalNum) );
			NewYarnSections2.AddSection( pYarnSections2->GetNodeSection(iLongitudinalNum) );
			m_Yarns[(*pTransverseYarns)[i][0]].AssignSection( NewYarnSections1 );
			m_Yarns[(*pTransverseYarns)[i+1][0]].AssignSection( NewYarnSections2 );	
			delete pYarnSections1;
			delete pYarnSections2;
		}
	}
}




