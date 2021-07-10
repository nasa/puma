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
#include "DomainPlanesDialog.h"
#include "WindowIDs.h"

BEGIN_EVENT_TABLE(CDomainPlanesDialog, wxDialog)
	EVT_BUTTON(ID_AddPlane, CDomainPlanesDialog::OnButton)
	EVT_BUTTON(ID_ReplacePlane, CDomainPlanesDialog::OnButton)
	EVT_BUTTON(ID_DeletePlane, CDomainPlanesDialog::OnButton)
	EVT_BUTTON(ID_BoxDomain, CDomainPlanesDialog::OnButton)
	EVT_LIST_ITEM_SELECTED(ID_PlaneListView, CDomainPlanesDialog::OnItemSelected)
END_EVENT_TABLE()

CDomainPlanesDialog::CDomainPlanesDialog(wxWindow* parent, wxWindowID id)
: wxDialog(parent, id, wxString(wxT("Domain Editor")))
, m_X(wxT("0"))
, m_Y(wxT("0"))
, m_Z(wxT("0"))
, m_D(wxT("0"))
{
	BuildDialog();
}

CDomainPlanesDialog::~CDomainPlanesDialog(void)
{
}

CDomainPlanes CDomainPlanesDialog::CreateDomain()
{
	CDomainPlanes Domain;
	PLANE Plane;
	wxListItem Item;
	int iItemID, iLength = m_pListView->GetItemCount();
	for (iItemID=0; iItemID<iLength; ++iItemID)
	{
        Item.SetId(iItemID);
        Item.SetMask(wxLIST_MASK_TEXT);
        Item.SetColumn(0);
		if (m_pListView->GetItem(Item))
			Item.GetText().ToDouble(&Plane.Normal.x);
        Item.SetColumn(1);
		if (m_pListView->GetItem(Item))
			Item.GetText().ToDouble(&Plane.Normal.y);
        Item.SetColumn(2);
		if (m_pListView->GetItem(Item))
			Item.GetText().ToDouble(&Plane.Normal.z);
        Item.SetColumn(3);
		if (m_pListView->GetItem(Item))
			Item.GetText().ToDouble(&Plane.d);
		Domain.AddPlane(Plane);
	}
	return Domain;
}

string CDomainPlanesDialog::GetCreateDomainCommand()
{
	stringstream StringStream;
	StringStream << "domain = CDomainPlanes()" << endl;
	wxListItem Item;
	int iItemID, iLength = m_pListView->GetItemCount();
	double dItem;
	for (iItemID=0; iItemID<iLength; ++iItemID)
	{
		StringStream << "domain.AddPlane(PLANE(XYZ(";
        Item.SetId(iItemID);
        Item.SetMask(wxLIST_MASK_TEXT);
        Item.SetColumn(0);
		if (m_pListView->GetItem(Item))
		{
			Item.GetText().ToDouble( &dItem );
			StringStream << dItem << ", ";
		}
        Item.SetColumn(1);
		if (m_pListView->GetItem(Item))
		{
			Item.GetText().ToDouble( &dItem );
			StringStream << dItem << ", ";
		}
        Item.SetColumn(2);
		if (m_pListView->GetItem(Item))
		{
			Item.GetText().ToDouble( &dItem );
			StringStream << dItem << "), ";
		}
        Item.SetColumn(3);
		if (m_pListView->GetItem(Item))
		{
			Item.GetText().ToDouble( &dItem );
			StringStream << dItem << "))" << endl;
		}
	}
	return StringStream.str();
}

void CDomainPlanesDialog::BuildDialog()
{
	wxSizerFlags SizerFlags;
	SizerFlags.Expand();
	SizerFlags.Border();
	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	m_pListView = new wxListView(this, ID_PlaneListView, wxDefaultPosition, wxSize(200, 200), wxLC_REPORT /*| wxLC_SINGLE_SEL*/ | wxLC_HRULES | wxLC_VRULES);
	m_pListView->InsertColumn(0, wxT("x"));
	m_pListView->InsertColumn(1, wxT("y"));
	m_pListView->InsertColumn(2, wxT("z"));
	m_pListView->InsertColumn(3, wxT("d"));
/*	pListView->InsertItem(0, "");
	pListView->InsertItem(1, "");
	pListView->InsertItem(2, "");
	pListView->InsertItem(3, "");
	pListView->SetItem(0, 0, "testing");
	pListView->SetItem(1, 1, "testing2");*/
	pMainSizer->Add(m_pListView, SizerFlags);
	wxSizer *pSubSizer;
	SizerFlags.Center();
	pSubSizer = new wxBoxSizer(wxHORIZONTAL);
	pSubSizer->Add(new wxStaticText(this, wxID_ANY, wxT("x:")), SizerFlags);
	pSubSizer->Add(new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_X)), SizerFlags);
	pSubSizer->Add(new wxStaticText(this, wxID_ANY, wxT("y:")), SizerFlags);
	pSubSizer->Add(new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_Y)), SizerFlags);
	pSubSizer->Add(new wxStaticText(this, wxID_ANY, wxT("z:")), SizerFlags);
	pSubSizer->Add(new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_Z)), SizerFlags);
	pSubSizer->Add(new wxStaticText(this, wxID_ANY, wxT("d:")), SizerFlags);
	pSubSizer->Add(new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_D)), SizerFlags);
	pMainSizer->Add(pSubSizer, wxSizerFlags().Center());
	pSubSizer = new wxBoxSizer(wxHORIZONTAL);
	pSubSizer->Add(new wxButton(this, ID_BoxDomain, wxT("Create Box")), SizerFlags);
	pSubSizer->Add(new wxButton(this, ID_AddPlane, wxT("Add Plane")), SizerFlags);
	pSubSizer->Add(new wxButton(this, ID_ReplacePlane, wxT("Replace Plane")), SizerFlags);
	pSubSizer->Add(new wxButton(this, ID_DeletePlane, wxT("Delete Plane(s)")), SizerFlags);
	pMainSizer->Add(pSubSizer, wxSizerFlags().Center());
	pSubSizer = new wxBoxSizer(wxHORIZONTAL);
	pSubSizer->Add(new wxButton(this, wxID_OK, wxT("Ok")), SizerFlags);
	pSubSizer->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")), SizerFlags);
	pMainSizer->Add(pSubSizer, wxSizerFlags().Center());
	SetSizerAndFit(pMainSizer);
}

