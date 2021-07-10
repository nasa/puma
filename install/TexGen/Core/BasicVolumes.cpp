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
#include "BasicVolumes.h"
#include "TexGen.h"
//#include "Yarn.h"
extern "C"
{
#include "../Triangle/triangle.h"
#include "../Triangle/triangle_api.h"
}

using namespace TexGen;
CBasicVolumes::CBasicVolumes(void)
: m_dTolerance(1e-6)
, m_pTextile(NULL)
, m_dSeed(1.0)
, m_bCreatePeriodic(false)
, m_bDebug(false)
{
}

CBasicVolumes::~CBasicVolumes(void)
{
}

bool CBasicVolumes::CreateBasicVolumes(string TextileName)
{
	CTextile* pTextile = TEXGEN.GetTextile(TextileName);
	if (pTextile)
		return CreateBasicVolumes(*pTextile);
	else
		return false;
}

bool CBasicVolumes::CreateBasicVolumes(CTextile &Textile)
{
	m_pTextile = &Textile;
	m_ProjectedMesh.Clear();
	m_DomainMesh.Clear();
	m_YarnMeshes.clear();
	m_ProjectedRegions.clear();
	m_TriangleRegions.clear();

	const CDomain* pDomain = Textile.GetDomain();
	if (!pDomain)
		return false;
	vector<CYarn> &Yarns = Textile.GetYarns();
	m_DomainMesh = pDomain->GetMesh();
	m_YarnMeshes.clear();
	m_YarnMeshes.resize(Yarns.size());
	vector<CYarn>::iterator itYarn;
	vector<CMesh>::iterator itMesh;
	TGLOG("Projecting volume outlines onto the x/y plane");
	for (itYarn = Yarns.begin(), itMesh = m_YarnMeshes.begin(); itYarn != Yarns.end() && itMesh != m_YarnMeshes.end(); ++itYarn, ++itMesh)
	{
		itYarn->AddSurfaceToMesh(*itMesh, *pDomain);
		itMesh->ConvertQuadstoTriangles();
		m_ProjectedMesh.InsertMesh(GetProjectedMesh(*itMesh));
	}
	m_DomainMesh.ConvertQuadstoTriangles();
	m_ProjectedMesh.InsertMesh(GetProjectedMesh(m_DomainMesh));
	//m_bDebug = true;
	if (m_bDebug)
		m_ProjectedMesh.SaveToVTK("EdgesProject");

	// This should have a higher tolerance than everything else
	m_ProjectedMesh.MergeNodes(m_dTolerance*10);

	// Note the order of this sequence is very important, don't mess with this
	// without thinking hard about it
	TGLOG("Removing degenerate segments");
	RemoveDegenerateSegments(m_ProjectedMesh);
	TGLOG("Splitting segments at the nodes");
	SplitLinesByNodes(m_ProjectedMesh);
	TGLOG("Removing duplicate segments");
	RemoveDuplicateSegments(m_ProjectedMesh);
	TGLOG("Merging straight segments");
	MergeStraightLines(m_ProjectedMesh);
	// Need to run this again because the merge may have
	// Unsplit some of the lines that we wanted to keep split
	TGLOG("Splitting segments at the nodes");
	SplitLinesByNodes(m_ProjectedMesh);
	TGLOG("Splitting segments by other segments");
	SplitLinesByLines(m_ProjectedMesh);

//	m_ProjectedMesh.SaveToVTK("Projected");

	// Check that the projected mesh is valid
	if (!ValidProjectedMesh())
		return false;

	TGLOG("Identifying projected regions");
	// Determine the different areas
	if (!CreateProjectedAreas())
		return false;

	if (m_bCreatePeriodic)
	{
		TGLOG("Manually seeding boundary segments");
		// Seed the outer boundary
		if (!SeedOuterBoundary())
			return false;
	}

	if (!RemoveOuterBoundary())
		return false;

	if (m_bDebug)
		SaveProjectedContoursToVTK("Contours");

	TGLOG("Identifying region centers");
	// Get the centers of each area
	if (!CreateProjectedCenters())
		return false;

	TGLOG("Identifying list of yarns contained within each region");
	// Figure out which yarns are in which areas
	CalculateYarnIndices();

	TGLOG("Meshing projected regions in 2D");
	// Mesh the areas
	if (!MeshProjectedAreas())
		return false;

	if (m_bDebug)
		SaveProjectedAreasToVTK("Projected");
	return true;
}

