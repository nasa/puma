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
#include "MeshDomainPlane.h"

extern "C"
{
#include "../Triangle/triangle.h"
#include "../Triangle/triangle_api.h"
}

using namespace TexGen;
using namespace std;

CMeshDomainPlane::CMeshDomainPlane(double Seed, bool bFillEnds)
{
	m_Seed = Seed;
	m_bFillEnds = bFillEnds;
}

CMeshDomainPlane::~CMeshDomainPlane(void)
{
}

void CMeshDomainPlane::MeshDomainPlanes( bool bPeriodic )
{
	// Add facets for domain planes
	vector<CMesh>::iterator itDomainMeshes;
	
	vector<CMesh>::iterator itTriangulatedMeshes;
	int NumEdgeTris = 0;
	int iNumNodes = CMesh::GetNumNodes(CMesh::TRI);
	vector<XYZ> DomainMeshNormals;
	int i;
	vector<PLANEPARAMS> PlaneParams;

	for (itDomainMeshes = m_DomainMeshes.begin(), i = 0; itDomainMeshes != m_DomainMeshes.end(); itDomainMeshes++, ++i)
	{
		const list<int> &QuadIndices = itDomainMeshes->GetIndices(CMesh::QUAD);
		const list<int> &PolygonIndices = itDomainMeshes->GetIndices(CMesh::POLYGON);
		list<int>::const_iterator itQuadIndices;
		list<int>::const_iterator itPolyIndices;
		vector<int> NumVertices;
		vector<XY> HolePoints;

		CMesh TriangleMesh;

		// Convert domain quad points to 2D points.  ConvertRef contains information to restore 2D points back to 3D on correct plane
		vector< vector<XY> > ArrayPoints2D;
		vector<XY> Points2D;
		PLANEPARAMS ConvertRef;
		ConvertDomainPointsTo2D(QuadIndices, *itDomainMeshes, Points2D, ConvertRef);
		PlaneParams.push_back(ConvertRef);
		ArrayPoints2D.push_back(Points2D);

		// Save number of indices in each polygon (each polygon represents intersection of yarn with domain plane)
		if (PolygonIndices.size() > 0)
		{
			for (itPolyIndices = PolygonIndices.begin(); itPolyIndices != PolygonIndices.end(); )
			{
				int Num = 0;
				int Start = *itPolyIndices;
				do
				{
					++Num;
					++itPolyIndices;
				} while ((itPolyIndices != PolygonIndices.end()) && ((*itPolyIndices) != Start));

				if ((*itPolyIndices) != Start)  // Reached end and not found complete polygon
				{
					TGERROR("Error creating intersection of yarn with domain");
					return;
				}
				++Num;
				NumVertices.push_back(Num);
				++itPolyIndices;
			}
			m_PolygonNumVertices.push_back(NumVertices);
		}
		// Add yarn end polygons to 2D point array
		int Poly = 0;
		for (itPolyIndices = PolygonIndices.begin(); itPolyIndices != PolygonIndices.end(); )
		{
			vector<XYZ> Points3D;
			for (int iNode = 0; iNode < NumVertices[Poly] - 1; ++iNode)
			{
				XYZ Point = itDomainMeshes->GetNode(*(itPolyIndices++));
				Points3D.push_back(Point);
			}
			Points2D.clear();
			Convert3DTo2DCoordinates(Points3D, ConvertRef, Points2D);
			ArrayPoints2D.push_back(Points2D);

			// Set up point inside polygon to be seed point if hole
			XY HolePoint = (Points2D[0] + Points2D[(int)Points2D.size()/2])/2.0;
			HolePoints.push_back(HolePoint);

			itPolyIndices++;
			Poly++;
		}

		int j;
		if (bPeriodic)
		{
			for (j = 0; j < i; ++j)
			{
				// If planes normals are equal and opposite (ie opposite faces of box) replicate mesh on second plane
				if (GetLength((ConvertRef.Normal + PlaneParams[j].Normal)) < 0.000000001)
				{
					double dDist = DotProduct(PlaneParams[j].Normal, (PlaneParams[j].RefPoint - ConvertRef.RefPoint));
					TriangleMesh = m_TriangulatedMeshes[j];
					OffsetMeshPoints(TriangleMesh, ConvertRef.Normal, dDist);
					break;
				}
			}
			if (j == i) // If not already triangulated plane, do so now
			{
				vector<XY> SeededSides;
				SeedSides(ArrayPoints2D[0]);
				Triangulate(ArrayPoints2D, HolePoints, TriangleMesh, ConvertRef);
			}
			NumEdgeTris += (int)TriangleMesh.GetIndices(CMesh::TRI).size() / iNumNodes;
			m_TriangulatedMeshes.push_back(TriangleMesh);
		}

	}

}

