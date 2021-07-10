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
	/// Fibre volume fraction is constant throughout the yarn
	class CLASS_DECLSPEC CFibreDistributionConst : public CFibreDistribution
	{
		public:
			CFibreDistributionConst();
			CFibreDistributionConst(TiXmlElement &Element);
			~CFibreDistributionConst() {};
			
			CFibreDistribution* Copy() const { return new CFibreDistributionConst(*this); }
			string GetType() const {return "CFibreDistributionConst";}
			void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

			double GetVolumeFraction(const vector<XY> &Section, double dFibreArea, XY Location, int YarnIndex = -1) const;
			double GetVolumeFraction(double dArea, double dFibreArea, int YarnIndex = -1) const;

		protected:
			double IntegrateDistribution(const vector<XY> &Section) const;
	};
};	// namespace TexGen



