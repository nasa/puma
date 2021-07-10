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
#include "WeaveWizard.h"
#include "WindowIDs.h"
#include "WeavePatternCtrl.h"
#include "PythonConverter.h"

#include "Wizard.xpm"
#include "RangeValidator.h"

BEGIN_EVENT_TABLE(CWeaveWizard, wxWizard)
    EVT_WIZARD_PAGE_CHANGING(wxID_ANY, CWeaveWizard::OnWizardPageChanging)
	EVT_CHECKBOX(ID_Create3DWeave, CWeaveWizard::On3DWeave)
	EVT_CHECKBOX(ID_Refine, CWeaveWizard::OnRefine)
	EVT_TEXT(ID_Width, CWeaveWizard::OnWidthChanged)
	EVT_TEXT(ID_Spacing, CWeaveWizard::OnSpacingChanged)
	EVT_TEXT(ID_Thickness, CWeaveWizard::OnThicknessChanged)
	EVT_INIT_DIALOG(CWeaveWizard::OnInit)
	EVT_CHECKBOX(ID_InPlaneTangents, CWeaveWizard::OnInPlaneTangents)
	EVT_CHECKBOX(ID_ShearTextile, CWeaveWizard::OnShear)
	EVT_CHECKBOX(ID_DefaultDomain, CWeaveWizard::OnDomain)
	EVT_CHECKBOX(ID_ShearedDomain, CWeaveWizard::OnShearedDomain)
	EVT_CHECKBOX(ID_CreateLayers, CWeaveWizard::OnLayers)
//	EVT_BUTTON(ID_WeavePattern, CWeaveWizard::OnWeavePattern)
END_EVENT_TABLE()

CWeaveWizard::CWeaveWizard(wxWindow* parent, wxWindowID id)
: wxWizard(parent, id, wxT("Weave Wizard"), wxBitmap(Wizard_xpm))
, m_pFirstPage(NULL)
, m_pWeavePatternDialog(NULL)
, m_pWeavePatternCtrl(NULL)
, m_pYYarnsSpin(NULL)
, m_pXYarnsSpin(NULL)
, m_YarnSpacing(wxT("1"))
, m_YarnWidth(wxT("0.8"))
, m_FabricThickness(wxT("0.2"))
, m_GapSize(wxT("0"))
, m_ShearAngle(wxT("0.0"))
, m_bCreateDomain(true)
, m_bRefine(true)
, m_bInPlaneTangents(true)
, m_b3DWeave(false)
, m_bWidthChanged(false)
, m_bSpacingChanged(false)
, m_bThicknessChanged(false)
, m_bShear(false)
, m_bShearedDomain(false)
, m_bLayers(false)
, m_pLayersSpin(NULL)
, m_bAddedDomainHeight(true)
{
	BuildPages();
	GetPageAreaSizer()->Add(m_pFirstPage);
}

CWeaveWizard::~CWeaveWizard(void)
{
	if (m_pWeavePatternDialog)
		m_pWeavePatternDialog->Destroy();
}

bool CWeaveWizard::RunIt()
{
	return RunWizard(m_pFirstPage);
}

void CWeaveWizard::BuildPages()
{
	m_pFirstPage = BuildFirstPage();
	m_pWeavePatternDialog = BuildWeavePatternDialog();

//	wxWizardPageSimple::Chain(m_pFirstPage, m_pSecondPage);
}

