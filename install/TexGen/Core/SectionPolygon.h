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

	/// Creates a polygonal section, where a list of points are given to form the closed polygon
	/**
	Points are in local XY coordinates relative to the yarn centreline.  
	Points start at (maxX, 0) and are ordered in an anticlockwise direction
	*/
	class CLASS_DECLSPEC CSectionPolygon : public CSection
	{
	public:
		CSectionPolygon(const vector<XY> &PolygonPoints, bool bSingleQuadrant = false);
		CSectionPolygon(TiXmlElement &Element);
		~CSectionPolygon(void);

		bool operator == (const CSection &CompareMe) const;
		CSection* Copy() const { return new CSectionPolygon(*this); }

		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		string GetType() const { return "CSectionPolygon"; }
		string GetDefaultName() const;

		XY GetPoint(double t) const;

		/// Change the scale of the section by multiplying each coordinate component by the component given by this XY struct
		void Scale(XY Scale);

		/// Change the scale of the section by multiplying each coordinate value by a scalar
		void Scale(double dScale);

	protected:
		/// Assign t value as proportion of distance around perimeter for each point
		void CalcTValues();

		vector<XY> m_PolygonPoints;
		/// The proportion of the distance around the total perimeter range from 0 to 1 for each point
		vector<double> m_t;

	};
};	// namespace TexGen
