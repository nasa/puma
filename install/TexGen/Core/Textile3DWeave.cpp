/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2013 Louise Brown

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
#include "Textile3DWeave.h"
//#include "SectionEllipse.h"
#include "SectionPowerEllipse.h"
#include "SectionPolygon.h"
#include "SectionHybrid.h"
#include "PatternDraft.h"
#include "ReedData.h"

using namespace TexGen;

#define TOL 0.000001

CTextile3DWeave::CTextile3DWeave(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing,
								 double dXHeight, double dYHeight, bool bRefine )
: m_iNumXYarns(iNumXYarns) 
, m_iNumYYarns(iNumYYarns)
, m_dGapSize(0)
, m_iResolution(40)
, m_iWarpRatio(1)
, m_iBinderRatio(1)
, m_dWarpPower(0.5)
, m_dWeftPower(0.5)
, m_dBinderPower(0.5)
, m_bRefine(bRefine)
, m_dMaxVolumeFraction(0.78)
, m_bWeftRepeat(true)
{
	m_Pattern.resize(iNumXYarns*iNumYYarns);
	YARNDATA YarnData;
	YarnData.dSpacing = dXSpacing;
	YarnData.dWidth = YarnData.dSpacing;
	YarnData.dHeight = dXHeight;
	//YarnData.iFibresPerYarn = 0;
	//YarnData.dFibreDiameter = 0.0;
	m_XYarnData.resize(iNumXYarns, YarnData);

	YarnData.dSpacing = dYSpacing;
	YarnData.dWidth = YarnData.dSpacing;
	YarnData.dHeight = dYHeight;
	m_YYarnData.resize(iNumYYarns, YarnData);
}

CTextile3DWeave::~CTextile3DWeave(void)
{
}

CTextile3DWeave::CTextile3DWeave(TiXmlElement &Element)
: CTextile(Element)
{
	Element.Attribute("NumXYarns", &m_iNumXYarns);
	Element.Attribute("NumYYarns", &m_iNumYYarns);
	Element.Attribute("GapSize", &m_dGapSize);
	Element.Attribute("FabricThickness", &m_dFabricThickness);
	Element.Attribute("Resolution", &m_iResolution);
	Element.Attribute("WarpRatio", &m_iWarpRatio);
	Element.Attribute("BinderRatio", &m_iBinderRatio);
	Element.Attribute("WarpPower", &m_dWarpPower);
	Element.Attribute("WeftPower", &m_dWeftPower);
	Element.Attribute("BinderPower", &m_dBinderPower);
	TiXmlElement *pSectionMesh = Element.FirstChildElement("SectionMesh");
	if (pSectionMesh)
	{
		m_pSectionMesh = CSectionMesh::CreateSectionMesh(*pSectionMesh);
	}
	FOR_EACH_TIXMLELEMENT(pPatternCell, Element, "PatternCell")
	{
		vector<PATTERN3D> Cell;
		FOR_EACH_TIXMLELEMENT(pCell, *pPatternCell, "PatternElement")
		{
			Cell.push_back(valueify<int>(pCell->Attribute("value")));
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
	SetBinderPattern();
}

void CTextile3DWeave::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextile::PopulateTiXmlElement(Element, OutputType);

	Element.SetAttribute("NumXYarns", m_iNumXYarns);
	Element.SetAttribute("NumYYarns", m_iNumYYarns);
	Element.SetAttribute("GapSize", stringify(m_dGapSize));
	Element.SetAttribute("FabricThickness", stringify(m_dFabricThickness));
	Element.SetAttribute("Resolution", m_iResolution);
	Element.SetAttribute("WarpRatio", m_iWarpRatio);
	Element.SetAttribute("BinderRatio", m_iBinderRatio);
	Element.SetAttribute("WarpPower",stringify(m_dWarpPower));
	Element.SetAttribute("WeftPower", stringify(m_dWeftPower));
	Element.SetAttribute("BinderPower", stringify(m_dBinderPower));
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

pair<int, int> CTextile3DWeave::GetCellCoordinates(int iIndex) const
{
	return make_pair(iIndex % m_iNumYYarns, iIndex / m_iNumYYarns);
}

const vector<PATTERN3D> &CTextile3DWeave::GetCell(int x, int y) const
{
	return m_Pattern[x + m_iNumYYarns*y];
}

vector<PATTERN3D> &CTextile3DWeave::GetCell(int x, int y)
{
	return m_Pattern[x + m_iNumYYarns*y];
}

void CTextile3DWeave::RemoveCell(int x, int y)
{
	m_Pattern.erase(m_Pattern.begin()+(x + m_iNumYYarns*y));
}

bool CTextile3DWeave::Valid() const
{
	// Check number of yarns is the same in the x direction
	int i, j, k, iNumYarns, iFirstNumYarns;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		for (j=0; j<m_iNumYYarns; ++j)
		{
			const vector<PATTERN3D> &Cell = GetCell(j, i);
			iNumYarns = 0;
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_XYARN)
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
			const vector<PATTERN3D> &Cell = GetCell(j, i);
			iNumYarns = 0;
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_YYARN)
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

bool CTextile3DWeave::BuildTextile() const
{
	m_Yarns.clear();
	m_YYarns.clear();
	m_XYarns.clear();

	m_YYarns.resize(m_iNumYYarns);
	m_XYarns.resize(m_iNumXYarns);

	m_dMinZ = 0.0;
	m_dMaxZ = 0.0;

/*	if (!Valid())
		return false;*/

	TGLOGINDENT("Building textile weave \"" << GetName() << "\"");

	bool bBinderYarns = BinderYarns();
	vector<int> Yarns;

	double x, y, z;

	// Add x yarns (yarns parallel to the x axis)
	int i, j, k, iYarn;
	y = 0;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		y += m_XYarnData[i].dSpacing/2.0;
		x = 0;
		Yarns.clear();
		for (j=0; j<=m_iNumYYarns; ++j)
		{
			const vector<PATTERN3D> &Cell = GetCell(j%m_iNumYYarns, i);
			int NextCellIndex;
			NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex );
			if (j==0)
			{
				for (k=0; k<(int)Cell.size(); ++k)
				{
					if (Cell[k] == PATTERN3D_XYARN)
					{
						Yarns.push_back(AddYarn(CYarn()));
					}
				}
			}
			m_XYarns[i] = Yarns;
			iYarn = 0;

			x += m_YYarnData[j%m_iNumYYarns].dSpacing/2.0;
			
			z = 0.0;
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_XYARN)
				{
					double dHalfHeight = m_XYarnData[i].dHeight / 2.0;
					if ( k == 0 && IsBinderYarn(i) )
					{
						z -= dHalfHeight;
						if ( (z - dHalfHeight) < m_dMinZ )
							m_dMinZ = z - dHalfHeight;
					}
					else
						z += dHalfHeight;
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(1, 0, 0)));
					++iYarn;
					z += dHalfHeight;
					if ( z > m_dMaxZ )
						m_dMaxZ = z;
				}
				else if ( Cell[k] == PATTERN3D_YYARN )
				{
					z += m_YYarnData[j%m_iNumYYarns].dHeight;
				}
				//else //if ( k > 0 )// PATTERN3D_NOYARN and not on bottom binder layer
				else if ( ((k > 0 && k < (int)Cell.size()-1) && bBinderYarns) || !bBinderYarns  ) 
				{
					int CellType = NextCell[k];
					if ( CellType == PATTERN3D_NOYARN )
					{
						do
						{
							NextCellIndex = FindNextCellIndex(NextCellIndex);
							const vector<PATTERN3D> &FindCell = GetCell(j%m_iNumYYarns, NextCellIndex );
							CellType = FindCell[k];
						} while ( CellType == PATTERN3D_NOYARN );
					}
					if ( CellType == PATTERN3D_XYARN )
					{
						z += m_XYarnData[NextCellIndex].dHeight;
					}
					else if ( CellType == PATTERN3D_YYARN )
					{
						z += m_YYarnData[j%m_iNumYYarns].dHeight;
					}
					else // PATTERN3D_NOYARN
					{					
					}
				}
			}
			if (j<m_iNumYYarns)
				x += m_YYarnData[j].dSpacing/2.0;
		}
		y += m_XYarnData[i].dSpacing/2.0;
	}

	// Add y yarns (yarns parallel to the y axis)
	x = 0;
	for (j=0; j<m_iNumYYarns; ++j)
	{
		y = 0;
		Yarns.clear();
		x += m_YYarnData[j].dSpacing/2.0;
		int iYCount = 0;

		int NumXYarns = m_iNumXYarns;
		if ( !m_bWeftRepeat )
			NumXYarns--;
		for (i=0; i<=NumXYarns; ++i)
		{
			const vector<PATTERN3D> &Cell = GetCell(j, i%m_iNumXYarns);
			
			int NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex);
			
			if ( !bBinderYarns )
			{
				for (k=0; k<(int)Cell.size(); ++k)
				{
					int Count = 0;
					if (Cell[k] == PATTERN3D_YYARN )
					{
						Count++;
						if ( Count > iYCount )   // Allow for partial weft yarns
						{
							Yarns.push_back(AddYarn(CYarn()));
							iYCount++;
						}
					}
				}
			}
			else if ( i == 0 )
			{
				for (k=0; k<(int)Cell.size(); ++k)
				{
					if (Cell[k] == PATTERN3D_YYARN)
					{
						Yarns.push_back(AddYarn(CYarn()));
					}
				}
			}

			m_YYarns[j] = Yarns;
			iYarn = 0;
			y += m_XYarnData[i%m_iNumXYarns].dSpacing/2.0;
			z = 0.0;
			//z = m_dFabricThickness/(2*Cell.size());
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_YYARN)
				{
					double dHalfHeight = m_YYarnData[j].dHeight / 2.0;
					z += dHalfHeight;
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(0, 1, 0)));
					++iYarn;
					z += dHalfHeight;
				}
				else if ( Cell[k] == PATTERN3D_XYARN && k > 0 ) // Don't adjust z if it's the binder yarn
				{
					z += m_XYarnData[i%m_iNumXYarns].dHeight;
				}
				//else //if ( k > 0 ) // PATTERN3D_NOYARN and not on bottom binder layer
				else if ( ((k > 0 && k < (int)Cell.size()-1) && bBinderYarns) || !bBinderYarns  ) 
				{
					int CellType = NextCell[k];
					if ( CellType == PATTERN3D_NOYARN )
					{
						do
						{
							NextCellIndex = FindNextCellIndex(NextCellIndex);
							const vector<PATTERN3D> &FindCell = GetCell(j%m_iNumYYarns, NextCellIndex );
							CellType = FindCell[k];
						} while ( CellType == PATTERN3D_NOYARN );
					}
					if ( CellType == PATTERN3D_XYARN )
					{
						z += m_XYarnData[NextCellIndex].dHeight;
					}
					else if ( CellType == PATTERN3D_YYARN )
					{
						z += m_YYarnData[j%m_iNumYYarns].dHeight;
					}
					else // PATTERN3D_NOYARN
					{
						// Does this ever happen?
					}
				}
			}
			if (i<m_iNumXYarns)
				y += m_XYarnData[i].dSpacing/2.0;
		}
		x += m_YYarnData[j].dSpacing/2.0;
	}


	// Assign sections to the yarns
	vector<int>::iterator itpYarn;
	double dWidth, dHeight;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		dWidth = m_XYarnData[i].dWidth;
		dHeight = m_XYarnData[i].dHeight;

		//CSectionEllipse Section(dWidth, dHeight);
		CSectionPowerEllipse Section(dWidth, dHeight, IsBinderYarn(i) ? m_dBinderPower : m_dWarpPower );
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		for (itpYarn = m_XYarns[i].begin(); itpYarn != m_XYarns[i].end(); ++itpYarn)
		{
			m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
			int iType = IsBinderYarn(i) ? BINDER : WARP;
			SetYarnProperties( m_Yarns[*itpYarn], iType );
		}
	}
	for (i=0; i<m_iNumYYarns; ++i)
	{
		dWidth = m_YYarnData[i].dWidth;
		dHeight = m_YYarnData[i].dHeight;
		//CSectionEllipse Section(dWidth, dHeight);
		CSectionPowerEllipse Section(dWidth, dHeight, m_dWeftPower);
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		for (itpYarn = m_YYarns[i].begin(); itpYarn != m_YYarns[i].end(); ++itpYarn)
		{
			m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
			SetYarnProperties( m_Yarns[*itpYarn], WEFT );
		}
	}

	ShapeWeftYarns();

	// Add repeats and set interpolation
	dWidth = GetWidth();
	dHeight = GetHeight();
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->AssignInterpolation(CInterpolationBezier());
		itYarn->SetResolution(m_iResolution);
		itYarn->AddRepeat(XYZ(dWidth, 0, 0));
		if ( m_bWeftRepeat )
			itYarn->AddRepeat(XYZ(0, dHeight, 0));
	}

	return true;
}