void CBasicVolumes::SaveProjectedContoursToVTK(string Filename)
{
	CMesh Mesh;
//	Mesh.m_Nodes = m_ProjectedMesh.m_Nodes;

	CMeshData<unsigned char> AreaIndex("AreaIndex", CMeshDataBase::ELEMENT);
	CMeshData<double> Area("Area", CMeshDataBase::ELEMENT);
	CMeshData<unsigned char> NumYarns("NumYarns", CMeshDataBase::ELEMENT);

	ostringstream ProjectedAreaIndexData;
	vector<PROJECTED_REGION>::iterator itRegion;
	int i = 0, j, k;
	int iSegment, i1, i2;
	list<int>::iterator itIndex;
	list<int> &Indices = m_ProjectedMesh.GetIndices(CMesh::LINE);
	for (itRegion = m_ProjectedRegions.begin(), j=0; itRegion != m_ProjectedRegions.end(); ++itRegion, ++j)
	{
		for (k=0; k<(int)itRegion->SegmentIndices.size(); ++k)
		{
			iSegment = itRegion->SegmentIndices[k];
			itIndex = Indices.begin();
			advance(itIndex, iSegment*2);
			i1 = *(itIndex++);
			i2 = *(itIndex++);
			Mesh.AddNode(m_ProjectedMesh.GetNode(i1) + XYZ(0,0,0.01*j));
			Mesh.AddNode(m_ProjectedMesh.GetNode(i2) + XYZ(0,0,0.01*j));
			Mesh.GetIndices(CMesh::LINE).push_back(i++);
			Mesh.GetIndices(CMesh::LINE).push_back(i++);
			AreaIndex.m_Data.push_back(j);
			Area.m_Data.push_back(itRegion->dArea);
			NumYarns.m_Data.push_back(itRegion->YarnIndices.size());
		}
	}

	vector<CMeshDataBase*> MeshData;

	MeshData.push_back(&AreaIndex);
	MeshData.push_back(&Area);
	MeshData.push_back(&NumYarns);

	Mesh.SaveToVTK(Filename, &MeshData);
}

void CBasicVolumes::SaveProjectedAreasToVTK(string Filename)
{
	vector<TiXmlElement> AdditionalData;

	CMeshData<unsigned char> AreaIndex("AreaIndex", CMeshDataBase::ELEMENT);
	CMeshData<double> Area("Area", CMeshDataBase::ELEMENT);
	CMeshData<unsigned char> NumYarns("NumYarns", CMeshDataBase::ELEMENT);

	ostringstream ProjectedAreaIndexData;
	vector<int>::iterator itRegion;
	for (itRegion = m_TriangleRegions.begin(); itRegion != m_TriangleRegions.end(); ++itRegion)
	{
		AreaIndex.m_Data.push_back(*itRegion);
		Area.m_Data.push_back(m_ProjectedRegions[*itRegion].dArea);
		NumYarns.m_Data.push_back(m_ProjectedRegions[*itRegion].YarnIndices.size());
	}

	vector<CMeshDataBase*> MeshData;

	MeshData.push_back(&AreaIndex);
	MeshData.push_back(&Area);
	MeshData.push_back(&NumYarns);

	m_ProjectedMesh.SaveToVTK(Filename, &MeshData);
}

bool CBasicVolumes::GetCommonEdgeIndices(int Indices1[3], int Indices2[3], int Common[2])
{
	int i, j, k=0;
	for (i=0; i<3; ++i)
	{
		for (j=0; j<3; ++j)
		{
			if (Indices1[i] == Indices2[j])
			{
				Common[k++] = Indices1[i];	// == Indices2[j]
				if (k == 2)
					return true;
			}
		}
	}
	return false;
}

int CBasicVolumes::MergeStraightLines(CMesh &Mesh)
{
	int iMergeCount = 0;
	list<int>::iterator itIndex, itStart;
	list<int>::iterator itCompareIndex, itCompareStart;
	list<int> &Indices = Mesh.GetIndices(CMesh::LINE);
	int a, b;
	int i[2];
	int j[2];
	int iCommon;
	int iEnd1, iEnd2;
	XYZ P, P1, P2, V1, V2;
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		itStart = itIndex;
		i[0] = *(itIndex++);
		i[1] = *(itIndex++);
		for (itCompareIndex = itIndex; itCompareIndex != Indices.end(); )
		{
			itCompareStart = itCompareIndex;
			j[0] = *(itCompareIndex++);
			j[1] = *(itCompareIndex++);
			iCommon = -1;
			for (a=0; a<2; ++a)
			{
				for (b=0; b<2; ++b)
				{
					if (i[a] == j[b])
					{
						iCommon = i[a];	// == j[b]
						iEnd1 = i[(a+1)%2];
						iEnd2 = j[(b+1)%2];
						break;
					}
				}
			}
			if (iCommon != -1)
			{
				P = Mesh.GetNode(iCommon);
				P1 = Mesh.GetNode(iEnd1);
				P2 = Mesh.GetNode(iEnd2);
				V1 = P1-P;
				V2 = P2-P;
				Normalise(V1);
				Normalise(V2);
				if (DotProduct(V1, V2)+1 <= m_dTolerance)
				{
					// Ok merge them
					Indices.erase(itStart, itIndex);
					if (itIndex == itCompareStart)
						itIndex = Indices.erase(itCompareStart, itCompareIndex);
					else
						Indices.erase(itCompareStart, itCompareIndex);

					Indices.push_back(iEnd1);
					Indices.push_back(iEnd2);

					++iMergeCount;
					break;
				}
			}
		}
	}

	Mesh.RemoveUnreferencedNodes();

	return iMergeCount;
}

