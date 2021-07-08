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
#include "Mesher.h"
#include "TexGen.h"
#include "PeriodicBoundaries.h"
//#include "Yarn.h"
extern "C"
{
#include "../Triangle/triangle.h"
}

using namespace TexGen;
CMesher::CMesher( int iBoundaryConditions )
: m_bHybrid(false)
, m_bQuadratic(false)
, m_bProjectMidSideNodes(true)
, m_dLayerMergeTolerance(1e-3)
, m_iBoundaryConditions(iBoundaryConditions)
{
	switch( iBoundaryConditions )
	{
		case MATERIAL_CONTINUUM:
		case SINGLE_LAYER_RVE:
			m_PeriodicBoundaries = new CPeriodicBoundaries;
			break;
		case SHEARED_BC:
			m_PeriodicBoundaries = new CShearedPeriodicBoundaries;
			break;
		case NO_BOUNDARY_CONDITIONS:
		default:
			m_PeriodicBoundaries = NULL;
			break;
	}
}

CMesher::~CMesher(void)
{
	if ( m_PeriodicBoundaries != NULL )
	{
		delete m_PeriodicBoundaries;
	}
}

bool CMesherBase::CreateMesh(string TextileName)
{
	CTextile* pTextile = TEXGEN.GetTextile(TextileName);
	if (pTextile)
		return CreateMesh(*pTextile);
	else
		return false;
}

bool CMesher::CreateMesh(CTextile &Textile)
{
	m_ProjectedNodes.clear();

	if (!CreateBasicVolumes(Textile))
		return false;

	TGLOG("Meshing basic volumes in 3D");
	CreateVolumeMesh(Textile);

	TGLOG("Getting element data");
	FillYarnTangentsData();

	if (m_bQuadratic)
	{
		TGLOG("Converting mesh to quadratic");
		ConvertMeshToQuadratic();
		if ( m_iBoundaryConditions != NO_BOUNDARY_CONDITIONS )
		{
			AddQuadraticNodesToSets();
		}
	}

	TGLOG("Meshing complete with " << m_VolumeMesh.GetNumElements() << " elements and " << m_VolumeMesh.GetNumNodes() << " nodes");

//	SaveVolumeMeshToVTK("Volume");

	return true;
}

void CMesher::SaveVolumeMeshToVTK(string Filename)
{
	TGLOG("Save Volume Mesh to VTK");
	CMeshData<unsigned short> RegionIndex("RegionIndex", CMeshDataBase::ELEMENT);
	CMeshData<unsigned char> Layer("Layer", CMeshDataBase::ELEMENT);
	CMeshData<char> YarnIndex("YarnIndex", CMeshDataBase::ELEMENT);
	CMeshData<XYZ> YarnTangent("YarnTangent", CMeshDataBase::ELEMENT);
	CMeshData<XY> Location("Location", CMeshDataBase::ELEMENT);
	CMeshData<double> VolumeFraction("VolumeFraction", CMeshDataBase::ELEMENT);
	CMeshData<double> SurfaceDistance("SurfaceDistance", CMeshDataBase::ELEMENT);
	CMeshData<XYZ> Orientation("Orientation", CMeshDataBase::ELEMENT);

	list<MESHER_ELEMENT_DATA>::iterator itElemData;
	int i;
	for (i = 0; i < CMesh::NUM_ELEMENT_TYPES; ++i)
	{
		for (itElemData = m_ElementData[i].begin(); itElemData != m_ElementData[i].end(); ++itElemData)
		{
			RegionIndex.m_Data.push_back(itElemData->iRegion);
			Layer.m_Data.push_back(itElemData->iLayer);
			YarnIndex.m_Data.push_back(itElemData->iYarnIndex);
			YarnTangent.m_Data.push_back(itElemData->YarnTangent);
			Location.m_Data.push_back(itElemData->Location);
			VolumeFraction.m_Data.push_back(itElemData->dVolumeFraction);
			SurfaceDistance.m_Data.push_back(itElemData->dSurfaceDistance);
			Orientation.m_Data.push_back(itElemData->Orientation);
		}
	}

	vector<CMeshDataBase*> MeshData;

	MeshData.push_back(&RegionIndex);
	MeshData.push_back(&Layer);
	MeshData.push_back(&YarnIndex);
	MeshData.push_back(&YarnTangent);
	MeshData.push_back(&Location);
	MeshData.push_back(&VolumeFraction);
	MeshData.push_back(&SurfaceDistance);
	MeshData.push_back(&Orientation);

	TGLOG("Calling SaveToVTK");
	m_VolumeMesh.SaveToVTK(Filename, &MeshData);
}

void CMesher::SaveVolumeMeshToABAQUS(string Filename, string TextileName )
{
	CTextile* pTextile = TEXGEN.GetTextile(TextileName);
	if ( pTextile == NULL )
	{
		TGERROR("Cannot save to ABAQUS: no textile created");
		return;
	}
	SaveVolumeMeshToABAQUS( Filename, *pTextile );
}

void CMesher::SaveVolumeMeshToABAQUS(string Filename, CTextile& Textile )
{
	TGLOG("Replacing spaces in filename with underscore for ABAQUS compatibility");
	Filename = ReplaceFilenameSpaces( Filename );
	vector<POINT_INFO> ElementsInfo;
	POINT_INFO Info;
	list<MESHER_ELEMENT_DATA>::const_iterator itData;
	int i;

	for (i = 0; i < CMesh::NUM_ELEMENT_TYPES; ++i)
	{
		for (itData = m_ElementData[i].begin(); itData != m_ElementData[i].end(); ++itData)
		{	
			Info.iYarnIndex = itData->iYarnIndex;
			Info.Location = itData->Location;
			Info.YarnTangent = itData->YarnTangent;
			Info.dVolumeFraction = itData->dVolumeFraction;
			Info.dSurfaceDistance = itData->dSurfaceDistance;
			Info.Orientation = itData->Orientation;
			Info.Up = itData->Up;
			
			ElementsInfo.push_back(Info);
		}
	}
	m_VolumeMesh.SaveToABAQUS(Filename, &ElementsInfo, false, false);

	ofstream Output(Filename.c_str(), ofstream::app );
	// Output material properties
	m_Materials = new CTextileMaterials;
	m_Materials->SetupMaterials( Textile );
	m_Materials->OutputMaterials( Output, Textile.GetNumYarns(), false );
	delete( m_Materials );

	if ( m_iBoundaryConditions != NO_BOUNDARY_CONDITIONS )
	{
		m_PeriodicBoundaries->SetDomainSize( Textile.GetDomain()->GetMesh() );
		if (SaveNodeSets() )
		{
			//ofstream Output(Filename.c_str(), ofstream::app );
			Output << "*****************" << endl;
			Output << "*** NODE SETS ***" << endl;
			Output << "*****************" << endl;
			Output << "** AllNodes - Node set containing all elements" << endl;
			Output << "*NSet, NSet=AllNodes, Generate" << endl;
			Output << "1, " << m_VolumeMesh.GetNumNodes() << ", 1" << endl;
			m_PeriodicBoundaries->CreatePeriodicBoundaries( Output, m_VolumeMesh.GetNumNodes() + 1, Textile, m_iBoundaryConditions, false );
		}
		else
			TGERROR("Unable to generate node sets");
	}
}

void CMesher::CreateVolumeMesh(CTextile &Textile)
{
	int iNumNodes = (int)m_ProjectedMesh.GetNumNodes();
	m_ProjectedNodes.clear();
	m_ProjectedNodes.resize(iNumNodes);

	NODE_PAIR_SETS  EdgeNodePairSets;
	const vector<XYZ> &Repeats = Textile.GetYarn(0)->GetRepeats();

	if ( m_bCreatePeriodic )
	{
		// Find matching nodes on opposite edges of domain
		vector<XYZ>::const_iterator itRepeat;
		
		for (itRepeat=Repeats.begin(); itRepeat!=Repeats.end(); ++itRepeat)
		{
			NODE_PAIR_SET NodePairs;
			m_ProjectedMesh.GetNodePairs(*itRepeat, NodePairs, 1e-5);
			SortPairs( NodePairs, (*itRepeat).y == 0 ? true:false );   // If pairs with constant x, sort by y
			EdgeNodePairSets.push_back( NodePairs );
		}
	}

	
	int i, j;
	set<int> CornerIndex;
	for (i=0; i<iNumNodes; ++i)
	{
		m_ProjectedNodes[i].Position = m_ProjectedMesh.GetNode(i);
		if ( m_bCreatePeriodic )
		{
			// Set up so that columns of nodes on opposite faces match
			if ( m_ProjectedNodes[i].RaisedNodes.empty() )  // Might already have been filled if RaiseNode was called for node on opposite face
			{
				set<int> PairIndices;
				RaiseNodes(i);
				GetEdgePairIndices(EdgeNodePairSets, i, PairIndices);  // Find matching nodes on opposite sides of domain.  If it's a corner will return all 4 nodes
				if ( !PairIndices.empty() )  // If found matching pairs add them to the projected nodes array
				{
					if ( PairIndices.size() == 4 )
						CornerIndex = PairIndices;
					set<int>::iterator	itPairIndices;
					for ( itPairIndices = PairIndices.begin(); itPairIndices != PairIndices.end(); ++itPairIndices )
					{
						if ( *itPairIndices != i )
							m_ProjectedNodes[*itPairIndices].RaisedNodes = m_ProjectedNodes[i].RaisedNodes;
					}
				}
			}
		}
		else
		{
			RaiseNodes(i);
		}
	}

	m_VolumeMesh.Clear();
	int iIndexCount=0;
	XYZ Pos;
	for (i=0; i<iNumNodes; ++i)
	{
		pair<int,vector<int> > ProjectedVolumeNode;
		vector<int> VolumeNodes;
		ProjectedVolumeNode.first = i;
		Pos = m_ProjectedNodes[i].Position;
		vector<RAISED_NODE> &RaisedNodes = m_ProjectedNodes[i].RaisedNodes;
		int iRaisedNum = (int)RaisedNodes.size();
		for (j=0; j<iRaisedNum; ++j)
		{
			RaisedNodes[j].iIndex = iIndexCount++;
			Pos.z = RaisedNodes[j].dZ;
			m_VolumeMesh.AddNode(Pos);
		}
	}

	if ( m_bCreatePeriodic && m_iBoundaryConditions != NO_BOUNDARY_CONDITIONS )
	{
		CreateNodeSets( EdgeNodePairSets, CornerIndex, Repeats );
	}

	list<int>::iterator itIndex;
	list<int> &Indices = m_ProjectedMesh.GetIndices(CMesh::TRI);
	int iRegion;
	for (i = 0; i < CMesh::NUM_ELEMENT_TYPES; ++i)
	{
		m_ElementData[i].clear();
	}
	m_EdgeConstraints.clear();
	TRIANGLE Tri;
	for (itIndex = Indices.begin(), i=0; itIndex != Indices.end(); ++i)
	{
		for (j=0; j<3; ++j)
			Tri.i[j] = *(itIndex++);
		iRegion = m_TriangleRegions[i];
		MeshColumn(Tri, iRegion);
	}

}