/*bool CTextile3DWeave::BuildTextile() const
{
	m_Yarns.clear();
	m_YYarns.clear();
	m_XYarns.clear();

	m_YYarns.resize(m_iNumYYarns);
	m_XYarns.resize(m_iNumXYarns);

	m_dMinZ = 0.0;
	m_dMaxZ = 0.0;

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
		y += m_XYarnData[i].dSpacing/2.0;
		x = 0;
		Yarns.clear();
		for (j=0; j<=m_iNumYYarns; ++j)
		{
			const vector<PATTERN3D> &Cell = GetCell(j%m_iNumYYarns, i);
			int NextCellIndex;
			NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex );
			if (j==0)
			{
				for (k=0; k<(int)Cell.size(); ++k)
				{
					if (Cell[k] == PATTERN3D_XYARN)
					{
						Yarns.push_back(AddYarn(CYarn()));
					}
				}
			}
			m_XYarns[i] = Yarns;
			iYarn = 0;

			x += m_YYarnData[j%m_iNumYYarns].dSpacing/2.0;
			
			z = 0.0;
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_XYARN)
				{
					double dHalfHeight = m_XYarnData[i].dHeight / 2.0;
					if ( k == 0 && IsBinderYarn(i) )
					{
						z -= dHalfHeight;
						if ( (z - dHalfHeight) < m_dMinZ )
							m_dMinZ = z - dHalfHeight;
					}
					else
						z += dHalfHeight;
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(1, 0, 0)));
					++iYarn;
					z += dHalfHeight;
					if ( z > m_dMaxZ )
						m_dMaxZ = z;
				}
				else if ( Cell[k] == PATTERN3D_YYARN )
				{
					z += m_YYarnData[j%m_iNumYYarns].dHeight;
				}
				else if ( k > 0 )// PATTERN3D_NOYARN and not on bottom binder layer
				{
					if ( NextCell[k] == PATTERN3D_XYARN )
					{
						z += m_XYarnData[NextCellIndex].dHeight;
					}
					else if ( NextCell[k] == PATTERN3D_YYARN )
					{
						z += m_YYarnData[j%m_iNumYYarns].dHeight;
					}
					else // PATTERN3D_NOYARN
					{
						// Does this ever happen?
					}
				}
			}
			if (j<m_iNumYYarns)
				x += m_YYarnData[j].dSpacing/2.0;
		}
		y += m_XYarnData[i].dSpacing/2.0;
	}

	// Add y yarns (yarns parallel to the y axis)
	x = 0;
	for (j=0; j<m_iNumYYarns; ++j)
	{
		y = 0;
		Yarns.clear();
		x += m_YYarnData[j].dSpacing/2.0;
		for (i=0; i<=m_iNumXYarns; ++i)
		{
			const vector<PATTERN3D> &Cell = GetCell(j, i%m_iNumXYarns);
			
			int NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex);
			if (i==0)
			{
				for (k=0; k<(int)Cell.size(); ++k)
				{
					if (Cell[k] == PATTERN3D_YYARN)
					{
						Yarns.push_back(AddYarn(CYarn()));
					}
				}
			}
			m_YYarns[j] = Yarns;
			iYarn = 0;
			y += m_XYarnData[i%m_iNumXYarns].dSpacing/2.0;
			z = 0.0;
			//z = m_dFabricThickness/(2*Cell.size());
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_YYARN)
				{
					double dHalfHeight = m_YYarnData[j].dHeight / 2.0;
					z += dHalfHeight;
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(0, 1, 0)));
					++iYarn;
					z += dHalfHeight;
				}
				else if ( Cell[k] == PATTERN3D_XYARN && k > 0 ) // Don't adjust z if it's the binder yarn
				{
					z += m_XYarnData[i%m_iNumXYarns].dHeight;
				}
				else if ( k > 0 ) // PATTERN3D_NOYARN and not on bottom binder layer
				{
					if ( NextCell[k] == PATTERN3D_XYARN )
					{
						z += m_XYarnData[NextCellIndex].dHeight;
					}
					else if ( NextCell[k] == PATTERN3D_YYARN )
					{
						z += m_YYarnData[j%m_iNumYYarns].dHeight;
					}
					else // PATTERN3D_NOYARN
					{
						// Does this ever happen?
					}
				}
			}
			if (i<m_iNumXYarns)
				y += m_XYarnData[i].dSpacing/2.0;
		}
		x += m_YYarnData[j].dSpacing/2.0;
	}


	// Assign sections to the yarns
	vector<int>::iterator itpYarn;
	double dWidth, dHeight;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		dWidth = m_XYarnData[i].dWidth;
		dHeight = m_XYarnData[i].dHeight;

		//CSectionEllipse Section(dWidth, dHeight);
		CSectionPowerEllipse Section(dWidth, dHeight, IsBinderYarn(i) ? m_dBinderPower : m_dWarpPower );
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
		//CSectionEllipse Section(dWidth, dHeight);
		CSectionPowerEllipse Section(dWidth, dHeight, m_dWeftPower);
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
}*/

void CTextile3DWeave::SetThickness(double dThickness)
{
	m_dFabricThickness = dThickness;
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::SetMaxVolFraction( double dVolFraction )
{
	m_dMaxVolumeFraction = dVolFraction;
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::SetGapSize(double dGapSize)
{
	m_dGapSize = dGapSize;
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::SetResolution(int iResolution)
{
	m_iResolution = iResolution;
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::AssignSectionMesh(const CSectionMesh &SectionMesh)
{
	m_pSectionMesh = SectionMesh;
	m_bNeedsBuilding = true;
}



/*bool CTextile3DWeave::NeedsMidSection(int iYarn, int iSection) const
{
	int i, j;
	for (i=0; i<(int)m_XYarns.size(); ++i)
	{
		for (j=0; j<(int)m_XYarns[i].size(); ++j)
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
			if (m_YYarns[i][0] == iYarn)
			{
				int iNextSection = (iSection+1) % m_XYarns.size();
				return !(GetCell(i, iSection) == GetCell(i, iNextSection));
			}
		}
	}
	return true;
}*/

void CTextile3DWeave::SetXYarnWidths(int iIndex, double dWidth)
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to set yarn width, index out of range: " << iIndex);
		return;
	}
	m_XYarnData[iIndex].dWidth = dWidth;
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::SetYYarnWidths(int iIndex, double dWidth)
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to set yarn width, index out of range: " << iIndex);
		return;
	}
	m_YYarnData[iIndex].dWidth = dWidth;
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::SetXYarnHeights(int iIndex, double dHeight)
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to set yarn height, index out of range: " << iIndex);
		return;
	}
	m_XYarnData[iIndex].dHeight = dHeight;
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::SetYYarnHeights(int iIndex, double dHeight)
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to set yarn height, index out of range: " << iIndex);
		return;
	}
	m_YYarnData[iIndex].dHeight = dHeight;
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::SetXYarnSpacings(int iIndex, double dSpacing)
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to set yarn spacing, index out of range: " << iIndex);
		return;
	}
	m_XYarnData[iIndex].dSpacing = dSpacing;
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::SetYYarnSpacings(int iIndex, double dSpacing)
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to set yarn spacing, index out of range: " << iIndex);
		return;
	}
	m_YYarnData[iIndex].dSpacing = dSpacing;
	m_bNeedsBuilding = true;
}

/*void CTextile3DWeave::SetXYarnFibresPerYarn( int iIndex, int iNumFibres )
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to set fibres per yarn, index out of range: " << iIndex);
		return;
	}
	m_XYarnData[iIndex].iFibresPerYarn = iNumFibres;
	m_bNeedsBuilding = true;
}
		
void CTextile3DWeave::SetYYarnFibresPerYarn( int iIndex, int iNumFibres )
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to set fibres per yarn, index out of range: " << iIndex);
		return;
	}
	m_YYarnData[iIndex].iFibresPerYarn = iNumFibres;
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::SetXYarnFibreDiameter( int iIndex, double dFibreDiameter )
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to set fibre diameter, index out of range: " << iIndex);
		return;
	}
	m_XYarnData[iIndex].dFibreDiameter = dFibreDiameter;
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::SetYYarnFibreDiameter( int iIndex, double dFibreDiameter )
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to set fibre diameter, index out of range: " << iIndex);
		return;
	}
	m_YYarnData[iIndex].dFibreDiameter = dFibreDiameter;
	m_bNeedsBuilding = true;
}*/


