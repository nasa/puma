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
#include "TexGenRenderWindow.h"
#include "PythonConsole.h"
#include "ControlsWindow.h"


class CTexGenMainFrame : public wxFrame
{
public:
	CTexGenMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~CTexGenMainFrame(void);

	void OnInit();

	enum OUTPUT_TYPE
	{
		OUTPUT_PYTHON,
		OUTPUT_TEXGEN,
	};

	void ReceiveOutput(string Text, OUTPUT_TYPE OutputType, bool bError, bool bInteractive);

	CTexGenRenderWindow *GetRenderWindow(string WindowName = "");
	CTexGenRenderWindow *CreateRenderWindow(string WindowName = "");
	bool DeleteRenderWindow(string WindowName = "");
	void TextileCallback(string TextileName, bool bAdded);
	void RendererCallback();
	void ProcessFiles(const wxArrayString& filenames);
	void UpdatePositionText();

	void SendPythonCode(string Code);
//	void RefreshDomain(string DomainName);
	bool RefreshTextile(string TextileName);

	wxPanel* GetOutliner() { return m_pOutliner; }

protected:
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnUserGuide(wxCommandEvent& event);

	// File drop-down menu
	void OnOpen(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnSaveScreenshot(wxCommandEvent& event);
	void OnOpenWiseTex(wxCommandEvent& event);
	void OnOpenWeavePattern(wxCommandEvent& event);
	void OnSaveGrid(wxCommandEvent& event);
	void OnSaveVoxel(wxCommandEvent& event);
	void OnSaveVolumeMesh(wxCommandEvent& event);
	void OnSaveSurfaceMesh(wxCommandEvent& event);
	void OnSaveIGES(wxCommandEvent& event);
	void OnSaveSTEP(wxCommandEvent& event);
	void OnSaveABAQUS(wxCommandEvent& event);
	void OnSaveABAQUSVoxels(wxCommandEvent& event);
	void OnSaveABAQUSSurface(wxCommandEvent& event);
	void OnSaveTetgenMesh(wxCommandEvent& event);
	void OnSaveVTUVoxels(wxCommandEvent& event);

	void OnWindow(wxCommandEvent& event);
	void OnChar(wxKeyEvent& event);
	void OnRendering(wxCommandEvent& event);
	void OnTextiles(wxCommandEvent& event);
	void OnGeometrySolve(wxCommandEvent& event);
	void OnModeller(wxCommandEvent& event);
	void OnDomains(wxCommandEvent& event);
	void OnPython(wxCommandEvent& event);
    void OnLogNotebook(wxAuiNotebookEvent& event);
    void OnViewerNotebookPageChanged(wxAuiNotebookEvent& event);
    void OnViewerNotebookClose(wxAuiNotebookEvent& event);
	void UpdateWindowChecks();
	void OnPaneButton(wxAuiManagerEvent& event);

	void OnPosition(wxCommandEvent& event);
	void OnSnapSize(wxCommandEvent& event);

	void OnTools(wxCommandEvent& event);
	void OnOptions(wxCommandEvent& event);

	void OnPeriodicBoundaries(wxCommandEvent& event);

	string GetTextileSelection();
	void UpdateRenderingPage();
	void UpdateModellingPage();
	void UpdateOutliner();
	string GetUserSelectedColor();

	wxAuiManager m_Manager;
	wxAuiNotebook *m_pViewerNotebook;
	wxAuiNotebook *m_pLogNotebook;
	CPythonConsole *m_pPythonConsole;
    wxTextCtrl *m_pPythonOutput;
    wxTextCtrl *m_pTexGenOutput;
	CPythonWrapper m_PythonWrapper;
	CControlsWindow *m_pControls;
	wxPanel* m_pOutliner;

	map<string, CTexGenRenderWindow*> m_RenderWindows;

	ofstream m_ScriptRecordFile;

	void LaunchSurvey();

	DECLARE_EVENT_TABLE()
};

class CNoteBookHandler : public wxEvtHandler
{
public:
	CNoteBookHandler(CTexGenMainFrame &MainFrame) : m_MainFrame(MainFrame) {}

protected:
    void OnSetFocus(wxFocusEvent &event);

	CTexGenMainFrame &m_MainFrame;

	DECLARE_EVENT_TABLE()
};

class COutlineHandler : public wxEvtHandler
{
public:
	COutlineHandler(CTexGenMainFrame &MainFrame) : m_MainFrame(MainFrame) {}

protected:
	void OnSelectionChanged(wxTreeEvent& event);
	void OnDeleteSelected(wxCommandEvent& event);
	void OnInsertNode(wxCommandEvent& event);
	void OnDuplicateYarn(wxCommandEvent& event);

	CTexGenMainFrame &m_MainFrame;

	DECLARE_EVENT_TABLE()
};

class CAbaqusVoxelInput : public wxDialog
{
public:
	CAbaqusVoxelInput( wxWindow* parent );

private:
	void OnOffsetUpdate(wxUpdateUIEvent& event);

	DECLARE_EVENT_TABLE()
};

class CVolumeMeshOptions : public wxDialog
{
public:
	CVolumeMeshOptions( wxWindow* parent );

private:
	void OnPeriodicBoundariesUpdate(wxUpdateUIEvent& event);

	DECLARE_EVENT_TABLE()
};

class CSurfaceMeshOptions : public wxDialog
{
public:
	CSurfaceMeshOptions(wxWindow* parent);

private:
	void OnFillYarnEndsUpdate(wxUpdateUIEvent& event);
	void OnSeedSizeUpdate(wxUpdateUIEvent& event);

	DECLARE_EVENT_TABLE()
};

class CSurveyDialog : public wxDialog
{
public:
	CSurveyDialog( wxWindow* parent );
	void SetFilename( string Filename ){m_Filename = Filename;}

private:
	void OnClickNextTime( wxCommandEvent& event );
	void OnClickNotAgain( wxCommandEvent& event );
	void OnClickTakeSurvey( wxHyperlinkEvent& event);

	string m_Filename;

	DECLARE_EVENT_TABLE()
};

class CTetgenOptions : public wxDialog
{
public:
	CTetgenOptions(wxWindow* parent);

private:
	void OnResolutionUpdate(wxUpdateUIEvent& event);

	DECLARE_EVENT_TABLE()
};











