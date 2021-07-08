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

	/// Lenticular Section
	class CLASS_DECLSPEC CSectionLenticular : public CSection
	{
	public:
		CSectionLenticular(double dWidth, double dHeight, double dDistortion = 0);
		CSectionLenticular(TiXmlElement &Element);
		~CSectionLenticular(void);

		bool operator == (const CSection &CompareMe) const;
		CSection* Copy() const { return new CSectionLenticular(*this); }

		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		string GetType() const { return "CSectionLenticular"; }
		string GetDefaultName() const;

		XY GetPoint(double t) const;

		// Accessor methods
		double GetWidth() const { return m_dWidth; }
		double GetHeight() const { return m_dHeight; }
		double GetDistortion() const { return m_dDistortion; }

	protected:
		double m_dWidth;
		double m_dHeight;
		double m_dDistortion;
	};
};	// namespace TexGen
