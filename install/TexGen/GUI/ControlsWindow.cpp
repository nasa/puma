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
#include "ControlsWindow.h"
#include "WindowIDs.h"
#include "Modeller.h"
#include "CustomWidget.h"

CControlsWindow::CControlsWindow(wxMenuBar* pMenuBar, wxWindow* parent, wxWindowID id)
: wxChoicebook(parent, id)
, m_pMenuBar(pMenuBar)
, m_bUpdatingPositionText(false)
{
	BuildControls();
}

CControlsWindow::~CControlsWindow(void)
{
}

void CControlsWindow::BuildControls()
{
	BuildTextilesPage();
	BuildModellerPage();
	BuildDomainsPage();
	BuildRenderingPage();
	BuildPythonPage();
	BuildToolsPage();
	BuildOptionsPage();
	ResizePages();
}

void CControlsWindow::ResizePages()
{
	size_t i;
	wxSizer *pSizer;
	int x = 0;
	int y = 0;
	for (i=0; i<GetPageCount(); ++i)
	{
		pSizer = GetPage(i)->GetSizer();
		if (pSizer)
		{
			x = max(x, pSizer->GetMinSize().x);
			y = max(y, pSizer->GetMinSize().y);
		}
	}
	for (i=0; i<GetPageCount(); ++i)
	{
		pSizer = GetPage(i)->GetSizer();
		if (pSizer)
		{
			pSizer->SetDimension(0, 0, x, y);
		}
	}
	SetSize(x, y);
}