void CMesher::RaiseNodes(int iIndex)
{
	set<int> YarnIndices;
	insert_iterator<set<int> > iiYarnIndices(YarnIndices, YarnIndices.end());
	set<int>::iterator itYarnIndex;

	list<int>::iterator itIndex;
	list<int> &Indices = m_ProjectedMesh.GetIndices(CMesh::TRI);
	int i, j, iNode, iRegion;

	// Find projected triangles which have node (iIndex) as a corner
	for (itIndex = Indices.begin(), i=0; itIndex != Indices.end(); ++i)
	{
		for (j=0; j<3; ++j)
		{
			iNode = *(itIndex++);
			if (iNode == iIndex)
			{
				iRegion = m_TriangleRegions[i];
				copy(m_ProjectedRegions[iRegion].YarnIndices.begin(), m_ProjectedRegions[iRegion].YarnIndices.end(), iiYarnIndices);
			}
		}
	}

	// Get the domain bounds
	XYZ Point = m_ProjectedMesh.GetNode(iIndex);
	pair<double, double> DomainBounds(0,0);
	bool bFound = GetMeshVerticalBounds(m_DomainMesh, Point, DomainBounds.first, DomainBounds.second, true);
	assert(bFound);

	// Get the yarn bounds
	map<int, pair<double, double> > YarnBounds;
	for (itYarnIndex=YarnIndices.begin(); itYarnIndex!=YarnIndices.end(); ++itYarnIndex)
	{
		pair<double, double> Bounds(0,0);
		bool bFound = GetMeshVerticalBounds(m_YarnMeshes[*itYarnIndex], Point, Bounds.first, Bounds.second, true);
		assert(bFound);
		//if ( bFound )
		YarnBounds[*itYarnIndex] = Bounds;
	}

	// Remove overlaps that may exist between yarn boundaries
	map<int, pair<double, double> >::iterator itYarnBound, itCompareBound;
	pair<double, double> *pAbove, *pBelow;
	double dAverage;
	for (itYarnBound = YarnBounds.begin(); itYarnBound != YarnBounds.end(); ++itYarnBound)
	{
		// Make sure all the yarns fit within the domain
		if (itYarnBound->second.first < DomainBounds.first)
			itYarnBound->second.first = DomainBounds.first;
		if (itYarnBound->second.second > DomainBounds.second)
			itYarnBound->second.second = DomainBounds.second;
		// Make sure the yarns don't overlap each other
		for (itCompareBound = itYarnBound, ++itCompareBound; itCompareBound != YarnBounds.end(); ++itCompareBound)
		{
			if (itYarnBound->second.first+itYarnBound->second.second >   // Check which yarn is on top
				itCompareBound->second.first+itCompareBound->second.second)
			{
				pAbove = &itYarnBound->second;
				pBelow = &itCompareBound->second;
			}
			else
			{
				pAbove = &itCompareBound->second;
				pBelow = &itYarnBound->second;
			}

			dAverage = 0.5*(pAbove->first + pBelow->second);  // Average of overlapping edges

			pAbove->first = max(pAbove->first, dAverage);  
			pAbove->second = max(pAbove->second, dAverage);

			pBelow->first = min(pBelow->first, dAverage);
			pBelow->second = min(pBelow->second, dAverage);
		}		
	}

	// Domain bounds
	{
		RAISED_NODE RaisedNode;
//		RaisedNode.YarnBoundaryIndices.push_back(-1);	// Not really necessary, and seems to be causing some bugs...
		RaisedNode.dZ = DomainBounds.first;
		m_ProjectedNodes[iIndex].RaisedNodes.push_back(RaisedNode);
		RaisedNode.dZ = DomainBounds.second;
		m_ProjectedNodes[iIndex].RaisedNodes.push_back(RaisedNode);
	}
	// Yarn bounds
	for (itYarnBound = YarnBounds.begin(); itYarnBound != YarnBounds.end(); ++itYarnBound)
	{
		RAISED_NODE RaisedNode;
		RaisedNode.YarnBoundaryIndices.push_back(itYarnBound->first);
		RaisedNode.dZ = itYarnBound->second.first;
		m_ProjectedNodes[iIndex].RaisedNodes.push_back(RaisedNode);
		RaisedNode.dZ = itYarnBound->second.second;
		m_ProjectedNodes[iIndex].RaisedNodes.push_back(RaisedNode);
	}

	SubdivideNodes(iIndex);
}

void CMesher::SubdivideNodes(int iIndex)
{
	PROJECTED_NODE &ProjectedNode = m_ProjectedNodes[iIndex];
	// Sort the nodes in ascending order of Z
	sort(ProjectedNode.RaisedNodes.begin(), ProjectedNode.RaisedNodes.end());

	double dZ1, dZ2;
	int i, j;
	bool bBottom, bTop;
	for (i=0; i<(int)ProjectedNode.RaisedNodes.size()-1; )
	{
		dZ1 = ProjectedNode.RaisedNodes[i].dZ;
		dZ2 = ProjectedNode.RaisedNodes[i+1].dZ;
		// Is Node1 the bottom-most node?
		bBottom = (i == 0);
		// Is Node2 the top-most node?
		bTop = (i == (int)ProjectedNode.RaisedNodes.size()-2);
		// Merge the boundaries that are less than a certain tolerance
		if (abs(dZ1 - dZ2) < m_dLayerMergeTolerance && !(bBottom && bTop))
		{
			ProjectedNode.RaisedNodes[i].bMerged = true;
			if (bBottom)
				ProjectedNode.RaisedNodes[i].dZ = dZ1;
			else if (bTop)
				ProjectedNode.RaisedNodes[i].dZ = dZ2;
			else
				ProjectedNode.RaisedNodes[i].dZ = 0.5*(dZ1+dZ2);
			for (j=0; j<(int)ProjectedNode.RaisedNodes[i+1].YarnBoundaryIndices.size(); ++j)
			{
				ProjectedNode.RaisedNodes[i].YarnBoundaryIndices.push_back(
					ProjectedNode.RaisedNodes[i+1].YarnBoundaryIndices[j]);
			}
			ProjectedNode.RaisedNodes.erase(ProjectedNode.RaisedNodes.begin()+i+1);
		}
		else
		{
			++i;
		}
	}

	// Divide these nodes up some..
//	double dSeed = m_dSeed;
	double dSeed = GetBestSeed(iIndex);
	double dNumDivs;
	int iNumDivisions;
	RAISED_NODE RaisedNode;
	int iSize = (int)ProjectedNode.RaisedNodes.size();
	for (i=0; i<iSize-1; ++i)
	{
		dZ1 = ProjectedNode.RaisedNodes[i].dZ;
		dZ2 = ProjectedNode.RaisedNodes[i+1].dZ;
		dNumDivs = (dZ2-dZ1)/dSeed;
		iNumDivisions = int(dNumDivs);
		if (dNumDivs-iNumDivisions > 0.5)
			++iNumDivisions;
		for (j=0; j<iNumDivisions-1; ++j)
		{
			RaisedNode.dZ = dZ1 + (j+1)*((dZ2-dZ1)/iNumDivisions);
			ProjectedNode.RaisedNodes.push_back(RaisedNode);
		}
	}

	// Sort in the new nodes that where just pushed in on the end
	sort(ProjectedNode.RaisedNodes.begin(), ProjectedNode.RaisedNodes.end());
}

double CMesher::GetBestSeed(int iIndex)
{
	list<int>::iterator itIndex;
	list<int> &Indices = m_ProjectedMesh.GetIndices(CMesh::TRI);
	int iCorner[3];
	int i;
	double dEdgeLength = 0;
	int iNumEdges = 0;
	for (itIndex = Indices.begin(), i=0; itIndex != Indices.end(); ++i)
	{
		for (i=0; i<3; ++i)
		{
			iCorner[i] = *(itIndex++);
		}
		for (i=0; i<3; ++i)
		{
			if (iCorner[i] == iIndex)
			{
				dEdgeLength += GetLength(m_ProjectedMesh.GetNode(iCorner[i]), m_ProjectedMesh.GetNode(iCorner[(i+1)%3]));
				dEdgeLength += GetLength(m_ProjectedMesh.GetNode(iCorner[i]), m_ProjectedMesh.GetNode(iCorner[(i+2)%3]));
				++iNumEdges;
				++iNumEdges;
			}
		}
	}
	double dSeed = m_dSeed;
	if (iNumEdges)
		dSeed = dEdgeLength/iNumEdges;
	return dSeed;
}

