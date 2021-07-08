/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2019 Louise Brown

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
#include "MeshDomainPlane.h"

namespace TexGen
{
	using namespace std;

	/// Class which generates a surface mesh with triangulated domain boundaries 
	/** (This assumes that
	the domain is specified to be a whole number of textile repeats).
	*/
	class CLASS_DECLSPEC CSurfaceMesh : public CMeshDomainPlane
	{
	public:
		/// Constructor for surface mesh class
		/**
		\param Seed Seed size for meshing domain surfaces
		\param bFillEnds True if yarn areas in domain surfaces are to be meshed
		*/
		CSurfaceMesh(double Seed, bool bFillEnds = true );
		virtual ~CSurfaceMesh(void);
		/// Save a textile as a surface mesh with triangulated domain faces
		/**
		\param Textile Textile to be meshed
		\param bSaveYarns True if yarn surfaces to be saved
		\param bSaveDomain True if domain surfaces to be saved
		\param bTrimToDomain True if yarns are to be trimmed to domain surfaces
		*/
		void SaveSurfaceMesh(CTextile &Textile, bool bSaveYarns = true, bool bSaveDomain = false, bool bTrimToDomain = true);

		/// Save surface mesh data to STL file
		/**
		\param Filename Filename to save data to
		\param bBinary True to save as binary file, False to save as ASCII
		*/
		void SaveToSTL(string Filename, bool bBinary);
		/// Save surface mesh data to VTK file
		void SaveToVTK(string Filename);
		/// Save surface mesh data to SCIRun format
		void SaveToSCIRun(string Filename);

	protected:
		///	Mesh used to store node points and elements
		CMesh			m_Mesh;

	};
};  // namespace TexGen