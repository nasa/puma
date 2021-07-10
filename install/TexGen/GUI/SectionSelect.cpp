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
#include "SectionSelect.h"
#include "WindowIDs.h"
#include "TexGenApp.h"
#include "PythonConverter.h"
#include "CircleRangeCtrl.h"

BEGIN_EVENT_TABLE(CSectionSelect, wxDialog)
	EVT_BUTTON(wxID_OK, CSectionSelect::OnOK)
//	EVT_BUTTON(wxID_CANCEL, CSectionSelect::OnCancel)
	EVT_INIT_DIALOG(CSectionSelect::OnInit)
	EVT_CHECKBOX(XRCID("Rotate"), CSectionSelect::OnRotate)
	EVT_CHECKBOX(XRCID("Scale"), CSectionSelect::OnScale)
	EVT_TEXT(XRCID("EllipseWidth"), CSectionSelect::OnTextChanged)
	EVT_TEXT(XRCID("EllipseHeight"), CSectionSelect::OnTextChanged)

	EVT_TEXT(XRCID("LenticularWidth"), CSectionSelect::OnTextChanged)
	EVT_TEXT(XRCID("LenticularHeight"), CSectionSelect::OnTextChanged)
	EVT_TEXT(XRCID("LenticularDistortion"), CSectionSelect::OnTextChanged)

	EVT_TEXT(XRCID("PowerEllipseWidth"), CSectionSelect::OnTextChanged)
	EVT_TEXT(XRCID("PowerEllipseHeight"), CSectionSelect::OnTextChanged)
	EVT_TEXT(XRCID("PowerEllipsePower"), CSectionSelect::OnTextChanged)
	EVT_TEXT(XRCID("PowerEllipseXOffset"), CSectionSelect::OnTextChanged)

	EVT_TEXT(XRCID("RectangleWidth"), CSectionSelect::OnTextChanged)
	EVT_TEXT(XRCID("RectangleHeight"), CSectionSelect::OnTextChanged)

	EVT_DIVISION_ADDED(XRCID("HybridControl"), CSectionSelect::OnDivisionChanged)
	EVT_DIVISION_DELETED(XRCID("HybridControl"), CSectionSelect::OnDivisionChanged)
	EVT_DIVISION_DRAGGING(XRCID("HybridControl"), CSectionSelect::OnDivisionChanged)
	EVT_SECTION_CLICKED(XRCID("HybridControl"), CSectionSelect::OnSectionClicked)

	EVT_TEXT(XRCID("Angle"), CSectionSelect::OnTextChanged)
	EVT_TEXT(XRCID("XScale"), CSectionSelect::OnTextChanged)
	EVT_TEXT(XRCID("YScale"), CSectionSelect::OnTextChanged)
	EVT_CHOICEBOOK_PAGE_CHANGED(XRCID("SectionType"), CSectionSelect::OnSectionTypeChanged)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(CPreviewWindowPainter, wxEvtHandler)
	EVT_PAINT(CPreviewWindowPainter::OnPaint)
END_EVENT_TABLE()

CSectionSelect::CSectionSelect()
: m_Width(wxT("1.0"))
, m_Height(wxT("1.0"))
, m_Distortion(wxT("0.0"))
, m_Power(wxT("1.0"))
, m_Angle(wxT("0.0"))
, m_XScale(wxT("1.0"))
, m_YScale(wxT("1.0"))
, m_XOffset(wxT("0.0"))
, m_bRotate(false)
, m_bScale(false)
, m_bFinishedInit(false)
, m_pPreviewPanel(NULL)
{
	BuildDialog();
}

CSectionSelect::~CSectionSelect(void)
{
	m_pPreviewPanel->PopEventHandler( true );
}

void CSectionSelect::BuildDialog()
{
	if (wxXmlResource::Get()->LoadDialog(this, ((CTexGenApp*)wxTheApp)->GetMainFrame(), wxT("SectionSelect")))
	{
		m_pPreviewPanel = XRCCTRL(*this, "PreviewPanel", wxPanel);
		m_pPreviewPanel->PushEventHandler(new CPreviewWindowPainter(*this));

		XRCCTRL(*this, "EllipseWidth", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_Width));
		XRCCTRL(*this, "EllipseHeight", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_Height));

		XRCCTRL(*this, "LenticularWidth", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_Width));
		XRCCTRL(*this, "LenticularHeight", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_Height));
		XRCCTRL(*this, "LenticularDistortion", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_Distortion));

		XRCCTRL(*this, "PowerEllipseWidth", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_Width));
		XRCCTRL(*this, "PowerEllipseHeight", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_Height));
		XRCCTRL(*this, "PowerEllipsePower", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_Power));
		XRCCTRL(*this, "PowerEllipseXOffset", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_XOffset));

		XRCCTRL(*this, "RectangleWidth", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_Width));
		XRCCTRL(*this, "RectangleHeight", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_Height));

		// Make an instance of our new custom class.
		wxCircleRangeCtrl* pHybridCtrl = new wxCircleRangeCtrl(this, wxID_ANY);
		pHybridCtrl->SetSize(wxSize(200, 80));
		pHybridCtrl->SetMinSize(wxSize(200, 80));
		// Attach it to the dialog
		wxXmlResource::Get()->AttachUnknownControl(wxT("HybridControl"), pHybridCtrl);

		XRCCTRL(*this, "Rotate", wxCheckBox)->SetValidator(wxGenericValidator(&m_bRotate));
		XRCCTRL(*this, "Angle", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_Angle));

		XRCCTRL(*this, "Scale", wxCheckBox)->SetValidator(wxGenericValidator(&m_bScale));
		XRCCTRL(*this, "XScale", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_XScale));
		XRCCTRL(*this, "YScale", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_YScale));
	}
}

