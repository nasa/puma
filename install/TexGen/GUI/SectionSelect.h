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

class CPreviewWindowPainter;
class wxCircleRangeCtrlEvent;

class CSectionItem : public wxClientData
{
public:
	CSectionItem(const CSection &Section = CSectionEllipse(1, 1)) : m_Section(Section) {}
	~CSectionItem(void) {;}

	CObjectContainer<CSection> m_Section;
};

class CSectionSelect : public wxDialog
{
friend class CPreviewWindowPainter;
public:
	CSectionSelect();
	~CSectionSelect(void);

	string GetCreateSectionCommand(string SectionVariableName = "section");
	void LoadSettings(const CSection &Section);
	const CSection &GetSection();
	static void UpdateSection(CObjectContainer<CSection> &Section);

//	wxString GetWidth() { return m_Width; }

protected:
	void BuildDialog();
	void OnOK(wxCommandEvent& event);
//	void OnCancel(wxCommandEvent& event);
	void OnInit(wxInitDialogEvent& event);
	void OnRotate(wxCommandEvent& event);
	void OnScale(wxCommandEvent& event);
	void OnTextChanged(wxCommandEvent &event);
	void OnSectionTypeChanged(wxChoicebookEvent& event);
	void OnDivisionChanged(wxCircleRangeCtrlEvent& event);
	void OnSectionClicked(wxCircleRangeCtrlEvent& event);

	void RefreshTextBoxes();
	void DrawSectionPreview();
	void DrawSectionPreview(wxDC &dc);
	void UpdateSectionFromDialog();
	void BuildHybridSection();

	wxString m_Width;
	wxString m_Height;
	wxString m_Distortion;
	wxString m_Power;
	wxString m_Angle;
	wxString m_XScale;
	wxString m_YScale;
	wxString m_XOffset;
	wxPanel* m_pPreviewPanel;

	bool m_bRotate;
	bool m_bScale;

	bool m_bFinishedInit;

	CObjectContainer<CSection> m_Section;

	DECLARE_EVENT_TABLE()
};

class CPreviewWindowPainter : public wxEvtHandler
{
public:
	CPreviewWindowPainter(CSectionSelect &SectionSelect) : m_SectionSelect(SectionSelect) {}

protected:
	void OnPaint(wxPaintEvent& event);

	CSectionSelect &m_SectionSelect;

	DECLARE_EVENT_TABLE()
};








