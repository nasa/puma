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

#include "../Core/PrecompiledHeaders.h"
#include "../Renderer/PrecompiledHeaders.h"
#include "../Core/TexGen.h"

using namespace TexGen;

class CTextileFactory
{
public:
	CTextileFactory();
	~CTextileFactory();

	CTextileWeave2D SatinWeave();
	CTextileWeave3D Weave3D8x4();
	/**
	Yarn includes
	CYarnSection: Constant, NodeInterp, PositionInterp
	Interpolation: Bezier, Cubic
	Sections: Ellipse, Lenticular
	*/
	CTextile InterpTest();
	/**
	Yarn includes
	CYarnSection: PositionInterp
	Interpolation: CInterpolationBezier
	Sections: CSectionBezier, CSectionEllipse, CSectionLenticular, CSectionPolygon, CSectionPowerEllipse
		CSectionRotated, CSectionScaled, CSectionHybrid
	*/
	CTextile SectionsTest();

	CTextile GetSingleYarn(int iNumMasterNodes, int iResolution);

	// Plain weave with rectangular mesh assigned
	CTextileWeave2D MeshedWeave();

	// Cotton weave with properties set
	CTextileWeave2D CottonWeave();

	// Simple plain weave
	CTextileWeave2D PlainWeave( int iWidth=2, int iHeight=2 );

	// Straight yarns with some properties set
	CTextile StraightYarns();

protected:

};
