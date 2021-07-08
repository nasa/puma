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

	/// Hybrid of any number of other sections
	class CLASS_DECLSPEC CSectionHybrid : public CSection
	{
	public:
		/// Empty hybrid section, parts can be added with the AddDivision and AssignSection functions
		CSectionHybrid(void);
		/// Hybrid of two sections split into the upper and lower half
		CSectionHybrid(const CSection &TopHalf, const CSection &BottomHalf);
		/// Hybrid of four sections split into four quadrants
		CSectionHybrid(const CSection &TopRight, const CSection &TopLeft, const CSection &BottomLeft, const CSection &BottomRight);
		CSectionHybrid(TiXmlElement &Element);
		~CSectionHybrid(void);

		bool operator == (const CSection &CompareMe) const;
		CSection* Copy() const { return new CSectionHybrid(*this); }

		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		string GetType() const { return "CSectionHybrid"; }
		string GetDefaultName() const;

		/// Add a division where the sections will transfer from one to the other
		void AddDivision(double dFraction);
		/// Assign a section between divisions
		bool AssignSection(int iIndex, const CSection &Section);

		// Accessor methods
		int GetNumDivisions() const { return (int)m_Divisions.size(); }
		int GetNumSections() { return (int)m_Sections.size(); }
		double GetDivision(int iIndex) const;
		const CSection &GetSection(int iIndex) const;
		XY GetPoint(double t) const;

	protected:
		vector<double> m_Divisions;
		vector<CObjectContainer<CSection> > m_Sections;
	};

};	// namespace TexGen