double CTextile3DWeave::GetXYarnWidths(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to set yarn width, index out of range: " << iIndex);
		return 0;
	}
	return m_XYarnData[iIndex].dWidth;
}

double CTextile3DWeave::GetYYarnWidths(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to get yarn width, index out of range: " << iIndex);
		return 0;
	}
	return m_YYarnData[iIndex].dWidth;
}

double CTextile3DWeave::GetXYarnHeights(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to get yarn height, index out of range: " << iIndex);
		return 0;
	}
	return m_XYarnData[iIndex].dHeight;
}

double CTextile3DWeave::GetYYarnHeights(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to get yarn height, index out of range: " << iIndex);
		return 0;
	}
	return m_YYarnData[iIndex].dHeight;
}

double CTextile3DWeave::GetXYarnSpacings(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to get yarn spacing, index out of range: " << iIndex);
		return 0;
	}
	return m_XYarnData[iIndex].dSpacing;
}

double CTextile3DWeave::GetYYarnSpacings(int iIndex) const
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to get yarn spacing, index out of range: " << iIndex);
		return 0;
	}
	return m_YYarnData[iIndex].dSpacing;
}

/*int CTextile3DWeave::GetXYarnFibresPerYarn( int iIndex ) const
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to get fibres per yarn, index out of range: " << iIndex);
		return 0;
	}
	return m_XYarnData[iIndex].iFibresPerYarn;
}

int CTextile3DWeave::GetYYarnFibresPerYarn( int iIndex ) const
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to get fibres per yarn, index out of range: " << iIndex);
		return 0;
	}
	return m_YYarnData[iIndex].iFibresPerYarn;
}

double CTextile3DWeave::GetXYarnFibreDiameter( int iIndex ) const
{
	if (iIndex<0 || iIndex>=m_iNumXYarns)
	{
		TGERROR("Unable to get fibre diameter, index out of range: " << iIndex);
		return 0;
	}
	return m_XYarnData[iIndex].dFibreDiameter;
}

double CTextile3DWeave::GetYYarnFibreDiameter( int iIndex ) const
{
	if (iIndex<0 || iIndex>=m_iNumYYarns)
	{
		TGERROR("Unable to get fibre diameter, index out of range: " << iIndex);
		return 0;
	}
	return m_YYarnData[iIndex].dFibreDiameter;
}*/

double CTextile3DWeave::GetXYarnGapSize(int iIndex) const
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

double CTextile3DWeave::GetYYarnGapSize(int iIndex) const
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


void CTextile3DWeave::SetXYarnWidths(double dWidth)
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		SetXYarnWidths(i, dWidth);
	}
}

void CTextile3DWeave::SetWarpYarnWidths( double dWidth )
{
	int i;
	for ( i=0; i < m_iNumXYarns; ++i )
	{
		if ( !IsBinderYarn( i ) )
		{
			SetXYarnWidths( i, dWidth );
		}
	}
}

void CTextile3DWeave::SetBinderYarnWidths( double dWidth )
{
	int i;
	for ( i=0; i < m_iNumXYarns; ++i )
	{
		if ( IsBinderYarn( i ) )
		{
			SetXYarnWidths( i, dWidth );
		}
	}
}

void CTextile3DWeave::SetYYarnWidths(double dWidth)
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		SetYYarnWidths(i, dWidth);
	}
}

void CTextile3DWeave::SetXYarnHeights(double dHeight)
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		SetXYarnHeights(i, dHeight);
	}
}

void CTextile3DWeave::SetYYarnHeights(double dHeight)
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		SetYYarnHeights(i, dHeight);
	}
}

void CTextile3DWeave::SetWarpYarnHeights( double dHeight )
{
	int i;
	for ( i=0; i < m_iNumXYarns; ++i )
	{
		if ( !IsBinderYarn( i ) )
		{
			SetXYarnHeights( i, dHeight );
		}
	}
}

void CTextile3DWeave::SetBinderYarnHeights( double dHeight )
{
	int i;
	for ( i=0; i < m_iNumXYarns; ++i )
	{
		if ( IsBinderYarn( i ) )
		{
			SetXYarnHeights( i, dHeight );
		}
	}
}

void CTextile3DWeave::SetXYarnSpacings(double dSpacing)
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		SetXYarnSpacings(i, dSpacing);
	}
}

void CTextile3DWeave::SetWarpYarnSpacings( double dSpacing )
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		if ( !IsBinderYarn( i ) )
		{
			SetXYarnSpacings(i, dSpacing);
		}
	}
}

void CTextile3DWeave::SetBinderYarnSpacings( double dSpacing )
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		if ( IsBinderYarn( i ) )
		{
			SetXYarnSpacings(i, dSpacing);
		}
	}
}

void CTextile3DWeave::SetYYarnSpacings(double dSpacing)
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		SetYYarnSpacings(i, dSpacing);
	}
}

/*void CTextile3DWeave::SetWarpYarnFibresPerYarn( int iNumFibres )
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		if ( !IsBinderYarn(i) )
		{
			SetXYarnFibresPerYarn(i, iNumFibres);
		}
	}
}

void CTextile3DWeave::SetYYarnFibresPerYarn( int iNumFibres )
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		SetYYarnFibresPerYarn(i, iNumFibres);
	}
}

void CTextile3DWeave::SetBinderYarnFibresPerYarn( int iNumFibres )
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		if ( IsBinderYarn(i) )
		{
			SetXYarnFibresPerYarn(i, iNumFibres);
		}
	}
}

void CTextile3DWeave::SetWarpYarnFibreDiameter( double dFibreDiameter )
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		if ( !IsBinderYarn(i) )
		{
			SetXYarnFibreDiameter(i, dFibreDiameter);
		}
	}
}

void CTextile3DWeave::SetYYarnFibreDiameter( double dFibreDiameter )
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		SetYYarnFibreDiameter(i, dFibreDiameter);
	}
}

void CTextile3DWeave::SetBinderYarnFibreDiameter( double dFibreDiameter )
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		if ( IsBinderYarn(i) )
		{
			SetXYarnFibreDiameter(i, dFibreDiameter);
		}
	}
}*/

void CTextile3DWeave::SetYarnLinearDensity( int iYarnType, double dValue, string Units )
{
	m_Properties[iYarnType].SetYarnLinearDensity( dValue, Units );
}

void CTextile3DWeave::SetFibreDensity(int iYarnType, double dValue, string Units)
{
	m_Properties[iYarnType].SetFibreDensity(dValue, Units);
}

void CTextile3DWeave::SetFibreArea(int iYarnType, double dValue, string Units)
{
	m_Properties[iYarnType].SetFibreArea(dValue, Units);
}

void CTextile3DWeave::SetFibresPerYarn( int iYarnType, int iNumFibres ) 
{
	m_Properties[iYarnType].SetFibresPerYarn( iNumFibres );
}

void CTextile3DWeave::SetFibreDiameter( int iYarnType, double dValue, string Units )
{
	m_Properties[iYarnType].SetFibreDiameter( dValue, Units );
}

double CTextile3DWeave::GetFibreArea( int iYarnType, string Units )
{
	return m_Properties[iYarnType].GetFibreArea( Units );
}

void CTextile3DWeave::SetYarnWidths(double dWidth)
{
	SetXYarnWidths(dWidth);
	SetYYarnWidths(dWidth);
}

void CTextile3DWeave::SetYarnHeights(double dHeight)
{
	SetXYarnHeights(dHeight);
	SetYYarnHeights(dHeight);
}

void CTextile3DWeave::SetYarnSpacings(double dSpacing)
{
	SetXYarnSpacings(dSpacing);
	SetYYarnSpacings(dSpacing);
}

double CTextile3DWeave::GetWidth() const
{
	double dWidth = 0;

	int i;

	for (i=0; i<m_iNumYYarns; ++i)
	{
		dWidth += m_YYarnData[i].dSpacing;
	}

	return dWidth;
}

double CTextile3DWeave::GetHeight() const
{
	double dHeight = 0;

	int j;

	for (j=0; j<m_iNumXYarns; ++j)
	{
		dHeight += m_XYarnData[j].dSpacing;
	}

	return dHeight;
}

CDomainPlanes CTextile3DWeave::GetDefaultDomain( bool bAddedHeight )
{
	XYZ Min, Max;
	
	FindMinMaxZ( Min.z, Max.z );
	double dGap = 0.0;
	if ( bAddedHeight )
		dGap = 0.05 * ( Max.z - Min.z );
	Min.z -= dGap;
	Max.z += dGap;
	 
	Min.x = 0.0;
	Min.y = 0.0;
	
	Max.x = Min.x + GetWidth();
	Max.y = Min.y + GetHeight();
	
	return CDomainPlanes(Min, Max);
}

void CTextile3DWeave::AssignDefaultDomain( bool bAddedHeight )
{
	CDomainPlanes Domain = GetDefaultDomain( bAddedHeight );
	AssignDomain(Domain);
}

void CTextile3DWeave::SetDomainZValues() 
{
	CDomain* Domain = GetDomain();
	if ( Domain && Domain->GetType() == "CDomainPlanes" )
	{
		XYZ Min, Max;
		if ( ((CDomainPlanes*)Domain)->GetBoxLimits( Min, Max ) )
		{
			if (BuildTextileIfNeeded())
			{
				Min.z = m_dMinZ;
				Max.z = m_dMaxZ;
				RemoveDomain();
				CDomainPlanes NewDomain = CDomainPlanes( Min, Max );
				AssignDomain( NewDomain );
			}
		}
	}
}

string CTextile3DWeave::GetDefaultName() const
{
	 return "3DWeave(W:" + stringify(m_iNumYYarns) + ",H:" + stringify(m_iNumXYarns) + ")";
}

double CTextile3DWeave::GetAverageYarnWidth() const
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

double CTextile3DWeave::GetAverageWeftYarnWidth() const
{
	int i;

	double dWidth = 0;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		dWidth += m_YYarnData[i].dWidth;
	}

	return dWidth/m_iNumYYarns;
}

double CTextile3DWeave::GetAverageWeftYarnHeight() const
{
	int i;

	double dHeight = 0;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		dHeight += m_YYarnData[i].dHeight;
	}

	return dHeight/m_iNumYYarns;
}


