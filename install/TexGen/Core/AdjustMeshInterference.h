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

#include "Mesh.h"
#include "SimulationAbaqus.h"


namespace TexGen
{ 
	using namespace std;

	class CTextile;
	class CMesh;

	/// Class which holds the information related to each intersection point, used for adjusting the mesh
	class CLASS_DECLSPEC CMeshIntersectionData
	{
		public:
			CMeshIntersectionData(void);
			virtual ~CMeshIntersectionData(void);

			void SetIndex( int index ){m_Index = index;}
			void SetDepth( double Depth ){m_Depth = Depth;}
			void SetPoint( XYZ Point ){m_Point = Point;}
			void SetYarn( int Yarn ){m_Yarn = Yarn;}
			void SetBoundaryPairIndex( int index ){m_BoundaryPairIndex = index;}
			void SetStartPoint( XYZ Point){m_StartPoint = Point;}
			void SetInterpIndex( int index ){m_InterpIndex = index;}

			int GetYarn() { return m_Yarn; }
			const vector<ELEMENT_INDICES> &GetElements() const { return m_Elements; }
			double GetDepth() { return m_Depth; }
			XYZ GetPoint() { return m_Point; }
			XYZ GetInterpPoint() { return m_InterpPoint; }
			int GetIndex() { return m_Index; }
			XYZ GetDisplacement() { return m_StartPoint - m_Point; }
			int GetInterpIndex() { return m_InterpIndex; }

			/// Finds the appropriate node in the elements to interpolate towards
			bool FindInterpolationNode( CMesh &YarnMesh );

			/// Given node index, find all elements containing that node
			void FindElements( vector<int> &IndexArray, CMesh::ELEMENT_TYPE ElementType );

			/// Move node by 0.5 * intersection depth towards the interpolation node
			bool MoveNode( CMesh &YarnMesh );

			/// Make final adjustments to interpolation nodes to even out elements
			void AdjustInterpolationNode( CMesh &YarnMesh );

			/// Find surface point in polygon & points on either side
			void FindPolygonPoints( list<int> &Polygons );

		protected:
			
			double  m_Depth;
			XYZ		m_StartPoint;
			XYZ		m_Point;
			XYZ		m_InterpPoint;
			int		m_Yarn;
			int		m_Index;
			int		m_BoundaryPairIndex;
			int		m_InterpIndex;
			bool	m_Wedge;
			vector<ELEMENT_INDICES> m_Elements;	
			int		m_PolygonPoints[3];
	};

	/// Class used to adjust small interferences in the fibre volume mesh
	/** 
	Takes the individual yarn meshes and corrects intersections based on Textile.DetectInterference()
	*/
	class CLASS_DECLSPEC CAdjustMeshInterference
	{
	public:
		CAdjustMeshInterference(void);
		virtual ~CAdjustMeshInterference(void);

		/// Top level function to adjust nodes on all yarns
		/**
		\param Textile The textile being adjusted
		\param YarnMeshes Array of meshes generated for each yarn, to be adjusted
		\param Tolerance Mesh will be adjusted until intersections are within this tolerance
		*/
		bool AdjustMesh( CTextile &Textile, vector<CMesh> &YarnMeshes, double Tolerance );
		vector<XYZ> GetNodeDisplacements( int i ){ return m_NodeDisplacements[i]; }

		/// Create a vector of volume meshes, one for each yarn
		bool CreateVolumeMeshes( CTextile &Textile, vector<CMesh> &YarnMeshes );

		/// Adjust yarns to interpolate between section meshes and change these to reflect intersection adjustments
		void AdjustSectionMeshes(CTextile &Textile, vector<CMesh> &YarnMeshes );

	protected:

		/// Calls Textile.DetectInterference and sets up MeshIntersectionData base on intersection depths
		bool CheckInitialIntersections( CTextile &Textile, vector<CMesh> &YarnMeshes );

		/// Takes the individual yarn meshes and corrects intersections based on Textile.DetectInterference()
		bool AdjustInitialIntersections( vector<CMesh> &YarnMeshes );

		/// Creates mesh for each yarn containing elements with intersection nodes
		void AddIntersectElementsToMesh( const vector<ELEMENT_INDICES> &Elements, int iYarn, CMesh &YarnMesh );

		/// Iteratively refines intersections until within tolerance
		bool AdjustIntersections( vector<CMesh> &YarnMeshes );

		void SetNodeDisplacements( int iNumYarns, vector<CMesh> &YarnMeshes );

		/// Final adjustment of interpolation nodes to even out mesh
		void AdjustInterpolationNodes( vector<CMesh> &YarnMeshes );

		/// Find the newly created polygon sections in the volume mesh
		int FindMeshPolygonSection( vector<XYZ> &SlaveNodePoints, CMesh &TempMesh, vector<int> &PolygonIndices, vector<XYZ> &Translations, int &OffsetIndex, int &iTrans );

		int CompareSections( vector< vector<XYZ> > &TranslatedSections, vector<XYZ> &PolygonPoints, int &iTrans );

		vector<CMeshIntersectionData> m_Intersections;
		vector<CMesh> m_IntersectionMeshes;
		double m_Tolerance;
		vector< vector<XYZ> > m_NodeDisplacements;
		vector<CMesh> m_TempYarnMeshes;
	};

	
};	// namespace TexGen
