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

	/// Section which represents a rotation of another section angle given in radians
	class CLASS_DECLSPEC CSectionRotated : public CSection
	{
	public:
		/// The angle is given in radians as an anti-clockwise rotation, -ve angles represent a clockwise rotation
		CSectionRotated(const CSection &Section, double dAngle);
		CSectionRotated(TiXmlElement &Element);
		~CSectionRotated(void);

		bool operator == (const CSection &CompareMe) const;
		CSection* Copy() const { return new CSectionRotated(*this); }

		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		string GetType() const { return "CSectionRotated"; }
		string GetDefaultName() const;
		string GetBaseType() const;

		XY GetPoint(double t) const;

		// Accessor methods
		const CSection &GetSection() const { return *m_pSection; }
		double GetAngle() const { return m_dAngle; }

	protected:
		double m_dAngle;
		CObjectContainer<CSection> m_pSection;
	};

};	// namespace TexGen
