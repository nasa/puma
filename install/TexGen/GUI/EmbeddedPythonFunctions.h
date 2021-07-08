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

namespace TexGen
{
	class CTexGenRenderer;
}

/// Shortcut function to get an instance of CTexGenRenderer associated with a particular window (if NULL the current)
TexGen::CTexGenRenderer *GetRenderWindow(string WindowName = "");
/// Shortcut function to create a new render window
TexGen::CTexGenRenderer *CreateRenderWindow(string WindowName = "");
/// Shortcut function to delete a new render window
bool DeleteRenderWindow(string WindowName = "");
/// Function to refresh the current render (may be necessary after performing Python commands which modify the Textile)
bool Refresh(string WindowName = "");
/// Function to capture standard output and direct it to the log window
void ReceivePythonOutput(string Output, string Type);












