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

#pragma once

class CTextileLayersDialog : public wxDialog
{
public:
	CTextileLayersDialog();
	~CTextileLayersDialog(void);

	void GetLayerNames( vector<string>& LayerNames );

protected:
	void BuildDialog();
	void OnOK(wxCommandEvent& event);
	//void OnInit(wxInitDialogEvent& event);

	void OnAddLayer(wxCommandEvent& event);
	void OnRemoveLayer(wxCommandEvent& event);
	void OnMoveLayerUp(wxCommandEvent& event);
	void OnMoveLayerDown(wxCommandEvent& event);

	wxListBox* m_pTextilesListBox;
	wxListBox* m_pLayersListBox;

	DECLARE_EVENT_TABLE()
};