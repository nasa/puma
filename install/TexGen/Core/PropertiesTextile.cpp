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
#include "PropertiesTextile.h"
#include "DefaultProperties.h"
using namespace TexGen;


CPropertiesTextile::CPropertiesTextile()
: m_ArealDensity("kg/m^2")
,m_GeometryScale("mm")
, m_MatrixYoungsModulus("MPa")
, m_MatrixAlpha(MATRIX_ALPHA)
, m_MatrixPoissonsRatio(0.1)
{
	SetDefaultMatrixProperties();
}

CPropertiesTextile::CPropertiesTextile(TiXmlElement &Element)
: CProperties(Element), m_ArealDensity("kg/m^2"),  m_GeometryScale("mm")
, m_MatrixYoungsModulus("MPa")
, m_MatrixAlpha(MATRIX_ALPHA)
, m_MatrixPoissonsRatio(0.1)
{
	const string* pGeomScale = Element.Attribute(string("GeometryScale"));
	if (pGeomScale)
		m_GeometryScale = *pGeomScale;
	m_ArealDensity.WriteAttribute(Element, "ArealDensity");
	m_MatrixYoungsModulus.ReadAttribute(Element, "MatrixYoungsModulus");
	Element.Attribute("MatrixPoissonsRatio", &m_MatrixPoissonsRatio);
	Element.Attribute("MatrixAlpha", &m_MatrixAlpha);
}

CPropertiesTextile::~CPropertiesTextile(void)
{
}

void CPropertiesTextile::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CProperties::PopulateTiXmlElement(Element, OutputType);

	Element.SetAttribute("GeometryScale", m_GeometryScale);
	m_ArealDensity.ReadAttribute(Element, "ArealDensity");
	m_MatrixYoungsModulus.WriteAttribute(Element, "MatrixYoungsModulus");
	if (m_MatrixPoissonsRatio != 0.0 )
		Element.SetAttribute("MatrixPoissonsRatio", stringify(m_MatrixPoissonsRatio));
	if (m_MatrixAlpha != 0.0 )
		Element.SetAttribute("MatrixAlpha", stringify(m_MatrixAlpha));
}

bool CPropertiesTextile::SetGeometryScale(string Units)
{
	// Check the units are of the same type
	if (CompatibleUnits("m", Units))
	{
		m_GeometryScale = Units;
		return true;
	}
	else
	{
		TGERROR("Unable to set geometry scale, units must represent a length!");
		return false;
	}
}

void CPropertiesTextile::SetArealDensity(double dValue, string Units)
{
	m_ArealDensity.SetValue(dValue, Units);
}

string CPropertiesTextile::GetGeometryScale() const
{
	return m_GeometryScale;
}

double CPropertiesTextile::GetArealDensity(string Units) const
{
	return m_ArealDensity.GetValue(Units);
}

void CPropertiesTextile::SetMatrixYoungsModulus(double dValue, string Units)
{
	m_MatrixYoungsModulus.SetValue(dValue, Units);
}

void CPropertiesTextile::SetMatrixPoissonsRatio(double dValue)
{
	m_MatrixPoissonsRatio = dValue;
}

void CPropertiesTextile::SetMatrixAlpha(double dValue)
{
	m_MatrixAlpha = dValue;
}

double CPropertiesTextile::GetMatrixYoungsModulus(string Units) const
{
	return m_MatrixYoungsModulus.GetValue(Units);
}

double CPropertiesTextile::GetMatrixPoissonsRatio() const
{
	return m_MatrixPoissonsRatio;
}

double CPropertiesTextile::GetMatrixAlpha() const
{
	return m_MatrixAlpha;
}

void CPropertiesTextile::SetDefaultMatrixProperties()
{
	SetMatrixYoungsModulus( MATRIX_E );	
	SetMatrixPoissonsRatio( MATRIX_POISSON );
	SetMatrixAlpha( MATRIX_ALPHA );
}













