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

#include "TextileDeformerVolumeMesh.h"

namespace TexGen
{ 
	using namespace std;

	/// Represents a textile cell containing yarns
	class CLASS_DECLSPEC CSimulation
	{
	public:
		CSimulation(void);
		virtual ~CSimulation(void);

		/// Clear all deformation steps
		void ClearDeformationSteps();

		/// Add a deformation step
		void AddDeformationStep(const CLinearTransformation &Deformation);

		/// Get the list of deformation steps
		const vector<CLinearTransformation> &GetDeformationSteps() const { return m_DeformationSteps; }

		/// Set the list of deformation steps
		void SetDeformationSteps(const vector<CLinearTransformation> &DeformationSteps) { m_DeformationSteps = DeformationSteps; }

		/// Return the final deformation in the series or identity if none exist
		/**
		This function is useful if the simulation consists of a single deformation
		step otherwise use GetDeformationSteps.
		*/
		const CLinearTransformation &GetDeformation() const;

		/// Set the deformation, this will clear all existing deformations
		/**
		This function is useful if the simulation consists of a single deformation
		step otherwise use AddDeformationStep or SetDeformationSteps.
		*/
		void SetDeformation(const CLinearTransformation &Deformation);

	protected:
		vector<CLinearTransformation> m_DeformationSteps;
	};
};	// namespace TexGen


















