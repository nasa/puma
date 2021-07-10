/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2018 Louise Brown

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

	/// Class used to generate Abaqus input file for periodic boundary conditions for a textile with rotated domain. 
	class CLASS_DECLSPEC CRotatedPeriodicBoundaries : public CPeriodicBoundaries
	{
	public:
		CRotatedPeriodicBoundaries(void);
		~CRotatedPeriodicBoundaries(void);

		/// Set up the domain size parameters used in the equations
		void SetDomainSize( const CMesh& Mesh );

	private:
		/// Output equations for rotated domain boundary conditions
		void OutputEquations( ostream& Output, int iBoundaryConditions );
		/// Displacement vectors for the three axes
		XYZ m_DispVec[3];
	};
}; // namespace TexGen