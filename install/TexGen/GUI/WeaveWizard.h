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

class wxWeavePatternCtrl;

class CWeaveWizard : public wxWizard
{
public:
	CWeaveWizard(wxWindow* parent, wxWindowID id = wxID_ANY);
	~CWeaveWizard(void);

	bool RunIt();
	string GetCreateTextileCommand(string ExistingTextile = "");
	void LoadSettings(const CTextileWeave2D &Weave);
	void LoadSettings(const CShearedTextileWeave2D &Weave);

protected:
	void OnWizardPageChanging(wxWizardEvent& event);
	void BuildPages();
	wxWizardPageSimple* BuildFirstPage();
	wxDialog* BuildWeavePatternDialog();
	
	bool RebuildWeavePatternCtrl();
	bool GetPatternCell(int i, int j);
//	void SetPatternCell(int i, int j, bool bVal);
	bool HasNextPage(wxWizardPage *page);
	void OnWidthChanged(wxCommandEvent& event) { m_bWidthChanged = true; }
	void OnSpacingChanged(wxCommandEvent& event) { m_bSpacingChanged = true; }
	void OnThicknessChanged(wxCommandEvent& event) { m_bThicknessChanged = true; }
	void OnInit(wxInitDialogEvent& event) {m_bWidthChanged = m_bSpacingChanged = m_bThicknessChanged = false;}

	void OnRefine(wxCommandEvent& event);
	void On3DWeave(wxCommandEvent& event);
	void OnInPlaneTangents(wxCommandEvent& event);
	void OnWeavePattern(wxCommandEvent& event);
	void RefreshGapTextBox();
	void OnShear(wxCommandEvent& event);
	void OnDomain(wxCommandEvent& event);
	void OnShearedDomain(wxCommandEvent& event);
	void OnLayers(wxCommandEvent& event);

	wxSpinCtrl *m_pXYarnsSpin;
	wxSpinCtrl *m_pYYarnsSpin;
	wxWeavePatternCtrl *m_pWeavePatternCtrl;
	wxString m_YarnSpacing;
	wxString m_YarnWidth;
	wxString m_FabricThickness;
	wxString m_GapSize;
	wxString m_ShearAngle;
	wxSpinCtrl *m_pLayersSpin;

	bool m_bCreateDomain;
	bool m_bRefine;
	bool m_bInPlaneTangents;
	bool m_b3DWeave;
	bool m_bShear;
	bool m_bShearedDomain;
	bool m_bLayers;
	bool m_bAddedDomainHeight;

	bool m_bWidthChanged;
	bool m_bSpacingChanged;
	bool m_bThicknessChanged;

	wxWizardPageSimple *m_pFirstPage;
	wxDialog *m_pWeavePatternDialog;

	DECLARE_EVENT_TABLE()
};
