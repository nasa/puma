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
#include "CompleteDialog.h"

CCompleteDialog::CCompleteDialog(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
: wxDialog(parent, id, wxT(""), pos, size, wxSTAY_ON_TOP | wxNO_BORDER)
, m_pListBox(NULL)
, m_iThumbSize(4)
, m_bVisible(false)
{
	m_pListBox = new wxListBox(this, wxID_ANY);
	wxFont ConsoleFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	m_pListBox->SetFont(ConsoleFont);
}

void CCompleteDialog::SetCompleteOptions(const vector<string> &Strings)
{
	wxArrayString ArrayString;
	int i;
	for (i=0; i<(int)Strings.size(); ++i)
	{
		ArrayString.Add(ConvertString(Strings[i]));
	}
	m_pListBox->Set(ArrayString);
	if (m_pListBox->GetCount() > 0)
		m_pListBox->SetSelection(0);
	m_bVisible = true;
}

void CCompleteDialog::MoveDown()
{
	int iSelection = m_pListBox->GetSelection();
	int iCount = m_pListBox->GetCount();
	if (iSelection != wxNOT_FOUND && iSelection < iCount-1)
		m_pListBox->SetSelection(iSelection+1);
}

void CCompleteDialog::MoveUp()
{
	int iSelection = m_pListBox->GetSelection();
	if (iSelection != wxNOT_FOUND && iSelection > 0)
		m_pListBox->SetSelection(iSelection-1);
}

void CCompleteDialog::MovePageDown()
{
//	m_pListBox->PageDown();
	int i;
	for (i=0; i<m_iThumbSize; ++i)
	{
		MoveDown();
	}
}

void CCompleteDialog::MovePageUp()
{
//	m_pListBox->PageUp();
	int i;
	for (i=0; i<m_iThumbSize; ++i)
	{
		MoveUp();
	}
}

wxString CCompleteDialog::GetSelectedText()
{
	return m_pListBox->GetStringSelection();
}

void CCompleteDialog::FakeHide()
{
	m_bVisible = false;
	SetSize(wxRect(0, 0, 0, 0));
}




