/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2019 Louise Brown

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
#include "TexGen.h"
#include "SurfaceMesh.h"

using namespace TexGen;
using namespace std;

CSurfaceMesh::CSurfaceMesh(double Seed, bool bFillEnds ) : CMeshDomainPlane(Seed, bFillEnds)
{
}

CSurfaceMesh::~CSurfaceMesh(void)
{
}

void CSurfaceMesh::SaveSurfaceMesh(CTextile &Textile, bool bSaveYarns, bool bSaveDomain, bool bTrimToDomain)
{
	XYZ P;

	if (!Textile.AddSurfaceToMesh(m_Mesh, m_DomainMeshes, bTrimToDomain))
	{
		TGERROR("Error creating surface mesh.");
		return;
	}
	m_Mesh.ConvertQuadstoTriangles(true);

	if (!bSaveYarns)
		m_Mesh.Clear();

	if (bSaveDomain)
	{
		MeshDomainPlanes(true);
		vector<CMesh>::iterator itTriangulatedMeshes;
		for (itTriangulatedMeshes = m_TriangulatedMeshes.begin(); itTriangulatedMeshes != m_TriangulatedMeshes.end(); ++itTriangulatedMeshes)
		{
			m_Mesh.InsertMesh(*itTriangulatedMeshes);
		}
	}
}

void CSurfaceMesh::SaveToSTL(string Filename, bool bBinary)
{
	m_Mesh.SaveToSTL( Filename, bBinary );
}

void CSurfaceMesh::SaveToVTK( string Filename )
{
	m_Mesh.SaveToVTK(Filename);
}

void CSurfaceMesh::SaveToSCIRun(string Filename)
{
	m_Mesh.SaveToSCIRun(Filename);
}
