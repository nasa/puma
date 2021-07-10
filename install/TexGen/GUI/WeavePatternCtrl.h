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

/// Weave pattern control used for setting the weave pattern
class wxWeavePatternCtrl : public wxControl
{
	DECLARE_DYNAMIC_CLASS(wxWeavePatternCtrl)
	DECLARE_EVENT_TABLE()

public:
	// Constructors
	wxWeavePatternCtrl();
	wxWeavePatternCtrl(wxWindow* parent, wxWindowID id,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxSUNKEN_BORDER|wxFULL_REPAINT_ON_RESIZE,
		const wxValidator& validator = wxDefaultValidator);
	~wxWeavePatternCtrl();
	// Creation
	bool Create(wxWindow* parent, wxWindowID id,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxSUNKEN_BORDER|wxFULL_REPAINT_ON_RESIZE,
		const wxValidator& validator = wxDefaultValidator);

	/// Sets the mode to 3d, this must be called before set weave size
	void SetWeaveSize(int iNumYYarns, int iNumXYarns, bool b3DMode);
	void SetWeaveSize(const CTextileWeave &Weave);
	int GetWeaveWidth();
	int GetWeaveHeight();
	bool GetCellStatus(int i, int j);
//	void SetCellStatus(int i, int j, bool bVal);
	bool bHasWeave() { return m_WeavePattern?true:false; }
	CTextileWeave &GetWeave() { return *m_WeavePattern; }
	bool Get3DMode() { return m_b3DMode; }

protected:
	enum
	{
		MENUID_ADDLAYER = 1,
		MENUID_REMOVELAYER,
		MENUID_SETYARNWIDTH,
		MENUID_SETYARNHEIGHT,
		MENUID_SETYARNSPACING,
	};

	struct YARN_POS_DATA
	{
		int iCellMin, iYarnMin, iYarnCenter, iYarnMax, iCellMax;
	};

	// Overrides
	wxSize DoGetBestSize() const { return wxSize(600, 400); }
	// Event handlers
	void OnPaint(wxPaintEvent& event);
	void OnMouseEvent(wxMouseEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMenu(wxCommandEvent& event);

	void Init();
	pair<int, int> GetCellIndices(wxPoint Position);
	pair<int, int> GetCellHeight(wxPoint Position);
	bool HandleIconSelect(wxPoint Position);
	void HandleIconSelect(wxRect MarqueeSelect);
	wxRect GetLeftIconsRegion();
	wxRect GetTopIconsRegion();
	wxRect GetTopIconRect(int i);
	wxRect GetLeftIconRect(int i);
	wxRect GetWeaveRegion();
	wxRect GetCrossSectionRegion();
	wxRect GetCellRegion(int x, int y);
	wxPoint GetCrossSectionPos(int iIndex, double dZValue);
	wxRect GetMarqueeRect();
	YARN_POS_DATA GetXCoordinate(int iIndex);
	YARN_POS_DATA GetYCoordinate(int iIndex);
	int GetNumberOfYarns(int i, int j);
	void DrawTopDownView(wxDC &dc);
	void DrawCell(wxDC &dc, int i, int j, bool bDirection);
	void DrawCrossSection(wxDC &dc, int iIndex);
	void DrawHorizontalYarn(wxDC &dc, vector<double> &YarnZValues);
	void DrawVerticalYarn(wxDC &dc, int iIndex, double dZValue);
	void DrawLeftIcons(wxDC &dc);
	void DrawTopIcons(wxDC &dc);
	int GetNumSelected();
	void CalculatePixelRatio();

	double m_dPixelRatio;
	int m_iFreeWidth;
	int m_iFreeHeight;
	double m_dLastSelectedWidth;
	double m_dLastSelectedHeight;
	double m_dLastSelectedSpacing;
	CObjectContainer<CTextileWeave> m_WeavePattern;
	// The amount of space left for the icons
	int m_iIconSpace;
	int m_iBorderSpace;
	bool m_b3DMode;
	// These refers to the cross section index along the y axis
	int m_iCrossSectionIndex;
	// These refer to indices in the cross section
	// the first is the index along the length of the yarn in the x axis
	// the second is the height index
	pair<int, int> m_SelectedIndices;
	vector<bool> m_SelectedYYarns;
	vector<bool> m_SelectedXYarns;
	wxPoint m_MarqueeStart;
	wxPoint m_MarqueeEnd;
};



/// Circle range control event class
class wxWeavePatternCtrlEvent : public wxCommandEvent
{
friend class wxWeavePatternCtrl;
public:
	wxWeavePatternCtrlEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
	: wxCommandEvent(commandType, id)
	{}
	wxWeavePatternCtrlEvent(const wxWeavePatternCtrlEvent& event)
	: wxCommandEvent(event)
	{}
	virtual wxEvent *Clone() const
	{
		return new wxWeavePatternCtrlEvent(*this);
	}

protected:
	DECLARE_DYNAMIC_CLASS(wxWeavePatternCtrlEvent);
};

typedef void (wxEvtHandler::*wxCircleRangeCtrlEventFunction) (wxWeavePatternCtrlEvent&);

// Circle range control events and macros for handling them
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_COMMAND_CROSSOVER_CLICKED, NULL)
END_DECLARE_EVENT_TYPES()

#define EVT_CROSSOVER_CLICKED(id, fn) DECLARE_EVENT_TABLE_ENTRY( \
wxEVT_COMMAND_CROSSOVER_CLICKED, id, -1, (wxObjectEventFunction) \
(wxEventFunction) (wxCircleRangeCtrlEventFunction) & fn, \
(wxObject *) NULL ),















