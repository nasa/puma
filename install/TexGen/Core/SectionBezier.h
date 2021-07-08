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

	/// Section made up of bezier curves
	/**
	Section defined as a series bezier curves, the class takes points as input.
	Four consecutive points describe a bezier curve, the fourth point of the current
	bezier curve and first point of the following bezier curve are shared. This ensures
	0th order continuity of the section. Thus the number of points specified should
	be a multiple of 3, the number of bezier curves created is the number of points
	divided by 3.
	*/
	class CLASS_DECLSPEC CSectionBezier : public CSection
	{
	public:
		/// The number of points given must be a multiple of 3
		CSectionBezier(const vector<XY> &BezierPoints, bool bSingleQuadrant = false);
		CSectionBezier(TiXmlElement &Element);
		~CSectionBezier(void);

		bool operator == (const CSection &CompareMe) const;
		CSection* Copy() const { return new CSectionBezier(*this); }

		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		string GetType() const { return "CSectionBezier"; }
		string GetDefaultName() const;

		XY GetPoint(double t) const;

	protected:
		vector<XY> m_BezierPoints;
	};
};	// namespace TexGen
