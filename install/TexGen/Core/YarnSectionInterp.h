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
#include "YarnSection.h"

namespace TexGen
{
	using namespace std;

	class CSection;

	/// Abstract base class to handle interpolation for derived classes
	class CLASS_DECLSPEC CYarnSectionInterp : public CYarnSection
	{
	public:
		/**
		If bRamped is set to true, the interpolation fraction given to functions in GetInterpedSection
		and GetInterpedSectionMesh will be ramped using a cubic equation. Where du = 0 at u = 0, and du = 0
		at u = 1. This provides a smoother transition between sections when the interpolation begins and ends.
		\param bConstMesh Defines whether all section meshes are forced to have same configuration
			ie number of rows and columns in mesh
		*/
		CYarnSectionInterp(bool bRamped, bool bPolar, bool bConstMesh = true);
		CYarnSectionInterp(TiXmlElement &Element);
		virtual ~CYarnSectionInterp(void);

		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		// Accessor methods
		bool GetRamped() { return m_bRamped; }
		bool GetPolar() { return m_bPolar; }
		
		

	protected:
		bool GetInterpedSection(const CSection &Section1, const CSection &Section2, double u, int iNumPoints, bool bEquiSpaced, vector<XY> &Points) const;
		bool GetInterpedSectionMesh(const CSection &Section1, const CSection &Section2, double u, int iNumPoints, bool bEquiSpaced, CMesh &Mesh) const;
		XY InterpolatePoints(XY P1, XY P2, double u) const;
		int CalculateNumberofLayers(const vector<XY> &Section) const;

		bool m_bRamped, m_bPolar;//, m_bConstMesh;
	};

};	// namespace TexGen
