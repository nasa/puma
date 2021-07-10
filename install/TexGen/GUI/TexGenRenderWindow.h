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
#include "wxVTKRenderWindowInteractor.h"
#include "../Renderer/PrecompiledHeaders.h"
#include "../Renderer/TexGenRenderer.h"
#include "Modeller.h"

class CTexGenRenderWindow : public wxVTKRenderWindowInteractor
{
public:
	CTexGenRenderWindow(void);
	CTexGenRenderWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize);
	~CTexGenRenderWindow(void);

	void CleanUp();
	void AttachModeller(string TextileName);
    void OnButtonDown(wxMouseEvent &event);
    void OnButtonUp(wxMouseEvent &event);
    void OnKeyDown(wxKeyEvent &event);
	void OnKeyUp(wxKeyEvent &event);

	CTexGenRenderer *GetRenderer() { return m_pRenderer; }
	CModeller *GetModeller() { return m_pModeller; }

protected:
	void Initialise();

	CTexGenRenderer *m_pRenderer;
	CModeller *m_pModeller;

	DECLARE_EVENT_TABLE()
};










