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
#include "CircleRangeCtrl.h"

BEGIN_EVENT_TABLE(wxCircleRangeCtrl, wxControl)
	EVT_PAINT(wxCircleRangeCtrl::OnPaint)
	EVT_MOUSE_EVENTS(wxCircleRangeCtrl::OnMouseEvent)
	EVT_SIZE(wxCircleRangeCtrl::OnSize)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxCircleRangeCtrl, wxControl)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_DIVISION_ADDED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DIVISION_DELETED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DIVISION_BEGIN_DRAG)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DIVISION_DRAGGING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_DIVISION_END_DRAG)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SECTION_CLICKED)

IMPLEMENT_DYNAMIC_CLASS(wxCircleRangeCtrlEvent, wxCommandEvent)

wxCircleRangeCtrl::wxCircleRangeCtrl()
{
	Init();
}

wxCircleRangeCtrl::wxCircleRangeCtrl(wxWindow* parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator)
{
	Init();
	Create(parent, id, pos, size, style, validator);
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

wxCircleRangeCtrl::~wxCircleRangeCtrl()
{
	while (!m_SectionData.empty())
	{
		if (m_SectionData.back())
			delete m_SectionData.back();
		m_SectionData.pop_back();
	}
}

bool wxCircleRangeCtrl::Create(wxWindow* parent, wxWindowID id,
		const wxPoint& pos, const wxSize& size, long style,	const wxValidator& validator)
{
	if (!wxControl::Create(parent, id, pos, size, style, validator))
		return false;
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	SetInitialSize(size);

	CalculateHandlePositions();

	return true;
}

void wxCircleRangeCtrl::Init()
{
	m_iDragHandleRadius = 4;
	m_DeleteHandleSize = wxSize(8, 8);
	m_iSelectedDivision = -1;
	m_SectionData.push_back(NULL);
	// Used in conjunction with wxBufferedPaintDC
	//SetBackgroundStyle(wxBG_STYLE_CUSTOM);
/*	m_Divisions.push_back(0.1);
	m_Divisions.push_back(0.3);
	m_Divisions.push_back(0.6);
	m_Divisions.push_back(0.75);
	OnNumDivisionsChanged();*/
}

wxRect wxCircleRangeCtrl::GetCircleRect()
{
	wxRect circleRect = GetClientRect();
	circleRect.Deflate(15, 15);
	return circleRect;
}

wxPoint wxCircleRangeCtrl::GetCircleCenter()
{
	wxRect circleRect = GetCircleRect();
	wxPoint circleCenter(circleRect.x + 0.5*circleRect.width, circleRect.y + 0.5*circleRect.height);
	return circleCenter;
}

double wxCircleRangeCtrl::GetFraction(wxPoint Position)
{
	wxRect circleRect = GetCircleRect();
	wxPoint circleCenter = GetCircleCenter();
	wxPoint relative = Position-circleCenter;
	double dFraction = atan2((double)-relative.y, (double)relative.x*(double)circleRect.height/(double)circleRect.width)/(2*PI);
	if (dFraction < 0)
		dFraction += 1;
	return dFraction;
}

bool wxCircleRangeCtrl::IsInside(wxPoint Position)
{
	wxRect circleRect = GetCircleRect();
	wxPoint circleCenter = GetCircleCenter();
	XY relative;
	relative.x = (Position.x-circleCenter.x)*((double)circleRect.height/(double)circleRect.width);
	relative.y = Position.y-circleCenter.y;
	double dLength = GetLength(relative);
	if (2*dLength <= circleRect.height)
		return true;
	else
		return false;
}

void wxCircleRangeCtrl::CalculateHandlePositions()
{
	m_DragHandles.clear();
	m_DeleteHandles.clear();
	int iDivisionCount;
	vector<double>::iterator itDivision;
	wxRect circleRect = GetCircleRect();
	wxPoint Center = GetCircleCenter();
	for (itDivision = m_Divisions.begin(), iDivisionCount = 0; itDivision != m_Divisions.end(); ++itDivision, ++iDivisionCount)
	{
		double dAngle = *itDivision*2*PI;
		wxPoint LineVec(0.5*circleRect.width*cos(dAngle), -0.5*circleRect.height*sin(dAngle));
		wxPoint Edge = Center + LineVec;
		m_DragHandles.push_back(Edge);

		XY LineDir(LineVec.x, LineVec.y);
		if (LineDir.x || LineDir.y)
			Normalise(LineDir);
		wxPoint CrossCenter = Edge;
		CrossCenter.x += 10*LineDir.x;
		CrossCenter.y += 10*LineDir.y;

		m_DeleteHandles.push_back(CrossCenter);
	}
}

void wxCircleRangeCtrl::OnNumDivisionsChanged()
{
	// Resize to the correct size while freeing memory where necessary
	int iNewSize = max((int)m_Divisions.size(), 1);
	while ((int)m_SectionData.size() < iNewSize)
		m_SectionData.push_back(NULL);
	while ((int)m_SectionData.size() > iNewSize)
	{
		if (m_SectionData.back())
			delete m_SectionData.back();
		m_SectionData.pop_back();
	}
}


void wxCircleRangeCtrl::OnSize(wxSizeEvent& event)
{
	CalculateHandlePositions();
}

void wxCircleRangeCtrl::OnPaint(wxPaintEvent& event)
{
	// This avoids flickering... very nice
	wxBufferedPaintDC dc(this);
	dc.Clear();
//	wxPaintDC dc(this);
	wxRect circleRect = GetCircleRect();

	if (m_Divisions.empty())
	{
		dc.SetBrush(wxBrush(ConvertColor(GetIndexedColor(0))));
		dc.DrawEllipse(circleRect);
	}
	else
	{
		// Draw filled elliptical arcs
		int i;
		pair<double, double> Bounds;
		for (i = 0; i<(int)m_Divisions.size(); ++i)
		{
			Bounds = GetSectionBoundaries(i);
			dc.SetBrush(wxBrush(ConvertColor(GetIndexedColor(i))));
			dc.DrawEllipticArc(circleRect.x, circleRect.y, circleRect.width, circleRect.height, Bounds.first*360, 
						Bounds.second < Bounds.first ? Bounds.second*360+360 : Bounds.second *360);  // Caters for division which goes past 0 degrees
		}
		dc.SetBrush(*wxWHITE);
		wxPoint Center = GetCircleCenter();
		int iDivisionCount;
		vector<wxPoint>::iterator itPoint;
		// Draw lines
		for (itPoint = m_DragHandles.begin(), iDivisionCount = 0; itPoint != m_DragHandles.end(); ++itPoint, ++iDivisionCount)
		{
			dc.DrawLine(Center.x, Center.y, itPoint->x, itPoint->y);
		}
		// Draw drag handles
		for (itPoint = m_DragHandles.begin(), iDivisionCount = 0; itPoint != m_DragHandles.end(); ++itPoint, ++iDivisionCount)
		{
			if (iDivisionCount == m_iSelectedDivision)
				dc.SetBrush(*wxRED);
			dc.DrawCircle(*itPoint, m_iDragHandleRadius);
			if (iDivisionCount == m_iSelectedDivision)
				dc.SetBrush(*wxWHITE);
		}
		// Draw delete handles
		for (itPoint = m_DeleteHandles.begin(), iDivisionCount = 0; itPoint != m_DeleteHandles.end(); ++itPoint, ++iDivisionCount)
		{
			DrawCross(dc, *itPoint, m_DeleteHandleSize);
		}
/*		wxString AngleText;
		AngleText << m_dMouseAngle;
		dc.DrawText(AngleText, Center.x, Center.y);*/
	}
/*	dc.DrawEllipticArc(rect.x, rect.y, rect.width, rect.height, 0, 120);

	dc.SetBrush(*wxBLUE_BRUSH);
	dc.DrawEllipticArc(rect.x, rect.y, rect.width, rect.height, 200, 300);*/
}

void wxCircleRangeCtrl::AddDivision(double dFraction)
{
	m_Divisions.push_back(dFraction);
	sort(m_Divisions.begin(), m_Divisions.end());
	OnNumDivisionsChanged();
	CalculateHandlePositions();
	Refresh();
}

void wxCircleRangeCtrl::OnMouseEvent(wxMouseEvent& event)
{
	if (event.LeftUp() || event.Leaving())
	{
		if (m_iSelectedDivision != -1)
		{
			m_iSelectedDivision = -1;
			Refresh();

			wxCircleRangeCtrlEvent RangeCtrlEvent(wxEVT_COMMAND_DIVISION_END_DRAG, GetId());
			RangeCtrlEvent.m_iDivision = m_iSelectedDivision;
			RangeCtrlEvent.SetEventObject(this);
			GetEventHandler()->ProcessEvent(RangeCtrlEvent);

			return;
		}
	}
	if (event.LeftDown())
	{
		m_iSelectedDivision = -1;
		wxPoint mousePos = event.GetPosition();
		vector<wxPoint>::iterator itPoint;
		XY Relative;
		int iDivisionCount;
		for (itPoint = m_DragHandles.begin(), iDivisionCount = 0; itPoint != m_DragHandles.end(); ++itPoint, ++iDivisionCount)
		{
			Relative.x = itPoint->x - mousePos.x;
			Relative.y = itPoint->y - mousePos.y;
			if (GetLength(Relative) <= m_iDragHandleRadius)
			{
				m_iSelectedDivision = iDivisionCount;
				Refresh();

				wxCircleRangeCtrlEvent RangeCtrlEvent(wxEVT_COMMAND_DIVISION_BEGIN_DRAG, GetId());
				RangeCtrlEvent.m_iDivision = iDivisionCount;
				RangeCtrlEvent.SetEventObject(this);
				GetEventHandler()->ProcessEvent(RangeCtrlEvent);

				return;
			}
		}
		for (itPoint = m_DeleteHandles.begin(), iDivisionCount = 0; itPoint != m_DeleteHandles.end(); ++itPoint, ++iDivisionCount)
		{
			Relative.x = itPoint->x - mousePos.x;
			Relative.y = itPoint->y - mousePos.y;
			if (abs(2*Relative.x) <= m_DeleteHandleSize.x &&
				abs(2*Relative.y) <= m_DeleteHandleSize.y)
			{
				m_Divisions.erase(m_Divisions.begin()+iDivisionCount);
				m_DragHandles.erase(m_DragHandles.begin()+iDivisionCount);
				m_DeleteHandles.erase(m_DeleteHandles.begin()+iDivisionCount);
				OnNumDivisionsChanged();
				Refresh();

				wxCircleRangeCtrlEvent RangeCtrlEvent(wxEVT_COMMAND_DIVISION_DELETED, GetId());
				RangeCtrlEvent.m_iDivision = iDivisionCount;
				RangeCtrlEvent.SetEventObject(this);
				GetEventHandler()->ProcessEvent(RangeCtrlEvent);

				return;
			}
		}

		double dFraction = GetFraction(mousePos);
		bool bInside = IsInside(mousePos);
		if (bInside)
		{
			vector<double>::iterator itDiv;
			int i;
			for (itDiv = m_Divisions.begin(), i=0; itDiv != m_Divisions.end(); ++itDiv, ++i)
			{
				if (dFraction <= *itDiv)
					break;
			}
			--i;
			if (i < 0)
				i = max((int)m_Divisions.size()-1, 0);

			// Send an event with the division index that was clicked
			wxCircleRangeCtrlEvent RangeCtrlEvent(wxEVT_COMMAND_SECTION_CLICKED, GetId());
			RangeCtrlEvent.m_iSection = i;
			RangeCtrlEvent.SetEventObject(this);
			GetEventHandler()->ProcessEvent(RangeCtrlEvent);
		}
		else
		{
			AddDivision(dFraction);

			wxCircleRangeCtrlEvent RangeCtrlEvent(wxEVT_COMMAND_DIVISION_ADDED, GetId());
			RangeCtrlEvent.m_dFraction = dFraction;
			RangeCtrlEvent.SetEventObject(this);
			GetEventHandler()->ProcessEvent(RangeCtrlEvent);

			return;
		}
	}
	if (event.Dragging() && m_iSelectedDivision != -1)
	{
		// Drag the division
		wxPoint mousePos = event.GetPosition();
		double dFraction = GetFraction(mousePos);
		m_Divisions[m_iSelectedDivision] = dFraction;
		sort(m_Divisions.begin(), m_Divisions.end());
		vector<double>::iterator itDivision = find(m_Divisions.begin(), m_Divisions.end(), dFraction);
		if (itDivision != m_Divisions.end())
			m_iSelectedDivision = distance(m_Divisions.begin(), itDivision);
		else
			m_iSelectedDivision = -1;
		// Recalculate handle positions, this could be optimised to just calc the dragged handle pos
		CalculateHandlePositions();
		Refresh();

		wxCircleRangeCtrlEvent RangeCtrlEvent(wxEVT_COMMAND_DIVISION_DRAGGING, GetId());
		RangeCtrlEvent.m_iDivision = m_iSelectedDivision;
		RangeCtrlEvent.SetEventObject(this);
		GetEventHandler()->ProcessEvent(RangeCtrlEvent);

		return;
	}
}

void wxCircleRangeCtrl::DrawCross(wxDC &dc, wxPoint Center, wxSize Size)
{
	dc.DrawRectangle(Center.x-0.5*Size.x, Center.y-0.5*Size.y, Size.x, Size.y);
	dc.DrawLine(Center.x-0.5*Size.x, Center.y-0.5*Size.y, Center.x+0.5*Size.x, Center.y+0.5*Size.y);
	dc.DrawLine(Center.x-0.5*Size.x, Center.y+0.5*Size.y, Center.x+0.5*Size.x, Center.y-0.5*Size.y);
}

void wxCircleRangeCtrl::SetClientObject(int iSectionIndex, wxClientData *data)
{
	assert(iSectionIndex >= 0 && iSectionIndex < (int)m_SectionData.size());
	m_SectionData[iSectionIndex] = data;
}

wxClientData * wxCircleRangeCtrl::GetClientObject(int iSectionIndex)
{
	assert(iSectionIndex >= 0 && iSectionIndex < (int)m_SectionData.size());
	return m_SectionData[iSectionIndex];
}

pair<double, double> wxCircleRangeCtrl::GetSectionBoundaries(int iSection)
{
	int iDivisions = (int)m_Divisions.size();
	assert(iSection >= 0 && iSection < max(iDivisions, 1));
	if (iDivisions == 0)
		return make_pair(0.0, 0.0);
	if (iDivisions == 1)
		return make_pair(m_Divisions[0], m_Divisions[0]);
	int iDiv1 = iSection%iDivisions;
	int iDiv2 = (iSection+1)%iDivisions;
	return make_pair(m_Divisions[iDiv1], m_Divisions[iDiv2]);
}

















