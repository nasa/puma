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

#pragma once

#include "Mesh.h"
#include "tetgen.h"

namespace TexGen
{
	/** Class which creates a triangulated mesh of a domain plane with the outlines

	*/

	using namespace std;

	/// Structure which contains information for transformation from 3D to 2D plane
	struct PLANEPARAMS {
		XYZ XAxis;
		XYZ YAxis;
		XYZ RefPoint;
		XYZ Normal;
	};

	class CLASS_DECLSPEC CMeshDomainPlane
	{
	public:
		CMeshDomainPlane(double Seed, bool bFillEnds=true);
		virtual ~CMeshDomainPlane(void);
		
		void MeshDomainPlanes(bool bPeriodic);

	protected:
		/// Triangulate the domain faces
		bool Triangulate(vector< vector<XY> > &PolygonPoints, vector<XY> &HolePoints, CMesh& OutputMesh, PLANEPARAMS& ConvertRef);

		/// Convert points on one domain surface to local 2D points
		bool ConvertDomainPointsTo2D(const list<int> &QuadIndices, CMesh& DomainMesh, vector<XY>& Points2D, PLANEPARAMS& ConvertRef);
		/// Convert local 2D coordinates to global 3D coordinates
		void Convert2DTo3DCoordinates(vector<XY>& Points2D, vector<XYZ>& Points3D, PLANEPARAMS& ConvertRef);
		/// Convert global 3D coordinates to local 2D coordinates
		void Convert3DTo2DCoordinates(vector<XYZ>& Points3D, PLANEPARAMS& ConvertRef, vector<XY>& Points2D);
		/// Calculates seed points along domain edge
		void SeedSides(vector<XY>& Points);
		/// Offsets points in mesh by given distance in direction of normal
		void OffsetMeshPoints(CMesh& Mesh, XYZ& Normal, double dDist);

		/// Vector of meshes used to store domain plane meshes
		vector<CMesh> m_DomainMeshes;
		/// Vector of triangulated domain plane meshes
		vector<CMesh> m_TriangulatedMeshes;
		/// Seed used for calculating boundary edge points
		double			m_Seed;
		/// Number of polygon vertices on each face. Number of outer vector members = number of faces
		vector<vector<int> > m_PolygonNumVertices;
		/// True if yarn areas are to be removed from domain
		bool m_bFillEnds;
	};
};  // namespace TexGen