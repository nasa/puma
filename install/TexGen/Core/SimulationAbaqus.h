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

#include "Simulation.h"
#include "Materials.h"

namespace TexGen
{ 
	using namespace std;

	class CTextileWeave;
	class CTextile3DWeave;
	class CDomain;
	class CMaterial;

	struct ELEMENT_INDICES
	{
		CMesh::ELEMENT_TYPE ElementType;
		vector<int>			Index;
	};

	/// Class used to generate an abaqus input deck for textile mechanics simulations
	class CLASS_DECLSPEC CSimulationAbaqus : public CSimulation
	{
	public:
		CSimulationAbaqus(void);
		virtual ~CSimulationAbaqus(void);

		bool CreateAbaqusInputFile(CTextile &Textile, string Filename, bool bRegenerateMesh, int iElementType = 0, bool bAdjustMesh = false, double Tolerance = 0.0000001 );

		void SetIncludePlates(bool bInclude) { m_bIncludePlates = bInclude; }
		bool GetIncludePlates() { return m_bIncludePlates; }

		void SetInitialPlateGap(double dPlateGap) { m_dInitialPlateGap = dPlateGap; }
		double GetInitialPlateGap() { return m_dInitialPlateGap; }

		void SetWholeSurfaces(bool bWholeSurface ) { m_bWholeSurfaces = bWholeSurface; }
		bool GetWholeSurfaces() { return m_bWholeSurfaces; }

		/// Assign a surface interaction to be used at contacts between yarns
		/**
		The string specified will be added in the .inp file below "*Surface Interaction, Name=Yarn".
		It should be a series of ABAQUS keywords which may span multiple lines, the last line should
		contain a newline character at the end.
		*/
		void SetYarnSurfaceInteraction(string AbaqusCommands);
		/// Assign a surface interaction to be used at contacts between yarns and compaction plates
		/**
		The string specified will be added in the .inp file below "*Surface Interaction, Name=Plate".
		It should be a series of ABAQUS keywords which may span multiple lines, the last line should
		contain a newline character at the end.
		*/
		void SetPlateSurfaceInteraction(string AbaqusCommands);
		
		void SetStaticStepParameters(double dInitTimeInc = 0, double dTimePeriod = 0, double dMinTimeInc = 0, double dMaxTimeInc = 0);

	protected:
		enum SET_TYPE
		{
			NODE_SET,
			ELEMENT_SET,
		};

		enum SURFACE_TYPE
		{
			SURFACE_UPPER,
			SURFACE_LOWER,
			SURFACE_ALL
		};

		struct ELEMENT_FACE
		{
			int iYarn;
			CMesh::ELEMENT_TYPE ElementType;
			int iElementIndex;
			int iFaceIndex;
		};

		/*struct ELEMENT_INDICES
		{
			CMesh::ELEMENT_TYPE ElementType;
			vector<int>			Index;
		};*/

		struct INTERSECTION_DATA
		{
		double  Depth;
		XYZ		Point;
		XYZ		InterpPoint;
		int		Yarn;
		int		Index;
		vector<ELEMENT_INDICES> Elements;
		};

		struct SECTION_VF_DATA
		{
			int iMin;
			int iMax;
			double dVolumeFraction;
		};

		

		/// Abstract base class to represent a surface interaction
		class CSurfaceInteraction
		{
		public:
			virtual CSurfaceInteraction* Copy() const = 0;
			virtual string GetAbaqusCommands() = 0;
		};

		/// Represents a surface interaction as a string of ABAQUS keywords
		class CKeywordInteraction : public CSurfaceInteraction
		{
		public:
			CKeywordInteraction() {}
			CKeywordInteraction(string AbaqusCommands):m_AbaqusCommands(AbaqusCommands){}
			CSurfaceInteraction* Copy() const { return new CKeywordInteraction(*this); }
			string GetAbaqusCommands() { return m_AbaqusCommands; }
			void SetAbaqusCommands(string AbaqusCommands) { m_AbaqusCommands = AbaqusCommands; }
		protected:
			string m_AbaqusCommands;
		};

		/// Create the materials for the yarns
		void CreateMaterials(ostream &Output, string Filename);

		
		/// Create a contact pair from two surface definitions
		void CreateContact(ostream &Output, string Name1, string Name2, string InteractionName);

		/// Output an element or node set from a list of indices
		/**
		Note that TexGen meshes are 0-based (first element has index 0) while Abaqus is 1 based.
		This function will autmatically add 1 to each index. So this function should be passed
		a set of 0-based indices which will the be converted to 1-based for Abaqus.
		\param Output Output stream to which the set will be written to
		\param Type On of the SET_TYPE enumurations, can be either node or element
		\param Name The name of the set to create
		\param Indices A list of 0-based indices that form the set
		\param bUnSorted Whether or not abaqus should sort the set
		*/
		void CreateSet(ostream &Output, SET_TYPE Type, string Name, vector<int> &Indices, bool bUnSorted = false);
		/// Create two node sets from a vector of pairs of ints
		/**
		The name is used as the base name, two sets will be create as <name>A and <name>B
		\param Output Output stream to which the set will be written to
		\param Name The base name of the set to create
		\param NodePairs A list of 0-based pairs of indices (perhaps obtained from CMesh::GetNodePairs)
		*/
		void CreateSet(ostream &Output, string Name, const vector<pair<int, int> > &NodePairs);

