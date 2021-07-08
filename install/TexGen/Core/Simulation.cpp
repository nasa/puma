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

#include "PrecompiledHeaders.h"
#include "Simulation.h"

using namespace TexGen;

CSimulation::CSimulation(void)
{
}

CSimulation::~CSimulation(void)
{
}

const CLinearTransformation &CSimulation::GetDeformation() const
{
	if (!m_DeformationSteps.empty())
		return m_DeformationSteps.back();
	static CLinearTransformation Identity;
	return Identity;
}

void CSimulation::SetDeformation(const CLinearTransformation &Deformation)
{
	ClearDeformationSteps();
	AddDeformationStep(Deformation);
}

void CSimulation::ClearDeformationSteps()
{
	m_DeformationSteps.clear();
}

void CSimulation::AddDeformationStep(const CLinearTransformation &Deformation)
{
	m_DeformationSteps.push_back(Deformation);
}




