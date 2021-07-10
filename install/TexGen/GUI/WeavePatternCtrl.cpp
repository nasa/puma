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
#include "WeavePatternCtrl.h"

BEGIN_EVENT_TABLE(wxWeavePatternCtrl, wxControl)
	EVT_PAINT(wxWeavePatternCtrl::OnPaint)
	EVT_MOUSE_EVENTS(wxWeavePatternCtrl::OnMouseEvent)
	EVT_SIZE(wxWeavePatternCtrl::OnSize)
	EVT_MENU(wxID_ANY, wxWeavePatternCtrl::OnMenu)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxWeavePatternCtrl, wxControl)

DEFINE_EVENT_TYPE(EVT_CROSSOVER_CLICKED)

IMPLEMENT_DYNAMIC_CLASS(wxWeavePatternCtrlEvent, wxCommandEvent)

wxWeavePatternCtrl::wxWeavePatternCtrl()
{
	Init();
}

wxWeavePatternCtrl::wxWeavePatternCtrl(wxWindow* parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator)
{
	Init();
	Create(parent, id, pos, size, style, validator);
	// Used in conjunction with wxBufferedPaintDC
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

wxWeavePatternCtrl::~wxWeavePatternCtrl()
{
}

bool wxWeavePatternCtrl::Create(wxWindow* parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size, long style,	const wxValidator& validator)
{
	if (!wxControl::Create(parent, id, pos, size, style, validator))
		return false;
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	SetInitialSize(size);

	return true;
}

void wxWeavePatternCtrl::Init()
{
	m_dPixelRatio = 1;
	m_b3DMode = false;
	m_iCrossSectionIndex = 0;
	m_SelectedIndices = make_pair(-1, -1);

	m_iIconSpace = 20;
	m_iBorderSpace = 10;

	m_MarqueeStart.x = -1;
	m_MarqueeStart.y = -1;

}

void wxWeavePatternCtrl::OnSize(wxSizeEvent& event)
{
}

void wxWeavePatternCtrl::OnPaint(wxPaintEvent& event)
{
	// This avoids flickering... very nice
	wxAutoBufferedPaintDC dc(this);
	dc.Clear();
//	wxPaintDC dc(this);

	if (!m_WeavePattern)
		return;

	CalculatePixelRatio();

	wxRegion UpdateRegion = GetUpdateRegion();
	if (UpdateRegion.Intersect(GetWeaveRegion()))
		DrawTopDownView(dc);

	if (UpdateRegion.Intersect(GetLeftIconsRegion()))
		DrawLeftIcons(dc);
	if (UpdateRegion.Intersect(GetTopIconsRegion()))
		DrawTopIcons(dc);
	if (UpdateRegion.Intersect(GetCrossSectionRegion()))
		DrawCrossSection(dc, m_iCrossSectionIndex);

	if (m_MarqueeStart.x != -1 || m_MarqueeStart.y != -1)
	{
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(wxPen(*wxBLACK, 1, wxDOT));
		dc.DrawRectangle(GetMarqueeRect());
		dc.SetBrush(wxNullBrush);
		dc.SetPen(wxNullPen);
	}
}

void wxWeavePatternCtrl::DrawTopDownView(wxDC &dc)
{
	CTextileWeave3D* pWeave3D = dynamic_cast<CTextileWeave3D*>(&*m_WeavePattern);
	int iWidth = m_WeavePattern->GetNumYYarns();
	int iHeight = m_WeavePattern->GetNumXYarns();
	int i, j;
	bool bHasXYarns = true;
	bool bHasYYarns = true;
	for (i=0; i<iWidth; ++i)
	{
		if (pWeave3D)
			bHasYYarns = pWeave3D->GetNumYLayers(i)?true:false;
		for (j=0; j<iHeight; ++j)
		{
			if (pWeave3D)
				bHasXYarns = pWeave3D->GetNumXLayers(j)?true:false;
			bool bDir = GetCellStatus(i, j);
			if (bDir == PATTERN_XYARN)
			{
				if (bHasYYarns)
					DrawCell(dc, i, j, PATTERN_YYARN);
				if (bHasXYarns)
					DrawCell(dc, i, j, PATTERN_XYARN);
			}
			else if (bDir == PATTERN_YYARN)
			{
				if (bHasXYarns)
					DrawCell(dc, i, j, PATTERN_XYARN);
				if (bHasYYarns)
					DrawCell(dc, i, j, PATTERN_YYARN);
			}
		}
	}
	wxRect Region = GetWeaveRegion();
	YARN_POS_DATA PosData = GetYCoordinate(m_iCrossSectionIndex);
	wxPen Pen(*wxCYAN, 3, wxDOT);
	dc.SetPen(Pen);
	dc.DrawLine(Region.x, PosData.iYarnCenter, Region.x+Region.width, PosData.iYarnCenter);
	dc.SetPen(wxNullPen);
}

void wxWeavePatternCtrl::DrawCrossSection(wxDC &dc, int iIndex)
{
//	wxPen Pen(*wxBLUE, GetYarnHeight(), wxSOLID);
//	dc.SetPen(Pen);
	dc.SetBrush(*wxBLUE_BRUSH);

	int i, j, iHeight;
	const CTextileWeave &WeavePattern = *m_WeavePattern;
	vector<bool>::const_iterator itCell;
	vector<double> YarnZValues;
	bool bFound = false;
	double dZValue;
	for (iHeight=0; ; ++iHeight)
	{
		YarnZValues.clear();
		for (j=0; j<m_WeavePattern->GetNumYYarns(); ++j)
		{
			bFound = false;
			const vector<bool> &CellPattern = WeavePattern.GetCell(j, iIndex);
			i = 0;
			for (itCell=CellPattern.begin(); itCell!=CellPattern.end(); ++itCell)
			{
				if (*itCell == PATTERN_XYARN)
				{
					if (i == iHeight)
					{
						bFound = true;
						dZValue = double(distance(CellPattern.begin(), itCell)+0.5)/double(CellPattern.size());
						YarnZValues.push_back(dZValue);
						break;
					}
					else
					{
						++i;
					}
				}
			}
			if (!bFound)
				break;
		}
		if (bFound)
			DrawHorizontalYarn(dc, YarnZValues);
		else
			break;
	}

//	dc.SetPen(wxNullPen);
	dc.SetBrush(wxNullBrush);


	dc.SetBrush(*wxRED_BRUSH);

	for (j=0; j<m_WeavePattern->GetNumYYarns(); ++j)
	{
		const vector<bool> &CellPattern = WeavePattern.GetCell(j, iIndex);
		for (itCell=CellPattern.begin(); itCell!=CellPattern.end(); ++itCell)
		{
			dZValue = double(distance(CellPattern.begin(), itCell))/double(CellPattern.size()-1);
			if (*itCell == PATTERN_YYARN)
			{
				dZValue = double(distance(CellPattern.begin(), itCell)+0.5)/double(CellPattern.size());
				DrawVerticalYarn(dc, j, dZValue);
			}
		}
	}

	dc.SetBrush(wxNullBrush);
}

wxPoint wxWeavePatternCtrl::GetCrossSectionPos(int iIndex, double dZValue)
{
	wxRect Region = GetCrossSectionRegion();
	wxPoint Point;
	YARN_POS_DATA PosData = GetXCoordinate(iIndex);
	Point.x = PosData.iYarnCenter;
//	Point.x = Region.x + Region.width*(double(iIndex+0.5)/double(m_WeavePattern->GetNumYYarns()));
	Point.y = Region.y + Region.height*(1-dZValue);
	return Point;
}

void wxWeavePatternCtrl::DrawHorizontalYarn(wxDC &dc, vector<double> &YarnZValues)
{
	if (YarnZValues.empty())
		return;

	int iYarnHeight = m_dPixelRatio*m_WeavePattern->GetXYarnHeights(m_iCrossSectionIndex);
//	int iYarnHeight = GetYarnHeight();
	int i, iSize = (int)YarnZValues.size();
	vector<double>::const_iterator itZValue;
	vector<CNode> MasterNodes;
	wxRect CrossSectionRegion = GetCrossSectionRegion();
	CNode FirstNode, LastNode;
	for (itZValue=YarnZValues.begin(), i=0; itZValue!=YarnZValues.end(); ++itZValue, ++i)
	{
		wxPoint Point = GetCrossSectionPos(i, *itZValue);
		if (i==0)
			LastNode = CNode(XYZ(Point.x+CrossSectionRegion.width, 0, Point.y), XYZ(1, 0, 0));
		if (i==(int)YarnZValues.size()-1)
			FirstNode = CNode(XYZ(Point.x-CrossSectionRegion.width, 0, Point.y), XYZ(1, 0, 0));
		MasterNodes.push_back(CNode(XYZ(Point.x, 0, Point.y), XYZ(1, 0, 0)));
	}
	MasterNodes.insert(MasterNodes.begin(), FirstNode);
	MasterNodes.push_back(LastNode);

	int iDivisions = 50;

	wxPoint* aPoints = new wxPoint[iDivisions*2];

	CInterpolationBezier Interp(true);
	vector<CSlaveNode> SlaveNodes = Interp.GetSlaveNodes(MasterNodes, iDivisions, false);
	XYZ Position, P1, P2;
	XYZ Tangent;
	for (i=0; i<iDivisions; ++i)
	{
		Position = SlaveNodes[i].GetPosition();
//		Tangent = SlaveNodes[i].GetTangent();
		Tangent = XYZ(1, 0, 0);
		Tangent = XYZ(Tangent.z, 0, -Tangent.x);
		P1 = Position+Tangent*(0.5*iYarnHeight);
		P2 = Position-Tangent*(0.5*iYarnHeight);
		aPoints[i].x = P1.x;
		aPoints[i].y = P1.z;
		aPoints[2*iDivisions-(i+1)].x = P2.x;
		aPoints[2*iDivisions-(i+1)].y = P2.z;
	}

//	dc.DrawLines(iSize, aPoints);
//	dc.DrawSpline(iSize, aPoints);
	dc.SetClippingRegion(CrossSectionRegion);
	dc.DrawPolygon(2*iDivisions, aPoints);
	dc.DestroyClippingRegion();

	delete [] aPoints;
}

void wxWeavePatternCtrl::DrawVerticalYarn(wxDC &dc, int iIndex, double dZValue)
{
	wxPoint Center = GetCrossSectionPos(iIndex, dZValue);

	YARN_POS_DATA XPosData = GetXCoordinate(iIndex);
	int iYarnHeight = m_dPixelRatio*m_WeavePattern->GetYYarnHeights(iIndex);
	wxSize Size(XPosData.iYarnMax-XPosData.iYarnMin, iYarnHeight);

	dc.DrawEllipse(Center.x-Size.x/2, Center.y-Size.y/2, Size.x, Size.y);
}

void wxWeavePatternCtrl::DrawCell(wxDC &dc, int i, int j, bool bDirection)
{
	YARN_POS_DATA XPosData = GetXCoordinate(i);
	YARN_POS_DATA YPosData = GetYCoordinate(j);
	wxRect Cell;
	if (bDirection == PATTERN_YYARN)
	{
		// Yarns going in the Y direction
		Cell.x = XPosData.iYarnMin;
		Cell.width = XPosData.iYarnMax-XPosData.iYarnMin;
		Cell.y = YPosData.iCellMin;
		Cell.height = YPosData.iCellMax-YPosData.iCellMin;

		dc.GradientFillLinear(Cell, wxColour(255, 0, 0), wxColour(200, 0, 0), wxEAST);
		dc.DrawLine(Cell.x, Cell.y, Cell.x, Cell.y+Cell.height);
		dc.DrawLine(Cell.x+Cell.width, Cell.y, Cell.x+Cell.width, Cell.y+Cell.height);
	}
	else if (bDirection == PATTERN_XYARN)
	{
		Cell.x = XPosData.iCellMin;
		Cell.width = XPosData.iCellMax-XPosData.iCellMin;
		Cell.y = YPosData.iYarnMin;
		Cell.height = YPosData.iYarnMax-YPosData.iYarnMin;

		dc.GradientFillLinear(Cell, wxColour(0, 0, 255), wxColour(0, 0, 200), wxSOUTH);
		dc.DrawLine(Cell.x, Cell.y, Cell.x+Cell.width, Cell.y);
		dc.DrawLine(Cell.x, Cell.y+Cell.height, Cell.x+Cell.width, Cell.y+Cell.height);
	}
}

int wxWeavePatternCtrl::GetWeaveWidth()
{
	if (!m_WeavePattern)
		return 0;
	return m_WeavePattern->GetNumYYarns();
}

int wxWeavePatternCtrl::GetWeaveHeight()
{
	if (!m_WeavePattern)
		return 0;
	return m_WeavePattern->GetNumXYarns();
}

bool wxWeavePatternCtrl::GetCellStatus(int i, int j)
{
	if (!m_WeavePattern)
		return false;
	const CTextileWeave &WeavePattern = *m_WeavePattern;
	const vector<bool> &CellPattern = WeavePattern.GetCell(i, j);
	if (CellPattern.empty())
		return false;	// Return some rubish
	return CellPattern.back();
}
/*
void wxWeavePatternCtrl::SetCellStatus(int i, int j, bool bVal)
{
	if (!m_WeavePattern)
		return;
	if (GetCellStatus(i, j) != bVal)
	{
		m_WeavePattern->SwapPosition(i, j);
	}
}
*/

void wxWeavePatternCtrl::DrawTopIcons(wxDC &dc)
{
	wxRect TopIconsRect = GetTopIconsRegion();

	wxRect Cell;
	wxPoint RefPoint;

	CTextileWeave3D* pWeave3D = dynamic_cast<CTextileWeave3D*>(&*m_WeavePattern);

	int i, iLayers = 0;
	for (i=0; i<m_WeavePattern->GetNumYYarns(); ++i)
	{
		Cell = GetTopIconRect(i);
		if (pWeave3D)
			iLayers = pWeave3D->GetNumYLayers(i);

		wxString NumLayers;
		if (iLayers)
			NumLayers << iLayers;

		if (m_SelectedYYarns[i])
			dc.SetBrush(*wxRED_BRUSH);
		else
			dc.SetBrush(*wxWHITE_BRUSH);
		dc.DrawRectangle(Cell.x, Cell.y, Cell.width, Cell.height);
		dc.DrawLabel(NumLayers, Cell, wxALIGN_CENTER);
		dc.SetBrush(wxNullBrush);
	}
}

void wxWeavePatternCtrl::DrawLeftIcons(wxDC &dc)
{
	wxRect LeftIconsRect = GetLeftIconsRegion();

	wxRect Cell;
	wxPoint RefPoint;

	CTextileWeave3D* pWeave3D = dynamic_cast<CTextileWeave3D*>(&*m_WeavePattern);

	int i, iLayers = 0;
	for (i=0; i<m_WeavePattern->GetNumXYarns(); ++i)
	{
		Cell = GetLeftIconRect(i);
		if (pWeave3D)
			iLayers = pWeave3D->GetNumXLayers(i);

		wxString NumLayers;
		if (iLayers)
			NumLayers << iLayers;

		if (m_SelectedXYarns[i])
			dc.SetBrush(*wxRED_BRUSH);
		else
			dc.SetBrush(*wxWHITE_BRUSH);
		dc.DrawRectangle(Cell.x, Cell.y, Cell.width, Cell.height);
		dc.DrawLabel(NumLayers, Cell, wxALIGN_CENTER);
		dc.SetBrush(wxNullBrush);
	}
}

wxRect wxWeavePatternCtrl::GetTopIconRect(int i)
{
	wxRect TopIconsRect = GetTopIconsRegion();
	YARN_POS_DATA XPosData = GetXCoordinate(i);

	TopIconsRect.x = XPosData.iCellMin;
	TopIconsRect.width = XPosData.iCellMax-XPosData.iCellMin;

	return TopIconsRect;
}

wxRect wxWeavePatternCtrl::GetLeftIconRect(int i)
{
	wxRect LeftIconsRect = GetLeftIconsRegion();
	YARN_POS_DATA YPosData = GetYCoordinate(i);

	LeftIconsRect.y = YPosData.iCellMin;
	LeftIconsRect.height = YPosData.iCellMax-YPosData.iCellMin;

	return LeftIconsRect;
}

void wxWeavePatternCtrl::HandleIconSelect(wxRect MarqueeSelect)
{
	wxRect TopIconsRect = GetTopIconsRegion();
	wxRect LeftIconsRect = GetLeftIconsRegion();
	if (TopIconsRect.Intersects(MarqueeSelect))
	{
		int i;
		for (i=0; i<m_WeavePattern->GetNumYYarns(); ++i)
		{
			if (GetTopIconRect(i).Intersects(MarqueeSelect))
			{
				m_SelectedYYarns[i] = true;
				m_dLastSelectedWidth = m_WeavePattern->GetYYarnWidths(i);
				m_dLastSelectedHeight = m_WeavePattern->GetYYarnHeights(i);
				m_dLastSelectedSpacing = m_WeavePattern->GetYYarnSpacings(i);
			}
		}
	}
	if (LeftIconsRect.Intersects(MarqueeSelect))
	{
		int i;
		for (i=0; i<m_WeavePattern->GetNumXYarns(); ++i)
		{
			if (GetLeftIconRect(i).Intersects(MarqueeSelect))
			{
				m_SelectedXYarns[i] = true;
				m_dLastSelectedWidth = m_WeavePattern->GetXYarnWidths(i);
				m_dLastSelectedHeight = m_WeavePattern->GetXYarnHeights(i);
				m_dLastSelectedSpacing = m_WeavePattern->GetXYarnSpacings(i);
			}
		}
	}
}

bool wxWeavePatternCtrl::HandleIconSelect(wxPoint Position)
{
	wxRect TopIconsRect = GetTopIconsRegion();
	wxRect LeftIconsRect = GetLeftIconsRegion();
	if (TopIconsRect.Contains(Position))
	{
		int i;
		for (i=0; i<m_WeavePattern->GetNumYYarns(); ++i)
		{
			if (GetTopIconRect(i).Contains(Position))
			{
				m_SelectedYYarns[i] = true;
				m_dLastSelectedWidth = m_WeavePattern->GetYYarnWidths(i);
				m_dLastSelectedHeight = m_WeavePattern->GetYYarnHeights(i);
				m_dLastSelectedSpacing = m_WeavePattern->GetYYarnSpacings(i);
				break;
			}
		}
		if (i != m_WeavePattern->GetNumYYarns())
		{
			Refresh();
			return true;
		}
	}
	else if (LeftIconsRect.Contains(Position))
	{
		int i;
		for (i=0; i<m_WeavePattern->GetNumXYarns(); ++i)
		{
			if (GetLeftIconRect(i).Contains(Position))
			{
				m_SelectedXYarns[i] = true;
				m_dLastSelectedWidth = m_WeavePattern->GetXYarnWidths(i);
				m_dLastSelectedHeight = m_WeavePattern->GetXYarnHeights(i);
				m_dLastSelectedSpacing = m_WeavePattern->GetXYarnSpacings(i);
				break;
			}
		}
		if (i != m_WeavePattern->GetNumYYarns())
		{
			Refresh();
			return true;
		}
	}
	return false;
}

wxRect wxWeavePatternCtrl::GetLeftIconsRegion()
{
	wxRect Rect = GetWeaveRegion();
	Rect.x -= m_iIconSpace;
	Rect.width = m_iIconSpace-2;
	return Rect;
}

wxRect wxWeavePatternCtrl::GetTopIconsRegion()
{
	wxRect Rect = GetWeaveRegion();
	Rect.y -= m_iIconSpace;
	Rect.height = m_iIconSpace-2;
	return Rect;
}

void wxWeavePatternCtrl::CalculatePixelRatio()
{
	wxRect Rect = GetClientRect();
	double dWidth = m_WeavePattern->GetWidth();
	double dHeight = m_WeavePattern->GetHeight();
	Rect.width -= m_iIconSpace;
	Rect.height -= m_iIconSpace;
	dHeight += m_WeavePattern->GetFabricThickness();
	Rect.height -= m_iBorderSpace;
	m_dPixelRatio = min(Rect.width/dWidth, Rect.height/dHeight);
	m_iFreeWidth = Rect.width-m_dPixelRatio*dWidth;
	m_iFreeHeight = Rect.height-m_dPixelRatio*dHeight;
}

wxRect wxWeavePatternCtrl::GetWeaveRegion()
{
	wxRect ClientRect = GetClientRect();
	wxRect Rect = ClientRect;
	Rect.x += m_iIconSpace;
	Rect.y += m_iIconSpace;
	Rect.width = m_WeavePattern->GetWidth()*m_dPixelRatio;
	Rect.height = m_WeavePattern->GetHeight()*m_dPixelRatio;

	Rect.x += m_iFreeWidth/2;
	Rect.y += m_iFreeHeight/2;

	return Rect;
}

wxRect wxWeavePatternCtrl::GetCrossSectionRegion()
{
	wxRect WeaveRegion = GetWeaveRegion();

	wxRect ClientRect = GetClientRect();
	wxRect CrossSectionRect;
	CrossSectionRect.y = WeaveRegion.y+WeaveRegion.height + m_iBorderSpace;
	CrossSectionRect.height = m_WeavePattern->GetFabricThickness() * m_dPixelRatio;

	CrossSectionRect.x = WeaveRegion.x;
	CrossSectionRect.width = WeaveRegion.width;

	return CrossSectionRect;
}

int wxWeavePatternCtrl::GetNumberOfYarns(int i, int j)
{
	const CTextileWeave &WeavePattern = *m_WeavePattern;
	return (int)WeavePattern.GetCell(i, j).size();
}

wxWeavePatternCtrl::YARN_POS_DATA wxWeavePatternCtrl::GetXCoordinate(int iIndex)
{
	double dAccumulated = 0;
	double dWidth, dGapBefore, dGapAfter;
	double dCellMin, dYarnMin, dYarnMax, dCellMax;
	double dTotalSpacingY = 0;
	int i, iNumYYarns = m_WeavePattern->GetNumYYarns();
	for (i=0; i<iNumYYarns; ++i)
	{
		dWidth = m_WeavePattern->GetYYarnWidths(i);
		if (i>0)
			dGapBefore = 0.5*m_WeavePattern->GetYYarnGapSize(i-1);
		else
			dGapBefore = 0.5*m_WeavePattern->GetYYarnGapSize(iNumYYarns-1);
		dGapAfter = 0.5*m_WeavePattern->GetYYarnGapSize(i);
		if (iIndex == i)
		{
			dCellMin = dAccumulated;
			dYarnMin = dCellMin+dGapBefore;
			dYarnMax = dYarnMin+dWidth;
			dCellMax = dYarnMax+dGapAfter;
		}
		dAccumulated += dWidth+dGapBefore+dGapAfter;
		dTotalSpacingY += m_WeavePattern->GetYYarnSpacings(i);
	}
	dCellMin /= dTotalSpacingY;
	dYarnMin /= dTotalSpacingY;
	dYarnMax /= dTotalSpacingY;
	dCellMax /= dTotalSpacingY;
	double dYarnCenter = 0.5*(dYarnMin+dYarnMax);
	wxRect WeaveRegion = GetWeaveRegion();
	YARN_POS_DATA YarnPosData;
	YarnPosData.iCellMin = WeaveRegion.x+dCellMin*WeaveRegion.width;
	YarnPosData.iYarnMin = WeaveRegion.x+dYarnMin*WeaveRegion.width;
	YarnPosData.iYarnCenter = WeaveRegion.x+dYarnCenter*WeaveRegion.width;
	YarnPosData.iYarnMax = WeaveRegion.x+dYarnMax*WeaveRegion.width;
	YarnPosData.iCellMax = WeaveRegion.x+dCellMax*WeaveRegion.width;
	return YarnPosData;
}

wxWeavePatternCtrl::YARN_POS_DATA wxWeavePatternCtrl::GetYCoordinate(int iIndex)
{
	double dAccumulated = 0;
	double dWidth, dGapBefore, dGapAfter;
	double dCellMin, dYarnMin, dYarnMax, dCellMax;
	double dTotalSpacingX = 0;
	int i, iNumXYarns = m_WeavePattern->GetNumXYarns();
	for (i=0; i<iNumXYarns; ++i)
	{
		dWidth = m_WeavePattern->GetXYarnWidths(i);
		if (i>0)
			dGapBefore = 0.5*m_WeavePattern->GetXYarnGapSize(i-1);
		else
			dGapBefore = 0.5*m_WeavePattern->GetXYarnGapSize(iNumXYarns-1);
		dGapAfter = 0.5*m_WeavePattern->GetXYarnGapSize(i);
		if (iIndex == i)
		{
			dCellMin = dAccumulated;
			dYarnMin = dCellMin+dGapBefore;
			dYarnMax = dYarnMin+dWidth;
			dCellMax = dYarnMax+dGapAfter;
		}
		dAccumulated += dWidth+dGapBefore+dGapAfter;
		dTotalSpacingX += m_WeavePattern->GetXYarnSpacings(i);
	}
	dCellMin /= dTotalSpacingX;
	dYarnMin /= dTotalSpacingX;
	dYarnMax /= dTotalSpacingX;
	dCellMax /= dTotalSpacingX;
	double dYarnCenter = 0.5*(dYarnMin+dYarnMax);
	wxRect WeaveRegion = GetWeaveRegion();
	YARN_POS_DATA YarnPosData;
	// Note: max and min are flipped because of Y flip
	YarnPosData.iCellMax = WeaveRegion.y+(1-dCellMin)*WeaveRegion.height;
	YarnPosData.iYarnMax = WeaveRegion.y+(1-dYarnMin)*WeaveRegion.height;
	YarnPosData.iYarnCenter = WeaveRegion.y+(1-dYarnCenter)*WeaveRegion.height;
	YarnPosData.iYarnMin = WeaveRegion.y+(1-dYarnMax)*WeaveRegion.height;
	YarnPosData.iCellMin = WeaveRegion.y+(1-dCellMax)*WeaveRegion.height;
	return YarnPosData;
}


wxRect wxWeavePatternCtrl::GetCellRegion(int x, int y)
{
	YARN_POS_DATA XPosData = GetXCoordinate(x);
	YARN_POS_DATA YPosData = GetYCoordinate(y);
	wxRect Cell;
	Cell.x = XPosData.iCellMin;
	Cell.width = XPosData.iCellMax-XPosData.iCellMin;
	Cell.y = YPosData.iCellMin;
	Cell.height = YPosData.iCellMax-YPosData.iCellMin;
	return Cell;
}

pair<int, int> wxWeavePatternCtrl::GetCellIndices(wxPoint Position)
{
	wxRect WeaveRegion = GetWeaveRegion();
	if (!WeaveRegion.Contains(Position))
		return make_pair(-1, -1);

	YARN_POS_DATA PosData;
	int i;
	int x = -1, y = -1;
	int iNumYYarns = m_WeavePattern->GetNumYYarns();
	int iNumXYarns = m_WeavePattern->GetNumXYarns();
	for (i=0; i<iNumYYarns; ++i)
	{
		PosData = GetXCoordinate(i);
		if (Position.x >= PosData.iCellMin &&
			Position.x < PosData.iCellMax)
			x = i;
	}
	for (i=0; i<iNumXYarns; ++i)
	{
		PosData = GetYCoordinate(i);
		if (Position.y >= PosData.iCellMin &&
			Position.y < PosData.iCellMax)
			y = i;
	}

	if (x != -1 && y != -1)
		return make_pair(x, y);
	else
		return make_pair(-1, -1);
}

pair<int, int> wxWeavePatternCtrl::GetCellHeight(wxPoint Position)
{
	wxRect Region = GetCrossSectionRegion();
	if (!Region.Contains(Position))
		return make_pair(-1, -1);

	int iWidth = m_WeavePattern->GetNumYYarns();
	int iDivisionWidth = Region.width/iWidth;
	int i = (Position.x-Region.x)/iDivisionWidth;
	if (i<0)
		i = 0;
	if (i>=iWidth)
		i = iWidth-1;

	int iHeight = GetNumberOfYarns(i, m_iCrossSectionIndex);
	int iDivisionHeight = Region.height/iHeight;
	int j = (Position.y-Region.y)/iDivisionHeight;
	j = iHeight-(j+1);
	return make_pair(i, j);
}

void wxWeavePatternCtrl::OnMenu(wxCommandEvent& event)
{
	CTextileWeave3D* pWeave3D = dynamic_cast<CTextileWeave3D*>(&*m_WeavePattern);
	switch (event.GetId())
	{
	case MENUID_ADDLAYER:
		if (pWeave3D)
		{
			int i;
			for (i=0; i<pWeave3D->GetNumYYarns(); ++i)
			{
				if (m_SelectedYYarns[i])
					pWeave3D->AddYLayers(i, 1);
			}
			for (i=0; i<pWeave3D->GetNumXYarns(); ++i)
			{
				if (m_SelectedXYarns[i])
					pWeave3D->AddXLayers(i, 1);
			}
		}
		break;
	case MENUID_REMOVELAYER:
		if (pWeave3D)
		{
			int i;
			for (i=0; i<pWeave3D->GetNumYYarns(); ++i)
			{
				if (m_SelectedYYarns[i])
					pWeave3D->DeleteYLayers(i, 1);
			}
			for (i=0; i<pWeave3D->GetNumXYarns(); ++i)
			{
				if (m_SelectedXYarns[i])
					pWeave3D->DeleteXLayers(i, 1);
			}
		}
		break;
	case MENUID_SETYARNWIDTH:
		{
			wxString Val = wxGetTextFromUser(wxT("Please enter the yarn width"), wxT("Input value"), wxString() << m_dLastSelectedWidth, this);
			double dVal;
			if (Val.ToDouble(&dVal))
			{
				m_dLastSelectedWidth = dVal;
				int i;
				for (i=0; i<m_WeavePattern->GetNumYYarns(); ++i)
				{
					if (m_SelectedYYarns[i])
						m_WeavePattern->SetYYarnWidths(i, dVal);
				}
				for (i=0; i<m_WeavePattern->GetNumXYarns(); ++i)
				{
					if (m_SelectedXYarns[i])
						m_WeavePattern->SetXYarnWidths(i, dVal);
				}
			}
		}
		break;
	case MENUID_SETYARNHEIGHT:
		{
			wxString Val = wxGetTextFromUser(wxT("Please enter the yarn height"), wxT("Input value"), wxString() << m_dLastSelectedHeight, this);
			double dVal;
			if (Val.ToDouble(&dVal))
			{
				m_dLastSelectedHeight = dVal;
				int i;
				for (i=0; i<m_WeavePattern->GetNumYYarns(); ++i)
				{
					if (m_SelectedYYarns[i])
						m_WeavePattern->SetYYarnHeights(i, dVal);
				}
				for (i=0; i<m_WeavePattern->GetNumXYarns(); ++i)
				{
					if (m_SelectedXYarns[i])
						m_WeavePattern->SetXYarnHeights(i, dVal);
				}
			}
		}
		break;
	case MENUID_SETYARNSPACING:
		{
			wxString Val = wxGetTextFromUser(wxT("Please enter the yarn spacing"), wxT("Input value"), wxString() << m_dLastSelectedSpacing, this);
			double dVal;
			if (Val.ToDouble(&dVal))
			{
				m_dLastSelectedSpacing = dVal;
				int i;
				for (i=0; i<m_WeavePattern->GetNumYYarns(); ++i)
				{
					if (m_SelectedYYarns[i])
						m_WeavePattern->SetYYarnSpacings(i, dVal);
				}
				for (i=0; i<m_WeavePattern->GetNumXYarns(); ++i)
				{
					if (m_SelectedXYarns[i])
						m_WeavePattern->SetXYarnSpacings(i, dVal);
				}
			}
		}
		break;
	}
	Refresh();
}

void wxWeavePatternCtrl::OnMouseEvent(wxMouseEvent& event)
{
	m_MarqueeEnd = event.GetPosition();
	if (event.LeftDown() || event.LeftDClick())
	{
		if (!event.ShiftDown() && !event.ControlDown() && GetNumSelected() > 0)
		{
			fill(m_SelectedYYarns.begin(), m_SelectedYYarns.end(), false);
			fill(m_SelectedXYarns.begin(), m_SelectedXYarns.end(), false);
			RefreshRect(GetTopIconsRegion());
			RefreshRect(GetLeftIconsRegion());
		}
		wxPoint MousePos = event.GetPosition();
		pair<int, int> Indices = GetCellIndices(MousePos);
		if (!m_b3DMode)
		{
			if (Indices.first >= 0 && Indices.first < m_WeavePattern->GetNumYYarns() &&
				Indices.second >= 0 && Indices.second < m_WeavePattern->GetNumXYarns())
			{
				CTextileWeave2D* pWeave2D = dynamic_cast<CTextileWeave2D*>(&*m_WeavePattern);
				if (pWeave2D)
				{
					pWeave2D->SwapPosition(Indices.first, Indices.second);
					RefreshRect(GetCellRegion(Indices.first, Indices.second));
					RefreshRect(GetCrossSectionRegion());
				}
			}
			else
			{
				if (!HandleIconSelect(MousePos))
					m_MarqueeStart = MousePos;
			}
		}
		else
		{
			if (Indices.second >= 0 && Indices.second < m_WeavePattern->GetNumXYarns())
			{
				m_iCrossSectionIndex = Indices.second;
				Refresh();
			}
			else
			{
				Indices = GetCellHeight(MousePos);
				if (Indices.first >= 0 && Indices.first < m_WeavePattern->GetNumYYarns())
				{
					m_SelectedIndices = Indices;
				}
				else
				{
					if (!HandleIconSelect(MousePos))
						m_MarqueeStart = MousePos;
				}
			}
		}
	}
	if (event.RightDown())
	{
		pair<int, int> Indices = GetCellIndices(event.GetPosition());
		if (Indices.second >= 0 && Indices.second < m_WeavePattern->GetNumXYarns())
		{
			m_iCrossSectionIndex = Indices.second;
			Refresh();
		}
		else
		{
			if (GetNumSelected() == 0)
			{
				HandleIconSelect(event.GetPosition());
			}
			// Note that HandleIconSelect may change the state of GetNumSelected(), so no else if used here
			if (GetNumSelected() != 0)
			{
				wxMenu PopupMenu;
				if (m_b3DMode)
				{
					PopupMenu.Append(MENUID_ADDLAYER, wxT("Add layer"));
					PopupMenu.Append(MENUID_REMOVELAYER, wxT("Remove layer"));
				}
				PopupMenu.Append(MENUID_SETYARNWIDTH, wxT("Set yarn width..."));
				PopupMenu.Append(MENUID_SETYARNHEIGHT, wxT("Set yarn height..."));
				PopupMenu.Append(MENUID_SETYARNSPACING, wxT("Set yarn spacing..."));
				wxWindow::PopupMenu(&PopupMenu);
			}
		}
	}
	if (event.Dragging())
	{
		if (m_SelectedIndices.first != -1 && m_SelectedIndices.second != -1)
		{
			wxPoint MousePos = event.GetPosition();
			pair<int, int> Indices = GetCellHeight(MousePos);
			if (Indices.first == m_SelectedIndices.first &&
				Indices.second != m_SelectedIndices.second &&
				Indices.second != -1)
			{
				CTextileWeave3D* pWeave3D = dynamic_cast<CTextileWeave3D*>(&*m_WeavePattern);
				if (pWeave3D)
				{
					pWeave3D->SwapPosition(m_SelectedIndices.first, m_iCrossSectionIndex, m_SelectedIndices.second, Indices.second);
					m_SelectedIndices = Indices;
					Refresh();
				}
			}
		}
		if (m_MarqueeStart.x != -1 || m_MarqueeStart.y != -1)
		{
			m_MarqueeEnd = event.GetPosition();
			Refresh();
		}
	}
	if (event.LeftUp() || event.Leaving())
	{
		if (m_MarqueeStart.x != -1 || m_MarqueeStart.y != -1)
		{
			HandleIconSelect(GetMarqueeRect());
			Refresh();
		}
		m_SelectedIndices = make_pair(-1, -1);
		m_MarqueeStart.x = -1;
		m_MarqueeStart.y = -1;
	}
}

int wxWeavePatternCtrl::GetNumSelected()
{
	int iNum = 0;
	iNum += count(m_SelectedYYarns.begin(), m_SelectedYYarns.end(), true);
	iNum += count(m_SelectedXYarns.begin(), m_SelectedXYarns.end(), true);
	return iNum;
}

wxRect wxWeavePatternCtrl::GetMarqueeRect()
{
	wxRect MarqueeRect(m_MarqueeStart.x, m_MarqueeStart.y, m_MarqueeEnd.x-m_MarqueeStart.x, m_MarqueeEnd.y-m_MarqueeStart.y);
	if (MarqueeRect.width < 0)
	{
		MarqueeRect.width *= -1;
		MarqueeRect.x -= MarqueeRect.width;
	}
	if (MarqueeRect.height < 0)
	{
		MarqueeRect.height *= -1;
		MarqueeRect.y -= MarqueeRect.height;
	}
	return MarqueeRect;
}

void wxWeavePatternCtrl::SetWeaveSize(int iNumYYarns, int iNumXYarns, bool b3DMode)
{
	m_b3DMode = b3DMode;
	if (!b3DMode)
	{
		m_WeavePattern = CTextileWeave2D(iNumYYarns, iNumXYarns, 1, 1, false);
	}
	else
	{
		CTextileWeave3D Weave3D(iNumYYarns, iNumXYarns, 1, 1);
		Weave3D.AddYLayers(1);
		Weave3D.AddXLayers(1);
		m_WeavePattern = Weave3D;
	}
	m_SelectedYYarns.resize(m_WeavePattern->GetNumYYarns());
	m_SelectedXYarns.resize(m_WeavePattern->GetNumXYarns());
}

void wxWeavePatternCtrl::SetWeaveSize(const CTextileWeave &Weave)
{
	if (Weave.GetType() == "CTextileWeave3D")
		m_b3DMode = true;
	else
		m_b3DMode = false;
	m_WeavePattern = Weave;
	m_SelectedYYarns.resize(m_WeavePattern->GetNumYYarns());
	m_SelectedXYarns.resize(m_WeavePattern->GetNumXYarns());
}

















