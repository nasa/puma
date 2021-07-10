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
#include "WeaveWizard3D.h"
#include "WindowIDs.h"
#include "WeavePatternCtrl3D.h"
#include "WeavePatternCtrl.h"
#include "PythonConverter.h"

#include "WizardTexGen.xpm"
#include "RangeValidator.h"

#include "Orthogonal.xpm"
#include "AngleInterlock.xpm"
#include "OffsetAngleInterlock.xpm"
#include "LayerToLayer.xpm"

using namespace TexGen;

BEGIN_EVENT_TABLE(CWeaveWizard3D, wxWizard)
EVT_WIZARD_PAGE_CHANGING(wxID_ANY, CWeaveWizard3D::OnWizardPageChanging)
EVT_TEXT(ID_WeftLayers, CWeaveWizard3D::OnWeftLayersChanged)
EVT_TEXT(ID_WeftYarns, CWeaveWizard3D::OnWeftYarnsChanged)
EVT_RADIOBOX(ID_WeaveType, CWeaveWizard3D::OnWeaveTypeChanged)
EVT_TEXT(ID_WarpYarns, CWeaveWizard3D::OnWarpYarnsChanged)
EVT_CHECKBOX( ID_OffsetWarpYarn, CWeaveWizard3D::OnOffsetWarpChanged)
EVT_TEXT(ID_WarpWidth, CWeaveWizard3D::OnTextChanged)
EVT_TEXT(ID_WarpHeight, CWeaveWizard3D::OnTextChanged)
EVT_TEXT(ID_WarpPower, CWeaveWizard3D::OnTextChanged)
EVT_TEXT(ID_WeftWidth, CWeaveWizard3D::OnTextChanged)
EVT_TEXT(ID_WeftHeight, CWeaveWizard3D::OnTextChanged)
EVT_TEXT(ID_WeftPower, CWeaveWizard3D::OnTextChanged)
EVT_TEXT(ID_BinderWidth, CWeaveWizard3D::OnTextChanged)
EVT_TEXT(ID_BinderHeight, CWeaveWizard3D::OnTextChanged)
EVT_TEXT(ID_BinderPower, CWeaveWizard3D::OnTextChanged)
EVT_TEXT(ID_GapSize, CWeaveWizard3D::OnTextChanged)
EVT_TEXT(ID_WarpLayers, CWeaveWizard3D::OnThicknessChanged)
EVT_TEXT(ID_TargetThickness, CWeaveWizard3D::OnTargetThicknessChanged)
//EVT_TEXT(ID_MaxVolumeFraction, CWeaveWizard3D::OnMaxVolumeFractionChanged)
EVT_CHECKBOX(ID_Refine3D, CWeaveWizard3D::OnRefine)
EVT_TEXT(ID_WarpRatio, CWeaveWizard3D::OnWarpRatioChanged)
EVT_TEXT(ID_BinderRatio, CWeaveWizard3D::OnWarpYarnsChanged)

END_EVENT_TABLE()

BEGIN_EVENT_TABLE(CSectionWindowPainter, wxEvtHandler)
	EVT_PAINT(CSectionWindowPainter::OnPaint)
END_EVENT_TABLE()

CWeaveWizard3D::CWeaveWizard3D(wxWindow* parent, wxWindowID id)
: wxWizard(parent, id, wxT("3D Weave Wizard"), wxBitmap(WizardTexGen_xpm))
, m_pFirstPage(NULL)
, m_WeaveType(ORTHOGONAL)
, m_pWeavePatternDialog(NULL)
, m_pWeavePatternCtrl(NULL)
, m_WarpSpacing(wxT("1"))
, m_WarpWidth(wxT("0.8"))
, m_WarpHeight(wxT("0.1"))
, m_WeftSpacing(wxT("1"))
, m_WeftWidth(wxT("0.8"))
, m_WeftHeight(wxT("0.1"))
, m_BinderWidth(wxT("0.4"))
, m_BinderHeight(wxT("0.05"))
, m_BinderSpacing(wxT("0.5"))
, m_GapSize(wxT("0.0"))
, m_bCreateDomain(true)
, m_WarpPower(wxT("0.6"))
, m_WeftPower(wxT("0.6"))
, m_BinderPower(wxT("0.6"))
, m_Thickness(wxT("0.0"))
, m_BuiltPages(false)
{
	BuildPages();
	GetPageAreaSizer()->Add(m_pFirstPage);
}

CWeaveWizard3D::~CWeaveWizard3D(void)
{
	if (m_pWeavePatternDialog)
		m_pWeavePatternDialog->Destroy();
}

void CWeaveWizard3D::BuildPages()
{
	m_pFirstPage = BuildFirstPage();
	m_pBinderYarnPage = BuildBinderYarnPage();  // Need to build binder page before warp so that OnWarpYarnsChanged works
	
	m_pWeftYarnPage = BuildWeftYarnPage();
	m_pWarpYarnPage = BuildWarpYarnPage();
	m_pThicknessTextCtrl->SetValue( ConvertString(stringify( CalcThickness(), 6, false )) ); 

	m_pWarpPropertiesPage = BuildPropertiesPage( m_WarpProperties, "warp" );
	m_pWeftPropertiesPage = BuildPropertiesPage( m_WeftProperties, "weft" );
	m_pBinderPropertiesPage = BuildPropertiesPage( m_BinderProperties, "binder" );
	
	m_pWeavePatternDialog = BuildWeavePatternDialog();

	wxWizardPageSimple::Chain(m_pFirstPage, m_pWeftYarnPage);
	wxWizardPageSimple::Chain(m_pWeftYarnPage, m_pWarpYarnPage);
	wxWizardPageSimple::Chain(m_pWarpYarnPage, m_pBinderYarnPage);
	wxWizardPageSimple::Chain(m_pBinderYarnPage, m_pWeftPropertiesPage);
	wxWizardPageSimple::Chain(m_pWeftPropertiesPage, m_pWarpPropertiesPage);
	wxWizardPageSimple::Chain(m_pWarpPropertiesPage, m_pBinderPropertiesPage);
	m_BuiltPages = true;
}

