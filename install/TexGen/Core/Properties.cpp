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
#include "Properties.h"
#include "DefaultProperties.h"
using namespace TexGen;

CProperties::CProperties()
: m_YarnLinearDensity("kg/m")
, m_FibreDensity("kg/m^3")
, m_FibreArea("m^2")
, m_FibreDiameter("m")
, m_YoungsModulusX("MPa")
, m_YoungsModulusY("MPa")
, m_YoungsModulusZ("MPa")
, m_ShearModulusXY("MPa")
, m_ShearModulusXZ("MPa")
, m_ShearModulusYZ("MPa")
, m_AlphaX("/K")
, m_AlphaY("/K")
, m_AlphaZ("/K")
, m_iFibresPerYarn(0)
, m_CalculatedArea("m^2")
{
	SetDefaultProperties();
	//m_PoissonsRatioX = m_PoissonsRatioY = m_PoissonsRatioZ = 0.1;
}

CProperties::CProperties(TiXmlElement &Element)
: m_YarnLinearDensity("kg/m")
, m_FibreDensity("kg/m^3")
, m_FibreArea("m^2")
, m_FibreDiameter("m")
, m_YoungsModulusX("MPa")
, m_YoungsModulusY("MPa")
, m_YoungsModulusZ("MPa")
, m_ShearModulusXY("MPa")
, m_ShearModulusXZ("MPa")
, m_ShearModulusYZ("MPa")
, m_AlphaX("/K")
, m_AlphaY("/K")
, m_AlphaZ("/K")
, m_iFibresPerYarn(0)
, m_CalculatedArea("m^2")
{
	m_YarnLinearDensity.ReadAttribute(Element, "YarnLinearDensity");
	m_FibreDensity.ReadAttribute(Element, "FibreDensity");
	m_FibreArea.ReadAttribute(Element, "FibreArea");
	m_FibreDiameter.ReadAttribute(Element, "FibreDiameter");
	Element.Attribute("FibresPerYarn", &m_iFibresPerYarn);
	m_YoungsModulusX.ReadAttribute(Element, "YoungsModulus");  // Leave X as YoungsModulus so compatible with older versions
	m_YoungsModulusY.ReadAttribute(Element, "YoungsModulusY");
	m_YoungsModulusZ.ReadAttribute(Element, "YoungsModulusZ");
	Element.Attribute("PoissonsRatio", &m_PoissonsRatioX);
	Element.Attribute("PoissonsRatioY", &m_PoissonsRatioY);
	Element.Attribute("PoissonsRatioZ", &m_PoissonsRatioZ);
	m_ShearModulusXY.ReadAttribute(Element, "ShearModulusXY");
	m_ShearModulusXZ.ReadAttribute(Element, "ShearModulusXZ");
	m_ShearModulusYZ.ReadAttribute(Element, "ShearModulusYZ");
	m_AlphaX.ReadAttribute(Element, "AlphaX");
	m_AlphaY.ReadAttribute(Element, "AlphaY");
	m_AlphaZ.ReadAttribute(Element, "AlphaZ");
}

CProperties::~CProperties(void)
{
}

void CProperties::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	m_YarnLinearDensity.WriteAttribute(Element, "YarnLinearDensity");
	m_FibreDensity.WriteAttribute(Element, "FibreDensity");
	m_FibreArea.WriteAttribute(Element, "FibreArea");
	m_FibreDiameter.WriteAttribute(Element, "FibreDiameter");
	if (m_iFibresPerYarn)
		Element.SetAttribute("FibresPerYarn", m_iFibresPerYarn);
	m_YoungsModulusX.WriteAttribute(Element, "YoungsModulus");
	m_YoungsModulusY.WriteAttribute(Element, "YoungsModulusY");
	m_YoungsModulusZ.WriteAttribute(Element, "YoungsModulusZ");
	
	if (m_PoissonsRatioX != 0.0 )
	{
		Element.SetAttribute("PoissonsRatio", stringify(m_PoissonsRatioX));
	}
	if ( m_PoissonsRatioY != 0.0 )
	{
		Element.SetAttribute("PoissonsRatioY", stringify(m_PoissonsRatioY));
	}
	if ( m_PoissonsRatioZ != 0.0 )
	{
		Element.SetAttribute("PoissonsRatioZ", stringify(m_PoissonsRatioZ));
	}
	
	m_ShearModulusXY.WriteAttribute(Element, "ShearModulusXY");
	m_ShearModulusXZ.WriteAttribute(Element, "ShearModulusXZ");
	m_ShearModulusYZ.WriteAttribute(Element, "ShearModulusYZ");

	m_AlphaX.WriteAttribute(Element, "AlphaX");
	m_AlphaY.WriteAttribute(Element, "AlphaY");
	m_AlphaZ.WriteAttribute(Element, "AlphaZ");
}

