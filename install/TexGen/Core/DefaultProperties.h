/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2011 Louise Brown

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
	#define YARN_EX			200000  //MPa
	#define YARN_EY			10000	//MPa	
	#define YARN_EZ			10000	//MPa
	#define YARN_GXY		5000	//MPa	
	#define YARN_GXZ		5000	//MPa
	#define YARN_GYZ		5000	//MPa
	#define YARN_ALPHAX		-0.2e-06	// /K
	#define YARN_ALPHAY		3e-06		// /K
	#define YARN_ALPHAZ		3e-06		// /K
	#define YARN_POISSONX	0.3	
	#define YARN_POISSONY	0.4
	#define YARN_POISSONZ	0.4	

	#define	MATRIX_E		3000	//MPa
	#define MATRIX_G		3000	//MPa
	#define MATRIX_ALPHA	6.5e-06	// /K
	#define	MATRIX_POISSON	0.2
};  // namespace TexGen