/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2012 Louise Brown

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
#include "VoxelMesh.h"

namespace TexGen
{ 
	using namespace std;

	class CTextile;	

	/// Class used to generate voxel mesh for output to ABAQUS
	class CLASS_DECLSPEC CRectangularVoxelMesh : public CVoxelMesh
	{
	public:
		/// 
		/**
		\param Type String giving periodic boundary condition type 
		*/
		CRectangularVoxelMesh(string Type= "CPeriodicBoundaries");
		virtual ~CRectangularVoxelMesh(void);

	protected:
		/// Calculate voxel size based on number of voxels on each axis and domain size
		bool CalculateVoxelSizes(CTextile &Textile);
		
		/// Save voxel mesh in Abaqus .inp format with periodic boundary conditions
		/// bOutputMatrix and bOutput yarn specify which of these are saved to the Abaqus file
		void SaveToAbaqus( string Filename, CTextile &Textile, bool bOutputMatrix, bool bOutputYarn, int iBoundaryConditions, int iElementType );
		
		/// Outputs nodes to .inp file and gets element information
		void OutputNodes(ostream &Output, CTextile &Textile, int Filetype = INP_EXPORT );

		
		/// Voxel size for each axis
		double			m_VoxSize[3];

	};
};	// namespace TexGen
