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
//#include "../tinyxml/tinyxml.h"
#include "MeshData.h"

//class TiXmlElement;
namespace TexGen
{
	using namespace std;

	/// Defines the nodes and elements of a surface or volume mesh
	/**
	CMesh is a data structure that represents a mesh whith some additional functions to
	perform basic tasks and output to various file formats. As such, direct access to the
	data is provided.
	m_Nodes is a vector of nodes contained within the mesh.
	m_Indices is a list of indices into the nodes.
	The element types supported are listed in the ELEMENT_TYPE enum, this can be easily
	be extended. If this is done the GetNumNodes function must be updated to specify
	the number of nodes the element type uses.

	Sample code to create a triangle mesh with two elements and save to STL is shown below:
	CMesh Mesh;
	Mesh.AddNode(XYZ(0,0,0));
	Mesh.AddNode(XYZ(1,0,0));
	Mesh.AddNode(XYZ(0,1,0));
	Mesh.AddNode(XYZ(1,1,0));
	vector<int> Tri1, Tri2;
	Tri1.push_back(0);
	Tri1.push_back(1);
	Tri1.push_back(2);
	Tri2.push_back(1);
	Tri2.push_back(3);
	Tri2.push_back(2);
	Mesh.AddElement(CMesh::TRI, Tri1);
	Mesh.AddElement(CMesh::TRI, Tri2);
	Mesh.SaveToSTL("sample.stl");
	*/
	class CLASS_DECLSPEC CMesh
	{
	public:
		CMesh(void);
		CMesh(TiXmlElement &Element);
		~CMesh(void);

		/// Each element type is represented by a unique integer value
		enum ELEMENT_TYPE
		{
			TRI,
			QUAD,
			TET,
			PYRAMID,
			WEDGE,
			HEX,
			LINE,
			POLYLINE,
			QUADRATIC_TET,
			POLYGON,
			NUM_ELEMENT_TYPES,		// Note this should always be directly after the last valid element
		};

		/// Get the number of nodes a particular element type contains
		static inline int GetNumNodes(ELEMENT_TYPE Type)
		{
			switch (Type)
			{
				case TRI:
					return 3;
				case QUAD:
					return 4;
				case TET:
					return 4;
				case PYRAMID:
					return 5;
				case WEDGE:
					return 6;
				case HEX:
					return 8;
				case LINE:
					return 2;
				case POLYLINE:
					return 2;
				case QUADRATIC_TET:
					return 10;
				case POLYGON:
					return -1;  // Closed polygon: start node = end node
			}
			return 0;
		}

		/// Used for saving data to XML
		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		//////////////////////////////////////////////
		// Methods to convert between element types //
		//////////////////////////////////////////////

		/// Convert a specific quad element to two triangles
		list<int>::iterator ConvertQuadtoTriangles(list<int>::iterator itQuad);

		/// Convert triangle elements to segments
		void ConvertTrianglestoSegments();

		/// Convert the quad elements to triangles
		void ConvertQuadstoTriangles(bool bQuality = true);

		/// Convert Hex elements to Quad elements representing their surface
		void ConvertHextoQuad();

		/// Convert Wedge elements to Quad and Triangle elements representing their surface
		void ConvertWedgeto2D();

		/// Convert Tet elements to Triangle elements representing their surface
		void ConvertTettoTriangle();

		/// Convert Hexahedral elements to Wedge elements
		void ConvertHextoWedge(bool bQuality = true);

		/// Convert Wedge elements to a combination of Tetrahedral and Pyramid elements
		void ConvertWedgetoTetandPyramid(bool bQuality = true);

		/// Convert Pyramid elements to Tetrahedral elements
		void ConvertPyramidtoTet(bool bQuality = true);

		/// Convert Wedge elements to Tetrahedral elements
		/**
		This is done by converting wedge element to tets and pyramids
		then to tets alone
		*/
		void ConvertWedgetoTet(bool bQuality = true);

		/// Convert Hexahedral elements to Tetrahedral elements
		/**
		This is done by converting hexes to wedge and then to tets
		*/
		void ConvertHextoTet(bool bQuality = true);

		/// Convert Pyramid elements to Quad and Triangle elements representing their surface
		void ConvertPyramidto2D();

		/// Convert all 3D elements to 2D elements representing their surface
		void Convert3Dto2D();

		/// Convert a volume mesh into a surface mesh (interior surfaces are elliminated)
		void ConvertToSurfaceMesh();

		/// Convert all elements to tetrahedrons where possible
		/// and remove those that cannot be converted
		void ConvertToTetMesh();

		/// Convert all elements to triangles where possible
		/// and remove those that cannot be converted
		void ConvertToTriangleMesh();

		/// Convert all elements to segments
		void ConvertToSegmentMesh();

