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
#include "TexGenRenderWindow.h"

BEGIN_EVENT_TABLE(CTexGenRenderWindow, wxVTKRenderWindowInteractor)
  EVT_LEFT_DOWN   (CTexGenRenderWindow::OnButtonDown)
  EVT_MIDDLE_DOWN (CTexGenRenderWindow::OnButtonDown)
  EVT_RIGHT_DOWN  (CTexGenRenderWindow::OnButtonDown)
  EVT_LEFT_UP     (CTexGenRenderWindow::OnButtonUp)
  EVT_MIDDLE_UP   (CTexGenRenderWindow::OnButtonUp)
  EVT_RIGHT_UP    (CTexGenRenderWindow::OnButtonUp)
  EVT_KEY_DOWN    (CTexGenRenderWindow::OnKeyDown)
  EVT_KEY_UP      (CTexGenRenderWindow::OnKeyUp)
END_EVENT_TABLE()

CTexGenRenderWindow::CTexGenRenderWindow(void)
: m_pRenderer(NULL)
, m_pModeller(NULL)
{
	Initialise();
}

CTexGenRenderWindow::CTexGenRenderWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
: m_pRenderer(NULL)
, wxVTKRenderWindowInteractor(parent, id, pos, size, wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE)
, m_pModeller(NULL)
{
	Initialise();
}

CTexGenRenderWindow::~CTexGenRenderWindow(void)
{
	CleanUp();
}

void CTexGenRenderWindow::CleanUp()
{
	if (m_pRenderer)
	{
		delete m_pRenderer;
		m_pRenderer = NULL;
	}
	if (m_pModeller)
	{
		delete m_pModeller;
		m_pModeller = NULL;
	}
}


// Declare function
void RendererCallback();

void CTexGenRenderWindow::Initialise()
{
	SetUseCaptureMouse(true);

	m_pRenderer = new CTexGenRenderer(this);
	vtkRenderWindow *pRenderWindow = GetRenderWindow();
	pRenderWindow->AddRenderer(m_pRenderer->GetRenderer());

/*	vtkInteractorStyleTerrain *pInteractionStyle = vtkInteractorStyleTerrain::New();
	SetInteractorStyle(pInteractionStyle);
	pInteractionStyle->Delete();*/

	m_pRenderer->SetCallback(RendererCallback);
}

void CTexGenRenderWindow::AttachModeller(string TextileName)
{
	if (m_pModeller)
		return;

	m_pModeller = new CModeller(m_pRenderer, TextileName);
}

void CTexGenRenderWindow::OnButtonDown(wxMouseEvent &event)
{
	SetFocus();
	if (m_pModeller)
		m_pModeller->OnButtonDown(event);
	else
		event.Skip();
}

void CTexGenRenderWindow::OnButtonUp(wxMouseEvent &event)
{
	if (m_pModeller)
		m_pModeller->OnButtonUp(event);
	else
		event.Skip();
}

void CTexGenRenderWindow::OnKeyDown(wxKeyEvent &event)
{
	if (m_pModeller)
		m_pModeller->OnKeyDown(event);
	else
		event.Skip();
}

void CTexGenRenderWindow::OnKeyUp(wxKeyEvent &event)
{
	if (m_pModeller)
		m_pModeller->OnKeyUp(event);
	else
		event.Skip();
}