vector<CYarn*> CTextile3DWeave::GetXYarns(int iIndex)
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

vector<CYarn*> CTextile3DWeave::GetYYarns(int iIndex)
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

int CTextile3DWeave::GetYarnIndex(int x, int y, int z) const
{
	int iXYarnCount = 0;
	int iYYarnCount = 0;
	vector<PATTERN3D> Pattern = GetCell(x, y);
	int k;
	if ( Pattern[z] == PATTERN3D_NOYARN )
		return -1;

	for (k=0; k<z; ++k)
	{
		if (Pattern[k] == PATTERN3D_XYARN)
			++iXYarnCount;
		else if (Pattern[k] == PATTERN3D_YYARN)
			++iYYarnCount;
	}
	if (Pattern[z] == PATTERN3D_XYARN)
		return m_XYarns[y][iXYarnCount];
	else
		return m_YYarns[x][iYYarnCount];
}

void CTextile3DWeave::AddYLayers(int x, int iNumberLayers)
{
	if (x<0 || x>=m_iNumYYarns)
	{
		TGERROR("Unable to add yarn layer, index out of range: " << x);
		return;
	}
	int i, j;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		for (j=0; j<iNumberLayers; ++j)
			GetCell(x, i).push_back(PATTERN3D_YYARN);
	}
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::AddXLayers(int y, int iNumberLayers)
{
	if (y<0 || y>=m_iNumXYarns)
	{
		TGERROR("Unable to add yarn layer, index out of range: " << y);
		return;
	}
	int i, j;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		for (j=0; j<iNumberLayers; ++j)
			GetCell(i, y).push_back(PATTERN3D_XYARN);
	}
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::AddXYarn(int y, PATTERN3D Pattern )
{
	if (y<0 || y>=m_iNumXYarns)
	{
		TGERROR("Unable to add yarn layer, index out of range: " << y);
		return;
	}
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		GetCell(i, y).push_back( Pattern );
	}
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::AddYYarn( int x, PATTERN3D Pattern )
{
	if ( x<0 || x>=m_iNumYYarns)
	{
		TGERROR("Unable to add yarn layer, index out of range: " << x);
		return;
	}
	
	for ( int i = 0; i < m_iNumXYarns; ++i )
	{
		GetCell(x,i).push_back( Pattern );
	}
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::AddYLayers(int iNumberLayers)
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		AddYLayers(i, iNumberLayers);
	}
}

void CTextile3DWeave::AddXLayers(int iNumberLayers)
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		AddXLayers(i, iNumberLayers);
	}
}

void CTextile3DWeave::AddNoYarnLayer()
{
	int i;
	for ( i = 0; i < m_iNumXYarns; ++i )
	{
		AddXYarn( i, PATTERN3D_NOYARN );
	}
}

void CTextile3DWeave::AddWarpLayer()
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		if ( IsBinderYarn( i ) )
		{
			AddXYarn( i, PATTERN3D_NOYARN );
		}
		else
		{
			AddXYarn( i, PATTERN3D_XYARN );
		}
	}
}

void CTextile3DWeave::AddBinderLayer()
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		if ( IsBinderYarn( i ) )
		{
			AddXYarn( i, PATTERN3D_XYARN );
		}
		else
		{
			AddXYarn( i, PATTERN3D_NOYARN );
		}
	}
}

void CTextile3DWeave::DeleteYLayers(int x, int iNumberLayers)
{
	if (x<0 || x>=m_iNumYYarns)
	{
		TGERROR("Unable to delete yarn layer, index out of range: " << x);
		return;
	}
	int i, j;
	vector<PATTERN3D>::reverse_iterator itCell;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		for (j=0; j<iNumberLayers; ++j)
		{
			vector<PATTERN3D> &Cell = GetCell(x, i);
			itCell = find(Cell.rbegin(), Cell.rend(), (int)PATTERN3D_YYARN);
			if (itCell != Cell.rend())
			{
				Cell.erase(itCell.base()-1);
			}
		}
	}
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::DeleteXLayers(int y, int iNumberLayers)
{
	if (y<0 || y>=m_iNumXYarns)
	{
		TGERROR("Unable to delete yarn layer, index out of range: " << y);
		return;
	}
	int i, j;
	vector<PATTERN3D>::reverse_iterator itCell;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		for (j=0; j<iNumberLayers; ++j)
		{
			vector<PATTERN3D> &Cell = GetCell(i, y);
			itCell = find(Cell.rbegin(), Cell.rend(), (int)PATTERN3D_XYARN);
			if (itCell != Cell.rend())
			{
				Cell.erase(itCell.base()-1);
			}
		}
	}
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::DeleteYLayers(int iNumberLayers)
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		DeleteYLayers(i, iNumberLayers);
	}
}

void CTextile3DWeave::DeleteXLayers(int iNumberLayers)
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		DeleteXLayers(i, iNumberLayers);
	}
}

int CTextile3DWeave::GetNumYLayers(int x) const
{
	const vector<PATTERN3D> &Cell = GetCell(x, 0);
	return count(Cell.begin(), Cell.end(), (int)PATTERN3D_YYARN);
}

int CTextile3DWeave::GetNumXLayers(int y) const
{
	const vector<PATTERN3D> &Cell = GetCell(0, y);
	return count(Cell.begin(), Cell.end(), (int)PATTERN3D_XYARN);
}

int CTextile3DWeave::GetMaxNumLayers() const
{
	int i, j;
	int iMaxLayers = 0;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		for (j=0; j<m_iNumXYarns; ++j)
		{
			iMaxLayers = max(iMaxLayers, (int)GetCell(i, j).size());
		}
	}
	return iMaxLayers;
}

void CTextile3DWeave::SwapPosition(int x, int y, int iLevel1, int iLevel2)
{
	if (x<0 || x>=m_iNumYYarns || y<0 || y>=m_iNumXYarns)
	{
		TGERROR("Unable to swap position, index out of range: " << x << ", " << y);
		return;
	}
	vector<PATTERN3D> &Cell = GetCell(x, y);
	if (iLevel1 < 0 || iLevel1 >= (int)Cell.size())
	{
		TGERROR("Unable to swap position, level out of range: " << iLevel1);
		return;
	}
	if (iLevel2 < 0 || iLevel2 >= (int)Cell.size())
	{
		TGERROR("Unable to swap position, level out of range: " << iLevel2);
		return;
	}
	int bTemp;
	bTemp = Cell[iLevel1];
	Cell[iLevel1] = Cell[iLevel2];
	Cell[iLevel2] = bTemp;
	m_bNeedsBuilding = true;
}

/*double CTextile3DWeave::GetFabricThickness()
{
	return ( 
}*/

bool CTextile3DWeave::IsBinderYarn( int index ) const
{
	if ( m_BinderPattern.size() > index )
		return m_BinderPattern[index];

	int iRepeat = m_iWarpRatio + m_iBinderRatio;

	if ( (index % iRepeat) < m_iWarpRatio )
		return WARP;
	return BINDER;
}

int CTextile3DWeave::GetXYarnIndex( int YarnInd )
{
	for ( int i = 0; i < m_XYarns.size(); ++i )
	{
		vector<int>::iterator itYarns;
		for ( itYarns = m_XYarns[i].begin(); itYarns != m_XYarns[i].end(); ++itYarns )
		{
			if ( *itYarns == YarnInd )
				return i;
		}
	}
	return -1;
}

bool CTextile3DWeave::BinderYarns() const
{
	for ( int i = 0; i < m_BinderPattern.size(); ++i )
	{
		if ( m_BinderPattern[i] )
			return true;
	}
	return false;
}

void CTextile3DWeave::SetWarpRatio( int iWarpRatio )
{ 
	m_iWarpRatio = iWarpRatio; 
	SetBinderPattern();
}

void CTextile3DWeave::SetBinderRatio( int iBinderRatio )
{
	if ( iBinderRatio < 1 )
	{
		TGERROR("Binder ratio must be greater than 0");
		return;
	}
	m_iBinderRatio = iBinderRatio; 
	SetBinderPattern();
}

void CTextile3DWeave::SetBinderPattern()
{
	m_BinderPattern.clear();
	int iRepeat = m_iWarpRatio + m_iBinderRatio;
	for ( int i = 0; i < m_iNumXYarns; ++i )
	{
		if ( (i % iRepeat) < m_iWarpRatio )
			m_BinderPattern.push_back( false );
		else
			m_BinderPattern.push_back( true );
	}
}

void CTextile3DWeave::SetBinderPattern( vector<bool> &BinderPattern )
{
	m_BinderPattern.clear();
	m_BinderPattern = BinderPattern;
}

int CTextile3DWeave::FindNextCellIndex( int StartIndex ) const
{
	int i = StartIndex + 1;
	while ( i < m_iNumXYarns && IsBinderYarn( i ) )
	{
		i++;
	}
	if ( i < m_iNumXYarns )
		return i;

	if ( StartIndex == 0 )
		return StartIndex;
	i = StartIndex - 1;
	while( i >= 0 && IsBinderYarn( i ) )
	{
		i--;
	}
	if ( i == -1 )
		return StartIndex;
	return i;
}

int CTextile3DWeave::FindNextYCellIndex( int Starti, int j, int k ) const
{
	int i = Starti + 1;
	while ( i < m_iNumYYarns )
	{
		const vector<PATTERN3D> &Cell = GetCell( i, j );
		if ( Cell[k] != PATTERN3D_NOYARN )
			break;
		i++;
	}
	if ( i < m_iNumYYarns )
		return i;

	if ( Starti == 0 )
		return Starti;
	i = Starti - 1;
	while( i >= 0 )
	{
		const vector<PATTERN3D> &Cell = GetCell( i, j );
		if ( Cell[k] != PATTERN3D_NOYARN )
			break;
		i--;
	}
	if ( i == -1 )
		return Starti;
	return i;
}

int CTextile3DWeave::FindPrevYCellIndex( int Starti, int j, int k ) const
{
	int i = Starti - 1;
	if ( i == -1 )
		i = m_iNumYYarns-1;

	while ( i >= 0 )
	{
		const vector<PATTERN3D> &Cell = GetCell( i, j );
		if ( Cell[k] != PATTERN3D_NOYARN )
			break;
		i--;
	}
	if ( i >= 0 )
		return i;

	if ( Starti == 0 )
		return Starti;

	i = Starti + 1;
	while( i < m_iNumYYarns )
	{
		const vector<PATTERN3D> &Cell = GetCell( i, j );
		if ( Cell[k] != PATTERN3D_NOYARN )
			break;
		i++;
	}
	if ( i == m_iNumYYarns )
		return Starti;
	return i;
}