int CBasicVolumes::RemoveDuplicateSegments(CMesh &Mesh)
{
	list<int>::iterator itIndex, itStart;
	list<int>::iterator itCompareIndex;
	list<int> &Indices = Mesh.GetIndices(CMesh::LINE);
	int i1, i2;
	int j1, j2;
	int iDuplicateCount = 0;
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		itStart = itIndex;
		i1 = *(itIndex++);
		i2 = *(itIndex++);
		for (itCompareIndex = itIndex; itCompareIndex != Indices.end(); )
		{
			j1 = *(itCompareIndex++);
			j2 = *(itCompareIndex++);
			if ((i1 == j1 && i2 == j2) || (i1 == j2 && i2 == j1))
			{
				Indices.erase(itStart, itIndex);
				++iDuplicateCount;
				break;
			}
		}
	}
	return iDuplicateCount;
}

int CBasicVolumes::RemoveDegenerateSegments(CMesh &Mesh)
{
	list<int>::iterator itIndex, itStart;
	list<int> &Indices = Mesh.GetIndices(CMesh::LINE);
	int i1, i2;
	int iDegenerateCount = 0;
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		itStart = itIndex;
		i1 = *(itIndex++);
		i2 = *(itIndex++);
		if (i1 == i2)
		{
			Indices.erase(itStart, itIndex);
			++iDegenerateCount;
		}
	}
	return iDegenerateCount;
}

int CBasicVolumes::SplitLinesByNodes(CMesh &Mesh)
{
	int iSplitCount = 0;
	list<int>::iterator itIndex, itStart;
	vector<XYZ>::const_iterator itNode;
	list<int> &Indices = Mesh.GetIndices(CMesh::LINE);
	int j, i1, i2;
	XYZ P, L1, L2;
	double dU, dUMin;
	double dDistanceSquared, dToleranceSquared = m_dTolerance*m_dTolerance;
	double dLengthSquared;
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		itStart = itIndex;
		i1 = *(itIndex++);
		i2 = *(itIndex++);
		L1 = Mesh.GetNode(i1);
		L2 = Mesh.GetNode(i2);
		dLengthSquared = GetLengthSquared(L1, L2);
		for (itNode = Mesh.NodesBegin(), j=0; itNode != Mesh.NodesEnd(); ++itNode, ++j)
		{
			// Skip this node if it is one of the segment's ends
			if (j == i1 || j == i2)
				continue;
			P = ShortestDistPointLine(*itNode, L1, L2, dU);
			// Check dU is within the range 0 to 1 and also that the distances
			// between the points P - L1 and P - L2 are greater than the tolerance
			// (this is done using squared lengths for performance reasons)
			// EDIT: This may be unnecessary, since nodes should already be merged
			// at this point
			dUMin = min(dU, 1-dU);
			if (dU > 0 && dU < 1 && dUMin*dUMin*dLengthSquared > dToleranceSquared)
			{
				// Check the point is close to the line
				dDistanceSquared = GetLengthSquared(P, *itNode);
				if (dDistanceSquared <= dToleranceSquared)
				{
					// OK! Let's split this sucker...
					// Remove the current segment
					Indices.erase(itStart, itIndex);

					// Add the new split segments
					Indices.push_back(i1);
					// If the line segment was the last in the list, some trickery is needed to
					// ensure the newly inserted line is checked for splits
					if (itIndex == Indices.end())
						--itIndex;
					Indices.push_back(j);

					Indices.push_back(j);
					Indices.push_back(i2);

					++iSplitCount;

					break;
				}
			}
		}
	}
	return iSplitCount;
}