void CMesher::MeshColumn(TRIANGLE Triangle, int iRegion)
{
	vector<int> &YarnIndices = m_ProjectedRegions[iRegion].YarnIndices;
	vector<vector<RAISED_NODE> > Column1, Column2, Column3;
	// Split each column into a number of smaller columns standing on top of each
	// other separated by the yarn boundaries
	SplitColumn(m_ProjectedNodes[Triangle.i[0]], YarnIndices, Column1);
	SplitColumn(m_ProjectedNodes[Triangle.i[1]], YarnIndices, Column2);
	SplitColumn(m_ProjectedNodes[Triangle.i[2]], YarnIndices, Column3);
	int i, iNumLayers = YarnIndices.size()*2+1;
	MESHER_ELEMENT_DATA ElemData;
	ElemData.iRegion = iRegion;
	int iTopYarnIndex, iBottomYarnIndex;
	for (i=0; i<iNumLayers; ++i)
	{
		ElemData.iLayer = i;
		if (i%2 == 0)
		{
			ElemData.iYarnIndex = -1;
			iBottomYarnIndex = -1;
			iTopYarnIndex = -1; 
			if (i>0)
				iBottomYarnIndex = YarnIndices[(i/2)-1];
			if (i/2<(int)YarnIndices.size())
				iTopYarnIndex = YarnIndices[i/2];
		}
		else
		{
			iBottomYarnIndex = iTopYarnIndex = ElemData.iYarnIndex = YarnIndices[i/2];
		}
		vector<RAISED_NODE> Columns[3];
		Columns[0] = Column1[i];
		Columns[1] = Column2[i];
		Columns[2] = Column3[i];
		if (m_bQuadratic && m_bProjectMidSideNodes && iBottomYarnIndex==iTopYarnIndex && iBottomYarnIndex!=-1)
			BuildMidSideNodes(Columns, iBottomYarnIndex);
		set<pair<int, int> > EdgeConstraints[3];
		BuildEdgeConstraints(Columns, EdgeConstraints);
		int iNumTets = TetMeshColumn(Columns, EdgeConstraints);
		m_ElementData[CMesh::TET].resize(m_ElementData[CMesh::TET].size()+iNumTets, ElemData);
/*		int iNumTets = m_VolumeMesh.GetNumElements(CMesh::TET);
		int iNumPyramids = m_VolumeMesh.GetNumElements(CMesh::PYRAMID);
		int iNumWedges = m_VolumeMesh.GetNumElements(CMesh::WEDGE);
		if (!m_bHybrid)
		{
			TetMeshColumn(Columns, EdgeConstraints);
		}
		else
		{
			MixedMeshColumn(Columns, EdgeConstraints);
		}
		iNumTets = m_VolumeMesh.GetNumElements(CMesh::TET) - iNumTets;
		iNumPyramids = m_VolumeMesh.GetNumElements(CMesh::PYRAMID) - iNumPyramids;
		iNumWedges = m_VolumeMesh.GetNumElements(CMesh::WEDGE) - iNumWedges;
		if (iNumTets)
			m_ElementData[CMesh::TET].resize(m_ElementData[CMesh::TET].size()+iNumTets, ElemData);
		if (iNumPyramids)
			m_ElementData[CMesh::PYRAMID].resize(m_ElementData[CMesh::PYRAMID].size()+iNumPyramids, ElemData);
		if (iNumWedges)
			m_ElementData[CMesh::WEDGE].resize(m_ElementData[CMesh::WEDGE].size()+iNumWedges, ElemData);*/
	}
}

bool CMesher::SplitColumn(PROJECTED_NODE &Node, vector<int> &YarnIndices, vector<vector<RAISED_NODE> > &Column)
{
	int i, j, iNumLayers = YarnIndices.size()*2+1;
	Column.clear();
	Column.resize(iNumLayers);
	int iLayer = 0;
	for (i=0; i<(int)Node.RaisedNodes.size(); ++i)
	{
		Column[iLayer].push_back(Node.RaisedNodes[i]);
		for (j=0; j<(int)Node.RaisedNodes[i].YarnBoundaryIndices.size(); ++j)
		{
			if (count(YarnIndices.begin(), YarnIndices.end(), Node.RaisedNodes[i].YarnBoundaryIndices[j]))
			{
				++iLayer;
				if (iLayer < iNumLayers)
				{
					Column[iLayer].push_back(Node.RaisedNodes[i]);
				}
				else
				{
					assert(false);
					return false;
				}
			}
		}
	}
	return true;
}

void CMesher::BuildMidSideNodes(vector<RAISED_NODE> Columns[3], int iYarnIndex)
{
	int i, i1, i2;
	for (i=0; i<3; ++i)
	{
		i1 = i;
		i2 = (i+1)%3;
		if (!Columns[i1].empty() && !Columns[i2].empty())
		{
			if (!Columns[i1].front().bMerged && !Columns[i2].front().bMerged)
				BuildMidSideNode(Columns[i1].front().iIndex, Columns[i2].front().iIndex, iYarnIndex, false);
			if (!Columns[i1].back().bMerged && !Columns[i2].back().bMerged)
				BuildMidSideNode(Columns[i1].back().iIndex, Columns[i2].back().iIndex, iYarnIndex, true);
		}
	}
}

void CMesher::BuildMidSideNode(int iNodeIndex1, int iNodeIndex2, int iYarnIndex, bool bTop)
{
	if (iNodeIndex2 > iNodeIndex1)
		swap(iNodeIndex1, iNodeIndex2);

	pair<int, int> MidIndex(iNodeIndex1, iNodeIndex2);
	
	if (m_MidSideNodeLocations.count(MidIndex))
		return;

	XYZ MidPos = 0.5 * (m_VolumeMesh.GetNode(iNodeIndex1) + m_VolumeMesh.GetNode(iNodeIndex2));

	pair<double, double> YarnBounds;
	bool bFound = GetMeshVerticalBounds(m_YarnMeshes[iYarnIndex], MidPos, YarnBounds.first, YarnBounds.second, true);
	if (bFound)
	{
		if (bTop)
			MidPos.z = YarnBounds.second;
		else
			MidPos.z = YarnBounds.first;
		m_MidSideNodeLocations[MidIndex] = MidPos;
	}
	else
		assert(false);
}

XYZ CMesher::GetMidSideNode(int iNodeIndex1, int iNodeIndex2)
{
	if (iNodeIndex2 > iNodeIndex1)
		swap(iNodeIndex1, iNodeIndex2);
	pair<int, int> MidIndex(iNodeIndex1, iNodeIndex2);
	map<pair<int, int>, XYZ>::iterator itNode = m_MidSideNodeLocations.find(MidIndex);
	if (itNode != m_MidSideNodeLocations.end())
		return itNode->second;
	return 0.5 * (m_VolumeMesh.GetNode(iNodeIndex1) + m_VolumeMesh.GetNode(iNodeIndex2));
}

void CMesher::ConvertMeshToQuadratic()
{
	CMesh QuadraticMesh;
	QuadraticMesh.GetNodes() = m_VolumeMesh.GetNodes();
	const list<int> &Indices = m_VolumeMesh.GetIndices(CMesh::TET);
	list<int>::const_iterator itIndex;
	int i1, i2, i3, i4;
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		i1 = *(itIndex++);
		i2 = *(itIndex++);
		i3 = *(itIndex++);
		i4 = *(itIndex++);
		vector<int> QuadraticTet;
		QuadraticTet.push_back(i1);
		QuadraticTet.push_back(i2);
		QuadraticTet.push_back(i3);
		QuadraticTet.push_back(i4);
		QuadraticTet.push_back(QuadraticMesh.AddNode(GetMidSideNode(i1, i2)));
		QuadraticTet.push_back(QuadraticMesh.AddNode(GetMidSideNode(i2, i3)));
		QuadraticTet.push_back(QuadraticMesh.AddNode(GetMidSideNode(i3, i1)));
		QuadraticTet.push_back(QuadraticMesh.AddNode(GetMidSideNode(i1, i4)));
		QuadraticTet.push_back(QuadraticMesh.AddNode(GetMidSideNode(i2, i4)));
		QuadraticTet.push_back(QuadraticMesh.AddNode(GetMidSideNode(i3, i4)));
		QuadraticMesh.AddElement(CMesh::QUADRATIC_TET, QuadraticTet);
	}
	// Merge the nodes, because we added some new nodes that share the same location
	QuadraticMesh.MergeNodes(1e-9);
	m_VolumeMesh = QuadraticMesh;
}

