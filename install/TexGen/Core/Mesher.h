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

#include <iterator>
#include "Mesh.h"
#include "BasicVolumes.h"
#include "Materials.h"

namespace TexGen
{ 
	using namespace std;
	
//	class CYarn;
	class CTextile;
	class CGeometrySolver;
	class CPeriodicBoundaries;
	class CMaterials;

	enum FACE
	{
		FACE_A,
		FACE_B,
		FACE_C,
		FACE_D,
		FACE_E,
		FACE_F
	};

	class CLASS_DECLSPEC CMesherBase
	{
	public:
		virtual ~CMesherBase(void) {}

		virtual bool CreateMesh(CTextile &Textile) = 0;
		virtual bool CreateMesh(string TextileName);

	protected:
	};

	/// Class for meshing fabric unit cells
	class CLASS_DECLSPEC CMesher : public CMesherBase, public CBasicVolumes
	{
	friend class CGeometrySolver;
	public:
		CMesher(int iBoundaryConditions = NO_BOUNDARY_CONDITIONS );
		~CMesher(void);

		bool CreateMesh(CTextile &Textile);
		// Necessary in order for SWIG to wrap it properly
		bool CreateMesh(string TextileName) { return CMesherBase::CreateMesh(TextileName); }

		void SetMergeTolerance(double dMergeTolerance) { m_dLayerMergeTolerance = dMergeTolerance; }
		double GetMergeTolerance() { return m_dLayerMergeTolerance; }

		void SetHybrid(bool bHybrid) { m_bHybrid = bHybrid; }
		bool GetHybrid() { return m_bHybrid; }

		void SetQuadratic(bool bQuadratic) { m_bQuadratic = bQuadratic; }
		bool GetQuadratic() { return m_bQuadratic; }

		void SetProjectMidSideNodes(bool bProjectMidSideNodes) { m_bProjectMidSideNodes = bProjectMidSideNodes; }
		bool GetProjectMidSideNodes() { return m_bProjectMidSideNodes; }

		void SaveVolumeMeshToVTK(string Filename);
		void SaveVolumeMeshToABAQUS(string Filename, string TextileName);
		void SaveVolumeMeshToABAQUS(string Filename, CTextile& Textile );

		const CMesh &GetMesh() { return m_VolumeMesh; }
		const list<MESHER_ELEMENT_DATA> *GetElementData(CMesh::ELEMENT_TYPE ElementType);

	protected:
		struct RAISED_NODE
		{
			int iIndex;  // Mesh index
			double dZ;
			vector<int> YarnBoundaryIndices;
			bool bMerged;
			bool operator < (const RAISED_NODE &right) const
			{
				return dZ < right.dZ;
			}
			RAISED_NODE() : iIndex(0), dZ(0), bMerged(false) {}
		};

		struct PROJECTED_NODE
		{
			vector<RAISED_NODE> RaisedNodes;
			XYZ Position;
		};

		struct TRIANGLE
		{
			int i[3];
		};

		typedef pair<int, int> NODE_PAIR;
		typedef vector<NODE_PAIR> NODE_PAIR_SET;
		typedef vector<NODE_PAIR_SET> NODE_PAIR_SETS;

		void CreateVolumeMesh(CTextile &Textile);
		void RaiseNodes(int iIndex);
		void SubdivideNodes(int iIndex);
		double GetBestSeed(int iIndex);
		bool ShouldConnect(vector<RAISED_NODE> &Column1, vector<RAISED_NODE> &Column2, int h1, int h2);
		bool ViolatesEdgeConstraint(const set<pair<int, int> > &EdgeConstraints1, const set<pair<int, int> > &EdgeConstraints2, int h, int h1, int h2);
		void MeshColumn(TRIANGLE Triangle, int iRegion);
		bool SplitColumn(PROJECTED_NODE &Node, vector<int> &YarnIndices, vector<vector<RAISED_NODE> > &Column);
//		int MixedMeshColumn(vector<RAISED_NODE> Columns[3], set<pair<int, int> > EdgeConstraints[3]);
		int TetMeshColumn(vector<RAISED_NODE> Columns[3], set<pair<int, int> > EdgeConstraints[3]);
		int MeshDifficultRegion(vector<RAISED_NODE> Columns[3], int Limits[6], set<pair<int, int> > EdgeConstraints[3]);
		void FillYarnTangentsData();
		void BuildEdgeConstraints(vector<RAISED_NODE> Columns[3], set<pair<int, int> > EdgeConstraints[3]);
		void AddElement(CMesh::ELEMENT_TYPE Type, const vector<int> &Indices);
		void AddEdgeConstraint(int i1, int i2);
		void BuildMidSideNodes(vector<RAISED_NODE> Columns[3], int iYarnIndex);
		void BuildMidSideNode(int iNodeIndex1, int iNodeIndex2, int iYarnIndex, bool bTop);
		XYZ GetMidSideNode(int iNodeIndex1, int iNodeIndex2);
		void ConvertMeshToQuadratic();