wxWizardPageSimple* CWeaveWizard3D::BuildFirstPage()
{
	wxWizardPageSimple *pPage = new wxWizardPageSimple(this);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
	SizerFlags.Border();
	SizerFlags.Expand();

	wxSizer *pSubSizer;

	pMainSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("This wizard will take you through the process of creating a 3D textile weave model.")), SizerFlags);

	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);

	wxArrayString strings;
	strings.Add(wxT("&Orthogonal"));
	strings.Add(wxT("&Angle Interlock"));
	strings.Add(wxT("&Layer to Layer"));

	
	pSubSizer = new wxFlexGridSizer(3);
	pSubSizer->Add( m_WeaveTypeRadio = new wxRadioBox( pPage, ID_WeaveType, wxT("Weave type"), wxDefaultPosition, wxDefaultSize, strings, 0, wxRA_VERTICAL), SizerFlags);
	
	pSubSizer->Add( new wxStaticBitmap( pPage, wxID_STATIC, wxBitmap(Orthogonal_xpm) ), SizerFlags );
	pSubSizer->Add(new wxStaticText(pPage, ID_OrthogonalText, wxT("Orthogonal\nBinder yarns are either at top or bottom\ntravelling vertically through layers.")), SizerFlags);
	pSubSizer->AddSpacer(0);
	pSubSizer->Add( new wxStaticBitmap( pPage, wxID_STATIC, wxBitmap(OffsetAngleInterlock_xpm) ), SizerFlags );
	pSubSizer->Add(new wxStaticText(pPage, ID_OffsetAngleInterlockText, wxT("Angle Interlock\nBinder travels in fixed pattern from top to bottom.\nNumber of wefts yarns and weft layers are linked.")), SizerFlags);
	pSubSizer->AddSpacer(0);
	pSubSizer->Add( new wxStaticBitmap( pPage, wxID_STATIC, wxBitmap(AngleInterlock_xpm) ), SizerFlags );
	pSubSizer->Add(new wxStaticText(pPage, ID_AngleInterlockText, wxT("Angle Interlock\nDeselect Offset Weft Yarns option on next\npage to give vertically stacked weft yarns.\nUse layer to layer if more flexibility is\nrequired for binder yarn path")), SizerFlags);
	pSubSizer->AddSpacer(0);
	pSubSizer->Add( new wxStaticBitmap( pPage, wxID_STATIC, wxBitmap(LayerToLayer_xpm) ), SizerFlags );
	pSubSizer->Add(new wxStaticText(pPage, ID_LayerToLayerText, wxT("Layer to Layer\nOne or more binder yarns in stack.\nBinder path is selected interactively.\nUse this option where specific binder\npath is required.")), SizerFlags);
	pMainSizer->Add(pSubSizer, SizerFlags);

	pPage->SetSizer(pMainSizer);
	pMainSizer->Fit(pPage);

	FindWindow(ID_OffsetAngleInterlockText)->Disable();
	FindWindow(ID_AngleInterlockText)->Disable();
	FindWindow(ID_LayerToLayerText)->Disable();
	return pPage;
}

wxWizardPageSimple* CWeaveWizard3D::BuildWarpYarnPage()
{
	wxWizardPageSimple *pPage = new wxWizardPageSimple(this);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
	SizerFlags.Border();
	SizerFlags.Expand();

	wxSizer *pSubSizer;

	{
		pSubSizer = new wxFlexGridSizer(2);
		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Total number of yarns in warp direction (Both warp and binder yarns):")), SizerFlags);
		pSubSizer->Add(m_pWarpYarnsSpin = new wxSpinCtrl(pPage, ID_WarpYarns, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100), SizerFlags);
		m_pWarpYarnsSpin->SetToolTip(wxT("Controls the number of yarns in the warp direction in the unit cell. (Warp + binder yarns)"));

		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	pMainSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Enter a warp ratio of 0 to make all yarns in warp direction binder yarns")), SizerFlags);

	{
		pSubSizer = new wxFlexGridSizer(4);
		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Ratio of Binder Yarns:")), SizerFlags);
		pSubSizer->Add(m_pBinderRatioSpin = new wxSpinCtrl(pPage, ID_BinderRatio, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100), SizerFlags);
		m_pBinderRatioSpin->SetToolTip(wxT("Controls the ratio of binder yarns to warp yarns."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("to Warp Yarns:")), SizerFlags);
		pSubSizer->Add(m_pWarpRatioSpin = new wxSpinCtrl(pPage, ID_WarpRatio, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100), SizerFlags);
		m_pWarpRatioSpin->SetToolTip(wxT("Controls the ratio of binder yarns to warp yarns."));

		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	pMainSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Enter the data for the warp yarns.")), SizerFlags);

	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);
	{
		pSubSizer = new wxFlexGridSizer(2);

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Number of Yarn Layers:")), SizerFlags);
		pSubSizer->Add(m_pWarpLayersSpin = new wxSpinCtrl(pPage, ID_WarpLayers, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100), SizerFlags);
		m_pWarpLayersSpin->SetToolTip(wxT("Sets the number of layers of warp yarns."));

		wxTextCtrl* pControl;
		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Spacing:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_WarpSpacing, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_WarpSpacing)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the spacing in which the warp yarns are centred."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Width:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_WarpWidth, wxT("0.8"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_WarpWidth)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the width of the warp yarns."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Height:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_WarpHeight, wxT("0.1"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_WarpHeight)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the height of the warp yarns."));

		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);
	{
		pSubSizer = new wxFlexGridSizer(3);
		wxTextCtrl* pControl;

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Power Ellipse Section Power:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_WarpPower, wxT("0.6"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_WarpPower)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the power for the warp yarn cross-sections."));

		wxPanel* Panel;
		pSubSizer->Add(Panel = new wxPanel(pPage, ID_WarpPanel, wxDefaultPosition, wxSize(150,50)));
		Panel->PushEventHandler( new CSectionWindowPainter( *this ) );

		pMainSizer->Add(pSubSizer, SizerFlags);
	}
	
	pPage->SetSizer(pMainSizer);
	pMainSizer->Fit(pPage);

	m_pWarpYarnsSpin->SetValue(3);
	m_pWarpLayersSpin->SetValue(2);
	m_pBinderRatioSpin->SetValue(1);
	m_pWarpRatioSpin->SetValue(2);

	return pPage;
}