wxWizardPageSimple* CWeaveWizard::BuildFirstPage()
{
	wxWizardPageSimple *pPage = new wxWizardPageSimple(this);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
	SizerFlags.Border();
	SizerFlags.Expand();

	wxSizer *pSubSizer;

	pMainSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("This wizard will create a 2D textile weave model for you.")), SizerFlags);

	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);
	pSubSizer = new wxFlexGridSizer(2);

	{
		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Warp Yarns:")), SizerFlags);
		pSubSizer->Add(m_pXYarnsSpin = new wxSpinCtrl(pPage, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100), SizerFlags);
		m_pXYarnsSpin->SetToolTip(wxT("Controls the number of warp yarns contained in the unit cell."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Weft Yarns:")), SizerFlags);
		pSubSizer->Add(m_pYYarnsSpin = new wxSpinCtrl(pPage, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100), SizerFlags);
		m_pYYarnsSpin->SetToolTip(wxT("Controls the number of weft yarns contained in the unit cell."));

		wxTextCtrl* pControl;
		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Spacing:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_Spacing, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_YarnSpacing)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the spacing between yarns."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Yarn Width:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_Width, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_YarnWidth)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the width of the yarns."));

		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Fabric Thickness:")), SizerFlags);
		pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_Thickness, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_FabricThickness)), SizerFlags);
		pControl->SetToolTip(wxT("Sets the thickness of the fabric."));
	}

	pMainSizer->Add(pSubSizer, SizerFlags);
	SizerFlags.Align(0);

	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);
	pSubSizer = new wxFlexGridSizer(3);
	{
		wxCheckBox* p3DWeaveBox;
		pSubSizer->Add(p3DWeaveBox = new wxCheckBox(pPage, ID_Create3DWeave, wxT("Create 3D weave"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_b3DWeave)), SizerFlags);
		pSubSizer->AddSpacer(0);
		pSubSizer->AddSpacer(0);

		wxCheckBox* pLayersBox;
		pSubSizer->Add(pLayersBox = new wxCheckBox(pPage, ID_CreateLayers, wxT("Create layered textile"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bLayers)), SizerFlags);
		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Number of weave layers:")), SizerFlags);
		pSubSizer->Add(m_pLayersSpin = new wxSpinCtrl(pPage, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100), SizerFlags);
		m_pLayersSpin->SetToolTip(wxT("Number of layers of the 2D weave."));
		if ( !m_bLayers )
			m_pLayersSpin->Disable();

		wxCheckBox* pDomainBox;
		wxCheckBox* pShearedDomainBox;
		wxCheckBox* pAddTenPercent;
		pSubSizer->Add(pDomainBox = new wxCheckBox(pPage, ID_DefaultDomain, wxT("Create default domain"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bCreateDomain)), SizerFlags);
		pSubSizer->Add(pShearedDomainBox = new wxCheckBox(pPage, ID_ShearedDomain, wxT("Create sheared domain"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bShearedDomain)), SizerFlags);
		pSubSizer->AddSpacer(0);
		pSubSizer->Add(pAddTenPercent = new wxCheckBox(pPage, ID_DomainHeight, wxT("Add 10% to domain height"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bAddedDomainHeight)), SizerFlags);
		pSubSizer->AddSpacer(0);
		pSubSizer->AddSpacer(0);

		pDomainBox->SetToolTip(wxT("Create box default domain"));
		pShearedDomainBox->SetToolTip(wxT("Creates domain with planes matching shear angle"));
		if ( !m_bShearedDomain )
			pShearedDomainBox->Disable();

		wxTextCtrl* pGapSize;
		wxCheckBox* pRefineBox;
		wxCheckBox* pInPlaneTangentsBox;
		pSubSizer->Add(pRefineBox = new wxCheckBox(pPage, ID_Refine, wxT("Refine model"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bRefine)), SizerFlags);
		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Gap size:")), SizerFlags);
		pSubSizer->Add(pGapSize = new wxTextCtrl(pPage, ID_GapSize, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_GapSize)), SizerFlags);
		pSubSizer->Add(pInPlaneTangentsBox = new wxCheckBox(pPage, ID_InPlaneTangents, wxT("Force in-plane tangents at nodes"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bInPlaneTangents)), SizerFlags);

