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
#include "TextileDeformerVolumeMesh.h"
#include "TexGen.h"

using namespace TexGen;

CTextileDeformerVolumeMesh::CTextileDeformerVolumeMesh(void)
{
}

CTextileDeformerVolumeMesh::~CTextileDeformerVolumeMesh(void)
{
}

bool CTextileDeformerVolumeMesh::SetYarnMeshDisplacements(int iYarn, const CMesh &Mesh, vector<XYZ> &Displacements)
{
	if (Mesh.GetNodes().size() != Displacements.size())
	{
		TGERROR("Cannot set yarn mesh displacements" 
				",the number of nodal displacements (" << Displacements.size() <<
				") does not equal the number of nodes (" << Mesh.GetNodes().size() << ")");
		return false;
	}
	if (iYarn < 0)
	{
		TGERROR("Cannot set yarn mesh displacements, yarn index given is invalid: " << iYarn);
		return false;
	}
	m_YarnMeshes.resize(iYarn+1);
	m_YarnMeshes[iYarn].Mesh = Mesh;
	m_YarnMeshes[iYarn].NodeDisplacements = Displacements;
	// The GetDisplacement function only works with a Tet mesh so let's
	// convert to a tet mesh now
	m_YarnMeshes[iYarn].Mesh.ConvertToTetMesh();
	return true;
}

double CTextileDeformerVolumeMesh::GetDisplacement(XYZ Pos, int iYarn, XYZ &Disp) const
{
	if (iYarn < 0 || iYarn >= (int)m_YarnMeshes.size())
		return false;
	const CMesh &Mesh = m_YarnMeshes[iYarn].Mesh;
	const vector<XYZ> &NodeDisplacements = m_YarnMeshes[iYarn].NodeDisplacements;
	list<int>::const_iterator itIndex;
	const list<int> &Indices = Mesh.GetIndices(CMesh::TET);
	int i1, i2, i3, i4;
	XYZ N1, N2, N3, N4;
	double a, b, c, d, dMin;
	double dAccuracy = -1;
	bool bFirst = true;
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		i1 = *(itIndex++);
		i2 = *(itIndex++);
		i3 = *(itIndex++);
		i4 = *(itIndex++);
		N1 = Mesh.GetNode(i1);
		N2 = Mesh.GetNode(i2);
		N3 = Mesh.GetNode(i3);
		N4 = Mesh.GetNode(i4);
		GetBarycentricCoordinates(Pos, N1, N2, N3, N4, a, b, c, d);
		dMin = min(a, b);
		dMin = min(dMin, c);
		dMin = min(dMin, d);
		if (bFirst || dMin > dAccuracy)
		{
			// If all barycentric coordinates are non-negative the point lies inside the Tet
			dAccuracy = dMin;
			Disp = a*NodeDisplacements[i1];
			Disp += b*NodeDisplacements[i2];
			Disp += c*NodeDisplacements[i3];
			Disp += d*NodeDisplacements[i4];
			bFirst = false;
		}
	}
	return dAccuracy;
}

