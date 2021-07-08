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
#include "SectionMesh.h"

namespace TexGen
{ 
	using namespace std;

	/// Create a rectangular mesh, the number of layers can be specified or set as -1 for automatic determination
	/**
	Mesh is created with regular rectangular grid
	*/
	class CLASS_DECLSPEC CSectionMeshRectangleSection : public CSectionMesh
	{
	public:
		CSectionMeshRectangleSection(int iNumLayers = -1);
		CSectionMeshRectangleSection(TiXmlElement &Element);
		~CSectionMeshRectangleSection(void);

		CSectionMesh* Copy() const { return new CSectionMeshRectangleSection(*this); }
		string GetType() const { return "CSectionMeshRectangleSection"; }
		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		void SetNumLayers( int iNum );
		int GetNumLayers() const { return m_iNumLayers; }
		int CalculateNumberofLayers(const vector<XY> &Section) const;

	protected:
		bool CreateMesh(const vector<XY> &Section) const;
		//void GetMinMaxXY(const vector<XY> &Section, XY &Min, XY &Max ) const;

		int m_iNumLayers;
	};

}	// namespace TexGen
