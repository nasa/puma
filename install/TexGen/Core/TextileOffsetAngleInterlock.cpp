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
#include "TextileOffsetAngleInterlock.h"


using namespace TexGen;

CTextileOffsetAngleInterlock::CTextileOffsetAngleInterlock(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing, double dXHeight, double dYHeight)
: CTextile3DWeave( iNumXYarns, iNumYYarns*2, dXSpacing, dYSpacing, dXHeight, dYHeight)
{
}

CTextileOffsetAngleInterlock::CTextileOffsetAngleInterlock(TiXmlElement &Element)
: CTextile3DWeave(Element)
{
}

CTextileOffsetAngleInterlock::~CTextileOffsetAngleInterlock()
{
}

void CTextileOffsetAngleInterlock::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextile3DWeave::PopulateTiXmlElement(Element, OutputType);
}

void CTextileOffsetAngleInterlock::SetBinderYarnPositions( int x, int y )
{
	if (x<0 || x>=m_iNumYYarns || y<0 || y>=m_iNumXYarns)
	{
		TGERROR("Unable to set binder position, index out of range: " << x << ", " << y);
		return;
	}

	if ( x % 2 )
	{
		TGERROR("Binder positions may only be set on top layer yarns");
		return;
	}
	vector<PATTERN3D> &Cell = GetCell(x, y);
	int size = (int)Cell.size();
	SetBinderPosition( size-1, Cell );
	int BinderPos = (size-1) - 1;

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
		if (BinderPos < 0 )
			BinderPos = 0;
	}
	
	m_bNeedsBuilding = true;
}

void CTextileOffsetAngleInterlock::SetBinderPosition( int z, vector<PATTERN3D>& Cell )
{
	int size = (int)Cell.size();
	for ( int j = 0; j < size; j++ )
	{
		if ( j == z )
		{
			Cell[j] = PATTERN3D_XYARN;
		}
		else if ( Cell[j] != PATTERN3D_YYARN )
		{
			Cell[j] = PATTERN3D_NOYARN;
		}
	}
}

void CTextileOffsetAngleInterlock::AddYLayer(bool bOffset)
{
	for ( int i=0; i < m_iNumYYarns; ++i )
	{
		if ( bOffset )
		{
			AddYYarn( i, PATTERN3D_NOYARN );
			++i;
			AddYYarn( i, PATTERN3D_YYARN );
		}
		else
		{
			AddYYarn( i, PATTERN3D_YYARN );
			++i;
			AddYYarn( i, PATTERN3D_NOYARN );
		}
	}
}

void CTextileOffsetAngleInterlock::FindMinMaxZ( double &dMinZ, double &dMaxZ )
{
	dMinZ = dMaxZ = 0.0;
	for (int i=0; i<m_iNumXYarns; ++i)
	{
		for (int j=0; j<=m_iNumYYarns; ++j)
		{
			const vector<PATTERN3D> &Cell = GetCell(j%m_iNumYYarns, i);

			int NextYCellIndex, NextXCellIndex;
			NextYCellIndex = (j+1)%m_iNumYYarns;
			
			const vector<PATTERN3D> &NextYCell = GetCell(NextYCellIndex, i%m_iNumXYarns );
			NextXCellIndex = FindNextCellIndex(i);
		
			double z = 0.0;
			for (int k=0; k<(int)Cell.size(); ++k)
			{
				if (Cell[k] == PATTERN3D_XYARN)
				{
					double dHalfHeight = m_XYarnData[i].dHeight / 2.0;
					if ( k == 0 && IsBinderYarn(i) )
					{
						z -= dHalfHeight + m_dGapSize;
						if ( (z - dHalfHeight) < dMinZ )
							dMinZ = z - dHalfHeight;
					}
					else
						z += dHalfHeight;
					
					z += dHalfHeight + m_dGapSize;
					
				}
				else if ( Cell[k] == PATTERN3D_YYARN )
				{
					z += m_YYarnData[j%m_iNumYYarns].dHeight + m_dGapSize;
				}
				else if ( k > 0 )// PATTERN3D_NOYARN and not on bottom binder layer
				{
	
					if ( NextYCell[k] == PATTERN3D_XYARN )
					{
						z += m_XYarnData[i].dHeight + m_dGapSize;
					}
					else if ( NextYCell[k] == PATTERN3D_YYARN )
					{
						z += m_YYarnData[NextYCellIndex].dHeight + m_dGapSize;
					}
					else // PATTERN3D_NOYARN
					{
						if ( k != (int)Cell.size()-1 )  // If top binder layer don't want to add x yarn thickness
							z += m_XYarnData[NextXCellIndex].dHeight + m_dGapSize;
					}
				}
				if ( z > dMaxZ )
					dMaxZ = z;
			}
		}
	}
}

