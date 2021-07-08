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

#include "TexGenMainFrame.h"
#include "TexGenApp.h"
/// Logger used to print all log and error messages to the screen
class CLoggerGUI : public CLogger
{
public:
	CLoggerGUI(bool bInteractive = false)
	: m_bInteractive(bInteractive)
	{

	}

	CLogger *Copy() const { return new CLoggerGUI(*this); }
	void TexGenError(std::string FileName, int iLineNumber, std::string Message)
	{
		if (wxTheApp)
		{
			CTexGenMainFrame *pMainFrame = ((CTexGenApp*)wxTheApp)->GetMainFrame();
			if (pMainFrame)
			{
				string ProcessedMessage;
				int i;
				for (i=0; i<m_iIndent; ++i)
					ProcessedMessage += "  ";
				ProcessedMessage += Message + "\n";
				pMainFrame->ReceiveOutput(ProcessedMessage, CTexGenMainFrame::OUTPUT_TEXGEN, true, m_bInteractive);
			}
		}
	}

	void TexGenLog(std::string FileName, int iLineNumber, std::string Message)
	{
		if (wxTheApp)
		{
			CTexGenMainFrame *pMainFrame = ((CTexGenApp*)wxTheApp)->GetMainFrame();
			if (pMainFrame)
			{
				string ProcessedMessage;
				int i;
				for (i=0; i<m_iIndent; ++i)
					ProcessedMessage += "  ";
				ProcessedMessage += Message + "\n";
				pMainFrame->ReceiveOutput(ProcessedMessage, CTexGenMainFrame::OUTPUT_TEXGEN, false, m_bInteractive);
			}
		}
	}

protected:
	bool m_bInteractive;
};


