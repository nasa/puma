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

#pragma once

#include "Mesh.h"

namespace TexGen
{ 
	using namespace std;
	
//	class CYarn;
	class CTextile;

	/// Class for meshing fabric unit cells
	class CLASS_DECLSPEC CBasicVolumes
	{
	public:
		CBasicVolumes(void);
		~CBasicVolumes(void);

		bool CreateBasicVolumes(CTextile &Textile);
		bool CreateBasicVolumes(string TextileName);

		void SetSeed(double dSeed) { m_dSeed = dSeed; }
		double GetSeed() { return m_dSeed; }

		void SetPeriodic(bool bPeriodic) { m_bCreatePeriodic = bPeriodic; }
		bool GetPeriodic() { return m_bCreatePeriodic; }

		void SetDebug(bool bDebug) { m_bDebug = bDebug; }
		bool GetDebug() { return m_bDebug; }

	protected:
		enum LINE_STATE
		{
			FORWARD = 1,
			REVERSE = 2,
		};

		/// Struct representing a region projected onto the XY plane
		struct PROJECTED_REGION
		{
			vector<int> SegmentIndices;
			vector<int> YarnIndices;
			vector<int> ContourNodes;
			XYZ Center;
			double dArea;
			PROJECTED_REGION():dArea(0){}
		};

		/// Functor defining the < operator based on the projected region's area
		/**
		This is used for sorting the projected regions by area.
		*/
		struct ProjectedRegionArea
		{
			bool operator()(const PROJECTED_REGION& left, const PROJECTED_REGION& right)
			{
				return left.dArea < right.dArea;
			}
		};

		void SaveProjectedContoursToVTK(string Filename);
		void SaveProjectedAreasToVTK(string Filename);
		bool ValidProjectedMesh();
		int SplitLinesByNodes(CMesh &Mesh);
		int SplitLinesByLines(CMesh &Mesh);
		int MergeStraightLines(CMesh &Mesh);
		int RemoveDuplicateSegments(CMesh &Mesh);
		int RemoveDegenerateSegments(CMesh &Mesh);
		CMesh GetProjectedMesh(const CMesh &Mesh);
		bool GetCommonEdgeIndices(int Indices1[3], int Indices2[3], int Common[2]);
		bool CreateProjectedAreas();
		bool SeedOuterBoundary();
		bool RemoveOuterBoundary();
		double GetRegionArea(const PROJECTED_REGION &Region);
		bool CreateProjectedCenters();
		bool PointInsideRegion(XYZ Point, int iRegion);
		void CalculateYarnIndices();
		bool MeshProjectedAreas();
		/**
		Find the intersections between a line passing through Point and parallel to the Z axis
		and the given mesh, then return the lowest and highest of those intersections.
		\param Mesh The mesh with which to perform the intersection test
		\param Point The point through which the line passes through (the z component is redundant)
		\param dMinZ The returned lowest intersection point
		\param dMaxZ The returned highest intersection point
		\param bForceFind Force the algorithm to find at least one intersection, what this means is that
						  it will find the intersection with the closest triangle and use that intersection
						  as the maximum and minimum returned bounds. This is usefull for cases where the
						  vertical line just misses the mesh and the closest intersection is needed.
		*/
		bool GetMeshVerticalBounds(const CMesh &Mesh, XYZ Point, double &dMinZ, double &dMaxZ, bool bForceFind = false);

		CMesh m_ProjectedMesh;
		CMesh m_DomainMesh;
		vector<CMesh> m_YarnMeshes;
		vector<PROJECTED_REGION> m_ProjectedRegions;
//		vector<int> m_TriangleNeighbors;

		// A map of triangle index to their region number
		vector<int> m_TriangleRegions;
		double m_dTolerance;
		CTextile* m_pTextile;
		double m_dSeed;
		bool m_bCreatePeriodic;
		bool m_bDebug;
	};

};	// namespace TexGen