void CTextileOffsetAngleInterlock::SetupLayers( int iNumWarpLayers, int iNumWeftLayers, int iNumBinderLayers )
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
		AddYLayer( iNumWeftLayers % 2 ? 0 : 1 );  // Offset even layers
		if ( iNumWarpLayers > 0 )
		{
			AddWarpLayer();
			iNumWarpLayers--;
		}
		iNumWeftLayers--;
	}

	AddBinderLayer();
}

bool CTextileOffsetAngleInterlock::BuildTextile() const
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

			int NextYCellIndex, NextXCellIndex;
			NextYCellIndex = (j+1)%m_iNumYYarns;
			
			const vector<PATTERN3D> &NextYCell = GetCell(NextYCellIndex, i%m_iNumXYarns );
			NextXCellIndex = FindNextCellIndex(i);
			
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
					if ( k == 0 && IsBinderYarn(i) )  // Bottom binder yarn
					{
						z -= dHalfHeight + m_dGapSize;
						if ( (z - dHalfHeight) < m_dMinZ )
							m_dMinZ = z - dHalfHeight;
					}
					else if ( k < (int)Cell.size()-1 && IsBinderYarn(i) )  // Binder yarn within mid-textile
					{
						dHalfHeight = m_YYarnData[NextYCellIndex].dHeight / 2.0;
						z += dHalfHeight;
					}
					else
						z += dHalfHeight;
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z)));
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
					if ( NextYCell[k] == PATTERN3D_XYARN )
					{
						z += m_XYarnData[i].dHeight + m_dGapSize;
					}
					else if ( NextYCell[k] == PATTERN3D_YYARN )
					{
						z += m_YYarnData[NextYCellIndex].dHeight + m_dGapSize;
					}
					else // PATTERN3D_NOYARN
					{
						z += m_XYarnData[NextXCellIndex].dHeight + m_dGapSize;
					}
				}
			}
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
			
			int NextYCellIndex, NextXCellIndex;
			NextYCellIndex = (j+1)%m_iNumYYarns;
			
			const vector<PATTERN3D> &NextYCell = GetCell(NextYCellIndex, i%m_iNumXYarns );
			NextXCellIndex = FindNextCellIndex(i);
			
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
					m_Yarns[Yarns[iYarn]].AddNode(CNode(XYZ(x, y, z)));
					++iYarn;
					z += dHalfHeight + m_dGapSize;
				}
				else if ( Cell[k] == PATTERN3D_XYARN && k > 0 ) // Don't adjust z if it's the bottom binder yarn
				{
					if ( k < (int)Cell.size()-1 && IsBinderYarn(i%m_iNumXYarns) )  // If binder yarn want to add height of surrounding weft yarns
					{
						z += m_YYarnData[NextYCellIndex].dHeight + m_dGapSize;
					}
					else
					{
						z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
					}
				}
				else if ( k > 0 ) // PATTERN3D_NOYARN and not on bottom binder layer
				{
					if ( NextYCell[k] == PATTERN3D_XYARN )
					{
						z += m_XYarnData[i%m_iNumXYarns].dHeight + m_dGapSize;
					}
					else if ( NextYCell[k] == PATTERN3D_YYARN )
					{
						z += m_YYarnData[NextYCellIndex].dHeight + m_dGapSize;
					}
					else // PATTERN3D_NOYARN
					{
						z += m_XYarnData[NextXCellIndex%m_iNumXYarns].dHeight + m_dGapSize;
					}
				}
			}
			if (i<m_iNumXYarns)
				y += m_XYarnData[i].dSpacing/2.0;
		}
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
		CSectionPowerEllipse Section(dWidth, dHeight, m_dWeftPower );
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

void CTextileOffsetAngleInterlock::ShapeBinderYarns() const
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