void CMesher::BuildEdgeConstraints(vector<RAISED_NODE> Columns[3], set<pair<int, int> > EdgeConstraints[3])
{
	// This structure contains the list of edge constraints that must be
	// respected in order for the mesh edges to be well matched
	int k;
	int i1, i2;
	vector<RAISED_NODE>::iterator itRaisedNode1, itRaisedNode2;
	vector<int>::iterator itYarnBound1, itYarnBound2;
	map<int, int> BoundCount1;
	map<int, int> BoundCount2;
	// Conform to edge constraints caused by yarn boundaries
	for (k=0; k<3; ++k)
	{
		BoundCount1.clear();
		for (i1 = 0; i1 < (int)Columns[k].size(); ++i1)
		{
			vector<int> &YarnBounds1 = Columns[k][i1].YarnBoundaryIndices;
			for (itYarnBound1 = YarnBounds1.begin(); itYarnBound1 != YarnBounds1.end(); ++itYarnBound1)
			{
				++BoundCount1[*itYarnBound1];
				BoundCount2.clear();
				for (i2 = 0; i2 < (int)Columns[(k+1)%3].size(); ++i2)
				{
					vector<int> &YarnBounds2 = Columns[(k+1)%3][i2].YarnBoundaryIndices;
					for (itYarnBound2 = YarnBounds2.begin(); itYarnBound2 != YarnBounds2.end(); ++itYarnBound2)
					{
						++BoundCount2[*itYarnBound2];
						if (*itYarnBound1 == *itYarnBound2 &&
							BoundCount1[*itYarnBound1] == BoundCount2[*itYarnBound2])
//							*itYarnBound1 != iTopYarnIndex &&
//							*itYarnBound1 != iBottomYarnIndex)
						{
							EdgeConstraints[k].insert(make_pair(i1, i2));
						}
					}
				}
			}
		}
	}

	// Conform to edge constraints caused by existing elements
	pair<int, int> Edge;
	for (k=0; k<3; ++k)
	{
		for (i1 = 0; i1 < (int)Columns[k].size(); ++i1)
		{
			for (i2 = 0; i2 < (int)Columns[(k+1)%3].size(); ++i2)
			{
				Edge.first = Columns[k][i1].iIndex;
				Edge.second = Columns[(k+1)%3][i2].iIndex;
				if (Edge.first > Edge.second)
					swap(Edge.first, Edge.second);
				if (m_EdgeConstraints.count(Edge))
				{
					EdgeConstraints[k].insert(make_pair(i1, i2));
				}
			}
		}
	}

}

int CMesher::TetMeshColumn(vector<RAISED_NODE> Columns[3], set<pair<int, int> > EdgeConstraints[3])
{
/*	vector<PROJECTED_NODE*> Nodes;
	Nodes.push_back(&m_ProjectedNodes[iColumn1]);
	Nodes.push_back(&m_ProjectedNodes[iColumn2]);
	Nodes.push_back(&m_ProjectedNodes[iColumn3]);*/


	int i, i1, i2;

	int h[3] = {0, 0, 0};
	int Sizes[3];
	for (i=0; i<3; ++i)
	{
		Sizes[i] = (int)Columns[i].size();
	}

	double dZ, dZMin;
	int iMinIndex;
	int iNumElements = 0;
	set<pair<int, int> >::iterator itEdge;
	bool bEdgeConstraintViolation;
	while (h[0] < Sizes[0] && h[1] < Sizes[1] && h[2] < Sizes[2])
	{
		iMinIndex = -1;
		for (i=0; i<3; ++i)
		{
			i1 = (i+1)%3;
			i2 = (i+2)%3;
			if (h[i]+1 < Sizes[i])
			{
				dZ = Columns[i][h[i]+1].dZ;
				if (iMinIndex==-1 || dZ < dZMin)
				{
					// Check if adding this as a tetrahedron would violate any of the edge constraints
					bEdgeConstraintViolation = ViolatesEdgeConstraint(EdgeConstraints[i], EdgeConstraints[i2], h[i], h[i1], h[i2]);
/*					bEdgeConstraintViolation = false;
					for (itEdge = EdgeConstraints[i].begin(); itEdge != EdgeConstraints[i].end(); ++itEdge)
					{
						if (h[i] >= itEdge->first && h[(i+1)%3] < itEdge->second)
						{
							bEdgeConstraintViolation = true;
						}
					}
					for (itEdge = EdgeConstraints[(i+2)%3].begin(); itEdge != EdgeConstraints[(i+2)%3].end(); ++itEdge)
					{
						if (h[i] >= itEdge->second && h[(i+2)%3] < itEdge->first)
						{
							bEdgeConstraintViolation = true;
						}
					}*/
					if (!bEdgeConstraintViolation)
					{
						dZMin = dZ;
						iMinIndex = i;
					}
				}
			}
		}
		if (iMinIndex == -1)
		{
			// This tet can't be meshed directly due to the edge constraints
//			if (h[0]+1 < Sizes[0] && h[1]+1 < Sizes[1] && h[2]+1 < Sizes[2])
			{
				int Limits[6];
				Limits[0] = Limits[3] = h[0];
				Limits[1] = Limits[4] = h[1];
				Limits[2] = Limits[5] = h[2];
				bool bChanges;
				// Identify which region is going to be hard to mesh and store its limits in the Limits
				// array.
				do
				{
					bChanges = false;
					for (i=0; i<3; ++i)
					{
						for (itEdge = EdgeConstraints[i].begin(); itEdge != EdgeConstraints[i].end(); ++itEdge)
						{
							if (Limits[i] <= itEdge->first && itEdge->first < max(Limits[i+3], Limits[i]+1) && 
								Limits[3+(i+1)%3] < itEdge->second)
							{
								Limits[3+(i+1)%3] = itEdge->second;
								bChanges = true;
							}
						}
						for (itEdge = EdgeConstraints[(i+2)%3].begin(); itEdge != EdgeConstraints[(i+2)%3].end(); ++itEdge)
						{
							if (Limits[i] <= itEdge->second && itEdge->second < max(Limits[i+3], Limits[i]+1) &&
								Limits[3+(i+2)%3] < itEdge->first)
							{
								Limits[3+(i+2)%3] = itEdge->first;
								bChanges = true;
							}
						}
					}
				} while (bChanges);

				if (Limits[0] == Limits[3] &&
					Limits[1] == Limits[4] &&
					Limits[2] == Limits[5])
					break;	// We don't have anything to mesh here, game over...

				// Mesh it by adding an additional point to the mesh
				iNumElements += MeshDifficultRegion(Columns, Limits, EdgeConstraints);

				// Done! move up to the next layer
				h[0] = Limits[3];
				h[1] = Limits[4];
				h[2] = Limits[5];
			}
/*			else
			{
//				assert(false);
				break;
			}*/
		}
		else
		{
			vector<int> Indices;
			for (i=0; i<3; ++i)
			{
				Indices.push_back(Columns[i][h[i]].iIndex);
			}
			++h[iMinIndex];
			Indices.push_back(Columns[iMinIndex][h[iMinIndex]].iIndex);

			AddElement(CMesh::TET, Indices);
//			copy(Indices.begin(), Indices.end(), back_inserter(m_VolumeMesh.GetIndices(CMesh::TET)));

			++iNumElements;
		}
	}
	return iNumElements;
}

