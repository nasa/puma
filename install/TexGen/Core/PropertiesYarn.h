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
	class CLASS_DECLSPEC CPropertiesYarn : public CProperties
	{
	public:
		CPropertiesYarn();
		CPropertiesYarn(TiXmlElement &Element);
		~CPropertiesYarn(void);

		/// Used for saving data to XML
		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

	protected:
	};
};	// namespace TexGen



