bool CMeshDomainPlane::ConvertDomainPointsTo2D(const list<int> &QuadIndices, CMesh& DomainMesh, vector<XY>& Points2D, PLANEPARAMS& ConvertRef)
{
	list<int>::const_iterator itQuadIndices;

	for (itQuadIndices = QuadIndices.begin(); itQuadIndices != QuadIndices.end(); )
	{
		int numNodes = CMesh::GetNumNodes(CMesh::QUAD);
		vector<XYZ> Points3D;
		for (int iNode = 0; iNode < numNodes; ++iNode)
		{
			XYZ Point = DomainMesh.GetNode(*(itQuadIndices++));
			Points3D.push_back(Point);
		}
		if (Points3D.size() < 3)
			return false;
		ConvertRef.RefPoint = Points3D[0];
		ConvertRef.XAxis = Points3D[1] - ConvertRef.RefPoint;
		ConvertRef.Normal = CrossProduct(ConvertRef.XAxis, (Points3D[2] - ConvertRef.RefPoint));
		ConvertRef.YAxis = CrossProduct(ConvertRef.Normal, ConvertRef.XAxis);
		ConvertRef.XAxis = Normalise(ConvertRef.XAxis);
		ConvertRef.YAxis = Normalise(ConvertRef.YAxis);
		ConvertRef.Normal = Normalise(ConvertRef.Normal);

		Convert3DTo2DCoordinates(Points3D, ConvertRef, Points2D);
	}
	return true;
}

void CMeshDomainPlane::Convert3DTo2DCoordinates(vector<XYZ>& Points3D, PLANEPARAMS& ConvertRef, vector<XY>& Points2D)
{
	vector<XYZ>::iterator itPoints3D;

	for (itPoints3D = Points3D.begin(); itPoints3D != Points3D.end(); ++itPoints3D)
	{
		XYZ Relative = *itPoints3D - ConvertRef.RefPoint;
		XY Point2D;
		Point2D.x = DotProduct(ConvertRef.XAxis, Relative);
		Point2D.y = DotProduct(ConvertRef.YAxis, Relative);
		Points2D.push_back(Point2D);
	}
}

void CMeshDomainPlane::OffsetMeshPoints(CMesh& Mesh, XYZ& Normal, double dDist)
{
	int iNumNodes = Mesh.GetNumNodes();
	for (int i = 0; i < iNumNodes; ++i)
	{
		XYZ Point = Mesh.GetNode(i);
		Point += Normal * dDist;
		Mesh.SetNode(i, Point);
	}
}

void CMeshDomainPlane::SeedSides(vector<XY>& Points)
{
	vector<XY> SeededSides;
	vector<XY>::iterator itPoints;

	XY StartPoint;
	for (itPoints = Points.begin(); itPoints != Points.end(); )
	{

		if (itPoints == Points.begin())
		{
			StartPoint = *itPoints;
		}
		XY P1 = *(itPoints++);
		XY P2;
		if (itPoints != Points.end())
		{
			P2 = *itPoints;
		}
		else
		{
			P2 = StartPoint;
		}
		SeededSides.push_back(P1);
		double dLength = GetLength(P1, P2);
		int iNumSplits = int(floor(dLength / m_Seed));
		for (int i = 1; i < iNumSplits; ++i)
		{
			double u = double(i) / double(iNumSplits);
			XY Point = P1 + (P2 - P1)*u;
			SeededSides.push_back(Point);
		}
	}
	Points.clear();
	Points.insert(Points.begin(), SeededSides.begin(), SeededSides.end());
}