int CMesher::MeshDifficultRegion(vector<RAISED_NODE> Columns[3], int Limits[6], set<pair<int, int> > EdgeConstraints[3])
{
	XYZ Center;
	int i, j;
	double dMinZ, dMaxZ;
	// Use projected triangle center for x and y components
	for (i=0; i<3; ++i)
	{
		Center += m_VolumeMesh.GetNode(Columns[i][Limits[i]].iIndex);
	}
	Center /= 3;
	// Find maximum and lowest Z, use the average as center z coordinate
	dMinZ = dMaxZ = Columns[0][Limits[0]].dZ;
	for (i=0; i<3; ++i)
	{
		for (j=Limits[i]; j<=Limits[i+3]; ++j)
		{
			if (Columns[i][j].dZ < dMinZ)
				dMinZ = Columns[i][j].dZ;
			else if (Columns[i][j].dZ > dMaxZ)
				dMaxZ = Columns[i][j].dZ;
		}
	}
	Center.z = 0.5*(dMinZ + dMaxZ);

	int iCenter = (int)m_VolumeMesh.GetNumNodes();
	m_VolumeMesh.AddNode(Center);

	set<pair<int, int> >::iterator itEdge;

	int iElementsCreated = 0;
	// Bottom tet
	{
		vector<int> Indices;
		Indices.push_back(Columns[0][Limits[0]].iIndex);
		Indices.push_back(Columns[1][Limits[1]].iIndex);
		Indices.push_back(Columns[2][Limits[2]].iIndex);
		Indices.push_back(iCenter);
		AddElement(CMesh::TET, Indices);
	}
	++iElementsCreated;
	int h1, h2;
	bool bEdgeConstraintViolation;
	// Side tets
	for (i=0; i<3; ++i)
	{
		h1 = Limits[i];
		h2 = Limits[(i+1)%3];
		while (h1 <= Limits[i+3])
		{
			while (h2 < Limits[(i+1)%3+3])
			{
				// If there is no edge constraint between higher than h1 to h2 or lower build a tet
				bEdgeConstraintViolation = false;
				for (itEdge = EdgeConstraints[i].begin(); itEdge != EdgeConstraints[i].end(); ++itEdge)
				{
					if (h1 < itEdge->first && h2 >= itEdge->second)
					{
						bEdgeConstraintViolation = true;
					}
				}
				if (bEdgeConstraintViolation)
					break;
				vector<int> Indices;
				Indices.push_back(iCenter);
				Indices.push_back(Columns[i][h1].iIndex);
				Indices.push_back(Columns[(i+1)%3][h2].iIndex);
				Indices.push_back(Columns[(i+1)%3][h2+1].iIndex);
				AddElement(CMesh::TET, Indices);
				++iElementsCreated;
				++h2;
			}
			if (h1 < Limits[i+3] && h2 <= Limits[(i+1)%3+3])
			{
				// If there is no edge constraint between higher than h2 to h1 or lower build a tet
				bEdgeConstraintViolation = false;
				for (itEdge = EdgeConstraints[i].begin(); itEdge != EdgeConstraints[i].end(); ++itEdge)
				{
					if (h1 >= itEdge->first && h2 < itEdge->second)
					{
						bEdgeConstraintViolation = true;
					}
				}
				if (!bEdgeConstraintViolation)
				{
					vector<int> Indices;
					Indices.push_back(iCenter);
					Indices.push_back(Columns[i][h1].iIndex);
					Indices.push_back(Columns[(i+1)%3][h2].iIndex);
					Indices.push_back(Columns[i][h1+1].iIndex);
					AddElement(CMesh::TET, Indices);
					++iElementsCreated;
				}
				else
				{
					assert(false);	// should never get here
				}
			}
			++h1;
		}
	}
	// Top tet
	{
		vector<int> Indices;
		Indices.push_back(iCenter);
		Indices.push_back(Columns[0][Limits[3]].iIndex);
		Indices.push_back(Columns[1][Limits[4]].iIndex);
		Indices.push_back(Columns[2][Limits[5]].iIndex);
		AddElement(CMesh::TET, Indices);
	}
	++iElementsCreated;

	return iElementsCreated;

}
/*
int CMesher::MixedMeshColumn(vector<RAISED_NODE> Columns[3], set<pair<int, int> > EdgeConstraints[3])
{
	int h[3] = {0, 0, 0};
	int Sizes[3];
	int i, i1, i2, j;
	for (i=0; i<3; ++i)
	{
		Sizes[i] = (int)Columns[i].size();
	}

	set<pair<int, int> >::iterator itEdge;
	int iNumElements = 0;
//	while (h[0] < Sizes[0] && h[1] < Sizes[1] && h[2] < Sizes[2])
	while (true)
	{
		bool up[3] = {true, true, true};
		bool bChanges;
		do
		{
			bChanges = false;
			for (i=0; i<3; ++i)
			{
				if (!up[i])
					continue;
				i1 = (i+1)%3;
				i2 = (i+2)%3;
				if (h[i]+1 >= Sizes[i] ||
					ShouldConnect(Columns[i], Columns[i1], h[i], h[i1]+1) ||
					ShouldConnect(Columns[i], Columns[i2], h[i], h[i2]+1) || 
					ViolatesEdgeConstraint(EdgeConstraints[i], EdgeConstraints[i2], h[i], h[i1], h[i2]))
				{
					up[i] = false;
					bChanges = true;
				}
			}
		} while(bChanges);
		int UpCount = count(up, up+3, true);
		if (UpCount == 0)
		{
			for (i=0; i<3; ++i)
			{
				if (h[i]+1 != Sizes[i])
					assert(false);
			}
			break;
//			THIS IS DIRECTLY PASTED FROM TETMESH, FIX IT UP...
//
//			int Limits[6];
//			Limits[0] = Limits[3] = h[0];
//			Limits[1] = Limits[4] = h[1];
//			Limits[2] = Limits[5] = h[2];
//			bool bChanges;
//			// Identify which region is going to be hard to mesh and store its limits in the Limits
//			// array.
//			do
//			{
//				bChanges = false;
//				for (i=0; i<3; ++i)
//				{
//					for (itEdge = EdgeConstraints[i].begin(); itEdge != EdgeConstraints[i].end(); ++itEdge)
//					{
//						if (Limits[i] <= itEdge->first && itEdge->first < max(Limits[i+3], Limits[i]+1) && 
//							Limits[3+(i+1)%3] < itEdge->second)
//						{
//							Limits[3+(i+1)%3] = itEdge->second;
//							bChanges = true;
//						}
//					}
//					for (itEdge = EdgeConstraints[(i+2)%3].begin(); itEdge != EdgeConstraints[(i+2)%3].end(); ++itEdge)
//					{
//						if (Limits[i] <= itEdge->second && itEdge->second < max(Limits[i+3], Limits[i]+1) &&
//							Limits[3+(i+2)%3] < itEdge->first)
//						{
//							Limits[3+(i+2)%3] = itEdge->first;
//							bChanges = true;
//						}
//					}
//				}
//			} while (bChanges);
//
//			if (Limits[0] == Limits[3] &&
//				Limits[1] == Limits[4] &&
//				Limits[2] == Limits[5])
//				break;	// We don't have anything to mesh here, game over...
//
//			// Mesh it by adding an additional point to the mesh
//			iNumElements += MeshDifficultRegion(Columns, Limits, EdgeConstraints);
//
//			// Done! move up to the next layer
//			h[0] = Limits[3];
//			h[1] = Limits[4];
//			h[2] = Limits[5];
//			continue;
		}

		switch (UpCount)
		{
		case 3:
			{
				// Mesh with wedge elements
//				list<int> &Indices = m_VolumeMesh.GetIndices(CMesh::WEDGE);
				vector<int> Indices;
				for (i=0; i<3; ++i)
				{
					Indices.push_back(Columns[i][h[i]+1].iIndex);
				}
				for (i=0; i<3; ++i)
				{
					Indices.push_back(Columns[i][h[i]].iIndex);
				}
				AddElement(CMesh::WEDGE, Indices);
			}
			break;
		case 2:
			{
				// Mesh with pyramid elements
//				list<int> &Indices = m_VolumeMesh.GetIndices(CMesh::PYRAMID);
				vector<int> Indices;
				int iNoUp = 0;
				for (i=0; i<3; ++i)
				{
					if (!up[i])
						iNoUp = i;
				}
				i1 = (iNoUp+2)%3;
				i2 = (iNoUp+1)%3;
				Indices.push_back(Columns[i1][h[i1]].iIndex);
				Indices.push_back(Columns[i2][h[i2]].iIndex);

				Indices.push_back(Columns[i2][h[i2]+1].iIndex);
				Indices.push_back(Columns[i1][h[i1]+1].iIndex);

				Indices.push_back(Columns[iNoUp][h[iNoUp]].iIndex);
				AddElement(CMesh::PYRAMID, Indices);
			}
			break;
		case 1:
			{
				// Mesh with wedge elements
//				list<int> &Indices = m_VolumeMesh.GetIndices(CMesh::TET);
				vector<int> Indices;
				for (i=0; i<3; ++i)
				{
					Indices.push_back(Columns[i][h[i]].iIndex);
				}
				for (i=0; i<3; ++i)
				{
					if (up[i])
					{
						Indices.push_back(Columns[i][h[i]+1].iIndex);
					}
				}
				AddElement(CMesh::TET, Indices);
			}
			break;
		}

		++iNumElements;

		for (i=0; i<3; ++i)
		{
			if (up[i])
				++h[i];
		}
	}
	return iNumElements;
}
*/
void CMesher::AddElement(CMesh::ELEMENT_TYPE Type, const vector<int> &Indices)
{
	if (!m_VolumeMesh.AddElement(Type, Indices))
		return;
	
	switch (Type)
	{
	case CMesh::TET:
		{
			NODE_PAIR NodePair;
			AddEdgeConstraint(Indices[0], Indices[1]);
			if ( GetPairIndices( Indices[0], Indices[1], NodePair ) )
				AddEdgeConstraint( NodePair.first, NodePair.second );
			AddEdgeConstraint(Indices[0], Indices[2]);
			if ( GetPairIndices( Indices[0], Indices[2], NodePair ) )
				AddEdgeConstraint( NodePair.first, NodePair.second );
			AddEdgeConstraint(Indices[0], Indices[3]);
			if ( GetPairIndices( Indices[0], Indices[3], NodePair ) )
				AddEdgeConstraint( NodePair.first, NodePair.second );
			AddEdgeConstraint(Indices[1], Indices[2]);
			if ( GetPairIndices( Indices[1], Indices[2], NodePair ) )
				AddEdgeConstraint( NodePair.first, NodePair.second );
			AddEdgeConstraint(Indices[1], Indices[3]);
			if ( GetPairIndices( Indices[1], Indices[3], NodePair ) )
				AddEdgeConstraint( NodePair.first, NodePair.second );
			AddEdgeConstraint(Indices[2], Indices[3]);
			if ( GetPairIndices( Indices[2], Indices[3], NodePair ) )
				AddEdgeConstraint( NodePair.first, NodePair.second );
			break;
		}
	case CMesh::PYRAMID:
		AddEdgeConstraint(Indices[0], Indices[1]);
		AddEdgeConstraint(Indices[1], Indices[2]);
		AddEdgeConstraint(Indices[2], Indices[3]);
		AddEdgeConstraint(Indices[3], Indices[0]);

		AddEdgeConstraint(Indices[0], Indices[4]);
		AddEdgeConstraint(Indices[1], Indices[4]);
		AddEdgeConstraint(Indices[2], Indices[4]);
		AddEdgeConstraint(Indices[3], Indices[4]);
		break;
	case CMesh::WEDGE:
		AddEdgeConstraint(Indices[0], Indices[1]);
		AddEdgeConstraint(Indices[1], Indices[2]);
		AddEdgeConstraint(Indices[2], Indices[0]);

		AddEdgeConstraint(Indices[3], Indices[4]);
		AddEdgeConstraint(Indices[4], Indices[5]);
		AddEdgeConstraint(Indices[5], Indices[3]);

		AddEdgeConstraint(Indices[0], Indices[3]);
		AddEdgeConstraint(Indices[1], Indices[4]);
		AddEdgeConstraint(Indices[2], Indices[5]);
		break;
	}
}

