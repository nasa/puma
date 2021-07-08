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
#include "Section.h"
#include "TexGen.h"
#include <limits>

using namespace TexGen;
CSection::CSection()
: m_bEquiSpaced(false)
{
	AssignDefaults();
}

CSection::~CSection(void)
{
}

CSection::CSection(TiXmlElement &Element)
{
	AssignDefaults();
	m_bEquiSpaced = valueify<bool>(Element.Attribute("EquiSpaced"));
	FOR_EACH_TIXMLELEMENT(pEdgePoint, Element, "EdgePoint")
	{
		m_EdgePoints.push_back(valueify<XY>(pEdgePoint->Attribute("value")));
	}
	TiXmlElement *pSectionMesh = Element.FirstChildElement("SectionMesh");
	if (pSectionMesh)
	{
		m_pSectionMesh = CSectionMesh::CreateSectionMesh(*pSectionMesh);
	}
}

void CSection::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	Element.SetAttribute("type", GetType());
	if (m_pSectionMesh)
	{
		TiXmlElement SectionMesh("SectionMesh");
		m_pSectionMesh->PopulateTiXmlElement(SectionMesh, OutputType);
		Element.InsertEndChild(SectionMesh);
	}
	if (OutputType == OUTPUT_FULL)
	{
		Element.SetAttribute("EquiSpaced", stringify(m_bEquiSpaced));
		vector<XY>::const_iterator itPoint;
		for (itPoint = m_EdgePoints.begin(); itPoint != m_EdgePoints.end(); ++itPoint)
		{
			TiXmlElement EdgePoint("EdgePoint");
			EdgePoint.SetAttribute("value", stringify(*itPoint));
			Element.InsertEndChild(EdgePoint);
		}
	}
}

void CSection::AssignDefaults()
{
	m_pSectionMesh = CSectionMeshRectangular();
}

CObjectContainer<CSection> CSection::CreateSection(TiXmlElement &Element)
{
	const string* pType = Element.Attribute(string("type"));
	if (pType)
	{
		if (*pType == "CSectionBezier")
			return CSectionBezier(Element);
		else if (*pType == "CSectionEllipse")
			return CSectionEllipse(Element);
		else if (*pType == "CSectionHybrid")
			return CSectionHybrid(Element);
		else if (*pType == "CSectionLenticular")
			return CSectionLenticular(Element);
		else if (*pType == "CSectionPolygon")
			return CSectionPolygon(Element);
		else if (*pType == "CSectionPowerEllipse")
			return CSectionPowerEllipse(Element);
		else if (*pType == "CSectionRotated")
			return CSectionRotated(Element);
		else if (*pType == "CSectionScaled")
			return CSectionScaled(Element);
		else if (*pType == "CSectionRectangle")
			return CSectionRectangle(Element);
	}
	// If the section was not recognised for some reason, revert to using an ellipse
	// to avoid further problems
	return CSectionEllipse(1, 1);
}

const CMesh &CSection::GetMesh(int iNumPoints, bool bEquiSpaced) const
{
	if (!m_pSectionMesh)
	{
		TGERROR("Unable to get section mesh, section has no mesh assigned to it: " + GetDefaultName());
		static CMesh EmptyMesh;
		return EmptyMesh;
	}
	const vector<XY> &Section = GetPoints(iNumPoints, bEquiSpaced);
	return m_pSectionMesh->GetMesh(Section);
}

const vector<XY> &CSection::GetPoints(int iNumPoints, bool bEquiSpaced) const
{
	if (iNumPoints != (int)m_EdgePoints.size() || bEquiSpaced != m_bEquiSpaced)
	{
		if (!bEquiSpaced || !CreateEquiSpacedSection(iNumPoints))
			CreateSection(iNumPoints);
	}
	return m_EdgePoints;
}

void CSection::CreateSection(int iNumPoints) const
{
	m_bEquiSpaced = false;
	m_EdgePoints.clear();
	XY Point;
	int i;
	double t;
	for (i=0; i<iNumPoints; ++i)
	{
		t = (double)i/(double)iNumPoints;
		m_EdgePoints.push_back(GetPoint(t));
	}
}

