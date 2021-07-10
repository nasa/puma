/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2014 Louise Brown
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

class CPatternDraftDialog : public wxDialog
{
public:
	CPatternDraftDialog();
	CPatternDraftDialog( CPatternDraft& PatternDraft );
	~CPatternDraftDialog(void);

	void DrawPatternDraft(wxDC &dc);
	void PrintPatternDraft( wxDC &dc, wxRect& Rect );
	void SetPatternDraft( CPatternDraft& PatternDraft ) { m_PatternDraft = PatternDraft; }

protected:
	void BuildDialog();
	void OnOK(wxCommandEvent& event);
	//void OnInit(wxInitDialogEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnPrint( wxCommandEvent& event );
	void OnSave( wxCommandEvent& event );

	void CalculatePixelRatio( wxRect& Rect );
	void DrawWeaveRegion( wxDC &dc, wxRect& Rect );
	void DrawHeddleRegion( wxDC &dc, wxRect& ClientRect );
	void DrawTieUpRegion( wxDC &dc, wxRect& ClientRect );
	void DrawChainRegion( wxDC &dc, wxRect& ClientRect );
	void DrawGrid( wxDC &dc, wxRect &Rect, int iNumX, int iNumY );
	void DrawSquare( wxDC &dc, wxPoint &BottomLeft, int i, int j );
	

	wxPanel* m_pPatternPanel;
	CPatternDraft m_PatternDraft;
	int m_iPixelRatio;
	int m_iFreeWidth;
	int m_iFreeHeight;

	DECLARE_EVENT_TABLE()
};

class CPatternDraftPainter : public wxEvtHandler
{
public:
	CPatternDraftPainter(CPatternDraftDialog &PatternDraftDialog ) : m_PatternDraftDialog( PatternDraftDialog ) {}

protected:
	void OnPaint(wxPaintEvent& event);
	void OnPrint(wxCommandEvent& event);
	//void OnSize(wxSizeEvent& event);

	CPatternDraftDialog &m_PatternDraftDialog;

	DECLARE_EVENT_TABLE()
};

class CPatternDraftPrintout : public wxPrintout
{
public:
	CPatternDraftPrintout( int page_amount, wxString title, float units_per_cm, CPatternDraftDialog &PatternDraftDialog);
	bool PerformPageSetup(const bool showPageSetupDialog);
	void OnBeginPrinting();
	wxPrintData GetPrintData();
	bool OnBeginDocument(int startPage, int endPage);
	void GetPageInfo(int *minPage, int *maxPage, int *pageSelFrom, int *pageSelTo);
	bool HasPage(int pageNum);
	bool OnPrintPage(int pageNum);
	void OnEndPrinting();

protected:
	CPatternDraftDialog &m_PatternDraftDialog;
	wxPageSetupDialogData m_page_setup;
 
    /** the type of paper (letter, A4, etc...) */
    wxPaperSize m_paper_type;
 
    /** horizontal or vertical */
    int m_orient;
	 int m_page_amount;
 
    /** margins, in millimeters */
    int m_margin_left, m_margin_right, m_margin_top, m_margin_bottom;
 
    /** we'll use this to determine the coordinate system; it describes the number of units per
      * centimeter (i.e. how fine the coordinate system is)
      */
    float m_units_per_cm;
 
    /** will contain the dimensions of the coordinate system, once it's calculated.
      * in the printing callback, you can then draw from (0, 0) to (m_coord_system_width, m_coord_system_height),
      * which will be the area covering the paper minus the margins
      */
    int m_coord_system_width, m_coord_system_height;
};