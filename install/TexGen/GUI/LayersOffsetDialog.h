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

#include "../LatinHypercube/latin_random.hpp"

#pragma once

enum
{
	CONSTANT,
	RANDOM,
	EDIT
};

class CLayersOffsetDialog : public wxDialog
{
public:
	CLayersOffsetDialog();
	~CLayersOffsetDialog(void);

	void LoadSettings( const vector<XY>& Offsets, XY& DomainSize );
	int GetOption();
	void GetEditOffsets( vector<XY>& Offsets );
	void GetConstantOffset( XY& Offset );

protected:
	void BuildDialog();
	void OnOK(wxCommandEvent& event);
	//void OnInit(wxInitDialogEvent& event);
	void OnSelectLayerOption(wxCommandEvent& event);
	void GetRandomOffsets();

	wxGrid* m_pGrid;
	wxRadioBox* m_OptionsRadio;
	wxTextCtrl* m_XOffset;
	wxTextCtrl* m_YOffset;

	XY m_DomainSize;

	DECLARE_EVENT_TABLE()
};