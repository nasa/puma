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
#include "SlaveNode.h"
using namespace TexGen;

CSlaveNode::CSlaveNode(XYZ Position, XYZ Tangent, XYZ Up)
:  CNode(Position, Tangent, Up)
,m_T(0)
,m_iIndex(0)
,m_2DSectionMesh(NULL)
,m_SectionMesh(NULL)
{
}

CSlaveNode::~CSlaveNode(void)
{
	if ( m_2DSectionMesh != NULL )
		delete m_2DSectionMesh;
	if ( m_SectionMesh != NULL )
		delete m_SectionMesh;
}

CSlaveNode::CSlaveNode(TiXmlElement &Element)
: CNode(Element)
,m_2DSectionMesh(NULL)
,m_SectionMesh(NULL)
{
	Element.Attribute("T", &m_T);
	Element.Attribute("Index", &m_iIndex);
	FOR_EACH_TIXMLELEMENT(pSectionPoint2D, Element, "SectionPoint2D")
	{
		m_2DSectionPoints.push_back(valueify<XY>(pSectionPoint2D->Attribute("value")));
	}
	FOR_EACH_TIXMLELEMENT(pSectionPoint, Element, "SectionPoint")
	{
		m_SectionPoints.push_back(valueify<XYZ>(pSectionPoint->Attribute("value")));
	}
	/*TiXmlElement *pSectionMesh2D = Element.FirstChildElement("SectionMesh2D");
	if (pSectionMesh2D)
	{
		m_2DSectionMesh = CMesh(*pSectionMesh2D);
	}
	TiXmlElement *pSectionMesh = Element.FirstChildElement("SectionMesh");
	if (pSectionMesh2D)
	{
		m_SectionMesh = CMesh(*pSectionMesh);
	}*/
}

void CSlaveNode::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CNode::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("T", stringify(m_T));
	Element.SetAttribute("Index", m_iIndex);
	vector<XY>::const_iterator itXY;
	for (itXY = m_2DSectionPoints.begin(); itXY != m_2DSectionPoints.end(); ++itXY)
	{
		TiXmlElement SectionPoint2D("SectionPoint2D");
		SectionPoint2D.SetAttribute("value", stringify(*itXY));
		Element.InsertEndChild(SectionPoint2D);
	}
	vector<XYZ>::const_iterator itXYZ;
	for (itXYZ = m_SectionPoints.begin(); itXYZ != m_SectionPoints.end(); ++itXYZ)
	{
		TiXmlElement SectionPoint("SectionPoint");
		SectionPoint.SetAttribute("value", stringify(*itXYZ));
		Element.InsertEndChild(SectionPoint);
	}
	/*TiXmlElement SectionMesh2D("SectionMesh2D");
	m_2DSectionMesh.PopulateTiXmlElement(SectionMesh2D, OutputType);
	Element.InsertEndChild(SectionMesh2D);
	TiXmlElement SectionMesh("SectionMesh");
	m_SectionMesh.PopulateTiXmlElement(SectionMesh, OutputType);
	Element.InsertEndChild(SectionMesh);*/
}

void CSlaveNode::UpdateSectionPoints(const vector<XY> *p2DSectionPoints)
{
	if (p2DSectionPoints)
		m_2DSectionPoints = *p2DSectionPoints;
	// Clear any existing section before creating the new one
	m_SectionPoints.clear();

	vector<XY>::iterator it2DSectionPoint;
	XYZ Pos;
	XYZ Side = GetSide();
	for (it2DSectionPoint = m_2DSectionPoints.begin(); it2DSectionPoint != m_2DSectionPoints.end(); ++it2DSectionPoint)
	{
		// Rotate the 2d section point to the global 3d coordinate system
		Pos = Side * it2DSectionPoint->x;
		Pos += m_Up * it2DSectionPoint->y;
		// Translate the point to its global position
		Pos += m_Position;
		// Add the new 3d point to the list
		m_SectionPoints.push_back(Pos);
	}
}

XYZ CSlaveNode::GetPointOnSection(const XY &p2DSectionPoint)
{
	vector<XY>::const_iterator it2DSectionPoint;
	XYZ Pos;
	XYZ Side = GetSide();
	
	// Rotate the 2d section point to the global 3d coordinate system
	Pos = Side * p2DSectionPoint.x;
	Pos += m_Up * p2DSectionPoint.y;
	// Translate the point to its global position
	Pos += m_Position;
	// Add the new 3d point to the list
	return Pos;
}

void CSlaveNode::UpdateSectionMesh(const CMesh *p2DSectionMesh)
{
	if (p2DSectionMesh)
	{
		if ( m_2DSectionMesh == NULL )
			m_2DSectionMesh = new CMesh;
		else
			m_2DSectionMesh->Clear();
		*m_2DSectionMesh = *p2DSectionMesh;
	}
	// Clear any existing section mesh before creating the new one
	if ( m_SectionMesh == NULL )
		m_SectionMesh = new CMesh;
	else
		m_SectionMesh->Clear();

	vector<XYZ>::iterator itNode;
	XYZ Pos;
	XYZ Side = GetSide();
	vector<XYZ> MeshNodes2D = m_2DSectionMesh->GetNodes();

	for (itNode = MeshNodes2D.begin(); itNode != MeshNodes2D.end(); ++itNode)
	{
		// Rotate the 2d section point to the global 3d coordinate system
		Pos = Side * itNode->x;
		Pos += m_Up * itNode->y;
		// Translate the point to its global position
		Pos += m_Position;
		m_SectionMesh->AddNode(Pos);
	}
	int i;
	for (i=0; i<CMesh::NUM_ELEMENT_TYPES; ++i)
	{
		m_SectionMesh->GetIndices((CMesh::ELEMENT_TYPE)i) = m_2DSectionMesh->GetIndices((CMesh::ELEMENT_TYPE)i);
	}
}

void CSlaveNode::Rotate(WXYZ Rotation)
{
	CNode::Rotate(Rotation);
	vector<XYZ>::iterator itSectionPoint;
	for (itSectionPoint = m_SectionPoints.begin(); itSectionPoint != m_SectionPoints.end(); ++itSectionPoint)
	{
		(*itSectionPoint) = Rotation * (*itSectionPoint);
	}
	m_SectionMesh->Rotate(Rotation);
}

void CSlaveNode::Translate(XYZ Vector)
{
	CNode::Translate(Vector);
	vector<XYZ>::iterator itSectionPoint;
	for (itSectionPoint = m_SectionPoints.begin(); itSectionPoint != m_SectionPoints.end(); ++itSectionPoint)
	{
		(*itSectionPoint) += Vector;
	}
	m_SectionMesh->Translate(Vector);
}












