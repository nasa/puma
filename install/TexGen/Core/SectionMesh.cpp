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
#include "SectionMesh.h"
#include "SectionMeshTriangulate.h"
#include "SectionMeshRectangular.h"
#include "SectionMeshRectangleSection.h"

using namespace TexGen;
CSectionMesh::CSectionMesh(void)
{
}

CSectionMesh::~CSectionMesh(void)
{
}

CSectionMesh::CSectionMesh(TiXmlElement &Element)
{
/*	TiXmlElement* pMesh = Element.FirstChildElement("Mesh");
	if (pMesh)
	{
		m_Mesh = CMesh(*pMesh);
	}*/
}

void CSectionMesh::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	Element.SetAttribute("type", GetType());
/*	TiXmlElement Mesh("Mesh");
	m_Mesh.PopulateTiXmlElement(Mesh, OutputType);
	Element.InsertEndChild(Mesh);*/
}

CObjectContainer<CSectionMesh> CSectionMesh::CreateSectionMesh(TiXmlElement &Element)
{
	const string* pType = Element.Attribute(string("type"));
	if (pType)
	{
		if (*pType == "CSectionMeshTriangulate")
			return CSectionMeshTriangulate(Element);
		else if (*pType == "CSectionMeshRectangular")
			return CSectionMeshRectangular(Element);
		else if (*pType == "CSectionMeshRectangleSection")
			return CSectionMeshRectangleSection(Element);
	}
	return CObjectContainer<CSectionMesh>();
}

bool CSectionMesh::CreateMeshIfNeeded(const vector<XY> &Section) const
{
	if (Section != m_Section)
	{
		m_Section = Section;
		return CreateMesh(Section);
	}
	return true;
}

const CMesh &CSectionMesh::GetMesh(const vector<XY> &Section) const
{
	CreateMeshIfNeeded(Section);
	return m_Mesh;
}



