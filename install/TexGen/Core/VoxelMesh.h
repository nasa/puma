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
#include "Materials.h"

namespace TexGen
{ 
	using namespace std;

	class CTextile;	
	class CPeriodicBoundaries;
	class CMaterials;

	/// Class used to generate voxel mesh for output to ABAQUS
	class CLASS_DECLSPEC CVoxelMesh
	{
	public:
		CVoxelMesh(string Type = "CPeriodicBoundaries");
		virtual ~CVoxelMesh(void);
		/** Save the voxel mesh as an Abaqus input file with periodic boundary conditions
		\param bOutputMatrix True to output the matrix area of the mesh
		\param bOutputYarns True to output the yarn areas
		\param iBoundaryConditions as in enum PERIODIC_BOUNDARY_CONDITIONS
		\param iElementType 0 for C3D8R, 1 for C3D8
		*/
		virtual void SaveVoxelMesh(CTextile &Textile, string OutputFilename, int XVoxNum, int YVoxNum, int ZVoxNum, bool bOutputMatrix, bool bOutputYarns, int iBoundaryConditions, int iElementType = 0, int FileType = INP_EXPORT);
		/// Add a row of element information
		void AddElementInfo(vector<POINT_INFO> &RowInfo);
		/// Outputs yarn orientations and element sets to .ori and .eld files
		void OutputOrientationsAndElementSets( string Filename );

		CTextileMaterials& GetMaterials() { return m_Materials; }

	protected:
		CTextileMaterials m_Materials;

	private:
		CVoxelMesh( const CVoxelMesh& CopyMe );  // Shouldn't need to copy - implement if need arises
		CVoxelMesh& operator=( const CVoxelMesh& CopyMe );

	protected:

		/// Calculate voxel size based on number of voxels on each axis and domain size
		virtual bool CalculateVoxelSizes(CTextile &Textile) = 0;
		
		/// Add nodes for whole domain
		//void AddNodes();
		/// Add hex elements
		//void AddElements();
		/// Save voxel mesh in VTK format without boundary conditions
		void SaveVoxelMeshToVTK(string Filename, CTextile &Textile, bool bOutputYarns);
		/// Save voxel mesh in Abaqus .inp format with periodic boundary conditions
		/// bOutputMatrix and bOutput yarn specify which of these are saved to the Abaqus file
		void SaveToAbaqus( string Filename, CTextile &Textile, bool bOutputMatrix, bool bOutputYarn, int iBoundaryConditions, int iElementType );
		/// Save voxel mesh in SCIRun .pts and .hex format without boundary conditions
		void SaveToSCIRun( string Filename, CTextile &Textile );
		/// Outputs nodes to .inp file and gets element information
		virtual void OutputNodes(ostream &Output, CTextile &Textile, int Filetype = INP_EXPORT) = 0;

		/// Output hex elements to .inp file
		/**
		\ return Maximum element index
		*/
		virtual int OutputHexElements(ostream &Output, bool bOutputMatrix, bool bOutputYarn, int Filetype = INP_EXPORT );

		/// Outputs yarn orientations and element sets to .ori and .eld files
		void OutputOrientationsAndElementSets( string Filename, ostream &Output );
		/// Outputs all elements when only outputting matrix
		void OutputMatrixElementSet( string Filename, ostream &Output, int iNumHexElements, bool bMatrixOnly );
		/// Output node set containing all nodes
		void OutputAllNodesSet( string Filename, ostream &Output );
		/// Output periodic boundary conditions to .inp file
		virtual void OutputPeriodicBoundaries(ostream &Output, CTextile& Textile, int iBoundaryConditions, bool bMatrixOnly);

		//void OutputSets( ostream& Output, vector<int>& GroupA, vector<int>& GroupB, int i, int iDummyNodeNum );
		/// Find intersections of yarn surfaces with grid of lines from node points in each axis
		//void GetYarnGridIntersections( CTextile &Textile );
		/// Calculate intersections for one orientation of grid lines
		//void CalculateIntersections( CMesh &Mesh, vector<pair<XYZ,XYZ> > &Lines, int YarnIndex, vector<vector<pair<int, double> > > &Intersections);

		//template <typename T>
		/// Output data with iMaxPerLine elements per line
		//static void WriteValues(ostream &Output, T &Values, int iMaxPerLine);

		/// Mesh stored when saving to VTK
		CMesh			m_Mesh;
		
		/// Number of voxels along x,y and z axes
		int				m_XVoxels;
		int				m_YVoxels;
		int				m_ZVoxels;
		

		/// Domain limits
		pair<XYZ, XYZ>	m_DomainAABB;
		/// Element information as calculated by GetPointInformation
		vector<POINT_INFO>	m_ElementsInfo;

		//CObjectContainer<CPeriodicBoundaries> m_PeriodicBoundaries;
		CPeriodicBoundaries* m_PeriodicBoundaries;
	};
};	// namespace TexGen
