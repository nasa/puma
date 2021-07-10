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
#include "PythonWrapper.h"
#include "CompleteDialog.h"

class CPythonConsole : public wxTextCtrl
{
public:
    CPythonConsole(CPythonWrapper &PythonWrapper, wxWindow *parent, wxWindowID id, const wxString &value,
					const wxPoint &pos, const wxSize &size, int style = 0);

private:
	void ShowAutoCompleteWindow(const vector<string> &Completions);
	void HideAutoCompleteWindow();
	void UpdateAutoCompleteWindow();
	void OnChar(wxKeyEvent& event);
	void OnText(wxCommandEvent& event);
	void OnSetFocus(wxFocusEvent& event);
	void OnKillFocus(wxFocusEvent& event);

	bool m_bUseCompleteWindow;
	long m_iInputPosition;
	long m_iCompletePosition;
	long m_iCompleteState;
	CPythonWrapper &m_PythonWrapper;
	CCompleteDialog m_CompleteDialog;

	DECLARE_EVENT_TABLE()
};