int CBasicVolumes::SplitLinesByLines(CMesh &Mesh)
{
	int iSplitCount = 0;

	list<int>::iterator itIndex, itStart;
	list<int>::iterator itCompareIndex, itCompareStart;
	list<int> &Indices = Mesh.GetIndices(CMesh::LINE);
	int i1, i2;
	int j1, j2;
	int iNewNodeIndex;
	XYZ P1, P2, P3, P4, P;
	double dU1, dU2;
	double dUMin1, dUMin2;
	double dClosestDistSquared;
	double dToleranceSquared = m_dTolerance*m_dTolerance;
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		itStart = itIndex;
		i1 = *(itIndex++);
		i2 = *(itIndex++);
		P1 = Mesh.GetNode(i1);
		P2 = Mesh.GetNode(i2);
		for (itCompareIndex = itIndex; itCompareIndex != Indices.end(); )
		{
			itCompareStart = itCompareIndex;
			j1 = *(itCompareIndex++);
			j2 = *(itCompareIndex++);
			if (i1 != j1 && i1 != j2 && i2 != j1 && i2 != j2)
			{
				P3 = Mesh.GetNode(j1);
				P4 = Mesh.GetNode(j2);
				if (LineLineIntersect2D(XY(P1.x, P1.y), XY(P2.x, P2.y), XY(P3.x, P3.y), XY(P4.x, P4.y), dU1, dU2))
				{
					dUMin1 = min(dU1, 1-dU1);
					dUMin2 = min(dU2, 1-dU2);
					dClosestDistSquared = min(GetLengthSquared(P1, P2)*dUMin1*dUMin1,
						GetLengthSquared(P1, P2)*dUMin2*dUMin2);
					if (dClosestDistSquared > dToleranceSquared)
					{
						P = P1 + (P2-P1)*dU1;
						Indices.erase(itStart, itIndex);
						if (itIndex == itCompareStart)
							itIndex = Indices.erase(itCompareStart, itCompareIndex);
						else
							Indices.erase(itCompareStart, itCompareIndex);
						
						iNewNodeIndex = Mesh.AddNode(P); //Mesh.m_Nodes.size()-1;

						Indices.push_back(i1);
						Indices.push_back(iNewNodeIndex);

						Indices.push_back(iNewNodeIndex);
						Indices.push_back(i2);

						Indices.push_back(j1);
						Indices.push_back(iNewNodeIndex);

						Indices.push_back(iNewNodeIndex);
						Indices.push_back(j2);

						++iSplitCount;
						break;
					}
				}
			}
		}
	}

	return iSplitCount;
}

bool CBasicVolumes::ValidProjectedMesh()
{
	// For the mesh to be valid, each line segment should
	// at least be connected at both ends
	list<int>::iterator itIndex;
	list<int>::iterator itCompareIndex;
	list<int> &Indices = m_ProjectedMesh.GetIndices(CMesh::LINE);
	int i1, i2;
	int j1, j2;
	bool bEnd1, bEnd2;
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		bEnd1 = false;
		bEnd2 = false;
		i1 = *(itIndex++);
		i2 = *(itIndex++);
		for (itCompareIndex = Indices.begin(); itCompareIndex != Indices.end(); )
		{
			j1 = *(itCompareIndex++);
			j2 = *(itCompareIndex++);
			if (i1 == j1 || i1 == j2)
				bEnd1 = true;
			if (i2 == j1 || i2 == j2)
				bEnd2 = true;
		}
		if (!bEnd1 || !bEnd2)
			return false;
	}

	return true;
}

