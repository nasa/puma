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

#include "PrecompiledHeaders.h"
#include "TextileLayersDialog.h"
#include "WindowIDs.h"
#include "TexGenApp.h"

BEGIN_EVENT_TABLE(CTextileLayersDialog, wxDialog)
	//EVT_INIT_DIALOG(CTextileLayersDialog::OnInit)
	EVT_BUTTON(wxID_OK, CTextileLayersDialog::OnOK)
	EVT_BUTTON(XRCID("AddLayerButton"), CTextileLayersDialog::OnAddLayer)
	EVT_BUTTON(XRCID("RemoveLayerButton"), CTextileLayersDialog::OnRemoveLayer)
	EVT_BUTTON(XRCID("MoveLayerUpButton"), CTextileLayersDialog::OnMoveLayerUp)
	EVT_BUTTON(XRCID("MoveLayerDownButton"), CTextileLayersDialog::OnMoveLayerDown)
END_EVENT_TABLE()

CTextileLayersDialog::CTextileLayersDialog()
{
	BuildDialog();
}

CTextileLayersDialog::~CTextileLayersDialog(void)
{
}

void CTextileLayersDialog::BuildDialog()
{
	if (wxXmlResource::Get()->LoadDialog(this, ((CTexGenApp*)wxTheApp)->GetMainFrame(), wxT("SelectLayers")))
	{
		m_pTextilesListBox = XRCCTRL(*this, "TextilesListBox", wxListBox);
		m_pLayersListBox = XRCCTRL(*this, "LayersListBox", wxListBox);
		if (m_pTextilesListBox->GetCount() == 0)
		{
			vector<string> Names;
			vector<string>::iterator itNames;
			CTexGen::GetInstance().GetTextileNames( Names );
			for ( itNames = Names.begin(); itNames != Names.end(); ++ itNames )
			{
				m_pTextilesListBox->Append( ConvertString( *itNames ) );
			}
		}
	}
}

void CTextileLayersDialog::OnOK(wxCommandEvent& event)
{
	if (!Validate())
		return;
	if (!TransferDataFromWindow())
		return;
	
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

void CTextileLayersDialog::OnAddLayer(wxCommandEvent& event)
{
	if ( m_pTextilesListBox->IsEmpty() || m_pTextilesListBox->GetSelection() == wxNOT_FOUND )
		return;
	wxString Name = m_pTextilesListBox->GetStringSelection();
	m_pLayersListBox->Append( Name );
}

void CTextileLayersDialog::GetLayerNames( vector<string>& LayerNames )
{
	wxArrayString wxLayerNames = m_pLayersListBox->GetStrings();

	LayerNames.clear();
	int iNum = (int)wxLayerNames.size();

	for ( int i = 0; i < iNum; ++i )
	{
		LayerNames.push_back( ConvertString(wxLayerNames[i]) );
	}
}

void CTextileLayersDialog::OnRemoveLayer(wxCommandEvent& event)
{
	if ( m_pLayersListBox->IsEmpty() )
		return;
	m_pLayersListBox->Delete( m_pLayersListBox->GetSelection() );
}

void CTextileLayersDialog::OnMoveLayerUp(wxCommandEvent& event)
{
	int index = m_pLayersListBox->GetSelection();
	if ( index < 1 )
		return;
	wxString Name = m_pLayersListBox->GetStringSelection();
	m_pLayersListBox->Delete( index );
	m_pLayersListBox->Insert(Name,index-1 );
	m_pLayersListBox->SetSelection( index-1 );
}

void CTextileLayersDialog::OnMoveLayerDown(wxCommandEvent& event)
{
	int index = m_pLayersListBox->GetSelection();
	if ( index == wxNOT_FOUND || index >= (int)m_pLayersListBox->GetCount()-1 )
		return;
	wxString Name = m_pLayersListBox->GetStringSelection();
	m_pLayersListBox->Delete( index );
	m_pLayersListBox->Insert(Name,index+1 );
	m_pLayersListBox->SetSelection( index+1 );
}