void CTextile3DWeave::FindMinMaxZ( double &dMinZ, double &dMaxZ )
{
	dMinZ = dMaxZ = 0.0;
	for (int i=0; i<m_iNumXYarns; ++i)
	{
		for (int j=0; j<=m_iNumYYarns; ++j)
		{
			const vector<PATTERN3D> &Cell = GetCell(j%m_iNumYYarns, i);
			int NextCellIndex;
			NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex%m_iNumXYarns );
			double z = 0.0;
			for (int k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_XYARN)
				{
					
					double dHalfHeight = m_XYarnData[i].dHeight / 2.0;
					if ( IsBinderYarn(i) && k != (int)Cell.size()-1 )
					{
						if ( k == 0 )
						{
							z -= dHalfHeight + m_dGapSize;
							if ( (z - dHalfHeight) < dMinZ )
								dMinZ = z - dHalfHeight;
						}
						else 
						{
							if ( m_XYarnData[NextCellIndex].dHeight > m_XYarnData[i].dHeight )
								dHalfHeight = m_XYarnData[NextCellIndex].dHeight / 2.0;
							z += dHalfHeight;
						}
					}
					else
						z += dHalfHeight;
					
					z += dHalfHeight + m_dGapSize;
					
				}
				else if ( Cell[k] == PATTERN3D_YYARN )
				{
					z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
				}
				else if ( k > 0 && k < (int)Cell.size()-1)// PATTERN3D_NOYARN and not on bottom binder layer
				{
					if ( NextCell[k] == PATTERN3D_XYARN )
					{
						z += m_XYarnData[NextCellIndex%m_iNumXYarns].dHeight + m_dGapSize;
					}
					else if ( NextCell[k] == PATTERN3D_YYARN )
					{
						z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
					}
					else // PATTERN3D_NOYARN
					{
						// Will get here if all x yarns are binder yarns so just use binder yarn height to give spacing
						z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
					}
				}
				if ( z > dMaxZ )
					dMaxZ = z;
			}
		}
	}
}

void CTextile3DWeave::SetupLayers( int iNumWarpLayers, int iNumWeftLayers, int iNumBinderLayers )
{
}

int CTextile3DWeave::GetNumBinderLayers() const
{
	return 1;
}

void CTextile3DWeave::ReplaceLastNode( int BinderYarnIndex, XYZ& NewNode, XYZ& BinderNode ) const
{
	int iLastNode = m_Yarns[BinderYarnIndex].GetNumNodes() - 1;
	XYZ LastNode = m_Yarns[BinderYarnIndex].GetNode(iLastNode)->GetPosition();
	XYZ Offset = BinderNode - NewNode;

	LastNode -= Offset;
	m_Yarns[BinderYarnIndex].ReplaceNode( iLastNode, LastNode );
}

CSectionPowerEllipse* CTextile3DWeave::GetWeftCrossSection( int WeftYarnIndex ) const
{
	if (m_Yarns[WeftYarnIndex].GetYarnSection()->GetType() != "CYarnSectionConstant")
			return NULL;
	CYarnSectionConstant* pYarnSection = (CYarnSectionConstant*)m_Yarns[WeftYarnIndex].GetYarnSection()->Copy();
	CSectionPowerEllipse* Section = (CSectionPowerEllipse*)pYarnSection->GetSection().Copy();
	delete pYarnSection;
	return Section;

}

CSection* CTextile3DWeave::GetCrossSection( int YarnIndex, int Node ) const
{
	string Type = m_Yarns[YarnIndex].GetYarnSection()->GetType();
	CSection* pSection;
	if (  Type == "CYarnSectionConstant" )
	{
		CYarnSectionConstant* pYarnSection = (CYarnSectionConstant*)m_Yarns[YarnIndex].GetYarnSection()->Copy();
		pSection = pYarnSection->GetSection().Copy();
		delete pYarnSection;
		return pSection;
	}
	else if ( Type == "CYarnSectionInterpNode" )
	{
		CYarnSectionInterpNode* pYarnSection = (CYarnSectionInterpNode*)m_Yarns[YarnIndex].GetYarnSection()->Copy();
		pSection = pYarnSection->GetNodeSection(Node).Copy();
		delete pYarnSection;
		return pSection;
	}
	else
		return NULL;
}

void CTextile3DWeave::ShapeWeftYarns() const
{
	//PROFILE_FUNC();
	int iNumXYarns = m_iNumXYarns;
	//if ( !m_bWeftRepeat )
	//		iNumXYarns--;
	vector<int> XNodes( iNumXYarns, 0 );
	for ( int i = 0; i < m_iNumYYarns; ++i )
	{
		int CurrentYNode = 0;
		for ( int j = 0; j < iNumXYarns; ++j )
		{
			CurrentYNode = AddWeftNodes( CurrentYNode, XNodes[j], i, j );
			if ( CurrentYNode >= 0 )
				XNodes[j]++;
			CurrentYNode++;
		}
		CheckUpVectors(i, PATTERN3D_YYARN );	
	}
}

int CTextile3DWeave::FindWeftHeight( const vector<PATTERN3D>& Cell ) const
{
	int i = Cell.size() - 1;
	while( i >= 0 )
	{
		if ( Cell[i] == PATTERN3D_YYARN )
			return i;
		--i;
	}
	return i;
}

int CTextile3DWeave::FindWarpAboveIndex( const vector<PATTERN3D> &Cell, int iIndex ) const
{
	iIndex++;
	while ( iIndex < (int)Cell.size() )
	{
		if ( Cell[iIndex] == PATTERN3D_XYARN )
			return iIndex;
		++iIndex;
	}
	return -1;
}

int CTextile3DWeave::FindWarpBelowIndex( const vector<PATTERN3D> &Cell, int iIndex ) const
{
	iIndex--;
	while ( iIndex >= 0 )
	{
		if ( Cell[iIndex] == PATTERN3D_XYARN )
			return iIndex;
		--iIndex;
	}
	return iIndex;
}

