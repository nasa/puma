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
#include "Logger.h"
#include "TexGen.h"

using namespace TexGen;
CLogIndent::CLogIndent()
{
	TGLOGINCREASEINDENT();
}

CLogIndent::~CLogIndent()
{
	TGLOGDECREASEINDENT();
}

CLogger::CLogger(void)
: m_iIndent(0)
{
}

CLogger::~CLogger(void)
{
}

/// Function to report a modelling error in texgen, to be used in conjunction with TGERROR() macro
/// so that the filename and linenumbers are filled in automatically
void CLoggerScreen::TexGenError(std::string FileName, int iLineNumber, std::string Message)
{
	std::cerr << FileName << "(" << iLineNumber << ") : " << Message << std::endl;
}

/// Function to report log messages from texgen, to be used in conjunction with TGLOG() macro
/// so that the filename and linenumbers are filled in automatically
void CLoggerScreen::TexGenLog(std::string FileName, int iLineNumber, std::string Message)
{
	int i;
	for (i=0; i<m_iIndent; ++i)
		std::cout << "  ";
	std::cout << Message << std::endl;
}

namespace TexGen
{
	CLogger &GetLogger()
	{
		return TEXGEN.GetLogger();
	}
}