void CDomainPlanesDialog::OnButton(wxCommandEvent& event)
{
	switch (event.GetId())
	{
	case ID_AddPlane:
		{
			TransferDataFromWindow();
			long iItemID = m_pListView->GetItemCount();
			m_pListView->InsertItem(iItemID, wxT(""));
			m_pListView->SetItem(iItemID, 0, m_X);
			m_pListView->SetItem(iItemID, 1, m_Y);
			m_pListView->SetItem(iItemID, 2, m_Z);
			m_pListView->SetItem(iItemID, 3, m_D);
		}
		break;
	case ID_ReplacePlane:
		{
			TransferDataFromWindow();
			long iItemID = m_pListView->GetFirstSelected();
			if (iItemID != -1)
			{
				m_pListView->SetItem(iItemID, 0, m_X);
				m_pListView->SetItem(iItemID, 1, m_Y);
				m_pListView->SetItem(iItemID, 2, m_Z);
				m_pListView->SetItem(iItemID, 3, m_D);
			}
/*			while ((iItemID = m_pListView->GetFirstSelected()) != -1)
			{
				m_pListView->DeleteItem(iItemID);
			}*/
		}
		break;
	case ID_DeletePlane:
		{
			long iItemID;
			while ((iItemID = m_pListView->GetFirstSelected()) != -1)
			{
				m_pListView->DeleteItem(iItemID);
			}
		}
		break;
	case ID_BoxDomain:
		{
			CreateBox();
		}
		break;
	}
}

bool CDomainPlanesDialog::CreateBox()
{
	CDomainPlanes Domain = CreateDomain();
	pair<XYZ, XYZ> Corners = Domain.GetMesh().GetAABB();
	wxString X1, Y1, Z1, X2, Y2, Z2;
	X1 << Corners.first.x;
	Y1 << Corners.first.y;
	Z1 << Corners.first.z;
	X2 << Corners.second.x;
	Y2 << Corners.second.y;
	Z2 << Corners.second.z;

	wxDialog BoxDomain;
	if (wxXmlResource::Get()->LoadDialog(&BoxDomain, this, wxT("BoxDomain")))
	{
		XRCCTRL(BoxDomain, "X1", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &X1));
		XRCCTRL(BoxDomain, "Y1", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &Y1));
		XRCCTRL(BoxDomain, "Z1", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &Z1));
		XRCCTRL(BoxDomain, "X2", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &X2));
		XRCCTRL(BoxDomain, "Y2", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &Y2));
		XRCCTRL(BoxDomain, "Z2", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &Z2));

		if (BoxDomain.ShowModal() == wxID_OK)
		{
			XYZ P1,P2;
			XYZ MinP, MaxP;
			X1.ToDouble(&P1.x);
			Y1.ToDouble(&P1.y);
			Z1.ToDouble(&P1.z);
			X2.ToDouble(&P2.x);
			Y2.ToDouble(&P2.y);
			Z2.ToDouble(&P2.z);
			MinP = Min(P1,P2);
			MaxP = Max(P1,P2);
			LoadSettings(CDomainPlanes(MinP, MaxP));
			return true;
		}
	}
	return false;
}

void CDomainPlanesDialog::OnItemSelected(wxListEvent& event)
{
	long iItemID = event.GetIndex();
	if (iItemID != -1)
	{
		wxListItem Item;
        Item.SetId(iItemID);
        Item.SetMask(wxLIST_MASK_TEXT);
        Item.SetColumn(0);
		if (m_pListView->GetItem(Item))
			m_X = Item.GetText();
        Item.SetColumn(1);
		if (m_pListView->GetItem(Item))
			m_Y = Item.GetText();
        Item.SetColumn(2);
		if (m_pListView->GetItem(Item))
			m_Z = Item.GetText();
        Item.SetColumn(3);
		if (m_pListView->GetItem(Item))
			m_D = Item.GetText();
	}
	TransferDataToWindow();
}

void CDomainPlanesDialog::LoadSettings(const CDomainPlanes &DomainPlanes)
{
	m_pListView->DeleteAllItems();
	int i;
	vector<PLANE>::const_iterator itPlane;
	for (itPlane = DomainPlanes.GetPlanes().begin(), i=0; itPlane != DomainPlanes.GetPlanes().end(); ++itPlane, ++i)
	{
		m_pListView->InsertItem(i, wxT(""));
		m_pListView->SetItem(i, 0, wxString() << itPlane->Normal.x);
		m_pListView->SetItem(i, 1, wxString() << itPlane->Normal.y);
		m_pListView->SetItem(i, 2, wxString() << itPlane->Normal.z);
		m_pListView->SetItem(i, 3, wxString() << itPlane->d);
	}
}








