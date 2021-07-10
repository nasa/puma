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

#pragma once

class CDomainPlanesDialog : public wxDialog
{
public:
	CDomainPlanesDialog(wxWindow* parent, wxWindowID id = wxID_ANY);
	~CDomainPlanesDialog(void);

	string GetCreateDomainCommand();
	CDomainPlanes CreateDomain();
	void LoadSettings(const CDomainPlanes &DomainPlanes);
	bool CreateBox();

protected:
	void BuildDialog();
	void OnButton(wxCommandEvent& event);
	void OnItemSelected(wxListEvent& event);

	wxListView* m_pListView;
	wxString m_X;
	wxString m_Y;
	wxString m_Z;
	wxString m_D;

	DECLARE_EVENT_TABLE()
};
