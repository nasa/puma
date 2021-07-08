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
#include "FibreDistributionConst.h"
#include "Section.h"

using namespace TexGen;

CFibreDistributionConst::CFibreDistributionConst()
{
}

CFibreDistributionConst::CFibreDistributionConst(TiXmlElement &Element)
: CFibreDistribution(Element)
{
}

void CFibreDistributionConst::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CFibreDistribution::PopulateTiXmlElement(Element, OutputType);
}

double CFibreDistributionConst::IntegrateDistribution(const vector<XY> &Section) const
{
	return CSection::GetArea(Section);
}

double CFibreDistributionConst::GetVolumeFraction(const vector<XY> &Section, double dFibreArea, XY Location, int YarnIndex) const
{
	double dArea = IntegrateDistribution(Section);
	double dVf = dFibreArea/dArea;
	if (dVf>0.86||dVf<0)
	{
		if ( YarnIndex == -1)
		{
			TGERROR("Warning: Volume fraction is not realistic " << dVf);
		}
		else
		{
			TGERROR("Warning: Volume fraction is not realistic: " << dVf << ", Yarn: " << YarnIndex);
		}
	}
	return dVf;
}

double CFibreDistributionConst::GetVolumeFraction(double dArea, double dFibreArea, int YarnIndex) const
{
	double dVf = dFibreArea/dArea;
	if (dVf>0.86||dVf<0)
	{
		if ( YarnIndex == -1)
		{
			TGERROR("Warning: Volume fraction is not realistic " << dVf);
		}
		else
		{
			TGERROR("Warning: Volume fraction is not realistic: " << dVf << ", Yarn: " << YarnIndex);
		}
	}
	return dVf;
}