void CMesher::AddEdgeConstraint(int i1, int i2)
{
	pair<int, int> EdgeConstraint(i1, i2);
	if (EdgeConstraint.first > EdgeConstraint.second)
		swap(EdgeConstraint.first, EdgeConstraint.second);
	m_EdgeConstraints.insert(EdgeConstraint);
}

bool CMesher::ViolatesEdgeConstraint(const set<pair<int, int> > &EdgeConstraints1, const set<pair<int, int> > &EdgeConstraints2, int h, int h1, int h2)
{
	set<pair<int, int> >::const_iterator itEdge;
	for (itEdge = EdgeConstraints1.begin(); itEdge != EdgeConstraints1.end(); ++itEdge)
	{
		if (h >= itEdge->first && h1 < itEdge->second)
		{
			return true;
		}
	}
	for (itEdge = EdgeConstraints2.begin(); itEdge != EdgeConstraints2.end(); ++itEdge)
	{
		if (h >= itEdge->second && h2 < itEdge->first)
		{
			return true;
		}
	}
	return false;
}

bool CMesher::ShouldConnect(vector<RAISED_NODE> &Column1, vector<RAISED_NODE> &Column2, int h1, int h2)
{
	if (h1 >= (int)Column1.size())
		return false;
	if (h2 >= (int)Column2.size())
		return false;

	double dDist = abs(Column1[h1].dZ - Column2[h2].dZ);
	bool bClosest = true;
	int j;
	for (j=0; j<(int)Column1.size(); ++j)
	{
		if (j == h1)
			continue;
		if (abs(Column1[j].dZ - Column2[h2].dZ) <= dDist)
		{
			bClosest = false;
			break;
		}
	}
	if (bClosest)
		return true;
	bClosest = true;
	for (j=0; j<(int)Column2.size(); ++j)
	{
		if (j == h2)
			continue;
		if (abs(Column1[h1].dZ - Column2[j].dZ) <= dDist)
		{
			bClosest = false;
			break;
		}
	}
	return bClosest;
}

void CMesher::FillYarnTangentsData()
{
	if (!m_pTextile)
		return;
	const CDomain* pDomain = m_pTextile->GetDomain();
	// This value may need tweaking
	//double dTolerance = 1e-1;
	double dTolerance = 1e-5;
	list<MESHER_ELEMENT_DATA>::iterator itElementData;
	list<int>::const_iterator itIndex;
	int i, iNumNodes;
	int iNumYarns = m_pTextile->GetNumYarns();
	bool bInside;
	vector<vector<XYZ> > YarnTranslations;
	YarnTranslations.resize(iNumYarns);
	if (pDomain)
	{
		for (i=0; i<iNumYarns; ++i)
		{
			YarnTranslations[i] = pDomain->GetTranslations(*m_pTextile->GetYarn(i));
		}
	}
	int j;
	for (j = 0; j < CMesh::NUM_ELEMENT_TYPES; ++j)
	{
		list<int> &Indices = m_VolumeMesh.GetIndices((CMesh::ELEMENT_TYPE)j);
		list<MESHER_ELEMENT_DATA> &ElementData = m_ElementData[(CMesh::ELEMENT_TYPE)j];
		iNumNodes = CMesh::GetNumNodes((CMesh::ELEMENT_TYPE)j);
		for (itIndex = Indices.begin(), itElementData = ElementData.begin(); itIndex != Indices.end() && itElementData != ElementData.end(); ++itElementData)
		{
			XYZ AvgPos;
			for (i=0; i<iNumNodes; ++i)
			{
				AvgPos += m_VolumeMesh.GetNode(*(itIndex++));
			}
			AvgPos /= iNumNodes;

			if (itElementData->iYarnIndex >= 0 && itElementData->iYarnIndex < iNumYarns)
			{
				CYarn* pYarn = m_pTextile->GetYarn(itElementData->iYarnIndex);
				bInside = pYarn->PointInsideYarn(AvgPos, YarnTranslations[itElementData->iYarnIndex], &itElementData->YarnTangent, &itElementData->Location, &itElementData->dVolumeFraction, &itElementData->dSurfaceDistance, dTolerance, &itElementData->Orientation, &itElementData->Up);
				//assert(bInside);
				if ( !bInside )
					TGLOG("PointInsideYarn false");
			}
		}
	}
}

const list<MESHER_ELEMENT_DATA> *CMesher::GetElementData(CMesh::ELEMENT_TYPE ElementType)
{
	if (ElementType < 0 || ElementType >= CMesh::NUM_ELEMENT_TYPES)
	{
		TGERROR("Unable to get element data, invalid element type: " << ElementType);
		return NULL;
	}
	return &m_ElementData[ElementType];
}

bool CMesher::GetPairIndices(int iIndex1, int iIndex2, NODE_PAIR &MatchPair)
{
	NODE_PAIR_SETS::iterator itSets = m_NodePairSets.begin();
	bool bFoundFirst = false, bFoundSecond = false;

	// Iterate through node pair sets until find one which contains both nodes (node pair set contains pairs on opposite boundaries)
	while ( itSets != m_NodePairSets.end() && !(bFoundFirst && bFoundSecond) ) 
	{
		NODE_PAIR_SET::iterator itNodePairSet = (*itSets).begin();
		bFoundFirst = false;
		bFoundSecond = false;
		while( itNodePairSet != (*itSets).end() && !(bFoundFirst && bFoundSecond) ) // Iterate through node pair set until found match for both nodes
		{
			if (!bFoundFirst)
			{
				if ( (*itNodePairSet).first == iIndex1 )
				{
					MatchPair.first = (*itNodePairSet).second;
					bFoundFirst = true;
				}
				else if ( (*itNodePairSet).second == iIndex1 )
				{
					MatchPair.first = (*itNodePairSet).first;
					bFoundFirst = true;
				}
			}
			if ( !bFoundSecond )
			{
				if ( (*itNodePairSet).first == iIndex2 )
				{
					MatchPair.second = (*itNodePairSet).second;
					bFoundSecond = true;
				}
				else if ( (*itNodePairSet).second == iIndex2 )
				{
					MatchPair.second = (*itNodePairSet).first;
					bFoundSecond = true;
				}
			}
			++itNodePairSet;
		}
		++itSets;
	}
	return ( bFoundFirst && bFoundSecond );
}

void CMesher::GetEdgePairIndices(const NODE_PAIR_SETS &NodePairSets, int iIndex, set<int> &Match)
{
	NODE_PAIR_SETS::const_iterator itSets = NodePairSets.begin();
	int iNumFound = 0;

	while ( itSets != NodePairSets.end() )
	{
		NODE_PAIR_SET::const_iterator itNodePairSet = (*itSets).begin();
		pair<set<int>::iterator, bool> ret;
		bool bFound = false;

		while( itNodePairSet != (*itSets).end() && !bFound )
		{
			if ( (*itNodePairSet).first == iIndex )
			{
				ret = Match.insert( (*itNodePairSet).second );
				if ( ret.second )  // Returns true in second if new element was added
				{
					bFound = true;
					iNumFound++;
				}
			}
			else if ( (*itNodePairSet).second == iIndex )
			{
				ret = Match.insert( (*itNodePairSet).first );
				if ( ret.second )
				{
					bFound = true;
					iNumFound++;
				}
			}
			++itNodePairSet;
		}
		++itSets;
	}
	if ( iNumFound > 1 )  // If found more than one must be a corner so call function again to get other corner node
	{
		GetEdgePairIndices( NodePairSets, *(Match.begin()), Match );
	}
}

void CMesher::SortPairs( NODE_PAIR_SET &NodePairs, bool bSwapY )
{
	NODE_PAIR_SET::iterator itNodePairs;

	for ( itNodePairs = NodePairs.begin(); itNodePairs != NodePairs.end(); ++itNodePairs )
	{
		XYZ Node1 = m_ProjectedMesh.GetNode((*itNodePairs).first);
		XYZ Node2 = m_ProjectedMesh.GetNode((*itNodePairs).second);
		if (bSwapY )
		{
			if ( Node1.y > Node2.y )
			{
				swap( (*itNodePairs).first, (*itNodePairs).first );
			}
		}
		else
		{
			if ( Node1.x > Node2.x )
			{
				swap( (*itNodePairs).first, (*itNodePairs).first );
			}
		}
	}
}

