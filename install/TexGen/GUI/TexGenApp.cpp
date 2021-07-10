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
#include "TexGenMainFrame.h"
#include "TexGenApp.h"
#include "LoggerGUI.h"

//#ifdef _MSC_VER
//#include <crtdbg.h>
//#endif

IMPLEMENT_APP(CTexGenApp)

/*
static PyObject* ReceivePythonOutput(PyObject *self, PyObject *args)
{
	const char *szOutput, *szType;
	if (!PyArg_ParseTuple(args, "ss", &szOutput, &szType))
		return NULL;

	string Type = szType;

	if (wxTheApp)
	{
		CTexGenMainFrame *pMainFrame = ((CTexGenApp*)wxTheApp)->GetMainFrame();
		if (pMainFrame)
		{
			if (Type == "script_stdout" || Type == "script_stderr")
				pMainFrame->AddPythonScriptText(szOutput);
			else if (Type == "interactive_stdout" || Type == "interactive_stderr")
				pMainFrame->AddPythonInteractiveText(szOutput);
		}
	}

	return Py_None;
}

static PyMethodDef EmbMethods[] =
{
    {"SendOutput", ReceivePythonOutput, METH_VARARGS, "Send output to C++ function."},
    {NULL, NULL, 0, NULL}
};
*/

void TextileCallback(string TextileName, bool bAdded)
{
	if (wxTheApp)
	{
		CTexGenMainFrame *pMainFrame = ((CTexGenApp*)wxTheApp)->GetMainFrame();
		if (pMainFrame)
		{
			pMainFrame->TextileCallback(TextileName, bAdded);
		}
	}
}

void RendererCallback()
{
	if (wxTheApp)
	{
		CTexGenMainFrame *pMainFrame = ((CTexGenApp*)wxTheApp)->GetMainFrame();
		if (pMainFrame)
		{
			pMainFrame->RendererCallback();
		}
	}	
}

extern "C" void init_Embedded(void);

CTexGenApp::CTexGenApp()
: m_pMainFrame(NULL)
{
//#ifdef _MSC_VER
//	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//#endif

	Py_Initialize();

	CTexGen::GetInstance().SetMessages( true, CLoggerGUI() );
	CTexGen::GetInstance().SetTextileCallback(TextileCallback);

	// Register the swig embedded module with python
	init_Embedded();

	// Hook into the standard output of Python
//	Py_InitModule("emb", EmbMethods);
	PyRun_SimpleString("import _Embedded");
	PyRun_SimpleString("class OutputCatcher:\n"
						"    def __init__(self, type):\n"
						"        self.data = type\n"
						"    def write(self, text):\n"
						"        _Embedded.ReceivePythonOutput(text, self.data)\n");	// This code causes program to crash on Py_Finalize() (only in Python 2.3 it seems)
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.insert(0, '.\\Python\\libxtra')");
	PyRun_SimpleString("sys.path.insert(0, '.\\Python\\libstd')");
	// Redirect output to the OutputCatcher python class
	PyRun_SimpleString("sys.stdout = OutputCatcher('script_stdout')");
	PyRun_SimpleString("sys.stderr = OutputCatcher('script_stderr')");
//	wxString SetPath = "sys.path.append('" + wxGetCwd() + "')";
//	PyRun_SimpleString(SetPath.c_str());
}

CTexGenApp::~CTexGenApp()
{
	// Restore output to original state
	PyRun_SimpleString("sys.stdout = sys.__stdout__");
	PyRun_SimpleString("sys.stderr = sys.__stderr__");

	Py_Finalize();
}

bool CTexGenApp::OnInit()
{
	SetAppName(wxT("TexGen"));

	wxConfigBase::Set(wxConfigBase::Create());

	wxXmlResource::Get()->InitAllHandlers();
	wxXmlResource::Get()->Load(wxStandardPaths::Get().GetDataDir() + wxT("/TexGen.xrc"));

	m_pMainFrame = new CTexGenMainFrame(wxT("TexGen"), wxDefaultPosition, wxSize(500,450));
	m_pMainFrame->Show(TRUE);
	m_pMainFrame->OnInit();

	SetTopWindow(m_pMainFrame);


	return wxApp::OnInit();
}

int CTexGenApp::OnExit()
{
	m_pMainFrame = NULL;

	return wxApp::OnExit();
}

void CTexGenApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	parser.AddParam(wxT("Filename(s) of TexGen file(s) or Python script(s)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL|wxCMD_LINE_PARAM_MULTIPLE);
	wxApp::OnInitCmdLine(parser);
}

bool CTexGenApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	int i;
	wxArrayString filenames;
	for (i = 0; i < (int)parser.GetParamCount(); i++)
	{
		filenames.Add(parser.GetParam(i));
	}
	if (m_pMainFrame)
	{
		m_pMainFrame->ProcessFiles(filenames);
	}
	return wxApp::OnCmdLineParsed(parser);
}

