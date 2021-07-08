/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2016 Louise Brown

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
# include "TestUtilities.h"

bool CompareFiles(string FileName1, string FileName2)
{
	ifstream Input1(FileName1.c_str());
	stringstream StringStream1;
	StringStream1 << Input1.rdbuf();
	ifstream Input2(FileName2.c_str());
	stringstream StringStream2;
	StringStream2 << Input2.rdbuf();
	return (StringStream1.str() == StringStream2.str());
}