		/// Convert Triangel elements to Quads
		/**
		This requires there to be two common nodes and for the normals 
		of the two elements to be equal
		*/
		void ConvertTriToQuad( double Tolerance = 1e-6 );

		/////////////////////////////
		// Various utility methods //
		/////////////////////////////

		/// Get an axis aligned bounding box for the mesh
		pair<XYZ, XYZ> GetAABB(double dGrowDistance = 0) const;

		/// If any nodes share the same coordinates merge the nodes together and adjust indices accordingly
		/**
		\param Tolerance Nodes within Tolerance distance of each other will be assumed to be at the same position and merged together
		\return Number of nodes merged together
		*/
		int MergeNodes(const double Tolerance = 1e-8);

		/// Remove nodes that are not referenced by any elements
		int RemoveUnreferencedNodes();

		/// Remove all elements except those of given type
		void RemoveAllElementsExcept(ELEMENT_TYPE Type);

		/// Remove elements of given type
		void RemoveElementType( ELEMENT_TYPE Type );

		/// Delete nodes and adjust node indices
		int DeleteNodes(const set<int> &Nodes);

		/// Change all the indices from one number to another
		void ChangeNodeIndices(int iChangeTo, int iChangeFrom);

		/// Change all the indices from one number to another
		/**
		This function is far more efficient than the function where the references are not given.
		References can be obtained with GetNodeElementReferences function which itself is rather
		time consuming. However GetNodeElementReferences need only be called once for multiple calls
		to ChangeNodeIndices. References will be updated reflecting the change in node indices.
		*/
		void ChangeNodeIndices(int iChangeTo, int iChangeFrom, vector<vector<int*> > &References);

		/// Remove triangles that have the same indices but opposite normals
		void RemoveOpposingTriangles();

		/// Remove quads that have the same indices but opposite normals
		void RemoveOpposingQuads();

		/// Remove triangles which have two equal corner indices
		void RemoveDegenerateTriangles();

		// Remove triangles which have the same indices
		void RemoveDuplicateTriangles();

		/// Remove segments which have the same indices
		void RemoveDuplicateSegments();

		/// Remove duplicate elements which have the same indices (leaves one copy of element)
		/**
		\param ElementType Type of elements to be removed
		*/
		void RemoveDuplicateElements(CMesh::ELEMENT_TYPE ElementType);

		/// Get a list of elements which reference each node
		/**
		To be used in conjunction with ChangeNodeIndices function
		*/
		void GetNodeElementReferences(vector<vector<int*> > &References);

		/// Add the nodes of Mesh to this mesh
		/**
		\param Mesh Mesh whose nodes should be added to this mesh
		\param Offset The inserted mesh can be translated by a given factor before instering
		\return The index of the first node added
		*/
		int InsertNodes(const CMesh &Mesh, XYZ Offset = XYZ(0, 0, 0));

		/// Add the contents of Mesh to this mesh
		/**
		\param Mesh Mesh to add to this mesh
		\param Offset The inserted mesh can be translated by a given factor before instering
		*/
		void InsertMesh(const CMesh &Mesh, XYZ Offset = XYZ(0, 0, 0));

		/// Rotate the whole mesh by given quaternion
		void Rotate(WXYZ Rotation, XYZ Origin = XYZ(0,0,0));

		/// Translate whole mesh by given vector
		void Translate(XYZ Vector);

		/// Flip the normals of the mesh for triangles and quads
		void FlipNormals();

		/// Given a list of indices which form a counter-clockwise closed loop and lie
		/// on the same plane, this function will mesh the interior with triangle elements.
		/**
		\param Normal Normal of the plane on which the points lie
		\param ClosedLoopVector Counter-clockwise list of points forming a closed loop
		\param bQuality Determines if the mesh should be of high quality (this will increase processing time)
		*/
		void MeshClosedLoop(const XYZ &Normal, const vector<int> &ClosedLoopVector, bool bQuality = false);

		/// Create a triangular convex hull of the nodes contained within the mesh.
		/**
		All elements will be removed before the hull is created. The incremental convex hull method is implemented,
		it is one of the simplest and more robust techniques however it can be slow for a large number of nodes.
		If this becomes a problem, an alternate method should be implemented.
		This function is only implemented for creating a convex hull of 3d points. A 2d equivalent should
		be fairly easy to implement, however is not necessary at this point.
		*/
		void MeshConvexHull();

		/// Empty mesh nodes and indices
		void Clear();

		/// Copy the mesh to the range given by the lower limit and upper limit with the given repeat vector
		void CopySelfToRange(XYZ Vector, int iLowerLimit, int iUpperLimit);

