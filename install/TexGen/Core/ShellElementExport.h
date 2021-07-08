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

#include <iterator>
#include "Mesh.h"
#include "Materials.h"

namespace TexGen
{ 
	using namespace std;	

	/// Class for exporting surface mesh to ABAQUS
	class CLASS_DECLSPEC CShellElementExport
	{
	public:
		CShellElementExport( bool bWholeSurfaces, bool bTrimSurface = true );
		~CShellElementExport(void);
		
		/// Save surface mesh to ABAQUS input file
		/**
		\param Filename including path. Any spaces will be stripped in function
		\param Textile to be exported
		\return True if export successful, False if not
		*/
		bool SaveShellElementToABAQUS(string Filename, CTextile& Textile );

		const CMesh &GetMesh() { return m_SurfaceMesh; }

	protected:
		/// Get the element info for centre points of surface mesh elements
		void GetElementInfo( CTextile& Textile );
		/// Generate node and element offsets for merging individual yarn nodes and elements into one ABAQUS file
		void BuildIndexOffsets();

		typedef int YarnNumber;
		void CreateSurfaceDefinitions( int iNumYarns, const vector<XYZ> &Repeats, map<string, vector< pair<int,int> > > &SurfaceDefinitions);
		void GetYarnSurfaces(int iYarn, const vector<XYZ> &Repeats, vector<pair<int,int> > &UpperFaces, vector<pair<int,int> > &LowerFaces );
		void GetYarnSurface(int iYarn, vector<pair<int,int> > &Elements );
		void CreateSurfaces(ostream &Output, map< string, vector<pair<YarnNumber,int> > > &SurfaceDefinitions);
		int GetGlobalElementIndex(pair<YarnNumber,int> Element);

		void CreateContacts(ostream &Output, const CTextile &Textile);
		void CreateContacts(ostream &Output, const CTextileWeave &Weave);
		void CreateContacts(ostream &Output, const CTextile3DWeave &Weave);
		void OutputContacts( ostream &Output, set<pair<int, int> > &Contacts );
		void CreateContact(ostream &Output, string Name1, string Name2, string InteractionName);
		void CreateInteractions( ostream &Output );

		CMesh m_SurfaceMesh;
		vector<CMesh> m_YarnMeshes;
		vector<POINT_INFO> m_ElementInfo;

		map<YarnNumber, int> m_ElementIndexOffsets;
		map<YarnNumber, int> m_NodeIndexOffsets;

		/// True if surfaces to be trimmed to domain
		bool m_bTrimSurface;
		/// True if domain surface is to be exported
		bool m_bExportDomain;
		bool m_bWholeSurfaces;

		/// Class for export of material properties
		CTextileMaterials* m_Materials;
	};

};	// namespace TexGen
