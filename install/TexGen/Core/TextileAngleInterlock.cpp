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
#include "TextileAngleInterlock.h"


using namespace TexGen;

CTextileAngleInterlock::CTextileAngleInterlock(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing, double dXHeight, double dYHeight)
: CTextile3DWeave( iNumXYarns, iNumYYarns, dXSpacing, dYSpacing, dXHeight, dYHeight)
{
}

CTextileAngleInterlock::CTextileAngleInterlock(TiXmlElement &Element)
: CTextile3DWeave(Element)
{
}

CTextileAngleInterlock::~CTextileAngleInterlock()
{
}

void CTextileAngleInterlock::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextile3DWeave::PopulateTiXmlElement(Element, OutputType);
}

void CTextileAngleInterlock::SetBinderYarnPositions( int x, int y )
{
	if (x<0 || x>=m_iNumYYarns || y<0 || y>=m_iNumXYarns)
	{
		TGERROR("Unable to set binder position, index out of range: " << x << ", " << y);
		return;
	}
	vector<PATTERN3D> &Cell = GetCell(x, y);
	int size = Cell.size();
	SetBinderPosition( size-1, Cell );
	int BinderPos = (size-1) - 2;

	for ( int i = 1; i <= m_iNumYYarns/2; ++i )
	{
		vector<PATTERN3D> &Cell = GetCell( (i+x)%m_iNumYYarns, y );
		SetBinderPosition( BinderPos, Cell );
		if ( BinderPos > 0 )
		{
			vector<PATTERN3D> &Cell = GetCell( (m_iNumYYarns-i+x)%m_iNumYYarns, y );
			SetBinderPosition( BinderPos, Cell );
		}
		BinderPos -= 2;
	}
	
	m_bNeedsBuilding = true;
}

void CTextileAngleInterlock::SetBinderPosition( int z, vector<PATTERN3D>& Cell )
{
	int size = Cell.size();
	for ( int j = 0; j < size; j+=2 )
	{
		if ( j == z )
		{
			Cell[j] = PATTERN3D_XYARN;
		}
		else
		{
			Cell[j] = PATTERN3D_NOYARN;
		}
	}
}

void CTextileAngleInterlock::SetupLayers( int iNumWarpLayers, int iNumWeftLayers, int iNumBinderLayers )
{
	if ( iNumWarpLayers != iNumWeftLayers-1 )
	{
		TGERROR("Unable to set up layers, number of warp layers should be one less than weft layers: warp layers " << iNumWarpLayers << ", weft layers " << iNumWeftLayers);
		return;
	}
	AddNoYarnLayer();

	// Add alternating layers
	while( iNumWeftLayers > 0 )
	{
		AddYLayers();
		if ( iNumWarpLayers > 0 )
		{
			AddWarpLayer();
			iNumWarpLayers--;
		}
		iNumWeftLayers--;
	}

	AddBinderLayer();
}