/*		wxButton* pButton;
		pSubSizer->Add(pButton = new wxButton(pPage, ID_WeavePattern, wxT("Edit weave pattern")), SizerFlags);
		pSubSizer->AddSpacer(0);
		pSubSizer->AddSpacer(0);*/

		p3DWeaveBox->SetToolTip(wxT("Allow multiple layers of weft and warp yarns."));
		pLayersBox->SetToolTip(wxT("Stack several layers of 2D weave."));
		pDomainBox->SetToolTip(wxT("Assign the default domain to the textile. The default domain is the same size as the unit cell in the x and y directions and and corresponds to the fabric thickness in the z direction."));
		pAddTenPercent->SetToolTip(wxT("Add 10% to the fabric thickness in the z direction if checked."));
		pRefineBox->SetToolTip(wxT("Refine the model cross sections such that yarn volumes don't intersect.\nThe resulting geometry always contains a minimum gap size between two yarns."));
		pGapSize->SetToolTip(wxT("Adjusts the minimum gap size between two yarns when refine model is enabled."));

		if (!m_bRefine)
			pGapSize->Disable();
		pSubSizer->AddSpacer(0);
		pSubSizer->AddSpacer(0);

		wxCheckBox* pShearBox;
		pSubSizer->Add(pShearBox = new wxCheckBox(pPage, ID_ShearTextile, wxT("Shear textile"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bShear)), SizerFlags);
		pShearBox->SetToolTip(wxT("Shear the textile. All data entered is for the unsheared textile to which the shear is then applied."));

		wxTextCtrl* pShearAngle;
		pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Shear angle (degrees):")), SizerFlags);
		pSubSizer->Add(pShearAngle = new wxTextCtrl(pPage, ID_ShearAngle, wxT("0.0"), wxDefaultPosition, wxDefaultSize, 0, RangeValidator(&m_ShearAngle, -89.0, 89.0)), SizerFlags);
		pShearAngle->SetToolTip(wxT("Sets the shear angle in degrees"));
		if (!m_bShear)
			pShearAngle->Disable();
	}
	pMainSizer->Add(pSubSizer, SizerFlags);
	SizerFlags.Align(0);

	pPage->SetSizer(pMainSizer);
	pMainSizer->Fit(pPage);

	m_pYYarnsSpin->SetValue(2);
	m_pXYarnsSpin->SetValue(2);
	m_pLayersSpin->SetValue(1);

	return pPage;
}