int CTextile3DWeave::AddWeftNodes( int CurrentNode, int XNode, int i, int j ) const
{
	const vector<PATTERN3D> &Cell = GetCell(i,j);
	int iIndex = FindWeftHeight( Cell );
	if ( iIndex == -1 )
		return -1;   // Partial weft - no weft yarn at this location

	/*if ( !m_bWeftRepeat && j == m_iNumXYarns - 1 )
	{
		// Insert node at end half x yarn width away
		// Change CurrentNode accordingly
		return CurrentNode;
	}

	if ( !m_bWeftRepeat && j == 0 )  // Also need to catch start of partial weft insertions?
	{
		// Insert node at beginning half x yarn width away
		// Change CurrentNode accordingly
		return CurrentNode;
	}*/
	
	int	NextCellIndex = (j+1)%m_iNumXYarns;
	int	PrevCellIndex = (j+m_iNumXYarns-1)%m_iNumXYarns;

	const vector<PATTERN3D> &NextCell = GetCell( i, NextCellIndex );
	const vector<PATTERN3D> &PrevCell = GetCell( i, PrevCellIndex );
	
	
	int iPrevIndex = FindWeftHeight( PrevCell );
	int iNextIndex = FindWeftHeight( NextCell );
	int iMaxIndex = Cell.size() - 1;

	XY SectionPoint;
	XYZ WarpBelowNode, WarpAboveNode;
	int iStartNode = CurrentNode;

	if ( iIndex < 0 )
		return CurrentNode;

	//double dBinderOffset = m_XYarnData[j].dHeight/2.0;
	double dWeftOffset = m_YYarnData[i].dHeight/2.0;

	int WeftYarnIndex = GetYarnIndex( i, j, iIndex );
	if ( WeftYarnIndex == -1 )  // Can this happen?
		return CurrentNode;

	int WarpAboveIndex, WarpBelowIndex;
	int WarpAboveCellIndex, WarpBelowCellIndex;

	// Get indices of warp yarns above and below weft
	if ( iIndex < (int)Cell.size() - 1 )
	{
		WarpAboveCellIndex = FindWarpAboveIndex( Cell, iIndex );
		if ( WarpAboveCellIndex != -1 )
		{
			WarpAboveIndex = GetYarnIndex( i, j, WarpAboveCellIndex );
			if ( WarpAboveIndex >= 0 )
				WarpAboveNode = m_Yarns[WarpAboveIndex].GetNode( XNode )->GetPosition();
		}
		else 
			WarpAboveIndex = -1;
	}
	else
		WarpAboveIndex = -1;

	if ( iIndex > 0 )
	{
		WarpBelowCellIndex = FindWarpBelowIndex( Cell, iIndex );
		//WarpBelowIndex = GetYarnIndex( i, j, iIndex - 1 );
		if ( WarpBelowCellIndex != -1 )
		{
			WarpBelowIndex = GetYarnIndex( i, j, WarpBelowCellIndex );
			if ( WarpBelowIndex >= 0 )
				WarpBelowNode = m_Yarns[WarpBelowIndex].GetNode( XNode )->GetPosition();
		}
		else
			WarpBelowIndex = -1;
	}
	else
		WarpBelowIndex = -1;

	//XYZ BinderNode = m_Yarns[BinderYarnIndex].GetNode( CurrentNode )->GetPosition();
	XYZ WeftNode = m_Yarns[WeftYarnIndex].GetNode( CurrentNode )->GetPosition();
	
	if ( iIndex > 0 && iPrevIndex != -1 && iPrevIndex < iIndex-1 && PrevCell[iPrevIndex] == PATTERN3D_YYARN   // Check for being more than one layer apart
		&& GetYarnIndex( i,PrevCellIndex, iPrevIndex ) == WeftYarnIndex)
	{
		// Get cross section of warp yarn below
		if ( WarpBelowIndex == -1 )
			return CurrentNode;
		CSection* YarnSection = GetCrossSection( WarpBelowIndex );
		if ( YarnSection == NULL )
			return CurrentNode;

		// Insert points around Top left quadrant of warp yarn
		XYZ NewNode = WarpBelowNode;

		if ( iIndex - iPrevIndex > 2 )
		{
			SectionPoint = YarnSection->GetPoint(0.5);
			NewNode.y = NewNode.y + SectionPoint.x - dWeftOffset - m_dGapSize;
			NewNode.z = NewNode.z + SectionPoint.y; 
			m_Yarns[WeftYarnIndex].InsertNode( NewNode, CurrentNode);
			CurrentNode++;
		}

		InsertWeftNode( YarnSection, 0.45, WarpBelowNode, CurrentNode, WeftYarnIndex, dWeftOffset + m_dGapSize );
		InsertWeftNode( YarnSection, 0.4, WarpBelowNode, CurrentNode, WeftYarnIndex, dWeftOffset + m_dGapSize );
		if( !( iIndex < iMaxIndex && iNextIndex > iIndex && NextCell[iNextIndex] == PATTERN3D_YYARN 
		&& GetYarnIndex( i, NextCellIndex, iNextIndex ) == WeftYarnIndex ) )
		{
			InsertWeftNode( YarnSection, 0.35, WarpBelowNode, CurrentNode, WeftYarnIndex, dWeftOffset + m_dGapSize );
			InsertWeftNode( YarnSection, 0.3, WarpBelowNode, CurrentNode, WeftYarnIndex, dWeftOffset + m_dGapSize );
		}
		
		/*if ( iNextIndex <= iIndex && NextCell[iNextIndex] == PATTERN3D_XYARN
				&& GetYarnIndex( NextCellIndex, j, iNextIndex ) == BinderYarnIndex)
		{
			InsertBinderNode( YarnSection, 0.25, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize, false );
		}*/
		delete YarnSection;
	}
	else if ( iIndex < iMaxIndex && iPrevIndex != -1 && iPrevIndex > iIndex+1 && PrevCell[iPrevIndex] == PATTERN3D_YYARN 
		&& GetYarnIndex( i, PrevCellIndex, iPrevIndex ) == WeftYarnIndex )
	{
		if ( WarpAboveCellIndex != iIndex+1 )  // Warp not directly above so don't need to change weft
			return CurrentNode;
		// Get cross section of warp yarn below
		CSection* YarnSection = GetCrossSection( WarpAboveIndex );
		if ( YarnSection == NULL )
			return CurrentNode;
		// Insert points around lower left quadrant of weft yarn
		XYZ NewNode = WarpAboveNode;

		if ( iPrevIndex - iIndex > 2 )
		{
			SectionPoint = YarnSection->GetPoint(0.5);
			NewNode.y = NewNode.y + SectionPoint.x - dWeftOffset - m_dGapSize;
			NewNode.z = NewNode.z + SectionPoint.y;
			m_Yarns[WeftYarnIndex].InsertNode( NewNode, CurrentNode);
			CurrentNode++;
		}

		NewNode = WarpAboveNode;
		
		InsertWeftNode( YarnSection, 0.55, WarpAboveNode, CurrentNode, WeftYarnIndex, -(dWeftOffset + m_dGapSize) );
		InsertWeftNode( YarnSection, 0.6, WarpAboveNode, CurrentNode, WeftYarnIndex, -(dWeftOffset + m_dGapSize) );
		
		if ( !( iIndex > 0 && iNextIndex < iIndex && NextCell[iNextIndex] == PATTERN3D_YYARN 
			&& GetYarnIndex( i, NextCellIndex, iNextIndex ) == WeftYarnIndex ) )
		{
			InsertWeftNode( YarnSection, 0.65, WarpAboveNode, CurrentNode, WeftYarnIndex, -(dWeftOffset + m_dGapSize) );
			InsertWeftNode( YarnSection, 0.7, WarpAboveNode, CurrentNode, WeftYarnIndex, -(dWeftOffset + m_dGapSize) );
		}
		
		if ( iNextIndex >= iIndex && NextCell[iNextIndex] == PATTERN3D_YYARN
			&& GetYarnIndex( i, NextCellIndex, iNextIndex ) == WeftYarnIndex )
		{
			InsertWeftNode( YarnSection, 0.75, WarpAboveNode, CurrentNode, WeftYarnIndex, -(dWeftOffset + m_dGapSize), false );
		}
		delete YarnSection;
	}

	

	if ( iIndex > 0 && iNextIndex != -1 && iNextIndex < iIndex-1 && NextCell[iNextIndex] == PATTERN3D_YYARN 
		&& GetYarnIndex( i, NextCellIndex, iNextIndex ) == WeftYarnIndex )
	{
		
		// Get cross section of weft yarn below
		CSection* YarnSection = GetCrossSection( WarpBelowIndex );
		if ( YarnSection == NULL )
			return CurrentNode;

		if ( iPrevIndex <= iIndex &&  PrevCell[iPrevIndex] == PATTERN3D_YYARN
			&& GetYarnIndex( i, PrevCellIndex, iPrevIndex ) == WeftYarnIndex )
		{
			InsertWeftNode( YarnSection, 0.25, WarpBelowNode, CurrentNode, WeftYarnIndex, dWeftOffset + m_dGapSize, false );
		}
		// Insert points around upper right quadrant of weft
		
		XYZ NewNode = WarpBelowNode;
		CurrentNode++; // Need to insert after node (ie before next node)
		if ( !(iIndex < iMaxIndex && iPrevIndex > iIndex && PrevCell[iPrevIndex] == PATTERN3D_YYARN 
			&& GetYarnIndex( i, PrevCellIndex, iPrevIndex ) == WeftYarnIndex ) )
		{
			InsertWeftNode( YarnSection, 0.2, WarpBelowNode, CurrentNode, WeftYarnIndex, dWeftOffset + m_dGapSize );
			InsertWeftNode( YarnSection, 0.15, WarpBelowNode, CurrentNode, WeftYarnIndex, dWeftOffset + m_dGapSize );
		}
		InsertWeftNode( YarnSection, 0.1, WarpBelowNode, CurrentNode, WeftYarnIndex, dWeftOffset + m_dGapSize );
		InsertWeftNode( YarnSection, 0.05, WarpBelowNode, CurrentNode, WeftYarnIndex, dWeftOffset + m_dGapSize );
		CurrentNode--;
		
		if ( iIndex - iNextIndex > 2 )
		{
			SectionPoint = YarnSection->GetPoint(0);
			NewNode = WarpBelowNode;
			NewNode.y = NewNode.y + SectionPoint.x + dWeftOffset + m_dGapSize;
			NewNode.z = NewNode.z + SectionPoint.y;
			m_Yarns[WeftYarnIndex].InsertNode( NewNode, CurrentNode+1 );
			CurrentNode++;
		}
		
		delete YarnSection;
	}
	else if( iIndex < iMaxIndex && iNextIndex != -1 && iNextIndex > iIndex+1 && NextCell[iNextIndex] == PATTERN3D_YYARN 
		&& GetYarnIndex( i, NextCellIndex, iNextIndex ) == WeftYarnIndex )
	{
		if ( WarpAboveCellIndex != iIndex+1 )  // Warp not directly above so don't need to change weft
			return CurrentNode;

		// Get cross section of weft yarn below
		CSection* YarnSection = GetCrossSection( WarpAboveIndex );
		if ( YarnSection == NULL )
			return CurrentNode;

		if ( iPrevIndex >= iIndex &&  PrevCell[iPrevIndex] == PATTERN3D_YYARN
			&& GetYarnIndex( i, PrevCellIndex, iPrevIndex ) == WeftYarnIndex )
		{
			InsertWeftNode( YarnSection, 0.75, WarpAboveNode, CurrentNode, WeftYarnIndex, -(dWeftOffset + m_dGapSize), false );
		}
		// Insert points around lower right quadrant of weft yarn
		{
			XYZ NewNode = WarpAboveNode;
			CurrentNode++;  // Need to insert after node (ie before next node)
			if ( !(iIndex > 0 && iPrevIndex < iIndex && PrevCell[iPrevIndex] == PATTERN3D_YYARN 
				&& GetYarnIndex( i, PrevCellIndex, iPrevIndex ) == WeftYarnIndex) )
			{
				InsertWeftNode( YarnSection, 0.8, WarpAboveNode, CurrentNode, WeftYarnIndex, -(dWeftOffset + m_dGapSize) );
				InsertWeftNode( YarnSection, 0.85, WarpAboveNode, CurrentNode, WeftYarnIndex, -(dWeftOffset + m_dGapSize) );
			}
			InsertWeftNode( YarnSection, 0.9, WarpAboveNode, CurrentNode, WeftYarnIndex, -(dWeftOffset + m_dGapSize) );
			InsertWeftNode( YarnSection, 0.95, WarpAboveNode, CurrentNode, WeftYarnIndex, -(dWeftOffset + m_dGapSize) );
			CurrentNode--;

			if ( iNextIndex - iIndex > 2 )
			{
				SectionPoint = YarnSection->GetPoint(0);
				NewNode = WarpAboveNode;
				NewNode.y = NewNode.y + SectionPoint.x + dWeftOffset + m_dGapSize;
				NewNode.z = NewNode.z + SectionPoint.y;
				m_Yarns[WeftYarnIndex].InsertNode( NewNode, CurrentNode+1 );
				CurrentNode++;
			}
		}
		
		delete YarnSection;
	}

	if ( iStartNode == 0 && m_bWeftRepeat )
	{
		XYZ NewNode = m_Yarns[WeftYarnIndex].GetNode(0)->GetPosition();
		ReplaceLastNode( WeftYarnIndex, NewNode, WeftNode );
	}
	
	return CurrentNode;
}

void CTextile3DWeave::CheckUpVectors( int Index, bool bYarn, bool bYarnsIndex ) const
{
	BuildTextileIfNeeded();
	int YarnIndex = 0;
	if ( bYarnsIndex )
	{
		if ( Index > m_Yarns.size()-1 )
			return;
		YarnIndex = Index;
	}
	else
	{
		if ( bYarn == PATTERN3D_XYARN )
		{
			if ( Index > m_iNumXYarns-1 )
				return;
			YarnIndex = m_XYarns[Index][0];
		}
		else
		{
			if ( Index > m_iNumYYarns-1 )
				return;
			YarnIndex = m_YYarns[Index][0];
		}
	}
	
	vector<CNode> Nodes = m_Yarns[YarnIndex].GetMasterNodes();
	vector<CNode>::iterator itNodes;

	XYZ CheckNodes[3];
	XYZ UpVector, StartUp;
	int size = Nodes.size();

	
	for ( int i = 0; i < size-1; ++i)
	{
		CheckNodes[1] = Nodes[i].GetPosition();
		if ( i == 0 )
		{
			CheckNodes[0] = CheckNodes[1];
			CheckNodes[2] = Nodes[i+1].GetPosition();
		}
		else
		{
			CheckNodes[2] = Nodes[i+1].GetPosition();
		}
		UpVector = GetUpVector( CheckNodes, bYarn );
		Nodes[i].SetUp( UpVector );
		if ( i == 0 )
			Nodes[size-1].SetUp(UpVector);
		CheckNodes[0] = CheckNodes[1];
	}
	m_Yarns[YarnIndex].SetNodes( Nodes );
}

