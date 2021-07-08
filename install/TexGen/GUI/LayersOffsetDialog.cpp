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
#include "LayersOffsetDialog.h"
#include "WindowIDs.h"
#include "TexGenApp.h"
#include "../LatinHypercube/latin_random.hpp"

BEGIN_EVENT_TABLE(CLayersOffsetDialog, wxDialog)
	//EVT_INIT_DIALOG(CTextileLayersDialog::OnInit)
	EVT_BUTTON(wxID_OK, CLayersOffsetDialog::OnOK)
	EVT_RADIOBOX(XRCID("LayerOptions"), CLayersOffsetDialog::OnSelectLayerOption)
	//EVT_BUTTON(XRCID("AddLayerButton"), CTextileLayersDialog::OnAddLayer)
	
END_EVENT_TABLE()

CLayersOffsetDialog::CLayersOffsetDialog()
{
	BuildDialog();
}

CLayersOffsetDialog::~CLayersOffsetDialog(void)
{
}

void CLayersOffsetDialog::BuildDialog()
{
	if (wxXmlResource::Get()->LoadDialog(this, ((CTexGenApp*)wxTheApp)->GetMainFrame(), wxT("LayerOffsets")))
	{
		m_pGrid = XRCCTRL(*this, "OffsetsGrid", wxGrid);
		m_pGrid->CreateGrid(1, 2);

		m_pGrid->SetGridLineColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW));
		m_pGrid->SetDefaultCellBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));

		m_pGrid->SetColLabelSize(20);
		m_pGrid->SetColLabelValue(0, wxT("X"));
		m_pGrid->SetColLabelValue(1, wxT("Y"));
		m_pGrid->SetColFormatFloat(0);
		m_pGrid->SetColFormatFloat(1);
		m_pGrid->EnableDragColSize(false);
		m_pGrid->EnableDragGridSize(false);
		m_pGrid->EnableDragRowSize(false);
		SetInitialSize(wxSize(300, 300));

		m_OptionsRadio = XRCCTRL(*this, "LayerOptions", wxRadioBox );
		m_OptionsRadio->SetSelection( CONSTANT );
		m_XOffset = XRCCTRL(*this, "XOffset", wxTextCtrl );
		m_YOffset = XRCCTRL(*this, "YOffset", wxTextCtrl );
		m_XOffset->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
		m_YOffset->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
		m_XOffset->SetValue( wxT("0.0"));
		m_YOffset->SetValue( wxT("0.0"));
		m_pGrid->Disable();
	}
}

void CLayersOffsetDialog::OnOK(wxCommandEvent& event)
{
	if (!Validate())
		return;
	if (!TransferDataFromWindow())
		return;
	m_pGrid->SaveEditControlValue();
	
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

void CLayersOffsetDialog::LoadSettings( const vector<XY>& Offsets, XY& DomainSize )
{
	int iNumLayers = (int)Offsets.size();
	
	m_pGrid->AppendRows(iNumLayers-1);

	for (int i=0; i<(int)Offsets.size(); ++i)
	//for (int i = iNumLayers-1; i >= 0; --i )
	{
		m_pGrid->SetRowLabelValue( i, ConvertString( stringify(iNumLayers-i)) );
		for (int j=0; j<2; ++j)
		{
			wxString Val;
			Val << Offsets[i][j];
			m_pGrid->SetCellValue(iNumLayers-1-i, j, Val);
		}
	}
	m_DomainSize = DomainSize;
}

void CLayersOffsetDialog::OnSelectLayerOption(wxCommandEvent& event)
{
	switch( m_OptionsRadio->GetSelection() )
	{
	case CONSTANT:
		m_pGrid->Disable();
		m_XOffset->Enable();
		m_YOffset->Enable();
		break;
	case RANDOM:
		GetRandomOffsets();
		break;
	case EDIT:
		m_pGrid->Enable();
		m_XOffset->Disable();
		m_YOffset->Disable();
		break;
	}
}

int CLayersOffsetDialog::GetOption()
{
	return m_OptionsRadio->GetSelection();
}

void CLayersOffsetDialog::GetEditOffsets( vector<XY>& Offsets )
{
	XY Offset;
	Offsets.clear();
	int iNumLayers = m_pGrid->GetNumberRows();
	for ( int i = iNumLayers-1; i >= 0; --i )
	{
		wxString X = m_pGrid->GetCellValue(i, 0);
		wxString Y = m_pGrid->GetCellValue(i, 1);
		X.ToDouble(&Offset.x);
		Y.ToDouble(&Offset.y);
		
		Offsets.push_back(Offset);
	}
}

void CLayersOffsetDialog::GetConstantOffset( XY& Offset )
{
	wxString x = m_XOffset->GetValue();
	wxString y = m_YOffset->GetValue();
	x.ToDouble(&Offset.x); 
	y.ToDouble(&Offset.y);
}

void CLayersOffsetDialog::GetRandomOffsets()
{
	int iNumLayers = m_pGrid->GetNumberRows();
	int iSeed = get_seed();
	double* Offsets;
	Offsets = new double[2*iNumLayers];
	latin_random( 2,iNumLayers, &iSeed, Offsets);
	for ( int i = 0; i < iNumLayers; ++i )
	{
		wxString x( ConvertString( stringify(Offsets[i]*m_DomainSize.x) ) );
		m_pGrid->SetCellValue(i,0, x );
		wxString y( ConvertString( stringify(Offsets[i+iNumLayers]*m_DomainSize.y) ) );
		m_pGrid->SetCellValue(i, 1, y);
	}
	delete Offsets;
}