bool CBasicVolumes::CreateProjectedAreas()
{
//    int iIterationCount = 0;
	// Start at a random segment, then follow it round either from origin to end
	// or end to origin (depending on i==0 or i==1)
	// Follow segments always following the one with the greatest clockwise angle
	vector<int> LineState;
	LineState.resize(m_ProjectedMesh.GetIndices(CMesh::LINE).size()/2, 0);
	list<int>::iterator itIndex, itStartIndex;
	list<int>::iterator itFollowIndex;
	list<int>::iterator itCompareIndex, itStartCompareIndex;
	list<int>::iterator itBestFind;
	list<int> &Indices = m_ProjectedMesh.GetIndices(CMesh::LINE);
	int iDir, iCurrentDir;
	int i1, i2;
	int j1, j2;
	int iBestj1, iBestj2;
	int iLineCount, iInitialLineCount;
	int iEndIndex;
	int iBestFindDir, iBestLineCount;
	double dMaxAngle, dAngle;
	XYZ V1, V2;
	m_ProjectedRegions.clear();
	for (iDir=FORWARD; iDir<=REVERSE; ++iDir)
	{
		for (itIndex = Indices.begin(), iInitialLineCount = 0; itIndex != Indices.end(); ++iInitialLineCount)
		{
			itFollowIndex = itIndex;
			itStartIndex = itIndex;
			iCurrentDir = iDir;
			i1 = *(itIndex++);
			i2 = *(itIndex++);
			if (LineState[iInitialLineCount] & iDir)
				continue;
			m_ProjectedRegions.push_back(PROJECTED_REGION());
			do
			{
				if (iCurrentDir == FORWARD)
					iEndIndex = i2;
				else
					iEndIndex = i1;
				V1 = m_ProjectedMesh.GetNode(i2) - m_ProjectedMesh.GetNode(i1);
				if (iCurrentDir == REVERSE)
					V1 *= -1;
				itBestFind = Indices.end();
				for (itCompareIndex = Indices.begin(), iLineCount=0; itCompareIndex != Indices.end(); ++iLineCount)
				{
					itStartCompareIndex = itCompareIndex;
					j1 = *(itCompareIndex++);
					j2 = *(itCompareIndex++);
					if (itStartCompareIndex == itFollowIndex)
						continue;
					if (iEndIndex == j1)
					{
						if (LineState[iLineCount] & FORWARD)
							continue;
						V2 = m_ProjectedMesh.GetNode(j2) - m_ProjectedMesh.GetNode(j1);
						// Normalisation not needed
						dAngle = atan2(CrossProduct(V1, V2).z, DotProduct(V1, V2));
						if (itBestFind == Indices.end() || dAngle > dMaxAngle)
						{
							dMaxAngle = dAngle;
							iBestFindDir = FORWARD;
							itBestFind = itStartCompareIndex;
							iBestLineCount = iLineCount;
							iBestj1 = j1;
							iBestj2 = j2;
						}
					}
					else if (iEndIndex == j2)
					{
						if (LineState[iLineCount] & REVERSE)
							continue;
						V2 = m_ProjectedMesh.GetNode(j1) - m_ProjectedMesh.GetNode(j2);
						// Normalisation not needed
						dAngle = atan2(CrossProduct(V1, V2).z, DotProduct(V1, V2));
						if (itBestFind == Indices.end() || dAngle > dMaxAngle)
						{
							dMaxAngle = dAngle;
							iBestFindDir = REVERSE;
							itBestFind = itStartCompareIndex;
							iBestLineCount = iLineCount;
							iBestj1 = j1;
							iBestj2 = j2;
						}
					}
				}
				if (itBestFind == Indices.end())
					return false;
				LineState[iBestLineCount] |= iBestFindDir;
				iCurrentDir = iBestFindDir;
				itFollowIndex = itBestFind;
				i1 = iBestj1;
				i2 = iBestj2;
				m_ProjectedRegions.back().SegmentIndices.push_back(iBestLineCount);
				if (iBestFindDir == FORWARD)
					m_ProjectedRegions.back().ContourNodes.push_back(iBestj2);
				else
					m_ProjectedRegions.back().ContourNodes.push_back(iBestj1);
			} while(itFollowIndex != itStartIndex);
			m_ProjectedRegions.back().dArea = GetRegionArea(m_ProjectedRegions.back());
		}
	}

	// The total area of all the regions should be 0, the large region encapsulating all the rest should
	// be negative and equal in magnitude to the sum of all the interior regions. If not there may have been
	// some problem creating the regions.
	double dAreaSum = 0;
	vector<PROJECTED_REGION>::iterator itRegion;
	for (itRegion = m_ProjectedRegions.begin(); itRegion != m_ProjectedRegions.end(); ++itRegion)
	{
		dAreaSum += itRegion->dArea;
	}

	if (abs(dAreaSum) > 1e-3)
	{
		TGERROR("Error creating projected areas, the total area is non-zero: " << dAreaSum);
		return false;
	}

	return true;
}

bool CBasicVolumes::SeedOuterBoundary()
{
	if (m_ProjectedRegions.empty())
		return false;
	// The outer region has its area inverted, so it will be the minimum of all areas
	vector<PROJECTED_REGION>::iterator itOuterRegion = 
		min_element(m_ProjectedRegions.begin(), m_ProjectedRegions.end(), ProjectedRegionArea());
	
	// The area should be negative
	assert(itOuterRegion->dArea <= 0);

	list<int> &Indices = m_ProjectedMesh.GetIndices(CMesh::LINE);
	XYZ P;
	double u;
	int i, i1, i2, iPrevIndex, iNewIndex;
	list<int>::iterator itIndex, itStart;
	vector<int>::iterator itSegment;
	double dLength;
	// Apply a small distorition to the seed to prevent the likelyhood of the segment length being equal
	// or very close to the seed length. It is important because opposite faces should be seeded in exactly
	// the same manner.
	const double dAdjustedSeed = m_dSeed + 1.2345e-3;
	set<int> DeleteSegments;
	// Split any segments up that are greater than the seed length
	for (itSegment=itOuterRegion->SegmentIndices.begin(); itSegment!=itOuterRegion->SegmentIndices.end(); ++itSegment)
	{
		itIndex = Indices.begin();
		advance(itIndex, (*itSegment)*2);
		i1 = *(itIndex++);
		i2 = *(itIndex++);
		const XYZ &N1 = m_ProjectedMesh.GetNode(i1);
		const XYZ &N2 = m_ProjectedMesh.GetNode(i2);
		dLength = GetLength(N1, N2);
		if (dLength > dAdjustedSeed)
		{
			int iNumSplits = int(floor(dLength / (dAdjustedSeed)));

			// Add the segment to the list of segements to be deleted
			// they will be deleted in the next loop so as not to invalidate
			// segment indices for this loop
			DeleteSegments.insert(*itSegment);

			// Create some new segments
			iPrevIndex = i1;
			for (i=0; i<iNumSplits; ++i)
			{
				u = double(i+1)/double(iNumSplits+1);
				P = N1 + (N2-N1)*u;
				iNewIndex = m_ProjectedMesh.AddNode(P);
				Indices.push_back(iPrevIndex);
				Indices.push_back(iNewIndex);
				iPrevIndex = iNewIndex;
			}
			Indices.push_back(iPrevIndex);
			Indices.push_back(i2);
		}
	}

	// Erase the segments that were split
	for (itIndex=Indices.begin(), i=0; itIndex!=Indices.end(); ++i)
	{
		if (DeleteSegments.count(i))
		{
			// Needs erasing
			itStart = itIndex;
			++itIndex;
			++itIndex;
			itIndex = Indices.erase(itStart, itIndex);
		}
		else
		{
			// Increment indices
			++itIndex;
			++itIndex;
		}
	}

	// The projected areas will have been invalidated by the splitting so calculate them again
	return CreateProjectedAreas();
}