void CSectionSelect::OnInit(wxInitDialogEvent& event)
{
	XRCCTRL(*this, "Ellipse", wxPanel)->TransferDataToWindow();
	XRCCTRL(*this, "Lenticular", wxPanel)->TransferDataToWindow();
	XRCCTRL(*this, "PowerEllipse", wxPanel)->TransferDataToWindow();
	XRCCTRL(*this, "Rectangle", wxPanel)->TransferDataToWindow();
	TransferDataToWindow();
	RefreshTextBoxes();
	UpdateSectionFromDialog();
	m_bFinishedInit = true;

	wxPanel* pPanel = XRCCTRL(*this, "PreviewPanel", wxPanel);

	// Goes with wxBufferedPaintDC
	pPanel->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void CSectionSelect::OnOK(wxCommandEvent& event)
{
	UpdateSectionFromDialog();
	if (IsModal())
	{
		EndModal(wxID_OK);
	}
	else
	{
		SetReturnCode(wxID_OK);
		this->Show(false);
	}
}
/*
void CSectionSelect::OnCancel(wxCommandEvent& event)
{
	if (IsModal())
	{
		EndModal(wxID_CANCEL);
	}
	else
	{
		SetReturnCode(wxID_CANCEL);
		this->Show(false);
	}
}
*/
string CSectionSelect::GetCreateSectionCommand(string SectionVariableName)
{
	UpdateSectionFromDialog();
	return ConvertSection(*m_Section, SectionVariableName);
}

void CSectionSelect::UpdateSectionFromDialog()
{
	if (!Validate())
		return;
	if (!TransferDataFromWindow())
		return;
	wxChoicebook* pChoicebook = XRCCTRL(*this, "SectionType", wxChoicebook);
	wxNotebookPage* pPage = pChoicebook->GetCurrentPage();
	if (pPage)
	{
		if (!pPage->Validate())
			return;
		if (!pPage->TransferDataFromWindow())
			return;
		if (pPage == XRCCTRL(*this, "Ellipse", wxPanel))
		{
			double dWidth=1, dHeight=1;
			m_Width.ToDouble(&dWidth);
			m_Height.ToDouble(&dHeight);
			m_Section = CSectionEllipse(dWidth, dHeight);
		}
		else if (pPage == XRCCTRL(*this, "Lenticular", wxPanel))
		{
			double dWidth=1, dHeight=1, dDistortion=0;
			m_Width.ToDouble(&dWidth);
			m_Height.ToDouble(&dHeight);
			m_Distortion.ToDouble(&dDistortion);
			m_Section = CSectionLenticular(dWidth, dHeight, dDistortion);
		}
		else if (pPage == XRCCTRL(*this, "PowerEllipse", wxPanel))
		{
			double dWidth=1, dHeight=1, dPower=1, dXOffset=1;
			m_Width.ToDouble(&dWidth);
			m_Height.ToDouble(&dHeight);
			m_Power.ToDouble(&dPower);
			m_XOffset.ToDouble(&dXOffset);
			m_Section = CSectionPowerEllipse(dWidth, dHeight, dPower, dXOffset);
		}
		else if (pPage == XRCCTRL(*this, "Rectangle", wxPanel))
		{
			double dWidth=1, dHeight=1;
			m_Width.ToDouble(&dWidth);
			m_Height.ToDouble(&dHeight);
			m_Section = CSectionRectangle(dWidth, dHeight);
		}
		else if (pPage == XRCCTRL(*this, "Hybrid", wxPanel))
		{
			BuildHybridSection();
		}
	}
	if (m_bRotate)
	{
		double dAngle;
		m_Angle.ToDouble(&dAngle);
		m_Section = CSectionRotated(*m_Section, dAngle*PI/180.0);
	}
	if (m_bScale)
	{
		XY Scale;
		m_XScale.ToDouble(&Scale.x);
		m_YScale.ToDouble(&Scale.y);
		m_Section = CSectionScaled(*m_Section, Scale);
	}
	DrawSectionPreview();
}

void CSectionSelect::BuildHybridSection()
{
	CSectionHybrid Hybrid;
	int i;
	wxCircleRangeCtrl* pCircleRangeCtrl = XRCCTRL(*this, "HybridControl", wxCircleRangeCtrl);
	pair<double, double> Bounds;
	for (i=0; i<pCircleRangeCtrl->GetNumDivisions(); ++i)
	{
		Hybrid.AddDivision(pCircleRangeCtrl->GetDivisions()[i]);
	}
	for (i=0; i<pCircleRangeCtrl->GetNumSections(); ++i)
	{
		CSectionItem* pItem = dynamic_cast<CSectionItem*>(pCircleRangeCtrl->GetClientObject(i));
		if (pItem && pItem->m_Section)
			Hybrid.AssignSection(i, *pItem->m_Section);
	}

	m_Section = Hybrid;
}

void CSectionSelect::OnRotate(wxCommandEvent& event)
{
	RefreshTextBoxes();
	UpdateSectionFromDialog();
}

void CSectionSelect::OnScale(wxCommandEvent& event)
{
	RefreshTextBoxes();
	UpdateSectionFromDialog();
}

void CSectionSelect::OnTextChanged(wxCommandEvent &event)
{
	if (m_bFinishedInit)
		UpdateSectionFromDialog();
}

void CSectionSelect::OnSectionTypeChanged(wxChoicebookEvent &event)
{
	if (m_bFinishedInit)
		UpdateSectionFromDialog();
}

void CSectionSelect::OnDivisionChanged(wxCircleRangeCtrlEvent& event)
{
	UpdateSectionFromDialog();
}

void CSectionSelect::OnSectionClicked(wxCircleRangeCtrlEvent& event)
{
	int iSection = event.GetSection();
/*	wxString MessageText;
	MessageText << iSection;
	wxMessageBox(MessageText, wxT("Section clicked"), wxOK | wxICON_INFORMATION);*/
	wxCircleRangeCtrl* pCircleRangeCtrl = wxDynamicCast(event.GetEventObject(), wxCircleRangeCtrl);
	if (pCircleRangeCtrl)
	{
		CSectionSelect Dialog;
		CSectionItem* pItem = dynamic_cast<CSectionItem*>(pCircleRangeCtrl->GetClientObject(iSection));
		if (pItem && pItem->m_Section)
			Dialog.LoadSettings(*pItem->m_Section);
		if (Dialog.ShowModal() == wxID_OK)
		{
			if (pItem)
				pItem->m_Section = Dialog.GetSection();
			else
				pCircleRangeCtrl->SetClientObject(iSection, new CSectionItem(Dialog.GetSection()));
		}
	}
	UpdateSectionFromDialog();
}


void CSectionSelect::RefreshTextBoxes()
{
	if (XRCCTRL(*this, "Rotate", wxCheckBox)->GetValue())
		XRCCTRL(*this, "Angle", wxTextCtrl)->Enable();
	else
		XRCCTRL(*this, "Angle", wxTextCtrl)->Disable();

	if (XRCCTRL(*this, "Scale", wxCheckBox)->GetValue())
	{
		XRCCTRL(*this, "XScale", wxTextCtrl)->Enable();
		XRCCTRL(*this, "YScale", wxTextCtrl)->Enable();
	}
	else
	{
		XRCCTRL(*this, "XScale", wxTextCtrl)->Disable();
		XRCCTRL(*this, "YScale", wxTextCtrl)->Disable();
	}
}

void CSectionSelect::LoadSettings(const CSection &Section)
{
	wxChoicebook* pChoicebook = XRCCTRL(*this, "SectionType", wxChoicebook);
	string Type = Section.GetType();
	if (Type == "CSectionEllipse")
	{
		CSectionEllipse &Ellipse = (CSectionEllipse&)Section;
		m_Width.clear();
		m_Height.clear();
		m_Width << Ellipse.GetWidth();
		m_Height << Ellipse.GetHeight();
		pChoicebook->SetSelection(0);
	}
	else if (Type == "CSectionLenticular")
	{
		CSectionLenticular &Lenticular = (CSectionLenticular&)Section;
		m_Width.clear();
		m_Height.clear();
		m_Distortion.clear();
		m_Width << Lenticular.GetWidth();
		m_Height << Lenticular.GetHeight();
		m_Distortion << Lenticular.GetDistortion();
		pChoicebook->SetSelection(1);
	}
	else if (Type == "CSectionPowerEllipse")
	{
		CSectionPowerEllipse &PowerEllipse = (CSectionPowerEllipse&)Section;
		m_Width.clear();
		m_Height.clear();
		m_Power.clear();
		m_XOffset.clear();
		m_Width << PowerEllipse.GetWidth();
		m_Height << PowerEllipse.GetHeight();
		m_Power << PowerEllipse.GetPower();
		m_XOffset << PowerEllipse.GetXOffset();
		pChoicebook->SetSelection(2);
	}
	else if (Type == "CSectionHybrid")
	{
		CSectionHybrid &Hybrid = (CSectionHybrid&)Section;
		// TODO: Load settings into control widget
		wxCircleRangeCtrl* pCircleRangeCtrl = XRCCTRL(*this, "HybridControl", wxCircleRangeCtrl);
		int i;
		for (i=0; i < Hybrid.GetNumDivisions(); ++i)
		{
			pCircleRangeCtrl->AddDivision(Hybrid.GetDivision(i));
		}
		for (i=0; i < Hybrid.GetNumSections(); ++i)
		{
			pCircleRangeCtrl->SetClientObject(i, new CSectionItem(Hybrid.GetSection(i)));
		}
		pChoicebook->SetSelection(3);
	}
	if (Type == "CSectionRectangle")
	{
		CSectionRectangle &Rectangle = (CSectionRectangle&)Section;
		m_Width.clear();
		m_Height.clear();
		m_Width << Rectangle.GetWidth();
		m_Height << Rectangle.GetHeight();
		pChoicebook->SetSelection(4);
	}
	else if (Type == "CSectionRotated")
	{
		CSectionRotated &Rotated = (CSectionRotated&)Section;
		LoadSettings(Rotated.GetSection());
		double dAngle = Rotated.GetAngle()*(180.0/PI);
		m_Angle.clear();
		m_Angle << dAngle;
		m_bRotate = true;
	}
	else if (Type == "CSectionScaled")
	{
		CSectionScaled &Scaled = (CSectionScaled&)Section;
		LoadSettings(Scaled.GetSection());
		XY Scale = Scaled.GetScale();
		m_XScale.clear();
		m_YScale.clear();
		m_XScale << Scale.x;
		m_YScale << Scale.y;
		m_bScale = true;
	}
}

const CSection &CSectionSelect::GetSection()
{
	return *m_Section;
}

void CSectionSelect::UpdateSection(CObjectContainer<CSection> &Section)
{
	CSectionSelect Dialog;
	if (Section)
		Dialog.LoadSettings(*Section);
	if (Dialog.ShowModal() == wxID_OK)
	{
		Section = Dialog.GetSection();
	}
}

void CSectionSelect::DrawSectionPreview()
{
	wxPanel* pPanel = XRCCTRL(*this, "PreviewPanel", wxPanel);
	wxClientDC dc(pPanel);
//	if (IsDoubleBuffered())
//	{
		DrawSectionPreview(dc);
//	}
//	else
//	{
//		wxBufferedDC BufferedDC(&dc, pPanel->GetClientSize());
//		DrawSectionPreview(BufferedDC);
//	}
}

void CSectionSelect::DrawSectionPreview(wxDC &dc)
{
//	wxBrush blackBrush(wxBLACK);
//	dc.SetBackground(*wxRED_BRUSH);
//	CSectionEllipse Ellipse(2, 1);
	dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE)));
	dc.Clear();
	if (!m_Section)
		return;
	vector<XY> Points = m_Section->GetPoints(100);
	int iNumPoints = (int)Points.size();
	wxPoint* pPointsArray = new wxPoint [iNumPoints];
	wxSize WindowSize = dc.GetSize();
	XY SectionSize;
	int i;
	for (i = 0; i<iNumPoints; ++i)
	{
		if (abs(Points[i].x) > SectionSize.x)
			SectionSize.x = abs(Points[i].x);
		if (abs(Points[i].y) > SectionSize.y)
			SectionSize.y = abs(Points[i].y);
	}

	double dScale = WindowSize.x/SectionSize.x;
	if (WindowSize.y/SectionSize.y < dScale)
		dScale = WindowSize.y/SectionSize.y;

	dScale *= 0.49;

	for (i = 0; i<iNumPoints; ++i)
	{
		pPointsArray[i] = wxPoint(int(Points[i].x*dScale+WindowSize.x*0.5), int(-Points[i].y*dScale+WindowSize.y*0.5));
	}
	dc.DrawPolygon(iNumPoints, pPointsArray);
	delete [] pPointsArray;
//	dc.CrossHair(10, 10);
}

void CPreviewWindowPainter::OnPaint(wxPaintEvent& event)
{
//	wxAutoBufferedPaintDC dc(dynamic_cast<wxWindow*>(event.GetEventObject()));
	wxPaintDC dc(dynamic_cast<wxWindow*>(event.GetEventObject()));

	m_SectionSelect.DrawSectionPreview(dc);
}









