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
#include "ReedData.h"

using namespace TexGen;

CReedData::CReedData()
{
}

CReedData::~CReedData()
{
}

double CReedData::GetClosestReedThickness(double dThickness)
{
	vector<double> ReedSizes(ReedThickness, ReedThickness + NUM_REED_SIZES);
	vector<double>::iterator after = lower_bound( ReedSizes.begin(), ReedSizes.end(), dThickness );
	
	if ( after == ReedSizes.begin() )  
		return *after;

	if ( after == ReedSizes.end() )
		return( *(after - 1 ) );

	return (*after - dThickness) < ( dThickness - *(after-1) ) ? *after : *(after - 1);
}