void CTextileDeformerVolumeMesh::GetBarycentricCoordinates(const XYZ &P, const XYZ &P1, const XYZ &P2, const XYZ &P3, const XYZ &P4, double &a, double &b, double &c, double &d) const
{
	// Code from Maple generated from:
	/*
	eqn1 := x = a*x1 + b*x2 + c*x3 + d*x4;
	eqn2 := y = a*y1 + b*y2 + c*y3 + d*y4;
	eqn3 := z = a*z1 + b*z2 + c*z3 + d*z4;
	eqn4 := a + b + c + d = 1;
	sol := solve({eqn1, eqn2, eqn3, eqn4}, [a,b,c,d]);
	with(CodeGeneration):
	C(sol[1]);
	*/
	a = -(-P3.x * P.z * P2.y - P3.x * P4.z * P.y + P3.x * P2.z * P.y + P3.x * P4.z * P2.y + P4.y * P3.x * P.z - P4.y * P3.x * P2.z - P4.x * P3.y * P.z + P4.x * P3.y * P2.z - P3.z * P4.y * P.x - P3.z * P4.x * P2.y + P3.y * P4.z * P.x + P3.z * P4.x * P.y - P3.z * P2.x * P.y + P3.z * P4.y * P2.x + P4.z * P2.x * P.y - P4.x * P2.z * P.y - P2.z * P3.y * P.x + P2.z * P4.y * P.x - P4.z * P.x * P2.y - P3.y * P4.z * P2.x + P3.z * P.x * P2.y + P.z * P4.x * P2.y + P.z * P3.y * P2.x - P.z * P4.y * P2.x) / (P4.z * P1.x * P2.y + P1.x * P3.y * P2.z - P1.x * P3.y * P4.z - P1.x * P4.y * P2.z - P4.x * P3.y * P2.z + P3.x * P1.z * P2.y - P3.x * P1.z * P4.y - P3.x * P4.z * P2.y - P4.x * P1.z * P2.y - P4.z * P1.y * P2.x + P1.y * P3.x * P4.z + P1.y * P4.x * P2.z - P1.y * P3.x * P2.z + P4.y * P3.x * P2.z - P3.y * P1.z * P2.x + P3.y * P1.z * P4.x + P3.y * P4.z * P2.x + P4.y * P1.z * P2.x + P3.z * P1.y * P2.x - P3.z * P1.y * P4.x + P3.z * P1.x * P4.y - P3.z * P1.x * P2.y - P3.z * P4.y * P2.x + P3.z * P4.x * P2.y);
	b = -0.1e1 / (P4.z * P1.x * P2.y + P1.x * P3.y * P2.z - P1.x * P3.y * P4.z - P1.x * P4.y * P2.z - P4.x * P3.y * P2.z + P3.x * P1.z * P2.y - P3.x * P1.z * P4.y - P3.x * P4.z * P2.y - P4.x * P1.z * P2.y - P4.z * P1.y * P2.x + P1.y * P3.x * P4.z + P1.y * P4.x * P2.z - P1.y * P3.x * P2.z + P4.y * P3.x * P2.z - P3.y * P1.z * P2.x + P3.y * P1.z * P4.x + P3.y * P4.z * P2.x + P4.y * P1.z * P2.x + P3.z * P1.y * P2.x - P3.z * P1.y * P4.x + P3.z * P1.x * P4.y - P3.z * P1.x * P2.y - P3.z * P4.y * P2.x + P3.z * P4.x * P2.y) * (-P3.z * P1.x * P4.y - P3.z * P4.x * P.y + P4.z * P1.y * P.x + P1.x * P3.y * P4.z + P4.x * P3.y * P.z - P1.y * P3.x * P4.z + P3.x * P4.z * P.y + P3.z * P1.x * P.y + P1.x * P4.y * P.z - P4.z * P1.x * P.y - P1.x * P3.y * P.z - P3.x * P1.z * P.y + P4.x * P1.z * P.y + P1.y * P3.x * P.z + P3.x * P1.z * P4.y - P1.y * P4.x * P.z - P3.z * P1.y * P.x - P4.y * P3.x * P.z + P3.y * P1.z * P.x - P3.y * P1.z * P4.x - P3.y * P4.z * P.x - P4.y * P1.z * P.x + P3.z * P4.y * P.x + P3.z * P1.y * P4.x);
	c = (-P4.z * P1.x * P.y + P1.x * P4.y * P.z + P4.x * P1.z * P.y + P4.z * P1.y * P.x - P1.y * P4.x * P.z - P4.y * P1.z * P.x - P1.x * P4.y * P2.z + P4.z * P1.x * P2.y - P4.x * P1.z * P2.y - P.z * P4.y * P2.x + P4.z * P2.x * P.y - P4.x * P2.z * P.y - P4.z * P.x * P2.y + P.z * P4.x * P2.y + P2.z * P4.y * P.x - P2.x * P1.z * P.y + P4.y * P1.z * P2.x + P2.z * P1.x * P.y + P.x * P1.z * P2.y - P.z * P1.x * P2.y - P2.z * P1.y * P.x + P1.y * P4.x * P2.z + P.z * P1.y * P2.x - P4.z * P1.y * P2.x) / (P4.z * P1.x * P2.y + P1.x * P3.y * P2.z - P1.x * P3.y * P4.z - P1.x * P4.y * P2.z - P4.x * P3.y * P2.z + P3.x * P1.z * P2.y - P3.x * P1.z * P4.y - P3.x * P4.z * P2.y - P4.x * P1.z * P2.y - P4.z * P1.y * P2.x + P1.y * P3.x * P4.z + P1.y * P4.x * P2.z - P1.y * P3.x * P2.z + P4.y * P3.x * P2.z - P3.y * P1.z * P2.x + P3.y * P1.z * P4.x + P3.y * P4.z * P2.x + P4.y * P1.z * P2.x + P3.z * P1.y * P2.x - P3.z * P1.y * P4.x + P3.z * P1.x * P4.y - P3.z * P1.x * P2.y - P3.z * P4.y * P2.x + P3.z * P4.x * P2.y);
	d = -(-P3.z * P1.x * P.y - P3.z * P.x * P2.y + P1.x * P3.y * P.z + P3.x * P1.z * P.y - P1.y * P3.x * P.z + P2.z * P3.y * P.x + P3.z * P1.y * P.x - P.z * P3.y * P2.x - P3.y * P1.z * P.x - P1.x * P3.y * P2.z - P3.x * P1.z * P2.y + P1.y * P3.x * P2.z + P3.y * P1.z * P2.x - P3.z * P1.y * P2.x + P3.z * P1.x * P2.y - P3.x * P2.z * P.y + P3.x * P.z * P2.y + P3.z * P2.x * P.y - P2.x * P1.z * P.y + P2.z * P1.x * P.y + P.x * P1.z * P2.y - P2.z * P1.y * P.x + P.z * P1.y * P2.x - P.z * P1.x * P2.y) / (P4.z * P1.x * P2.y + P1.x * P3.y * P2.z - P1.x * P3.y * P4.z - P1.x * P4.y * P2.z - P4.x * P3.y * P2.z + P3.x * P1.z * P2.y - P3.x * P1.z * P4.y - P3.x * P4.z * P2.y - P4.x * P1.z * P2.y - P4.z * P1.y * P2.x + P1.y * P3.x * P4.z + P1.y * P4.x * P2.z - P1.y * P3.x * P2.z + P4.y * P3.x * P2.z - P3.y * P1.z * P2.x + P3.y * P1.z * P4.x + P3.y * P4.z * P2.x + P4.y * P1.z * P2.x + P3.z * P1.y * P2.x - P3.z * P1.y * P4.x + P3.z * P1.x * P4.y - P3.z * P1.x * P2.y - P3.z * P4.y * P2.x + P3.z * P4.x * P2.y);
}