		// Given a pair of indices, find the matching pair on the opposite boundary
		bool GetPairIndices(int iIndex1, int iIndex2, NODE_PAIR &MatchPair);
		// Find matching nodes on opposite side of domain (for top surface edges).  If corner will return all four nodes in Match
		void GetEdgePairIndices(const NODE_PAIR_SETS &NodePairSets, int iIndex, set<int> &Match);
		// If bSwapY is true sort by y value, otherwise sort by x
		void SortPairs( NODE_PAIR_SET &NodePairs, bool bSwapY );
		// Create Face, Edge and Corner node sets for periodic boundary conditions
		void CreateNodeSets( NODE_PAIR_SETS &EdgeNodePairSets, set<int> &CornerIndex, const vector<XYZ> &Repeats );
		// Save node sets to CPeriodicBoundaries class
		bool SaveNodeSets();
		// Add extra edge nodes for quadratic elements to node sets
		void AddQuadraticNodesToSets();
		// Create node sets for each face containing all nodes on face, including edges and corners
		void SetupFaceSets( vector< set<int> >& FaceSets );
		// Find all face sets which contain iIndex
		vector<int> FindFaceSets( vector< set<int> >& FaceSets, int iIndex );
		// Add quad node to appropriate face,edge or corner set
		void AddQuadNodeToSet( int i, int j, vector<int>& FaceSeti, vector<int>& FaceSetj, vector<int> &Indices );
		// Add quad node to correct face set
		void AddQuadNodeToFace( int i, int j, int iFace, vector<int> &Indices );
		// Returns index for quad edge node between i and j
		int GetQuadNodeToAdd( int i, int j );
		// Add quad node to correct edge set
		void AddQuadNodeToEdge( int i, int j, int iEdge, vector<int> &Indices );
		// Finds the edge between face1 and face2
		int GetEdge( int iFace1, int iFace2 );
		// Remove duplicate nodes from node sets
		void RemoveDuplicateNodes();
		void RemoveDuplicateFaceNodes( vector<int>& FaceSet );

		// Each node in the volume mesh is assigned an integer z order
		// the z order provides a consistant method of determining if one
		// node is higher than the other. This method is consistant in that
		// it will always give the same results even if two heights are equal.
//		vector<int> m_TriangleNeighbors;
		CMesh m_VolumeMesh;

		// This set represents the edges that have already been created by adding elements
		// when adding new elements it is necessary to avoid creating edges which cross
		// over existing edges so that a conforming mesh can be acheived. The pair of ints
		// represents the two node indices, where the first in the pair is always the lowest.
		set<pair<int, int> > m_EdgeConstraints;

		// This map represents the mid-side node locations when mid-side nodes are activated.
		// Similarly to the edge constraints, the two ints represent the indices of the nodes.
		// and the XYZ represents the location of the mid-side node.
		map<pair<int, int>, XYZ> m_MidSideNodeLocations;

		// Sets of nodes on opposite faces of the domain
		NODE_PAIR_SETS m_NodePairSets;

		vector<PROJECTED_NODE> m_ProjectedNodes;
		bool m_bHybrid;
		bool m_bQuadratic;
		bool m_bProjectMidSideNodes;
		double m_dLayerMergeTolerance;
		// Contains data about each element of the elements in m_VolumeMesh
		list<MESHER_ELEMENT_DATA> m_ElementData[CMesh::NUM_ELEMENT_TYPES];

		CPeriodicBoundaries* m_PeriodicBoundaries;
		int m_iBoundaryConditions;

		// Node sets for periodic boundary conditions
		vector<int> m_FaceA;
		vector<int> m_FaceB;
		vector<int> m_FaceC;
		vector<int> m_FaceD;
		vector<int> m_FaceE;
		vector<int> m_FaceF;
		vector< vector<int> > m_Edges;
		vector<int> m_Vertices;

		// Class for export of material properties
		CTextileMaterials* m_Materials;
	};

};	// namespace TexGen