wxWizardPageSimple* CWeaveWizard3D::BuildWeftYarnPage()
{
	wxWizardPageSimple *pPage = new wxWizardPageSimple(this);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
	SizerFlags.Border();
	SizerFlags.Expand();

	wxSizer *pSubSizer;

	pMainSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Enter the data for the weft yarns.")), SizerFlags);

	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);
	{
		pSubSizer = new wxFlexGridSizer(2);

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarns:")), SizerFlags);
		pSubSizer->Add(m_pWeftYarnsSpin = new wxSpinCtrl(pPage, ID_WeftYarns, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100), SizerFlags);
		m_pWeftYarnsSpin->SetToolTip(wxT("Controls the number of weft yarns contained in the unit cell."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Number of Yarn Layers:")), SizerFlags);
		pSubSizer->Add(m_pWeftLayersSpin = new wxSpinCtrl(pPage, ID_WeftLayers, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100), SizerFlags);
		m_pWeftLayersSpin->SetToolTip(wxT("Sets the number of layers of weft yarns."));

		wxTextCtrl* pControl;
		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Spacing:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_WeftSpacing, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_WeftSpacing)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the spacing in which the weft yarns are centred."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Width:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_WeftWidth, wxT("0.8"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_WeftWidth)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the width of the weft yarns."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Height:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_WeftHeight, wxT("0.1"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_WeftHeight)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the height of the weft yarns."));

		

		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	{
		pSubSizer = new wxFlexGridSizer(3);
		wxTextCtrl* pControl;

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Power Ellipse Section Power:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_WeftPower, wxT("0.6"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_WeftPower)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the power for the weft yarn cross-sections."));

		wxPanel* Panel;
		pSubSizer->Add(Panel = new wxPanel(pPage, ID_WeftPanel, wxDefaultPosition, wxSize(150,50)));
		Panel->PushEventHandler( new CSectionWindowPainter( *this ) );

		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	pSubSizer = new wxFlexGridSizer(2);
	{
		pSubSizer->Add(m_pOffsetWeftBox = new wxCheckBox(pPage, ID_OffsetWarpYarn, wxT("Offset weft yarns"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bOffsetWeftYarns)), SizerFlags);

		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	pPage->SetSizer(pMainSizer);
	pMainSizer->Fit(pPage);

	m_pWeftYarnsSpin->SetValue(2);
	m_pWeftLayersSpin->SetValue(3);
	m_pOffsetWeftBox->Disable();
	m_pOffsetWeftBox->SetValue(true);

	return pPage;
}

wxWizardPageSimple* CWeaveWizard3D::BuildBinderYarnPage()
{
	wxWizardPageSimple *pPage = new wxWizardPageSimple(this);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
	SizerFlags.Border();
	SizerFlags.Expand();

	wxSizer *pSubSizer;
	pMainSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Enter the data for the binder yarns.")), SizerFlags);
	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);

	{
		pSubSizer = new wxFlexGridSizer(2);

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Width:")), SizerFlags);
		pSubSizer->Add(m_BinderWidthTextCtrl = new wxTextCtrl(pPage, ID_BinderWidth, wxT("0.8"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_BinderWidth)), SizerFlags);
		m_BinderWidthTextCtrl->SetToolTip(wxT("Sets the width of the binder yarns."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Height:")), SizerFlags);
		pSubSizer->Add(m_BinderHeightTextCtrl = new wxTextCtrl(pPage, ID_BinderHeight, wxT("0.05"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_BinderHeight)), SizerFlags);
		m_BinderHeightTextCtrl->SetToolTip(wxT("Sets the height of the binder yarns."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Spacing:")), SizerFlags);
		pSubSizer->Add(m_BinderSpacingTextCtrl = new wxTextCtrl(pPage, ID_BinderSpacing, wxT("0.5"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_BinderSpacing)), SizerFlags);
		m_BinderSpacingTextCtrl->SetToolTip(wxT("Sets the spacing in which the binder yarns are centred."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Gap size:")), SizerFlags);
		pSubSizer->Add(m_GapSizeTextCtrl = new wxTextCtrl(pPage, ID_GapSize, wxT("0.0"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_GapSize)), SizerFlags);
		m_GapSizeTextCtrl->SetToolTip(wxT("Sets the spacing between yarns at contact points."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Number of Binder Yarn Layers:")), SizerFlags);
		pSubSizer->Add(m_pBinderLayersSpin = new wxSpinCtrl(pPage, ID_BinderLayers, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100), SizerFlags);
		m_pBinderLayersSpin->SetToolTip(wxT("Sets the number of layers of binder yarns."));
		m_pBinderLayersSpin->Disable();  // Defaults to orthogonal at start

		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	{
		pSubSizer = new wxFlexGridSizer(3);

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Power Ellipse Section Power:")), SizerFlags);
		pSubSizer->Add(m_BinderPowerTextCtrl = new wxTextCtrl(pPage, ID_BinderPower, wxT("0.6"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_BinderPower)), SizerFlags);
		m_BinderPowerTextCtrl->SetToolTip(wxT("Sets the power for the binder yarn cross-sections."));

		wxPanel* Panel;
		pSubSizer->Add(Panel = new wxPanel(pPage, ID_BinderPanel, wxDefaultPosition, wxSize(150,50)));
		Panel->PushEventHandler( new CSectionWindowPainter( *this ) );

		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);
	{
		pSubSizer = new wxFlexGridSizer(3);

		pSubSizer->Add(m_pRefineBox = new wxCheckBox(pPage, ID_Refine3D, wxT("Refine"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bRefine)), SizerFlags);
		m_pRefineBox->SetValue(true);
		m_pRefineBox->SetToolTip(wxT("Refine textile to achieve target thickness"));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Target thickness:")), SizerFlags);
		pSubSizer->Add(m_pThicknessTextCtrl = new wxTextCtrl(pPage, ID_TargetThickness, wxT("0.0"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_Thickness)), SizerFlags);
		m_pThicknessTextCtrl->SetToolTip(wxT("Sets the target thickness for the final textile."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Maximum yarn volume fraction:")), SizerFlags);
		pSubSizer->Add(m_pVolFractionTextCtrl = new wxTextCtrl(pPage, ID_MaxVolumeFraction, wxT("0.78"), wxDefaultPosition, wxDefaultSize, 0, RangeValidator(&m_MaxVolFraction, 0.0, 1.0)), SizerFlags);
		m_pVolFractionTextCtrl->SetToolTip(wxT("Sets the maximum yarn volume fraction. Must be in range 0 to 1"));

		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	SizerFlags.Align(0);

	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);
	pSubSizer = new wxFlexGridSizer(2);
	{
		wxCheckBox* pDomainBox;
		wxCheckBox* pAddedDomainHeight;
		pSubSizer->Add(pDomainBox = new wxCheckBox(pPage, wxID_ANY, wxT("Create default domain"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bCreateDomain)), SizerFlags);
		pDomainBox->SetValue(true);
		pDomainBox->SetToolTip(wxT("Assign the default domain to the textile. The default domain is the same size as the unit cell in the x and y directions and corresponds to the fabric thickness in the z direction."));
		pSubSizer->Add(pAddedDomainHeight = new wxCheckBox(pPage, wxID_ANY, wxT("Add 10% to domain height"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bAddedDomainHeight)), SizerFlags);
		pAddedDomainHeight->SetValue(true);
		pAddedDomainHeight->SetToolTip(wxT("Add 10% to the fabric thickness in the z direction if checked."));
		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	pPage->SetSizer(pMainSizer);
	pMainSizer->Fit(pPage);

	m_pBinderLayersSpin->SetValue(1);

	return pPage;
}

wxWizardPageSimple* CWeaveWizard3D::BuildPropertiesPage( YARN_PROPERTIES& Properties, string Type )
{
	wxWizardPageSimple *pPage = new wxWizardPageSimple(this);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
	SizerFlags.Border();
	SizerFlags.Expand();

	wxSizer *pSubSizer;

	string Title = "Enter the data for the " + Type + " yarn properties.\nNote that it is not necessary to complete all of these values.\nEither the fibre area, or fibres per yarn and fibre diameter,\nor yarn linear density and fibre density will be sufficient.";
	wxString wxTitle = ConvertString( Title );
	pMainSizer->Add(new wxStaticText(pPage, wxID_ANY, wxTitle), SizerFlags);

	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);
	{
		pSubSizer = new wxFlexGridSizer(3);
		wxTextCtrl* pControl;

		Properties.YarnLinearDensity = wxT("0.0");
		Properties.YarnLinearDensityUnits = wxT("kg/m");
		Properties.FibreDensity = wxT("0.0");
		Properties.FibreDensityUnits = wxT("kg/m^3");
		Properties.FibreArea = wxT("0.0");
		Properties.FibreAreaUnits = wxT("m^2");
		Properties.FibreDiameter = wxT("0.0");
		Properties.FibreDiameterUnits = wxT("m");
		Properties.FibresPerYarn = wxT("0");
	
		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Linear Density:")), SizerFlags);
		
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, wxID_ANY, wxT("0.0"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &Properties.YarnLinearDensity)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the yarn linear density."));
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, wxID_ANY, wxT("kg/m"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_ASCII, &Properties.YarnLinearDensityUnits)), SizerFlags);
		pControl->SetToolTip(wxT("Set the units for yarn linear density."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Fibre Density:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, wxID_ANY, wxT("0.0"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &Properties.FibreDensity)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the fibre density."));
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, wxID_ANY, wxT("kg/m^3"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_ASCII, &Properties.FibreDensityUnits)), SizerFlags);
		pControl->SetToolTip(wxT("Set the units for yarn fibre density."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Fibre Area:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, wxID_ANY, wxT("0.0"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &Properties.FibreArea)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the fibre area."));
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, wxID_ANY, wxT("m^2"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_ASCII, &Properties.FibreAreaUnits)), SizerFlags);
		pControl->SetToolTip(wxT("Set the units for yarn fibre area."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Fibre Diameter:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, wxID_ANY, wxT("0.0"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &Properties.FibreDiameter)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the fibre density."));
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, wxID_ANY, wxT("kg/m^3"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_ASCII, &Properties.FibreDiameterUnits)), SizerFlags);
		pControl->SetToolTip(wxT("Set the units for yarn fibre density."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Fibres Per Yarn:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &Properties.FibresPerYarn)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the number of fibres per yarn."));

		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	pPage->SetSizer(pMainSizer);
	pMainSizer->Fit(pPage);

	return pPage;
}

wxDialog* CWeaveWizard3D::BuildWeavePatternDialog()
{
	wxDialog* pDialog = new wxDialog();

	pDialog->Create(this, wxID_ANY, wxT("Weave pattern"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
	SizerFlags.Border();
	SizerFlags.Expand();

	pMainSizer->Add(new wxStaticText(pDialog, wxID_ANY, wxT("Click binder yarns in top view to set the pattern for orthogonal and angle interlock weaves.\nDrag binder yarns in the side view to set the pattern for layer to layer weaves.")), SizerFlags);
	pMainSizer->Add(new wxStaticText(pDialog, wxID_ANY, wxT("Note that this is a schematic diagram to visualise binder yarn path and does not give an accurate display of the final positions")), SizerFlags);


	SizerFlags.Proportion(1);

	m_pWeavePatternCtrl = new wxWeavePatternCtrl3D(pDialog, wxID_ANY);
	pMainSizer->Add(m_pWeavePatternCtrl, SizerFlags);

	wxSizer* pSubSizer = pDialog->CreateStdDialogButtonSizer(wxOK | wxCANCEL);
	if (pSubSizer)
	{
		SizerFlags.Proportion(0);
		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	pDialog->SetSizer(pMainSizer);
	pMainSizer->Fit(pDialog);

	return pDialog;
}

bool CWeaveWizard3D::RebuildWeavePatternCtrl( )
{
	if (!m_pWeavePatternCtrl || !m_pWeftYarnsSpin || !m_pWarpYarnsSpin 
		|| !m_pWarpLayersSpin || !m_pWeftLayersSpin || !m_pBinderRatioSpin || !m_pWarpRatioSpin)
		return false;

	int iNumWeft = m_pWeftYarnsSpin->GetValue();
	int iTotalNumWarp = m_pWarpYarnsSpin->GetValue();
	int iNumWarpLayers = m_pWarpLayersSpin->GetValue();
	int iNumWeftLayers = m_pWeftLayersSpin->GetValue();

	int iWarpRatio = m_pWarpRatioSpin->GetValue();
	int iNumBinder = 0, iNumWarp = 0;

	if ( iWarpRatio == 0 )
	{
		iNumBinder = iTotalNumWarp;
	}
	else
	{
		int iBinderRatio = m_pBinderRatioSpin->GetValue();
		int iNumRepeats = iTotalNumWarp / ( iBinderRatio + iWarpRatio );
		int iRem = 0;
		iNumBinder = iNumRepeats * iBinderRatio;
		iNumWarp = iNumRepeats * iWarpRatio;
		iRem = iTotalNumWarp % ( iBinderRatio + iWarpRatio );
		if ( iWarpRatio < iRem )
		{
			iNumWarp += iWarpRatio;
			iNumBinder += (iRem - iWarpRatio);
		}
		else
		{
			iNumWarp += iRem;
		}
	}
	//int iNumBinder = m_pBinderRatioSpin->GetValue() * ( iNumWarp / m_pWarpRatioSpin->GetValue() );
	int iNumBinderLayers;
	if ( m_WeaveTypeRadio->GetSelection() == LAYER_TO_LAYER )
		iNumBinderLayers = m_pBinderLayersSpin->GetValue();
	else
		iNumBinderLayers = 1;
	
	
	YARNDATA Warp;
	YARNDATA Weft;
	YARNDATA Binder;
	
	m_WarpWidth.ToDouble(&Warp.dWidth);
	m_WeftWidth.ToDouble(&Weft.dWidth);
	m_BinderWidth.ToDouble(&Binder.dWidth);
	m_WarpSpacing.ToDouble(&Warp.dSpacing);
	m_WeftSpacing.ToDouble(&Weft.dSpacing);
	m_BinderSpacing.ToDouble(&Binder.dSpacing);
	m_WarpHeight.ToDouble(&Warp.dHeight);
	m_WeftHeight.ToDouble(&Weft.dHeight);
	m_BinderHeight.ToDouble(&Binder.dHeight);

	bool bChanged = false;

	if (m_pWeavePatternCtrl->GetWeaveWidth() != iNumWeft ||
		m_pWeavePatternCtrl->GetWeaveHeight() != iTotalNumWarp ||
		m_pWeavePatternCtrl->GetWeaveNumWarpLayers() != iNumWarpLayers ||
		m_pWeavePatternCtrl->GetWeaveNumWeftLayers() != iNumWeftLayers ||
		m_pWeavePatternCtrl->GetWeaveNumBinderLayers() != iNumBinderLayers)
	{
		m_pWeavePatternCtrl->SetWeaveSize(m_WeaveTypeRadio->GetSelection(),iNumWarp + iNumBinder, iNumWeft, Warp, Weft, Binder, iNumBinderLayers, m_pOffsetWeftBox->GetValue() );
		bChanged = true;
	}	

	CTextile3DWeave &Weave = m_pWeavePatternCtrl->GetWeave();
	

	Weave.SetWarpRatio( m_pWarpRatioSpin->GetValue() );
	Weave.SetBinderRatio( m_pBinderRatioSpin->GetValue() );
	Weave.SetWarpYarnWidths(Warp.dWidth);
	Weave.SetYYarnWidths(Weft.dWidth);
	Weave.SetBinderYarnWidths(Binder.dWidth);
	Weave.SetBinderYarnSpacings( Binder.dSpacing );
	Weave.SetBinderYarnHeights( Binder.dHeight );
	if ( bChanged )
	{
		Weave.SetupLayers( iNumWarpLayers, iNumWeftLayers, iNumBinderLayers );
	}
	m_pWeavePatternCtrl->SetCrossSectionIndex( m_pWarpRatioSpin->GetValue() );

	double dFabricThickness = (iNumWarpLayers * Warp.dHeight) + (iNumWeftLayers * Weft.dHeight) + ( 2 * Binder.dHeight );
	Weave.SetThickness( dFabricThickness );


	return true;
}

bool CWeaveWizard3D::RunIt()
{
	return RunWizard(m_pFirstPage);
}

void CWeaveWizard3D::OnWizardPageChanging(wxWizardEvent& event)
{
	
	if ( (event.GetPage() == m_pBinderYarnPage && event.GetDirection() == true && !m_bRefine)
		|| (event.GetPage() == m_pBinderPropertiesPage && event.GetDirection() == true) )
	{
		RebuildWeavePatternCtrl();
		if (m_pWeavePatternDialog->ShowModal() != wxID_OK)
			event.Veto();
	}
}

bool CWeaveWizard3D::HasNextPage(wxWizardPage *page)
{
	if ( page == m_pBinderYarnPage ) 
		return true;
	return wxWizard::HasNextPage(page);
}

string CWeaveWizard3D::GetCreateTextileCommand(string ExistingTextile)
{
	if (!m_pWeavePatternCtrl->bHasWeave())
		return "";
	stringstream StringStream;
	int iWidth = m_pWeavePatternCtrl->GetWeaveWidth();
	int iHeight = m_pWeavePatternCtrl->GetWeaveHeight();
	int iNumWarpLayers = m_pWarpLayersSpin->GetValue(), iNumWeftLayers = m_pWeftLayersSpin->GetValue();

	YARNDATA Warp;
	YARNDATA Weft;
	YARNDATA Binder;
	double GapSize;
	double dWarpPower, dWeftPower, dBinderPower, dThickness, dMaxVolFraction;
	
	m_WarpWidth.ToDouble(&Warp.dWidth);
	m_WeftWidth.ToDouble(&Weft.dWidth);
	m_BinderWidth.ToDouble(&Binder.dWidth);
	m_WarpSpacing.ToDouble(&Warp.dSpacing);
	m_WeftSpacing.ToDouble(&Weft.dSpacing);
	m_BinderSpacing.ToDouble(&Binder.dSpacing);
	m_WarpHeight.ToDouble(&Warp.dHeight);
	m_WeftHeight.ToDouble(&Weft.dHeight);
	m_BinderHeight.ToDouble(&Binder.dHeight);
	m_GapSize.ToDouble(&GapSize);
	m_WarpPower.ToDouble(&dWarpPower);
	m_WeftPower.ToDouble(&dWeftPower);
	m_BinderPower.ToDouble(&dBinderPower);
	m_Thickness.ToDouble(&dThickness);
	m_MaxVolFraction.ToDouble(&dMaxVolFraction);

	switch( m_WeaveTypeRadio->GetSelection() )
	{
		case ORTHOGONAL:
			{	
				int iWarpRatio = m_pWarpRatioSpin->GetValue();
				StringStream << "weave = CTextileOrthogonal(" << iHeight << ", " << iWidth << ", " << Warp.dSpacing << ", " 
					<< Weft.dSpacing << ", " << Warp.dHeight << ", " << Weft.dHeight << ", bool(" << m_pRefineBox->GetValue() << "))" << endl;	
				StringStream << "weave.SetWarpRatio( " << iWarpRatio << ")" << endl;
				StringStream << "weave.SetBinderRatio( " << m_pBinderRatioSpin->GetValue() << ")" << endl;
				StringStream << "weave.SetWarpYarnWidths( " << Warp.dWidth << ")" << endl;
				StringStream << "weave.SetYYarnWidths( " << Weft.dWidth << ")" << endl;
				StringStream << "weave.SetBinderYarnWidths( " << Binder.dWidth << ")" << endl;
				
				if ( iWarpRatio == 0 )
					iNumWarpLayers = 0;
				StringStream << "weave.SetupLayers( " << iNumWarpLayers << ", " << iNumWeftLayers << ")" << endl;
				StringStream << "weave.SetGapSize( " << GapSize << " )" << endl;
				
				if ( m_pRefineBox->GetValue() )
				{
					StringStream << "weave.SetThickness( " << dThickness << " )" << endl;
					StringStream << ConvertProperties( m_WarpProperties, WARP, "weave" );
					StringStream << ConvertProperties( m_WeftProperties, WEFT, "weave" );
					StringStream << ConvertProperties( m_BinderProperties, BINDER, "weave" );
					StringStream << "weave.SetMaxVolFraction( " << dMaxVolFraction << ")" << endl;
				}

				for (int i=0; i<iWidth; ++i)
				{
					for (int j=0; j<iHeight; ++j)
					{
						if (GetBinderCellBottom(i, j) == PATTERN3D_XYARN)
						{
							StringStream << "weave.SwapBinderPosition(" << i <<", " << j << ")" << endl;
						}
					}
				}
			}
			break;
		case LAYER_TO_LAYER:
			{
				StringStream << "weave = CTextileLayerToLayer(" << iHeight << ", " << iWidth << ", " << Warp.dSpacing << ", " 
					<< Weft.dSpacing << ", " << Warp.dHeight << ", " << Weft.dHeight << ", " << m_pBinderLayersSpin->GetValue() << ")" << endl;		
				StringStream << "weave.SetWarpRatio( " << m_pWarpRatioSpin->GetValue() << ")" << endl;
				StringStream << "weave.SetBinderRatio( " << m_pBinderRatioSpin->GetValue() << ")" << endl;
				StringStream << "weave.SetWarpYarnWidths( " << Warp.dWidth << ")" << endl;
				StringStream << "weave.SetYYarnWidths( " << Weft.dWidth << ")" << endl;
				StringStream << "weave.SetBinderYarnWidths( " << Binder.dWidth << ")" << endl;
				
				StringStream << "weave.SetupLayers( " << iNumWarpLayers << ", " << iNumWeftLayers << ", " << m_pBinderLayersSpin->GetValue() << ")" << endl;
				StringStream << "weave.SetGapSize( " << GapSize << " )" << endl;

				CTextile3DWeave &Weave = m_pWeavePatternCtrl->GetWeave();
				CTextileLayerToLayer* pWeaveLayerToLayer = dynamic_cast<CTextileLayerToLayer*>(&Weave);
				for (int j=0; j<iHeight; ++j) 
				{
					if ( m_pWeavePatternCtrl->IsBinderYarn( j ))
					{
						for (int i=0; i<iWidth; ++i)
						{
							int iOffset = pWeaveLayerToLayer->GetBinderOffset( i, j );
							StringStream << "weave.SetBinderPosition(" << i <<", " << j << ", " << iOffset << ")" << endl;
						}
					}
				}
			}
			break;
		case ANGLE_INTERLOCK:
			{
				if ( m_pOffsetWeftBox->GetValue() )
				{
					StringStream << "weave = CTextileOffsetAngleInterlock(" << iHeight << ", " << iWidth/2 << ", " << Warp.dSpacing << ", " 
												<< Weft.dSpacing*2.0 << ", " << Warp.dHeight << ", " << Weft.dHeight << ")" << endl;
				}
				else
				{
					StringStream << "weave = CTextileAngleInterlock(" << iHeight << ", " << iWidth << ", " << Warp.dSpacing << ", " 
												<< Weft.dSpacing << ", " << Warp.dHeight << ", " << Weft.dHeight << ")" << endl;				
				}
				StringStream << "weave.SetWarpRatio( " << m_pWarpRatioSpin->GetValue() << ")" << endl;
				StringStream << "weave.SetBinderRatio( " << m_pBinderRatioSpin->GetValue() << ")" << endl;
				StringStream << "weave.SetWarpYarnWidths( " << Warp.dWidth << ")" << endl;
				StringStream << "weave.SetYYarnWidths( " << Weft.dWidth << ")" << endl;
				StringStream << "weave.SetBinderYarnWidths( " << Binder.dWidth << ")" << endl;
				
				StringStream << "weave.SetupLayers( " << iNumWarpLayers << ", " << iNumWeftLayers << ")" << endl;
				StringStream << "weave.SetGapSize( " << GapSize << " )" << endl;

				for (int i=0; i<iWidth; ++i)
				{
					for (int j=0; j<iHeight; ++j)
					{
						if (GetBinderCellBottom(i, j) == PATTERN3D_XYARN)
						{
							StringStream << "weave.SetBinderYarnPositions(" << (i+iWidth/2)%iWidth <<", " << j << ")" << endl;
						}
					}
				}
			}
			break;
		default:
			return "";
	}
	
	StringStream << "weave.SetWarpYarnPower( " << dWarpPower << ")" << endl;
	StringStream << "weave.SetWeftYarnPower( " << dWeftPower << ")" << endl;
	StringStream << "weave.SetBinderYarnPower( " << dBinderPower << ")" << endl;

	StringStream << ConvertWeaveYarnDimensions(m_pWeavePatternCtrl->GetWeave(), "weave");
	if (m_bCreateDomain)
	{
		StringStream << "weave.AssignDefaultDomain(";
		if ( !m_bAddedDomainHeight )
			StringStream << "False";
		StringStream << ")" << endl;
		
		if ( m_WeaveTypeRadio->GetSelection() == ORTHOGONAL && m_pRefineBox->GetValue() )
		{
			StringStream << "weave.SetDomainZValues()" << endl;
		}
	}
	if (ExistingTextile.empty())
	{
		StringStream << "textilename = AddTextile(weave)" << endl;
	}
	else
	{
		StringStream << "AddTextile('" << ExistingTextile << "', weave, True)" << endl;
	}
	return StringStream.str();
}

int CWeaveWizard3D::GetBinderCellBottom(int i, int j)
{
	if ( m_pWeavePatternCtrl->IsBinderYarn( j ) )
	{
		return m_pWeavePatternCtrl->GetCellBottom(i, j);
	}
	return false;
}

void CWeaveWizard3D::OnWeftLayersChanged(wxCommandEvent& event)
{
	int Layers = m_pWeftLayersSpin->GetValue();
	if ( m_WeaveTypeRadio->GetSelection() == ANGLE_INTERLOCK )
	{
		if ( m_pOffsetWeftBox->GetValue() )
		{
			m_pWeftYarnsSpin->SetValue( Layers + 1 );
		}
		else
		{
			m_pWeftYarnsSpin->SetValue( Layers * 2 );
		}
		m_pWarpLayersSpin->SetValue( Layers - 1 );
	}
	if ( m_WeaveTypeRadio->GetSelection() == LAYER_TO_LAYER )
	{
		m_pWarpLayersSpin->SetValue( Layers - 1 );
		m_pBinderLayersSpin->SetRange(1, Layers);
	}

	OnThicknessChanged( event );
}

void CWeaveWizard3D::OnWeftYarnsChanged(wxCommandEvent& event)
{
	if ( m_WeaveTypeRadio->GetSelection() == ANGLE_INTERLOCK )
	{
		static int PrevVal = m_pWeftYarnsSpin->GetValue();
		int Value = m_pWeftYarnsSpin->GetValue();
		if ( m_pOffsetWeftBox->GetValue() )  // Offset angle interlock
		{
			m_pWeftLayersSpin->SetValue( Value - 1 );
			m_pWarpLayersSpin->SetValue( Value - 2 );
		}
		else
		{
			
			// Must have even number of weft yarns - force value accordingly
			if ( Value % 2 )
			{
				if ( Value > PrevVal || Value == 1 )
				{
					m_pWeftYarnsSpin->SetValue( ++Value );
				}
				else
				{
					m_pWeftYarnsSpin->SetValue( --Value );
				}
			}
			PrevVal = Value;
			// Set number of warp & weft layers to be correct for number of weft yarns
			m_pWeftLayersSpin->SetValue( Value/2 );
			m_pWarpLayersSpin->SetValue( Value/2 - 1 );
		}
	}
}

void CWeaveWizard3D::OnWeaveTypeChanged(wxCommandEvent& event)
{
	int WeaveType = m_WeaveTypeRadio->GetSelection();
	if ( WeaveType == ANGLE_INTERLOCK || WeaveType == LAYER_TO_LAYER || m_pWarpRatioSpin->GetValue() == 0 ) // or orthogonal, all binders
	{
		if ( WeaveType != ORTHOGONAL )
		{
			m_pWarpLayersSpin->Disable();
			if ( WeaveType == ANGLE_INTERLOCK && m_pOffsetWeftBox->GetValue() )
			{
				m_pWeftLayersSpin->SetValue( 3 );
				m_pWarpLayersSpin->SetValue( 2 );
			}
			else
			{
				m_pWeftLayersSpin->SetValue( 2 );
				m_pWarpLayersSpin->SetValue( 1 );
			}
			m_pWeftYarnsSpin->SetValue( 4 );
		}

		m_bRefine = false;
		m_pRefineBox->Disable();
		m_pThicknessTextCtrl->Disable();
		m_pVolFractionTextCtrl->Disable();
		m_pRefineBox->SetValue(false);
		m_pBinderYarnPage->SetNext(NULL);
		
	}
	else  // Orthogonal with warp yarns
	{
		m_pWarpLayersSpin->Enable();
		
		m_pRefineBox->Enable();
		m_pRefineBox->SetValue(true);
		m_pThicknessTextCtrl->Enable();
		m_pVolFractionTextCtrl->Enable();
		m_bRefine = true;
		m_pBinderYarnPage->SetNext( m_pWeftPropertiesPage );
	}

	if ( WeaveType == LAYER_TO_LAYER )
	{
		m_pBinderLayersSpin->Enable();
		m_pBinderLayersSpin->SetRange( 1, m_pWeftLayersSpin->GetValue() );
		m_pBinderLayersSpin->SetValue( 2 );
	}
	else
		m_pBinderLayersSpin->Disable();

	if ( WeaveType == ANGLE_INTERLOCK )
	{
		m_pOffsetWeftBox->Enable();
	}
	else
	{
		m_pOffsetWeftBox->Disable();
	}

	switch( WeaveType )
	{
		case ORTHOGONAL:
			FindWindow(ID_OrthogonalText)->Enable();
			FindWindow(ID_AngleInterlockText)->Disable();
			FindWindow(ID_OffsetAngleInterlockText)->Disable();
			FindWindow(ID_LayerToLayerText)->Disable();
			break;
		case ANGLE_INTERLOCK:
			FindWindow(ID_OrthogonalText)->Disable();
			FindWindow(ID_AngleInterlockText)->Enable();
			FindWindow(ID_OffsetAngleInterlockText)->Enable();
			FindWindow(ID_LayerToLayerText)->Disable();
			break;
		case LAYER_TO_LAYER:
			FindWindow(ID_OrthogonalText)->Disable();
			FindWindow(ID_AngleInterlockText)->Disable();
			FindWindow(ID_OffsetAngleInterlockText)->Disable();
			FindWindow(ID_LayerToLayerText)->Enable();
			break;
	}
	RebuildWeavePatternCtrl();
}

void CWeaveWizard3D::OnWarpYarnsChanged(wxCommandEvent& event)
{
	int iMax = m_pWarpYarnsSpin->GetValue()- m_pBinderRatioSpin->GetValue();
	m_pWarpRatioSpin->SetRange( 0, iMax < 0 ? 0 : iMax );
	m_pBinderRatioSpin->SetRange( 1, m_pWarpYarnsSpin->GetValue() );
}

void CWeaveWizard3D::OnWarpRatioChanged(wxCommandEvent& event)
{
	if ( m_pWarpRatioSpin->GetValue() )
	{
		m_pWarpLayersSpin->Enable();
		FindWindow(ID_WarpSpacing)->Enable();
		FindWindow(ID_WarpWidth)->Enable();
		FindWindow(ID_WarpHeight)->Enable();
		FindWindow(ID_WarpPower)->Enable();
		FindWindow(ID_WarpPanel)->Enable();
		// if orthogonal
		if ( m_WeaveTypeRadio->GetSelection() == ORTHOGONAL )
		{
			m_bRefine = true;
			m_pRefineBox->Enable();
			m_pThicknessTextCtrl->Enable();
			m_pVolFractionTextCtrl->Enable();
			m_pRefineBox->SetValue(true);
			m_pBinderYarnPage->SetNext( m_pWeftPropertiesPage );
		}
	}
	else
	{
		m_pWarpLayersSpin->Disable();
		FindWindow(ID_WarpSpacing)->Disable();
		FindWindow(ID_WarpWidth)->Disable();
		FindWindow(ID_WarpHeight)->Disable();
		FindWindow(ID_WarpPower)->Disable();
		FindWindow(ID_WarpPanel)->Disable();
		if ( m_WeaveTypeRadio->GetSelection() == ORTHOGONAL )
		{
			m_bRefine = false;
			m_pRefineBox->Disable();
			m_pThicknessTextCtrl->Disable();
			m_pVolFractionTextCtrl->Disable();
			m_pRefineBox->SetValue(false);
			m_pBinderYarnPage->SetNext(NULL);
		}
	}
}

void CWeaveWizard3D::OnOffsetWarpChanged(wxCommandEvent& event)
{
	if ( m_WeaveTypeRadio->GetSelection() == ANGLE_INTERLOCK )
	{
		int Value = m_pWeftYarnsSpin->GetValue();
		if ( m_pOffsetWeftBox->GetValue() )  // Offset angle interlock
		{
			m_pWeftLayersSpin->SetValue( Value - 1 );
			m_pWarpLayersSpin->SetValue( Value - 2 );
		}
		else
		{
			
			// Must have even number of weft yarns - force value accordingly
			if ( Value % 2 )
			{
				m_pWeftYarnsSpin->SetValue( ++Value );
			}
			// Set number of warp & weft layers to be correct for number of weft yarns
			m_pWeftLayersSpin->SetValue( Value/2 );
			m_pWarpLayersSpin->SetValue( Value/2 - 1 );
		}
	}
}

void CWeaveWizard3D::OnTextChanged(wxCommandEvent& event)
{
	if ( !m_BuiltPages )
		return;
	if ( !Validate())
		return;
	if ( !TransferDataFromWindow() )
		return;
	wxWizardPage *Page = this->GetCurrentPage();
	if ( Page )
	{
		if (!Page->Validate())
			return;
		if ( !Page->TransferDataFromWindow() )
			return;
	}
	DrawSectionPreview();
	
	int EventID = event.GetId();
	if ( EventID == ID_WarpHeight || EventID == ID_WeftHeight 
		|| EventID == ID_BinderHeight || EventID == ID_GapSize )
	{
		OnThicknessChanged( event );
	}
}

void CWeaveWizard3D::DrawSectionPreview()
{
	wxPanel* Panel;
	if ( this->GetCurrentPage() == m_pWarpYarnPage )
	{
		Panel = wxDynamicCast( FindWindow(ID_WarpPanel), wxPanel);
	}
	else if ( this->GetCurrentPage() == m_pWeftYarnPage )
	{
		Panel = wxDynamicCast( FindWindow(ID_WeftPanel), wxPanel);
	}
	else if ( this->GetCurrentPage() == m_pBinderYarnPage )
	{
		Panel = wxDynamicCast( FindWindow(ID_BinderPanel), wxPanel);
	}
	else
		return;
	
	wxClientDC dc(Panel);
	DrawSectionPreview(dc);
}

void CWeaveWizard3D::DrawSectionPreview(wxDC &dc)
{
	double dWidth;
	double dHeight;
	double dPower;

	if ( this->GetCurrentPage() == m_pWarpYarnPage )
	{
		m_WarpWidth.ToDouble(&dWidth);
		m_WarpHeight.ToDouble(&dHeight);
		m_WarpPower.ToDouble(&dPower);
	}
	else if ( this->GetCurrentPage() == m_pWeftYarnPage )
	{
		m_WeftWidth.ToDouble(&dWidth);
		m_WeftHeight.ToDouble(&dHeight);
		m_WeftPower.ToDouble(&dPower);
	}
	else if ( this->GetCurrentPage() == m_pBinderYarnPage )
	{
		m_BinderWidth.ToDouble(&dWidth);
		m_BinderHeight.ToDouble(&dHeight);
		m_BinderPower.ToDouble(&dPower);
	}
	else
		return;

	wxPanel* Panel = wxDynamicCast( FindWindow(ID_WarpPanel), wxPanel);

	wxSize WindowSize = Panel->GetSize();
	wxRect WindowRect = Panel->GetRect();
	dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE)));
	dc.Clear();
	
	CSectionPowerEllipse Section( dWidth, dHeight, dPower );
	vector<XY> Points = Section.GetPoints(100);
	
	int iNumPoints = (int)Points.size();
	wxPoint* pPointsArray = new wxPoint [iNumPoints];
	
	XY SectionSize;
	int i;
	for (i = 0; i<iNumPoints; ++i)
	{
		if (abs(Points[i].x) > SectionSize.x)
			SectionSize.x = abs(Points[i].x);
		if (abs(Points[i].y) > SectionSize.y)
			SectionSize.y = abs(Points[i].y);
	}

	double dScale = WindowRect.GetWidth()/SectionSize.x;
	if (WindowRect.GetHeight()/SectionSize.y < dScale)
		dScale = WindowRect.GetHeight()/SectionSize.y;

	dScale *= 0.49;

	for (i = 0; i<iNumPoints; ++i)
	{
		pPointsArray[i] = wxPoint(int(Points[i].x*dScale+WindowRect.GetWidth()*0.5 ), int(-Points[i].y*dScale+WindowRect.GetHeight()*0.5));
	}
	dc.DrawPolygon(iNumPoints, pPointsArray);
	delete [] pPointsArray;
}

double CWeaveWizard3D::CalcThickness()
{
	// Assumes binder at both top and bottom
	double WarpHeight, WeftHeight, BinderHeight;
	int WarpLayers = m_pWarpLayersSpin->GetValue();
	int WeftLayers = m_pWeftLayersSpin->GetValue();
	double GapSize;
	m_WarpHeight.ToDouble(&WarpHeight);
	m_WeftHeight.ToDouble(&WeftHeight);
	m_BinderHeight.ToDouble(&BinderHeight);
	m_GapSize.ToDouble(&GapSize);

	return WarpLayers*WarpHeight + WeftLayers*WeftHeight + 2*BinderHeight + (WarpLayers + WeftLayers + 1)*GapSize ;
}

void CWeaveWizard3D::OnThicknessChanged( wxCommandEvent &event )
{
	if ( m_BuiltPages )
		m_pThicknessTextCtrl->SetValue(ConvertString(stringify( CalcThickness(), 6, false )) );
}

void CWeaveWizard3D::OnTargetThicknessChanged( wxCommandEvent &event )
{
	if ( !m_BuiltPages )
		return;
	double Thickness = CalcThickness();
	double TargetThickness;
	
	wxWizardPage *Page = this->GetCurrentPage();
	if ( Page )
	{
		if (!Page->Validate())
			return;
		if ( !Page->TransferDataFromWindow() )
			return;
	}

	//if ( !TransferDataFromWindow() )
	//	return;
	m_Thickness.ToDouble( &TargetThickness );
	
	if ( (TargetThickness > Thickness && fabs(TargetThickness-Thickness) > 0.000001) || TargetThickness < 0.0 )
	{
		wxString sThickness( ConvertString(stringify( Thickness, 6, false ) ) );
		m_Thickness = sThickness; // Must reset m_Thickness before set value otherwise recursively calls OnTargetThicknessChanged & get stack overflow
		m_pThicknessTextCtrl->SetValue( sThickness );	
	}
}

/*void CWeaveWizard3D::OnMaxVolumeFractionChanged( wxCommandEvent &event )
{
	if ( !m_BuiltPages )
		return;

	//double MaxVolFraction;
	//wxString sOldVolFraction = m_MaxVolFraction;
	
	wxWizardPage *Page = this->GetCurrentPage();
	if ( Page )
	{
		if (!Page->Validate())
			return;
		if ( !Page->TransferDataFromWindow() )
			return;
	}

	/*m_MaxVolFraction.ToDouble( &MaxVolFraction );
	
	if ( MaxVolFraction < 0.0 || MaxVolFraction > 1.0 )
	{
		m_MaxVolFraction = sOldVolFraction;  // If out of range set to old value;
		m_pVolFractionTextCtrl->SetValue( m_MaxVolFraction );
	}*/	
//}*/

void CWeaveWizard3D::OnRefine( wxCommandEvent &event )
{
	m_pBinderYarnPage->Validate();
	m_pBinderYarnPage->TransferDataFromWindow();
	if ( m_bRefine )
	{
		m_pBinderYarnPage->SetNext( m_pWeftPropertiesPage );
		m_pThicknessTextCtrl->Enable();
		m_pVolFractionTextCtrl->Enable();
	}
	else
	{
		m_pBinderYarnPage->SetNext( NULL );
		m_pThicknessTextCtrl->Disable();
		m_pVolFractionTextCtrl->Disable();
	}
}

string CWeaveWizard3D::ConvertProperties( YARN_PROPERTIES &Properties, int iYarnType, string PythonString )
{
	stringstream StringStream;

	double	dYarnLinearDensity, dFibreDensity, dFibreArea, dFibreDiameter;
	long	FibresPerYarn;
	
	Properties.YarnLinearDensity.ToDouble( &dYarnLinearDensity );
	Properties.FibreDensity.ToDouble( &dFibreDensity );
	Properties.FibreArea.ToDouble( &dFibreArea );
	Properties.FibreDiameter.ToDouble( &dFibreDiameter );
	Properties.FibresPerYarn.ToLong( &FibresPerYarn );

	StringStream << PythonString << ".SetYarnLinearDensity( " << iYarnType << ", " << dYarnLinearDensity << ", '" << ConvertString( Properties.YarnLinearDensityUnits ) << "' )" << endl;
	StringStream << PythonString << ".SetFibreDensity( " << iYarnType << ", " << dFibreDensity << ", '" << ConvertString( Properties.FibreDensityUnits ) << "' )" << endl;
	StringStream << PythonString << ".SetFibreArea( " << iYarnType << ", " << dFibreArea << ", '" << ConvertString( Properties.FibreAreaUnits ) << "' )" << endl;
	StringStream << PythonString << ".SetFibreDiameter( " << iYarnType << ", " << dFibreDiameter << ", '" << ConvertString( Properties.FibreDiameterUnits ) << "' )" << endl;
	StringStream << PythonString << ".SetFibresPerYarn( " << iYarnType << ", " << (int)FibresPerYarn << " )" << endl;
	return StringStream.str();
}

void CWeaveWizard3D::LoadSettings(const CTextile3DWeave &Weave)
{	
	string WeaveType = Weave.GetType();

	if ( WeaveType == "CTextileOrthogonal" )
		m_WeaveTypeRadio->SetSelection( ORTHOGONAL );
	else if ( WeaveType == "CTextileLayerToLayer" )
		m_WeaveTypeRadio->SetSelection( LAYER_TO_LAYER );
	else
		m_WeaveTypeRadio->SetSelection( ANGLE_INTERLOCK );
	m_WeaveTypeRadio->Disable();  // If editing don't want to be able to change the weave type

	m_WarpSpacing.clear();
	m_WeftSpacing.clear();
	m_WarpWidth.clear();
	m_WeftWidth.clear();
	m_WarpHeight.clear();
	m_WeftHeight.clear();
	m_BinderWidth.clear();
	m_BinderHeight.clear();
	m_BinderSpacing.clear();
	m_Thickness.clear();
	m_MaxVolFraction.clear();
	
	m_GapSize.clear();

	m_WarpPower.clear();
	m_WeftPower.clear();
	m_BinderPower.clear();

	YARNDATA AveWarp, AveBinder;

	int iNumBinderYarns, iNumWarpYarns, iBinderRatio, iWarpRatio;
	Weave.GetWarpYarnInfo( iNumBinderYarns, iNumWarpYarns, iBinderRatio, iWarpRatio, AveWarp, AveBinder );
	
	m_pWarpYarnsSpin->SetValue( iNumWarpYarns );
	m_pBinderRatioSpin->SetValue( iBinderRatio );
	m_pWarpRatioSpin->SetValue(iWarpRatio);

	m_pWeftYarnsSpin->SetValue(Weave.GetNumYYarns());
	m_pWarpLayersSpin->SetValue(Weave.GetNumXLayers(0));
	int iNumWeftLayers = Weave.GetNumYLayers(0);
	if ( WeaveType == "CTextileOffsetAngleInterlock" )
		iNumWeftLayers += Weave.GetNumYLayers(1);
	m_pWeftLayersSpin->SetValue(iNumWeftLayers);
	m_pBinderLayersSpin->SetValue(Weave.GetNumBinderLayers());

	m_WeftSpacing << Weave.GetAverageWeftYarnSpacing();
	m_WarpSpacing << AveWarp.dSpacing;
	m_BinderSpacing << AveBinder.dSpacing;
	m_BinderSpacingTextCtrl->SetValue(ConvertString(stringify( AveBinder.dSpacing, 6, false )));

	m_WarpWidth << AveWarp.dWidth;
	m_BinderWidth << AveBinder.dWidth;
	m_BinderWidthTextCtrl->SetValue(ConvertString(stringify( AveBinder.dWidth, 6, false )));
	m_WeftWidth << Weave.GetAverageWeftYarnWidth();

	m_WarpHeight << AveWarp.dHeight;
	m_BinderHeight << AveBinder.dHeight;
	m_BinderHeightTextCtrl->SetValue(ConvertString(stringify( AveBinder.dHeight, 6, false )));
	m_WeftHeight << Weave.GetAverageWeftYarnHeight();

	m_WarpPower << Weave.GetWarpPower();
	m_WeftPower << Weave.GetWeftPower();
	m_BinderPower << Weave.GetBinderPower();
	m_BinderPowerTextCtrl->SetValue(ConvertString(stringify( m_BinderPower, 6, false )));

	//m_FabricThickness << Weave.GetFabricThickness();
	m_GapSize << Weave.GetGapSize();
	m_GapSizeTextCtrl->SetValue(ConvertString(stringify( Weave.GetGapSize(), 6, false )));
//	m_bCreateDomain = false;
	m_bRefine = false;
	m_pRefineBox->SetValue(false);
	m_pBinderYarnPage->SetNext( NULL );
	m_pThicknessTextCtrl->Disable();	
	m_pVolFractionTextCtrl->Disable();
		
	if ( WeaveType == "CTextileOrthogonal" )
	{
		m_pRefineBox->Enable();
	}
	else
	{
		m_pRefineBox->Disable();
	}

	if ( WeaveType == "CTextileOffsetAngleInterlock" )
		m_pOffsetWeftBox->SetValue( true );
	else
		m_pOffsetWeftBox->SetValue( false );
	m_pOffsetWeftBox->Disable();  // Don't want to be able to change weave type when editing

	if ( WeaveType == "CTextileLayerToLayer" )
	{
		m_pBinderLayersSpin->Enable();
		m_pBinderLayersSpin->SetRange( 1, m_pWeftLayersSpin->GetValue() );
		m_pBinderLayersSpin->Enable();
	}
	else
		m_pBinderLayersSpin->Disable();

	//wxCheckBox *m_pRefineBox;
	m_pThicknessTextCtrl->SetValue( ConvertString(stringify( Weave.GetFabricThickness(), 6, false )));
	m_pVolFractionTextCtrl->SetValue( ConvertString(stringify( Weave.GetMaxVolFraction(), 6, false )));
	
	m_pWeavePatternCtrl->SetWeaveSize(Weave);
}

void CSectionWindowPainter::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(dynamic_cast<wxPanel*>(event.GetEventObject()));

	m_WeaveWizard3D.DrawSectionPreview(dc);
}



