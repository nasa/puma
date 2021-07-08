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

#include "Properties.h"

namespace TexGen
{ 
	using namespace std;

	/// Class to store properties related to a textile
	/**
	This class combines geometrical information stored in the CTextile class with user
	input data relating to physical properties of the textile. Several interesting
	values can be obtained in this manner, for example the volume fraction is calculated
	from the volume of yarns in the geometrical model along with information about fibre
	density and areal density.
	*/
	class CLASS_DECLSPEC CPropertiesTextile : public CProperties
	{
	public:
		CPropertiesTextile();
		CPropertiesTextile(TiXmlElement &Element);
		~CPropertiesTextile(void);

		/// Used for saving data to XML
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		/// Set the scale of the geometry
		/**
		If 1 unit represents 1 mm in real world units then the geometry scale should be set to Units::mm.
		By default the geometry scale is set to millimetres.
		*/
		bool SetGeometryScale(string Units);

		/// Set the areal density, i.e. the mass per unit area of fabric
		void SetArealDensity(double dValue, string Units = "kg/m^2");

		void ResetArealDensity() { m_ArealDensity.Reset(); }
		/// Set the Young's modulus for matrix
		void SetMatrixYoungsModulus( double dValue, string Units = "MPa");

		/// Set the Poisson's ration for matrix
		void SetMatrixPoissonsRatio( double dValue );

		/// Set the coefficient of thermal expansion for the matrix
		void SetMatrixAlpha( double dValue );

		string GetGeometryScale() const;
		double GetArealDensity(string Units = "kg/m^2") const;

		double GetMatrixYoungsModulus(string Units = "MPa") const;
		double GetMatrixPoissonsRatio() const;
		double GetMatrixAlpha() const;

		CProperty m_ArealDensity;
		CProperty m_MatrixYoungsModulus;
		//CProperty m_MatrixAlpha;

	protected:
		string m_GeometryScale;
		double m_MatrixPoissonsRatio;
		double m_MatrixAlpha;

		/// Set default matrix properties as specified by DefaultProperties.h
		void SetDefaultMatrixProperties();
	};
};	// namespace TexGen



















