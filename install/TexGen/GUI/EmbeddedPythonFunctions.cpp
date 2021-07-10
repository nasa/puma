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
#include "EmbeddedPythonFunctions.h"
#include "TexGenApp.h"

CTexGenRenderer *CreateRenderWindow(string WindowName)
{
	if (wxTheApp)
	{
		CTexGenMainFrame *pMainFrame = ((CTexGenApp*)wxTheApp)->GetMainFrame();
		if (pMainFrame)
		{
			CTexGenRenderWindow* pRenderWindow = pMainFrame->CreateRenderWindow(WindowName);
			if (pRenderWindow)
				return pRenderWindow->GetRenderer();
		}
	}
	return NULL;
}

CTexGenRenderer *GetRenderWindow(string WindowName)
{
	if (wxTheApp)
	{
		CTexGenMainFrame *pMainFrame = ((CTexGenApp*)wxTheApp)->GetMainFrame();
		if (pMainFrame)
		{
			CTexGenRenderWindow* pRenderWindow = pMainFrame->GetRenderWindow(WindowName);
			if (pRenderWindow)
				return pRenderWindow->GetRenderer();
		}
	}
	return NULL;
}

bool DeleteRenderWindow(string WindowName)
{
	if (wxTheApp)
	{
		CTexGenMainFrame *pMainFrame = ((CTexGenApp*)wxTheApp)->GetMainFrame();
		if (pMainFrame)
		{
			return pMainFrame->DeleteRenderWindow(WindowName);
		}
	}
	return false;
}

bool Refresh(string WindowName)
{
	if (wxTheApp)
	{
		CTexGenMainFrame *pMainFrame = ((CTexGenApp*)wxTheApp)->GetMainFrame();
		if (pMainFrame)
		{
			return pMainFrame->RefreshTextile(WindowName);
		}
	}
	return false;
}

void ReceivePythonOutput(string Output, string Type)
{
	if (wxTheApp)
	{
		CTexGenMainFrame *pMainFrame = ((CTexGenApp*)wxTheApp)->GetMainFrame();
		if (pMainFrame)
		{
			if (Type == "script_stdout")
				pMainFrame->ReceiveOutput(Output, CTexGenMainFrame::OUTPUT_PYTHON, false, false);
			else if (Type == "script_stderr")
				pMainFrame->ReceiveOutput(Output, CTexGenMainFrame::OUTPUT_PYTHON, true, false);
			else if (Type == "interactive_stdout")
				pMainFrame->ReceiveOutput(Output, CTexGenMainFrame::OUTPUT_PYTHON, false, true);
			else if (Type == "interactive_stderr")
				pMainFrame->ReceiveOutput(Output, CTexGenMainFrame::OUTPUT_PYTHON, true, true);
		}
	}
}

#include "Embedded_wrap.cxx"
