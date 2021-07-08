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

#include "Property.h"

namespace TexGen
{ 
	using namespace std;

	/// Class to store properties related to a textile and/or yarn
	/**
	This class combines geometrical information stored in the CTextile class with user
	input data relating to physical properties of the textile. Several interesting
	values can be obtained in this manner, for example the volume fraction is calculated
	from the volume of yarns in the geometrical model along with information about fibre
	density and areal density.

	The units are specified as a string which enables quite a bit of flexibility. The units
	supported that are relevant to TexGen are:

	<table border="1">
	<tr>  <td>Quantity</td>  <td>Unit Name</td>  <td>Abbr.</td>  </tr>
	<tr>  <td>Length</td>  <td>meter</td>  <td>m</td>  </tr>
	<tr>  <td>Mass</td>  <td>kilogram</td>  <td>kg</td>  </tr>
	<tr>  <td>Pressure</td>  <td>pascal</td>  <td>Pa</td>  </tr>
	<tr>  <td>Temperature</td>  <td>Kelvin</td>  <td>K</td? </tr>
	</table>

	Which can be prefixed with any of the following:

	<table border="1">
	<tr>
	<td>Prefix Name</td><td>Abbr.</td><td>Factor</td>
	</tr>
	<tr>  <td>yotta</td>  <td>Y</td>  <td>1e24</td>  </tr>
	<tr>  <td>zetta</td>  <td>Z</td>  <td>1e21</td>  </tr>
	<tr>  <td>exa</td>  <td>E</td>  <td>1e18</td>  </tr>
	<tr>  <td>peta</td>  <td>P</td>  <td>1e15</td>  </tr>
	<tr>  <td>tera</td>  <td>T</td>  <td>1e12</td>  </tr>
	<tr>  <td>giga</td>  <td>G</td>  <td>1e9</td>  </tr>
	<tr>  <td>mega</td>  <td>M</td>  <td>1e6</td>  </tr>
	<tr>  <td>kilo</td>  <td>k</td>  <td>1e3</td>  </tr>
	<tr>  <td>hecto</td>  <td>h</td>  <td>1e2</td>  </tr>
	<tr>  <td>deka</td>  <td>da</td>  <td>1e1</td>  </tr>
	<tr>  <td>deca</td>  <td></td>  <td>1e1</td>  </tr>
	<tr>  <td></td>  <td></td>  <td></td>  </tr>
	<tr>  <td>deci</td>  <td>d</td>  <td>1e-1</td>  </tr>
	<tr>  <td>centi</td>  <td>c</td>  <td>1e-2</td>  </tr>
	<tr>  <td>milli</td>  <td>m</td>  <td>1e-3</td>  </tr>
	<tr>  <td>micro</td>  <td>u</td>  <td>1e-6</td>  </tr>
	<tr>  <td>nano</td>  <td>n</td>  <td>1e-9</td>  </tr>
	<tr>  <td>pico</td>  <td>p</td>  <td>1e-12</td>  </tr>
	<tr>  <td>femto</td>  <td>f</td>  <td>1e-15</td>  </tr>
	<tr>  <td>atto</td>  <td>a</td>  <td>1e-18</td>  </tr>
	<tr>  <td>zepto</td>  <td>z</td>  <td>1e-21</td>  </tr>
	<tr>  <td>yocto</td>  <td>y</td>  <td>1e-24</td>  </tr>
	</table>

	For example to specify a unit of length in millimeters the units would be "mm".
	Units of area can be specified as "m^2", units of linear density can be specified
	as "kg/m".

	In addition to the SI units + prefix method, there are various units derived from
	these units with special names. The ones relavent to TexGen are described in the
	following table:

	<table border="1">
	<tr>  <td>Unit Name</td>  <td>Abbr.</td>  <td>Base Units</td>  </tr>                    
	<tr>  <td>inch</td>  <td>in</td>  <td>2.54E-2 m</td>  </tr>
	<tr>  <td>yard</td>  <td>yd</td>  <td>9.144E-1 m</td>  </tr>
	<tr>  <td>foot</td>  <td>ft</td>  <td>3.048E-1 m</td>  </tr>
	<tr>  <td>mile</td>  <td>mi</td>  <td>1.609344E3 m</td>  </tr>
	<tr>  <td>tex</td>  <td></td>  <td>g/km</td>  </tr>
	<tr>  <td>denier</td>  <td></td>  <td>g / 9 km</td>  </tr>
	</table>

	For more details read "units/units.html" in the TexGen source tree or download
	it from http://sourceforge.net/projects/units/. Note that tex and denier
	where added for TexGen and won't be found in that document.
	*/


	class CLASS_DECLSPEC CProperties
	{
	public:
		CProperties();
		CProperties(TiXmlElement &Element);
		virtual ~CProperties(void);

		/// Used for saving data to XML
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		/// Set the fibre linear density, i.e. the mass per unit length of fibre
		/**
		Suppose 1 meter length of yarn weighs X kg, it's linear density is X kg/m
		*/
		void SetYarnLinearDensity(double dValue, string Units = "kg/m");

		/// Set the fibre density, i.e. the mass per unit volume of fibre
		void SetFibreDensity(double dValue, string Units = "kg/m^3");

