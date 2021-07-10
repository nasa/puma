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

%module(directors="1") Renderer

%include <stl.i>

%{
#include "../Renderer/PrecompiledHeaders.h"
#include "../Renderer/TexGenRenderer.h"
using namespace TexGen;
%}

namespace TexGen {}

using namespace std;
using namespace TexGen;

#define CLASS_DECLSPEC

%import "../Core/Properties.h"
%import "../Core/PropertiesTextile.h"
%import "../Core/PropertiesYarn.h"
%import "../Core/Yarn.h"
%import "../Core/Textile.h"
%import "../Core/Domain.h"
%import "../Core/mymath.h"
%warnfilter(401);
%include "../Core/Misc.h"
%warnfilter(+401);

%include "../Renderer/TexGenRenderer.h"