		/// Output the node coordinates in the mesh each on a new line, components seperated by commas
		/**
		\param Output Output stream to output the nodes to
		\param iStartIndex Index to start numbering the nodes at, if less than zero then no index used
		\param Seperator The string to use to seperate values
		\param bSCIRun true if outputting to SCIRun format.  In all other cases, false
		*/
		int OutputNodes(ostream &Output, int iStartIndex = 1, string Seperator = ", ", bool bSCIRun = false) const;

		/// Output the element indices in the mesh each on a new line, indices seperated by commas
		/**
		\param Output Output stream to output the nodes to
		\param ElementType The type of elements to output
		\param iStartIndex Index to start numbering the elements at, if less than zero then no index used
		\param iIndexOffset This value will be added to each of the zero based node indices forming the element
		\param Seperator The string to use to seperate values
		\param bSCIRun Set to false for all mesh output other than SCIRun
		*/
		int OutputElements(ostream &Output, ELEMENT_TYPE ElementType, int iStartIndex = 1, int iIndexOffset = 1, string Seperator = ", ", bool bSCIRun = false) const;

		/// Calculate the volume of the mesh
		/**
		If the mesh is a surface mesh, the surface must be completely closed and have triangle normals
		facing outside the volume. If the mesh contains quads, a copy of the mesh will be created and
		then the quads will be converted to triangles for volume calculation. Similarly if the mesh contains
		any 3d elements, these will be converted to 2d elements which will in turn be converted to triangles.
		The original mesh will not be affected.
		*/
		double CalculateVolume() const;

		/// Get a vector of element centers, one entry for each element
		vector<XYZ> GetElementCenters() const;

		/// Get a vector of element centres of a given element type
		vector<XYZ> GetElementCenters( ELEMENT_TYPE type );

		/// Check and count if any of the elements are inverted
		/**
		This function is not efficient
		*/
		int CountInvertedElements() const;

		/// Check and count if the elements of given type are inverted
		/**
		This function is not efficient
		*/
		int CountInvertedElements(ELEMENT_TYPE ElementType) const;

		/// Find the points where a line intersects the mesh
		/**
		Given a line defined by points P1 and P2, a list of intersection points Pi
		are calculated such that \f$P_i = P1 + (P2-P1) \mu_i \f$. The values of \f$\mu_i\f$ are returned
		in IntersectionPoints along with the normal of the surface it intersects with. The list is sorted
		in ascending order of \f$\mu_i\f$.
		This operation only works for triangular meshes! If you want to perform it for other mesh types
		it must be converted to triangles first using Convert3Dto2D and/or ConvertQuadstoTriangles.
		\param P1 Origin of the line
		\param P2 End of the line
		\param IntersectionPoints Information about where the intersections occured
		\param TrimResults If true then only intersections found where \f$ 0 \le \mu_i \le 1 \f$ are returned
		\param bForceFind Force the algorithm to find the closest intersection if no exact intersections can be found
		\return Number of intersections found
		*/
		int IntersectLine(const XYZ &P1, const XYZ &P2, vector< pair<double, XYZ> > &IntersectionPoints, pair<bool, bool> TrimResults = make_pair(false, false), bool bForceFind = false/*, const CElementsOctree *pOctree = NULL*/) const;

		/// Build grid of points
		/**
		Builds a structured grid of points
		\param Min The lower bound for the set of points
		\param Max The upper bound for the set of points
		\param iNumX Number of points along the x axis
		\param iNumY Number of points along the y axis
		\param iNumZ Number of points along the z axis
		*/
		void BuildGrid(XYZ Min, XYZ Max, int iNumX, int iNumY, int iNumZ);

		/// Build grid of points
		/**
		Builds a structured grid of points
		\param Min The lower bound for the set of points
		\param Max The upper bound for the set of points
		\param dPointsPerUnit Number of points to per unit length
		*/
		void BuildGrid(XYZ Min, XYZ Max, double dPointsPerUnit);

		/// Return a list of node pairs (A, B) where A == B + Vector
		/**
		This is usefull for applying boundary conditions. For example given a block
		width w and height h, all node pairs given by vector XYZ(w, 0, 0) should be 
		tied together and all node pairs given by vector XYZ(0, h, 0) should be tied
		*/
		vector<pair<int, int> > GetNodePairs(XYZ Vector, const double Tolerance = 1e-6) const;
		void GetNodePairs(XYZ Vector, vector<pair<int, int> > &NodePairs, const double Tolerance = 1e-6) const;

		/// Get the index of the node closest to the given position space
		/**
		\param Position The position to which the closest node should be found
		\return The index of the closest node or -1 if no nodes exist in this mesh
		*/
		int GetClosestNode(XYZ Position) const;

		/// Get the index of the node within a given tolerance distance to the given position space
		/**
		\param Position The position to which the closest node should be found
		\param dTol The tolerance for the distance from the node to the point
		\return The index of the closest node or -1 if no nodes exist in this mesh
		*/
		int GetClosestNodeDistance(XYZ Position, double dTol ) const;

