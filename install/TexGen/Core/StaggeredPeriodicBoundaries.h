/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2013 Louise Brown

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

	/// Class used to generate Abaqus input file for periodic boundary conditions for a textile with staggered offset. Gives reduced unit cell size
	class CLASS_DECLSPEC CStaggeredPeriodicBoundaries : public CPeriodicBoundaries
	{
	public:
		CStaggeredPeriodicBoundaries(int NumEdges = 18, int NumVertices = 12);
		~CStaggeredPeriodicBoundaries(void);

		void SetFaceD( vector<int>& D1, vector<int>& D2 );
		void SetOffset( double Offset ) { m_Offset = Offset; }

	protected:
		void OutputFaceSets( ostream& Output );
		/// Output equations for staggered boundary conditions
		void OutputEquations( ostream& Output, int iBoundaryConditions );

		pair< vector<int>, vector<int> > m_FaceD;

		double m_Offset;
		
	};
}; // namespace TexGen