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
#include "Domain.h"
#include "TexGen.h"
#include "Yarn.h"

using namespace TexGen;
CDomain::CDomain(void)
{
}

CDomain::~CDomain(void)
{
}

CDomain::CDomain(TiXmlElement &Element)
{
	TiXmlElement* pMesh = Element.FirstChildElement("Mesh");
	if (pMesh)
	{
		m_Mesh = CMesh(*pMesh);
	}
}

void CDomain::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	Element.SetAttribute("type", GetType());
	if (OutputType == OUTPUT_FULL)
	{
		TiXmlElement Mesh("Mesh");
		m_Mesh.PopulateTiXmlElement(Mesh, OutputType);
		Element.InsertEndChild(Mesh);
	}
}

vector<pair<int, int> > CDomain::ConvertLimitsToInt(const vector<pair<double, double> > &RepeatLimits)
{
	vector<pair<int, int> > IntRepeatLimits;
	vector<pair<double, double> >::const_iterator itRepeatLimits;
	for (itRepeatLimits = RepeatLimits.begin(); itRepeatLimits != RepeatLimits.end(); ++itRepeatLimits)
	{
		IntRepeatLimits.push_back(pair<int, int>((int)ceil(itRepeatLimits->first), (int)floor(itRepeatLimits->second)));
	}
	return IntRepeatLimits;
}

double CDomain::GetVolume() const
{
	return m_Mesh.CalculateVolume();
}

