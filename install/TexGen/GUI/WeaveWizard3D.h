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

#pragma once

class wxWeavePatternCtrl3D;
class wxWeavePatternCtrl;
class CSectionWindowPainter;

enum
{
	ORTHOGONAL,
	ANGLE_INTERLOCK,
	LAYER_TO_LAYER
};

struct YARN_PROPERTIES
{
	wxString	YarnLinearDensity;
	wxString	YarnLinearDensityUnits;
	wxString	FibreDensity;
	wxString	FibreDensityUnits;
	wxString	FibreArea;
	wxString	FibreAreaUnits;
	wxString	FibreDiameter;
	wxString	FibreDiameterUnits;
	wxString	FibresPerYarn;
};

class CWeaveWizard3D : public wxWizard
{
friend class CSectionWindowPainter;
public:
	CWeaveWizard3D(wxWindow* parent, wxWindowID id = wxID_ANY);
	~CWeaveWizard3D(void);

	bool RunIt();
	// Use the dialog input to create the string for the Python commands to build the textile
	string GetCreateTextileCommand(string ExistingTextile = "");
	void LoadSettings(const CTextile3DWeave &Weave);

protected:
	void OnWizardPageChanging(wxWizardEvent& event);
	void OnWeftLayersChanged(wxCommandEvent& event);
	void OnWeftYarnsChanged(wxCommandEvent& event);
	void OnWeaveTypeChanged(wxCommandEvent& event);
	void OnWarpYarnsChanged(wxCommandEvent& event);
	void OnOffsetWarpChanged(wxCommandEvent& event);
	void OnTextChanged(wxCommandEvent& event);
	void OnThicknessChanged(wxCommandEvent& event);
	void OnTargetThicknessChanged(wxCommandEvent& event);
	void OnMaxVolumeFractionChanged(wxCommandEvent& event);
	void OnRefine(wxCommandEvent& event);
	void OnWarpRatioChanged(wxCommandEvent& event);
	
	void BuildPages();
	wxWizardPageSimple* BuildFirstPage();
	wxWizardPageSimple* BuildWarpYarnPage();
	wxWizardPageSimple* BuildWeftYarnPage();
	wxWizardPageSimple* BuildBinderYarnPage();
	wxWizardPageSimple* BuildPropertiesPage( YARN_PROPERTIES& Properties, string Type );
	wxDialog* BuildWeavePatternDialog();
	bool RebuildWeavePatternCtrl( );
	int GetBinderCellBottom(int i, int j);
	double CalcThickness();

	void DrawSectionPreview();
	void DrawSectionPreview(wxDC &dc);

	bool HasNextPage(wxWizardPage *page);

	string ConvertProperties( YARN_PROPERTIES &Properties, int iYarnType, string PythonString );

	wxSpinCtrl *m_pWarpYarnsSpin;
	wxSpinCtrl *m_pWeftYarnsSpin;
	wxSpinCtrl *m_pWarpLayersSpin;
	wxSpinCtrl *m_pWeftLayersSpin;
	wxSpinCtrl *m_pBinderRatioSpin;
	wxSpinCtrl *m_pWarpRatioSpin;
	wxSpinCtrl *m_pBinderLayersSpin;
	wxWeavePatternCtrl3D *m_pWeavePatternCtrl;
	wxString m_WarpSpacing;
	wxString m_WeftSpacing;
	wxString m_WarpWidth;
	wxString m_WeftWidth;
	wxString m_WarpHeight;
	wxString m_WeftHeight;
	wxString m_BinderWidth;
	wxString m_BinderHeight;
	wxString m_BinderSpacing;
	wxString m_GapSize;
	wxString m_WarpPower;
	wxString m_WeftPower;
	wxString m_BinderPower;
	wxString m_Thickness;
	wxString m_MaxVolFraction;
	wxCheckBox *m_pOffsetWeftBox;
	wxCheckBox *m_pRefineBox;
	wxTextCtrl *m_pThicknessTextCtrl;
	wxTextCtrl *m_pVolFractionTextCtrl;
	wxTextCtrl *m_BinderWidthTextCtrl;
	wxTextCtrl *m_BinderHeightTextCtrl;
	wxTextCtrl *m_BinderPowerTextCtrl;
	wxTextCtrl *m_BinderSpacingTextCtrl;
	wxTextCtrl *m_GapSizeTextCtrl;
	bool m_bCreateDomain;
	bool m_bOffsetWeftYarns;
	bool m_bRefine;
	bool m_bAddedDomainHeight;

	wxRadioBox* m_WeaveTypeRadio;

	YARN_PROPERTIES m_WarpProperties;
	YARN_PROPERTIES m_WeftProperties;
	YARN_PROPERTIES m_BinderProperties;

	int m_WeaveType;
	bool m_BuiltPages;

	wxWizardPageSimple *m_pFirstPage;
	wxWizardPageSimple *m_pWarpYarnPage;
	wxWizardPageSimple *m_pWeftYarnPage;
	wxWizardPageSimple *m_pBinderYarnPage;
	//wxBinderYarnPage *m_pBinderYarnPage;
	wxWizardPageSimple *m_pWarpPropertiesPage;
	wxWizardPageSimple *m_pWeftPropertiesPage;
	wxWizardPageSimple *m_pBinderPropertiesPage;
	wxDialog *m_pWeavePatternDialog;

	DECLARE_EVENT_TABLE()
};


class CSectionWindowPainter : public wxEvtHandler
{
public:
	CSectionWindowPainter(CWeaveWizard3D &WeaveWizard3D ) : m_WeaveWizard3D(WeaveWizard3D) {}

protected:
	void OnPaint(wxPaintEvent& event);

	CWeaveWizard3D &m_WeaveWizard3D;

	DECLARE_EVENT_TABLE()
};