bool CBasicVolumes::RemoveOuterBoundary()
{
	if (m_ProjectedRegions.empty())
		return false;
	// The outer region has its area inverted, so it will be the minimum of all areas
	vector<PROJECTED_REGION>::iterator itOuterRegion = 
		min_element(m_ProjectedRegions.begin(), m_ProjectedRegions.end(), ProjectedRegionArea());

	m_ProjectedRegions.erase(itOuterRegion);
	return true;
}

double CBasicVolumes::GetRegionArea(const PROJECTED_REGION &Region)
{
	vector<int>::const_iterator itIndex;
	list<int> &Indices = m_ProjectedMesh.GetIndices(CMesh::LINE);
	vector<XYZ> Nodes;
	for (itIndex = Region.ContourNodes.begin(); itIndex != Region.ContourNodes.end(); ++itIndex)
	{
		Nodes.push_back(m_ProjectedMesh.GetNode(*itIndex));
	}
	return GetArea(&Nodes.front(), Nodes.size());
}

bool CBasicVolumes::CreateProjectedCenters()
{
	vector<PROJECTED_REGION>::iterator itRegion;
	vector<int>::iterator itSegment;
	list<int>::iterator itIndex;
	list<int> &Indices = m_ProjectedMesh.GetIndices(CMesh::LINE);
	int i, i1, i2;
	XYZ Min, Max, P1, P2;
	for (itRegion = m_ProjectedRegions.begin(), i=0; itRegion != m_ProjectedRegions.end(); ++itRegion, ++i)
	{
		XYZ Point;
		for (itSegment=itRegion->SegmentIndices.begin(); itSegment!=itRegion->SegmentIndices.end(); ++itSegment)
		{
			itIndex = Indices.begin();
			advance(itIndex, (*itSegment)*2);
			i1 = *(itIndex++);
			i2 = *(itIndex++);
			P1 = m_ProjectedMesh.GetNode(i1);
			P2 = m_ProjectedMesh.GetNode(i2);
			if (itSegment == itRegion->SegmentIndices.begin())
			{
				Min = Max = P1;
			}
			Min = ::Min(Min, P1);
			Min = ::Min(Min, P2);
			Max = ::Max(Max, P1);
			Max = ::Max(Max, P2);
			Point += P1 + P2;
		}
		Point /= 2*itRegion->SegmentIndices.size();
		// The center may not be within the region, so if its not just generate
		// random points within the bounding box until one is found
		while (!PointInsideRegion(Point, i))
		{
			Point.x = RandomNumber(Min.x, Max.x);
			Point.y = RandomNumber(Min.y, Max.y);
		}
		itRegion->Center = Point;
	}
	return true;
}

// Algorithm borrowed from http://local.wasp.uwa.edu.au/~pbourke/geometry/insidepoly/
bool CBasicVolumes::PointInsideRegion(XYZ p, int iRegion)
{
	PROJECTED_REGION &Region = m_ProjectedRegions[iRegion];
	vector<int> &ContourNodes = Region.ContourNodes;
	vector<int>::iterator itNodeIndex;

	int counter = 0;
	int i, N = (int)ContourNodes.size();
	double xinters;
	XYZ p1, p2;
	p1 = m_ProjectedMesh.GetNode(ContourNodes[0]);
	for (i=1;i<=N;i++)
	{
		p2 = m_ProjectedMesh.GetNode(ContourNodes[i % N]);
		if (p.y > min(p1.y,p2.y))
		{
			if (p.y <= max(p1.y,p2.y))
			{
				if (p.x <= max(p1.x,p2.x))
				{
					if (p1.y != p2.y)
					{
						xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
						if (p1.x == p2.x || p.x <= xinters)
							counter++;
					}
				}
			}
		}
		p1 = p2;
	}

	if (counter % 2 == 1)
		return true;
	else
		return false;
}