bool CSection::CreateEquiSpacedSection(int iNumPoints) const
{
	m_bEquiSpaced = true;
	int i, j;
	int iMaxIterations = 1000;
	double dStdDevTolerance = 1E-12;
	double t;
	double dLength;
	double dStdDev;
	double dAvgLength;
	int IncreaseStdDevCount = 0;
	double PrevStdDev = 0.0;
	double MinDouble = std::numeric_limits<double>::min();
	vector<double> dLengths;
	dLengths.resize(iNumPoints);

	m_EdgePoints.resize(iNumPoints);
	// The spacing vector stores the increment in t between nodes.
	// Initially the increment in t values between all nodes should be equal.
	// Note that the sum of t increments in the Spacing vector must equal 1.
	vector<double> Spacing;
	t = 1/double(iNumPoints);
	Spacing.resize(iNumPoints, t);
	
	for (j=0; j<iMaxIterations; ++j)
	{
		// Get the section using the spacing vector
		for (i=0, t=0; i<iNumPoints; t+=Spacing[i], ++i)
		{
			m_EdgePoints[i]=GetPoint(t);
		}
		// Find the average distance between section points
		dAvgLength = 0;
		for (i=0; i<iNumPoints; ++i)
		{
			dLengths[i] = GetLength(m_EdgePoints[i], m_EdgePoints[(i+1)%iNumPoints]);
			if ( dLengths[i] < MinDouble )
			//if ( dLengths[i] < 10 * DBL_MIN )
			{
				TGERROR("Unable to create equispaced points, Length = " + stringify(dLength) );
				return false;
			}
			dAvgLength += dLengths[i];
		}
		dAvgLength /= iNumPoints;
		// Calculate the standard deviation of distance between nodes
		// And adjust the spacing vector such that the nodes become
		// more evenly spaced
		dStdDev = 0;
		for (i=0; i<iNumPoints; ++i)
		{	
			dStdDev += (dLengths[i]-dAvgLength)*(dLengths[i]-dAvgLength);
			Spacing[i] *= dAvgLength/dLengths[i];
		}
		dStdDev /= iNumPoints;
		if (dStdDev > PrevStdDev )
			++IncreaseStdDevCount;
		PrevStdDev = dStdDev;
		// If the standard deviation is low enough, our points are evenly spaced enough
		if (dStdDev < dStdDevTolerance)
		{
			return true;
		}
		if ( IncreaseStdDevCount > 200 )  // Iteration switched direction too many times - not reaching answer
		{
			TGERROR("Unable to create equispaced section points, iteration unstable: " + GetDefaultName());
			return false;
		}
		// Calculate the total value of t in the spacing vector
		// and adjust all values so that the sum equals 1
		t = 0;
		for (i=0; i<iNumPoints; ++i)
		{
			t += Spacing[i];
		}
		for (i=0; i<iNumPoints; ++i)
		{
			Spacing[i] /= t;
		}
	}
	TGERROR("Unable to create equispaced section points, maximum iterations exceeded: " + GetDefaultName());
	return false;
}

void CSection::AssignSectionMesh(const CSectionMesh &SectionMesh)
{
	m_pSectionMesh = SectionMesh;
}

void CSection::SetSectionMeshLayers( int iNum )
{
	if ( !m_pSectionMesh )
		return;
	m_pSectionMesh->SetNumLayers( iNum );
}

int CSection::GetSectionMeshLayers()
{
	if ( !m_pSectionMesh )
		return -1;
	int iNumLayers = -1;
	
	string type = m_pSectionMesh->GetType();
	if ( type == "CSectionMeshRectangular" )
	{
		CSectionMeshRectangular* sectionMesh = (CSectionMeshRectangular*)m_pSectionMesh->Copy();
		iNumLayers = sectionMesh->GetNumLayers();
		delete sectionMesh;
	}
	else if ( type == "CSectionMeshRectangleSection" )
	{
		CSectionMeshRectangleSection* sectionMesh = (CSectionMeshRectangleSection*)m_pSectionMesh->Copy();
		iNumLayers = sectionMesh->GetNumLayers();
		delete sectionMesh;
	}
	return iNumLayers;
}

double CSection::GetArea(const vector<XY> &Section)
{
	double dArea = 0;
	int i;
	for (i=0; i<(int)Section.size(); ++i)
	{
		dArea += Section[i].x*Section[(i+1)%Section.size()].y - Section[(i+1)%Section.size()].x*Section[i].y;
	}
	dArea /= 2;
	return dArea;
}

double CSection::GetCircumference(const vector<XY> &Section)
{
	int i;
	double dCircumference = 0;
	for (i=0; i<(int)Section.size(); ++i)
	{
		dCircumference += ::GetLength(Section[i], Section[(i+1)%Section.size()]);
	}
	return dCircumference;
}

/*
double CSection::GetCircumference()
{
	int i;
	const int iDivisions = 1000;
	double t, dCircumference = 0;
	XY CurPos, PrevPos;
	PrevPos = GetPoint(0);
	for (i=0; i<iDivisions; ++i)
	{
		t = (double)(i+1)/(double)(iDivisions);
		CurPos = GetPoint(t);
		dCircumference += ::GetLength(PrevPos, CurPos);
		PrevPos = CurPos;
	}
	return dCircumference;
}
*/







