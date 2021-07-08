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
#include "TextileWeave3D.h"
using namespace TexGen;

CTextileWeave3D::CTextileWeave3D(int iWidth, int iHeight, double dSpacing, double dThickness)
: CTextileWeave(iWidth, iHeight, dSpacing, dThickness)
{
}

CTextileWeave3D::~CTextileWeave3D(void)
{
}

CTextileWeave3D::CTextileWeave3D(TiXmlElement &Element)
: CTextileWeave(Element)
{

}

void CTextileWeave3D::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextileWeave::PopulateTiXmlElement(Element, OutputType);
}

string CTextileWeave3D::GetDefaultName() const
{
	 return "3DWeave(W:" + stringify(m_iNumYYarns) + ",H:" + stringify(m_iNumXYarns) + ")";
}

void CTextileWeave3D::AddYLayers(int x, int iNumberLayers)
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
			GetCell(x, i).push_back(PATTERN_YYARN);
	}
	m_bNeedsBuilding = true;
}

void CTextileWeave3D::AddXLayers(int y, int iNumberLayers)
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
			GetCell(i, y).push_back(PATTERN_XYARN);
	}
	m_bNeedsBuilding = true;
}

void CTextileWeave3D::AddYLayers(int iNumberLayers)
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		AddYLayers(i, iNumberLayers);
	}
}

void CTextileWeave3D::AddXLayers(int iNumberLayers)
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		AddXLayers(i, iNumberLayers);
	}
}

void CTextileWeave3D::DeleteYLayers(int x, int iNumberLayers)
{
	if (x<0 || x>=m_iNumYYarns)
	{
		TGERROR("Unable to delete yarn layer, index out of range: " << x);
		return;
	}
	int i, j;
	vector<bool>::reverse_iterator itCell;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		for (j=0; j<iNumberLayers; ++j)
		{
			vector<bool> &Cell = GetCell(x, i);
			itCell = find(Cell.rbegin(), Cell.rend(), (bool)PATTERN_YYARN);
			if (itCell != Cell.rend())
			{
				Cell.erase(itCell.base()-1);
			}
		}
	}
	m_bNeedsBuilding = true;
}

void CTextileWeave3D::DeleteXLayers(int y, int iNumberLayers)
{
	if (y<0 || y>=m_iNumXYarns)
	{
		TGERROR("Unable to delete yarn layer, index out of range: " << y);
		return;
	}
	int i, j;
	vector<bool>::reverse_iterator itCell;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		for (j=0; j<iNumberLayers; ++j)
		{
			vector<bool> &Cell = GetCell(i, y);
			itCell = find(Cell.rbegin(), Cell.rend(), (bool)PATTERN_XYARN);
			if (itCell != Cell.rend())
			{
				Cell.erase(itCell.base()-1);
			}
		}
	}
	m_bNeedsBuilding = true;
}

void CTextileWeave3D::DeleteYLayers(int iNumberLayers)
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		DeleteYLayers(i, iNumberLayers);
	}
}

void CTextileWeave3D::DeleteXLayers(int iNumberLayers)
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		DeleteXLayers(i, iNumberLayers);
	}
}

int CTextileWeave3D::GetNumYLayers(int x) const
{
	const vector<bool> &Cell = GetCell(x, 0);
	return count(Cell.begin(), Cell.end(), (bool)PATTERN_YYARN);
}

int CTextileWeave3D::GetNumXLayers(int y) const
{
	const vector<bool> &Cell = GetCell(0, y);
	return count(Cell.begin(), Cell.end(), (bool)PATTERN_XYARN);
}

int CTextileWeave3D::GetMaxNumLayers() const
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

void CTextileWeave3D::PushDown(int x, int y, int iLevels)
{
	if (x<0 || x>=m_iNumYYarns || y<0 || y>=m_iNumXYarns)
	{
		TGERROR("Unable to push down, index out of range: " << x << ", " << y);
		return;
	}
	vector<bool> &Cell = GetCell(x, y);
	int i;
	for (i=0; i<iLevels; ++i)
	{
		bool bBottom = *Cell.begin();
		Cell.erase(Cell.begin());
		Cell.push_back(bBottom);
	}
	m_bNeedsBuilding = true;
}

void CTextileWeave3D::PushUp(int x, int y, int iLevels)
{
	if (x<0 || x>=m_iNumYYarns || y<0 || y>=m_iNumXYarns)
	{
		TGERROR("Unable to push up, index out of range: " << x << ", " << y);
		return;
	}
	vector<bool> &Cell = GetCell(x, y);
	int i;
	vector<bool>::iterator itTop;
	for (i=0; i<iLevels; ++i)
	{
		bool bTop = Cell[Cell.size()-1];
		Cell.pop_back();
		Cell.insert(Cell.begin(), bTop);
	}
	m_bNeedsBuilding = true;
}

void CTextileWeave3D::PushYDown(int x, int iLevels)
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		PushDown(x, i, iLevels);
	}
}

void CTextileWeave3D::PushYUp(int x, int iLevels)
{
	int i;
	for (i=0; i<m_iNumXYarns; ++i)
	{
		PushUp(x, i, iLevels);
	}
}

void CTextileWeave3D::PushXDown(int y, int iLevels)
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		PushDown(i, y, iLevels);
	}
}

void CTextileWeave3D::PushXUp(int y, int iLevels)
{
	int i;
	for (i=0; i<m_iNumYYarns; ++i)
	{
		PushUp(i, y, iLevels);
	}
}

void CTextileWeave3D::SwapPosition(int x, int y, int iLevel1, int iLevel2)
{
	if (x<0 || x>=m_iNumYYarns || y<0 || y>=m_iNumXYarns)
	{
		TGERROR("Unable to swap position, index out of range: " << x << ", " << y);
		return;
	}
	vector<bool> &Cell = GetCell(x, y);
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
	bool bTemp;
	bTemp = Cell[iLevel1];
	Cell[iLevel1] = Cell[iLevel2];
	Cell[iLevel2] = bTemp;
	m_bNeedsBuilding = true;
}







