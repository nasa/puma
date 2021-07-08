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
#include "FibreDistribution.h"

namespace TexGen
{
	/// Fibre volume fraction is defined as a quadratic equation varying only along the X axis
	class CLASS_DECLSPEC CFibreDistribution1DQuad : public CFibreDistribution
	{
		public:
			CFibreDistribution1DQuad(double dDropOff);
			CFibreDistribution1DQuad(TiXmlElement &Element);
			~CFibreDistribution1DQuad() {};

			CFibreDistribution* Copy() const { return new CFibreDistribution1DQuad(*this); }
			string GetType() const {return "CFibreDistribution1DQuad";}
			void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

			double GetVolumeFraction(const vector<XY> &Section, double dFibreArea, XY Location, int YarnIndex = -1) const;

		protected:
			double ComputeMaxX(const vector<XY> &Section) const;
			double Distribution(double max, double min, double x, double dMaxX) const;
			double IntegrateDistribution(const vector<XY> &Section, double dMaxX) const;

			double m_dDropOff;
	};
};	// namespace TexGen



