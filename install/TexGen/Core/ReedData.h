/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2014 Louise Brown

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
	using namespace std;

	#define NUM_REED_SIZES		30
	static double ReedThickness[NUM_REED_SIZES] = { 0.1, 0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.2,
										0.22, 0.24, 0.26, 0.28, 0.31, 0.32, 0.34,
										0.36, 0.37, 0.4, 0.45, 0.5, 0.6,
										0.65, 0.7, 0.8,
										1.0, 1.25, 1.6 };

	class CLASS_DECLSPEC CReedData
	{
	public:
		CReedData();
		virtual ~CReedData(void);
		double GetClosestReedThickness( double dThickness );
	protected:
		
	};
};	// namespace TexGen