		/// Set the fibre diameter
		void SetFibreDiameter(double dValue, string Units = "m");

		/// Set the number of fibres in a yarn
		void SetFibresPerYarn(int iValue);

		/// Set the Young's modulus for a yarn
		void SetYoungsModulus( double dValue, string Units = "MPa"); // Set x,y,z equal.  Left in for backward compatibility
		void SetYoungsModulusX( double dValue, string Units = "MPa");
		void SetYoungsModulusY( double dValue, string Units = "MPa");
		void SetYoungsModulusZ( double dValue, string Units = "MPa");

		/// Set the Shear modulus for  a yarn
		void SetShearModulusXY( double dValue, string Units = "MPa");
		void SetShearModulusXZ( double dValue, string Units = "MPa");
		void SetShearModulusYZ( double dValue, string Units = "MPa");

		/// Set the Poisson's ratio for a yarn
		void SetPoissonsRatio( double dValue );  // Set x,y,z equal.  Left in for backward compatibility
		void SetPoissonsRatioX( double dValue );
		void SetPoissonsRatioY( double dValue );
		void SetPoissonsRatioZ( double dValue );

		/// Set the coefficients of thermal expansion for a yarn
		void SetAlphaX( double dValue, string Units = "/K");
		void SetAlphaY( double dValue, string Units = "/K");
		void SetAlphaZ( double dValue, string Units = "/K");

		/// Set the area occupied by fibres in a yarn cross-section
		/**
		If not set it can be calculated from the yarn linear density and fibre density.
		*/
		void SetFibreArea(double dValue, string Units = "m^2");

		void ResetYarnLinearDensity() { m_YarnLinearDensity.Reset(); }
		void ResetFibreDensity() { m_FibreDensity.Reset(); }
		void ResetFibreDiameter() { m_FibreDiameter.Reset(); }
		void ResetFibresPerYarn() { m_iFibresPerYarn = 0; }
		void ResetFibreArea() { m_FibreArea.Reset(); }
		void ResetYoungsModulusX() { m_YoungsModulusX.Reset(); }
		void ResetYoungsModulusY() { m_YoungsModulusY.Reset(); }
		void ResetYoungsModulusZ() { m_YoungsModulusZ.Reset(); }
		void ResetShearModulusXY() { m_ShearModulusXY.Reset(); }
		void ResetShearModulusXZ() { m_ShearModulusXZ.Reset(); }
		void ResetShearModulusYZ() { m_ShearModulusYZ.Reset(); }
		void ResetAlphaX() { m_AlphaX.Reset(); }
		void ResetAlphaY() { m_AlphaY.Reset(); }
		void ResetAlphaZ() { m_AlphaZ.Reset(); }
		void ResetPoissonsRatioX() { m_PoissonsRatioX = 0.0; }
		void ResetPoissonsRatioY() { m_PoissonsRatioY = 0.0; }
		void ResetPoissonsRatioZ() { m_PoissonsRatioZ = 0.0; }
		

		double GetYarnLinearDensity(string Units = "kg/m") const;
		double GetFibreDensity(string Units = "kg/m^3") const;
		double GetFibreDiameter(string Units = "m") const;
		int GetFibresPerYarn() const;
		/// Get the area occupied by fibres given fibre diameter and number of fibres
		/**
		Either returns the value stored in m_dFibreArea or calculates it from 
		m_iFibresPerYarn and m_FibreDiameter if they are set OR from
		m_dYarnLinearDensity and m_dFibreDensity if they are set.
		*/
		double GetFibreArea(string Units = "m^2") const;
		double GetYoungsModulusX( string Units = "MPa") const;
		double GetYoungsModulusY( string Units = "MPa") const;
		double GetYoungsModulusZ( string Units = "MPa") const;
		double GetShearModulusXY( string Units = "MPa") const;
		double GetShearModulusXZ( string Units = "MPa") const;
		double GetShearModulusYZ( string Units = "MPa") const;
		double GetAlphaX( string Units = "/K") const;
		double GetAlphaY( string Units = "/K") const;
		double GetAlphaZ( string Units = "/K") const;

		double GetPoissonsRatioX() const;
		double GetPoissonsRatioY() const;
		double GetPoissonsRatioZ() const;

		CProperty m_YarnLinearDensity;
		CProperty m_FibreDensity;
		CProperty m_FibreArea;
		CProperty m_FibreDiameter;
		CProperty m_YoungsModulusX;
		CProperty m_YoungsModulusY;
		CProperty m_YoungsModulusZ;
		CProperty m_ShearModulusXY;
		CProperty m_ShearModulusXZ;
		CProperty m_ShearModulusYZ;
		CProperty m_AlphaX;
		CProperty m_AlphaY;
		CProperty m_AlphaZ;
		CProperty m_CalculatedArea;

	protected:
		int m_iFibresPerYarn;
		double m_PoissonsRatioX;
		double m_PoissonsRatioY;
		double m_PoissonsRatioZ;

		/// Set default properties as specified by DefaultProperties.h
		void SetDefaultProperties();
	};
};	// namespace TexGen



















