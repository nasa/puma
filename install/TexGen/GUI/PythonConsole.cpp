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
#include "PythonConsole.h"


BEGIN_EVENT_TABLE(CPythonConsole, wxTextCtrl)
	EVT_CHAR(CPythonConsole::OnChar)
	EVT_TEXT(wxID_ANY, CPythonConsole::OnText)
	EVT_SET_FOCUS(CPythonConsole::OnSetFocus)
	EVT_KILL_FOCUS(CPythonConsole::OnKillFocus)
END_EVENT_TABLE()

CPythonConsole::CPythonConsole(CPythonWrapper &PythonWrapper, wxWindow *parent, wxWindowID id, const wxString &value,
							   const wxPoint &pos, const wxSize &size, int style)
: wxTextCtrl(parent, id, value, pos, size, style)
, m_iInputPosition(GetLastPosition())
, m_iCompletePosition(GetLastPosition())
, m_iCompleteState(0)
, m_PythonWrapper(PythonWrapper)
, m_CompleteDialog(this, wxID_ANY, wxPoint(0, 0), wxSize(0, 0))
, m_bUseCompleteWindow(false)
{
	if (m_bUseCompleteWindow)
		m_CompleteDialog.Show(true);
	SetInsertionPointEnd();
}

void CPythonConsole::UpdateAutoCompleteWindow()
{
	if (!m_bUseCompleteWindow)
	{
		HideAutoCompleteWindow();
		return;
	}

	wxWindow *pFocusWindow = FindFocus();
	if (pFocusWindow != this && !m_CompleteDialog.IsActive())
	{
		HideAutoCompleteWindow();
	}
	else if (pFocusWindow == this)
	{
		long iPos = GetInsertionPoint();
		if (iPos >= m_iInputPosition)
		{
			m_iCompletePosition = iPos;
			wxString Text = GetRange(m_iInputPosition, m_iCompletePosition);
			Text = Text.AfterLast(' ');
			if (!Text.IsEmpty())
			{
				ShowAutoCompleteWindow(m_PythonWrapper.GetCompleteOptions(ConvertString(Text)));
			}
			else
			{
				HideAutoCompleteWindow();
			}
		}
	}
}

void CPythonConsole::OnSetFocus(wxFocusEvent& event)
{
//	UpdateAutoCompleteWindow();
}

void CPythonConsole::OnKillFocus(wxFocusEvent& event)
{
	UpdateAutoCompleteWindow();
}

void CPythonConsole::OnText(wxCommandEvent& event)
{
	UpdateAutoCompleteWindow();
}

void CPythonConsole::OnChar(wxKeyEvent& event)
{
	wxString key;
	long keycode = event.GetKeyCode();
	if (keycode == WXK_RETURN)
	{
		long iPos = GetInsertionPoint();
		if (iPos >= m_iInputPosition)
		{
			wxString Text = GetRange(m_iInputPosition, GetLastPosition());
			AppendText(wxT("\n"));
			if (m_PythonWrapper.SendCommand(ConvertString(Text)))
				AppendText(wxT("... "));
			else
				AppendText(wxT(">>> "));
			m_iInputPosition = GetLastPosition();
			m_iCompleteState = 0;
			SetInsertionPoint(m_iInputPosition);
			SetFocus();		// Get the focus back in case another window stole it (e.g. on CreateRenderWindow())
		}
		return;
	}
	else
	{
		long iPos = GetInsertionPoint();
		switch (keycode)
		{
		case WXK_TAB:
			if (iPos >= m_iInputPosition)
			{
				if (m_PythonWrapper.CompleterInstanceCreated())
				{
					if (m_bUseCompleteWindow && m_CompleteDialog.IsShown())
					{
						wxString Text = GetRange(m_iInputPosition, m_iCompletePosition);
						int iStringStart = Text.Find(' ', true);		// if the text is not found -1 is returned which is rather convenient since it will cancel out the +1 on the next line
						int iVarStart = m_iInputPosition + iStringStart + 1;	// get the start of the variable to autocomplete which is the point one after the space
						Replace(iVarStart, m_iCompletePosition, m_CompleteDialog.GetSelectedText());	// Replace the variable with the one selected in the complete box
					}
					else if (!m_bUseCompleteWindow)
					{
						if (m_iCompleteState == 0)
							m_iCompletePosition = iPos;
						else
							Remove(m_iCompletePosition, iPos);
						wxString Text = GetRange(m_iInputPosition, m_iCompletePosition);
						Text = Text.AfterLast(' ');
						string Completion = m_PythonWrapper.Complete(ConvertString(Text), m_iCompleteState);
						if (Completion.empty())
						{
							m_iCompleteState = 0;
						}
						else
						{
							++m_iCompleteState;
							WriteText(ConvertString(Completion));
						}
					}
				}
				return;
			}
			break;
		case WXK_HOME:
			if (iPos >= m_iInputPosition-4)
			{
				if (event.ShiftDown())
					SetSelection(iPos, m_iInputPosition);
				else
					SetInsertionPoint(m_iInputPosition);
				return;
			}
			break;
		case WXK_UP:
			if (m_CompleteDialog.Visible())
			{
				m_CompleteDialog.MoveUp();
				return;
			}
			else
			{
				if (iPos >= m_iInputPosition)
				{
					Remove(m_iInputPosition, GetLastPosition());
					AppendText(ConvertString(m_PythonWrapper.GetPrevHistoryCommand()));
					SetInsertionPointEnd();
					return;
				}
			}
			break;
		case WXK_DOWN:
			if (m_CompleteDialog.Visible())
			{
				m_CompleteDialog.MoveDown();
				return;
			}
			else
			{
				if (iPos >= m_iInputPosition)
				{
					Remove(m_iInputPosition, GetLastPosition());
					AppendText(ConvertString(m_PythonWrapper.GetNextHistoryCommand()));
					SetInsertionPointEnd();
					return;
				}
			}
			break;
		case WXK_PAGEUP:
//		case WXK_PRIOR:
			if (m_CompleteDialog.Visible())
			{
				m_CompleteDialog.MovePageUp();
				return;
			}
		case WXK_PAGEDOWN:
//		case WXK_NEXT:
			if (m_CompleteDialog.Visible())
			{
				m_CompleteDialog.MovePageDown();
				return;
			}
		case WXK_END:
		case WXK_LEFT:
		case WXK_RIGHT:
		case 3:		// This is CTRL-C ON WINDOWS, NOT SURE IF THIS WILL BE CROSS PLATFORM OR NOT...
			break;
		default:
			long iFrom, iTo;
			GetSelection(&iFrom, &iTo);
			if (iFrom == iTo)
			{
				if (iPos < m_iInputPosition)
					return;
				if (keycode == WXK_BACK && iPos == m_iInputPosition)
					return;
			}
			else
			{
				if (iFrom < m_iInputPosition)
					return;
			}
		}
	}
	event.Skip();
	m_iCompleteState = 0;
}

void CPythonConsole::ShowAutoCompleteWindow(const vector<string> &Completions)
{
	if (Completions.empty())
	{
		HideAutoCompleteWindow();
		return;
	}
	string s;
	wxRect Rect;
	wxSize Size(300, 100);
	wxPoint Position = ClientToScreen(wxPoint(0, 0));
	Position.y -= Size.y;
	Rect.SetPosition(Position);
	Rect.SetSize(Size);
	m_CompleteDialog.SetSize(Rect);
	m_CompleteDialog.SetCompleteOptions(Completions);
}

void CPythonConsole::HideAutoCompleteWindow()
{
	m_CompleteDialog.FakeHide();
}








