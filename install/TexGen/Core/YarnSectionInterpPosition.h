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
#include "YarnSectionInterp.h"

namespace TexGen
{ 
	using namespace std;

	/// Interpolate sections between arbritrary positions along the length of the yarn
	/**
	Sections should be added to the class by use of the AddSection function.
	*/
	class CLASS_DECLSPEC CYarnSectionInterpPosition : public CYarnSectionInterp
	{
	public:
		/**
		\param bRamped, bPolar  Specify how points are interpolated between master nodes using
			CYarnSectionInterp::InterpolatePoints()
			If bRamped is set to true, the interpolation fraction given to functions in GetInterpedSection
			and GetInterpedSectionMesh will be ramped using a cubic equation. Where du = 0 at u = 0, and du = 0
			at u = 1. This provides a smoother transition between sections when the interpolation begins and ends.
		\param bConstMesh Defines whether all section meshes are forced to have same configuration
			ie number of rows and columns in mesh
		*/
		CYarnSectionInterpPosition(bool bRamped = true, bool bPolar = false, bool bConstMesh = true);
		CYarnSectionInterpPosition(TiXmlElement &Element);
		~CYarnSectionInterpPosition(void);

		vector<XY> GetSection(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced = false) const;
		CMesh GetSectionMesh(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced) const;
		CYarnSection* Copy() const { return new CYarnSectionInterpPosition(*this); }
		string GetType() const { return "CYarnSectionInterpPosition"; }
		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		/// Add a section at a specific point along the path of the yarn
		/**
		Positions in between specified sections will be interpolated. If no section
		is specified at the start and end then the sections are applied periodically.
		\param dPosition Represents the position along the length of the yarn
			where the section is to be applied, this value should vary between 0 and 1
		\param Section The section to be applied at that position
		*/
		void AddSection(double dPosition, const CSection &Section);

		/// Function to set the number of layers equal for all sections in a given yarn
		void SetSectionMeshLayersEqual( int iNumPoints ) const;

		// Accessor methods
		int GetNumNodeSections() const { return (int)m_Sections.size(); }
		const CSection &GetSection(int iIndex) const;
		double GetSectionPosition(int iIndex) const;

	protected:
		vector<pair<double, CObjectContainer<CSection> > > m_Sections;
	};

};	// namespace TexGen
