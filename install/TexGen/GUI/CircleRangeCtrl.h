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

#pragma once

/// Circle range control used for hybrid cross section
class wxCircleRangeCtrl : public wxControl
{
	DECLARE_DYNAMIC_CLASS(wxCircleRangeCtrl)
	DECLARE_EVENT_TABLE()

public:
	// Constructors
	wxCircleRangeCtrl();
	wxCircleRangeCtrl(wxWindow* parent, wxWindowID id,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxSUNKEN_BORDER,
		const wxValidator& validator = wxDefaultValidator);
	~wxCircleRangeCtrl();
	// Creation
	bool Create(wxWindow* parent, wxWindowID id,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxSUNKEN_BORDER,
		const wxValidator& validator = wxDefaultValidator);

	void SetClientObject(int iSectionIndex, wxClientData *data);
	wxClientData * GetClientObject(int iSectionIndex);
	pair<double, double> GetSectionBoundaries(int iSection);
	void AddDivision(double dFraction);

	// Accessor methods
	int GetNumDivisions() { return (int)m_Divisions.size(); }
	int GetNumSections() { return max((int)m_Divisions.size(), 1); }
	const vector<double> &GetDivisions() { return m_Divisions; }

protected:
	// Overrides
	wxSize DoGetBestSize() const { return wxSize(200, 40); }
	// Event handlers
	void OnPaint(wxPaintEvent& event);
	void OnMouseEvent(wxMouseEvent& event);
	void OnSize(wxSizeEvent& event);
	wxColour ConvertColor(COLOR Color)
	{
		return wxColour(Color.r*255, Color.g*255, Color.b*255);
	}

	void Init();
	void DrawCross(wxDC &dc, wxPoint Center, wxSize Size);
	wxRect GetCircleRect();
	wxPoint GetCircleCenter();
	void CalculateHandlePositions();
	double GetFraction(wxPoint Position);
	bool IsInside(wxPoint Position);
	void OnNumDivisionsChanged();

	vector<double> m_Divisions;
	vector<wxPoint> m_DragHandles;
	vector<wxPoint> m_DeleteHandles;
	vector<wxClientData*> m_SectionData;
	int m_iSelectedDivision;
	int m_iDragHandleRadius;
	wxSize m_DeleteHandleSize;
};



/// Circle range control event class
class wxCircleRangeCtrlEvent : public wxCommandEvent
{
friend class wxCircleRangeCtrl;
public:
	wxCircleRangeCtrlEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
	: wxCommandEvent(commandType, id)
	, m_iSection(-1)
	, m_iDivision(-1)
	, m_dFraction(-1)
	{}
	wxCircleRangeCtrlEvent(const wxCircleRangeCtrlEvent& event)
	: wxCommandEvent(event)
	, m_iSection(-1)
	, m_iDivision(-1)
	, m_dFraction(-1)
	{}
	virtual wxEvent *Clone() const
	{
		return new wxCircleRangeCtrlEvent(*this);
	}

	int GetSection() { return m_iSection; }
	int GetDivision() { return m_iDivision; }
	double GetFraction() { return m_dFraction; }

protected:
	int m_iSection;
	int m_iDivision;
	double m_dFraction;

	DECLARE_DYNAMIC_CLASS(wxCircleRangeCtrlEvent);
};

typedef void (wxEvtHandler::*wxCircleRangeCtrlEventFunction) (wxCircleRangeCtrlEvent&);

// Circle range control events and macros for handling them
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_COMMAND_DIVISION_ADDED, NULL)
	DECLARE_EVENT_TYPE(wxEVT_COMMAND_DIVISION_DELETED, NULL)
	DECLARE_EVENT_TYPE(wxEVT_COMMAND_DIVISION_BEGIN_DRAG, NULL)
	DECLARE_EVENT_TYPE(wxEVT_COMMAND_DIVISION_DRAGGING, NULL)
	DECLARE_EVENT_TYPE(wxEVT_COMMAND_DIVISION_END_DRAG, NULL)
	DECLARE_EVENT_TYPE(wxEVT_COMMAND_SECTION_CLICKED, NULL)
END_DECLARE_EVENT_TYPES()

#define EVT_DIVISION_ADDED(id, fn) DECLARE_EVENT_TABLE_ENTRY( \
wxEVT_COMMAND_DIVISION_ADDED, id, -1, (wxObjectEventFunction) \
(wxEventFunction) (wxCircleRangeCtrlEventFunction) & fn, \
(wxObject *) NULL ),

#define EVT_DIVISION_DELETED(id, fn) DECLARE_EVENT_TABLE_ENTRY( \
wxEVT_COMMAND_DIVISION_DELETED, id, -1, (wxObjectEventFunction) \
(wxEventFunction) (wxCircleRangeCtrlEventFunction) & fn, \
(wxObject *) NULL ),

#define EVT_DIVISION_BEGIN_DRAG(id, fn) DECLARE_EVENT_TABLE_ENTRY( \
wxEVT_COMMAND_DIVISION_BEGIN_DRAG, id, -1, (wxObjectEventFunction) \
(wxEventFunction) (wxCircleRangeCtrlEventFunction) & fn, \
(wxObject *) NULL ),

#define EVT_DIVISION_DRAGGING(id, fn) DECLARE_EVENT_TABLE_ENTRY( \
wxEVT_COMMAND_DIVISION_DRAGGING, id, -1, (wxObjectEventFunction) \
(wxEventFunction) (wxCircleRangeCtrlEventFunction) & fn, \
(wxObject *) NULL ),

#define EVT_DIVISION_END_DRAG(id, fn) DECLARE_EVENT_TABLE_ENTRY( \
wxEVT_COMMAND_DIVISION_END_DRAG, id, -1, (wxObjectEventFunction) \
(wxEventFunction) (wxCircleRangeCtrlEventFunction) & fn, \
(wxObject *) NULL ),

#define EVT_SECTION_CLICKED(id, fn) DECLARE_EVENT_TABLE_ENTRY( \
wxEVT_COMMAND_SECTION_CLICKED, id, -1, (wxObjectEventFunction) \
(wxEventFunction) (wxCircleRangeCtrlEventFunction) & fn, \
(wxObject *) NULL ),














