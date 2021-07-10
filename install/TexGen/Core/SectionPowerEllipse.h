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

	/// Power ellipse section
	/**
	Power ellipse is equivalent to an ellipse where the y coordinate has a power
	applied to it before scaling to given height. With a power lower than 1 the
	section resembles a rectangle with rounded edges, with a greater than 1
	the section resembles a lenticular section. When power = 1 the section is an ellipse
	The X offset moves the maximum height to the given offset from the centre
	*/
	class CLASS_DECLSPEC CSectionPowerEllipse : public CSection
	{
	public:
		CSectionPowerEllipse(double dWidth, double dHeight, double dPower, double dXOffset=0.0);
		CSectionPowerEllipse(TiXmlElement &Element);
		~CSectionPowerEllipse(void);

		bool operator == (const CSection &CompareMe) const;
		CSection* Copy() const { return new CSectionPowerEllipse(*this); }

		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		string GetType() const { return "CSectionPowerEllipse"; }
		string GetDefaultName() const;

		XY GetPoint(double t) const;
		double GetTangent( double t) const;

		// Accessor methods
		double GetWidth() const { return m_dWidth; }
		double GetHeight() const { return m_dHeight; }
		double GetPower() const { return m_dPower; }
		double GetXOffset() const { return m_dXOffset; }
		void SetWidth( double dWidth ){ m_dWidth = dWidth; }
		void SetHeight( double dHeight ){ m_dHeight = dHeight; }
		void SetPower( double dPower ){ m_dPower = dPower; }

	protected:
		double m_dWidth, m_dHeight, m_dPower, m_dXOffset;
	};
};	// namespace TexGen