void CBasicVolumes::CalculateYarnIndices()
{
	int i, iNumRegions = (int)m_ProjectedRegions.size();
	int j, iNumYarns = (int)m_YarnMeshes.size();
	XYZ Point;
	vector<pair<double, int> > YarnHeightIndex;
	for (i=0; i<iNumRegions; ++i)
	{
		Point = m_ProjectedRegions[i].Center;
		YarnHeightIndex.clear();
		for (j=0; j<iNumYarns; ++j)
		{
			double dMinZ = 0, dMaxZ = 0;
			if (GetMeshVerticalBounds(m_YarnMeshes[j], Point, dMinZ, dMaxZ))
			{
				YarnHeightIndex.push_back(make_pair((dMinZ+dMaxZ)/2, j));
			}
		}
		sort(YarnHeightIndex.begin(), YarnHeightIndex.end(), LessPairDoubleInt());
		for (j=0; j<(int)YarnHeightIndex.size(); ++j)
		{
			m_ProjectedRegions[i].YarnIndices.push_back(YarnHeightIndex[j].second);
		}
	}
}

bool CBasicVolumes::MeshProjectedAreas()
{
	stringstream Switches;

	double dMaxArea = 0.5*m_dSeed*m_dSeed;
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

#ifndef _DEBUG
	Switches << "Q";
#endif
	// Triangle has trouble parsing values given in scientific format so use fixed format with a
	// redicilously high precision to get around the problem
	Switches << "pzAPBq" << setiosflags(ios::fixed) << setprecision(20) << dMinAngle << "a" << dMaxArea;

	if (m_bCreatePeriodic)
		Switches << "Y";

	triangleio TriangleInput, TriangleOutput;

	context *ctx;
	ctx = triangle_context_create();

	triangle_context_options(ctx, (char*)Switches.str().c_str());

	memset(&TriangleInput, 0, sizeof(TriangleInput));
	memset(&TriangleOutput, 0, sizeof(TriangleOutput));

	// Input Nodes
	TriangleInput.pointlist = new REAL [m_ProjectedMesh.GetNumNodes()*2];
	TriangleInput.numberofpoints = (int)m_ProjectedMesh.GetNumNodes();

	int i;
	for (i=0; i<TriangleInput.numberofpoints; ++i)
	{
		TriangleInput.pointlist[i*2] = m_ProjectedMesh.GetNode(i).x;
		TriangleInput.pointlist[i*2+1] = m_ProjectedMesh.GetNode(i).y;
	}

	// Input Segments
	list<int>::iterator itIndex;
	list<int> &Indices = m_ProjectedMesh.GetIndices(CMesh::LINE);

	TriangleInput.segmentlist = new int [Indices.size()];
	TriangleInput.numberofsegments = (int)Indices.size()/2;

	for (itIndex = Indices.begin(), i = 0; itIndex != Indices.end(); ++itIndex, ++i)
	{
		TriangleInput.segmentlist[i] = *itIndex;
	}

	// Input regions
	TriangleInput.regionlist = new REAL [m_ProjectedRegions.size()*4];
	TriangleInput.numberofregions = m_ProjectedRegions.size();

	for (i=0; i<TriangleInput.numberofregions; ++i)
	{
		TriangleInput.regionlist[i*4] = m_ProjectedRegions[i].Center.x;
		TriangleInput.regionlist[i*4+1] = m_ProjectedRegions[i].Center.y;
		TriangleInput.regionlist[i*4+2] = i;
		TriangleInput.regionlist[i*4+3] = 0;	// this is unused
	}

	triangle_mesh_create(ctx, &TriangleInput);


	delete [] TriangleInput.pointlist;
	delete [] TriangleInput.segmentlist;
	delete [] TriangleInput.regionlist;

	m_ProjectedMesh.Clear();

	triangle_mesh_copy(ctx, &TriangleOutput, 1, 1);

	XYZ Point;
	for (i=0; i<TriangleOutput.numberofpoints; ++i)
	{
		Point.x = TriangleOutput.pointlist[i*2];
		Point.y = TriangleOutput.pointlist[i*2+1];
		m_ProjectedMesh.AddNode(Point);
	}

	m_TriangleRegions.clear();
	for (i=0; i<TriangleOutput.numberoftriangles; ++i)
	{
/*		// Lets do a quick check to see if the elements are ordered consistently
		int i1, i2, i3;
		i1 = TriangleOutput.trianglelist[i*3];
		i2 = TriangleOutput.trianglelist[i*3+1];
		i3 = TriangleOutput.trianglelist[i*3+2];
		XYZ P1, P2, P3;
		P1 = m_ProjectedMesh.m_Nodes[i1];
		P2 = m_ProjectedMesh.m_Nodes[i2];
		P3 = m_ProjectedMesh.m_Nodes[i3];
		assert(CrossProduct(P2-P1, P3-P1).z >= 0);*/

		m_ProjectedMesh.GetIndices(CMesh::TRI).push_back(TriangleOutput.trianglelist[i*3]);
		m_ProjectedMesh.GetIndices(CMesh::TRI).push_back(TriangleOutput.trianglelist[i*3+1]);
		m_ProjectedMesh.GetIndices(CMesh::TRI).push_back(TriangleOutput.trianglelist[i*3+2]);
		m_TriangleRegions.push_back((int)TriangleOutput.triangleattributelist[i]);
	}

	triangle_free(TriangleOutput.pointlist);
	triangle_free(TriangleOutput.trianglelist);
	triangle_free(TriangleOutput.triangleattributelist);

	triangle_context_destroy(ctx);
	return true;
}


