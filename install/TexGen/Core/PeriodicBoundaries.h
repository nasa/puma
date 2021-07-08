/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2010 Louise Brown

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
	//class CMaterial;

	using namespace std;

	/// Class used to generate Abaqus output for periodic boundary conditions
	class CLASS_DECLSPEC CPeriodicBoundaries
	{
	public:
		CPeriodicBoundaries(int NumEdges = 12, int NumVertices = 8);
		~CPeriodicBoundaries(void);

		void CreatePeriodicBoundaries( ostream& Output, int iDummyNodeNum, CTextile& Textile, int iBoundarConditions, bool bMatrixOnly );

		virtual void SetDomainSize( const CMesh& Mesh );
		void SetFaceA( vector<int>& A1, vector<int>& A2 );
		void SetFaceB( vector<int>& B1, vector<int>& B2 );
		void SetFaceC( vector<int>& C1, vector<int>& C2 );
		virtual void SetFaceD( vector<int>&D1, vector<int>& D2 ){};
		void SetEdges( vector<int>& Edge );
		void SetVertex( int Vertex );

		virtual void SetOffset( double Offset ){};

		// Functions which create stream and then call protected functions (for use when called from Python)
		/// Output 6 dummy nodes: x = 0, y = 1, z = 2, xy = 3, xz = 4, yz = 5
		void OutputDummyNodeSets( string Filename, int iDummyNodeNum );
		
		/// Output a step including field outputs
		void OutputStep( string Filename, int iBoundaryConditions );

	protected:
		/// Pairs of node sets for opposite faces
		pair< vector<int>, vector<int> > m_FaceA;
		pair< vector<int>, vector<int> > m_FaceB;
		pair< vector<int>, vector<int> > m_FaceC;

		/// Array of vectors containing nodes for edge sets
		vector< vector<int> > m_Edges;
		int m_NumEdges;

		/// Array of corner nodes
		vector<int> m_Vertices;
		int m_NumVertices;

		XYZ m_DomSize;
		/// Output equations for boundary conditions
		virtual void OutputEquations( ostream& Output, int iBoundarConditions );
		/// Output an unsorted node set
		void OutputSets( ostream& Output, vector<int>& Set, string SetName );
		/// Output 6 dummy nodes: x = 0, y = 1, z = 2, xy = 3, xz = 4, yz = 5
		void OutputDummyNodeSets( ostream& Output, int iDummyNodeNum );
		/// Output 6 face node sets
		virtual void OutputFaceSets( ostream& Output );
		/// Output 12 edge node sets
		void OutputEdgeSets( ostream& Output );
		/// Output 8 corner node sets
		void OutputVertexSets(ostream& Output );
		/// Output a step including field outputs
		void OutputStep( ostream& Output, int iBoundaryConditions );
		/// Output 6 load cases
		void OutputLoadCase( ostream& Output, int iCase );
		
	};
}; // namespace TexGen