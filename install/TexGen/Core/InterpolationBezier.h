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
#include "Interpolation.h"

namespace TexGen
{
	using namespace std;

	/// Bezier interpolation for yarn paths
	class CLASS_DECLSPEC CInterpolationBezier : public CInterpolation
	{
	public:
		CInterpolationBezier(bool bPeriodic = true, bool bForceInPlaneTangent = false, bool bForceMasterNodeTangent = false);
		CInterpolationBezier(TiXmlElement &Element);
		~CInterpolationBezier(void);

		CInterpolation* Copy() const { return new CInterpolationBezier(*this); }
		string GetType() const { return "CInterpolationBezier"; }
//		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		/// Calculate the node tangents (use node tangents of they exist) and store them in m_Tangents
		void Initialise(const vector<CNode> &MasterNodes) const;

		/// Get a node from parametric function where t is specified
		CSlaveNode GetNode(const vector<CNode> &MasterNodes, int iIndex, double t) const;

	protected:
		mutable vector<XYZ> m_Tangents;
	};

};	// namespace TexGen
