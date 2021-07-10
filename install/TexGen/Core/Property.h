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

	class CLASS_DECLSPEC CProperty
	{
	public:
		CProperty(string SIUnits);

		void WriteAttribute(TiXmlElement &Element, string AttributeName);
		void ReadAttribute(TiXmlElement &Element, string AttributeName);

		string GetString() const;
		double GetValue(string Units) const;
		double GetSIValue() const;
		void SetValue(double dValue, string Units);
		void SetValue( double dValue );
		void SetUnits( string Units );
		void Reset();

		double GetValue() const;
		string GetUnits() const;
		string GetSIUnits() const;
		bool IsSet() const;

	protected:
		double m_dValue;
		string m_Units;
		string m_SIUnits;
		bool m_bSet;
	};
};	// namespace TexGen