void CProperties::SetYarnLinearDensity(double dValue, string Units)
{
	m_YarnLinearDensity.SetValue(dValue, Units);
	if ( m_FibreDensity.IsSet() )
	{
		double dFibreArea = m_YarnLinearDensity.GetSIValue()/m_FibreDensity.GetSIValue();
		m_CalculatedArea.SetValue(dFibreArea, "m^2");
	}
}

void CProperties::SetFibreDensity(double dValue, string Units)
{
	m_FibreDensity.SetValue(dValue, Units);
	if ( m_YarnLinearDensity.IsSet() )
	{
		double dFibreArea = m_YarnLinearDensity.GetSIValue()/m_FibreDensity.GetSIValue();
		m_CalculatedArea.SetValue(dFibreArea, "m^2");
	}
}

void CProperties::SetFibreArea(double dValue, string Units)
{
	m_FibreArea.SetValue(dValue, Units);
}

void CProperties::SetFibreDiameter(double dValue, string Units)
{
	m_FibreDiameter.SetValue(dValue, Units);
	if ( m_iFibresPerYarn )
	{
		double dRadius = 0.5*m_FibreDiameter.GetSIValue();
		double dFibreArea = PI * dRadius * dRadius * m_iFibresPerYarn;
		m_CalculatedArea.SetValue(dFibreArea, "m^2");
	}
}

void CProperties::SetFibresPerYarn(int iValue)
{
	m_iFibresPerYarn = iValue;
	if ( m_iFibresPerYarn && m_FibreDiameter.IsSet() )
	{
		double dRadius = 0.5*m_FibreDiameter.GetSIValue();
		double dFibreArea = PI * dRadius * dRadius * m_iFibresPerYarn;
		m_CalculatedArea.SetValue(dFibreArea, "m^2");
	}
}

void CProperties::SetYoungsModulus(double dValue, string Units)
{
	SetYoungsModulusX( dValue, Units );
	SetYoungsModulusY( dValue, Units );
	SetYoungsModulusZ( dValue, Units );
}

void CProperties::SetYoungsModulusX(double dValue, string Units)
{
	m_YoungsModulusX.SetValue(dValue, Units);
}

void CProperties::SetYoungsModulusY(double dValue, string Units)
{
	m_YoungsModulusY.SetValue(dValue, Units);
}

void CProperties::SetYoungsModulusZ(double dValue, string Units)
{
	m_YoungsModulusZ.SetValue(dValue, Units);
}

void CProperties::SetShearModulusXY(double dValue, string Units)
{
	m_ShearModulusXY.SetValue(dValue, Units);
}

void CProperties::SetShearModulusXZ(double dValue, string Units)
{
	m_ShearModulusXZ.SetValue(dValue, Units);
}

void CProperties::SetShearModulusYZ(double dValue, string Units)
{
	m_ShearModulusYZ.SetValue(dValue, Units);
}

void CProperties::SetAlphaX(double dValue, string Units)
{
	m_AlphaX.SetValue(dValue, Units);
}

void CProperties::SetAlphaY(double dValue, string Units)
{
	m_AlphaY.SetValue(dValue, Units);
}

void CProperties::SetAlphaZ(double dValue, string Units)
{
	m_AlphaZ.SetValue(dValue, Units);
}

