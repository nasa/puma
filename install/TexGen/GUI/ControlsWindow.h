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

#include "../Renderer/PrecompiledHeaders.h"
#include "../Renderer/TexGenRenderer.h"

class CModeller;

class CControlsWindow : public wxChoicebook
{
public:
	CControlsWindow(wxMenuBar* pMenuBar, wxWindow* parent, wxWindowID id);
	~CControlsWindow(void);

	void UpdateModellingPage(const CModeller *pModeller);
	void UpdateRenderingPage(const CTexGenRenderer *pRenderer);
	void UpdatePythonPage(bool bRecording);
	void UpdatePositionText(const CModeller *pModeller);
	void UpdateSnapSize(const CModeller *pModeller);

	bool m_bUpdatingPositionText;

protected:
	void BuildControls();
	void BuildTextilesPage();
	void BuildModellerPage();
	void BuildRenderingPage();
	void BuildDomainsPage();
	void BuildPythonPage();
	void ResizePages();
	void BuildOptionsPage();
	void BuildToolsPage();

	void UpdateRenderWidget(int iID, const CTexGenRenderer *pRenderer, CTexGenRenderer::PROP_TYPE Type);
	void UpdateCheckWidget(int iID, bool bChecked);

	wxMenuBar* m_pMenuBar;

	wxString m_PositionX;
	wxString m_PositionY;
	wxString m_PositionZ;
	wxString m_SnapSize;
};
