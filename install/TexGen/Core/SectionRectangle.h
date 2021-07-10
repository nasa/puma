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
#include "Section.h"

namespace TexGen
{ 
	using namespace std;

	/// Rectangle section
	/**
	Creates a rectangular section where points around perimeter are spaced as evenly as possible
	along sides within constraint of having a point at each corner.
	*/
	
	class CLASS_DECLSPEC CSectionRectangle : public CSection
	{
	public:
		CSectionRectangle(double dWidth, double dHeight);
		CSectionRectangle(TiXmlElement &Element);
		~CSectionRectangle(void);

		bool operator == (const CSection &CompareMe) const;
		CSection* Copy() const { return new CSectionRectangle(*this); }

		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		/// Get a section with given number of points on the perimeter
		virtual const vector<XY> &GetPoints(int iNumPoints, bool bEquiSpaced = false) const;

		string GetType() const { return "CSectionRectangle"; }
		string GetDefaultName() const;

		XY GetPoint(double t) const;

		// Accessor methods
		double GetWidth() const { return m_dWidth; }
		double GetHeight() const { return m_dHeight; }
		void SetWidth( double dWidth ){ m_dWidth = dWidth; }
		void SetHeight( double dHeight ){ m_dHeight = dHeight; }

	protected:
		double m_dWidth, m_dHeight;
		double m_XSpacing, m_YSpacing;
	};
};	// namespace TexGen