void CProperties::SetPoissonsRatio(double dValue)
{
	SetPoissonsRatioX( dValue );
	SetPoissonsRatioY( dValue );
	SetPoissonsRatioZ( dValue );
}

void CProperties::SetPoissonsRatioX(double dValue)
{
	m_PoissonsRatioX = dValue;
}

void CProperties::SetPoissonsRatioY(double dValue)
{
	m_PoissonsRatioY = dValue;
}

void CProperties::SetPoissonsRatioZ(double dValue)
{
	m_PoissonsRatioZ = dValue;
}

double CProperties::GetYarnLinearDensity(string Units) const
{
	return m_YarnLinearDensity.GetValue(Units);
}

double CProperties::GetFibreDensity(string Units) const
{
	return m_FibreDensity.GetValue(Units);
}

double CProperties::GetFibreArea(string Units) const
{
	if (m_FibreArea.IsSet())
		return m_FibreArea.GetValue(Units);
	else if (m_iFibresPerYarn && m_FibreDiameter.IsSet())
	{
		if ( m_CalculatedArea.IsSet() )
			return m_CalculatedArea.GetValue(Units);
		else
		{
			double dRadius = 0.5*m_FibreDiameter.GetSIValue();
			double dFibreArea = PI * dRadius * dRadius * m_iFibresPerYarn;
			return ConvertUnits(dFibreArea, "m^2", Units);
		}
	}
	else if (m_YarnLinearDensity.IsSet() && m_FibreDensity.IsSet())
	{
		if ( m_CalculatedArea.IsSet() )
			return m_CalculatedArea.GetValue(Units);
		else
		{
			double dFibreArea = m_YarnLinearDensity.GetSIValue()/m_FibreDensity.GetSIValue();
			return ConvertUnits(dFibreArea, "m^2", Units);
		}
	}
	return 0;
}

double CProperties::GetFibreDiameter(string Units) const
{
	return m_FibreDiameter.GetValue(Units);
}

int CProperties::GetFibresPerYarn() const
{
	return m_iFibresPerYarn;
}

double CProperties::GetYoungsModulusX( string Units) const
{
	return m_YoungsModulusX.GetValue(Units);
}

double CProperties::GetYoungsModulusY( string Units) const
{
	return m_YoungsModulusY.GetValue(Units);
}

double CProperties::GetYoungsModulusZ( string Units) const
{
	return m_YoungsModulusZ.GetValue(Units);
}

double CProperties::GetShearModulusXY( string Units) const
{
	return m_ShearModulusXY.GetValue(Units);
}

double CProperties::GetShearModulusXZ( string Units) const
{
	return m_ShearModulusXZ.GetValue(Units);
}

double CProperties::GetShearModulusYZ( string Units) const
{
	return m_ShearModulusYZ.GetValue(Units);
}

double CProperties::GetAlphaX( string Units) const
{
	return m_AlphaX.GetValue(Units);
}

double CProperties::GetAlphaY( string Units) const
{
	return m_AlphaY.GetValue(Units);
}

double CProperties::GetAlphaZ( string Units) const
{
	return m_AlphaZ.GetValue(Units);
}

double CProperties::GetPoissonsRatioX() const
{
	return m_PoissonsRatioX;
}

double CProperties::GetPoissonsRatioY() const
{
	return m_PoissonsRatioY;
}

double CProperties::GetPoissonsRatioZ() const
{
	return m_PoissonsRatioZ;
}

void CProperties::SetDefaultProperties()
{
	SetYoungsModulusX( YARN_EX );
	SetYoungsModulusY( YARN_EY );
	SetYoungsModulusZ( YARN_EZ );
		
	SetShearModulusXY( YARN_GXY );
	SetShearModulusXZ( YARN_GXZ );
	SetShearModulusYZ( YARN_GYZ );
	
	SetPoissonsRatioX( YARN_POISSONX );
	SetPoissonsRatioY( YARN_POISSONY );
	SetPoissonsRatioZ( YARN_POISSONZ );
		
	SetAlphaX( YARN_ALPHAX );
	SetAlphaY( YARN_ALPHAY );
	SetAlphaZ( YARN_ALPHAZ );
}