wxDialog* CWeaveWizard::BuildWeavePatternDialog()
{
//	wxDialog* pDailog = new wxDialog(NULL, wxID_ANY, wxT("Weave pattern"));
	wxDialog* pDailog = new wxDialog();

	pDailog->Create(this, wxID_ANY, wxT("Weave pattern"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
	SizerFlags.Border();
	SizerFlags.Expand();

	pMainSizer->Add(new wxStaticText(pDailog, wxID_ANY, wxT("Set the weave pattern. Right click on top or side bars to change individual yarn settings")), SizerFlags);

	SizerFlags.Proportion(1);

	m_pWeavePatternCtrl = new wxWeavePatternCtrl(pDailog, wxID_ANY);
	pMainSizer->Add(m_pWeavePatternCtrl, SizerFlags);

	wxSizer* pSubSizer = pDailog->CreateStdDialogButtonSizer(wxOK | wxCANCEL);
	if (pSubSizer)
	{
		SizerFlags.Proportion(0);
		pMainSizer->Add(pSubSizer, SizerFlags);
	}

	pDailog->SetSizer(pMainSizer);
	pMainSizer->Fit(pDailog);

	return pDailog;
}

bool CWeaveWizard::RebuildWeavePatternCtrl()
{
	if (!m_pWeavePatternCtrl || !m_pYYarnsSpin || !m_pXYarnsSpin)
		return false;

	int iNumWidth = m_pYYarnsSpin->GetValue(), iNumHeight = m_pXYarnsSpin->GetValue();
	if (m_pWeavePatternCtrl->GetWeaveWidth() != iNumWidth ||
		m_pWeavePatternCtrl->GetWeaveHeight() != iNumHeight ||
		m_pWeavePatternCtrl->Get3DMode() != m_b3DWeave)
	{
		m_pWeavePatternCtrl->SetWeaveSize(iNumWidth, iNumHeight, m_b3DWeave);
		m_bWidthChanged = true;
		m_bSpacingChanged = true;
		m_bThicknessChanged = true;
	}

	CTextileWeave &Weave = m_pWeavePatternCtrl->GetWeave();
	
	double dWidth = 1;
	double dSpacing = 1;
	double dThickness = 1;

	m_YarnWidth.ToDouble(&dWidth);
	m_YarnSpacing.ToDouble(&dSpacing);
	m_FabricThickness.ToDouble(&dThickness);

	if (m_bWidthChanged)
		Weave.SetYarnWidths(dWidth);
	if (m_bSpacingChanged)
		Weave.SetYarnSpacings(dSpacing);
	if (m_bThicknessChanged)
		Weave.SetThickness(dThickness);

	m_bWidthChanged = false;
	m_bSpacingChanged = false;
	m_bThicknessChanged = false;

	return true;
}

void CWeaveWizard::OnWizardPageChanging(wxWizardEvent& event)
{
	if (event.GetPage() == m_pFirstPage && event.GetDirection() == true)
	{
		RebuildWeavePatternCtrl();
		if (m_pWeavePatternDialog->ShowModal() != wxID_OK)
			event.Veto();
	}
/*	if (event.GetPage() == m_pSecondPage)
	{
//		SaveWeavePattern();
	}*/
}

bool CWeaveWizard::GetPatternCell(int i, int j)
{
	return !m_pWeavePatternCtrl->GetCellStatus(i, j);
}

/*void CWeaveWizard::SetPatternCell(int i, int j, bool bVal)
{
	m_pWeavePatternCtrl->SetCellStatus(i, j, bVal);
}*/

string CWeaveWizard::GetCreateTextileCommand(string ExistingTextile)
{
	if (!m_pWeavePatternCtrl->bHasWeave())
		return "";
	stringstream StringStream;
	int iWidth = m_pWeavePatternCtrl->GetWeaveWidth();
	int iHeight = m_pWeavePatternCtrl->GetWeaveHeight();
	double dYarnSpacing, dFabricThickness, dGapSize, dShearAngle;
	m_YarnSpacing.ToDouble(&dYarnSpacing);
	m_FabricThickness.ToDouble(&dFabricThickness);
	m_GapSize.ToDouble(&dGapSize);
	m_ShearAngle.ToDouble(&dShearAngle);
	int iLayers = m_pLayersSpin->GetValue();
	if (!m_b3DWeave)
	{
		if ( m_bShear )
		{
			StringStream << "weave = CShearedTextileWeave2D(" << iWidth << ", " << iHeight << ", " << dYarnSpacing << ", " << dFabricThickness << ", " << dShearAngle*PI/180.0 << ", bool(" << m_bRefine << "), bool(" << m_bInPlaneTangents << "))" << endl;
		}
		else
		{
			StringStream << "weave = CTextileWeave2D(" << iWidth << ", " << iHeight << ", " << dYarnSpacing << ", " << dFabricThickness << ", bool(" << m_bRefine << "), bool(" << m_bInPlaneTangents << "))" << endl;
		}
		if (m_bRefine)
		{
			StringStream << "weave.SetGapSize(" << dGapSize << ")" << endl;
		}
	}
	else
		StringStream << "weave = CTextileWeave3D(" << iWidth << ", " << iHeight << ", " << dYarnSpacing << ", " << dFabricThickness << ")" << endl;
	StringStream << "weave.SetYarnWidths(" << m_YarnWidth << ")" << endl;
	int i, j;
	if (!m_b3DWeave )
	{
		for (i=0; i<iWidth; ++i)
		{
			for (j=0; j<iHeight; ++j)
			{
				if (GetPatternCell(i, j))
				{
					StringStream << "weave.SwapPosition(" << i <<", " << j/*iHeight-(j+1)*/ << ")" << endl;
				}
			}
		}
	}
	else
	{
		CTextileWeave3D* pWeave3D = dynamic_cast<CTextileWeave3D*>(&m_pWeavePatternCtrl->GetWeave());
		if (pWeave3D)
		{
			StringStream << ConvertWeave3DPattern(*pWeave3D, "weave");
			double dFabricThickness = 1;
			m_FabricThickness.ToDouble(&dFabricThickness);
			StringStream << "weave.SetYarnHeights(" << dFabricThickness/pWeave3D->GetMaxNumLayers() << ")" << endl;
		}
	}
	StringStream << ConvertWeaveYarnDimensions(m_pWeavePatternCtrl->GetWeave(), "weave");
	if ( m_bLayers )
	{
		StringStream << ConvertLayeredWeave( "weave", "LayeredTextile", iLayers, dFabricThickness );
	}
	if (m_bCreateDomain)
	{
		StringStream << "weave.AssignDefaultDomain(";
		if ( !m_bAddedDomainHeight )
			StringStream << "False, False";
		StringStream << ")" << endl;
	}
	if (m_bShearedDomain)
	{
		StringStream << "weave.AssignDefaultDomain( True";
		if ( !m_bAddedDomainHeight )
			StringStream << ",False";
		StringStream << ")" << endl;
	}

	if ( (m_bCreateDomain || m_bShearedDomain) && m_bLayers )
	{
		if ( m_bShearedDomain )
		{
			StringStream << "Domain = weave.GetDefaultDomain( True";
			if ( !m_bAddedDomainHeight )
				StringStream << ",False";
			StringStream << ")" << endl;

		}
		else
		{
			StringStream << "Domain = weave.GetDefaultDomain(";
			if ( !m_bAddedDomainHeight )
				StringStream << "False, False";
			StringStream << ")" << endl;
		}
		StringStream << "Plane = PLANE()" << endl;
		StringStream << "index = Domain.GetPlane( XYZ(0,0,-1), Plane )" << endl;
		StringStream << "Plane.d -= " << dFabricThickness * (iLayers-1) << endl;
		StringStream << "Domain.SetPlane( index, Plane )" << endl;
		StringStream << "LayeredTextile.AssignDomain( Domain )" << endl;
	}
	if (ExistingTextile.empty())
	{
		if ( !m_bLayers )
			StringStream << "textilename = AddTextile(weave)" << endl;
		else
			StringStream << "textilename = AddTextile( LayeredTextile )" << endl;
	}
	else
	{
		if ( !m_bLayers )
			StringStream << "AddTextile('" << ExistingTextile << "', weave, True)" << endl;
		else
			StringStream << "AddTextile('" << ExistingTextile << "', LayeredTextile, True)" << endl;
	}
	return StringStream.str();
}

void CWeaveWizard::LoadSettings(const CTextileWeave2D &Weave)
{
	m_YarnSpacing.clear();
	m_YarnWidth.clear();
	m_FabricThickness.clear();
	m_GapSize.clear();
	m_pXYarnsSpin->SetValue(Weave.GetNumXYarns());
	m_pYYarnsSpin->SetValue(Weave.GetNumYYarns());
	m_YarnSpacing << Weave.GetAverageYarnSpacing();
	m_YarnWidth << Weave.GetAverageYarnWidth();
	m_FabricThickness << Weave.GetFabricThickness();
	m_GapSize << Weave.GetGapSize();
//	m_bCreateDomain = false;
	m_bRefine = Weave.GetRefine();
	m_bInPlaneTangents = Weave.GetInPlaneTangents();
	m_b3DWeave = false;
	m_pWeavePatternCtrl->SetWeaveSize(Weave);
	m_bShear = false;
	RefreshGapTextBox();
}

void CWeaveWizard::LoadSettings(const CShearedTextileWeave2D &Weave)
{
	m_YarnSpacing.clear();
	m_YarnWidth.clear();
	m_FabricThickness.clear();
	m_GapSize.clear();
	m_ShearAngle.Clear();
	m_pXYarnsSpin->SetValue(Weave.GetNumXYarns());
	m_pYYarnsSpin->SetValue(Weave.GetNumYYarns());
	m_YarnSpacing << Weave.GetAverageYarnSpacing();
	m_YarnWidth << Weave.GetAverageYarnWidth();
	m_FabricThickness << Weave.GetFabricThickness();
	m_GapSize << Weave.GetGapSize();

	m_bRefine = Weave.GetRefine();
	m_bInPlaneTangents = Weave.GetInPlaneTangents();
	m_b3DWeave = false;
	m_pWeavePatternCtrl->SetWeaveSize(Weave);
	m_bShear = true;
	wxCheckBox* pShear = (wxCheckBox*)FindWindowById(ID_ShearTextile, this);
	pShear->SetValue( m_bShear );
	m_ShearAngle << Weave.GetShearAngle()* 180.0 / PI;
	m_bShearedDomain = Weave.GetShearedDomain();
	if ( m_bShearedDomain )
		m_bCreateDomain = false;
	RefreshGapTextBox();
}

void CWeaveWizard::OnRefine(wxCommandEvent& event)
{
	RefreshGapTextBox();
}

void CWeaveWizard::On3DWeave(wxCommandEvent& event)
{
	RefreshGapTextBox();
}

/*void CWeaveWizard::OnWeavePattern(wxCommandEvent& event)
{
	if (m_pFirstPage && m_pFirstPage->Validate())
	{
		m_pFirstPage->TransferDataFromWindow();
		RebuildWeavePatternCtrl();
		m_pWeavePatternDialog->ShowModal();
	}
}*/

void CWeaveWizard::RefreshGapTextBox()
{
	wxCheckBox* p3DWeave = (wxCheckBox*)FindWindowById(ID_Create3DWeave, this);
	wxCheckBox* pRefine = (wxCheckBox*)FindWindowById(ID_Refine, this);
	wxTextCtrl* pGapSize = (wxTextCtrl*)FindWindowById(ID_GapSize, this);
	wxCheckBox* pInPlaneTangents = (wxCheckBox*)FindWindowById(ID_InPlaneTangents, this);
	wxCheckBox* pShear = (wxCheckBox*)FindWindowById(ID_ShearTextile, this);
	wxTextCtrl* pShearAngle = (wxTextCtrl*)FindWindowById(ID_ShearAngle, this);
	wxCheckBox* pShearedDomain = (wxCheckBox*)FindWindowById(ID_ShearedDomain, this);
	wxCheckBox* pLayers = (wxCheckBox*)FindWindowById(ID_CreateLayers, this);
	if (pRefine && pGapSize && p3DWeave && pInPlaneTangents && pShear && pShearedDomain && pLayers)
	{
		if (p3DWeave->GetValue())
		{
			pRefine->Disable();
			pGapSize->Disable();
			pInPlaneTangents->Disable();
			pShear->Disable();
			pShearAngle->Disable();
			pLayers->Disable();
			m_pLayersSpin->Disable();
		}
		else
		{
			pRefine->Enable();
			pInPlaneTangents->Enable();
			if (pRefine->GetValue())
				pGapSize->Enable();
			else
				pGapSize->Disable();

			pShear->Enable();
			if (pShear->GetValue())
			{
				pShearAngle->Enable();
				pShearedDomain->Enable();
			}
			else
			{
				pShearAngle->Disable();
				pShearedDomain->Disable();
			}
			pLayers->Enable();
			if (pLayers->GetValue())
			{
				m_pLayersSpin->Enable();
			}
			else
			{
				m_pLayersSpin->Disable();
			}
		}
	}
}

bool CWeaveWizard::HasNextPage(wxWizardPage *page)
{
	if (page == m_pFirstPage)
		return true;
	return wxWizard::HasNextPage(page);
}


void CWeaveWizard::OnInPlaneTangents(wxCommandEvent& event)
{
	RefreshGapTextBox();
}

void CWeaveWizard::OnShear(wxCommandEvent& event)
{
	RefreshGapTextBox();
}

void CWeaveWizard::OnDomain(wxCommandEvent& event)
{
	wxCheckBox* pShear = (wxCheckBox*)FindWindowById(ID_ShearTextile, this);
	wxCheckBox* pShearedDomain = (wxCheckBox*)FindWindowById(ID_ShearedDomain, this);
	wxCheckBox* pDefaultDomain = (wxCheckBox*)FindWindowById(ID_DefaultDomain, this);
	wxCheckBox* pDomainHeight = (wxCheckBox*)FindWindowById(ID_DomainHeight, this);

	if ( pDefaultDomain->GetValue() )
		pShearedDomain->SetValue( false );
	if ( pShearedDomain->GetValue() )
		pDefaultDomain->SetValue( false );

	if ( pDefaultDomain->GetValue() || pShearedDomain->GetValue() )
		pDomainHeight->Enable();
	else
		pDomainHeight->Disable();
}

void CWeaveWizard::OnShearedDomain(wxCommandEvent& event)
{
	wxCheckBox* pShear = (wxCheckBox*)FindWindowById(ID_ShearTextile, this);
	wxCheckBox* pShearedDomain = (wxCheckBox*)FindWindowById(ID_ShearedDomain, this);
	wxCheckBox* pDefaultDomain = (wxCheckBox*)FindWindowById(ID_DefaultDomain, this);
	wxCheckBox* pDomainHeight = (wxCheckBox*)FindWindowById(ID_DomainHeight, this);

	if ( pShearedDomain->GetValue() )
		pDefaultDomain->SetValue( false );
	if ( pDefaultDomain->GetValue() )
		pShearedDomain->SetValue( false );

	if ( pDefaultDomain->GetValue() || pShearedDomain->GetValue() )
		pDomainHeight->Enable();
	else
		pDomainHeight->Disable();
}

void CWeaveWizard::OnLayers(wxCommandEvent& event)
{
	RefreshGapTextBox();
}