void CControlsWindow::BuildTextilesPage()
{
	wxNotebookPage *pControls = new wxNotebookPage(this, wxID_ANY);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
//	SizerFlags.Border(wxALL, 1);
	SizerFlags.Expand();

	vector<wxSizer*> SubSizers;
	wxSizer *pSubSizer;

	pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Create:"));
	pSubSizer->Add(new wxButton(pControls, ID_CreateEmptyTextile, wxT("Empty"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pSubSizer->Add(new wxButton(pControls, ID_Create2DWeave, wxT("Weave"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pSubSizer->Add(new wxButton(pControls, ID_Create3DTextile, wxT("3D Weave"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pSubSizer->Add(new wxButton(pControls, ID_CreateLayeredTextile, wxT("Layered"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
//	pSubSizer->Add(new wxButton(pControls, wxID_ANY, "Braid", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pMainSizer->Add(pSubSizer, SizerFlags);
	SubSizers.push_back(pSubSizer);

//	NOTE: TEMPORARILY DISABLED UNTIL A BETTER WAY TO DEAL WITH LONG RUNNING TASKS IS FOUND...
/*	pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Algorithms:"));
	pSubSizer->Add(new wxButton(pControls, ID_GeometrySolve, wxT("Geometry Solve"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pMainSizer->Add(pSubSizer, SizerFlags);
	SubSizers.push_back(pSubSizer);*/

	pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("")/*"Existing:"*/);
	pSubSizer->Add(new wxButton(pControls, ID_EditTextile, wxT("Edit"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pSubSizer->Add(new wxButton(pControls, ID_DeleteTextile, wxT("Delete"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pSubSizer->Add(new wxButton(pControls, ID_RotateTextile, wxT("Rotate"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pMainSizer->Add(pSubSizer, SizerFlags);
	SubSizers.push_back(pSubSizer);

	int i;
	for (i=0; i<(int)SubSizers.size(); ++i)
	{
		SubSizers[i]->SetDimension(0, 0, pMainSizer->GetMinSize().x, SubSizers[i]->GetMinSize().y);
	}

	pControls->SetSizer(pMainSizer);
	AddPage(pControls, wxT("Textiles"));

	wxMenu *pMenu = new wxMenu;
	pMenu->Append(ID_CreateEmptyTextile, wxT("Create Empty..."));
	pMenu->Append(ID_Create2DWeave, wxT("Create Weave..."));
	pMenu->Append(ID_Create3DTextile, wxT("Create 3D Weave..."));
	pMenu->Append(ID_CreateLayeredTextile, wxT("Create Layered..."));
	pMenu->Append(ID_SetLayerOffsets, wxT("Set Layer Offsets"));
	//pMenu->Append(ID_NestLayers, wxT("&Nest Layers"));
	{
		wxMenu *pNestingSubMenu = new wxMenu;
		{
			pNestingSubMenu->Append(ID_NestLayers, wxT("Keep Offsets"));
			pNestingSubMenu->Append(ID_MaxNestLayers, wxT("Maximum nesting"));
		}
		pMenu->Append(wxID_ANY, wxT("&Nest Layers"), pNestingSubMenu);
	}
//	pMenu->Append(wxID_ANY, "Create Braid...");
	pMenu->AppendSeparator();
//	NOTE: TEMPORARILY DISABLED UNTIL A BETTER WAY TO DEAL WITH LONG RUNNING TASKS IS FOUND...
/*	pMenu->Append(ID_GeometrySolve, wxT("Geometry Solve..."));
	pMenu->AppendSeparator();*/

	pMenu->Append(ID_EditTextile, wxT("Edit Textile..."));
	pMenu->Append(ID_DeleteTextile, wxT("Delete Textile"));
	pMenu->Append(ID_RotateTextile, wxT("Rotate Textile"));
	m_pMenuBar->Append(pMenu, wxT("&Textiles"));
}

void CControlsWindow::BuildModellerPage()
{
	wxNotebookPage *pControls = new wxNotebookPage(this, wxID_ANY);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
//	SizerFlags.Border(wxALL, 1);
	SizerFlags.Expand();

	vector<wxSizer*> SubSizers;
	wxSizer *pSubSizer;
	{
		pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Create:"));
		{
			pSubSizer->Add(new wxButton(pControls, ID_CreateYarn, wxT("Yarn"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
		}
		pMainSizer->Add(pSubSizer, SizerFlags);
		SubSizers.push_back(pSubSizer);

		pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Assign:"));
		{
			pSubSizer->Add(new wxButton(pControls, ID_AssignSection, wxT("Section"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
			pSubSizer->Add(new wxButton(pControls, ID_AssignInterpolation, wxT("Interpolation"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
			pSubSizer->Add(new wxButton(pControls, ID_AssignRepeats, wxT("Repeats"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
			pSubSizer->Add(new wxButton(pControls, ID_AssignProperties, wxT("Yarn Properties"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
			pSubSizer->Add(new wxButton(pControls, ID_AssignMatrixProperties, wxT("Matrix Properties"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
		}
		pMainSizer->Add(pSubSizer, SizerFlags);
		SubSizers.push_back(pSubSizer);

/*		pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Dialogs:"));
		{
			pSubSizer->Add(new wxButton(pControls, ID_ShowOutliner, wxT("Outliner"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
		}
		pMainSizer->Add(pSubSizer, SizerFlags);
		SubSizers.push_back(pSubSizer);*/

		wxSizer *pSubSubSizer;
		wxTextCtrl *pTextCtrl;
		pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Position:"));
		{
			pSubSubSizer = new wxBoxSizer(wxHORIZONTAL);
			pSubSubSizer->Add(new wxStaticText(pControls, wxID_ANY, wxT("X:")), wxSizerFlags(0).Centre());
			pSubSubSizer->Add(pTextCtrl = new wxTextCtrl(pControls, ID_PositionX, wxT("0.0"), wxDefaultPosition, wxSize(50, -1)), wxSizerFlags(1).Centre());
			pTextCtrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_PositionX));
			pSubSizer->Add(pSubSubSizer, SizerFlags);
		}
		{
			pSubSubSizer = new wxBoxSizer(wxHORIZONTAL);
			pSubSubSizer->Add(new wxStaticText(pControls, wxID_ANY, wxT("Y:")), wxSizerFlags(0).Centre());
			pSubSubSizer->Add(pTextCtrl = new wxTextCtrl(pControls, ID_PositionY, wxT("0.0"), wxDefaultPosition, wxSize(50, -1)), wxSizerFlags(1).Centre());
			pTextCtrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_PositionY));
			pSubSizer->Add(pSubSubSizer, SizerFlags);
		}
		{
			pSubSubSizer = new wxBoxSizer(wxHORIZONTAL);
			pSubSubSizer->Add(new wxStaticText(pControls, wxID_ANY, wxT("Z:")), wxSizerFlags(0).Centre());
			pSubSubSizer->Add(pTextCtrl = new wxTextCtrl(pControls, ID_PositionZ, wxT("0.0"), wxDefaultPosition, wxSize(50, -1)), wxSizerFlags(1).Centre());
			pTextCtrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_PositionZ));
			pSubSizer->Add(pSubSubSizer, SizerFlags);
		}
		pMainSizer->Add(pSubSizer, SizerFlags);
		SubSizers.push_back(pSubSizer);

		pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Snapping:"));
		{
			pSubSizer->Add(new wxCheckBox(pControls, ID_Snap, wxT("Snap")), SizerFlags);
			pSubSizer->Add(new wxCheckBox(pControls, ID_Relative, wxT("Relative")), SizerFlags);

			{
				pSubSubSizer = new wxBoxSizer(wxHORIZONTAL);
				pSubSubSizer->Add(new wxStaticText(pControls, wxID_ANY, wxT("Size:")), wxSizerFlags(0).Centre());
				pSubSubSizer->Add(pTextCtrl = new wxTextCtrl(pControls, ID_SnapSize, wxT("0.1"), wxDefaultPosition, wxSize(50, -1)), wxSizerFlags(1).Centre());
				pTextCtrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_SnapSize));
				pSubSizer->Add(pSubSubSizer, SizerFlags);
			}
		}
		pMainSizer->Add(pSubSizer, SizerFlags);
		SubSizers.push_back(pSubSizer);

		pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Tool:"));
		{
			pSubSizer->Add(new wxRadioButton(pControls, ID_SelectTool, wxT("Select"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP), SizerFlags);
			pSubSizer->Add(new wxRadioButton(pControls, ID_MoveTool, wxT("Move"), wxDefaultPosition, wxDefaultSize), SizerFlags);
			pSubSizer->Add(new wxRadioButton(pControls, ID_RotateTool, wxT("Rotate"), wxDefaultPosition, wxDefaultSize), SizerFlags);
			pSubSizer->Add(new wxRadioButton(pControls, ID_ScaleTool, wxT("Scale"), wxDefaultPosition, wxDefaultSize), SizerFlags);
		}
		pMainSizer->Add(pSubSizer, SizerFlags);
		SubSizers.push_back(pSubSizer);

		pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Selection Filter:"));
		{
			pSubSizer->Add(new wxCheckBox(pControls, ID_FilterNodes, wxT("Nodes")), SizerFlags);
			pSubSizer->Add(new wxCheckBox(pControls, ID_FilterPath, wxT("Paths")), SizerFlags);
			pSubSizer->Add(new wxCheckBox(pControls, ID_FilterSurface, wxT("Surfaces")), SizerFlags);
		}
		pMainSizer->Add(pSubSizer, SizerFlags);
		SubSizers.push_back(pSubSizer);
	}

	int i;
	for (i=0; i<(int)SubSizers.size(); ++i)
	{
		SubSizers[i]->SetDimension(0, 0, pMainSizer->GetMinSize().x, SubSizers[i]->GetMinSize().y);
	}

	pControls->SetSizer(pMainSizer);
	AddPage(pControls, wxT("Modeller"));

	wxMenu *pMenu = new wxMenu;
	pMenu->Append(ID_CreateYarn, wxT("Create Yarn..."));
	pMenu->AppendSeparator();
	pMenu->Append(ID_AssignSection, wxT("Assign Section..."));
	pMenu->Append(ID_AssignInterpolation, wxT("Assign Interpolation..."));
	pMenu->Append(ID_AssignRepeats, wxT("Assign Repeats..."));
	pMenu->Append(ID_AssignProperties, wxT("Assign Yarn Properties..."));
	pMenu->Append(ID_AssignMatrixProperties, wxT("Assign Matrix Properties..."));
	pMenu->AppendSeparator();
	pMenu->Append(ID_Snap, wxT("Snap"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_Relative, wxT("Relative Snap"), wxT(""), wxITEM_CHECK);
	pMenu->AppendSeparator();
	pMenu->Append(ID_SelectTool, wxT("Select Tool"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_MoveTool, wxT("Move Tool"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_RotateTool, wxT("Rotate Tool"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_ScaleTool, wxT("Scale Tool"), wxT(""), wxITEM_CHECK);
	pMenu->AppendSeparator();
	pMenu->Append(ID_FilterNodes, wxT("Select Nodes"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_FilterPath, wxT("Select Paths"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_FilterSurface, wxT("Select Surfaces"), wxT(""), wxITEM_CHECK);
	m_pMenuBar->Append(pMenu, wxT("&Modeller"));
}

void CControlsWindow::BuildRenderingPage()
{
	wxNotebookPage *pControls = new wxNotebookPage(this, wxID_ANY);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
//	SizerFlags.Border(wxALL, 1);
	SizerFlags.Expand();

	vector<wxSizer*> SubSizers;
	wxSizer *pSubSizer;

	pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Display:"));
	pSubSizer->Add(new wxCheckBox(pControls, ID_RenderNodes, wxT("Nodes")), SizerFlags);
	pSubSizer->Add(new wxCheckBox(pControls, ID_RenderPaths, wxT("Paths")), SizerFlags);
	pSubSizer->Add(new wxCheckBox(pControls, ID_RenderSurface, wxT("Surface")), SizerFlags);
	pSubSizer->Add(new wxCheckBox(pControls, ID_RenderVolume, wxT("Volume")), SizerFlags);
	pSubSizer->Add(new wxCheckBox(pControls, ID_RenderInterference, wxT("Interference")), SizerFlags);
	pSubSizer->Add(new wxCheckBox(pControls, ID_RenderInterferenceDepth, wxT("Interference Depth")), SizerFlags);
	pSubSizer->Add(new wxCheckBox(pControls, ID_RenderOrientation, wxT("Orientation")), SizerFlags);
	pMainSizer->Add(pSubSizer, SizerFlags);
	SubSizers.push_back(pSubSizer);

	pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT(""));
	pSubSizer->Add(new wxCheckBox(pControls, ID_RenderDomain, wxT("Domain")), SizerFlags);
	pSubSizer->Add(new wxCheckBox(pControls, ID_RenderDomainAxes, wxT("Domain Axes")), SizerFlags);
	pSubSizer->Add(new wxCheckBox(pControls, ID_RenderAxes, wxT("Axes")), SizerFlags);
	pMainSizer->Add(pSubSizer, SizerFlags);
	SubSizers.push_back(pSubSizer);

	pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT(""));
	pSubSizer->Add(new wxCheckBox(pControls, ID_XRay, wxT("X-Ray")), SizerFlags);
	pSubSizer->Add(new wxCheckBox(pControls, ID_ParallelProjection, wxT("Parallel Projection")), SizerFlags);
	pSubSizer->Add(new wxCheckBox(pControls, ID_TrimtoDomain, wxT("Trim to domain")), SizerFlags);
	pMainSizer->Add(pSubSizer, SizerFlags);
	SubSizers.push_back(pSubSizer);

	pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Colors"));
	pSubSizer->Add(new wxButton(pControls, ID_ChangeBackgroundColor, wxT("Background")), SizerFlags);
	pSubSizer->Add(new wxButton(pControls, ID_ChangeSurfaceColor, wxT("Surface")), SizerFlags);
	pSubSizer->Add(new wxButton(pControls, ID_RefreshView, wxT("Refresh View")), SizerFlags);
	pMainSizer->Add(pSubSizer, SizerFlags);
	SubSizers.push_back(pSubSizer);

	int i;
	for (i=0; i<(int)SubSizers.size(); ++i)
	{
		SubSizers[i]->SetDimension(0, 0, pMainSizer->GetMinSize().x, SubSizers[i]->GetMinSize().y);
	}

	pControls->SetSizer(pMainSizer);
	AddPage(pControls, wxT("Rendering"));

	wxMenu *pMenu = new wxMenu;
	pMenu->Append(ID_RenderNodes, wxT("Render Textile Nodes"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_RenderPaths, wxT("Render Textile Paths"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_RenderSurface, wxT("Render Textile Surface"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_RenderVolume, wxT("Render Textile Volume"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_RenderInterference, wxT("Render Textile Interference"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_RenderInterferenceDepth, wxT("Render Textile Interference Depth"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_RenderOrientation, wxT("Render Fibre Orientation"), wxT(""), wxITEM_CHECK);
	pMenu->AppendSeparator();
	pMenu->Append(ID_RenderDomain, wxT("Render Domain"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_RenderDomainAxes, wxT("Render Domain Axes"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_RenderAxes, wxT("Render Axes"), wxT(""), wxITEM_CHECK);
	pMenu->AppendSeparator();
	pMenu->Append(ID_XRay, wxT("X-Ray"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_ParallelProjection, wxT("Parallel Projection"), wxT(""), wxITEM_CHECK);
	pMenu->Append(ID_TrimtoDomain, wxT("Trim to Domain"), wxT(""), wxITEM_CHECK);
	pMenu->AppendSeparator();
	pMenu->Append(ID_ChangeBackgroundColor, wxT("Change Background Colour"));
	pMenu->Append(ID_ChangeSurfaceColor, wxT("Change Surface Colour"));
	pMenu->Append(ID_RefreshView, wxT("RefreshView"));
	m_pMenuBar->Append(pMenu, wxT("&Rendering"));
}

void CControlsWindow::BuildDomainsPage()
{
	wxNotebookPage *pControls = new wxNotebookPage(this, wxID_ANY);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
//	SizerFlags.Border(wxALL, 1);
	SizerFlags.Expand();

	vector<wxSizer*> SubSizers;
	wxSizer *pSubSizer;

	pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Create:"));
	pSubSizer->Add(new wxButton(pControls, ID_CreateDomainBox, wxT("Box"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pSubSizer->Add(new wxButton(pControls, ID_CreateDomainPlanes, wxT("Planes"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pMainSizer->Add(pSubSizer, SizerFlags);
	SubSizers.push_back(pSubSizer);

	pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("")/*"Existing:"*/);
	pSubSizer->Add(new wxButton(pControls, ID_EditDomain, wxT("Edit"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pSubSizer->Add(new wxButton(pControls, ID_DeleteDomain, wxT("Delete"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pMainSizer->Add(pSubSizer, SizerFlags);
	SubSizers.push_back(pSubSizer);

	int i;
	for (i=0; i<(int)SubSizers.size(); ++i)
	{
		SubSizers[i]->SetDimension(0, 0, pMainSizer->GetMinSize().x, SubSizers[i]->GetMinSize().y);
	}

	pControls->SetSizer(pMainSizer);
	AddPage(pControls, wxT("Domain"));

	wxMenu *pMenu = new wxMenu;
	pMenu->Append(ID_CreateDomainBox, wxT("Create Box..."));
	pMenu->Append(ID_CreateDomainPlanes, wxT("Create Planes..."));
	pMenu->AppendSeparator();
	pMenu->Append(ID_EditDomain, wxT("Edit Domain..."));
	pMenu->Append(ID_DeleteDomain, wxT("Delete Domain"));
	m_pMenuBar->Append(pMenu, wxT("&Domain"));
}

void CControlsWindow::BuildPythonPage()
{
	wxNotebookPage *pControls = new wxNotebookPage(this, wxID_ANY);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
//	SizerFlags.Border(wxALL, 1);
	SizerFlags.Expand();

	vector<wxSizer*> SubSizers;
	wxSizer *pSubSizer;

	pSubSizer = new wxStaticBoxSizer(wxVERTICAL, pControls, wxT("Script:"));
	pSubSizer->Add(new wxButton(pControls, ID_RunScript, wxT("Run"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pSubSizer->Add(new wxButton(pControls, ID_RecordMacro, wxT("Record"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT), SizerFlags);
	pMainSizer->Add(pSubSizer, SizerFlags);
	SubSizers.push_back(pSubSizer);

	int i;
	for (i=0; i<(int)SubSizers.size(); ++i)
	{
		SubSizers[i]->SetDimension(0, 0, pMainSizer->GetMinSize().x, SubSizers[i]->GetMinSize().y);
	}

	pControls->SetSizer(pMainSizer);
	AddPage(pControls, wxT("Python"));

	wxMenu *pMenu = new wxMenu;
	pMenu->Append(ID_RunScript, wxT("Run Script..."));
	pMenu->Append(ID_RecordMacro, wxT("Record Script..."));
	m_pMenuBar->Append(pMenu, wxT("&Python"));
}

void CControlsWindow::UpdateModellingPage(const CModeller *pModeller)
{
	wxRadioButton* pRadioButton = NULL;
	bool bChecked = false;
	int i;
	int iID;
	for (i=TOOL_SELECT; i<=TOOL_SCALE; ++i)
	{
		bChecked = (i == pModeller->GetActiveTool());
		switch (i)
		{
		case TOOL_SELECT:
			iID = ID_SelectTool;
			break;
		case TOOL_MOVE:
			iID = ID_MoveTool;
			break;
		case TOOL_ROTATE:
			iID = ID_RotateTool;
			break;
		case TOOL_SCALE:
			iID = ID_ScaleTool;
			break;
		}
		pRadioButton = (wxRadioButton*)FindWindowById(iID, this);
		if (pRadioButton)
		{
			pRadioButton->SetValue(bChecked);
		}
		m_pMenuBar->Check(iID, bChecked);
	}
	UpdateCheckWidget(ID_FilterNodes, pModeller->GetSelectNodes());
	UpdateCheckWidget(ID_FilterPath, pModeller->GetSelectPaths());
	UpdateCheckWidget(ID_FilterSurface, pModeller->GetSelectSurfaces());
	UpdateCheckWidget(ID_Snap, pModeller->GetWidget()->GetSnap());
	UpdateCheckWidget(ID_Relative, pModeller->GetWidget()->GetRelativeSnap());
	UpdatePositionText(pModeller);
	UpdateSnapSize(pModeller);
}

void CControlsWindow::UpdatePositionText(const CModeller *pModeller)
{
	m_bUpdatingPositionText = true;
	XYZ Pos = pModeller->GetPosition();
	if (abs(Pos.x) < 1e-6)
		Pos.x = 0;
	if (abs(Pos.y) < 1e-6)
		Pos.y = 0;
	if (abs(Pos.z) < 1e-6)
		Pos.z = 0;
	m_PositionX.clear();
	m_PositionY.clear();
	m_PositionZ.clear();
	m_PositionX << Pos.x;
	m_PositionY << Pos.y;
	m_PositionZ << Pos.z;
	int i;
	for (i=ID_PositionX; i<=ID_PositionZ; ++i)
	{
		wxTextCtrl* pTextCtrl = dynamic_cast<wxTextCtrl*>(FindWindowById(i, this));
		if (pTextCtrl)
		{
			wxValidator* pValidator = pTextCtrl->GetValidator();
			pValidator->TransferToWindow();
		}
	}
	m_bUpdatingPositionText = false;
}

void CControlsWindow::UpdateSnapSize(const CModeller *pModeller)
{
	double dSnapSize = pModeller->GetWidget()->GetSnapSize();
	m_SnapSize.clear();
	m_SnapSize << dSnapSize;
	wxTextCtrl* pTextCtrl = dynamic_cast<wxTextCtrl*>(FindWindowById(ID_SnapSize, this));
	if (pTextCtrl)
	{
		wxValidator* pValidator = pTextCtrl->GetValidator();
		pValidator->TransferToWindow();
	}
}

void CControlsWindow::UpdateRenderingPage(const CTexGenRenderer *pRenderer)
{
	UpdateRenderWidget(ID_RenderNodes, pRenderer, CTexGenRenderer::PROP_NODE);
	UpdateRenderWidget(ID_RenderPaths, pRenderer, CTexGenRenderer::PROP_PATH);
	UpdateRenderWidget(ID_RenderSurface, pRenderer, CTexGenRenderer::PROP_SURFACE);
	UpdateRenderWidget(ID_RenderVolume, pRenderer, CTexGenRenderer::PROP_VOLUME);
	UpdateRenderWidget(ID_RenderInterference, pRenderer, CTexGenRenderer::PROP_INTERFERENCE);
	UpdateRenderWidget(ID_RenderInterferenceDepth, pRenderer, CTexGenRenderer::PROP_INTERFERENCE_DEPTH);
	UpdateRenderWidget(ID_RenderOrientation, pRenderer, CTexGenRenderer::PROP_ORIENTATION);
	UpdateRenderWidget(ID_RenderDomain, pRenderer, CTexGenRenderer::PROP_DOMAIN);
	UpdateRenderWidget(ID_RenderDomainAxes, pRenderer, CTexGenRenderer::PROP_DOMAINAXES);
	UpdateRenderWidget(ID_RenderAxes, pRenderer, CTexGenRenderer::PROP_AXES);
	// Find out if the widget should be checked or not
	bool bChecked = pRenderer->GetXRay();
	wxCheckBox* pCheckBox = (wxCheckBox*)FindWindowById(ID_XRay, this);
	if (pCheckBox)
	{
		pCheckBox->SetValue(bChecked);
	}
	m_pMenuBar->Check(ID_XRay, bChecked);
	// Find out if the parallel projection widget should be checked or not
	bChecked = pRenderer->GetParallelProjection();
	pCheckBox = (wxCheckBox*)FindWindowById(ID_ParallelProjection, this);
	if (pCheckBox)
	{
		pCheckBox->SetValue(bChecked);
	}
	m_pMenuBar->Check(ID_ParallelProjection, bChecked);
	// Find out if the widget should be checked or not
	bChecked = pRenderer->GetTrimToDomain();
	pCheckBox = (wxCheckBox*)FindWindowById(ID_TrimtoDomain, this);
	if (pCheckBox)
	{
		pCheckBox->SetValue(bChecked);
	}
	m_pMenuBar->Check(ID_TrimtoDomain, bChecked);
}

void CControlsWindow::UpdatePythonPage(bool bRecording)
{
	wxButton* pButton = (wxButton*)FindWindowById(ID_RecordMacro, this);
	if (pButton)
	{
		if (bRecording)
			pButton->SetLabel(wxT("Stop Recording"));
		else
			pButton->SetLabel(wxT("Record"));
	}
	wxMenuItem* pMenuItem = m_pMenuBar->FindItem(ID_RecordMacro);
	if (pMenuItem)
	{
		if (bRecording)
			pMenuItem->SetText(wxT("Stop Recording"));
		else
			pMenuItem->SetText(wxT("Record Script..."));
	}
}

void CControlsWindow::UpdateCheckWidget(int iID, bool bChecked)
{
	wxCheckBox* pCheckBox;
	pCheckBox = (wxCheckBox*)FindWindowById(iID, this);
	if (pCheckBox)
	{
		pCheckBox->SetValue(bChecked);
	}
	m_pMenuBar->Check(iID, bChecked);
}

void CControlsWindow::UpdateRenderWidget(int iID, const CTexGenRenderer *pRenderer, CTexGenRenderer::PROP_TYPE Type)
{
	// Find out if the widget should be checked or not
	bool bChecked = (pRenderer->GetNumProps(Type) > 0);
	// Check the checkbox on the controls list
	wxCheckBox* pCheckBox = (wxCheckBox*)FindWindowById(iID, this);
	if (pCheckBox)
	{
		pCheckBox->SetValue(bChecked);
	}
	// Check the drop down menu item
	m_pMenuBar->Check(iID, bChecked);
}

void CControlsWindow::BuildToolsPage()
{
	wxMenu *pMenu = new wxMenu;
	pMenu->Append(ID_PatternDraft, wxT("Create Pattern Draft"));
	pMenu->Append(ID_QuickDomainVolumeFraction, wxT("Quick Calculate Domain Volume Fraction"));
	pMenu->Append(ID_DomainVolumeFraction, wxT("Calculate Domain Volume Fraction"));
	pMenu->Append(ID_YarnFibreVolumeFraction, wxT("Yarn Fibre Volume Fraction"));
	m_pMenuBar->Append(pMenu, wxT("&Tools"));
}

void CControlsWindow::BuildOptionsPage()
{
	wxMenu *pMenu = new wxMenu;
	pMenu->Append(ID_OutputMessages, wxT("Output Messages"), wxT(""), wxITEM_CHECK );
	pMenu->Check(ID_OutputMessages, CTexGen::GetInstance().GetMessagesOn());
	m_pMenuBar->Append(pMenu, wxT("&Options"));
}


