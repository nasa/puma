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

namespace TexGen
{
	using namespace std;

	/// Abstract base class that defines how the fibres are distributed within a yarn
	/**
	* This class is there to provide local volume fraction information for a yarn
	* cross section, from this different Vf distributions can be derived 
	*/
	class CLASS_DECLSPEC CFibreDistribution
	{
		public:
			CFibreDistribution();
			CFibreDistribution(TiXmlElement &Element);
			virtual ~CFibreDistribution(void);

			/// Create a copy of the derived fibre distribution and return a pointer to the newly created instance
			virtual CFibreDistribution* Copy() const = 0;
			/// Derived class should return the class name
			virtual string GetType() const = 0;
			/// Used for saving data to XML
			virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

			/// Get the volume fraction for a given location
			virtual double GetVolumeFraction(const vector<XY> &Section, double dFibreArea, XY Location, int YarnIndex=-1) const = 0;
			/// Get the volume fraction given an area
			virtual double GetVolumeFraction(double dArea, double dFibreArea, int YarnIndex=-1) const;


		protected:

	};
};	// namespace TexGen