		/// Convert element index list to vector so can access by index
		/**
		\param ElementType The element type of the list to be converted
		\param Indices Array to add list to
		*/
		void ConvertElementListToVector( ELEMENT_TYPE ElementType, vector<int> &Indices );

		///////////////////////////////////////////////
		// Methods to output to various file formats //
		///////////////////////////////////////////////

		/// Save the mesh to STL file
		/**
        This operation only works for triangular meshes, however if the mesh contains other element
		types they are automatically converted to triangles.
		The file should have a .stl extension, if not it will be appended to the filename.
		*/
		bool SaveToSTL(string Filename, bool bBinary = true) const;

		/// Save the mesh to VTK unstructured grid file format (.vtu)
		/**
		\param Filename Name of the .vtu filename (extension will be automatically appended if ommited)
		\param pMeshData Contains a vector of additional data attached to nodes or elements that should be saved
		*/
		bool SaveToVTK(string Filename, const vector<CMeshDataBase*> *pMeshData = NULL) const;

		/// Save the mesh to ABAQUS input file format with information such as yarn tangents
		/**
		Note: The created input file makes use of the "Distribution Table" keyword which is only supported
		in ABAQUS version 6.7 and above. Therefore meshes will not be readable with previous version of ABAQUS.
		\param Filename Name of the .inp filename (extension will be automatically appended if ommited)
		\param pElementInfo Contains a vector of additional data attached to nodes or elements that should be saved
				(if specified additional .ori and .eld files will be generated)
		\param bCreateStep Create a step definition in the ABAQUS input deck, it may be desirable to ommit this
				and create step definitions manually, mainly because after a step definition is created any lines
				after this which affect the model definition are ignored.
		\param bCreateMaterial Create a material definition? Set this to false if you plan to create your own
				material definition.
		\param iElementType C3D8R if 0, C3D8 if 1
		*/
		bool SaveToABAQUS(string Filename, const vector<POINT_INFO> *pElementInfo = NULL, bool bCreateStep = true, bool bCreateMaterial = true, int iElementType = 0);

		/// Save the mesh to a .smesh file to be used in conjunction with tetgen
		bool SaveToSMESH(string Filename) const;

		/// Save the mesh to SCIRun format
		/**
		At the moment only saves nodes and tri elements
		*/
		bool SaveToSCIRun(string Filename);

//		bool ReadFromTetGen(string NodesFile, string ElementsFile, string FacesFile = "");

		/////////////////////////////////////////////////////////////////////////////
		// Bunch of accessor methods to prevent direct access to underlying vector //
		/////////////////////////////////////////////////////////////////////////////

		/// Add an element to the mesh of given type with node number checking
		bool AddElement(ELEMENT_TYPE Type, const vector<int> &Indices);

		/// Get the number of elements of a given type
		int GetNumElements(ELEMENT_TYPE Type) const;

		/// Get the total number of elements in the mesh
		int GetNumElements() const;

		vector<XYZ>::const_iterator NodesBegin() const;
		vector<XYZ>::const_iterator NodesEnd() const;
		vector<XYZ>::iterator NodesBegin();
		vector<XYZ>::iterator NodesEnd();

		/// Append a node to the list of nodes, the integer returns the index of the node	
		const int AddNode(XYZ Node);
		/// Set the node at given index
		void SetNode(int iIndex, XYZ Node);
		/// Get the node with given ID
		const XYZ& GetNode(int iIndex)const;
		/// Delete a node given iterator
		vector<XYZ>::iterator DeleteNode(vector<XYZ>::iterator it);
		/// Return the number of nodes
		int GetNumNodes() const;
		/// Returns true if the nodes array is empty
		const bool NodesEmpty() const;
		/// Get a const reference to the nodes
		const vector<XYZ>& GetNodes() const;
		vector<XYZ>& GetNodes();
		/// Resize the vector size
		void SetNumNodes(int NumNodes);

		/// Get the element indices of a given element type
		const list<int>& GetIndices(ELEMENT_TYPE ElemType) const;
		list<int>& GetIndices(ELEMENT_TYPE ElemType);


	protected:
		/// Add an edge to the stack of edges if it doesn't already exist otherwise if it exists delete it
		/**
		This function is used in conjunction with MeshConvexHull.
		*/
		static void AddOrCancel(list<pair<int, int> > &EdgeStack, pair<int, int> Edge);

		static void WriteBinaryXYZ(ostream &Output, XYZ Vector);

		int FillVTKPointData(TiXmlElement &Points) const;
		int FillVTKCellData(TiXmlElement &Cells) const;

		/// List of nodes
		vector<XYZ> m_Nodes;
		/// Map of indices into the nodes
		list<int> m_Indices[NUM_ELEMENT_TYPES];

	};

};	// namespace TexGen




