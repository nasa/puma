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

#include "SectionSelect.h"

class CInterpPositionItem : public wxClientData
{
public:
	CInterpPositionItem(const CSection &Section = CSectionEllipse(1, 1)) : m_dPosition(0), m_Section(Section) {}
	~CInterpPositionItem(void) {;}

	double m_dPosition;
	CObjectContainer<CSection> m_Section;
};

class CInterpNodesItem : public wxClientData
{
public:
	CInterpNodesItem(int iNode, const CSection &Section = CSectionEllipse(1, 1)) : m_iNode(iNode), m_Section(Section) {}
	~CInterpNodesItem(void) {;}

	int m_iNode;
	CObjectContainer<CSection> m_Section;
};

class CYarnSectionSelect : public wxDialog
{
public:
	CYarnSectionSelect();
	~CYarnSectionSelect(void);

	string GetCreateSectionCommand(string YarnSectionVariableName = "yarnsection");
	void SetNumberOfNodes(int iNumNodes);
	void LoadSettings(const CYarnSection &YarnSection);

protected:
	void BuildDialog();
	void OnOK(wxCommandEvent& event);
	void OnConstantSection(wxCommandEvent& event);
	void OnInit(wxInitDialogEvent& event);
	void SetDefaultSection(const CSection &Section);

	void OnInterpPositionInsert(wxCommandEvent& event);
	void OnInterpPositionRemove(wxCommandEvent& event);
	void OnInterpPositionSection(wxCommandEvent& event);
	void OnInterpPositionSlider(wxScrollEvent &event);
	void OnInterpPositionText(wxCommandEvent &event);
	void OnInterpPositionSelectionChanged(wxCommandEvent &event);

	void UpdateInterpPositionListboxText();
	void UpdateInterpPositionText();
	void UpdateInterpPositionSlider();
	void UpdateInterpPositionItemText(int iSelection);


	void OnInterpNodesInsert(wxCommandEvent& event);
	void OnInterpNodesRemove(wxCommandEvent& event);
	void OnInterpNodesSection(wxCommandEvent& event);

	void UpdateInterpNodesListboxText();
	void UpdateInterpNodesItemText(int iSelection);	

	// Returns true if some, but not all, of sections are rectangle sections
	bool CheckRectangleSections( wxListBox* pListBox, string str );
	// Check if any of sections within hybrid are rectangle sections
	bool CheckHybridSection( const CSection& Hybrid );

	wxString m_Position;
	int m_iSliderPosition;

	bool m_bPolar, m_bRamped, m_bConstMesh;

	CObjectContainer<CSection> m_ConstactSection;

	DECLARE_EVENT_TABLE()
};