bool CBasicVolumes::GetMeshVerticalBounds(const CMesh &Mesh, XYZ Point, double &dMinZ, double &dMaxZ, bool bForceFind)
{
	vector< pair<double, XYZ> > IntersectionPoints;
	vector< pair<double, XYZ> >::iterator itIntersectionPt;
	XYZ P1, P2;
	P1 = P2 = Point;
	P1.z = 0;
	P2.z = 1;
	double dZ;
	Mesh.IntersectLine(P1, P2, IntersectionPoints, make_pair(false, false), bForceFind);
	for (itIntersectionPt = IntersectionPoints.begin(); itIntersectionPt != IntersectionPoints.end(); )
	{
		// Ignore intersections with vertical wall faces
		if (abs(itIntersectionPt->second.z) < m_dTolerance &&
			(!IntersectionPoints.empty() || !bForceFind))	// Make sure you don't erase the last find if force find is on
			itIntersectionPt = IntersectionPoints.erase(itIntersectionPt);
		else
			++itIntersectionPt;
	}
	if (IntersectionPoints.empty())
		return false;
	for (itIntersectionPt = IntersectionPoints.begin(); itIntersectionPt != IntersectionPoints.end(); ++itIntersectionPt)
	{
		dZ = itIntersectionPt->first;
		if (itIntersectionPt == IntersectionPoints.begin() || dZ < dMinZ)
			dMinZ = dZ;
		if (itIntersectionPt == IntersectionPoints.begin() || dZ > dMaxZ)
			dMaxZ = dZ;
	}
	return true;
}

CMesh CBasicVolumes::GetProjectedMesh(const CMesh &Mesh)
{
	CMesh ProjectedMesh = Mesh;
//	ProjectedMesh.ConvertQuadstoTriangles();
	list<int>::iterator itIndex;
	list<int>::iterator itCompareIndex;
	list<int> &Indices = ProjectedMesh.GetIndices(CMesh::TRI);
	int i[3];
	int j[3];
	int CommonIndices[2];
	bool bDirection, bCompareDirection;
	XYZ V1, V2;
	
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		i[0] = *(itIndex++);
		i[1] = *(itIndex++);
		i[2] = *(itIndex++);
		V1 = ProjectedMesh.GetNode(i[1])-ProjectedMesh.GetNode(i[0]);
		V2 = ProjectedMesh.GetNode(i[2])-ProjectedMesh.GetNode(i[0]);

		bDirection = V1.x*V2.y-V2.x*V1.y>0?true:false;
		for (itCompareIndex = itIndex; itCompareIndex != Indices.end(); )
		{
			j[0] = *(itCompareIndex++);
			j[1] = *(itCompareIndex++);
			j[2] = *(itCompareIndex++);
			if (GetCommonEdgeIndices(i, j, CommonIndices))
			{
				V1 = ProjectedMesh.GetNode(j[1])-ProjectedMesh.GetNode(j[0]);
				V2 = ProjectedMesh.GetNode(j[2])-ProjectedMesh.GetNode(j[0]);
				bCompareDirection = V1.x*V2.y-V2.x*V1.y>0?true:false;
				if (bDirection != bCompareDirection)
				{
					ProjectedMesh.GetIndices(CMesh::LINE).push_back(CommonIndices[0]);
					ProjectedMesh.GetIndices(CMesh::LINE).push_back(CommonIndices[1]);
				}
			}
		}
	}

	ProjectedMesh.RemoveAllElementsExcept(CMesh::LINE);
	ProjectedMesh.RemoveUnreferencedNodes();

	vector<XYZ>::iterator itNode;
	vector<XYZ> &ProjectedMeshNodes = ProjectedMesh.GetNodes();
	for (itNode = ProjectedMeshNodes.begin(); itNode != ProjectedMeshNodes.end(); ++itNode)
	{
		itNode->z = 0;
	}

	return ProjectedMesh;
}




















