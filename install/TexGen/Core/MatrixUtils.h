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

#include "mymath.h"
#include "Matrix.h"

namespace TexGen
{ 
	using namespace std;

	// Maths from http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
	inline CMatrix ConvertRotation(WXYZ Q)
	{
		CMatrix Mat;
		Mat.InitialiseIdentity(3);
		// Diagonal
		Mat(0, 0) = Q.w*Q.w + Q.x*Q.x - Q.y*Q.y - Q.z*Q.z;
		Mat(1, 1) = Q.w*Q.w - Q.x*Q.x + Q.y*Q.y - Q.z*Q.z;
		Mat(2, 2) = Q.w*Q.w - Q.x*Q.x - Q.y*Q.y + Q.z*Q.z;
		// Upper right
		Mat(0, 1) = 2*Q.x*Q.y - 2*Q.w*Q.z;
		Mat(0, 2) = 2*Q.w*Q.y + 2*Q.x*Q.z;
		Mat(1, 2) = 2*Q.y*Q.z - 2*Q.w*Q.x;
		// Lower left
		Mat(1, 0) = 2*Q.w*Q.z + 2*Q.x*Q.y;
		Mat(2, 0) = 2*Q.x*Q.z - 2*Q.w*Q.y;
		Mat(2, 1) = 2*Q.w*Q.x + 2*Q.y*Q.z;
		return Mat;
	}

	inline XYZ operator * (const CMatrix &Transform, const XYZ &Vector)
	{
		CMatrix VecMat;
		VecMat.Initialise(3, 1);
		VecMat(0, 0) = Vector.x;
		VecMat(1, 0) = Vector.y;
		VecMat(2, 0) = Vector.z;
		VecMat = Transform * VecMat;
		return XYZ(VecMat(0, 0), VecMat(1, 0), VecMat(2, 0));		
	}

};	// namespace TexGen










