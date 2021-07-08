/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2011 Louise Brown

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
#include "TextileOrthogonal.h"
#include "SectionHybrid.h"
#include "SectionPolygon.h"
#include "SectionRectangle.h"
#include "PatternDraft.h"
//#define SHINY_PROFILER 0

//#include "Shiny.h"

# define TOL 0.000001
# define MAX_VOLUME_FRACTION	0.87 //0.78
# define TOP	1
# define BOTTOM 0

using namespace TexGen;

CTextileOrthogonal::CTextileOrthogonal(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing, double dXHeight, double dYHeight, bool bRefine, bool bWeavePattern)
: CTextile3DWeave( iNumXYarns, iNumYYarns, dXSpacing, dYSpacing, dXHeight, dYHeight, bRefine)
{
	m_bWeavePattern = bWeavePattern;
	m_WeftYarns.resize( iNumXYarns * iNumYYarns );
}

CTextileOrthogonal::CTextileOrthogonal(TiXmlElement &Element)
: CTextile3DWeave(Element)
{
}

CTextileOrthogonal::~CTextileOrthogonal()
{
}

void CTextileOrthogonal::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextile3DWeave::PopulateTiXmlElement(Element, OutputType);
}

void CTextileOrthogonal::SwapBinderPosition( int x, int y )
{
	if (x<0 || x>=m_iNumYYarns || y<0 || y>=m_iNumXYarns)
	{
		TGERROR("Unable to swap position, index out of range: " << x << ", " << y);
		return;
	}
	vector<PATTERN3D> &Cell = GetCell(x, y);
	int size = Cell.size();
	
	// Binder yarns are at either top or bottom of cell
	int bTemp;
	bTemp = Cell[0];
	Cell[0] = Cell[size-1];
	Cell[size-1] = bTemp;
	m_bNeedsBuilding = true;
}

void CTextileOrthogonal::SetupLayers( int iNumWarpLayers, int iNumWeftLayers, int iNumBinderLayers )
{
	AddNoYarnLayer();

	// Add alternating layers
	while( iNumWeftLayers > 1 )
	{
		AddYLayers();
		if ( iNumWarpLayers > 0 )
		{
			AddWarpLayer();
			iNumWarpLayers--;
		}
		iNumWeftLayers--;
	}
	// If more warp than weft layers, add remaining layers
	while( iNumWarpLayers > 0 )
	{
		AddWarpLayer();
		iNumWarpLayers--;
	}
	// Must have weft layer next to binders
	AddYLayers();

	AddBinderLayer();
}

bool CTextileOrthogonal::BuildTextile() const
{
	if ( m_bWeavePattern )
	{
		return BuildWeavePatternTextile();
	}

	// x yarns are parallel to the x axis - warp and binder yarns
	// y yarns are parallel to the y axis - weft yarns

	//PROFILE_BEGIN(Begin);
	m_Yarns.clear();
	m_YYarns.clear();
	m_XYarns.clear();

	m_YYarns.resize(m_iNumYYarns);
	m_XYarns.resize(m_iNumXYarns);

	m_dMinZ = 0.0;
	m_dMaxZ = 0.0;

	/*if (!Valid())
		return false;*/
	bool bSuccess = true;

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
		// For each stack of x yarns add a node at the crossover with each y yarn
		// Continue until j == m_iNumYYarns so that get start and end nodes the same
		for (j=0; j<=m_iNumYYarns; ++j)
		{
			// Get the cell for the 
			const vector<PATTERN3D> &Cell = GetCell(j%m_iNumYYarns, i);
			int NextCellIndex;
			NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex );

			const vector<PATTERN3D> &NextYCell = GetCell((j+1)%m_iNumYYarns, i );
			// Create the yarns
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
			// Increment x coordinate to position of next y yarn
			x += m_YYarnData[j%m_iNumYYarns].dSpacing/2.0;
			
			// Work upwards through cell: Cell[0] and Cell[max] are always binder yarn entries
			z = 0.0;
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_XYARN) // x yarn so add node
				{
					double dHalfHeight = m_XYarnData[i].dHeight / 2.0;
					if ( k == 0 && IsBinderYarn(i) )
					{
						z -= dHalfHeight + m_dGapSize;
						if ( (z - dHalfHeight) < m_dMinZ )
							m_dMinZ = z - dHalfHeight;
					}
					else
						z += dHalfHeight;
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(1, 0, 0)));
					++iYarn;
					if ( k == (int)Cell.size()-1 && IsBinderYarn(i) ) // Don't need to add gap above top binder yarn
						z += dHalfHeight;
					else
						z += dHalfHeight + m_dGapSize;
					if ( z > m_dMaxZ )
						m_dMaxZ = z;
				}
				else if ( Cell[k] == PATTERN3D_YYARN ) // y yarn so add y yarn height
				{
					z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
				}
				else if ( k > 0 )// PATTERN3D_NOYARN and not on bottom binder layer so add height of adjacent yarn
				{
					if ( NextCell[k] == PATTERN3D_XYARN )
					{
						z += m_XYarnData[NextCellIndex].dHeight + m_dGapSize;
					}
					else if ( NextCell[k] == PATTERN3D_YYARN )
					{
						z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
					}
					else // PATTERN3D_NOYARN
					{
						if ( NextYCell[k] == PATTERN3D_YYARN )
						{
							z += m_YYarnData[(j+1)%m_iNumYYarns].dHeight + m_dGapSize;
						}
						else if ( NextYCell[k] == PATTERN3D_XYARN )
						{
							z += m_XYarnData[i].dHeight + m_dGapSize;
						}
						//else
						//{
							// Does this ever happen!
						//}
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

		// For each stack of y yarns add a node at the crossover with each x yarn
		// Continue until j == m_iNumXYarns so that get start and end nodes the same
		for (i=0; i<=m_iNumXYarns; ++i)
		{
			const vector<PATTERN3D> &Cell = GetCell(j, i%m_iNumXYarns);
			
			int NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex%m_iNumXYarns);

			const vector<PATTERN3D> &NextYCell = GetCell((j+1)%m_iNumYYarns, i%m_iNumXYarns );
			// Create the yarns
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
			// Increment y coordinate to position of next x yarn
			y += m_XYarnData[i%m_iNumXYarns].dSpacing/2.0;
			z = 0.0;
			// Work upwards through cell
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_YYARN) // y yarn so add node
				{
					double dHalfHeight = m_YYarnData[j].dHeight / 2.0;
					z += dHalfHeight;
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(0, 1, 0)));
					++iYarn;
					z += dHalfHeight + m_dGapSize;
				}
				else if ( Cell[k] == PATTERN3D_XYARN && k > 0 ) // Don't adjust z if it's the binder yarn
				{
					z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
				}
				else if ( k > 0 ) // PATTERN3D_NOYARN and not on bottom binder layer
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
						if ( NextYCell[k] == PATTERN3D_YYARN )
						{
							z += m_YYarnData[(j+1)%m_iNumYYarns].dHeight + m_dGapSize;
						}
						else if ( NextYCell[k] == PATTERN3D_XYARN )
						{
							z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
						}
						//else
						//{
							// Does this ever happen!
						//}
					}
				}
			}
			if (i<m_iNumXYarns)
				y += m_XYarnData[i].dSpacing/2.0;
		}
		x += m_YYarnData[j].dSpacing/2.0;
	}

	// Assign sections and properties to the yarns
	vector<int>::iterator itpYarn;
	double dWidth, dHeight;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		dWidth = m_XYarnData[i].dWidth;
		dHeight = m_XYarnData[i].dHeight;
		CSectionPowerEllipse Section(dWidth, dHeight, IsBinderYarn(i) ? m_dBinderPower : m_dWarpPower );
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		
		for (itpYarn = m_XYarns[i].begin(); itpYarn != m_XYarns[i].end(); ++itpYarn)
		{
			// If refining will be changing the sections at each node so use CYarnSectionInterpNode
			if ( m_bRefine /*&& !IsBinderYarn(i)*/) 
			{
				CYarnSectionInterpNode YarnSections;
				for ( j = 0; j <= m_iNumYYarns; ++j )
				{ 
					YarnSections.AddSection( Section );
				}
				m_Yarns[*itpYarn].AssignSection( YarnSections );
			}
			else  // otherwise constant section along yarn
			{
				m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
			}

			int iType = IsBinderYarn(i) ? BINDER : WARP;
			SetYarnProperties( m_Yarns[*itpYarn], iType );
		}
	}
	for (i=0; i<m_iNumYYarns; ++i)
	{
		dWidth = m_YYarnData[i].dWidth;
		dHeight = m_YYarnData[i].dHeight;
		CSectionPowerEllipse Section(dWidth, dHeight, m_dWeftPower );
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		
		for (itpYarn = m_YYarns[i].begin(), j=0; itpYarn != m_YYarns[i].end(); ++itpYarn, ++j)
		{
			if ( m_bRefine ) 
			{
				CYarnSectionInterpNode YarnSections;
				for ( k = 0; k <= m_iNumXYarns; ++k )
				{
					YarnSections.AddSection( Section );
				}
				m_Yarns[*itpYarn].AssignSection( YarnSections );
			}
			else
			{
				m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
			}
			SetYarnProperties( m_Yarns[*itpYarn], WEFT );
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

	// Refine yarns
	if ( m_bRefine && m_dFabricThickness > 0.0 )
	{
		CheckBinderWidths();
		
		if ( !AdjustMidLayerHeights() )  // Try to change total thickness of layers, excluding top & bottom weft layers
		{						  // May not achieve if can't stay within maximum Vf without exceeding yarn spacing
			bSuccess = false;
		}
		else
		{
			AdjustOuterWeftYarns();   // Change outer weft yarns to hybrid section. 
			AdjustBinderYarns();
		}
	}

	ShapeBinderYarns();  // Add nodes to binder yarns to follow profile of top & bottom weft yarns

	

	//PROFILE_END();
	//PROFILER_UPDATE();
	//PROFILER_OUTPUT("ProfileOutput.txt");
	return bSuccess;
}

bool CTextileOrthogonal::BuildWeavePatternTextile() const
{
	// x yarns are parallel to the x axis - warp and binder yarns
	// y yarns are parallel to the y axis - weft yarns

	//PROFILE_BEGIN(Begin);
	m_Yarns.clear();
	m_YYarns.clear();
	m_XYarns.clear();

	m_YYarns.resize(m_iNumYYarns);
	m_XYarns.resize(m_iNumXYarns);

	m_dMinZ = 0.0;
	m_dMaxZ = 0.0;

	/*if (!Valid())
		return false;*/
	bool bSuccess = true;

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
		// For each stack of x yarns add a node at the crossover with each y yarn
		// Continue until j == m_iNumYYarns so that get start and end nodes the same
		for (j=0; j<=m_iNumYYarns; ++j)
		{
			// Get the cell for the 
			const vector<PATTERN3D> &Cell = GetCell(j%m_iNumYYarns, i);
			int NextCellIndex;
			NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex );

			
			// Create the yarns
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
			// Increment x coordinate to position of next y yarn
			x += m_YYarnData[j%m_iNumYYarns].dSpacing/2.0;
			
			// Work upwards through cell: Cell[0] and Cell[max] are always binder yarn entries 
			z = 0.0;
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_XYARN) // x yarn so add node
				{
					double dHalfHeight = m_XYarnData[i].dHeight / 2.0;
					if ( k == 0 && IsBinderYarn(i) )
					{
						z -= dHalfHeight + m_dGapSize;
						if ( (z - dHalfHeight) < m_dMinZ )
							m_dMinZ = z - dHalfHeight;
					}
					else
						z += dHalfHeight;
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(1, 0, 0)));
					++iYarn;
					if ( k == (int)Cell.size()-1 && IsBinderYarn(i) ) // Don't need to add gap above top binder yarn
						z += dHalfHeight;
					else
						z += dHalfHeight + m_dGapSize;
					if ( z > m_dMaxZ )
						m_dMaxZ = z;
				}
				else if ( Cell[k] == PATTERN3D_YYARN ) // y yarn so add y yarn height
				{
					z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
				}
				else if ( k > 0 )// PATTERN3D_NOYARN and not on bottom binder layer so add height of adjacent yarn
				{
					if ( NextCell[k] == PATTERN3D_XYARN )
					{
						z += m_XYarnData[NextCellIndex].dHeight + m_dGapSize;
					}
					else if ( NextCell[k] == PATTERN3D_YYARN )
					{
						z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
					}
					else // PATTERN3D_NOYARN
					{
						if ( k != (int)Cell.size()- 1 ) // If at top and no yarn don't need to bother adding to height
						{
							int NextYCellIndex = FindNextYCellIndex( j, i, k );
							const vector<PATTERN3D> &NextYCell = GetCell(NextYCellIndex, i );
							if ( NextYCell[k] == PATTERN3D_YYARN )
							{
								z += m_YYarnData[(j+1)%m_iNumYYarns].dHeight + m_dGapSize;
							}
							else if ( NextYCell[k] == PATTERN3D_XYARN )
							{
								z += m_XYarnData[i].dHeight + m_dGapSize;
							}
							//else
							//{
								// Does this ever happen!
							//}
						}
					}
				}
			}
			if (j<m_iNumYYarns)
				x += m_YYarnData[j].dSpacing/2.0;
		}
		y += m_XYarnData[i].dSpacing/2.0;
	}

	// Add y yarns (yarns parallel to the y axis)
	// Create yarns first so that won't cause problem if yarns cross over
	m_iYYarnOffset = m_Yarns.size();
	for ( j=0; j<m_iNumYYarns; ++j)
	{
		Yarns.clear();
		
		const vector<PATTERN3D> &Cell = GetCell(j, 0);
		for (k=0; k<(int)Cell.size(); ++k)
		{
			if (Cell[k] == PATTERN3D_YYARN)
			{
				Yarns.push_back(AddYarn(CYarn()));
			}
		}
		
		m_YYarns[j] = Yarns;
	}

	x = 0;
	for (j=0; j<m_iNumYYarns; ++j)
	{
		y = 0;
		Yarns.clear();
		x += m_YYarnData[j].dSpacing/2.0;

		// For each stack of y yarns add a node at the crossover with each x yarn
		// Continue until j == m_iNumXYarns so that get start and end nodes the same
		//for (i=0; i<=m_iNumXYarns; ++i)
		for (i=0; i<m_iNumXYarns; ++i)
		{
			const vector<PATTERN3D> &Cell = GetCell(j, i%m_iNumXYarns);
			const vector<int> &YarnCell = GetYarnCell(j, i%m_iNumXYarns);
			
			int NextCellIndex = FindNextCellIndex(i);
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex%m_iNumXYarns);

			
			
			//iYarn = 0;
			// Increment y coordinate to position of next x yarn
			y += m_XYarnData[i%m_iNumXYarns].dSpacing/2.0;
			z = 0.0;
			// Work upwards through cell
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_YYARN) // y yarn so add node
				{
					double dHalfHeight = m_YYarnData[j].dHeight / 2.0;
					z += dHalfHeight;
					//m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(0, 1, 0)));
					m_Yarns[YarnCell[k]+m_iYYarnOffset].AddNode(CNode(XYZ(x, y, z), XYZ(0, 1, 0)));
					//++iYarn;
					z += dHalfHeight + m_dGapSize;
				}
				else if ( Cell[k] == PATTERN3D_XYARN && k > 0 ) // Don't adjust z if it's the binder yarn
				{
					z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
				}
				else if ( k > 0 ) // PATTERN3D_NOYARN and not on bottom binder layer
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
						//int NextYCellIndex = FindNextYCellIndex( j, i, k );
						//const vector<PATTERN3D> &NextYCell = GetCell(NextYCellIndex, i%m_iNumXYarns );
						int NextYCellIndex = FindPrevYCellIndex( j, i, k );
						const vector<PATTERN3D> &NextYCell = GetCell(NextYCellIndex, i%m_iNumXYarns );
						//const vector<PATTERN3D> &NextYCell = GetCell((j+1)%m_iNumYYarns, i%m_iNumXYarns );
						if ( NextYCell[k] == PATTERN3D_YYARN )
						{
							z += m_YYarnData[(j+1)%m_iNumYYarns].dHeight + m_dGapSize;
						}
						else if ( NextYCell[k] == PATTERN3D_XYARN )
						{
							z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
						}
						//else
						//{
							// Does this ever happen!
						//}
					}
				}
			}
			if (i<m_iNumXYarns)
				y += m_XYarnData[i].dSpacing/2.0;
		}
		x += m_YYarnData[j].dSpacing/2.0;
	}

	// Assign sections and properties to the yarns
	vector<int>::iterator itpYarn;
	double dWidth, dHeight;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		dWidth = m_XYarnData[i].dWidth;
		dHeight = m_XYarnData[i].dHeight;
		CSectionPowerEllipse Section(dWidth, dHeight, IsBinderYarn(i) ? m_dBinderPower : m_dWarpPower );
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		
		for (itpYarn = m_XYarns[i].begin(); itpYarn != m_XYarns[i].end(); ++itpYarn)
		{
			// If refining will be changing the sections at each node so use CYarnSectionInterpNode
			if ( m_bRefine /*&& !IsBinderYarn(i)*/) 
			{
				CYarnSectionInterpNode YarnSections;
				for ( j = 0; j <= m_iNumYYarns; ++j )
				{ 
					YarnSections.AddSection( Section );
				}
				m_Yarns[*itpYarn].AssignSection( YarnSections );
			}
			else  // otherwise constant section along yarn
			{
				m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
			}

			int iType = IsBinderYarn(i) ? BINDER : WARP;
			SetYarnProperties( m_Yarns[*itpYarn], iType );
		}
	}
	for (i=0; i<m_iNumYYarns; ++i)
	{
		dWidth = m_YYarnData[i].dWidth;
		dHeight = m_YYarnData[i].dHeight;
		CSectionPowerEllipse Section(dWidth, dHeight, m_dWeftPower );
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		
		for (itpYarn = m_YYarns[i].begin(), j=0; itpYarn != m_YYarns[i].end(); ++itpYarn, ++j)
		{
			if ( m_bRefine ) 
			{
				CYarnSectionInterpNode YarnSections;
				for ( k = 0; k <= m_iNumXYarns; ++k )
				{
					YarnSections.AddSection( Section );
				}
				m_Yarns[*itpYarn].AssignSection( YarnSections );
			}
			else
			{
				m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
			}
			SetYarnProperties( m_Yarns[*itpYarn], WEFT );
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
		//itYarn->AddRepeat(XYZ(dWidth, 0, 0));
		//itYarn->AddRepeat(XYZ(0, dHeight, 0));
	}

	// Refine yarns
	/*if ( m_bRefine && m_dFabricThickness > 0.0 )
	{
		CheckBinderWidths();
		
		if ( !AdjustMidLayerHeights() )  // Try to change total thickness of layers, excluding top & bottom weft layers
		{						  // May not achieve if can't stay within maximum Vf without exceeding yarn spacing
			bSuccess = false;
		}
		else
		{
			AdjustOuterWeftYarns();   // Change outer weft yarns to hybrid section. 
			AdjustBinderYarns();
		}
	}*/

	ShapeBinderYarns();  // Add nodes to binder yarns to follow profile of top & bottom weft yarns

	

	//PROFILE_END();
	//PROFILER_UPDATE();
	//PROFILER_OUTPUT("ProfileOutput.txt");
	return bSuccess;
}