XYZ CTextile3DWeave::GetUpVector( XYZ CheckNodes[], bool bYarn ) const
{
	double dHorz, dz;
	dz = CheckNodes[2].z - CheckNodes[0].z;

	if ( bYarn == PATTERN3D_XYARN )
		dHorz = CheckNodes[2].x - CheckNodes[0].x;
	else
		dHorz = CheckNodes[2].y - CheckNodes[0].y;
	XYZ UpVector;

	if ( fabs(dz) > fabs(dHorz) )
	{
		if ( dz >= 0.0 )
		{
			if ( bYarn == PATTERN3D_XYARN )
				UpVector.x = -1;
			else
				UpVector.y = -1;
		}
		else 
		{
			if ( bYarn == PATTERN3D_XYARN )
				UpVector.x = 1;	
			else
				UpVector.y = 1;

		}
	}
	else
	{
		if ( dHorz >= 0.0 )
		{
			UpVector.z = 1;
		}
		else
		{
			UpVector.z = -1;
		}
	}
    return UpVector;        
}

void CTextile3DWeave::InsertBinderNode( CSectionPowerEllipse* YarnSection, double t, XYZ& WeftNode, int& CurrentNode, int BinderYarnIndex, double Offset, bool bInsert ) const
{
	XY SectionPoint = YarnSection->GetPoint(t);
	XYZ NewNode = WeftNode;

	NewNode.x = NewNode.x + SectionPoint.x;
	NewNode.z = NewNode.z + SectionPoint.y + Offset; 
	if ( bInsert )
	{
		m_Yarns[BinderYarnIndex].InsertNode( NewNode, CurrentNode);
		CurrentNode++;
	}
	else
		m_Yarns[BinderYarnIndex].ReplaceNode( CurrentNode, NewNode );
	
}

void CTextile3DWeave::InsertBinderNode( CSection* YarnSection, double t, XYZ& WeftNode, int& CurrentNode, int BinderYarnIndex, double Offset, bool bInsert ) const
{
	string Type = YarnSection->GetType();
	XY SectionPoint;
	if ( Type == "CSectionPowerEllipse" )
		SectionPoint = ((CSectionPowerEllipse*)YarnSection)->GetPoint(t);
	else
		SectionPoint = ((CSectionHybrid*)YarnSection)->GetPoint(t);
	XYZ NewNode = WeftNode;

	NewNode.x = NewNode.x + SectionPoint.x;
	NewNode.z = NewNode.z + SectionPoint.y + Offset; 
	if ( bInsert )
	{
		m_Yarns[BinderYarnIndex].InsertNode( NewNode, CurrentNode);
		CurrentNode++;
	}
	else
		m_Yarns[BinderYarnIndex].ReplaceNode( CurrentNode, NewNode );
	
}

void CTextile3DWeave::InsertWeftNode( CSection* YarnSection, double t, XYZ& WarpNode, int& CurrentNode, int WeftYarnIndex, double Offset, bool bInsert ) const
{
	string Type = YarnSection->GetType();
	XY SectionPoint;
	if ( Type == "CSectionPowerEllipse" )
		SectionPoint = ((CSectionPowerEllipse*)YarnSection)->GetPoint(t);
	else
		SectionPoint = ((CSectionHybrid*)YarnSection)->GetPoint(t);
	XYZ NewNode = WarpNode;

	NewNode.y = NewNode.y + SectionPoint.x;
	NewNode.z = NewNode.z + SectionPoint.y + Offset; 
	if ( bInsert )
	{
		m_Yarns[WeftYarnIndex].InsertNode( NewNode, CurrentNode);
		CurrentNode++;
	}
	else
		m_Yarns[WeftYarnIndex].ReplaceNode( CurrentNode, NewNode );
	
}

bool CTextile3DWeave::AdjustPowerEllipseSectionWidth( double &HeightReduction, double &MaxWidth, double TargetArea, CSectionPowerEllipse* YarnSection ) const
{
	double StartHeight = YarnSection->GetHeight();
	double TargetHeight = StartHeight * HeightReduction;
	double NewWidth;
	if ( fabs( 1 - HeightReduction ) > TOL )
		NewWidth = YarnSection->GetWidth() * 1.1 / HeightReduction;
	else
		NewWidth = YarnSection->GetWidth() * 1.1;  

	if ( NewWidth > MaxWidth )
		NewWidth = MaxWidth;

	double dWidth = NewWidth - YarnSection->GetWidth();

	CSectionPowerEllipse NewSection( NewWidth, TargetHeight, YarnSection->GetPower() );

	if ( fabs(dWidth) < TOL )
	{
		*YarnSection = NewSection;
		return false;
	}

	

	double Area = NewSection.GetArea( NewSection.GetPoints(40));
	bool bFoundRange = false;

	while( fabs( Area - TargetArea ) > TOL )
	{
		if ( Area > TargetArea )
		{
			dWidth /= 2.0;
			NewWidth -= dWidth;
			bFoundRange = true;
		}
		else if ( Area < TargetArea )
		{
			if ( NewWidth > MaxWidth )
			{
				CSectionPowerEllipse Section( MaxWidth, TargetHeight, YarnSection->GetPower() );
				*YarnSection = Section;
			/*	if ( HeightReduction == 1.0 )
					StartHeight *= 1.5;
				FindPowerEllipseSectionHeight( StartHeight, TargetArea, YarnSection );*/
				return false;
			}
			if ( bFoundRange )
				dWidth /= 2;
			NewWidth += dWidth;
		}
		
		CSectionPowerEllipse Section( NewWidth, TargetHeight, YarnSection->GetPower() );
		Area = Section.GetArea( Section.GetPoints(40));
	}
	if ( NewWidth > MaxWidth )
	{
		NewSection.SetWidth( MaxWidth );
		*YarnSection = NewSection;
		/*if ( HeightReduction == 1.0 )
			StartHeight *= 1.5;
		FindPowerEllipseSectionHeight( StartHeight, TargetArea, YarnSection );*/
		return false;
	}
	CSectionPowerEllipse Section( NewWidth, TargetHeight, YarnSection->GetPower() );
	*YarnSection = Section;
	return true;
}

void CTextile3DWeave::FindPowerEllipseSectionHeight( double& MaxHeight, double& TargetArea, CSectionPowerEllipse* YarnSection ) const
{
	double NewHeight = MaxHeight;
	double StartHeight = NewHeight;
	double dHeight = StartHeight - YarnSection->GetHeight();

	string Type = YarnSection->GetType();

	CSectionPowerEllipse NewSection( YarnSection->GetWidth(), StartHeight, YarnSection->GetPower() );
	double Area = NewSection.GetArea( NewSection.GetPoints(40));

	if ( Area < TargetArea ) // Max available area less than target area so return section with max height
	{
		*YarnSection = NewSection;
		return;
	}

	while( fabs( Area - TargetArea ) > TOL )
	{
		if ( Area > TargetArea )
		{
			dHeight /= 2.0;
			NewHeight -= dHeight;
		}
		else if ( Area < TargetArea )
		{
			dHeight /= 2;
			NewHeight += dHeight;
		}
		
		CSectionPowerEllipse Section( YarnSection->GetWidth(), NewHeight, YarnSection->GetPower() );
		Area = Section.GetArea( Section.GetPoints(40));
	}
	
	CSectionPowerEllipse Section( YarnSection->GetWidth(), NewHeight, YarnSection->GetPower() );
	*YarnSection = Section;
}

bool CTextile3DWeave::AdjustPowerEllipsePower( double &HeightReduction, double TargetArea, CSectionPowerEllipse* YarnSection, double MinPower ) const
{
	double MaxPower = YarnSection->GetPower();
	double TargetHeight = YarnSection->GetHeight() * HeightReduction;
	double dWidth = YarnSection->GetWidth();

	CSectionPowerEllipse MinPowerSection( dWidth, TargetHeight, MinPower );

	double Area = MinPowerSection.GetArea( MinPowerSection.GetPoints(40));
	if ( Area < TargetArea )
		return false;  // Can't achieve target area by just increasing power

	double MidPower = ( MaxPower + MinPower )/2.0;
	CSectionPowerEllipse MidSection( dWidth, TargetHeight, MidPower );
	double MidArea = MidSection.GetArea( MidSection.GetPoints(40));
	
	while ( fabs( MidArea - TargetArea ) > TOL )
	{
		if ( MidArea > TargetArea )
		{
			MinPower = MidPower;
		}
		else
		{
			MaxPower = MidPower;
		}
		MidPower = ( MinPower + MaxPower ) / 2.0;
		CSectionPowerEllipse Section( dWidth, TargetHeight, MidPower );  // Need to create new section rather than just setting power as doesn't recalculate section points
		MidArea = Section.GetArea( Section.GetPoints(40));
	}
	CSectionPowerEllipse Section( dWidth, TargetHeight, MidPower );
	*YarnSection = Section;
	return true;
}

void CTextile3DWeave::ReducePowerEllipseHeight( double& TargetArea, CSectionPowerEllipse* YarnSection ) const
{
	double Area = YarnSection->GetArea( YarnSection->GetPoints(40) );
	if ( Area <= TargetArea )
		return;

	double MaxHeight = YarnSection->GetHeight();
	double MinHeight = YarnSection->GetHeight() / 2.0;
	double Width = YarnSection->GetWidth();
	double MidHeight = ( MaxHeight + MinHeight )/2.0;
	double Power = YarnSection->GetPower();

	CSectionPowerEllipse MidSection( Width, MidHeight, Power );
	double MidArea = MidSection.GetArea( MidSection.GetPoints(40));

	while ( fabs( MidArea - TargetArea ) > TOL )
	{
		if ( MidArea > TargetArea )
		{
			MaxHeight = MidHeight;
		}
		else
		{
			MinHeight = MidHeight;
		}
		MidHeight = ( MinHeight + MaxHeight ) / 2.0;
		CSectionPowerEllipse Section( Width, MidHeight, Power );
		MidArea = Section.GetArea( Section.GetPoints(40));
	}
	CSectionPowerEllipse Section( Width, MidHeight, Power );
	*YarnSection = Section;
}

