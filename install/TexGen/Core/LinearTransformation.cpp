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
#include "LinearTransformation.h"

using namespace TexGen;

CLinearTransformation::CLinearTransformation(void)
{
	InitialiseIdentity(3);
}

CLinearTransformation::CLinearTransformation(const CMatrix &Matrix)
{
	if (Matrix.GetWidth() == 3 && Matrix.GetHeight() == 3)
	{
		*((CMatrix*)this) = Matrix;
	}
	else
	{
		TGERROR("Tried to initialise a linear transformation with a matrix of size: " << Matrix.GetWidth() << "x" << Matrix.GetHeight());
		InitialiseIdentity(3);
	}
}

CLinearTransformation::~CLinearTransformation(void)
{
}

void CLinearTransformation::ResetTransformation()
{
	Identity();
}

void CLinearTransformation::MultiplyTransformation(const CMatrix &Deformation)
{
	*this = Deformation * *this;
}

void CLinearTransformation::AddScale(double dXScale, double dYScale, double dZScale)
{
	CMatrix Deform;
	Deform.InitialiseIdentity(3);
	Deform(0, 0) = dXScale;
	Deform(1, 1) = dYScale;
	Deform(2, 2) = dZScale;
	MultiplyTransformation(Deform);
}

/*
void CLinearTransformation::AddShearX(double dYbydX, double dZbydX)
{
	CMatrix Deform;
	Deform.InitialiseIdentity(3);
	Deform(1, 0) = dYbydX;
	Deform(2, 0) = dZbydX;
	AddDeformation(Deform);
}

void CLinearTransformation::AddShearY(double dXbydY, double dZbydY)
{
	CMatrix Deform;
	Deform.InitialiseIdentity(3);
	Deform(0, 1) = dXbydY;
	Deform(2, 1) = dZbydY;
	AddDeformation(Deform);
}

void CLinearTransformation::AddShearZ(double dXbydZ, double dYbydZ)
{
	CMatrix Deform;
	Deform.InitialiseIdentity(3);
	Deform(0, 2) = dXbydZ;
	Deform(1, 2) = dYbydZ;
	AddDeformation(Deform);
}
*/
void CLinearTransformation::Rotate(WXYZ Rotation)
{
	MultiplyTransformation(ConvertRotation(Rotation));
}