void CMesher::CreateNodeSets( NODE_PAIR_SETS &EdgeNodePairSets, set<int> &CornerIndex, const vector<XYZ> &Repeats )
{
	// Find boundary node pair sets
	m_NodePairSets.clear();
	NODE_PAIR_SETS::iterator  itEdgeNodeSets;
	
	set<int> CompletedCorners;
	set<int> EdgeIndices;

	m_Edges.resize(12);
	m_Vertices.resize(8);
	int j = 0;

	for ( itEdgeNodeSets = EdgeNodePairSets.begin(); itEdgeNodeSets != EdgeNodePairSets.end(); ++itEdgeNodeSets )
	{
		NODE_PAIR_SET::iterator itNodePairs;
		NODE_PAIR_SET VolMeshPairs;
		VolMeshPairs.clear();

		bool bConstX = (Repeats[j++].y == 0);

		for ( itNodePairs = (*itEdgeNodeSets).begin(); itNodePairs != (*itEdgeNodeSets).end(); ++itNodePairs )
		{
			// Indices in the pairs in the edge node sets correspond to index into m_Projected Nodes
			NODE_PAIR NodePair;
			vector<RAISED_NODE>::iterator  itRaisedNodes;
			int ind1 = (*itNodePairs).first;
			int ind2 = (*itNodePairs).second;
			int size = m_ProjectedNodes[ind1].RaisedNodes.size();
			EdgeIndices.insert(ind1);
			EdgeIndices.insert(ind2);

			assert( size == m_ProjectedNodes[ind2].RaisedNodes.size() );
			if ( CornerIndex.find(ind1) != CornerIndex.end() )  // Is corner node
			{
				if ( CompletedCorners.find(ind1) == CompletedCorners.end() )  // Check if already entered nodes
				{
					vector<int> Edge1, Edge2;
					for ( int i = 1; i < size-1; ++i )
					{
						Edge1.push_back(m_ProjectedNodes[ind1].RaisedNodes[i].iIndex + 1);
						Edge2.push_back(m_ProjectedNodes[ind2].RaisedNodes[i].iIndex + 1);
					}
					if ( m_Edges[0].empty() )
					{
						m_Edges[0] = Edge1;
						m_Edges[1] = Edge2;
					}
					else
					{
						m_Edges[2] = Edge2;
						m_Edges[3] = Edge1;
					}

					int StartInd = 0;
					if ( !CompletedCorners.empty() )
					{
						StartInd = 2;
						swap(ind1,ind2);
					}
					m_Vertices[StartInd++] = (m_ProjectedNodes[ind1].RaisedNodes[0].iIndex + 1);
					m_Vertices[StartInd++] = (m_ProjectedNodes[ind2].RaisedNodes[0].iIndex + 1);
					StartInd += 2;
					
					m_Vertices[StartInd++] = (m_ProjectedNodes[ind1].RaisedNodes[size-1].iIndex + 1);
					m_Vertices[StartInd] = (m_ProjectedNodes[ind2].RaisedNodes[size-1].iIndex + 1);
					
					CompletedCorners.insert(ind1);
					CompletedCorners.insert(ind2);
				}
			}
			else
			{
				int StartInd = bConstX ? 4 : 8;
				m_Edges[StartInd++].push_back(m_ProjectedNodes[ind1].RaisedNodes[0].iIndex + 1);
				m_Edges[StartInd++].push_back(m_ProjectedNodes[ind2].RaisedNodes[0].iIndex + 1);
				m_Edges[StartInd++].push_back(m_ProjectedNodes[ind2].RaisedNodes[size-1].iIndex + 1);
				m_Edges[StartInd++].push_back(m_ProjectedNodes[ind1].RaisedNodes[size-1].iIndex + 1);
				
				for ( int i = 1; i < size-1; ++i )
				{
					if ( bConstX )
					{
						m_FaceA.push_back(m_ProjectedNodes[ind2].RaisedNodes[i].iIndex + 1);
						m_FaceB.push_back(m_ProjectedNodes[ind1].RaisedNodes[i].iIndex + 1);
					}
					else
					{
						m_FaceC.push_back(m_ProjectedNodes[ind2].RaisedNodes[i].iIndex + 1);
						m_FaceD.push_back(m_ProjectedNodes[ind1].RaisedNodes[i].iIndex + 1);
					}
				}
			}
			// Iterate through the column of nodes and match up the corresponding nodes for each of the pair	
			for ( int i = 0; i < m_ProjectedNodes[ind1].RaisedNodes.size(); ++i )
			{
				NodePair.first = m_ProjectedNodes[ind1].RaisedNodes[i].iIndex + 1;  // RaisedNodes[i].iIndex contains index in the volume mesh
				NodePair.second = m_ProjectedNodes[ind2].RaisedNodes[i].iIndex + 1;
				VolMeshPairs.push_back( NodePair );
			}
		}
		m_NodePairSets.push_back( VolMeshPairs );
	}
	

	int Size = m_ProjectedNodes.size();
	for ( int i = 0; i < Size; ++i )
	{
		if ( EdgeIndices.find(i) == EdgeIndices.end() )
		{
			m_FaceE.push_back(m_ProjectedNodes[i].RaisedNodes[m_ProjectedNodes[i].RaisedNodes.size()-1].iIndex + 1);
			m_FaceF.push_back(m_ProjectedNodes[i].RaisedNodes[0].iIndex + 1);
		}
	}
}

bool CMesher::SaveNodeSets()
{
	if (m_Vertices.size() != 8 )
		return false;
	for ( int i = 0; i < 8; ++i )
	{
		m_PeriodicBoundaries->SetVertex(m_Vertices[i]);
	}

	if ( m_Edges.size() != 12 )
		return false;
	for ( int i = 0; i < 12; ++i )
	{
		m_PeriodicBoundaries->SetEdges( m_Edges[i] );
	}

	if ( m_FaceA.size() == 0 || m_FaceB.size() == 0 || m_FaceC.size() == 0 || m_FaceD.size() == 0
		|| m_FaceE.size() == 0 || m_FaceF.size() == 0 )
		return false;
	m_PeriodicBoundaries->SetFaceA(m_FaceA, m_FaceB);
	m_PeriodicBoundaries->SetFaceB(m_FaceC, m_FaceD);
	m_PeriodicBoundaries->SetFaceC(m_FaceE, m_FaceF);

	return true;
}

void CMesher::AddQuadraticNodesToSets()
{
	vector< set<int> > FaceSets;
	SetupFaceSets( FaceSets );

	int NumNodes = m_VolumeMesh.GetNumNodes();
	map<int,vector<int> > FaceSetIndices;

	for ( int i = 1; i <= NumNodes; ++i )
	{
		vector<int> FaceSet = FindFaceSets( FaceSets, i );
		if ( FaceSet.size() > 0 )
		{
			FaceSetIndices[i] = FaceSet;
		}
	}

	const list<int> &Indices = m_VolumeMesh.GetIndices(CMesh::QUADRATIC_TET);
	NumNodes = CMesh::GetNumNodes(CMesh::QUADRATIC_TET);
	list<int>::const_iterator itIndex;
	map<int,vector<int> >::iterator itEndIndices = FaceSetIndices.end();
	
	vector<int> iIndex;
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		list<int> Faces;

		iIndex.clear();
		list<int>::const_iterator itEndIndex = itIndex;
		advance( itEndIndex, NumNodes);
		iIndex.insert( iIndex.begin(), itIndex, itEndIndex );
		for ( int i = 0; i < 4; ++i )
		{
			if ( FaceSetIndices.find(iIndex[i]+1) == itEndIndices )
				continue;  // Node not on surface therefore mid node won't be either
			
			for ( int j = i+1; j < 4; ++j )
			{
				
				if ( FaceSetIndices.find(iIndex[j]+1) == itEndIndices )
					continue;  // Node not on surface
				AddQuadNodeToSet( i, j, FaceSetIndices[iIndex[i]+1], FaceSetIndices[iIndex[j]+1], iIndex );
				
			}
		}
		
		advance(itIndex, NumNodes);
	}
	RemoveDuplicateNodes();
}

void CMesher::SetupFaceSets( vector< set<int> >& FaceSets )
{
	set<int> FaceSet;
	FaceSet.insert( m_FaceA.begin(), m_FaceA.end() );
	FaceSet.insert( m_Edges[1].begin(), m_Edges[1].end() );
	FaceSet.insert( m_Edges[2].begin(), m_Edges[2].end() );
	FaceSet.insert( m_Edges[5].begin(), m_Edges[5].end() );
	FaceSet.insert(m_Edges[6].begin(), m_Edges[6].end() );
	FaceSet.insert( m_Vertices[1] );
	FaceSet.insert( m_Vertices[2] );
	FaceSet.insert( m_Vertices[5] );
	FaceSet.insert( m_Vertices[6] );
	FaceSets.push_back( FaceSet );

	FaceSet.clear();
	FaceSet.insert( m_FaceB.begin(), m_FaceB.end() );
	FaceSet.insert( m_Edges[0].begin(), m_Edges[0].end() );
	FaceSet.insert( m_Edges[3].begin(), m_Edges[3].end() );
	FaceSet.insert( m_Edges[4].begin(), m_Edges[4].end() );
	FaceSet.insert( m_Edges[7].begin(), m_Edges[7].end() );
	FaceSet.insert( m_Vertices[0] );
	FaceSet.insert( m_Vertices[3] );
	FaceSet.insert( m_Vertices[4] );
	FaceSet.insert( m_Vertices[7] );
	FaceSets.push_back( FaceSet );

	FaceSet.clear();
	FaceSet.insert( m_FaceC.begin(), m_FaceC.end() );
	FaceSet.insert( m_Edges[2].begin(), m_Edges[2].end() );
	FaceSet.insert( m_Edges[3].begin(), m_Edges[3].end() );
	FaceSet.insert( m_Edges[9].begin(), m_Edges[9].end() );
	FaceSet.insert( m_Edges[10].begin(), m_Edges[10].end() );
	FaceSet.insert( m_Vertices[2] );
	FaceSet.insert( m_Vertices[3] );
	FaceSet.insert( m_Vertices[6] );
	FaceSet.insert( m_Vertices[7] );
	FaceSets.push_back( FaceSet );

	FaceSet.clear();
	FaceSet.insert( m_FaceD.begin(), m_FaceD.end() );
	FaceSet.insert( m_Edges[0].begin(), m_Edges[0].end() );
	FaceSet.insert( m_Edges[1].begin(), m_Edges[1].end() );
	FaceSet.insert( m_Edges[8].begin(), m_Edges[8].end() );
	FaceSet.insert( m_Edges[11].begin(), m_Edges[11].end() );
	FaceSet.insert( m_Vertices[0] );
	FaceSet.insert( m_Vertices[1] );
	FaceSet.insert( m_Vertices[4] );
	FaceSet.insert( m_Vertices[5] );
	FaceSets.push_back( FaceSet );

	FaceSet.clear();
	FaceSet.insert( m_FaceE.begin(), m_FaceE.end() );
	FaceSet.insert( m_Edges[6].begin(), m_Edges[6].end() );
	FaceSet.insert( m_Edges[7].begin(), m_Edges[7].end() );
	FaceSet.insert( m_Edges[10].begin(), m_Edges[10].end() );
	FaceSet.insert( m_Edges[11].begin(), m_Edges[11].end() );
	FaceSet.insert( m_Vertices[4] );
	FaceSet.insert( m_Vertices[5] );
	FaceSet.insert( m_Vertices[6] );
	FaceSet.insert( m_Vertices[7] );
	FaceSets.push_back( FaceSet );

	FaceSet.clear();
	FaceSet.insert( m_FaceF.begin(), m_FaceF.end() );
	FaceSet.insert( m_Edges[4].begin(), m_Edges[4].end() );
	FaceSet.insert( m_Edges[5].begin(), m_Edges[5].end() );
	FaceSet.insert( m_Edges[8].begin(), m_Edges[8].end() );
	FaceSet.insert( m_Edges[9].begin(), m_Edges[9].end() );
	FaceSet.insert( m_Vertices[0] );
	FaceSet.insert( m_Vertices[1] );
	FaceSet.insert( m_Vertices[2] );
	FaceSet.insert( m_Vertices[3] );
	FaceSets.push_back( FaceSet );
}