void CTextile3DWeave::SetYarnProperties( CYarn& Yarn, int iType ) const
{
	// Only want to set values if actually input otherwise sets Property m_bSet to true & returns zeros for area
	if ( m_Properties[iType].GetFibresPerYarn() )
		Yarn.SetFibresPerYarn( m_Properties[iType].GetFibresPerYarn() );
	if ( m_Properties[iType].GetFibreDiameter() != 0.0 )
		Yarn.SetFibreDiameter( m_Properties[iType].GetFibreDiameter());
	if ( m_Properties[iType].GetYarnLinearDensity() != 0.0 )
		Yarn.SetYarnLinearDensity( m_Properties[iType].GetYarnLinearDensity());
	if ( m_Properties[iType].GetFibreDensity() != 0.0 )
		Yarn.SetFibreDensity( m_Properties[iType].GetFibreDensity());
	if ( m_Properties[iType].GetFibreArea() != 0.0 )
		Yarn.SetFibreArea( m_Properties[iType].GetFibreArea());
}

void CTextile3DWeave::GetWarpYarnInfo( int& iNumBinderYarns, int& iNumWarpYarns, int& iBinderRatio, int& iWarpRatio, YARNDATA& AveWarp, YARNDATA& AveBinder ) const
{
	double WarpWidth = 0.0;
	double BinderWidth = 0.0;
	double WarpHeight = 0.0;
	double BinderHeight = 0.0;
	iNumWarpYarns = 0;
	iNumBinderYarns = 0;
	iBinderRatio = 0;
	iWarpRatio = 0;
	bool bRatioFound = false;
	for ( int i=0; i < m_iNumXYarns; ++i )
	{
		if ( !IsBinderYarn(i) )
		{
			iNumWarpYarns++;
			WarpWidth += m_XYarnData[i].dWidth;
			WarpHeight += m_XYarnData[i].dHeight;
			if ( iBinderRatio == 0 ) // First set of warps so count up ratio
			{
				iWarpRatio++;
			}
			else 
			{
				bRatioFound = true;
			}
		}
		else
		{
			iNumBinderYarns++;
			BinderWidth += m_XYarnData[i].dWidth;
			BinderHeight += m_XYarnData[i].dHeight;
			if ( !bRatioFound ) // First set of binders so count up ratio
			{
				iBinderRatio++;
			}
		}
	}

	if ( iNumWarpYarns )
	{
		AveWarp.dWidth = WarpWidth / iNumWarpYarns;
		AveWarp.dHeight = WarpHeight / iNumWarpYarns;
	}
	else // all binder yarns
	{
		AveWarp.dHeight = AveWarp.dWidth = 0.0;
	}

	AveBinder.dWidth = BinderWidth / iNumBinderYarns;
	AveBinder.dHeight = BinderHeight / iNumBinderYarns;
	
	double AveGap = ( GetHeight() - (WarpWidth+BinderWidth) ) / m_iNumXYarns;
	AveBinder.dSpacing = AveBinder.dWidth + AveGap;
	AveWarp.dSpacing = AveWarp.dWidth + AveGap;

	iNumWarpYarns += iNumBinderYarns;  // NumWarpYarns is total number of yarns in warp direction
}

void CTextile3DWeave::SetupWeftRow( vector<int>& Layers, vector<int>& Warp, int NumWarps, int Weft )
{
	int NumLayers = *(max_element( Layers.begin(), Layers.end() )) + 1;
	m_iTotalXYarns = Layers.size();
	vector<int>::iterator itWarp;
	vector<int>::iterator itLayers;

	if ( Layers.size() != Warp.size() )
		TGERROR("Can't add weft row, warp and layer sizes do not match");

	itWarp = Warp.begin();
	int j = NumWarps-1;
	int PrevYCellIndex = -1;
	// Left hand side of weave pattern corresponds to maximum y value (or max j in cell array)
	for ( itLayers = Layers.begin(); itLayers != Layers.end(); )
	{
		vector<PATTERN3D> &Cell = GetCell(Weft, j);
		bool bFirst = true;  // bFirst is true until the weft yarn has been set for the current cell
		int WarpIndex = -1;
		do
		{
			if ( *itWarp == 0 && bFirst )  // Warp yarn is down and weft not yet set
			{
				// Set the weft yarn (y yarn) at the cell index above the current layer
				if ( PrevYCellIndex != -1 && WarpIndex != -1 && PrevYCellIndex >= WarpIndex-1 )
				{
					Cell[WarpIndex-1] = PATTERN3D_YYARN;
					PrevYCellIndex = WarpIndex-1;
				}
				else
				{
					Cell[NumLayers - (*itLayers)] = PATTERN3D_YYARN;
					PrevYCellIndex = NumLayers - (*itLayers);
				}
				// Set the cell for the warp yarn
				Cell[NumLayers - (*itLayers)-1] = PATTERN3D_XYARN;
				bFirst = false;
			}
			else if ( *itWarp != PATTERN3D_NOYARN )
			{
				if ( !bFirst )  // Warp is up so set the x yarn cell allowing for one cell having been taken by weft yarn
					Cell[NumLayers - (*itLayers)-1] = PATTERN3D_XYARN;
				else
				{   // Warp is up and no weft yarn yet. Set x yarn cell without space allowed for weft yarn
					WarpIndex = NumLayers - (*itLayers);
					Cell[WarpIndex] = PATTERN3D_XYARN;
				}
			}
			
			++itLayers;
			++itWarp;
		} while ( itLayers != Layers.end() && *itLayers != 1 );  // Contunue until reach end of current stack of layers
		if ( bFirst && *(itWarp-1) != PATTERN3D_NOYARN )  // If reached end of stack without setting weft yarn need to set it at bottom of cell entries
		{
			Cell[NumLayers - *(itLayers-1)-1] = PATTERN3D_YYARN;
			PrevYCellIndex = NumLayers - *(itLayers-1)-1;
		}
		--j; // Move to next cell

	}
	m_bNeedsBuilding = true;
}

void CTextile3DWeave::ConvertToPatternDraft( int iWeftOrder )
{
	m_PatternDraft.ClearWeavePattern();
	for ( int i = m_iNumYYarns-1; i >=0; --i )
	{
		string Row;
		for ( int j = m_iNumXYarns-1; j >= 0 ; --j ) // Row of cells in y direction correspond to one row of weave pattern. lhs of weave pattern at max j
		{
			vector<PATTERN3D> &Cell = GetCell(i, j);
			if ( NoYarnCell( Cell ) )
				continue;
			bool bWeftSet = false;
			for ( int k = Cell.size()-1; k > 0; --k )  // Top of cell at lhs of weave pattern
			{
				if ( !( k == 1 && Cell[0] == PATTERN3D_NOYARN) )
				{
					switch (Cell[k])
					{
					case PATTERN3D_XYARN:
						if ( !bWeftSet )
							Row.push_back('1');
						else
							Row.push_back('0');   // Assumes only one weft.  Anything after the weft must be a warp down
						break;
					case PATTERN3D_YYARN:
						Row.push_back('0');
						bWeftSet = true;
						break;
					case PATTERN3D_NOYARN:
						//Row.push_back('2');
						break;
					}
				}
			}
		}
		if ( (int)Row.size() < m_iTotalXYarns )
		{
			for ( int i = (int)Row.size()+1; i <= m_iTotalXYarns; ++i )
			{
				Row.push_back('2');
			}
		}
		m_PatternDraft.AddRow( Row );
	}
}

bool CTextile3DWeave::NoYarnCell( vector<PATTERN3D> &Cell )
{
	for ( int i = 0; i < Cell.size(); ++i )
	{
		if ( Cell[i] != PATTERN3D_NOYARN )
			return false;
	}
	return true;
}

void CTextile3DWeave::CalculateReedDesign( string Filename )
{
	vector<int> LayerInfo;
	double dReedWidth = 0;  // Calculate total width based on spacings. At later date could use individual spacings to get distribution of ends in dents
	double dMaxWarpWidth = 0.0;
	for ( int j = m_iNumXYarns-1; j >= 0 ; --j )
	{
		int Layers = 0;
		vector<PATTERN3D> &Cell = GetCell(0, j);
		for ( int k = Cell.size()-1; k >= 0; --k )
		{
			if ( Cell[k] == PATTERN3D_XYARN )
				++Layers;
		}
		LayerInfo.push_back( Layers );
		dReedWidth += m_XYarnData[j].dSpacing;
		if ( m_XYarnData[j].dWidth > dMaxWarpWidth )
			dMaxWarpWidth = m_XYarnData[j].dWidth;
	}
	double dReedDensity = (m_iNumXYarns / dReedWidth);  // Reed density in ends/mm
	double dDentWidth = 1.0/dReedDensity;
	double dToothSize = 0.2 * dDentWidth;
	
	CReedData ReedData;
	dToothSize = ReedData.GetClosestReedThickness( dToothSize );
	// Need to add check that gap left (dent width - tooth thickness) is ok for yarn width
	double dDentGap = dDentWidth - dToothSize;
	if ( dMaxWarpWidth/3.0 > dDentGap )
	{
		TGERROR( "Yarn width greater than dent gap. Compressed yarn width = " << dMaxWarpWidth << ", Dent gap = " << dDentGap );
	}

	double dWeftWidth = 0;
	for ( int i = 0; i < m_iNumYYarns; ++i )
	{
		dWeftWidth += m_YYarnData[i].dSpacing;
	}
	double dWeftDensity = (m_iNumYYarns / dWeftWidth) * 10.0;  // Wefts/cm

	AddExtensionIfMissing(Filename, ".txt");
	ofstream Output(Filename.c_str());

	Output << "Reed width: " << dReedWidth << "mm" << endl;
	Output << "Reed density: " << dReedDensity * 10.0 << " dents/cm" << endl;
	Output << "Tooth thickness: " << dToothSize << "mm" << endl;
	Output << "Weft density: " << dWeftDensity << " wefts/cm" << endl;
	Output << "Number of used dents: " << m_iNumXYarns << endl;
	Output << endl;
	Output << "Dent      Number of ends" << endl; 
	
	for ( int i = 0; i < LayerInfo.size(); ++i )
	{
		Output.width(10);
		Output << std::left << i+1;
		Output.width(10);
		Output << std::left << LayerInfo[i] << endl;
	}
}

bool CTextile3DWeave::CheckCells() const
{
	for (int i=0; i<m_iNumXYarns; ++i)
	{
		for (int j=0; j<m_iNumYYarns; ++j)
		{
			const vector<PATTERN3D> &Cell = GetCell(j, i);
			if ( Cell.size() == 0 )
				return false;
		}
	}
	return true;
}