		/// Create ABAQUS surface definitions
		/**
		This function creates 2 surface definitions for each yarn, one for the upper surface
		named YarnXUpper and one for the lower surface named YarnXLower where X is replaced
		with the yarn index
		*/
		void CreateSurfaces(ostream &Output, map<string, vector<ELEMENT_FACE> > &SurfaceDefinitions);

		void CreateSurfaceDefinitions( int iNumYarns, const vector<XYZ> &Repeats, map<string, vector<ELEMENT_FACE> > &SurfaceDefinitions);

		/// Create the contact definitions
		/**
		This function simply delegates the task to a function which takes
		a specific textile type. Or prints an error if no such function
		exists
		*/
		void CreateContacts(ostream &Output, const CTextile &Textile);

		/// Create the contact definitions for a woven fabric
		void CreateContacts(ostream &Output, const CTextileWeave &Weave);

		/// Output the contact definitions
		void OutputContacts( ostream &Output, set<pair<int, int> > &Contacts );

		/// Create the contact definitions for a 3D woven fabric
		void CreateContacts(ostream &Output, const CTextile3DWeave &Weave);

		/// Create two rigid body analytical planes: one above and one below
		void CreateCompressionPlates(ostream &Output);

		/// Create a step with given deformation
		void CreatePeriodicBoundaries(ostream &Output, const CDomain &Domain);

		/// Create a step 
		void CreateStep(ostream &Output);

		/// Create output request and boundary conditions with given deformation
		void CreateBoundary(ostream &Output, CLinearTransformation Deformation, const vector<int> &YarnCenterNodes, const vector<XYZ> &Repeats);

		/// Get an element face index as defined by ABAQUS documentation given the node indices
		/// making up the face
		/**
		Used by CreateSurfaces to define the surfaces
		*/
		int GetFaceIndex(CMesh::ELEMENT_TYPE ElemType, const set<int> &NodeIndices);

		/// Get a list of faces which make up the surfaces of a yarn
		/**
		\param iYarn The yarn index
		\param Repeats The yarn repeat vectors
		\param UpperFaces, LowerFaces Vectors for lists of upper and lower surface faces
		*/
		void GetYarnSurfaces(int iYarn, const vector<XYZ> &Repeats, vector<ELEMENT_FACE> &UpperFaces, vector<ELEMENT_FACE> &LowerFaces );

		/// Get a list of faces which make up the surface of a yarn
		/**
		\param iYarn The yarn index
		\param Repeats The yarn repeat vectors
		\param Faces, LowerFaces Vectors for lists of surface faces
		*/
		void GetYarnSurface(int iYarn, const vector<XYZ> &Repeats, vector<ELEMENT_FACE> &Faces );

		/// Given a surface element, find the volume element it belongs to along with its face index
		/**
		\param iYarn The yarn index
		\param SurfaceType The surface element type (usually either triangle or quad)
		\param SurfIndices The indices of the surface element
		*/
		ELEMENT_FACE FindFaceIndex(int iYarn, const vector<int> &SurfIndices);

		/// Find the common indices between a surface element and a volume element
		/**
		\param SurfIndices The indices of the surface element
		\param VolIndices The indices of the volume element
		\return Returns a list of local node indices into the volume element
		*/
		set<int> GetCommonIndices(const vector<int> &SurfIndices, const vector<int> &VolIndices);

		/// Get the central node index of a given mesh
		int GetCenterNode(const CMesh &Mesh);

		/// Populate m_IndexOffsets parameter
		void BuildIndexOffsets();

		/// Given the element index of a particular yarn and element type,
		/// return the global element index
		int GetGlobalElementIndex(int iYarn, CMesh::ELEMENT_TYPE ElemType, int iIndex);

		/// Get the global element index of a given face
		int GetGlobalElementIndex(ELEMENT_FACE Face);

		/// Get the global element index of a given face
		int GetGlobalNodeIndex(int iYarn, int iIndex);

		/// Get the cross-sectional area of the yarn section
		double GetSectionArea( vector<int> &Section, CMesh &Mesh );

		/// Get the volume fraction based on a given area
		double GetSectionVolumeFraction( double Area, CTextile &Textile, int Yarn );

		/// Create an array of volume fractions at the mid points between sections
		void CreateMidPointVolumeFractions( vector<SECTION_VF_DATA> &VFData, vector<SECTION_VF_DATA> &VFMidData );

		/// Save the volume fraction for a given element - taken to be at the centre of the element
		void GetElementVolumeFractions( vector<POINT_INFO> &ElementsInfo, vector<SECTION_VF_DATA> &MidVFData );

		/// Get volume fraction of section saved in POLYGON mesh section
		void GetSectionVolumeFractions(CTextile &Textile, vector<SECTION_VF_DATA> &VolFractionData, int iYarn );

		

		vector<CMesh> m_YarnMeshes;
		CMesh m_TextileMesh;
		typedef int YarnNumber;
		map<YarnNumber, map<CMesh::ELEMENT_TYPE, int> > m_ElementIndexOffsets;
		map<YarnNumber, int> m_NodeIndexOffsets;
		int m_iTotalNumNodes;		// This is including any dummy nodes created for whatever purposes
		bool m_bIncludePlates;
		XYZ m_TopPlatePos;
		XYZ m_BottomPlatePos;
		double m_dInitialPlateGap;
		bool m_bWholeSurfaces;
		
		CTextileMaterials m_Materials;
		
		string m_StaticStepParameters;
		CObjectContainer<CSurfaceInteraction> m_YarnInteraction;
		CObjectContainer<CSurfaceInteraction> m_PlateInteraction;
	};
};	// namespace TexGen















