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
#include "Section.h"

namespace TexGen
{ 
	using namespace std;

	/// Section which represents a scaled version of another section
	class CLASS_DECLSPEC CSectionScaled : public CSection
	{
	public:
		CSectionScaled(const CSection &Section, XY Scale);
		CSectionScaled(TiXmlElement &Element);
		~CSectionScaled(void);

		bool operator == (const CSection &CompareMe) const;
		CSection* Copy() const { return new CSectionScaled(*this); }

		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		string GetType() const { return "CSectionScaled"; }
		string GetDefaultName() const;

		XY GetPoint(double t) const;

		// Accessor methods
		const CSection &GetSection() const { return *m_pSection; }
		XY GetScale() const { return m_Scale; }

	protected:
		XY m_Scale;
		CObjectContainer<CSection> m_pSection;
	};

};	// namespace TexGen
