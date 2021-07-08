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
#include "tetgen.h"
#include "MeshDomainPlane.h"

namespace TexGen
{
	using namespace std;

	/// Class which tetrahedralizes a textile using Tetgen software
	/**The surface mesh and domain are used as tetgen input, the domain boundaries first being
	triangulated such that opposite boundaries have the same input mesh. (This assumes that
	the domain is specified to be a whole number of textile repeats).
	*/
	class CLASS_DECLSPEC CTetgenMesh : public CMeshDomainPlane
	{
	public:
		CTetgenMesh( double Seed );
		virtual ~CTetgenMesh(void);

		/// Save a textile as a tetrahedralized mesh using Tetgen
		/**
		\param Textile Textile to be meshed
		\param Filename for Tetgen output files
		\param Parameters tetgen flags to be applied during tetrahedralization
		\param bPeriodic If set true, opposite faces of mesh will be replicated
		*/
		void SaveTetgenMesh(CTextile &Textile, string OutputFilename, string Parameters, bool bPeriodic, int FileType );

	protected:
		///	Mesh used to store input node points and elements
		CMesh			m_Mesh;
		/// Tetgen input and output structures
		tetgenio		m_in, m_out;
		/// Mesh used to store output nodes and elements
		CMesh			m_OutputMesh;
		/// Element information for output mesh
		vector<POINT_INFO> m_ElementsInfo;

		/// Save tetgenio data to CMesh
		void SaveMesh(CTextile &Textile);
		/// Save output mesh to Abaqus export file
		void SaveToAbaqus( string Filename, CTextile &Textile );
		/// Save output mesh to VTK format
		void SaveToVTK(string Filename);
		
	};
};  // namespace TexGen
