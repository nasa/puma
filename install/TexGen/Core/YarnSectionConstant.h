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
#include "YarnSection.h"
namespace TexGen
{
	using namespace std;

	/// Creates a section which is constant all along the yarn
	class CLASS_DECLSPEC CYarnSectionConstant : public CYarnSection
	{
	public:
		CYarnSectionConstant(const CSection &Section);
		CYarnSectionConstant(TiXmlElement &Element);
		~CYarnSectionConstant(void);

		CYarnSection* Copy() const { return new CYarnSectionConstant(*this); }
		string GetType() const { return "CYarnSectionConstant"; }
		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		vector<XY> GetSection(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced = false) const;
		CMesh GetSectionMesh(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced) const;

		// Accessor methods
		const CSection &GetSection() const { return *m_pSection; }

	protected:
		CObjectContainer<CSection> m_pSection;
	};

};	// namespace TexGen