int CTextileOffsetAngleInterlock::AddBinderNodes( int CurrentNode, int i, int j ) const
{
	const vector<PATTERN3D> &Cell = GetCell(i,j);
	
	int iMaxIndex = (int)Cell.size() - 1;
	XY SectionPoint;

	if ( iMaxIndex < 1 )
		return CurrentNode;

	double dBinderOffset = m_XYarnData[j].dHeight/2.0;

	if ( Cell[iMaxIndex] == PATTERN3D_XYARN && Cell[iMaxIndex-1] == PATTERN3D_YYARN ) // Binder yarn at top and yarn underneath is weft yarn
	{
		int BinderYarnIndex = GetYarnIndex( i, j, iMaxIndex );
		int WeftYarnIndex = GetYarnIndex( i, j, iMaxIndex-1 );

		if ( BinderYarnIndex == -1 || WeftYarnIndex == -1 )
			return CurrentNode;

		XYZ WeftNode = m_Yarns[WeftYarnIndex].GetNode( j )->GetPosition();
		XYZ BinderNode = m_Yarns[BinderYarnIndex].GetNode( CurrentNode )->GetPosition();

		// Get cross section of weft yarn 
		CSectionPowerEllipse* YarnSection = GetWeftCrossSection( WeftYarnIndex );
		if ( YarnSection == NULL )
			return CurrentNode;
		
		// Insert points around Top left quadrant of weft yarn
		{
			XYZ NewNode = WeftNode;
			
			SectionPoint = YarnSection->GetPoint(0.45);
			NewNode.x = NewNode.x + SectionPoint.x;
			NewNode.z = NewNode.z + SectionPoint.y + dBinderOffset + m_dGapSize; 
			
			m_Yarns[BinderYarnIndex].InsertNode( NewNode, CurrentNode);

			if ( CurrentNode == 0 )  // Change last node to match start
			{
				ReplaceLastNode( BinderYarnIndex, NewNode, BinderNode );
			}
			CurrentNode++;

			InsertBinderNode( YarnSection, 0.4, WeftNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
			InsertBinderNode( YarnSection, 0.35, WeftNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
			InsertBinderNode( YarnSection, 0.3, WeftNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
		}

		// Insert points around upper right quadrant of weft
		{
			XYZ NewNode = WeftNode;
			CurrentNode++; // Need to insert after node (ie before next node)
			InsertBinderNode( YarnSection, 0.2, WeftNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
			InsertBinderNode( YarnSection, 0.15, WeftNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
			InsertBinderNode( YarnSection, 0.1, WeftNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
			InsertBinderNode( YarnSection, 0.05, WeftNode, CurrentNode, BinderYarnIndex, dBinderOffset + m_dGapSize );
			CurrentNode--;
		}
		delete YarnSection;
	}
	else if ( Cell[0] == PATTERN3D_XYARN && Cell[1] == PATTERN3D_YYARN )  // Binder yarn at bottom and yarn above is weft yarn
	{
		int BinderYarnIndex = GetYarnIndex( i, j, 0 );
		int WeftYarnIndex = GetYarnIndex( i, j, 1 ); 

		if ( BinderYarnIndex == -1 || WeftYarnIndex == -1 )
			return CurrentNode;

		// Get cross section of weft yarn
		CSectionPowerEllipse* YarnSection = GetWeftCrossSection( WeftYarnIndex );
		if ( YarnSection == NULL )
			return CurrentNode;

		XYZ WeftNode = m_Yarns[WeftYarnIndex].GetNode( j )->GetPosition();
		XYZ BinderNode = m_Yarns[BinderYarnIndex].GetNode( CurrentNode )->GetPosition();
		
		// Insert points around lower left quadrant of weft yarn
		{
			XYZ NewNode = WeftNode;
			SectionPoint = YarnSection->GetPoint(0.55);
			NewNode.x = NewNode.x + SectionPoint.x;
			NewNode.z = NewNode.z + SectionPoint.y - dBinderOffset - m_dGapSize;
			m_Yarns[BinderYarnIndex].InsertNode( NewNode, CurrentNode);

			if ( CurrentNode == 0 )  // Change last node to match start
			{
				ReplaceLastNode( BinderYarnIndex, NewNode, BinderNode );
			}
			CurrentNode++;

			NewNode = WeftNode;
			InsertBinderNode( YarnSection, 0.6, WeftNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			InsertBinderNode( YarnSection, 0.65, WeftNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			InsertBinderNode( YarnSection, 0.7, WeftNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
		}

		// Insert points around lower right quadrant of weft yarn
		{
			XYZ NewNode = WeftNode;
			CurrentNode++;  // Need to insert after node (ie before next node)
			InsertBinderNode( YarnSection, 0.8, WeftNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			InsertBinderNode( YarnSection, 0.85, WeftNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			InsertBinderNode( YarnSection, 0.9, WeftNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			InsertBinderNode( YarnSection, 0.95, WeftNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			//InsertBinderNode( YarnSection, 0.98, WeftNode, CurrentNode, BinderYarnIndex, -(dBinderOffset + m_dGapSize) );
			CurrentNode--;
		}
		delete YarnSection;
	}
	return CurrentNode;
}

int CTextileOffsetAngleInterlock::FindBinderHeight( const vector<PATTERN3D>& Cell ) const
{
	int i = (int)Cell.size() - 1;
	while( i > 0 )
	{
		if ( Cell[i] == PATTERN3D_XYARN )
			return i;
		--i;
	}
	return i;
}

double CTextileOffsetAngleInterlock::GetWidth() const
{
	double dWidth = 0;

	int i;

	for (i=0; i<m_iNumYYarns; ++i)
	{
		dWidth += m_YYarnData[i].dSpacing/2.0;
	}

	return dWidth;
}