vector<int> CMesher::FindFaceSets( vector< set<int> >& FaceSets, int iIndex )
{
	int i;
	vector<int> Indices;
	for ( i = 0; i < FaceSets.size(); ++i )
	{
		if ( FaceSets[i].find( iIndex ) != FaceSets[i].end() )
			Indices.push_back(i);
	}
	return( Indices );
}

void CMesher::AddQuadNodeToSet( int i, int j, vector<int>& FaceSet1, vector<int>& FaceSet2, vector<int> &Indices )
{
	int iSize1 = FaceSet1.size();
	int iSize2 = FaceSet2.size();

	if ( iSize1 == 1 && iSize2 == 1 ) // Both face sets
	{
		if ( FaceSet1[0] == FaceSet2[0] ) // Same face, add mid node to it
			AddQuadNodeToFace( i, j, FaceSet1[0], Indices );
		return;
	}
	else if ( (iSize1 == 1 && iSize2 == 2) || (iSize1 == 2 && iSize2 == 1) ) // One face, one edge
	{
		if ( iSize1 == 1 )
		{
			if ( FaceSet1[0] == FaceSet2[0] || FaceSet1[0] == FaceSet2[1] )
				AddQuadNodeToFace( i, j, FaceSet1[0], Indices );
		}
		else
		{
			if ( FaceSet2[0] == FaceSet1[0] || FaceSet2[0] == FaceSet1[1] )
				AddQuadNodeToFace( i, j, FaceSet2[0], Indices );
		}
	}
	else if ( iSize1 == 2 && iSize2 == 2 )  // Both edges
	{
		if ( (FaceSet1[0] == FaceSet2[0]) && (FaceSet1[1] == FaceSet2[1]) )  // Created from sets so assume sorted
		{
			AddQuadNodeToEdge( i, j, GetEdge(FaceSet1[0], FaceSet1[1]), Indices );  // Faces same so both have common edge
		}
		else if ( (FaceSet1[0] == FaceSet2[0]) || (FaceSet1[0] == FaceSet2[1]) ) // One face in common so node on face between two edges
		{
			AddQuadNodeToFace( i, j, FaceSet1[0], Indices );  
		}
		else if ( (FaceSet1[1] == FaceSet2[0]) || (FaceSet1[1] == FaceSet2[1]) )
		{
			AddQuadNodeToFace( i, j, FaceSet1[1], Indices );
		}
	}
	else if ( (iSize1 == 1 && iSize2 == 3) || (iSize1 == 3 && iSize2 == 1) ) // One face, one corner
	{
		if ( iSize1 == 1 )
		{
			if ( (FaceSet1[0] == FaceSet2[0]) || (FaceSet1[0] == FaceSet2[1]) || (FaceSet1[0] == FaceSet2[2]) )
				AddQuadNodeToFace( i, j, FaceSet1[0], Indices ); 
		}
		else
		{
			if ( (FaceSet2[0] == FaceSet1[0]) || (FaceSet2[0] == FaceSet1[1]) || (FaceSet2[0] == FaceSet1[2]) )
				AddQuadNodeToFace( i, j, FaceSet2[0], Indices ); 
		}
	}
	else if ( (iSize1 == 2 && iSize2 == 3) || (iSize1 == 3 && iSize2 == 2) )  // One edge, one corner
	{
		vector<int> Face1 = iSize1 == 2 ? FaceSet1 : FaceSet2;
		vector<int> Face2 = iSize1 == 2 ? FaceSet2 : FaceSet1;

		if ( Face1[0] == Face2[0] )
		{
			if ( Face1[1] == Face2[1] || Face1[1] == Face2[2] )
			{
				AddQuadNodeToEdge( i, j, GetEdge( Face1[0], Face1[1]), Indices );
			}
			else 
				AddQuadNodeToFace( i, j, Face1[0], Indices );
		}
		else if ( Face1[0] == Face2[1] )
		{
			if ( Face1[1] == Face2[2] )
			{
				AddQuadNodeToEdge( i, j, GetEdge( Face1[0], Face1[1] ), Indices);
			}
			else
				AddQuadNodeToFace( i, j, Face1[0], Indices );
		}
		else if ( Face1[0] == Face2[2] )
		{
			AddQuadNodeToFace( i, j, Face1[0], Indices );
		}
		else if ( Face1[1] == Face2[0] || Face1[1] == Face2[1] || Face1[1] == Face2[2] )
		{
			AddQuadNodeToFace( i, j, Face1[1], Indices );
		}
	}
	else if ( iSize1 == 3 && iSize2 == 3 )  // Both corners
	{
		// Will only occur if mesh is one element wide between corners - need to code?  See notes for 26-11-13
	}
}

void CMesher::AddQuadNodeToFace( int i, int j, int iFace, vector<int> &Indices )
{
	int iNode = GetQuadNodeToAdd( i, j );
	switch (iFace)
	{  
	case FACE_A:
		m_FaceA.push_back( Indices[iNode]+1 );
		break;
	case FACE_B:
		m_FaceB.push_back( Indices[iNode]+1 );
		break;
	case FACE_C:
		m_FaceC.push_back( Indices[iNode]+1 );
		break;
	case FACE_D:
		m_FaceD.push_back( Indices[iNode]+1 );
		break;
	case FACE_E:
		m_FaceE.push_back( Indices[iNode]+1 );
		break;
	case FACE_F:
		m_FaceF.push_back( Indices[iNode]+1 );
		break;
	}
}

void CMesher::AddQuadNodeToEdge( int i, int j, int iEdge, vector<int> &Indices )
{
	int iNode = GetQuadNodeToAdd( i, j );
	m_Edges[iEdge].push_back( Indices[iNode]+1);
}

int CMesher::GetEdge( int iFace1, int iFace2 )
{
	switch ( iFace1 )
	{
		case FACE_A:
		{
			switch ( iFace2 )
			{
			case FACE_C:
				return 2;
			case FACE_D:
				return 1;
			case FACE_E:
				return 6;
			case FACE_F:
				return 5;
			}
			break;
		}
		
		case FACE_B:
		{
			switch ( iFace2 )
			{
			case FACE_C:
				return 3;
			case FACE_D:
				return 0;
			case FACE_E:
				return 7;
			case FACE_F:
				return 4;
			}
			break;
		}
		
		case FACE_C:
		{
			switch ( iFace2 )
			{
			case FACE_E:
				return 10;
			case FACE_F:
				return 9;
			}
			break;
		}

		case FACE_D:
		{
			switch ( iFace2 )
			{
			case FACE_E:
				return 11;
			case FACE_F:
				return 8;
			}
			break;
		}

		default:
			break;
	}
	return -1;
}

int CMesher::GetQuadNodeToAdd( int i, int j )
{
	if ( i == 0 )
	{
		if ( j == 1 )
			return 4;
		if ( j == 2 )
			return 6;
		if ( j == 3 )
			return 7;
	}
	if ( i == 1 )
	{
		if ( j == 2 )
			return 5;
		if ( j == 3 )
			return 8;
	}
	return 9;
}

void CMesher::RemoveDuplicateNodes()
{
	vector<vector<int> >::iterator  itEdge;
	RemoveDuplicateFaceNodes( m_FaceA );
	RemoveDuplicateFaceNodes( m_FaceB );
	RemoveDuplicateFaceNodes( m_FaceC );
	RemoveDuplicateFaceNodes( m_FaceD );
	RemoveDuplicateFaceNodes( m_FaceE );
	RemoveDuplicateFaceNodes( m_FaceF );

	for ( itEdge = m_Edges.begin(); itEdge != m_Edges.end(); ++itEdge )
	{
		RemoveDuplicateFaceNodes( *itEdge );
	}
}

void CMesher::RemoveDuplicateFaceNodes( vector<int>& FaceSet )
{
	vector<int>::iterator itNewEnd;
	sort( FaceSet.begin(), FaceSet.end() );
	itNewEnd = unique(FaceSet.begin(), FaceSet.end());		// remove duplicates
	FaceSet.erase(itNewEnd, FaceSet.end());
}