bool CMeshDomainPlane::Triangulate(vector<vector<XY> > &PolygonPoints, vector<XY> &HolePoints, CMesh& OutputMesh, PLANEPARAMS& ConvertRef)
{
	//	char szSwitches[128];
	stringstream Switches;

	double dMaxArea = 0.5*m_Seed*m_Seed;
	double dMinAngle = 20;

	// These are the switches to be used with triangle
	// http://www-2.cs.cmu.edu/~quake/triangle.switch.html
	// -p Triangulates a Planar Straight Line Graph (.poly file).
	// -z Numbers all items starting from zero (rather than one).
	// -n Outputs (to a .neigh file) a list of triangles neighboring each triangle.
	// -P Suppresses the output .poly file.
	// -B Suppresses boundary markers in the output .node, .poly, and .edge output files.
	// -A Assigns a regional attribute to each triangle that identifies what segment-bounded region it belongs to.
	// -Q Quiet: Suppresses all explanation of what Triangle is doing, unless an error occurs.
	// -q Quality mesh generation with no angles smaller than 20 degrees. An alternate minimum angle may be specified after the `q'.
	// -a Imposes a maximum triangle area constraint. A fixed area constraint (that applies to every triangle) may be specified after the `a', or varying area constraints may be read from a .poly file or .area file.
	// -Y Prohibits the insertion of Steiner points on the mesh boundary

	/*#ifdef _DEBUG
	sprintf(szSwitches, "pzAPBq%fa%f", dMinAngle, dMaxArea);
	#else // _DEBUG
	sprintf(szSwitches, "pzAQPBq%fa%f", dMinAngle, dMaxArea);
	#endif // _DEBUG*/
#ifndef _DEBUG
	Switches << "Q";
#endif
	// Triangle has trouble parsing values given in scientific format so use fixed format with a
	// rediculously high precision to get around the problem
	Switches << "pzAPBq" << setiosflags(ios::fixed) << setprecision(20) << dMinAngle << "a" << dMaxArea;
	Switches << "YY";

	triangleio TriangleInput;
	triangleio TriangleOutput;

	context *ctx;
	ctx = triangle_context_create();

	triangle_context_options(ctx, (char*)Switches.str().c_str());

	memset(&TriangleInput, 0, sizeof(TriangleInput));
	memset(&TriangleOutput, 0, sizeof(TriangleOutput));

	// Input Nodes
	vector<vector<XY> >::iterator itArrayPolygonPoints;
	int iNumPoints = 0;
	for (itArrayPolygonPoints = PolygonPoints.begin(); itArrayPolygonPoints != PolygonPoints.end(); ++itArrayPolygonPoints)
	{
		iNumPoints += (int)(*itArrayPolygonPoints).size();
	}
	TriangleInput.pointlist = new REAL[iNumPoints * 2];
	TriangleInput.numberofpoints = iNumPoints;

	TriangleInput.segmentlist = new int[iNumPoints * 2];
	TriangleInput.numberofsegments = iNumPoints;

	int i = 0;
	for (itArrayPolygonPoints = PolygonPoints.begin(); itArrayPolygonPoints != PolygonPoints.end(); ++itArrayPolygonPoints)
	{
		vector<XY>::iterator itPolyPoints;
		int j = 0;
		int iNumPolyPoints = (int)(*itArrayPolygonPoints).size();
		int StartIndex = i;
		for (itPolyPoints = (*itArrayPolygonPoints).begin(); itPolyPoints != (*itArrayPolygonPoints).end(); ++itPolyPoints)
		{
			// Input nodes
			TriangleInput.pointlist[i * 2] = itPolyPoints->x;
			TriangleInput.pointlist[i * 2 + 1] = itPolyPoints->y;

			// Input Segments
			TriangleInput.segmentlist[i * 2] = i;
			if (j < iNumPolyPoints - 1)
			{
				TriangleInput.segmentlist[i * 2 + 1] = i + 1;
			}
			else
			{
				TriangleInput.segmentlist[i * 2 + 1] = StartIndex;
			}

			++i;
			++j;
		}
	}

	if (!m_bFillEnds)
	{
		// Input hole points
		vector<XY>::iterator itHolePoints;
		int iNumHoles = (int)HolePoints.size();
		TriangleInput.holelist = new REAL[iNumHoles * 2];
		TriangleInput.numberofholes = iNumHoles;
		for (itHolePoints = HolePoints.begin(), i = 0; itHolePoints != HolePoints.end(); ++itHolePoints, ++i)
		{
			TriangleInput.holelist[i * 2] = itHolePoints->x;
			TriangleInput.holelist[i * 2 + 1] = itHolePoints->y;
		}
	}

	// Input regions
	/*	TriangleInput.regionlist = new REAL [m_ProjectedRegions.size()*4];
	TriangleInput.numberofregions = m_ProjectedRegions.size();

	for (i=0; i<TriangleInput.numberofregions; ++i)
	{
	TriangleInput.regionlist[i*4] = m_ProjectedRegions[i].Center.x;
	TriangleInput.regionlist[i*4+1] = m_ProjectedRegions[i].Center.y;
	TriangleInput.regionlist[i*4+2] = i;
	TriangleInput.regionlist[i*4+3] = 0;	// this is unused
	}*/

	triangle_mesh_create(ctx, &TriangleInput);


	delete[] TriangleInput.pointlist;
	delete[] TriangleInput.segmentlist;
	if (!m_bFillEnds)
		delete[] TriangleInput.holelist;
	//	delete [] TriangleInput.regionlist;

	//	m_ProjectedMesh.Clear();
	triangle_mesh_copy(ctx, &TriangleOutput, 1, 1);

	vector<XY> Points2D;
	for (int i = 0; i<TriangleOutput.numberofpoints; ++i)
	{
		XY Point;
		Point.x = TriangleOutput.pointlist[i * 2];
		Point.y = TriangleOutput.pointlist[i * 2 + 1];
		Points2D.push_back(Point);
		//OutputMesh.AddNode(Point);
	}
	vector<XYZ> Points3D;
	vector<XYZ>::iterator itPoints3D;
	Convert2DTo3DCoordinates(Points2D, Points3D, ConvertRef);

	for (itPoints3D = Points3D.begin(); itPoints3D != Points3D.end(); ++itPoints3D)
	{
		OutputMesh.AddNode(*itPoints3D);
	}

	//m_TriangleRegions.clear();
	for (int i = 0; i<TriangleOutput.numberoftriangles; ++i)
	{
		OutputMesh.GetIndices(CMesh::TRI).push_back(TriangleOutput.trianglelist[i * 3]);
		OutputMesh.GetIndices(CMesh::TRI).push_back(TriangleOutput.trianglelist[i * 3 + 1]);
		OutputMesh.GetIndices(CMesh::TRI).push_back(TriangleOutput.trianglelist[i * 3 + 2]);
		//m_TriangleRegions.push_back((int)TriangleOutput.triangleattributelist[i]);
	}

	triangle_free(TriangleOutput.pointlist);
	triangle_free(TriangleOutput.trianglelist);
	//trifree(TriangleOutput.triangleattributelist);
	//	trifree(TriangleOutput.neighborlist);

	triangle_context_destroy(ctx);
	return true;
}

void CMeshDomainPlane::Convert2DTo3DCoordinates(vector<XY>& Points2D, vector<XYZ>& Points3D, PLANEPARAMS& ConvertRef)
{
	// Convert the 2D points back to the global 3d coordinate system
	vector<XY>::iterator itPoints2D;

	for (itPoints2D = Points2D.begin(); itPoints2D != Points2D.end(); ++itPoints2D)
	{
		XYZ Point3D;
		Point3D = ConvertRef.XAxis * itPoints2D->x;
		Point3D += ConvertRef.YAxis * itPoints2D->y;
		// Translate the point to its global position
		Point3D += ConvertRef.RefPoint;
		// Add the new 3d point to the list
		Points3D.push_back(Point3D);
	}
}