bool CTextileAngleInterlock::BuildTextile() const
{
	m_Yarns.clear();
	m_YYarns.clear();
	m_XYarns.clear();

	m_YYarns.resize(m_iNumYYarns);
	m_XYarns.resize(m_iNumXYarns);

	m_dMinZ = 0.0;
	m_dMaxZ = 0.0;

	if (!Valid() || !CheckCells())
	{
		TGERROR("Cannot build textile - incorrect yarn setup");
		return false;
	}

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
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex%m_iNumXYarns );
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
					if ( IsBinderYarn(i) )
					{
						if ( k == 0 )
						{
							z -= dHalfHeight + m_dGapSize;
							if ( (z - dHalfHeight) < m_dMinZ )
								m_dMinZ = z - dHalfHeight;
						}
						else
						{
							if ( NextCell[k] == PATTERN3D_XYARN )
							{
								dHalfHeight = m_XYarnData[NextCellIndex].dHeight / 2.0;
							}
							else if ( NextCell[k] == PATTERN3D_YYARN )
							{
								dHalfHeight = m_YYarnData[j%m_iNumYYarns].dHeight / 2.0;
							}
							else // PATTERN3D_NOYARN
							{
								// Does this ever happen?
							}
							z += dHalfHeight;
						}
					}
					else
					{
						z += dHalfHeight;
					}
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(1, 0, 0)));
					++iYarn;
					z += dHalfHeight + m_dGapSize;
					if ( z > m_dMaxZ )
						m_dMaxZ = z;
				}
				else if ( Cell[k] == PATTERN3D_YYARN )
				{
					z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
				}
				else if ( k > 0 )// PATTERN3D_NOYARN and not on bottom binder layer
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
						// Will get here if all x yarns are binder yarns so just use binder yarn height to give spacing
						z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
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
			const vector<PATTERN3D> &NextCell = GetCell(j%m_iNumYYarns, NextCellIndex%m_iNumXYarns);
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
			
			for (k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_YYARN)
				{
					double dHalfHeight = m_YYarnData[j].dHeight / 2.0;
					z += dHalfHeight;
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z), XYZ(0, 1, 0)));
					++iYarn;
					z += dHalfHeight + m_dGapSize;
				}
				else if ( Cell[k] == PATTERN3D_XYARN && k > 0 ) // Don't adjust z if it's the bottom binder yarn
				{
					if ( IsBinderYarn(i%m_iNumXYarns) )
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
							// Does this ever happen?
						}
					}
					else
					{
						z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
					}
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
						// Will get here if all x yarns are binder yarns so just use binder yarn height to give spacing
						z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
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
		CSectionPowerEllipse Section(dWidth, dHeight, IsBinderYarn(i) ? m_dBinderPower : m_dWarpPower);
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
		CSectionPowerEllipse Section(dWidth, dHeight, m_dWeftPower);
		if (m_pSectionMesh)
			Section.AssignSectionMesh(*m_pSectionMesh);
		for (itpYarn = m_YYarns[i].begin(); itpYarn != m_YYarns[i].end(); ++itpYarn)
		{
			m_Yarns[*itpYarn].AssignSection(CYarnSectionConstant(Section));
		}
	}

	ShapeBinderYarns();

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