void CTextileOrthogonal::ShapeBinderYarns() const
{
	//PROFILE_FUNC();
	for ( int j = 0; j < m_iNumXYarns; ++j )
	{
		if ( IsBinderYarn( j ) )
		{
			int CurrentNode = 0;
			for ( int i = 0; i < m_iNumYYarns; ++i )
			{
				CurrentNode = AddBinderNodes( CurrentNode, i, j );
				CurrentNode++;
			}
			CheckUpVectors(j);
		}
	}
}

int CTextileOrthogonal::AddBinderNodes( int CurrentNode, int i, int j ) const
{
	//PROFILE_FUNC();
	const vector<PATTERN3D> &Cell = GetCell(i,j);
	const vector<PATTERN3D> &NextCell = GetCell( (i+1)%m_iNumYYarns, j );
	const vector<PATTERN3D> &PrevCell = GetCell( (i+m_iNumYYarns-1)%m_iNumYYarns, j );
	
	int iBinderIndex = Cell.size() - 1;
	XY SectionPoint;

	if ( iBinderIndex < 1 )
		return CurrentNode;

	double dDefaultBinderOffset = m_XYarnData[j].dHeight/2.0;
	CSectionPowerEllipse DefaultBinderSection(m_XYarnData[j].dWidth, m_XYarnData[j].dHeight, m_dBinderPower );

	if ( Cell[iBinderIndex] != PATTERN3D_XYARN )
	{
		iBinderIndex = FindBinderHeight( Cell );
	}

	if ( iBinderIndex == Cell.size()-1 && Cell[iBinderIndex] == PATTERN3D_XYARN && Cell[iBinderIndex-1] == PATTERN3D_YYARN ) // Binder yarn at top and yarn underneath is weft yarn
	{
		int BinderYarnIndex = GetYarnIndex( i, j, iBinderIndex );
		int WeftYarnIndex;
		if ( m_bWeavePattern )
			WeftYarnIndex = GetWeavePatternYarnIndex( i, j, iBinderIndex-1 );
		else
			WeftYarnIndex = GetYarnIndex( i, j, iBinderIndex-1 );

		if ( BinderYarnIndex == -1 || WeftYarnIndex == -1 )
			return CurrentNode;

		XYZ WeftNode = m_Yarns[WeftYarnIndex].GetNode( j )->GetPosition();
		XYZ BinderNode = m_Yarns[BinderYarnIndex].GetNode( CurrentNode )->GetPosition();

		// Get cross section of weft yarn 
		CSection* YarnSection = GetCrossSection( WeftYarnIndex, j );
		if ( YarnSection == NULL )
			return CurrentNode;

		string WeftType = YarnSection->GetType();
		if ( WeftType != "CSectionPowerEllipse" && WeftType != "CSectionHybrid" )
			return CurrentNode;

		
		CSection* BinderYarnSection = GetCrossSection( BinderYarnIndex, CurrentNode );
		string BinderType = BinderYarnSection->GetType();
		double dBinderOffset = dDefaultBinderOffset;
		if ( BinderType == "CSectionHybrid" )
		{
			dBinderOffset = GetHybridHeight( (CSectionHybrid*)BinderYarnSection );
		}

		int PrevCellBinder = FindBinderHeight( PrevCell );
 
		if ( PrevCellBinder < iBinderIndex )   // Insert points around Top left quadrant of weft yarn
		{
			// Insert point at left hand end of weft yarn. 
			XYZ NewNode = WeftNode;
			SectionPoint = GetSectionPoint( YarnSection, 0.5 );
			map<pair<int, int>, YARNDATA>::iterator itBinderData = m_BinderData.find( make_pair((i-1+m_iNumYYarns)%m_iNumYYarns,j) );

			NewNode.x = NewNode.x + SectionPoint.x - m_dGapSize;
			if ( m_bRefine && itBinderData != m_BinderData.end() )
				NewNode.x -= itBinderData->second.dHeight / 2.0;
			else
				NewNode.x -= dDefaultBinderOffset;
			NewNode.z = NewNode.z + SectionPoint.y; 
			m_Yarns[BinderYarnIndex].InsertNode( NewNode, CurrentNode);

			if ( m_bRefine )
				ReplaceThroughBinderSection( BinderYarnIndex, CurrentNode, DefaultBinderSection, itBinderData );

			if ( CurrentNode == 0 )  // Change last node to match start
			{
				ReplaceLastNode( BinderYarnIndex, NewNode, BinderNode );
				if ( m_bRefine )
					ReplaceThroughBinderSection( BinderYarnIndex, m_Yarns[BinderYarnIndex].GetNumNodes() - 1, DefaultBinderSection, itBinderData );
			}
			CurrentNode++;

			// Insert points between new end node and centre, top node 
			double t = m_bRefine ? 0.4 : 0.45;
			int i = m_bRefine ? 3 : 4;
			while( i-- > 0)
			{
				InsertBinderNode( YarnSection, t, WeftNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
				t -= 0.05;
			}
		}

		int NextCellBinder = FindBinderHeight( NextCell );
		 
		if ( NextCellBinder < iBinderIndex )    // Insert points around upper right quadrant of weft
		{
			XYZ NewNode = WeftNode;
			CurrentNode++; // Need to insert after node (ie before next node)
			double t = 0.2;
			int k = m_bRefine ? 3 : 4;
			while( k-- > 0 )
			{
				InsertBinderNode( YarnSection, t, WeftNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
				if ( m_bRefine )
					ReplaceYarnSection( BinderYarnIndex, CurrentNode-1, *BinderYarnSection );
				t -= 0.05;
			}
			
			CurrentNode--;
			SectionPoint = GetSectionPoint( YarnSection, 0.0 );
			
			map<pair<int, int>, YARNDATA>::iterator itBinderData = m_BinderData.find( make_pair(i,j) );
			NewNode = WeftNode;
			NewNode.x = NewNode.x + SectionPoint.x + m_dGapSize;
			if ( m_bRefine && itBinderData != m_BinderData.end() )
				NewNode.x += itBinderData->second.dHeight / 2.0;
			else
				NewNode.x += dDefaultBinderOffset;
			NewNode.z = NewNode.z + SectionPoint.y;
			m_Yarns[BinderYarnIndex].InsertNode( NewNode, CurrentNode+1 );
			CurrentNode++;
			
			if ( m_bRefine )
				ReplaceThroughBinderSection( BinderYarnIndex, CurrentNode, DefaultBinderSection, itBinderData );				
		}
		delete YarnSection;
		delete BinderYarnSection;
	}
	// Binder yarn at bottom and yarn above is weft yarn
	else if ( iBinderIndex == 0 && Cell[iBinderIndex] == PATTERN3D_XYARN && Cell[iBinderIndex+1] == PATTERN3D_YYARN )
	{
		int BinderYarnIndex = GetYarnIndex( i, j, iBinderIndex );
		int WeftYarnIndex;
		if ( m_bWeavePattern )
			WeftYarnIndex = GetWeavePatternYarnIndex( i, j, iBinderIndex+1 );
		else
			WeftYarnIndex = GetYarnIndex( i, j, iBinderIndex+1 ); 

		if ( BinderYarnIndex == -1 || WeftYarnIndex == -1 )
			return CurrentNode;

		// Get cross section of weft yarn
		CSection* YarnSection = GetCrossSection( WeftYarnIndex, j );
		if ( YarnSection == NULL )
			return CurrentNode;

		string WeftType = YarnSection->GetType();
		if ( WeftType != "CSectionPowerEllipse" && WeftType != "CSectionHybrid" )
		{
			delete YarnSection;
			return CurrentNode;
		}

		CSection* BinderYarnSection = GetCrossSection( BinderYarnIndex, CurrentNode );
		string BinderType = BinderYarnSection->GetType();
		double dBinderOffset = dDefaultBinderOffset;
		if ( BinderType == "CSectionHybrid" )
		{
			dBinderOffset = GetHybridHeight( (CSectionHybrid*)BinderYarnSection );
		}

		XYZ WeftNode = m_Yarns[WeftYarnIndex].GetNode( j )->GetPosition();
		XYZ BinderNode = m_Yarns[BinderYarnIndex].GetNode( CurrentNode )->GetPosition();

		int PrevCellBinder = FindBinderHeight( PrevCell );
		
		if ( PrevCellBinder > iBinderIndex )    // Insert points around lower left quadrant of weft yarn
		{
			XYZ NewNode = WeftNode;
			map<pair<int, int>, YARNDATA>::iterator itBinderData = m_BinderData.find( make_pair((i-1+m_iNumYYarns)%m_iNumYYarns,j) );

			SectionPoint = GetSectionPoint( YarnSection, 0.5 );
			NewNode.x = NewNode.x + SectionPoint.x - m_dGapSize;
			if ( m_bRefine && itBinderData != m_BinderData.end() )
				NewNode.x -= itBinderData->second.dHeight / 2.0;
			else
				NewNode.x -= dDefaultBinderOffset;
			NewNode.z = NewNode.z + SectionPoint.y;
			m_Yarns[BinderYarnIndex].InsertNode( NewNode, CurrentNode);
			
			if ( m_bRefine )
				ReplaceThroughBinderSection( BinderYarnIndex, CurrentNode, DefaultBinderSection, itBinderData );

			if ( CurrentNode == 0 )  // Change last node to match start
			{
				ReplaceLastNode( BinderYarnIndex, NewNode, BinderNode );
				if ( m_bRefine )
					ReplaceThroughBinderSection( BinderYarnIndex, m_Yarns[BinderYarnIndex].GetNumNodes() - 1, DefaultBinderSection, itBinderData );
			}
			CurrentNode++;

			NewNode = WeftNode;
			double t = m_bRefine ? 0.6 : 0.55;
			int k = m_bRefine ? 3 : 4;
			while ( k-- > 0 )
			{
				InsertBinderNode( YarnSection, t, WeftNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
				t += 0.05;
			}
		}

		int NextCellBinder = FindBinderHeight( NextCell );

		if ( NextCellBinder > iBinderIndex )    // Insert points around lower right quadrant of weft yarn
		{
			XYZ NewNode = WeftNode;
			CurrentNode++;  // Need to insert after node (ie before next node)
			double t = 0.8;
			int k = m_bRefine ? 3 : 4;
			while ( k-- > 0 )
			{
				InsertBinderNode( YarnSection, t, WeftNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
				if ( m_bRefine )
					ReplaceYarnSection( BinderYarnIndex, CurrentNode-1, *BinderYarnSection );
				t += 0.05;
			}
			
			CurrentNode--;
			map<pair<int, int>, YARNDATA>::iterator itBinderData = m_BinderData.find( make_pair(i,j) );
			SectionPoint = GetSectionPoint( YarnSection, 0.0 );
			NewNode = WeftNode;
			NewNode.x = NewNode.x + SectionPoint.x + m_dGapSize;
			if ( m_bRefine && itBinderData != m_BinderData.end() )
				NewNode.x += itBinderData->second.dHeight / 2.0;
			else
				NewNode.x += dDefaultBinderOffset;
			NewNode.z = NewNode.z + SectionPoint.y;
			m_Yarns[BinderYarnIndex].InsertNode( NewNode, CurrentNode+1 );
			CurrentNode++;
			
			if ( m_bRefine )
				ReplaceThroughBinderSection( BinderYarnIndex, CurrentNode, DefaultBinderSection, itBinderData );
		}
		delete YarnSection;
		delete BinderYarnSection;
	}
	return CurrentNode;
}

void CTextileOrthogonal::CheckBinderWidths() const
{
	// Iterate through each x yarn and check widths if it's a binder yarn
	for ( int j = 0; j < m_iNumXYarns; ++j )
	{
		if ( IsBinderYarn(j) )
		{
			int iBinderIndex = -1;
			
			for ( int i = 0; i < m_iNumYYarns; ++i )
			{
				// Get the cells for the two weft yarns that the yarn would be passing between
				const vector<PATTERN3D> &Cell = GetCell(i,j);
				const vector<PATTERN3D> &NextCell = GetCell((i+1)%m_iNumYYarns, j);
				int iCellSize = Cell.size();
				
				if ( Cell[0] != NextCell[0] ) // Binder is through thickness between cells
				{							  // In orthogonal weaves binder is forced to be at either top or bottom
					if ( iBinderIndex == -1 )
					{
						if ( Cell[0] == PATTERN3D_XYARN )
							iBinderIndex = GetYarnIndex(i,j,0);
						else
							iBinderIndex = GetYarnIndex(i,j,iCellSize-1);
					}

					// Get initial specified cross section
					CSection* BinderYarnSection = GetCrossSection( iBinderIndex, i );
					CSectionPowerEllipse* YarnSection = (CSectionPowerEllipse*)BinderYarnSection->Copy();

					double FibreArea = m_Yarns[iBinderIndex].GetFibreArea("mm^2");  // Actual area of fibres within yarn
					double InitialArea = YarnSection->GetArea(YarnSection->GetPoints(40)); 
					if ( FibreArea/InitialArea > m_dMaxVolumeFraction )
					{
						TGERROR( "Binder yarn initial volume fraction exceeds max Vf Yarn: " << iBinderIndex );
						delete YarnSection;
						return;
					}	
					
					
					YARNDATA Binder;
					if ( m_XYarnData[j].dWidth > (m_XYarnData[j].dSpacing - m_dGapSize) ) // Yarn is wider than spacing
					{													// ***Should this do check == max height check and look at actual space between warps either side?
						double dMaxWidth, dMaxHeight;
						dMaxWidth = m_XYarnData[j].dSpacing - m_dGapSize;
						// Calculate maximum space available between weft yarns
						dMaxHeight = (m_YYarnData[i].dSpacing-m_YYarnData[i].dWidth)/2.0 
									+ (m_YYarnData[(i+1)%m_iNumYYarns].dSpacing-m_YYarnData[(i+1)%m_iNumYYarns].dWidth)/2.0 - m_dGapSize;
						double dMaxArea = dMaxWidth * dMaxHeight; // Max area available for binder yarn to pass through
						Binder.dWidth = dMaxWidth;
						Binder.dSpacing = dMaxWidth;

						if ( InitialArea < dMaxArea ) // Adjust height to maintain initial area whilst reducing width
						{
							Binder.dHeight = InitialArea/dMaxWidth;  // Assuming will be rectangular section
							Binder.dSpacing = dMaxWidth;
						}
						else  // Area will need to reduce so check if this would result in exceeding the max volume fraction
						{
							if ( FibreArea/dMaxArea > m_dMaxVolumeFraction )
							{
								// Wefts will need to reduce in width in order to make room for binder at max Vf
								// ***Valid assumption that it's the wefts that move and not and/or warps?
								double MaxVfArea = FibreArea/m_dMaxVolumeFraction;
								Binder.dHeight = MaxVfArea/dMaxWidth;  // Binder width and height now set to give max possible Vf
							}
							else  // Volume fraction ok
							{
								Binder.dHeight = dMaxHeight;
							}
						}
						m_BinderData[make_pair(i,j)] = Binder;
					}
					else  // Width of binder fits into spacing between warps so make pair.  
					{				// ***This assumes that binder height < spacing between wefts - is this correct?   
						m_BinderData[make_pair(i,j)] = m_XYarnData[j];
					}
				}
			}
		}
	}
}

bool CTextileOrthogonal::AdjustMidLayerHeights() const
{
	//PROFILE_FUNC();
	vector<int> RectYarnIndex;  // Yarn indices of yarns containing sections to be converted to rectangles

	for ( int j = 0; j < m_iNumXYarns; ++j )
	{
		for ( int i = 0; i < m_iNumYYarns; ++i )
		{
			const vector<PATTERN3D> &Cell = GetCell(i,j);
			int iCellSize = Cell.size();
			// Have to allow for specified gap size. All reduction is taken up in yarns
			int iTopWeftIndex = GetYarnIndex(i,j,iCellSize-2);
			int iBottomWeftIndex = GetYarnIndex(i,j,1);
			XYZ TopNode, BottomNode;
			if ( iTopWeftIndex != -1 )
				TopNode = m_Yarns[iTopWeftIndex].GetNode(j)->GetPosition();
			if ( iBottomWeftIndex != -1 )
				BottomNode = m_Yarns[iBottomWeftIndex].GetNode(j)->GetPosition();
			double TargetThickness = (iCellSize-4)*m_dFabricThickness/(iCellSize-2);  //Ratio of number of mid-layers to total warp/weft layers
			TargetThickness -= (iCellSize-3)*m_dGapSize; 
			
			double StartThickness = 0.0;
			
			// Find the starting thickness of the mid layers (ie without binder layer and top and bottom weft layer)
			for ( int k = 2; k < iCellSize-2; ++k )
			{
				if ( Cell[k] == PATTERN3D_XYARN )
				{
					StartThickness += m_XYarnData[j].dHeight;
				}
				else if ( Cell[k] == PATTERN3D_YYARN )
				{
					StartThickness += m_YYarnData[i].dHeight;
				}
				else // PATTERN3D_NOYARN
				{
					StartThickness += m_XYarnData[(j+m_iNumXYarns-1)%m_iNumXYarns].dHeight;
				}
			}
			double CellHeightReduction = TargetThickness / StartThickness;
			double zDisplacement = 0.0;
			if ( fabs( 1.0 - CellHeightReduction ) > TOL && CellHeightReduction < 1.0 ) // If cell contains thinner yarns would result in HeightReduction > 1 in which case don't need to refine
			{
				// Work through each mid-layer yarn in the stack at the given i,j position
				for ( int k = 2; k < iCellSize-2; ++k )
				{										
					int YarnIndex;
					YarnIndex = GetYarnIndex( i, j, k );
					XYZ Node;
					double MaxWidth = 0.0;
					
					if ( YarnIndex == -1 )
						continue;
					if ( m_Yarns[YarnIndex].GetYarnSection()->GetType() != "CYarnSectionInterpNode" )
						continue;
					double FibreArea = m_Yarns[YarnIndex].GetFibreArea("mm^2");
					if ( FibreArea == 0.0 )
					{
						TGERROR("Cannot adjust height of yarn" << YarnIndex << ": No fibre area set" );
						continue;
					}

					CYarnSectionInterpNode* pYarnSections = (CYarnSectionInterpNode*)m_Yarns[YarnIndex].GetYarnSection()->Copy();
					CSectionPowerEllipse* YarnSection = NULL;
					
					if ( Cell[k] == PATTERN3D_XYARN )
					{
						Node = m_Yarns[YarnIndex].GetNode(i)->GetPosition();
						YarnSection = (CSectionPowerEllipse*)pYarnSections->GetNodeSection(i).Copy();
						MaxWidth = m_XYarnData[j].dSpacing - m_dGapSize;
					}
					else if ( Cell[k] == PATTERN3D_YYARN )
					{
						Node = m_Yarns[YarnIndex].GetNode(j)->GetPosition();
						YarnSection = (CSectionPowerEllipse*)pYarnSections->GetNodeSection(j).Copy();
						if ( IsBinderYarn(j) && Cell[k-1] == PATTERN3D_NOYARN )  // Binder yarn cell so no warp yarn below
						{														 // Assume this weft will be at same height as previous node	
							XYZ PrevNode = m_Yarns[YarnIndex].GetNode(j-1)->GetPosition();
							zDisplacement = Node.z - PrevNode.z;
							Node.z = PrevNode.z;
						}
						MaxWidth = m_YYarnData[i].dSpacing - m_dGapSize;  
						if ( IsBinderYarn(j) )  // If it's a binder yarn reduce the max width available for the weft by half the binder height on either side
						{						// Binder data will only be available at the given position if binder is passing through thickness at that point
							map<pair<int, int>, YARNDATA>::iterator itBinderData;

							itBinderData = m_BinderData.find( make_pair((i-1+m_iNumYYarns)%m_iNumYYarns,j) );
							if ( itBinderData != m_BinderData.end() )
							{
								MaxWidth -= itBinderData->second.dHeight / 2.0;
							}
							itBinderData = m_BinderData.find( make_pair(i,j) );
							if ( itBinderData != m_BinderData.end() )
							{
								MaxWidth -= itBinderData->second.dHeight / 2.0;  
							}
						}
					}
					
					if ( YarnSection == NULL || MaxWidth == 0.0)
						continue;

					// Check volume fraction
					double OldHeight = YarnSection->GetHeight();
					double TargetHeight = CellHeightReduction * OldHeight;
					double MaxArea = MaxWidth * TargetHeight;
					
					double TargetArea = YarnSection->GetArea(YarnSection->GetPoints(40));
					double StartVolumeFraction = FibreArea/TargetArea;
					double AreaVolumeFraction = FibreArea/MaxArea; // Volume fraction if yarn was rectangle filling all available space
					//double MaxVolumeFraction = StartVolumeFraction > MAX_VOLUME_FRACTION ? StartVolumeFraction : MAX_VOLUME_FRACTION;
					double MaxVolumeFraction = m_dMaxVolumeFraction; // Need some allowance for solid yarns?
					
					if ( AreaVolumeFraction > MaxVolumeFraction )  // Volume fraction for space available exceeds physically achievable 
					{
						TGERROR("Cannot achieve this thickness: exceeds maximum volume fraction");
						return false;
					}

					if ( AreaVolumeFraction > StartVolumeFraction )  // Whole area gives Vf > original yarn Vf so create section to fill area
					{
						CSectionPowerEllipse Section( MaxWidth, TargetHeight, 0.0 );
						*YarnSection = Section;
					}
					else  // Vf can be achieved maintaining original Vf so find new power and width
					{
						if ( !AdjustPowerEllipsePower( CellHeightReduction, TargetArea, YarnSection, 0.0 ) )
						{
							YarnSection->SetPower( 0.0 );
							AdjustPowerEllipseSectionWidth( CellHeightReduction, MaxWidth, TargetArea, YarnSection );  
						}
					}
					
					double HeightChange = OldHeight - YarnSection->GetHeight();
					if ( !( IsBinderYarn(j) && Cell[k-1] == PATTERN3D_NOYARN) ) 
					{
						Node.z = Node.z - 0.5*HeightChange - zDisplacement;
						zDisplacement += HeightChange;
					}

					if ( Cell[k] != PATTERN3D_NOYARN )
					{
						// Replace node position and cross-section with new values
						int iIndex = Cell[k] == PATTERN3D_XYARN ? i : j;
						int iNumYarns = Cell[k] == PATTERN3D_XYARN ? m_iNumYYarns : m_iNumXYarns;
						m_Yarns[YarnIndex].ReplaceNode(iIndex,CNode(Node));
						pYarnSections->ReplaceSection( iIndex, *YarnSection );
						if ( iIndex == 0 ) // make 1st and last nodes match
						{
							XYZ EndNode = m_Yarns[YarnIndex].GetNode(iNumYarns)->GetPosition();
							EndNode.z = Node.z;
							m_Yarns[YarnIndex].ReplaceNode( iNumYarns, CNode(EndNode) );
							pYarnSections->ReplaceSection( iNumYarns, *YarnSection );
						}
					}
					// Save list of yarns which have rectangular sections in them
					if ( YarnSection->GetPower() == 0.0 )
					{
						vector<int>::iterator itRectYarnIndex = find( RectYarnIndex.begin(), RectYarnIndex.end(), YarnIndex );
						if ( itRectYarnIndex == RectYarnIndex.end() )
						{
							RectYarnIndex.push_back( YarnIndex );
						}
					}
					m_Yarns[YarnIndex].AssignSection(*pYarnSections);
					delete pYarnSections;
					delete YarnSection;

					
				}
			}
			else
			{
				return true;  // No change so return
			}
			// Move top weft and binder yarns by vertical displacement
			// Assume it's a weft node
			int YarnIndex = GetYarnIndex( i, j, iCellSize-2 );
			XYZ Node = m_Yarns[YarnIndex].GetNode(j)->GetPosition();

			if ( IsBinderYarn(j) && Cell[iCellSize-3] == PATTERN3D_NOYARN )
			{
				XYZ PrevNode = m_Yarns[YarnIndex].GetNode(j-1)->GetPosition();
				zDisplacement = Node.z - PrevNode.z;
				Node.z = PrevNode.z;
			}
			else
			{
				Node.z -= zDisplacement;
			}
			m_Yarns[YarnIndex].ReplaceNode( j, CNode(Node) );
			if ( j == 0 )
			{
				XYZ EndNode = m_Yarns[YarnIndex].GetNode(m_iNumXYarns)->GetPosition();
				EndNode.z = Node.z;
				m_Yarns[YarnIndex].ReplaceNode( m_iNumXYarns, CNode(EndNode) );
			}
			if ( Cell[iCellSize-1] == PATTERN3D_XYARN ) // ie binder yarn
			{
				YarnIndex = GetYarnIndex( i, j, iCellSize-1 );
				Node = m_Yarns[YarnIndex].GetNode(i)->GetPosition();
				Node.z -= zDisplacement;
				m_Yarns[YarnIndex].ReplaceNode( i, CNode(Node) );
				if ( i == 0 )
				{
					XYZ EndNode = m_Yarns[YarnIndex].GetNode(m_iNumYYarns)->GetPosition();
					EndNode.z = Node.z;
					m_Yarns[YarnIndex].ReplaceNode( m_iNumYYarns, CNode(EndNode) );
				}
			}
		}
	}

	// Replace power ellipses with power = 0 with rectangular sections
	vector<int>::iterator itRectYarnIndex;
	for ( itRectYarnIndex = RectYarnIndex.begin(); itRectYarnIndex != RectYarnIndex.end(); ++itRectYarnIndex )
	{
		CYarnSectionInterpNode* pYarnSections = (CYarnSectionInterpNode*)m_Yarns[*itRectYarnIndex].GetYarnSection()->Copy();
		for ( int i = 0; i < pYarnSections->GetNumNodeSections(); ++i )
		{
			CSectionPowerEllipse* YarnSection = (CSectionPowerEllipse*)pYarnSections->GetNodeSection(i).Copy();
			if ( YarnSection->GetPower() == 0.0 )
			{
				CSectionRectangle Rect = CSectionRectangle( YarnSection->GetWidth(), YarnSection->GetHeight() );
				pYarnSections->ReplaceSection( i, Rect );
			}
			else // non-rectangular section in yarn with rectangular sections.  Set section meshes to not have triangular corners so that volume meshing works
			{
				CSectionMeshRectangular SectionMesh = CSectionMeshRectangular( -1, false );
				YarnSection->AssignSectionMesh( SectionMesh );
				pYarnSections->ReplaceSection( i, *YarnSection );
			}
		}
		m_Yarns[*itRectYarnIndex].AssignSection(*pYarnSections);
	}
	return true;
}

void CTextileOrthogonal::AdjustOuterWeftYarns() const
{
	//PROFILE_FUNC();
	m_dMinZ = 0.0;
	m_dMaxZ = 0.0;

	for ( int j = 0; j < m_iNumXYarns; ++j )
	{
		for ( int i = 0; i < m_iNumYYarns; ++i )
		{
			const vector<PATTERN3D> &Cell = GetCell(i,j);
			
			int YarnIndex;
			// Bottom yarn
			YarnIndex = GetYarnIndex( i, j, 1 );

			if ( YarnIndex == -1 )
				continue;
			if ( m_Yarns[YarnIndex].GetYarnSection()->GetType() != "CYarnSectionInterpNode" )
				continue;
			double FibreArea = m_Yarns[YarnIndex].GetFibreArea("mm^2");
			if ( FibreArea == 0.0 )
			{
				TGERROR("Cannot adjust height of yarn" << YarnIndex << ": No fibre area set" );
				continue;
			}

			// Get current section and find area
			CYarnSectionInterpNode* pYarnSections = (CYarnSectionInterpNode*)m_Yarns[YarnIndex].GetYarnSection()->Copy();
			CSectionPowerEllipse* YarnSection;
			YarnSection = (CSectionPowerEllipse*)pYarnSections->GetNodeSection(j).Copy();
			double TargetArea = YarnSection->GetArea(YarnSection->GetPoints(40))*2.0; // *2.0 because only using half in hybrid section

			// Find total height of mid-section yarns
			XYZ BottomYarn = m_Yarns[YarnIndex].GetNode(j)->GetPosition();
			int TopYarnIndex = GetYarnIndex( i, j, Cell.size()-2 );
			XYZ TopYarn = m_Yarns[TopYarnIndex].GetNode(j)->GetPosition();
			double MidSectionHeight = TopYarn.z - BottomYarn.z - YarnSection->GetHeight();

			double MaxVolumeFraction = m_dMaxVolumeFraction;

			// Find maximum width for weft yarn. If binder yarn section then need to reduce width if through thickness binder on either of sides
			double MaxWidth = m_YYarnData[i].dSpacing - (2*m_dGapSize); 
			if ( IsBinderYarn(j) )
			{
				map<pair<int, int>, YARNDATA>::iterator itBinderData;

				itBinderData = m_BinderData.find( make_pair((i-1+m_iNumYYarns)%m_iNumYYarns,j) );
				if ( itBinderData != m_BinderData.end() )
				{
					MaxWidth -= itBinderData->second.dHeight / 2.0;
				}
				itBinderData = m_BinderData.find( make_pair(i,j) );
				if ( itBinderData != m_BinderData.end() )
				{  
					MaxWidth -= itBinderData->second.dHeight / 2.0;
				}
			}

			CSectionPowerEllipse PowerEllipseSection( MaxWidth < m_YYarnData[i].dWidth ? MaxWidth : m_YYarnData[i].dWidth, m_YYarnData[i].dHeight * 2.0, 1.4 ); 

			// Reduce height of power ellipse by changing cross-section whilst maintaining area
			ChangePowerEllipseSection( TargetArea, &PowerEllipseSection, FibreArea );
			CSectionPowerEllipse TopPowerEllipseSection( PowerEllipseSection );

			double TargetHeight, WeftTargetHeight, BinderWeftTargetHeight;
			// Calculate weft height without binder yarn anyway as will need for opposite side to binder
			{
				TargetHeight = (m_dFabricThickness - MidSectionHeight)/2.0;
				WeftTargetHeight = TargetHeight - m_dGapSize;
			}
			if ( IsBinderYarn(j) ) // Weft and binder heights reduced in proportion to their original heights to give target height in total
			{
				BinderWeftTargetHeight = (TargetHeight - m_dGapSize)*m_YYarnData[i].dHeight/(m_YYarnData[i].dHeight+m_XYarnData[j].dHeight);
			}
			
			if ( IsBinderYarn(j) && Cell[0] == PATTERN3D_XYARN ) // Binder yarn at bottom
			{
				if ( (fabs(PowerEllipseSection.GetHeight()/2.0 - BinderWeftTargetHeight) > TOL && (PowerEllipseSection.GetHeight()/2.0) > BinderWeftTargetHeight ) )
				{
					AdjustWeftHeight( PowerEllipseSection, MaxWidth, TargetArea, MaxVolumeFraction, FibreArea, BinderWeftTargetHeight );
				}
			}
			else
			{
				if ( fabs(PowerEllipseSection.GetHeight()/2.0 - WeftTargetHeight) > TOL ) 
				{
					if ( (PowerEllipseSection.GetHeight()/2.0) > WeftTargetHeight )
					{
						AdjustWeftHeight( PowerEllipseSection, MaxWidth, TargetArea, MaxVolumeFraction, FibreArea, WeftTargetHeight );
					}
					else
					{
						double EllipseTargetHeight = WeftTargetHeight * 2.0;
						FindPowerEllipseSectionHeight( EllipseTargetHeight, TargetArea, &PowerEllipseSection );
					}
				}
			}
			
			CSectionEllipse EllipseSection( PowerEllipseSection.GetWidth(), 0.0 );
			ChangeWeftSection( EllipseSection, PowerEllipseSection, YarnIndex, j, m_YYarnData[i].dHeight/2.0);

			double dHeight = m_YYarnData[i].dHeight - PowerEllipseSection.GetHeight()/2.0;
			if ( Cell[0] == PATTERN3D_XYARN ) // Binder yarn at bottom
			{
				AdjustBinderPosition( 0, i, j, dHeight, BOTTOM );
			}
			
			// Top yarn
			
			if ( IsBinderYarn(j) && Cell[Cell.size()-1] == PATTERN3D_XYARN ) // Binder yarn at top
			{
				if ( (fabs(TopPowerEllipseSection.GetHeight()/2.0 - BinderWeftTargetHeight) > TOL && (TopPowerEllipseSection.GetHeight()/2.0) > BinderWeftTargetHeight ) )
				{
					AdjustWeftHeight( TopPowerEllipseSection, MaxWidth, TargetArea, MaxVolumeFraction, FibreArea, BinderWeftTargetHeight );
				}
			}
			else
			{
				if ( fabs(TopPowerEllipseSection.GetHeight()/2.0 - WeftTargetHeight) > TOL )
				{
					
					if ( (TopPowerEllipseSection.GetHeight()/2.0) > WeftTargetHeight )
					{
						AdjustWeftHeight( TopPowerEllipseSection, MaxWidth, TargetArea, MaxVolumeFraction, FibreArea, WeftTargetHeight );
					}
					else
					{
						double EllipseTargetHeight = WeftTargetHeight * 2.0;
						FindPowerEllipseSectionHeight( EllipseTargetHeight, TargetArea, &TopPowerEllipseSection );
					}
				}
			}

			CSectionEllipse TopEllipseSection( TopPowerEllipseSection.GetWidth(), 0.0 );
			ChangeWeftSection( TopPowerEllipseSection, TopEllipseSection, TopYarnIndex, j, -m_YYarnData[i].dHeight/2.0);
			if ( Cell[Cell.size()-1] == PATTERN3D_XYARN ) // Binder yarn at top
			{
				dHeight = m_YYarnData[i].dHeight - TopPowerEllipseSection.GetHeight()/2.0;
				AdjustBinderPosition( Cell.size()-1, i, j, -dHeight, TOP );
			}
			delete pYarnSections;
			delete YarnSection;
		}
	}
}

void CTextileOrthogonal::ChangeWeftSection( CSection& TopHalf, CSection& BottomHalf, int YarnIndex, int Node, double Offset) const
{
	//PROFILE_FUNC();
	CSectionHybrid Section( TopHalf, BottomHalf );
	CYarnSectionInterpNode* pYarnSections = (CYarnSectionInterpNode*)m_Yarns[YarnIndex].GetYarnSection()->Copy();

	pYarnSections->ReplaceSection( Node, Section );
	XYZ NodePos = m_Yarns[YarnIndex].GetNode(Node)->GetPosition();
	NodePos.z += Offset;
	m_Yarns[YarnIndex].ReplaceNode( Node, CNode(NodePos) );

	if ( Node == 0 )
	{
		NodePos = m_Yarns[YarnIndex].GetNode(m_iNumXYarns)->GetPosition();
		NodePos.z += Offset;
		m_Yarns[YarnIndex].ReplaceNode( m_iNumXYarns, CNode(NodePos) );
		pYarnSections->ReplaceSection( m_iNumXYarns, Section );
	}
	m_Yarns[YarnIndex].AssignSection( *pYarnSections );
	delete pYarnSections;
}

bool CTextileOrthogonal::AdjustWeftHeight( CSectionPowerEllipse& PowerEllipseSection, double& MaxWidth, double& TargetArea, double& MaxVolumeFraction, double& FibreArea, double TargetHeight ) const
{
	double InitialHeight = PowerEllipseSection.GetHeight();
	
	double HeightReduction = TargetHeight / (PowerEllipseSection.GetHeight()/2.0);
	if ( HeightReduction <= 0.1 )
		HeightReduction = 0.1;

	// Adjust width whilst maintaining original area
	if ( !AdjustPowerEllipseSectionWidth( HeightReduction, MaxWidth, TargetArea, &PowerEllipseSection ) )
	{
		// Reached max width with area > target (ie > original Vf )
		// Find new volume fraction & check if ok
		double Area = PowerEllipseSection.GetArea( PowerEllipseSection.GetPoints(40) );
		double VolumeFraction = FibreArea/(Area/2.0);
		if ( VolumeFraction > MaxVolumeFraction )  
		{
			// Vf too large so increase height to give MaxVolumeFraction. 
			double MinArea = (FibreArea/MaxVolumeFraction) * 2.0;
			FindPowerEllipseSectionHeight( InitialHeight, MinArea, &PowerEllipseSection );
			return false;
		}
	}
	return true;
}

void CTextileOrthogonal::AdjustWeftToFitBinderHeight( int i, int j, CSectionPowerEllipse& PowerEllipseSection, double& MaxWidth, double& TargetArea, double& MaxVolumeFraction, double& FibreArea ) const
{
	double InitialHeight = PowerEllipseSection.GetHeight();
	// Find proportion by which need to reduce height
	// If binder height is >= weft yarn height set a minimum height reduction - arbitrarily 0.1 at the moment.
	// Any way of determining realistic value/proportion for reduction? Currently assume weft and binder will reduce in proportion to their initial height ratio
	double TargetHeight = (m_YYarnData[i].dHeight - 1.5*m_dGapSize) * m_YYarnData[i].dHeight/(m_YYarnData[i].dHeight+m_XYarnData[j].dHeight);
	//double HeightReduction = (m_YYarnData[i].dHeight - m_XYarnData[j].dHeight - 1.5*m_dGapSize)/ (PowerEllipseSection.GetHeight()/2.0);
	double HeightReduction = TargetHeight / (PowerEllipseSection.GetHeight()/2.0);
	if ( HeightReduction <= 0.1 )
		HeightReduction = 0.1;

	// Adjust width whilst maintaining original area
	if ( !AdjustPowerEllipseSectionWidth( HeightReduction, MaxWidth, TargetArea, &PowerEllipseSection ) )
	{
		// Reached max width with area > target (ie > original Vf )
		// Find new volume fraction & check if ok
		double Area = PowerEllipseSection.GetArea( PowerEllipseSection.GetPoints(40) );
		double VolumeFraction = FibreArea/(Area/2.0);
		if ( VolumeFraction > MaxVolumeFraction )  
		{
			// Vf too large so increase height to give MaxVolumeFraction.  Will therefore need to adjust binders
			double MinArea = (FibreArea/MaxVolumeFraction) * 2.0;
			FindPowerEllipseSectionHeight( InitialHeight, MinArea, &PowerEllipseSection );
		}
	}
}

void CTextileOrthogonal::AdjustBinderPosition( int iCellIndex, int i, int j, double dHeight, bool bIsTop ) const
{
	int YarnIndex = GetYarnIndex( i, j, iCellIndex );
	XYZ Node = m_Yarns[YarnIndex].GetNode(i)->GetPosition();
	Node.z += dHeight;
	m_Yarns[YarnIndex].ReplaceNode( i, CNode(Node) );
	if ( i == 0 )
	{
		XYZ LastNode = m_Yarns[YarnIndex].GetNode(m_iNumYYarns)->GetPosition();
		LastNode.z = Node.z;
		m_Yarns[YarnIndex].ReplaceNode( m_iNumYYarns, CNode(LastNode) );
	}
	if ( bIsTop )
	{
		double NewMaxZ = Node.z + m_XYarnData[j].dHeight/2.0;
		if ( NewMaxZ > m_dMaxZ )
			m_dMaxZ = NewMaxZ;
	}
	else
	{
		double NewMinZ = Node.z - m_XYarnData[j].dHeight/2.0;
		if ( NewMinZ < m_dMinZ )
			m_dMinZ = NewMinZ;
	}
}

void CTextileOrthogonal::AdjustBinderYarns() const
{
	//PROFILE_FUNC();
	for ( int j = 0; j < m_iNumXYarns; ++j )
	{
		if ( IsBinderYarn( j ) )
		{
			for ( int i = 0; i < m_iNumYYarns; ++i )
			{
				AdjustBinderYarnSection( i, j );
			}
		}
	}
}

void CTextileOrthogonal::AdjustBinderYarnSection( int i, int j ) const
{
	const vector<PATTERN3D> &Cell = GetCell(i,j);
	int iMaxIndex = (int)Cell.size() - 1;

	if ( iMaxIndex < 1 )
		return;

	if ( Cell[iMaxIndex] == PATTERN3D_XYARN && Cell[iMaxIndex-1] == PATTERN3D_YYARN ) // Binder yarn at top and yarn underneath is weft yarn
	{
		int BinderYarnIndex = GetYarnIndex( i, j, iMaxIndex );
		int WeftYarnIndex = GetYarnIndex( i, j, iMaxIndex-1 );
		int BottomWeftYarnIndex = GetYarnIndex( i, j, 1 );

		if ( BinderYarnIndex == -1 || WeftYarnIndex == -1 )
			return;

		AdjustBinderYarnSection( i, j, BinderYarnIndex, WeftYarnIndex, BottomWeftYarnIndex, TOP );
	}
	else if ( Cell[0] == PATTERN3D_XYARN && Cell[1] == PATTERN3D_YYARN ) // Binder yarn at bottom and yarn above is weft yarn
	{
		int BinderYarnIndex = GetYarnIndex( i, j, 0 );
		int WeftYarnIndex = GetYarnIndex( i, j, 1 );
		int TopWeftYarnIndex = GetYarnIndex( i, j, iMaxIndex-1 );

		if ( BinderYarnIndex == -1 || WeftYarnIndex == -1 )
			return;

		AdjustBinderYarnSection( i, j, BinderYarnIndex, WeftYarnIndex, TopWeftYarnIndex, BOTTOM );
	}
}

void CTextileOrthogonal::AdjustBinderYarnSection( int i, int j, int BinderYarnIndex, int WeftYarnIndex, int OppositeWeftYarnIndex, bool IsTop ) const
{
	XYZ WeftNode = m_Yarns[WeftYarnIndex].GetNode( j )->GetPosition();
	XYZ BinderNode = m_Yarns[BinderYarnIndex].GetNode( i )->GetPosition();
	XYZ OppositeWeftNode = m_Yarns[OppositeWeftYarnIndex].GetNode( j )->GetPosition();

	// Get cross section of weft yarn 
	CSection* WeftYarnSection = GetCrossSection( WeftYarnIndex, j );
	string Type = WeftYarnSection->GetType();
	
	if ( Type != "CSectionHybrid" )
	{
		delete WeftYarnSection;
		return;
	}
	double WeftHeight = GetHybridHeight( (CSectionHybrid*)WeftYarnSection );

	CSection* OppositeWeftSection = GetCrossSection( OppositeWeftYarnIndex, j );
	Type = WeftYarnSection->GetType();
	if ( Type != "CSectionHybrid" )
	{
		delete OppositeWeftSection;
		return;
	}
	double OppositeWeftHeight = GetHybridHeight( (CSectionHybrid*)OppositeWeftSection );
	double Height;
	if ( IsTop )
	{
		Height = WeftNode.z - OppositeWeftNode.z;
	}
	else
	{
		Height = OppositeWeftNode.z - WeftNode.z;
	}

	double MidSectionHeight = Height;
	Height = MidSectionHeight/2.0 + WeftHeight;   // Add whole height as node moved to flat surface of hybrid
	double BinderTargetHeight = m_dFabricThickness/2.0 - Height; 
	
	CSection* BinderYarnSection = GetCrossSection( BinderYarnIndex, i );
	CSectionPowerEllipse* YarnSection = (CSectionPowerEllipse*)BinderYarnSection->Copy();

	double FibreArea = m_Yarns[BinderYarnIndex].GetFibreArea("mm^2");
	double InitialArea = YarnSection->GetArea(YarnSection->GetPoints(40));
	if ( FibreArea/InitialArea > m_dMaxVolumeFraction )
	{
		TGERROR( "Binder yarn initial volume fraction exceeds max Vf Yarn: " << BinderYarnIndex );
		delete YarnSection;
		delete WeftYarnSection;
		return;
	}

	if ( fabs(m_XYarnData[j].dHeight - BinderTargetHeight) < TOL || m_XYarnData[j].dHeight <= BinderTargetHeight ) // Enough space for binder yarn so return
	{
		delete WeftYarnSection;
		return;
	}

	
	double TargetArea = InitialArea * 2.0; // *2.0 because only using half in hybrid section
	
	// Create new power ellipse section with more lenticular shape - this is an assumption - may need better method of selecting appropriate profile
	// Create new section with target height
	CSectionPowerEllipse PowerEllipseSection(YarnSection->GetWidth(), BinderTargetHeight*2.0, 1.7 );
	double PowerEllipseArea = PowerEllipseSection.GetArea( PowerEllipseSection.GetPoints(40));
	double dCrimp = 0.0;
	bool bCorrectCrimpInterference = false;

	if ( PowerEllipseArea < TargetArea )  // New section has smaller cross-section than original so need to adjust width
	{
		double WidthHeightReduction = 1.0;
		// Make assumption about max width for the moment;
		double MaxWidth = PowerEllipseSection.GetWidth() * 2.25;
		if ( !AdjustPowerEllipseSectionWidth( WidthHeightReduction, MaxWidth, TargetArea, &PowerEllipseSection ) )
		{
			// Increased to max width without achieving target area.  Check if reduced area gives ok Vf
			PowerEllipseArea = PowerEllipseSection.GetArea( PowerEllipseSection.GetPoints(40));
			double VolumeFraction = FibreArea/(PowerEllipseArea/2.0);
			if ( VolumeFraction > m_dMaxVolumeFraction )
			{
				// New section Vf too high so increase height until achieve max Vf
				double MinArea = (FibreArea/m_dMaxVolumeFraction) * 2.0;
				// Send undeformed height of yarn as max height
				FindPowerEllipseSectionHeight( m_XYarnData[j].dHeight, MinArea, &PowerEllipseSection );

				// Have now increased binder height over available space so need to check if weft was at max Vf. 
				// If not then room to compress weft further
				double WeftArea = WeftYarnSection->GetArea( WeftYarnSection->GetPoints(40));
				double WeftFibreArea = m_Yarns[WeftYarnIndex].GetFibreArea("mm^2");
				double WeftVolumeFraction = WeftFibreArea/WeftArea;
				if ( fabs(WeftVolumeFraction- m_dMaxVolumeFraction) > TOL && WeftVolumeFraction < m_dMaxVolumeFraction )  // Weft not at max volume fraction so room to compress further
				{
					CSectionPowerEllipse* WeftPowerEllipse = (CSectionPowerEllipse*)((CSectionHybrid*)WeftYarnSection)->GetSection( IsTop?0:1 ).Copy();
					CSectionEllipse* WeftEllipse = (CSectionEllipse*)((CSectionHybrid*)WeftYarnSection)->GetSection( IsTop?1:0 ).Copy();
					CSectionPowerEllipse* MinWeftPowerEllipse = (CSectionPowerEllipse*)(WeftPowerEllipse->Copy());

					// Find height of power ellipse with max Vf.  Set initial height so that less than start height
					MinWeftPowerEllipse->SetHeight( MinWeftPowerEllipse->GetHeight()/2.0 );  
					// Find height available for weft yarn now binder yarn height has increased
				//	double TargetWeftHeightOld = m_YYarnData[i].dHeight - PowerEllipseSection.GetHeight()/2.0 - 1.5*m_dGapSize;

					double TargetWeftHeight = (m_dFabricThickness - MidSectionHeight)/2.0 - PowerEllipseSection.GetHeight()/2.0 - m_dGapSize;
					double TargetWeftArea = (WeftFibreArea * 2.0 )/ m_dMaxVolumeFraction;  // Multiply by 2 because calculating for whole power ellipse
					double MaxHeight = WeftHeight * 2.0;                                  // not the half used in the actual section
					FindPowerEllipseSectionHeight( MaxHeight, TargetWeftArea, MinWeftPowerEllipse );

					if ( TargetWeftHeight >= MinWeftPowerEllipse->GetHeight()/2.0 ) // Is max Vf weft height less than required height? If yes then can set weft to required height
					{
						WeftPowerEllipse->SetHeight( TargetWeftHeight * 2.0 );
						if ( IsTop )
						{
							ChangeWeftSection( *WeftPowerEllipse, *WeftEllipse, WeftYarnIndex, j, 0.0 );
						}
						else
						{
							ChangeWeftSection( *WeftEllipse, *WeftPowerEllipse, WeftYarnIndex, j, 0.0 );
						}
						WeftArea = WeftPowerEllipse->GetArea( WeftPowerEllipse->GetPoints(40) ) / 2.0;
					}

					delete WeftPowerEllipse;
					delete WeftEllipse;
					delete MinWeftPowerEllipse;
				}
				WeftVolumeFraction = WeftFibreArea/WeftArea;
				
				if ( fabs(WeftVolumeFraction - m_dMaxVolumeFraction) < TOL || WeftVolumeFraction >= m_dMaxVolumeFraction ) 
				{
					// Weft and binder now both at max Vf
					TGERROR("Can't reduce outer weft and binder to fit without exceeding max Vf: Binder Yarn " << BinderYarnIndex << "Weft Yarn " << WeftYarnIndex );
					// If yes, return with error - can't achieve this thickness without exceeding max Vf
					// OR find combined height of binder & weft at max Vf and introduce crimp to acommodate excess height => need change in next warp layer
					double dBinderWeftTargetHeight = (m_dFabricThickness - MidSectionHeight)/2.0;
					dCrimp = WeftHeight + PowerEllipseSection.GetHeight()/2.0 - dBinderWeftTargetHeight;
					// Check to see if opposite weft yarn filled all of space available
					// If not move stack by smaller of dCrimp and space available
					
					Height = MidSectionHeight/2.0 + OppositeWeftHeight;   
					double dOppositeSpace = m_dFabricThickness/2.0 - Height;
					double dOffset;
					if ( dCrimp > 0.0 )
					{
						if ( dOppositeSpace < TOL )  // No space to move in z-direction, just correct interference
						{
							bCorrectCrimpInterference = true;
						}
						else
						{
							if ( dCrimp < dOppositeSpace )
							{
								dOffset = dCrimp;
							}
							else
							{
								dOffset = dOppositeSpace;
								bCorrectCrimpInterference = true;
							}
							const vector<PATTERN3D> &Cell = GetCell(i,j);
							int iMaxIndex = (int)Cell.size() - 1;
							if ( IsTop )
							{
								for ( int k = 1; k < iMaxIndex-1; ++k )
								{
									OffsetYarn( i, j, k, -dOffset, Cell[k] );
								}
							}
							else
							{
								for ( int k = 2; k < iMaxIndex; ++k )
								{
									OffsetYarn( i, j, k, dOffset, Cell[k] );
								}
							}
						}
					}
				}				
			}
		}
	}
	else if ( PowerEllipseArea > TargetArea )
	{
		// New section gives larger area => Vf has decreased at target height
		// Is this a fair representation?  What's the weft yarn Vf? If this has increased to max Vf then might be that should
		// find midway where binder & weft Vf changes by similar amount?
		//ReducePowerEllipseHeight( TargetArea, &PowerEllipseSection );
		TGLOG( "New binder power ellipse area > target area => Vf decreased");
	}
	CSectionEllipse EllipseSection( PowerEllipseSection.GetWidth(), 0.0 );

	// Work out offset for binder node			
	XYZ Node = m_Yarns[BinderYarnIndex].GetNode(i)->GetPosition();
	double Offset;
	Offset = (-(m_XYarnData[j].dHeight - PowerEllipseSection.GetHeight()/2.0)/2.0) + PowerEllipseSection.GetHeight()/4.0 - dCrimp;
	//Offset = PowerEllipseSection.GetHeight()/2.0 - m_XYarnData[j].dHeight;
		
	if ( IsTop )
	{
		ChangeBinderSection( EllipseSection, PowerEllipseSection, BinderYarnIndex, i, Offset );
		WeftNode.z -= dCrimp;
	}
	else
	{
		ChangeBinderSection( PowerEllipseSection, EllipseSection, BinderYarnIndex, i, -Offset );
		WeftNode.z += dCrimp;
	}
	m_Yarns[WeftYarnIndex].ReplaceNode( j, WeftNode );

	//if ( dCrimp > 0.0 )
	if ( bCorrectCrimpInterference )
		CorrectCrimpInterference( i, j, WeftYarnIndex, IsTop );

	delete WeftYarnSection;
	delete BinderYarnSection;
	delete YarnSection;
}

void CTextileOrthogonal::ChangeBinderSection( CSection& TopHalf, CSection& BottomHalf, int YarnIndex, int Node, double Offset) const
{
	//PROFILE_FUNC();
	CSectionHybrid Section( TopHalf, BottomHalf );
	CYarnSectionInterpNode* pYarnSections = (CYarnSectionInterpNode*)m_Yarns[YarnIndex].GetYarnSection()->Copy();

	pYarnSections->ReplaceSection( Node, Section );
	XYZ NodePos = m_Yarns[YarnIndex].GetNode(Node)->GetPosition();
	NodePos.z += Offset;
	m_Yarns[YarnIndex].ReplaceNode( Node, CNode(NodePos) );

	if ( Node == 0 )
	{
		NodePos = m_Yarns[YarnIndex].GetNode(m_iNumYYarns)->GetPosition();
		NodePos.z += Offset;
		m_Yarns[YarnIndex].ReplaceNode( m_iNumYYarns, CNode(NodePos) );
		pYarnSections->ReplaceSection( m_iNumYYarns, Section );
	}
	m_Yarns[YarnIndex].AssignSection( *pYarnSections );
	delete pYarnSections;
}

double CTextileOrthogonal::GetHybridHeight( CSectionHybrid* Section ) const
{
	XY TopSectionPoint, BottomSectionPoint;
	TopSectionPoint = Section->GetPoint(0.25);
	BottomSectionPoint = Section->GetPoint(0.75);
	return TopSectionPoint.y - BottomSectionPoint.y;
}

double CTextileOrthogonal::GetHybridWidth( CSectionHybrid* Section ) const
{
	XY LeftPoint, RightPoint;
	LeftPoint = Section->GetPoint(0.5);
	RightPoint = Section->GetPoint(0.0);
	return RightPoint.x - LeftPoint.x;
}

void CTextileOrthogonal::ReplaceYarnSection( int YarnIndex, int Node, CSection& Section ) const
{
	CYarnSectionInterpNode* pInterpNodeSection = (CYarnSectionInterpNode*)m_Yarns[YarnIndex].GetYarnSection()->Copy();
	pInterpNodeSection->ReplaceSection( Node, Section );
	m_Yarns[YarnIndex].AssignSection(*pInterpNodeSection);
	delete pInterpNodeSection;
}

void CTextileOrthogonal::ReplaceThroughBinderSection( int BinderYarnIndex, int Node, CSection& Section, map<pair<int, int>, YARNDATA>::iterator& itBinderData ) const
{
	if ( itBinderData != m_BinderData.end() && itBinderData->second.dWidth >= (itBinderData->second.dSpacing-TOL) )
	{
		CSectionPowerEllipse ThroughBinder( itBinderData->second.dWidth, itBinderData->second.dHeight, 0.0 );
		ReplaceYarnSection( BinderYarnIndex, Node, ThroughBinder );
	}
	else
	{
		ReplaceYarnSection( BinderYarnIndex, Node, Section );
	}
}

void CTextileOrthogonal::ChangePowerEllipseSection( double& TargetArea, CSectionPowerEllipse* YarnSection, double FibreArea ) const
{
	double Area = YarnSection->GetArea( YarnSection->GetPoints(40) );
	if ( Area > TargetArea )
		ReducePowerEllipseHeight( TargetArea, YarnSection );
	else if ( Area < TargetArea ) // do nothing if equal
	{
		double NewVolumeFraction = FibreArea/(TargetArea/2.0);
		if ( NewVolumeFraction <= m_dMaxVolumeFraction )
			return;  // Reduced area but volume fraction still ok so just return
		// Need to increase area until volume fraction ok
		double HeightReduction = 1.0;
		AdjustPowerEllipsePower( HeightReduction, TargetArea, YarnSection, 0.1 );
	}
}

int CTextileOrthogonal::FindNextBinderIndex( int StartIndex ) const
{
	int i = StartIndex + 1;
	while ( i < m_iNumXYarns && !IsBinderYarn( i ) )
	{
		i++;
	}
	if ( i < m_iNumXYarns )
		return i;

	if ( StartIndex == 0 )
		return StartIndex;
	i = StartIndex - 1;
	while( i >= 0 && !IsBinderYarn( i ) )
	{
		i--;
	}
	if ( i == -1 )
		return StartIndex;
	return i;
}

XY CTextileOrthogonal::GetSectionPoint( CSection* YarnSection, double t ) const
{
	string WeftType = YarnSection->GetType();
	XY SectionPoint;
	if ( WeftType == "CSectionPowerEllipse" )
	{
		SectionPoint = ((CSectionPowerEllipse*)YarnSection)->GetPoint(t);
	}
	else 
		SectionPoint = ((CSectionHybrid*)YarnSection)->GetPoint(t);

	return SectionPoint;
}

void CTextileOrthogonal::CorrectCrimpInterference( int i, int j, int WeftYarnIndex, bool IsTop) const
{
	
	//int i, j, k;
	const vector<PATTERN3D> &Cell = GetCell(i,j);

	int k;
	CMesh WeftYarnMesh;
	vector<int>::iterator itpYarn;
	vector<pair<int, int> > RepeatLimits;
	vector<pair<double, XYZ> > Intersections;
	XYZ Centre, P;
	const CYarnSection* pYarnSection;
	const CInterpolation* pInterpolation;
	CSlaveNode Node;
	XYZ Side, Up;
	YARN_POSITION_INFORMATION YarnPosInfo;

	RepeatLimits.resize(2, pair<int, int>(-1, 0));
	vector<double> Modifiers;
	vector< vector<double> > YarnSectionModifiers;

	int WarpYarnIndex [2];
	int WarpCellIndex = IsTop ? Cell.size()-3 : 2;
	int WarpPrevIndex = (j+m_iNumXYarns-1)%m_iNumXYarns;
	int WarpNextIndex = (j+1)%m_iNumXYarns;
	WarpYarnIndex[0] = GetYarnIndex( i, WarpPrevIndex, WarpCellIndex );
	WarpYarnIndex[1] = GetYarnIndex( i, WarpNextIndex, WarpCellIndex );
	

	// Find at how much the cross sections need to be compressed to leave given gap size
	WeftYarnMesh.Clear();
	m_Yarns[WeftYarnIndex].AddSurfaceToMesh(WeftYarnMesh, RepeatLimits);

	WeftYarnMesh.Convert3Dto2D();
	WeftYarnMesh.ConvertQuadstoTriangles();

	
	double WeftWidth [2];
	WeftWidth[0] = GetHybridSectionWidth( WarpPrevIndex, WeftYarnIndex );
	WeftWidth[1] = GetHybridSectionWidth( WarpNextIndex, WeftYarnIndex );

	for (k=0; k < 2; ++k)
	{
		YarnPosInfo.iSection = i;
		YarnPosInfo.dSectionPosition = 0;
		YarnPosInfo.SectionLengths = m_Yarns[WarpYarnIndex[k]].GetYarnSectionLengths();

		pInterpolation = m_Yarns[WarpYarnIndex[k]].GetInterpolation();
		Node = pInterpolation->GetNode(m_Yarns[WarpYarnIndex[k]].GetMasterNodes(), i, 0);
		Up = Node.GetUp();
		Side = CrossProduct(Node.GetTangent(), Up);

		pYarnSection = m_Yarns[WarpYarnIndex[k]].GetYarnSection();
		vector<XY> Points = pYarnSection->GetSection(YarnPosInfo, m_Yarns[WarpYarnIndex[k]].GetNumSectionPoints());
		Centre = m_Yarns[WarpYarnIndex[k]].GetMasterNodes()[i].GetPosition();
		vector<XY>::iterator itPoint;
		Modifiers.clear();
		for (itPoint = Points.begin(); itPoint != Points.end(); ++itPoint)
		{
			P = itPoint->x * Side + itPoint->y * Up + Centre;
			if (WeftYarnMesh.IntersectLine(Centre, P, Intersections, make_pair(true, false)))
			{
				double dU = Intersections[0].first;
				XYZ Normal = Intersections[0].second;
				double dProjectedGap = m_dGapSize / DotProduct(Normal, Centre-P);
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
		YarnSectionModifiers.push_back(Modifiers);		
	}
		
	
	
	
	for ( k=0; k<2; ++k )
	{
		vector<XY> Points;
		CYarnSectionInterpNode* pYarnSections = (CYarnSectionInterpNode*)m_Yarns[WarpYarnIndex[k]].GetYarnSection()->Copy();
		
		pYarnSection = m_Yarns[WarpYarnIndex[k]].GetYarnSection();
		YarnPosInfo.SectionLengths = m_Yarns[WarpYarnIndex[k]].GetYarnSectionLengths();
		// Add sections at the warp node
		YarnPosInfo.dSectionPosition = 0;
		YarnPosInfo.iSection = i;

		Points = pYarnSection->GetSection(YarnPosInfo, YarnSectionModifiers[k].size());
		for ( int ind=0; ind<(int)Points.size(); ++ind)
		{
			Points[ind] *= YarnSectionModifiers[k][ind];
		}
		CSectionPolygon Section(Points);
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		pYarnSections->ReplaceSection( i, Section );
		if ( i == 0 )
		{
			pYarnSections->ReplaceSection( m_iNumYYarns, Section );
		}

		// Add mid-sections along warp so that section is the same across the width of the crossing weft yarn
		double u = 0.95 * ( WeftWidth[k]/2.0 ) / YarnPosInfo.SectionLengths[i];  // 0.95 so that sections don't end up in wrong order due to fp errors if butt up to each other
		pYarnSections->InsertSection( i, u, Section );
		int PrevSectionIndex = (i+m_iNumYYarns-1)%m_iNumYYarns;
		u = 1 - 0.95 * ( (WeftWidth[k]/2.0) / YarnPosInfo.SectionLengths[PrevSectionIndex] );
		pYarnSections->InsertSection( PrevSectionIndex, u, Section );

		m_Yarns[WarpYarnIndex[k]].AssignSection(*pYarnSections);
		delete pYarnSections;
	}
}

double CTextileOrthogonal::GetHybridSectionWidth( int WarpIndex, int WeftIndex ) const 
{
	double Width;
	CYarnSectionInterpNode* pYarnSections = (CYarnSectionInterpNode*)m_Yarns[WeftIndex].GetYarnSection()->Copy();
	CSectionHybrid* WeftSection;
	WeftSection = (CSectionHybrid*)pYarnSections->GetNodeSection(WarpIndex).Copy();
	Width = GetHybridWidth( WeftSection );
	delete WeftSection;
	delete pYarnSections;
	return Width;
}

void CTextileOrthogonal::OffsetYarn( int i, int j, int k, double dHeight, int YarnType ) const
{
	int YarnIndex = GetYarnIndex( i, j, k );
	if ( YarnIndex == -1 )
		return;
	XYZ Node;
	if ( YarnType == PATTERN3D_XYARN )
	{
		Node = m_Yarns[YarnIndex].GetNode(i)->GetPosition();
		Node.z += dHeight;
		m_Yarns[YarnIndex].ReplaceNode( i, CNode(Node) );
		if ( i == 0 )
		{
			XYZ LastNode = m_Yarns[YarnIndex].GetNode(m_iNumYYarns)->GetPosition();
			LastNode.z = Node.z;
			m_Yarns[YarnIndex].ReplaceNode( m_iNumYYarns, CNode(LastNode) );
		}
	}
	else if ( YarnType == PATTERN3D_YYARN )
	{
		Node = m_Yarns[YarnIndex].GetNode(j)->GetPosition();
		Node.z += dHeight;
		m_Yarns[YarnIndex].ReplaceNode( j, CNode(Node) );
		if ( j == 0 )
		{
			XYZ LastNode = m_Yarns[YarnIndex].GetNode(m_iNumXYarns)->GetPosition();
			LastNode.z = Node.z;
			m_Yarns[YarnIndex].ReplaceNode( m_iNumXYarns, CNode(LastNode) );
		}
	}
}

void CTextileOrthogonal::ConvertToPatternDraft( int iWeftOrder )
{
	m_PatternDraft.ClearWeavePattern();
	for ( int i = m_iNumYYarns-1; i >=0; --i )
	{
		int iNumWefts = (int)m_YYarns[i].size();
		vector<string> Rows;
		Rows.resize(iNumWefts);

		for ( int j = m_iNumXYarns-1; j >= 0 ; --j )
		{
			vector<PATTERN3D> &Cell = GetCell(i, j);
			if ( NoYarnCell( Cell ) )
				continue;
			vector<bool> bWeftSet(iNumWefts, false);
			int iWeft = iNumWefts-1;
			
			for ( int k = (int)Cell.size()-1; k >= 0; --k )
			{
				switch (Cell[k])
				{
				case PATTERN3D_XYARN:
					for ( int iW = 0; iW < iNumWefts; ++iW )
					{
						if ( !bWeftSet[iW] )
							Rows[iW].push_back('1');
						else
							Rows[iW].push_back('0');   
					}
					break;
				case PATTERN3D_YYARN:
					bWeftSet[iWeft] = true;
					iWeft--;
					break;
				case PATTERN3D_NOYARN:
					break;
				}
			}
		}
		
		if ( iWeftOrder == BOTTOM_TO_TOP || (iWeftOrder == ALTERNATE_WEFT_STACK && i%2))
		{
			// Add wefts from bottom to top
			for ( int iW = iNumWefts-1 ; iW >=0; --iW )
				m_PatternDraft.AddRow( Rows[iW] );
		}
		else
		{
			// Add wefts from top to bottom
			for ( int iW = 0 ; iW < iNumWefts; ++iW )
				m_PatternDraft.AddRow( Rows[iW] );
		}
	}
}

void CTextileOrthogonal::SetupWeftRow( vector<int>& Layers, vector<int>& Warp, int NumWarps, int Weft )
{
	int NumLayers = *(max_element( Layers.begin(), Layers.end() ));  // Size of max warp stack
	m_iTotalXYarns = Layers.size();
	vector<int>::iterator itWarp;
	vector<int>::iterator itLayers;

	if ( Layers.size() != Warp.size() )
		TGERROR("Can't add weft row, warp and layer sizes do not match");

	itWarp = Warp.begin();
	int j = NumWarps-1;
	int PrevYCellIndex = -1;

	// Find weft index
	bool bFound = false;
	itLayers = Layers.begin();
	while ( itLayers != Layers.end() && !bFound )
	{
		vector<PATTERN3D> &Cell = GetCell(Weft, j);
		if ( IsBinderYarn( j ) )
		{
			++itLayers;
			++itWarp;
		}
		else
		{
			bool bFirst = true;  // bFirst is true until the weft yarn has been set for the current cell
			int WarpIndex = -1;
			do
			{
				if ( *itWarp == 0 && bFirst )  // Warp yarn is down and weft not yet set
				{
					PrevYCellIndex = (NumLayers - (*itLayers)+1 )*2 + 1;
					// Set the cell for the warp yarn
					bFirst = false;
					bFound = true;
				}
				++itLayers;
				++itWarp;
			} while ( itLayers != Layers.end() && *itLayers != 1 );  // Contunue until reach end of current stack of layers
			if ( !bFound )  // If reached end of stack without setting weft yarn need to set it at bottom of cell entries
			{
				PrevYCellIndex = (NumLayers - *(itLayers-1))*2 +1;
				bFound = true;
			}
		}
		--j; // Move to next cell

	}

	itWarp = Warp.begin();
	j = NumWarps-1;
	// Left hand side of weave pattern corresponds to maximum y value (or max j in cell array)
	for ( itLayers = Layers.begin(); itLayers != Layers.end(); )
	{
		vector<PATTERN3D> &Cell = GetCell(Weft, j);
		vector<int> &YarnCell = GetYarnCell(Weft, j);
		YarnCell.assign(Cell.size(), -1);
		if ( IsBinderYarn( j ) )
		{
			if ( *itWarp == 0 )
				Cell[0] = PATTERN3D_XYARN;
			else
				Cell[Cell.size()-1] = PATTERN3D_XYARN;
			// Need to get weft layer of next cell to set for this one
			if ( PrevYCellIndex != -1 )  // Still a problem if binder is first warp
			{
				Cell[PrevYCellIndex] = PATTERN3D_YYARN;
				YarnCell[PrevYCellIndex] = Weft;
			}
			++itLayers;
			++itWarp;
		}
		else
		{
			bool bFirst = true;  // bFirst is true until the weft yarn has been set for the current cell
			int WarpIndex = -1;
			do
			{
				if ( *itWarp == 0 && bFirst )  // Warp yarn is down and weft not yet set
				{
					int WeftCellIndex = (NumLayers - (*itLayers)+1 )*2 + 1;
					PrevYCellIndex = WeftCellIndex;
					Cell[WeftCellIndex] = PATTERN3D_YYARN;
					YarnCell[WeftCellIndex] = Weft;
					
					// Set the cell for the warp yarn
					Cell[WeftCellIndex-1] = PATTERN3D_XYARN;
					bFirst = false;
				}
				else if ( *itWarp != PATTERN3D_NOYARN )
				{
					 // Warp is up and no weft yarn yet. Set x yarn cell without space allowed for weft yarn
						WarpIndex = (NumLayers - (*itLayers)+1) *2;
						Cell[WarpIndex] = PATTERN3D_XYARN;
				}
				
				++itLayers;
				++itWarp;
			} while ( itLayers != Layers.end() && *itLayers != 1 );  // Contunue until reach end of current stack of layers
			if ( bFirst && *(itWarp-1) != PATTERN3D_NOYARN )  // If reached end of stack without setting weft yarn need to set it at bottom of cell entries
			{
				int WeftCellIndex = (NumLayers - *(itLayers-1))*2 +1;
				PrevYCellIndex = WeftCellIndex;
				Cell[WeftCellIndex] = PATTERN3D_YYARN;
				//Cell[(NumLayers - *(itLayers-1))*2 +1] = PATTERN3D_YYARN;
				YarnCell[(NumLayers - *(itLayers-1))*2 +1] = Weft;
			}
		}
		--j; // Move to next cell

	}
	for ( j = 1; j < m_iNumXYarns-1; ++j )
	{
		if ( IsBinderYarn(j) )
		{
			vector<PATTERN3D> &Cell = GetCell(Weft, j);
			vector<int> &YarnCell = GetYarnCell(Weft, j);
			vector<PATTERN3D> &PrevCell = GetCell(Weft, j-1);
			vector<PATTERN3D> &NextCell = GetCell(Weft, j+1);
			int WeftHeight = FindWeftHeight( Cell );
			int NextWeftHeight = FindWeftHeight( NextCell );
			int PrevWeftHeight = FindWeftHeight( PrevCell);
			if ( NextWeftHeight != PrevWeftHeight )
			{
				Cell[WeftHeight] = PATTERN3D_NOYARN;
				Cell[PrevWeftHeight] = PATTERN3D_YYARN;
				YarnCell[WeftHeight] = -1;
				YarnCell[PrevWeftHeight] = Weft;
			}
		}
	}
	m_bNeedsBuilding = true;
}

void CTextileOrthogonal::ConsolidateCells()
{
	//bool bWeftChanged = false;
	vector<int> Levels;

	for (int i = 0; i < m_iNumYYarns-1; ++i )
	{
		int j;
		int WeftLevel = -1;
		Levels.clear();
		bool bRemoveCells = true;
		for ( j = 0; j < m_iNumXYarns; ++j )
		{
			
			vector<PATTERN3D> &Cell1 = GetCell(i,j);
			vector<PATTERN3D> &Cell2 = GetCell(i+1,j);

			if ( (Cell1[0] != Cell2[0] || Cell1[Cell1.size()-1] != Cell2[Cell2.size()-1]) ) // Binder yarn changed level - can't consolidate
			{
				break;
			}
			int Level2 = FindWeftHeight( Cell2 );
			if ( Level2 == -1 )
			{
				Levels.push_back(Level2);
				bRemoveCells = false;
				continue;
			}
			if ( Cell1[Level2] != PATTERN3D_NOYARN ) // If no space in first cell at weft level, can't consolidate
			{
				Levels.push_back(-1);
				bRemoveCells = false;
				//break;
				continue;
			}
			else
				Levels.push_back(Level2);
			/*if ( WeftLevel == -1 )
			{
				WeftLevel = Level2;
			}
			else if ( WeftLevel != Level2 )  // Assume at moment that if weft changes layers can't consolidate
			{
				Consolidate
				break;
			}*/
		}
		if ( j == m_iNumXYarns )  // Only consolidate if can move cell at each warp position (ie across whole width)
		{
			if ( bRemoveCells )
			{
				for ( j = m_iNumXYarns-1; j >= 0 ; --j )
				{
					vector<PATTERN3D> &Cell = GetCell(i,j);
					vector<int> &YarnCell = GetYarnCell(i,j);
					vector<int> &NextYarnCell = GetYarnCell(i+1,j);
					//Cell[WeftLevel] = PATTERN3D_YYARN;
					Cell[Levels[j]] = PATTERN3D_YYARN;
					YarnCell[Levels[j]] = NextYarnCell[Levels[j]];
					RemoveCell(i+1,j);
					RemoveYarnCell(i+1,j);

					//LastLevel = WeftLevel;
				}
				m_iNumYYarns--;
				m_YYarnData.erase(m_YYarnData.begin()+(i+1));
				--i;
			}
			else
			{
				for ( j = m_iNumXYarns-1; j >= 0 ; --j )
				{
					vector<PATTERN3D> &Cell = GetCell(i,j);
					vector<PATTERN3D> &Cell1 = GetCell(i+1,j);
					vector<int> &YarnCell = GetYarnCell(i,j);
					vector<int> &YarnCell1 = GetYarnCell(i+1,j);
					if ( Levels[j] != -1 )
					{
						Cell[Levels[j]] = PATTERN3D_YYARN;
						Cell1[Levels[j]] = PATTERN3D_NOYARN;
						YarnCell[Levels[j]] = YarnCell1[Levels[j]];
						YarnCell1[Levels[j]] = -1;
					}
				}
			}
		}
	}

	vector<int> DeleteCell;
	for (int i = 0; i < m_iNumYYarns-1; ++i )
	{
		int j;
		int WeftLevel = -1;
		
		DeleteCell.clear();
		
		for ( j = 0; j < m_iNumXYarns; ++j )
		{
			
			vector<PATTERN3D> &Cell1 = GetCell(i,j);
			vector<PATTERN3D> &Cell2 = GetCell(i+1,j);

			int Weft1 = FindWeftHeight( Cell1 );

			if ( ((Cell1[0] != Cell2[0] || Cell1[Cell1.size()-1] != Cell2[Cell2.size()-1])) ) // Binder yarn changed level 
			{
				if ( Weft1 == -1 )
				{
					DeleteCell.push_back(0);
					continue;
				}
				else
					break;
			}
			int k;
			for ( k = 1; k < Cell2.size(); ++k )
			{
				if ( Cell2[k] == PATTERN3D_YYARN )
				{
					if ( Cell1[k] != PATTERN3D_NOYARN )
						break;
				}
			}
			if ( k != Cell2.size() )
				break;
			DeleteCell.push_back(1);
		}
		if ( j == m_iNumXYarns )
		{
			
			for ( j = m_iNumXYarns-1; j >= 0 ; --j )
			{
				if ( DeleteCell[j] == 0 )
				{
					RemoveCell(i,j);
					RemoveYarnCell(i,j);
				}
				else
				{
					vector<PATTERN3D> &Cell = GetCell(i,j);
					vector<PATTERN3D> &Cell1 = GetCell(i+1,j);
					vector<int> &YarnCell = GetYarnCell(i,j);
					vector<int> &YarnCell1 = GetYarnCell(i+1,j);
					for ( int k = 1; k < Cell1.size(); ++k )
					{
						if ( Cell1[k] == PATTERN3D_YYARN )
						{
							Cell[k] = PATTERN3D_YYARN;
							YarnCell[k] = YarnCell1[k]; 
						}
					}
					RemoveCell(i+1,j);
					RemoveYarnCell(i+1,j);
				}
			}
			m_iNumYYarns--;
			m_YYarnData.erase(m_YYarnData.begin()+(i+1));  // Assumes yarn data is same for both sets of yarns otherwise won't be correct
			--i;
		}
	}

	for ( int j = 0; j < m_iNumXYarns; ++j )
	{
		if ( !IsBinderYarn( j ) )
			continue;
		for (int i = 0; i < m_iNumYYarns; ++i )
		{
			vector<PATTERN3D> &Cell = GetCell(i,j);
			MoveBinderYarnPosition( Cell );
		}
	}
}

void CTextileOrthogonal::MoveBinderYarnPosition( vector<PATTERN3D> &Cell )
{
	if ( Cell[0] == PATTERN3D_XYARN )  //  Binder at bottom. Track up through cell until
	{
		int i = 1;
		if ( Cell[1] != PATTERN3D_NOYARN )  // No space directly over binder - don't need to move
			return;
		while ( i < Cell.size() && Cell[i] == PATTERN3D_NOYARN )
		{
			++i;
		}
		if ( i < Cell.size() )
		{
			Cell[i-1] = PATTERN3D_XYARN;
			Cell[0] = PATTERN3D_NOYARN;
		}
	}
	else if ( Cell[Cell.size()-1] == PATTERN3D_XYARN )
	{
		int i = Cell.size()-2;
		if ( Cell[i] != PATTERN3D_NOYARN )  // No space directly under binder - don't need to move
			return;
		while ( i >= 0 && Cell[i] == PATTERN3D_NOYARN )
		{
			--i;
		}
		if ( i >= 0 )
		{
			Cell[i+1] = PATTERN3D_XYARN;
			Cell[Cell.size()-1] = PATTERN3D_NOYARN;
		}
	}
}

int CTextileOrthogonal::FindBinderHeight( const vector<PATTERN3D>& Cell ) const
{
	int i = Cell.size() - 1;
	while( i > 0 )
	{
		if ( Cell[i] == PATTERN3D_XYARN )
			return i;
		--i;
	}
	return i;
}

vector<int> &CTextileOrthogonal::GetYarnCell(int x, int y)
{
	return m_WeftYarns[x + m_iNumYYarns*y];
}

const vector<int> &CTextileOrthogonal::GetYarnCell(int x, int y) const
{
	return m_WeftYarns[x + m_iNumYYarns*y];
}

void CTextileOrthogonal::RemoveYarnCell(int x, int y)
{
	m_WeftYarns.erase(m_WeftYarns.begin()+(x + m_iNumYYarns*y));
}

int CTextileOrthogonal::GetWeavePatternYarnIndex(int x, int y, int z) const
{
	int iXYarnCount = 0;
	
	vector<PATTERN3D> Pattern = GetCell(x, y);
	vector<int> YarnCell = GetYarnCell(x, y);
	int k;
	if ( Pattern[z] == PATTERN3D_NOYARN )
		return -1;

	if ( Pattern[z] == PATTERN3D_YYARN )
	{
		return YarnCell[z]+m_iYYarnOffset;
	}
	for (k=0; k<z; ++k)
	{
		if (Pattern[k] == PATTERN3D_XYARN)
			++iXYarnCount;
	}
	
	return m_XYarns[y][iXYarnCount];
	
}