void CTextileAngleInterlock::ShapeBinderYarns() const
{
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

int CTextileAngleInterlock::AddBinderNodes( int CurrentNode, int i, int j ) const
{
	const vector<PATTERN3D> &Cell = GetCell(i,j);
	const vector<PATTERN3D> &NextCell = GetCell( (i+1)%m_iNumYYarns, j );
	const vector<PATTERN3D> &PrevCell = GetCell( (i+m_iNumYYarns-1)%m_iNumYYarns, j );
	
	int iIndex = FindBinderHeight( Cell );
	int iMaxIndex = Cell.size() - 1;
	XY SectionPoint;
	XYZ WeftBelowNode, WeftAboveNode;
	int iStartNode = CurrentNode;

	if ( iIndex < 0 )
		return CurrentNode;

	double dBinderOffset = m_XYarnData[j].dHeight/2.0;

	
	int BinderYarnIndex = GetYarnIndex( i, j, iIndex );
	if ( BinderYarnIndex == -1 )
		return CurrentNode;

	int WeftAboveIndex, WeftBelowIndex;

	// Get indices of weft yarns above and below binder
	if ( iIndex < (int)Cell.size() - 1 )
	{
		WeftAboveIndex = GetYarnIndex( i, j, iIndex + 1 );
		WeftAboveNode = m_Yarns[WeftAboveIndex].GetNode( j )->GetPosition();
	}
	else
		WeftAboveIndex = -1;

	if ( iIndex > 0 )
	{
		WeftBelowIndex = GetYarnIndex( i, j, iIndex - 1 );
		WeftBelowNode = m_Yarns[WeftBelowIndex].GetNode( j )->GetPosition();
	}
	else
		WeftBelowIndex = -1;

	XYZ BinderNode = m_Yarns[BinderYarnIndex].GetNode( CurrentNode )->GetPosition();
	if ( iIndex > 0 && PrevCell[iIndex-2] == PATTERN3D_XYARN )
	{
		// Get cross section of weft yarn below
		CSectionPowerEllipse* YarnSection = GetWeftCrossSection( WeftBelowIndex );
		if ( YarnSection == NULL )
			return CurrentNode;

		// Insert points around Top left quadrant of weft yarn
		{
			XYZ NewNode = WeftBelowNode;

			InsertBinderNode( YarnSection, 0.45, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
			InsertBinderNode( YarnSection, 0.4, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
			if ( !( iIndex < iMaxIndex && NextCell[iIndex+2] == PATTERN3D_XYARN ) )
				InsertBinderNode( YarnSection, 0.35, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
			//InsertBinderNode( YarnSection, 0.3, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
		}
		delete YarnSection;
	}
	else if ( iIndex < iMaxIndex && PrevCell[iIndex+2] == PATTERN3D_XYARN )
	{
		// Get cross section of weft yarn below
		CSectionPowerEllipse* YarnSection = GetWeftCrossSection( WeftAboveIndex );
		if ( YarnSection == NULL )
			return CurrentNode;
		// Insert points around lower left quadrant of weft yarn
		{
			XYZ NewNode = WeftAboveNode;

			NewNode = WeftAboveNode;
			InsertBinderNode( YarnSection, 0.55, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			InsertBinderNode( YarnSection, 0.6, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			if ( !( iIndex > 0 && NextCell[iIndex-2] == PATTERN3D_XYARN ) )
				InsertBinderNode( YarnSection, 0.65, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			//InsertBinderNode( YarnSection, 0.7, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
		}
	}

	if ( iStartNode == 0 )
	{
		XYZ NewNode = m_Yarns[BinderYarnIndex].GetNode(0)->GetPosition();
		ReplaceLastNode( BinderYarnIndex, NewNode, BinderNode );
	}

	if ( iIndex > 0 && NextCell[iIndex-2] == PATTERN3D_XYARN )
	{
		
		// Get cross section of weft yarn below
		CSectionPowerEllipse* YarnSection = GetWeftCrossSection( WeftBelowIndex );
		if ( YarnSection == NULL )
			return CurrentNode;

		// Insert points around upper right quadrant of weft
		XYZ NewNode = WeftBelowNode;
		CurrentNode++; // Need to insert after node (ie before next node)
		//InsertBinderNode( YarnSection, 0.2, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
		if ( !( iIndex < iMaxIndex && PrevCell[iIndex+2] == PATTERN3D_XYARN ) )
			InsertBinderNode( YarnSection, 0.15, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
		InsertBinderNode( YarnSection, 0.1, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
		InsertBinderNode( YarnSection, 0.05, WeftBelowNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
		CurrentNode--;
	}
	else if( iIndex < iMaxIndex && NextCell[iIndex+2] == PATTERN3D_XYARN )
	{
		// Get cross section of weft yarn below
		CSectionPowerEllipse* YarnSection = GetWeftCrossSection( WeftAboveIndex );
		if ( YarnSection == NULL )
			return CurrentNode;
		// Insert points around lower right quadrant of weft yarn
		{
			XYZ NewNode = WeftAboveNode;
			CurrentNode++;  // Need to insert after node (ie before next node)
			//InsertBinderNode( YarnSection, 0.8, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			if ( !( iIndex > 0 && PrevCell[iIndex-2] == PATTERN3D_XYARN ) )
				InsertBinderNode( YarnSection, 0.85, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			InsertBinderNode( YarnSection, 0.9, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			InsertBinderNode( YarnSection, 0.95, WeftAboveNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			CurrentNode--;
		}
	}
	
	return CurrentNode;
}

int CTextileAngleInterlock::FindBinderHeight( const vector<PATTERN3D>& Cell ) const
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
