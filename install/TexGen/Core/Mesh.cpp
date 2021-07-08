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
#include "Mesh.h"
#include "Plane.h"
#include "MeshOctreeClasses.h"
#include "Textile.h"
#include "TexGen.h"

using namespace TexGen;
extern "C"
{
#include "../Triangle/triangle.h"
}

CMesh::CMesh(void)
{
}

CMesh::~CMesh(void)
{
}

CMesh::CMesh(TiXmlElement &Element)
{
	FOR_EACH_TIXMLELEMENT(pNode, Element, "Node")
	{
		m_Nodes.push_back(valueify<XYZ>(pNode->Attribute("value")));
	}
	FOR_EACH_TIXMLELEMENT(pType, Element, "Element")
	{
		ELEMENT_TYPE iType = (ELEMENT_TYPE)valueify<int>(pType->Attribute("type"));
		FOR_EACH_TIXMLELEMENT(pIndex, *pType, "Index")
		{
			m_Indices[iType].push_back(valueify<int>(pIndex->Attribute("value")));
		}
	}
}

void CMesh::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	vector<XYZ>::const_iterator itNode;
	for (itNode = m_Nodes.begin(); itNode != m_Nodes.end(); ++itNode)
	{
		TiXmlElement Node("Node");
		Node.SetAttribute("value", stringify(*itNode));
		Element.InsertEndChild(Node);
	}
	int i;
	list<int>::const_iterator itIndex;
	for (i=0; i<NUM_ELEMENT_TYPES; ++i)
	{
		TiXmlElement Indices("Element");
		Indices.SetAttribute("type", i);
		for (itIndex = m_Indices[i].begin(); itIndex != m_Indices[i].end(); ++itIndex)
		{
			TiXmlElement Index("Index");
			Index.SetAttribute("value", *itIndex);
			Indices.InsertEndChild(Index);
		}
		Element.InsertEndChild(Indices);
	}
}

int CMesh::InsertNodes(const CMesh &Mesh, XYZ Offset)
{
	int iOffset = (int)m_Nodes.size();
	vector<XYZ>::const_iterator itNode;
	for (itNode = Mesh.m_Nodes.begin(); itNode != Mesh.m_Nodes.end(); ++itNode)
	{
		m_Nodes.push_back(*itNode+Offset);
	}
	return iOffset;
}

void CMesh::InsertMesh(const CMesh &Mesh, XYZ Offset)
{
	int iOffset = InsertNodes(Mesh, Offset);
	int i;
	list<int>::const_iterator itIndex;
	for (i=0; i<NUM_ELEMENT_TYPES; ++i)
	{
		for (itIndex = Mesh.m_Indices[i].begin(); itIndex != Mesh.m_Indices[i].end(); ++itIndex)
		{
			m_Indices[i].push_back(*itIndex + iOffset);
		}
	}
}

void CMesh::ChangeNodeIndices(int iChangeTo, int iChangeFrom)
{
	int i;
	list<int>::iterator itIndex;
	for (i=0; i<NUM_ELEMENT_TYPES; ++i)
	{
		for (itIndex = m_Indices[i].begin(); itIndex != m_Indices[i].end(); ++itIndex)
		{
			if (*itIndex == iChangeFrom)
			{
				*itIndex = iChangeTo;
			}
		}
	}
}

void CMesh::ChangeNodeIndices(int iChangeTo, int iChangeFrom, vector<vector<int*> > &References)
{
	vector<int*>::iterator itIntPointer;
	vector<int*> &ChangeFromPointers = References[iChangeFrom];
	vector<int*> &ChangeToPointers = References[iChangeTo];
	for (itIntPointer = ChangeFromPointers.begin(); itIntPointer != ChangeFromPointers.end(); ++itIntPointer)
	{
		**itIntPointer = iChangeTo;
		ChangeToPointers.push_back(*itIntPointer);
	}
	ChangeFromPointers.clear();
}

void CMesh::RemoveOpposingTriangles()
{
	list<int> &TriangleIndices = m_Indices[TRI];
	int i1[3];
	int i2[3];
	list<int>::iterator itIter1, itIter2;
	list<int>::iterator itTriStart1, itTriStart2;
//	int iCommonNodes;
//	int i, j;
	for (itIter1 = TriangleIndices.begin(); itIter1 != TriangleIndices.end(); )
	{
		itTriStart1 = itIter1;
		i1[0] = *(itIter1++);
		i1[1] = *(itIter1++);
		i1[2] = *(itIter1++);
		for (itIter2 = itIter1; itIter2 != TriangleIndices.end(); )
		{
			itTriStart2 = itIter2;
			i2[0] = *(itIter2++);
			i2[1] = *(itIter2++);
			i2[2] = *(itIter2++);

			if (i1[0] == i2[0] && i1[1] == i2[2] && i1[2] == i2[1] ||
				i1[0] == i2[1] && i1[1] == i2[0] && i1[2] == i2[2] ||
				i1[0] == i2[2] && i1[1] == i2[1] && i1[2] == i2[0])
			{
				TriangleIndices.erase(itTriStart1, itIter1);
				if (itIter1 == itTriStart2)
					itIter1 = TriangleIndices.erase(itTriStart2, itIter2);
				else
					TriangleIndices.erase(itTriStart2, itIter2);
				//continue;
				break;
			}
		}		
	}
}

void CMesh::RemoveOpposingQuads()
{
	list<int> &QuadIndices = m_Indices[QUAD];
	int i1[4];
	int i2[4];
	list<int>::iterator itIter1, itIter2;
	list<int>::iterator itQuadStart1, itQuadStart2;
//	int iCommonNodes;
//	int i, j;
	for (itIter1 = QuadIndices.begin(); itIter1 != QuadIndices.end(); )
	{
		itQuadStart1 = itIter1;
		i1[0] = *(itIter1++);
		i1[1] = *(itIter1++);
		i1[2] = *(itIter1++);
		i1[3] = *(itIter1++);
		for (itIter2 = itIter1; itIter2 != QuadIndices.end(); )
		{
			itQuadStart2 = itIter2;
			i2[0] = *(itIter2++);
			i2[1] = *(itIter2++);
			i2[2] = *(itIter2++);
			i2[3] = *(itIter2++);

			if (i1[0] == i2[0] && i1[1] == i2[3] && i1[2] == i2[2] && i1[3] == i2[1] ||
				i1[0] == i2[1] && i1[1] == i2[0] && i1[2] == i2[3] && i1[3] == i2[2] ||
				i1[0] == i2[2] && i1[1] == i2[1] && i1[2] == i2[0] && i1[3] == i2[3] ||
				i1[0] == i2[3] && i1[1] == i2[2] && i1[2] == i2[1] && i1[3] == i2[0])
			{
				QuadIndices.erase(itQuadStart1, itIter1);
				if (itIter1 == itQuadStart2)
					itIter1 = QuadIndices.erase(itQuadStart2, itIter2);
				else
					QuadIndices.erase(itQuadStart2, itIter2);
				continue;
			}
		}		
	}
}

void CMesh::RemoveDegenerateTriangles()
{
	list<int> &TriangleIndices = m_Indices[TRI];
	list<int>::iterator itIter, itTriStart;
	int i1;
	int i2;
	int i3;
	for (itIter = TriangleIndices.begin(); itIter != TriangleIndices.end(); )
	{
		itTriStart = itIter;
		i1 = *(itIter++);
		i2 = *(itIter++);
		i3 = *(itIter++);
		if (i1 == i2 || i2 == i3 || i3 == i1)
		{
			itIter = TriangleIndices.erase(itTriStart, itIter);
		}
	}
}

void CMesh::RemoveDuplicateElements(CMesh::ELEMENT_TYPE ElementType)
{
	list<int> &ElementIndices = m_Indices[ElementType];
	vector<int> Index1;
	vector<int> Index2;
	list<int>::iterator itIter1, itIter2;
	list<int>::iterator itStart1, itStart2;

	for (itIter1 = ElementIndices.begin(); itIter1 != ElementIndices.end(); )
	{
		itStart1 = itIter1;
		Index1.clear();
		for ( int i=0; i < GetNumNodes(ElementType); ++i )
		{
			Index1.push_back( *(itIter1++));
		}
		sort( Index1.begin(), Index1.end() ); 
		for (itIter2 = itIter1; itIter2 != ElementIndices.end(); )
		{
			itStart2 = itIter2;
			Index2.clear();
			for ( int j=0; j < GetNumNodes(ElementType); ++j )
			{
				Index2.push_back( *(itIter2++));
			}
			sort( Index2.begin(), Index2.end() );

			if ( equal( Index1.begin(), Index1.end(), Index2.begin() ) )
			{
				if (itIter1 == itStart2)
				{
					itIter1 = ElementIndices.erase(itStart2, itIter2);
					itIter2 = itIter1;
				}
				else
					itIter2 = ElementIndices.erase(itStart2, itIter2);
				//continue;
			}
		}		
	}
}

void CMesh::RemoveDuplicateTriangles()
{
	list<int> &TriangleIndices = m_Indices[TRI];
	int i1[3];
	int i2[3];
	list<int>::iterator itIter1, itIter2;
	list<int>::iterator itTriStart1, itTriStart2;
//	int iCommonNodes;
//	int i, j;
	for (itIter1 = TriangleIndices.begin(); itIter1 != TriangleIndices.end(); )
	{
		itTriStart1 = itIter1;
		i1[0] = *(itIter1++);
		i1[1] = *(itIter1++);
		i1[2] = *(itIter1++);
		for (itIter2 = itIter1; itIter2 != TriangleIndices.end(); )
		{
			itTriStart2 = itIter2;
			i2[0] = *(itIter2++);
			i2[1] = *(itIter2++);
			i2[2] = *(itIter2++);

			if ( i1[0] == i2[0] && i1[1] == i2[1] && i1[2] == i2[2] )
			{
				//TriangleIndices.erase(itTriStart1, itIter1);
				if (itIter1 == itTriStart2)
				{
					itIter1 = TriangleIndices.erase(itTriStart2, itIter2);
					itIter2 = itIter1;
				}
				else
					itIter2 = TriangleIndices.erase(itTriStart2, itIter2);
				//continue;
			}
		}		
	}
}

void CMesh::RemoveDuplicateSegments()
{
	list<int> &LineIndices = m_Indices[LINE];
	list<int>::iterator itIter, itLineStart;
	list<int>::iterator itCompare;
	int i1, i2;
	int j1, j2;
	for (itIter = LineIndices.begin(); itIter != LineIndices.end(); )
	{
		itLineStart = itIter;
		i1 = *(itIter++);
		i2 = *(itIter++);
		for (itCompare = itIter; itCompare != LineIndices.end(); )
		{
			j1 = *(itCompare++);
			j2 = *(itCompare++);
			if ((i1 == j1 && i2 == j2) ||
				(i1 == j2 && i2 == j1))
			{
				itIter = LineIndices.erase(itLineStart, itIter);
				break;
			}
		}
	}
}

pair<XYZ, XYZ> CMesh::GetAABB(double dGrowDistance) const
{
	pair<XYZ, XYZ> AABB;

	vector<XYZ>::const_iterator itNode;
	int iNode;
	for (itNode = m_Nodes.begin(), iNode=0; itNode != m_Nodes.end(); ++itNode, ++iNode)
	{
		if (iNode == 0)
		{
			AABB.first = AABB.second = *itNode;
		}
		else
		{
			AABB.first = Min(AABB.first, *itNode);
			AABB.second = Max(AABB.second, *itNode);
/*			if (itNode->x < AABB.first.x)
				AABB.first.x = itNode->x;
			if (itNode->y < AABB.first.y)
				AABB.first.y = itNode->y;
			if (itNode->z < AABB.first.z)
				AABB.first.z = itNode->z;
			if (itNode->x > AABB.second.x)
				AABB.second.x = itNode->x;
			if (itNode->y > AABB.second.y)
				AABB.second.y = itNode->y;
			if (itNode->z > AABB.second.z)
				AABB.second.z = itNode->z;*/
		}
	}

	AABB.first.x -= dGrowDistance;
	AABB.first.y -= dGrowDistance;
	AABB.first.z -= dGrowDistance;
	AABB.second.x += dGrowDistance;
	AABB.second.y += dGrowDistance;
	AABB.second.z += dGrowDistance;

	return AABB;
}


int CMesh::MergeNodes(const double TOL)
{
	// Merging of nodes is optimised using an octree, that is the space is partitioned to group
	// nodes that are in proximity to each other. Which reduces the number of checks needed to be performed
	// when comparing node positions.

	// Get the dimensions of the octree

	pair<XYZ, XYZ> AABB = GetAABB(TOL);

	double dSize = AABB.second.x - AABB.first.x;
	if (dSize < AABB.second.y - AABB.first.y)
		dSize = AABB.second.y - AABB.first.y;
	if (dSize < AABB.second.z - AABB.first.z)
		dSize = AABB.second.z - AABB.first.z;

	// Prevent problems with creating an octree too small
	dSize = max(dSize, 1e-3);

	// Create the octree

	XYZ Avg = 0.5*(AABB.second + AABB.first);
	dSize *= 1.1;
	Octree<pair<int, XYZ> > Octree(Vector3f(float(Avg.x-0.5*dSize), float(Avg.y-0.5*dSize), float(Avg.z-0.5*dSize)), (float)dSize, 10, 10);
	COctreeAgentNode Agent(TOL);

	// Add the nodes to the octree, the indices of the nodes need to be stored with the nodes because
	// the elements refer to the nodes by their index.

	vector<pair<int, XYZ> > NodesWithIndices;
	NodesWithIndices.resize(m_Nodes.size());

	vector<XYZ>::iterator itNode;
	int iNode;
	for (itNode = m_Nodes.begin(), iNode=0; itNode != m_Nodes.end(); ++itNode, ++iNode)
	{
		NodesWithIndices[iNode] = pair<int, XYZ>(iNode, *itNode);
		Octree.insertItem(NodesWithIndices[iNode], Agent);
	}

	// Visit the octree with the visitor class, this actually does the node merging

	COctreeVisitorMergeNodes Visitor(*this, TOL);

	Octree.visit(Visitor);

	return Visitor.GetNumMerged();


	// UNOPTIMISED VERSION (worth keeping for debuging purposes)

	/* //vector<bool> DeletedNodes;
	set<int> DeletedNodes;
	//DeletedNodes.resize(m_Nodes.size(), false);
	
	map<ELEMENT_TYPE, list<int> >::iterator itType;
	vector<XYZ>::iterator itNode1;
	vector<XYZ>::iterator itNode2;
	int iNode1, iNode2;
	int iNumMerged = 0;
	for (itNode1 = m_Nodes.begin(), iNode1=0; itNode1 != m_Nodes.end(); ++itNode1, ++iNode1)
	{
		for (itNode2 = itNode1+1, iNode2 = iNode1+1; itNode2 != m_Nodes.end(); ++itNode2, ++iNode2)
		{
			if (GetLengthSquared(*itNode2, *itNode1) < TOL*TOL)
			{
				++iNumMerged;
				ChangeNodeIndices(iNode1, iNode2);
				//DeletedNodes[iNode2] = true;
				DeletedNodes.insert(iNode2);
			}
		}
	}

	DeleteNodes(DeletedNodes);

	cout << "Num Merged Nodes: " << iNumMerged << endl;

	return iNumMerged;*/
}

vector<pair<int, int> > CMesh::GetNodePairs(XYZ Vector, const double TOL) const
{
	vector<pair<int, int> > NodePairs;
	vector<XYZ>::const_iterator itNode1;
	vector<XYZ>::const_iterator itNode2;
	int iNode1, iNode2;
	double dTolSquared = TOL*TOL;
	for (itNode1 = m_Nodes.begin(), iNode1=0; itNode1 != m_Nodes.end(); ++itNode1, ++iNode1)
	{
		for (itNode2 = m_Nodes.begin(), iNode2=0; itNode2 != m_Nodes.end(); ++itNode2, ++iNode2)
		{
			if (GetLengthSquared(*itNode1, *itNode2-Vector) < dTolSquared)
			{
				NodePairs.push_back(make_pair(iNode1, iNode2));
				break;
			}
		}
	}
	return NodePairs;
}

 void CMesh::GetNodePairs(XYZ Vector, vector<pair<int, int> > &NodePairs, const double TOL ) const
{
	vector<XYZ>::const_iterator itNode1;
	vector<XYZ>::const_iterator itNode2;

	vector<XYZ> OffsetNodes;
	for ( itNode2 = m_Nodes.begin(); itNode2 != m_Nodes.end(); ++itNode2 )
	{
		OffsetNodes.push_back( *itNode2 - Vector );
	}

	bool bProgress = m_Nodes.size() > 10000;
	int iProgressInc = m_Nodes.size()/10;

	int iNode1, iNode2;
	double dTolSquared = TOL*TOL;
	bool bFound;
	for (itNode1 = m_Nodes.begin(), iNode1=0; itNode1 != m_Nodes.end(); ++itNode1, ++iNode1)
	{
		if ( bProgress && !(iNode1 % iProgressInc) )
		{
			TGLOG( "GetNodePairs " << iNode1/iProgressInc*10 << "% complete");
		}
		bFound = false;
		for ( iNode2 = iNode1; iNode2 < OffsetNodes.size(); ++ iNode2 )  // Matching node normally later in mesh than itNode1 so start checking here
		{                                                                // to improve speed
			if (GetLengthSquared(*itNode1, OffsetNodes[iNode2]) < dTolSquared)
			{
				NodePairs.push_back(make_pair(iNode1, iNode2));
				bFound = true;
				break;
			}
		}
		if ( !bFound )
		{
			for ( iNode2 = 0; iNode2 < iNode1; ++iNode2 )   // Check from beginning if node earlier in mesh than one checking against
			{
				if (GetLengthSquared(*itNode1, OffsetNodes[iNode2]) < dTolSquared)
				{
					NodePairs.push_back(make_pair(iNode1, iNode2));
					break;
				}
			}
		}
	}
	//return NodePairs;
}

int CMesh::GetClosestNode(XYZ Position) const
{
	vector<XYZ>::const_iterator itNode;
	double dClosestDistSqrd = -1;
	double dDistSqrd;
	int iClosestNode = -1, i;
	for (itNode = m_Nodes.begin(), i=0; itNode != m_Nodes.end(); ++itNode, ++i)
	{
		dDistSqrd = GetLengthSquared(Position, *itNode);
		if ( dDistSqrd < dClosestDistSqrd || dClosestDistSqrd == -1 )
		{
			dClosestDistSqrd = dDistSqrd;
			iClosestNode = i;
		}
	}
	return iClosestNode;
}

int CMesh::GetClosestNodeDistance(XYZ Position, double dTol ) const
{
	vector<XYZ>::const_iterator itNode;
	double dClosestDistSqrd = -1;
	double dDistSqrd;
	int iClosestNode = -1, i;
	for (itNode = m_Nodes.begin(), i=0; itNode != m_Nodes.end(); ++itNode, ++i)
	{
		dDistSqrd = GetLengthSquared(Position, *itNode);
		if ( dDistSqrd < dClosestDistSqrd || dClosestDistSqrd == -1 )
		{
			dClosestDistSqrd = dDistSqrd;
			if ( dClosestDistSqrd <= dTol )
				iClosestNode = i;
		}
	}
	return iClosestNode;
}

void CMesh::ConvertToSurfaceMesh()
{
	Convert3Dto2D();
	RemoveOpposingTriangles();
	RemoveOpposingQuads();
}

void CMesh::Convert3Dto2D()
{
	ConvertHextoQuad();
	ConvertWedgeto2D();
	ConvertTettoTriangle();
	ConvertPyramidto2D();
}

void CMesh::ConvertToTetMesh()
{
	ConvertWedgetoTet();
	ConvertHextoTet();
	ConvertPyramidtoTet();
	RemoveAllElementsExcept(TET);
}

void CMesh::ConvertToTriangleMesh()
{
	Convert3Dto2D();
	ConvertQuadstoTriangles();
	RemoveAllElementsExcept(TRI);
}

void CMesh::ConvertToSegmentMesh()
{
	ConvertToTriangleMesh();
	ConvertTrianglestoSegments();
	RemoveDuplicateSegments();
}

void CMesh::ConvertTriToQuad( double Tolerance )
{
	list<int> &TriangleIndices = m_Indices[TRI];
	vector<int> i1;
	int i2[3];
	list<int>::iterator itIter1, itIter2;
	list<int>::iterator itTriStart1, itTriStart2;
	vector<int>::iterator iti1;

	int iNumNodes = GetNumNodes(TRI);
	for (itIter1 = TriangleIndices.begin(); itIter1 != TriangleIndices.end(); )
	{
		itTriStart1 = itIter1;
		i1.clear();
		i1.push_back(*(itIter1++));
		i1.push_back(*(itIter1++));
		i1.push_back(*(itIter1++));
		for ( itIter2 = itIter1; itIter2 != TriangleIndices.end(); )
		{
			itTriStart2 = itIter2;
			list<int> RemInd;
			i2[0] = *(itIter2++);
			RemInd.push_back(i2[0]);
			i2[1] = *(itIter2++);
			RemInd.push_back(i2[1]);
			i2[2] = *(itIter2++);
			RemInd.push_back(i2[2]);
			vector<int> CommonInd;
			
			int i;
			for ( iti1 = i1.begin(), i = 0; iti1 != i1.end(); iti1++, ++i )
			{
				
				for ( int j = 0; j < iNumNodes; ++j )
				{
					if ( *iti1 == i2[j] )
					{
						CommonInd.push_back(i);
						RemInd.remove(i2[j]);
						break;
					}
				}
			}
			if ( CommonInd.size() == 2 )
			{
				// Check normals
				XYZ N1 = m_Nodes[i1[0]];
				XYZ N2 = m_Nodes[i1[1]];
				XYZ N3 = m_Nodes[i1[2]];
				XYZ Normal1 = CrossProduct(N2-N1, N3-N1);
				Normal1 /= GetLength(Normal1);  // Normalise

				N1 = m_Nodes[i2[0]];
				N2 = m_Nodes[i2[1]];
				N3 = m_Nodes[i2[2]];
				XYZ Normal2 = CrossProduct(N2-N1, N3-N1);
				Normal2 /= GetLength(Normal2);  // Normalise

				if ( Normal1 == Normal2 || Normal1 == -Normal2 )
				{
					if ( CommonInd[0] == 0 && CommonInd[1] == 2 )
						i1.push_back( *(RemInd.begin()) );
					else
						i1.insert( i1.begin()+CommonInd[1],*(RemInd.begin()));
					AddElement(	QUAD, i1 );
					if ( itIter1 == itTriStart2 )
						itIter1 = TriangleIndices.erase( itTriStart1, itIter2 );
					else
					{
						TriangleIndices.erase( itTriStart2, itIter2 );
						itIter1 = TriangleIndices.erase( itTriStart1, itIter1 );
					}
					
					break;
				}
				
			}
		}
	}
}

int CMesh::RemoveUnreferencedNodes()
{
/*	vector<bool> UnreferencedNodes;
	UnreferencedNodes.resize(m_Nodes.size(), true);*/
	set<int> UnreferencedNodes;
	int i;
	for (i=0; i<(int)m_Nodes.size(); ++i)
	{
		UnreferencedNodes.insert(i);
	}

	list<int>::iterator itIndex;
	for (i = 0; i < NUM_ELEMENT_TYPES; ++i)
	{
		for (itIndex = m_Indices[i].begin(); itIndex != m_Indices[i].end(); ++itIndex)
		{
//			UnreferencedNodes[*itIndex] = false;
			UnreferencedNodes.erase(*itIndex);   // Get rid of the nodes which are referenced..
		}
	}

	return DeleteNodes(UnreferencedNodes);   // and delete the unused ones remaining
}

void CMesh::RemoveAllElementsExcept(ELEMENT_TYPE Type)
{
	int i;
	for (i = 0; i < NUM_ELEMENT_TYPES; ++i)
	{
		if (i == Type)
			continue;
		else
			m_Indices[i].clear();
	}
}

void CMesh::RemoveElementType( ELEMENT_TYPE Type )
{
	m_Indices[Type].clear();
}

int CMesh::DeleteNodes(const set<int> &Nodes)
{
	vector<int> NewIndices;
	NewIndices.resize(m_Nodes.size(), 0);
	int i, j;
	int iNumNodesDeleted = 0;
	for (i=0, j=0; i<(int)NewIndices.size(); ++i)
	{
		NewIndices[i] = j;
		if (Nodes.count(i))
			++iNumNodesDeleted;
		else
			++j;
	}

	list<int>::iterator itIndex;
	for (i = 0; i < NUM_ELEMENT_TYPES; ++i)
	{
		for (itIndex = m_Indices[i].begin(); itIndex != m_Indices[i].end(); ++itIndex)
		{
			*itIndex = NewIndices[*itIndex];
		}
	}

	vector<XYZ>::iterator itNode;
	vector<XYZ> NewNodes;
	int iNodeIndex;
	for (itNode = m_Nodes.begin(), iNodeIndex=0; itNode != m_Nodes.end(); ++itNode, ++iNodeIndex)
	{
		if (!Nodes.count(iNodeIndex))
			NewNodes.push_back(*itNode);
	}
	m_Nodes = NewNodes;

	return iNumNodesDeleted;
}

void CMesh::ConvertHextoQuad()
{
	list<int>::iterator itIter;
	int i1, i2, i3, i4, i5, i6, i7, i8;
	for (itIter = m_Indices[HEX].begin(); itIter != m_Indices[HEX].end(); )
	{
		i1 = *(itIter++);
		i2 = *(itIter++);
		i3 = *(itIter++);
		i4 = *(itIter++);
		i5 = *(itIter++);
		i6 = *(itIter++);
		i7 = *(itIter++);
		i8 = *(itIter++);

		m_Indices[QUAD].push_back(i4);
		m_Indices[QUAD].push_back(i3);
		m_Indices[QUAD].push_back(i2);
		m_Indices[QUAD].push_back(i1);

		m_Indices[QUAD].push_back(i1);
		m_Indices[QUAD].push_back(i2);
		m_Indices[QUAD].push_back(i6);
		m_Indices[QUAD].push_back(i5);

		m_Indices[QUAD].push_back(i2);
		m_Indices[QUAD].push_back(i3);
		m_Indices[QUAD].push_back(i7);
		m_Indices[QUAD].push_back(i6);

		m_Indices[QUAD].push_back(i3);
		m_Indices[QUAD].push_back(i4);
		m_Indices[QUAD].push_back(i8);
		m_Indices[QUAD].push_back(i7);

		m_Indices[QUAD].push_back(i4);
		m_Indices[QUAD].push_back(i1);
		m_Indices[QUAD].push_back(i5);
		m_Indices[QUAD].push_back(i8);

		m_Indices[QUAD].push_back(i5);
		m_Indices[QUAD].push_back(i6);
		m_Indices[QUAD].push_back(i7);
		m_Indices[QUAD].push_back(i8);
	}
	m_Indices[HEX].clear();
}

void CMesh::ConvertWedgeto2D()
{
	list<int>::iterator itIter;
	int i1, i2, i3, i4, i5, i6;
	for (itIter = m_Indices[WEDGE].begin(); itIter != m_Indices[WEDGE].end(); )
	{
		i1 = *(itIter++);
		i2 = *(itIter++);
		i3 = *(itIter++);
		i4 = *(itIter++);
		i5 = *(itIter++);
		i6 = *(itIter++);

		m_Indices[TRI].push_back(i1);
		m_Indices[TRI].push_back(i2);
		m_Indices[TRI].push_back(i3);

		m_Indices[QUAD].push_back(i2);
		m_Indices[QUAD].push_back(i1);
		m_Indices[QUAD].push_back(i4);
		m_Indices[QUAD].push_back(i5);

		m_Indices[QUAD].push_back(i3);
		m_Indices[QUAD].push_back(i2);
		m_Indices[QUAD].push_back(i5);
		m_Indices[QUAD].push_back(i6);

		m_Indices[QUAD].push_back(i1);
		m_Indices[QUAD].push_back(i3);
		m_Indices[QUAD].push_back(i6);
		m_Indices[QUAD].push_back(i4);

		m_Indices[TRI].push_back(i6);
		m_Indices[TRI].push_back(i5);
		m_Indices[TRI].push_back(i4);
	}
	m_Indices[WEDGE].clear();
}

void CMesh::ConvertTettoTriangle()
{
	list<int>::iterator itIter;
	int i1, i2, i3, i4;
	for (itIter = m_Indices[TET].begin(); itIter != m_Indices[TET].end(); )
	{
		i1 = *(itIter++);
		i2 = *(itIter++);
		i3 = *(itIter++);
		i4 = *(itIter++);

		m_Indices[TRI].push_back(i3);
		m_Indices[TRI].push_back(i2);
		m_Indices[TRI].push_back(i1);

		m_Indices[TRI].push_back(i1);
		m_Indices[TRI].push_back(i2);
		m_Indices[TRI].push_back(i4);

		m_Indices[TRI].push_back(i2);
		m_Indices[TRI].push_back(i3);
		m_Indices[TRI].push_back(i4);

		m_Indices[TRI].push_back(i3);
		m_Indices[TRI].push_back(i1);
		m_Indices[TRI].push_back(i4);
	}
	m_Indices[TET].clear();
}

void CMesh::ConvertHextoWedge(bool bQuality)
{
	list<int>::iterator itIter;
	int i0, i1, i2, i3, i4, i5, i6, i7;
	for (itIter = m_Indices[HEX].begin(); itIter != m_Indices[HEX].end(); )
	{
		i0 = *(itIter++);
		i1 = *(itIter++);
		i2 = *(itIter++);
		i3 = *(itIter++);
		i4 = *(itIter++);
		i5 = *(itIter++);
		i6 = *(itIter++);
		i7 = *(itIter++);

		// Split it up randomly without regard to quality
		m_Indices[WEDGE].push_back(i4);
		m_Indices[WEDGE].push_back(i5);
		m_Indices[WEDGE].push_back(i6);
		m_Indices[WEDGE].push_back(i0);
		m_Indices[WEDGE].push_back(i1);
		m_Indices[WEDGE].push_back(i2);

		m_Indices[WEDGE].push_back(i6);
		m_Indices[WEDGE].push_back(i7);
		m_Indices[WEDGE].push_back(i4);
		m_Indices[WEDGE].push_back(i2);
		m_Indices[WEDGE].push_back(i3);
		m_Indices[WEDGE].push_back(i0);

		// Todo: If the bQuality flag is set then use a more intelligent
		// way to decide how to split the element up
	}
	m_Indices[HEX].clear();
}

void CMesh::ConvertWedgetoTetandPyramid(bool bQuality)
{
	list<int>::iterator itIter;
	int i0, i1, i2, i3, i4, i5;
	for (itIter = m_Indices[WEDGE].begin(); itIter != m_Indices[WEDGE].end(); )
	{
		i0 = *(itIter++);
		i1 = *(itIter++);
		i2 = *(itIter++);
		i3 = *(itIter++);
		i4 = *(itIter++);
		i5 = *(itIter++);

		// Split it up randomly without regard to quality
		m_Indices[PYRAMID].push_back(i0);
		m_Indices[PYRAMID].push_back(i1);
		m_Indices[PYRAMID].push_back(i4);
		m_Indices[PYRAMID].push_back(i3);
		m_Indices[PYRAMID].push_back(i2);

		m_Indices[TET].push_back(i3);
		m_Indices[TET].push_back(i4);
		m_Indices[TET].push_back(i5);
		m_Indices[TET].push_back(i2);

		// Todo: If the bQuality flag is set then use a more intelligent
		// way to decide how to split the element up
	}
	m_Indices[WEDGE].clear();
}

void CMesh::ConvertPyramidtoTet(bool bQuality)
{
	list<int>::iterator itIter;
	int i0, i1, i2, i3, i4;
	for (itIter = m_Indices[PYRAMID].begin(); itIter != m_Indices[PYRAMID].end(); )
	{
		i0 = *(itIter++);
		i1 = *(itIter++);
		i2 = *(itIter++);
		i3 = *(itIter++);
		i4 = *(itIter++);

		double d1 = 0, d2 = 0;
		if (bQuality)
		{
			// If we don't care about the quality then just skip this calculation
			d1 = GetLengthSquared(m_Nodes[i0], m_Nodes[i2]);
			d2 = GetLengthSquared(m_Nodes[i1], m_Nodes[i3]);
		}
		if (d1<d2)
		{
			m_Indices[TET].push_back(i0);
			m_Indices[TET].push_back(i1);
			m_Indices[TET].push_back(i2);
			m_Indices[TET].push_back(i4);

			m_Indices[TET].push_back(i0);
			m_Indices[TET].push_back(i2);
			m_Indices[TET].push_back(i3);
			m_Indices[TET].push_back(i4);
		}
		else
		{
			m_Indices[TET].push_back(i1);
			m_Indices[TET].push_back(i2);
			m_Indices[TET].push_back(i3);
			m_Indices[TET].push_back(i4);

			m_Indices[TET].push_back(i3);
			m_Indices[TET].push_back(i0);
			m_Indices[TET].push_back(i1);
			m_Indices[TET].push_back(i4);
		}
	}
	m_Indices[PYRAMID].clear();
}

void CMesh::ConvertWedgetoTet(bool bQuality)
{
	ConvertWedgetoTetandPyramid(bQuality);
	ConvertPyramidtoTet(bQuality);
}

void CMesh::ConvertHextoTet(bool bQuality)
{
	ConvertHextoWedge(bQuality);
	ConvertWedgetoTet(bQuality);
}

void CMesh::ConvertPyramidto2D()
{
	list<int>::iterator itIter;
	int i0, i1, i2, i3, i4;
	for (itIter = m_Indices[PYRAMID].begin(); itIter != m_Indices[PYRAMID].end(); )
	{
		i0 = *(itIter++);
		i1 = *(itIter++);
		i2 = *(itIter++);
		i3 = *(itIter++);
		i4 = *(itIter++);

		m_Indices[QUAD].push_back(i3);
		m_Indices[QUAD].push_back(i2);
		m_Indices[QUAD].push_back(i1);
		m_Indices[QUAD].push_back(i0);

		m_Indices[TRI].push_back(i0);
		m_Indices[TRI].push_back(i1);
		m_Indices[TRI].push_back(i4);

		m_Indices[TRI].push_back(i1);
		m_Indices[TRI].push_back(i2);
		m_Indices[TRI].push_back(i4);

		m_Indices[TRI].push_back(i2);
		m_Indices[TRI].push_back(i3);
		m_Indices[TRI].push_back(i4);

		m_Indices[TRI].push_back(i3);
		m_Indices[TRI].push_back(i0);
		m_Indices[TRI].push_back(i4);
	}
	m_Indices[PYRAMID].clear();
}

list<int>::iterator CMesh::ConvertQuadtoTriangles(list<int>::iterator itQuad)
{
	int i0, i1, i2, i3;

	list<int>::iterator itEnd = itQuad;

	i0 = *(itEnd++);
	i1 = *(itEnd++);
	i2 = *(itEnd++);
	i3 = *(itEnd++);

	m_Indices[TRI].push_back(i0);
	m_Indices[TRI].push_back(i1);
	m_Indices[TRI].push_back(i2);

	m_Indices[TRI].push_back(i0);
	m_Indices[TRI].push_back(i2);
	m_Indices[TRI].push_back(i3);

	return m_Indices[QUAD].erase(itQuad, itEnd);
}

void CMesh::ConvertTrianglestoSegments()
{
	list<int>::iterator itIter;
	int i0, i1, i2;
	for (itIter = m_Indices[TRI].begin(); itIter != m_Indices[TRI].end(); )
	{
		i0 = *(itIter++);
		i1 = *(itIter++);
		i2 = *(itIter++);

		m_Indices[LINE].push_back(i0);
		m_Indices[LINE].push_back(i1);

		m_Indices[LINE].push_back(i1);
		m_Indices[LINE].push_back(i2);

		m_Indices[LINE].push_back(i2);
		m_Indices[LINE].push_back(i0);
	}
	m_Indices[TRI].clear();
}

void CMesh::ConvertQuadstoTriangles(bool bQuality)
{
	list<int>::iterator itIter;
	int i0, i1, i2, i3;
	for (itIter = m_Indices[QUAD].begin(); itIter != m_Indices[QUAD].end(); )
	{
		i0 = *(itIter++);
		i1 = *(itIter++);
		i2 = *(itIter++);
		i3 = *(itIter++);

		if (!bQuality)
		{
			m_Indices[TRI].push_back(i0);
			m_Indices[TRI].push_back(i1);
			m_Indices[TRI].push_back(i2);

			m_Indices[TRI].push_back(i0);
			m_Indices[TRI].push_back(i2);
			m_Indices[TRI].push_back(i3);
		}
		else
		{
			double d1, d2;
			d1 = GetLengthSquared(m_Nodes[i0], m_Nodes[i2]);
			d2 = GetLengthSquared(m_Nodes[i1], m_Nodes[i3]);
			if (d1<d2)
			{
				m_Indices[TRI].push_back(i0);
				m_Indices[TRI].push_back(i1);
				m_Indices[TRI].push_back(i2);

				m_Indices[TRI].push_back(i0);
				m_Indices[TRI].push_back(i2);
				m_Indices[TRI].push_back(i3);
			}
			else
			{
				m_Indices[TRI].push_back(i1);
				m_Indices[TRI].push_back(i2);
				m_Indices[TRI].push_back(i3);

				m_Indices[TRI].push_back(i3);
				m_Indices[TRI].push_back(i0);
				m_Indices[TRI].push_back(i1);
			}
		}
	}
	m_Indices[QUAD].clear();
}

void CMesh::Translate(XYZ Vector)
{
	vector<XYZ>::iterator itNode;
	for (itNode = m_Nodes.begin(); itNode != m_Nodes.end(); ++itNode)
	{
		*itNode += Vector;
	}
}

void CMesh::Rotate(WXYZ Rotation, XYZ Origin)
{
	vector<XYZ>::iterator itNode;
	for (itNode = m_Nodes.begin(); itNode != m_Nodes.end(); ++itNode)
	{
		(*itNode) = Rotation * (*itNode-Origin) + Origin;
	}	
}
/*
void CMesh::Rotate(XYZ X, XYZ Y, XYZ Z)
{
	vector<XYZ>::iterator itNode;
	for (itNode = m_Nodes.begin(); itNode != m_Nodes.end(); ++itNode)
	{
		*itNode = X * itNode->x + Y * itNode->y + Z * itNode->z;
	}
}
*/
void CMesh::FlipNormals()
{
	list<int>::iterator itIter;
	list<int>::iterator it0, it1, it2, it3;
	int i0, i1, i2, i3;
	for (itIter = m_Indices[QUAD].begin(); itIter != m_Indices[QUAD].end(); )
	{
		it0 = itIter++;
		it1 = itIter++;
		it2 = itIter++;
		it3 = itIter++;
		i0 = *it0;
		i1 = *it1;
		i2 = *it2;
		i3 = *it3;
		*it0 = i3;
		*it1 = i2;
		*it2 = i1;
		*it3 = i0;
	}
	for (itIter = m_Indices[TRI].begin(); itIter != m_Indices[TRI].end(); )
	{
		it0 = itIter++;
		it1 = itIter++;
		it2 = itIter++;
		i0 = *it0;
		i1 = *it1;
		i2 = *it2;
		*it0 = i2;
		*it1 = i1;
		*it2 = i0;
	}
}

void CMesh::MeshClosedLoop(const XYZ &Normal, const vector<int> &ClosedLoopVector, bool bQuality)
{
	// A more efficient algorithm is described in http://www.cs.umd.edu/~mount/754/Lects/754lects.pdf
	// worth implementing if this function needs optimising. Although will need extending to 3d.
	const double TOL = 1e-10;

	list<int> ClosedLoop(ClosedLoopVector.begin(), ClosedLoopVector.end());

	list<int> &TriIndices = m_Indices[CMesh::TRI];

	list<int>::iterator itPrev;
	list<int>::iterator itCurrent;
	list<int>::iterator itNext;
	list<int>::iterator itCompare;
	list<int>::iterator itBest;

	XYZ V1, V2, V3;
	XYZ P1, P2, P3, P;

	PLANE PL1, PL2, PL3;
	double l1, l2;

	double dBestAngle;
	double dAngle;

	bool bFound;

	// Keep going while there are at least 3 indices in the loop
	while (ClosedLoop.size() >= 3)
	{
		// Three iterators are used to keep track of 3 consecutive points
		bFound = false;
		itPrev = ClosedLoop.end(); --itPrev;
		itCurrent = ClosedLoop.begin();
		itNext = ClosedLoop.begin(); ++itNext;
		// Go round increment the three points until all combination of triangles have been looked at
		while (itCurrent != ClosedLoop.end())
		{
			// Get the point coordinates
			P1 = m_Nodes[*itPrev];
			P2 = m_Nodes[*itCurrent];
			P3 = m_Nodes[*itNext];

			// Form vectors from the edges of the triangle
			V1 = P2 - P1;
			V2 = P3 - P2;
			V3 = P1 - P3;

			// Check that the triangle is facing the right way, this is important
			// for concave loops
			if (DotProduct(Normal, CrossProduct(V1, V3)) > -TOL)
			{
				PL1.Normal = CrossProduct(V1, Normal);
				Normalise(PL1.Normal);
				PL1.d = DotProduct(PL1.Normal, P1);

				PL2.Normal = CrossProduct(V2, Normal);
				Normalise(PL2.Normal);
				PL2.d = DotProduct(PL2.Normal, P2);

				PL3.Normal = CrossProduct(V3, Normal);
				Normalise(PL3.Normal);
				PL3.d = DotProduct(PL3.Normal, P3);

				// Make sure that none of the other points lie within the triangle, again important
				// for concave loops
				for (itCompare = ClosedLoop.begin(); itCompare != ClosedLoop.end(); ++itCompare)
				{
					if (itCompare == itPrev || itCompare == itCurrent || itCompare == itNext)
						continue;

					// Get the coordinate of the point
					P = m_Nodes[*itCompare];

					// If true then the point lies within the triangle, we must not form a triangle with this
					// set of points
					if (DotProduct(P, PL1.Normal) > PL1.d && DotProduct(P, PL2.Normal) > PL2.d && DotProduct(P, PL3.Normal) > PL3.d)
					{
						break;
					}
				}

				// If this is true then no other point lies within the proposed triangle
				if (itCompare == ClosedLoop.end())
				{
					l1 = GetLength(V1);
					l2 = GetLength(V2);

					// Find the angle between V1 and V2
					dAngle = acos(DotProduct(V1, V2)/(l1*l2));

					// If this angle is larger than the rest of the valid triangles
					// then consider this as our best canditate for creating a triangle
					if (!bFound || dAngle > dBestAngle)
					{
						dBestAngle = dAngle;
						itBest = itCurrent;
						bFound = true;
						if (!bQuality)
							break;
					}
				}
			}

			// Go to the next set of 3 points
			++itPrev;
			++itCurrent;
			++itNext;
			if (itPrev == ClosedLoop.end())
				itPrev = ClosedLoop.begin();
			if (itNext == ClosedLoop.end())
				itNext = ClosedLoop.begin();
		}

		// Didn't find a best triangle, in which case just choose one at random.
		// this should never happen however.
		if (!bFound) // Does happen if slice through yarn at edge of domain
		{            // but don't want the whole program to keel over so taken assert out!
					 // Just doesn't render that end section
			TGERROR( "Couldn't mesh closed loop on domain edge" );
			return;
			//itBest = ClosedLoop.begin();
			//assert(false);
		}

		// The best 3 points will be used to form a triangle, the middle point will
		// then be removed and the remaining polygon will continue to be meshed.

		if (itBest == ClosedLoop.begin())
			{itPrev = ClosedLoop.end(); --itPrev;}
		else
			{itPrev = itBest; --itPrev;}
		itCurrent = itBest;
		itNext = itBest; ++itNext;
		if (itNext == ClosedLoop.end())
			itNext = ClosedLoop.begin();
		TriIndices.push_back(*itPrev);
		TriIndices.push_back(*itCurrent);
		TriIndices.push_back(*itNext);
		ClosedLoop.erase(itCurrent);
	}
}


/*void CMesh::MeshClosedLoop(const XYZ &Normal, const vector<int> &ClosedLoop)
{
	// NOTE: Triangle is not very robust, if anything fucks up, the program is likely to just crash
	// thus the code has been replaced with my own meshing algorithms. This is kept here in case
	// it needs to be re-implemented.

	// Triangle will be used to triangulate the gaps

	if (ClosedLoop.size() < 3)	// Triangle will quit if no vertices are given to it
		return;

	bool bReverse = false;

	char szSwitches[] = "pzQPB";

	triangulateio TriangleInput;
	triangulateio TriangleOutput;
	memset(&TriangleInput, 0, sizeof(TriangleInput));
	memset(&TriangleOutput, 0, sizeof(TriangleOutput));

	TriangleInput.pointlist = new REAL [ClosedLoop.size()*2];
	TriangleInput.numberofpoints = (int)ClosedLoop.size();

	// Ignore one of the coordinates, the one with the highest value for the plane normal
	// This is a very basic projection of the 3D coordinates to a 2d plane. One of the three planes
	// XY, YZ or XZ is chosen. The plane is chosen to give minimum distortion. There will still be some
	// distortion, but this is not an issue since a quality mesh is not needed.
	int iIgnoreCoordinate;
	if (abs(Normal.x) >= abs(Normal.y) && abs(Normal.x) >= abs(Normal.z))
		iIgnoreCoordinate = 0;
	else if (abs(Normal.y) >= abs(Normal.z))
		iIgnoreCoordinate = 1;
	else
		iIgnoreCoordinate = 2;

	if (Normal[iIgnoreCoordinate] > 0)
		bReverse = true;

	// Fill the nodes into the TriangleInput structure
	int i;
	for (i=0; i<(int)ClosedLoop.size(); ++i)
	{
		// Ignore one of the coordinates, this is decided further up
		TriangleInput.pointlist[i*2] = m_Nodes[ClosedLoop[i]][(iIgnoreCoordinate+1)%3];
		TriangleInput.pointlist[i*2+1] = m_Nodes[ClosedLoop[i]][(iIgnoreCoordinate+2)%3];
	}

	TriangleInput.segmentlist = new int [ClosedLoop.size()*2];
	TriangleInput.numberofsegments = (int)ClosedLoop.size();

	// Pass in the segments with the new node indices
	for (i=0; i<(int)ClosedLoop.size(); ++i)
	{
		TriangleInput.segmentlist[i*2] = i;
		TriangleInput.segmentlist[i*2+1] = (i+1)%ClosedLoop.size();
	}

	// Triangulate
	triangulate(szSwitches, &TriangleInput, &TriangleOutput, NULL);

	delete [] TriangleInput.pointlist;
	delete [] TriangleInput.segmentlist;

	list<int> &TriIndices = m_Indices[CMesh::TRI];

	int i1, i2, i3;
	for (i=0; i<TriangleOutput.numberoftriangles; ++i)
	{
		i1 = TriangleOutput.trianglelist[i*3];
		i2 = TriangleOutput.trianglelist[i*3+1];
		i3 = TriangleOutput.trianglelist[i*3+2];
		// Convert the node from those passed to triangle to the Mesh indices
		if (!bReverse)
		{
			TriIndices.push_back(ClosedLoop[i1]);
			TriIndices.push_back(ClosedLoop[i2]);
			TriIndices.push_back(ClosedLoop[i3]);
		}
		else
		{
			TriIndices.push_back(ClosedLoop[i1]);
			TriIndices.push_back(ClosedLoop[i3]);
			TriIndices.push_back(ClosedLoop[i2]);
		}
	}

	// Free up the memory allocated by triangle
	trifree(TriangleOutput.pointlist);
	trifree(TriangleOutput.trianglelist);

	return;
}*/

void CMesh::CopySelfToRange(XYZ Vector, int iLowerLimit, int iUpperLimit)
{
	CMesh Original = *this;
	Clear();
	int i;
	for (i=iLowerLimit; i<=iUpperLimit; ++i)
	{
		InsertMesh(Original, i*Vector);
	}
}

void CMesh::Clear()
{
	m_Nodes.clear();
	int i;
	for (i = 0; i < NUM_ELEMENT_TYPES; ++i)
	{
		m_Indices[i].clear();
	}
/*	map<ELEMENT_TYPE, list<int> >::iterator itType;
	for (itType = m_Indices.begin(); itType != m_Indices.end(); ++itType)
	{
		itType->second.clear();
	}*/
}

int CMesh::OutputNodes(ostream &Output, int iStartIndex, string Seperator, bool bSCIRun) const
{
	int iNodeIndex;
	vector<XYZ>::const_iterator itNode;

	if ( bSCIRun )
		Output << m_Nodes.size() << endl;

	for (itNode = m_Nodes.begin(), iNodeIndex=0; itNode != m_Nodes.end(); ++itNode, ++iNodeIndex)
	{
		if (iStartIndex >= 0 && !bSCIRun)
			Output << iNodeIndex+iStartIndex << Seperator;
		Output << *itNode << endl;
	}
	return iNodeIndex+iStartIndex;
}

int CMesh::OutputElements(ostream &Output, CMesh::ELEMENT_TYPE ElementType, int iStartIndex, int iIndexOffset, string Seperator, bool bSCIRun) const
{
	assert(ElementType >= 0 && ElementType < NUM_ELEMENT_TYPES);
	int i;
	int iElementNumber = 0;
	list<int>::const_iterator itIndex;

	if ( bSCIRun )
		Output << m_Indices[ElementType].size()/GetNumNodes(ElementType) << endl;
	for (itIndex = m_Indices[ElementType].begin(); itIndex != m_Indices[ElementType].end(); ++iElementNumber)
	{
		if (iStartIndex >= 0 && !bSCIRun)
			Output << iElementNumber+iStartIndex << Seperator;
		for (i=0; i<GetNumNodes(ElementType); ++i, ++itIndex)
		{
			if (i>0)
				Output << Seperator;
			Output << (*itIndex)+iIndexOffset;
		}
		Output << endl;
	}
	return iElementNumber+iStartIndex;
}

void CMesh::GetNodeElementReferences(vector<vector<int*> > &References)
{
	References.clear();
	References.resize(m_Nodes.size());
	int i;
	list<int>::iterator itIndex;
	for (i = 0; i < NUM_ELEMENT_TYPES; ++i)
	{
		for (itIndex = m_Indices[i].begin(); itIndex != m_Indices[i].end(); ++itIndex)
		{
			References[*itIndex].push_back(&(*itIndex));
		}
	}
}

double CMesh::CalculateVolume() const
{
	int i;
	for (i = 0; i < NUM_ELEMENT_TYPES; ++i)
	{
		if (i != TRI && !m_Indices[i].empty())
		{
			CMesh TriMesh(*this);
			TriMesh.ConvertToTriangleMesh();  // Converts whole mesh to triangle not just current element type
			return TriMesh.CalculateVolume(); // Which is why can return after CalculateVolume
		}
	}

	double dVolume = 0;
	// Code from http://www.gamedev.net/reference/articles/article2247.asp
	const list<int> &TriangleIndices = m_Indices[TRI];
	XYZ P1, P2, P3;
	list<int>::const_iterator itIter;
	for (itIter = TriangleIndices.begin(); itIter != TriangleIndices.end(); )
	{
		P1 = m_Nodes[*(itIter++)];
		P2 = m_Nodes[*(itIter++)];
		P3 = m_Nodes[*(itIter++)];

		dVolume += ((P2.y-P1.y)*(P3.z-P1.z)-(P2.z-P1.z)*(P3.y-P1.y)) * (P1.x+P2.x+P3.x);
	}

	return dVolume / 6;
}

vector<XYZ> CMesh::GetElementCenters() const
{
	TGLOG("Get Element Centres");
	vector<XYZ> ElementCenters;
	int i, j, iNumNodes;
	list<int>::const_iterator itIndex;
	for (i = 0; i < NUM_ELEMENT_TYPES; ++i)
	{
		iNumNodes = GetNumNodes((ELEMENT_TYPE)i);
		if ( iNumNodes != -1 )
		{
			for (itIndex = m_Indices[i].begin(); itIndex != m_Indices[i].end(); )
			{
				XYZ Center;
				for (j=0; j<iNumNodes; ++j)
				{
					Center += m_Nodes[*itIndex];
					++itIndex;
				}
				Center /= iNumNodes;
				ElementCenters.push_back(Center);
			}
		}
	}
	return ElementCenters;
}

vector<XYZ> CMesh::GetElementCenters( ELEMENT_TYPE type )
{
	vector<XYZ> ElementCentres;
	int iNumNodes = GetNumNodes(type);
	list<int>::const_iterator itIndex;
	for ( itIndex = m_Indices[type].begin(); itIndex != m_Indices[type].end(); )
	{
		XYZ Centre;
		for (int i=0; i<iNumNodes; ++i)
		{
			Centre += m_Nodes[*itIndex];
			++itIndex;
		}
		Centre /= iNumNodes;
		ElementCentres.push_back(Centre);
	}
	return ElementCentres;
}

int CMesh::CountInvertedElements() const
{
	int i;
	int iNumInvertedElements = 0;
	for (i = 0; i < NUM_ELEMENT_TYPES; ++i)
	{
		iNumInvertedElements += CountInvertedElements((ELEMENT_TYPE)i);
	}
	return iNumInvertedElements;
}

int CMesh::CountInvertedElements(ELEMENT_TYPE ElementType) const
{
	if (ElementType != TET && ElementType != PYRAMID && ElementType != WEDGE)
	{
		return 0;
	}
	list<int>::const_iterator itIter;
	int i, iNumInvertedElements = 0;
	CMesh CopiedMesh;
	CopiedMesh.m_Nodes = m_Nodes;
	const list<int> &Indices = m_Indices[ElementType];
	int iNumNodes = GetNumNodes(ElementType);
	for (itIter = Indices.begin(); itIter != Indices.end(); )
	{
		CopiedMesh.m_Indices[ElementType].clear();
		for (i=0; i<iNumNodes; ++i)
		{
			CopiedMesh.m_Indices[ElementType].push_back(*(itIter++));
		}
		if (CopiedMesh.CalculateVolume() < 0)
		{
			++iNumInvertedElements;
		}
	}
	return iNumInvertedElements;
}

int CMesh::IntersectLine(const XYZ &P1, const XYZ &P2, vector<pair<double, XYZ> > &IntersectionPoints, pair<bool, bool> TrimResults, bool bForceFind/*, const CElementsOctree *pOctree*/) const
{
	int i;
	for (i = 0; i < NUM_ELEMENT_TYPES; ++i)
	{
		if (i != TRI && !m_Indices[i].empty())
		{
			TGERROR("Warning: IntersectLine only works for triangle mesh, mesh contains other element types");
			break;
		}
	}

	IntersectionPoints.clear();

	XYZ T1, T2, T3;
	XYZ Normal, Intersection;
	double dU;
	const double dTolerance = 1e-9;
/*	if (pOctree && pOctree->GetOctree())
	{
		list<vector<int> > Elements;
		list<vector<int> >::iterator itIter;

		COctreeVisitorElementNearLine Visitor(P1, P2, Elements, TrimResults);

		pOctree->GetOctree()->visit(Visitor);

		for (itIter = Elements.begin(); itIter != Elements.end(); ++itIter)
		{
			T1 = m_Nodes[(*itIter)[0]];
			T2 = m_Nodes[(*itIter)[1]];
			T3 = m_Nodes[(*itIter)[2]];

			Normal = CrossProduct(T2-T1, T3-T1);
			Normalise(Normal);

			if (GetIntersectionLinePlane(P1, P2, T1, Normal, Intersection, &dU))
			{
				if (TrimResults.first && dU < 0)
					; // Do nothing
				else if(TrimResults.second && dU > 1)
					; // Do nothing
				else if (PointInsideTriangle(T1, T2, T3, Intersection, Normal))
				{
					IntersectionPoints.push_back(pair<double, XYZ>(dU, Normal));
				}
			}
		}
	}
	else
	{*/
		bool bFirst = true;
		double dMin;
		double dAccuracy;
		double dBestU;
		XYZ BestNormal;

		const list<int> &TriangleIndices = m_Indices[TRI];
		list<int>::const_iterator itIter;
		for (itIter = TriangleIndices.begin(); itIter != TriangleIndices.end(); )
		{
			T1 = m_Nodes[*(itIter++)];
			T2 = m_Nodes[*(itIter++)];
			T3 = m_Nodes[*(itIter++)];

			Normal = CrossProduct(T2-T1, T3-T1);
			if (Normal)
			{
				Normalise(Normal);

				if (GetIntersectionLinePlane(P1, P2, T1, Normal, Intersection, &dU))
				{
					if (TrimResults.first && dU < 0)
						continue; // Do nothing
					if(TrimResults.second && dU > 1)
						continue; // Do nothing
					dMin = PointInsideTriangleAccuracy(T1, T2, T3, Intersection, Normal);
					if (dMin >= -dTolerance)
					{
						IntersectionPoints.push_back(pair<double, XYZ>(dU, Normal));
					}
					if (bFirst || dMin > dAccuracy)
					{
						dAccuracy = dMin;
						bFirst = false;
						dBestU = dU;
						BestNormal = Normal;
					}
/*					if (PointInsideTriangle(T1, T2, T3, Intersection, Normal))
					{
						IntersectionPoints.push_back(pair<double, XYZ>(dU, Normal));
					}*/
				}
			}
		}
//	}

	if (bForceFind && IntersectionPoints.empty())
	{
		IntersectionPoints.push_back(pair<double, XYZ>(dBestU, BestNormal));
	}

	sort(IntersectionPoints.begin(), IntersectionPoints.end(), LessPairDoubleXYZ());

	return IntersectionPoints.size();
}

void CMesh::AddOrCancel(list<pair<int, int> > &EdgeStack, pair<int, int> Edge)
{
	list<pair<int, int> >::iterator FindResult;
	FindResult = find(EdgeStack.begin(), EdgeStack.end(), Edge);
	if (FindResult != EdgeStack.end())
	{
		EdgeStack.erase(FindResult);
		return;
	}
	FindResult = find(EdgeStack.begin(), EdgeStack.end(), pair<int, int>(Edge.second, Edge.first));
	if (FindResult != EdgeStack.end())
	{
		EdgeStack.erase(FindResult);
		return;
	}
	EdgeStack.push_back(Edge);
}

void CMesh::MeshConvexHull()
{
	// Algorithm and code taken from: (the incremental method is implemented since it seems to be
	// the easiest to implement and quite robust, I tried to implement the gift wrap method however
	// it doesn't seem as robust, problems occur when points are collinear and/or coplanar)
	// http://www.cse.unsw.edu.au/~lambert/java/3d/hull.html
	// http://www.cse.unsw.edu.au/~lambert/java/3d/incremental.html
	// http://www.cse.unsw.edu.au/~lambert/java/3d/source/Incremental.java
	const double TOL = 1e-7;

	list<pair<int, int> > EdgeStack;
	list<pair<int, int> >::iterator itEdge;

	MergeNodes();

	int i;
	for (i = 0; i < NUM_ELEMENT_TYPES; ++i)
	{
		m_Indices[i].clear();
	}

	if (m_Nodes.size() < 3)
		return;

	list<int> &TriangleIndices = m_Indices[TRI];
	// Generate a list of triangle normals
	list<PLANE> TrianglePlanes;
	PLANE Plane;
	XYZ P1, P2, P3, P;

	// Find the first set of 3 points that are not collinear to create a triangle
	for (i=0; i+2<(int)m_Nodes.size(); ++i)
	{
		P1 = m_Nodes[i+0];
		P2 = m_Nodes[i+1];
		P3 = m_Nodes[i+2];
		Plane.Normal = CrossProduct(P2 - P1, P3 - P1);
		if (GetLengthSquared(Plane.Normal) > TOL*TOL)
		{
			break;
		}
	}
	if (i+2 == (int)m_Nodes.size())
	{
		TGERROR("Unable to create convex hull, all points are collinear");
		assert(false);
		return;
	}

	TriangleIndices.push_back(i+0);
	TriangleIndices.push_back(i+1);
	TriangleIndices.push_back(i+2);
	Normalise(Plane.Normal);
	Plane.d = DotProduct(Plane.Normal, P1);
	TrianglePlanes.push_back(Plane);

	TriangleIndices.push_back(i+0);
	TriangleIndices.push_back(i+2);
	TriangleIndices.push_back(i+1);
	Plane.Normal = -Plane.Normal;
	Plane.d = -Plane.d;
	TrianglePlanes.push_back(Plane);

	list<int>::iterator itIter, itTriStart;
	list<PLANE>::iterator itPlane;
	int i1, i2, i3;
	bool bFacesAdded;
	// This loop will be iterated until no more faces are added, this is necessary when
	// we have more than 3 nodes that are coplanar. If not some nodes will be missed.
	do
	{
		bFacesAdded = false;
		for (i=0; i<(int)m_Nodes.size(); ++i)
		{
			P = m_Nodes[i];
			// Delete faces that this vertex can see
			for (itIter = TriangleIndices.begin(), itPlane = TrianglePlanes.begin(); itIter != TriangleIndices.end(); )
			{
				itTriStart = itIter;
				i1 = *(itIter++);
				i2 = *(itIter++);
				i3 = *(itIter++);
				// If the vertex can see the plane (with a tolerance) we don't want to remove
				// triangles that are in the same place as the vertex or the algorithm will fail.
				if (DotProduct(itPlane->Normal, P) > itPlane->d+TOL)
				{
					// If the edge already exist in the edge stack then it should cancel with it
					// (remove the edge rather than adding it) otherwise add the edge.
					AddOrCancel(EdgeStack, pair<int, int>(i1, i2));
					AddOrCancel(EdgeStack, pair<int, int>(i2, i3));
					AddOrCancel(EdgeStack, pair<int, int>(i3, i1));
					// Delete the triangle
					itIter = TriangleIndices.erase(itTriStart, itIter);
					itPlane = TrianglePlanes.erase(itPlane);
				}
				else
					++itPlane;
			}
			// Create new triangles and calculate the planes of the new triangle.
			// Not only is it an optimisation to calculate the triangle planes only once
			// it is also for consitency.
			for (itEdge = EdgeStack.begin(); itEdge != EdgeStack.end(); ++itEdge)
			{
				i1 = itEdge->first;
				i2 = itEdge->second;
				i3 = i;
				TriangleIndices.push_back(i1);
				TriangleIndices.push_back(i2);
				TriangleIndices.push_back(i3);
				P1 = m_Nodes[i1];
				P2 = m_Nodes[i2];
				P3 = m_Nodes[i3];
				Plane.Normal = CrossProduct(P2 - P1, P3 - P1);
				Normalise(Plane.Normal);
				Plane.d = DotProduct(Plane.Normal, P1);
				TrianglePlanes.push_back(Plane);
				bFacesAdded = true;
			}
			EdgeStack.clear();
		}
	} while (bFacesAdded);
}

void CMesh::BuildGrid(XYZ Min, XYZ Max, int iNumX, int iNumY, int iNumZ)
{
	XYZ P;
	int i, j, k;
	for (i=0; i<iNumX; ++i)
	{
		for (j=0; j<iNumY; ++j)
		{
			for (k=0; k<iNumZ; ++k)
			{
				P.x = i/double(iNumX-1);
				P.y = j/double(iNumY-1);
				P.z = k/double(iNumZ-1);
				P = Min + (Max-Min)*P;
				m_Nodes.push_back(P);
				// Create hex elements out of the grid
				if (i < iNumX-1 && j < iNumY-1 && k < iNumZ-1)
				{
					m_Indices[HEX].push_back((k+0) + (j+0)*iNumZ + (i+0)*iNumZ*iNumY);
					m_Indices[HEX].push_back((k+0) + (j+0)*iNumZ + (i+1)*iNumZ*iNumY);
					m_Indices[HEX].push_back((k+0) + (j+1)*iNumZ + (i+1)*iNumZ*iNumY);
					m_Indices[HEX].push_back((k+0) + (j+1)*iNumZ + (i+0)*iNumZ*iNumY);
					m_Indices[HEX].push_back((k+1) + (j+0)*iNumZ + (i+0)*iNumZ*iNumY);
					m_Indices[HEX].push_back((k+1) + (j+0)*iNumZ + (i+1)*iNumZ*iNumY);
					m_Indices[HEX].push_back((k+1) + (j+1)*iNumZ + (i+1)*iNumZ*iNumY);
					m_Indices[HEX].push_back((k+1) + (j+1)*iNumZ + (i+0)*iNumZ*iNumY);
				}
			}
		}
	}
}

void CMesh::BuildGrid(XYZ Min, XYZ Max, double dPointsPerUnit)
{
	int iNumX, iNumY, iNumZ;
	iNumX = Round((Max.x-Min.x)*dPointsPerUnit);
	iNumY = Round((Max.y-Min.y)*dPointsPerUnit);
	iNumZ = Round((Max.z-Min.z)*dPointsPerUnit);
	BuildGrid(Min, Max, iNumX, iNumY, iNumZ);
}

void CMesh::WriteBinaryXYZ(ostream &Output, XYZ Vector)
{
	float val;
	int i;
	for (i=0; i<3; ++i)
	{
		val = (float)Vector[i];
		Output.write((char*)&val, 4);
	}
}

bool CMesh::SaveToSTL(string Filename, bool bBinary) const
{
	AddExtensionIfMissing(Filename, ".stl");

	int i;
	for (i = 0; i < NUM_ELEMENT_TYPES; ++i)
	{
		if (i != TRI && !m_Indices[i].empty())
		{
			CMesh TriMesh(*this);
			TriMesh.ConvertToTriangleMesh();
			return TriMesh.SaveToSTL(Filename, bBinary);
		}
	}

	ofstream Output;
	if (bBinary)
		Output.open(Filename.c_str(), ios::out|ios::binary);
	else
		Output.open(Filename.c_str(), ios::out);
	if (!Output)
		return false;

	const list<int> &TriangleIndices = m_Indices[TRI];

	if (bBinary)
	{
		char szHeader[80];
		int iNumFacets = TriangleIndices.size()/3;
		strncpy(szHeader, Filename.c_str(), 80);
		Output.write(szHeader, 80);
		Output.write((char*)&iNumFacets, 4);
	}
	else
		Output << "solid " << Filename << endl;

	XYZ T1, T2, T3, Normal;
	list<int>::const_iterator itIndex;
	short int Padding = 0;
	for (itIndex = TriangleIndices.begin(); itIndex != TriangleIndices.end(); )
	{
		T1 = m_Nodes[*(itIndex++)];
		T2 = m_Nodes[*(itIndex++)];
		T3 = m_Nodes[*(itIndex++)];

		Normal = CrossProduct(T2-T1, T3-T1);
		Normalise(Normal);

		if (bBinary)
		{
			WriteBinaryXYZ(Output, Normal);
			WriteBinaryXYZ(Output, T1);
			WriteBinaryXYZ(Output, T2);
			WriteBinaryXYZ(Output, T3);
			Output.write((char*)&Padding, 2);
		}
		else
		{
			Output << " facet normal " << Normal.x << " " << Normal.y << " " << Normal.z << endl;
			Output << "  outer loop" << endl;
			Output << "   vertex " << T1.x << " " << T1.y << " " << T1.z << endl;
			Output << "   vertex " << T2.x << " " << T2.y << " " << T2.z << endl;
			Output << "   vertex " << T3.x << " " << T3.y << " " << T3.z << endl;
			Output << "  endloop" << endl;
			Output << " endfacet" << endl;
		}
	}

	if (!bBinary)
		Output << "endsolid " << Filename << endl;

	Output.close();

	return true;
}

bool CMesh::SaveToSMESH(string Filename) const
{
	AddExtensionIfMissing(Filename, ".smesh");

	ofstream Output(Filename.c_str(), ios::out);
	if (!Output)
		return false;
	Output << "# node count, 3 dim, no attribute, no boundary marker" << endl;
	Output << m_Nodes.size() << " 3 0 0" << endl;

	Output << "# node index, node coordinates" << endl;
	int iNodeIndex;
	vector<XYZ>::const_iterator itNode;
	for (itNode = m_Nodes.begin(), iNodeIndex=0; itNode != m_Nodes.end(); ++itNode, ++iNodeIndex)
	{
		Output << iNodeIndex << " " << itNode->x << " " << itNode->y << " " << itNode->z << endl;
	}

	int iNumTriangles = m_Indices[TRI].size()/3;
	int iNumQuads = m_Indices[QUAD].size()/4;

	Output << "# facet count, no boundary marker" << endl;
	Output << iNumTriangles+iNumQuads << " 0" << endl;
	Output << "# facets" << endl;
	list<int>::const_iterator itIndex;
	int iNumNodesPerElement;
	int i, j;
	ELEMENT_TYPE ElemType;
	for (j=0; j<2; ++j)
	{
		if (j==0)
			ElemType = QUAD;
		else
			ElemType = TRI;
		iNumNodesPerElement = GetNumNodes(ElemType);
		for (itIndex = m_Indices[ElemType].begin(); itIndex != m_Indices[ElemType].end(); )
		{
			Output << iNumNodesPerElement << " ";
			for (i=0; i<iNumNodesPerElement; ++i, ++itIndex)
			{
				if (i>0)
					Output << " ";
				Output << (*itIndex);
			}
			Output << endl;
		}
	}
	Output << "# Part 3 - hole list" << endl;
	Output << "0 # no hole" << endl;
	Output << "# Part 4 - region list" << endl;
	Output << "0 # no region" << endl;

	return true;
}
/*
bool CMesh::ReadFromTetGen(string NodesFile, string ElementsFile, string FacesFile)
{
	// Development paused, implemented in python more easily
	ifstream Nodes(NodesFile.c_str());
	ifstream Elements(ElementsFile.c_str());
	ifstream Faces(FacesFile.c_str());

	if (Nodes)
	{
		int iNumNodes;
	}
	else
		return false;	// Not much point in reading the other files if we don't have nodal information
	if (Elements)
	{

	}
	if (Faces)
	{

	}
	return true;
}
*/

bool CMesh::SaveToABAQUS(string Filename, const vector<POINT_INFO> *pElementInfo, bool bCreateStep, bool bCreateMaterial, int iElementType)
{
	AddExtensionIfMissing(Filename, ".inp");

	ofstream Output(Filename.c_str());

	if (!Output)
	{
		TGERROR("Unable to output mesh to ABAQUS file format, could not open file: " << Filename);
		return false;
	}

	TGLOG("Saving mesh data to " << Filename);

	Output << "*Heading" << endl;
	Output << "File generated by TexGen v" << TEXGEN.GetVersion() << endl;

	Output << "************" << endl;
	Output << "*** MESH ***" << endl;
	Output << "************" << endl;
	Output << "*Node" << endl;
	OutputNodes(Output, 1);

	int iStartIndex = 1;

	// Linear elements
	if (!m_Indices[TET].empty())
	{
		Output << "*Element, Type=C3D4" << endl;
		iStartIndex = OutputElements(Output, TET, iStartIndex, 1);
	}

	if (!m_Indices[WEDGE].empty())
	{
		Output << "*Element, Type=C3D6" << endl;
		// NOTE: The element ordering is different between ABAQUS and VTK...
		// and since the index ordering in this class is based on VTK we need
		// to reorder the elements :(. So we won't use the OutputElements function
		// in this case
//		iStartIndex = OutputElements(Output, WEDGE, iStartIndex, 1);
		int i1, i2, i3, i4, i5, i6;
		int iElementNumber = 0;
		list<int>::const_iterator itIndex;
		for (itIndex = m_Indices[WEDGE].begin(); itIndex != m_Indices[WEDGE].end(); ++iElementNumber)
		{
			i1 = *(itIndex++)+1;
			i2 = *(itIndex++)+1;
			i3 = *(itIndex++)+1;
			i4 = *(itIndex++)+1;
			i5 = *(itIndex++)+1;
			i6 = *(itIndex++)+1;
			Output << iElementNumber+iStartIndex
				<< ", " << i4 << ", " << i5 << ", " << i6
				<< ", " << i1 << ", " << i2 << ", " << i3
				<< endl;
		}
		iStartIndex += iElementNumber;
	}

	if (!m_Indices[HEX].empty())
	{
		if ( !iElementType )
		{
			Output << "*Element, Type=C3D8R" << endl;
		}
		else
		{
			Output << "*Element, Type=C3D8" << endl;
		}
		iStartIndex = OutputElements(Output, HEX, iStartIndex, 1);
	}

	// Quadratic elements
	if (!m_Indices[QUADRATIC_TET].empty())
	{
		Output << "*Element, Type=C3D10" << endl;
		iStartIndex = OutputElements(Output, QUADRATIC_TET, iStartIndex, 1);
	}

	// Shell elements
	if (!m_Indices[QUAD].empty())
	{
		Output << "*Element, Type=S4R" << endl;
		iStartIndex = OutputElements(Output, QUAD, iStartIndex, 1);
	}

	if (pElementInfo)
	{
		string OrientationsFilename = Filename;
		OrientationsFilename.replace(OrientationsFilename.end()-4, OrientationsFilename.end(), ".ori");
		ofstream OriOutput(OrientationsFilename.c_str());
		string ElementDataFilename = Filename;
		ElementDataFilename.replace(ElementDataFilename.end()-4, ElementDataFilename.end(), ".eld");
		ofstream DataOutput(ElementDataFilename.c_str());

		if (!OriOutput)
		{
			TGERROR("Unable to output orientations, could not open file: " << OrientationsFilename);
			return false;
		}
		if (!DataOutput)
		{
			TGERROR("Unable to output additional element data, could not open file: " << ElementDataFilename);
			return false;
		}

		TGLOG("Saving element orientations data to " << OrientationsFilename);
		TGLOG("Saving additional element data to " << ElementDataFilename);

		WriteOrientationsHeader( Output );
		Output << "*Distribution Table, Name=TexGenOrientationVectors" << endl;
		Output << "COORD3D,COORD3D" << endl;
		Output << "*Distribution, Location=Element, Table=TexGenOrientationVectors, Name=TexGenOrientationVectors, Input=" << StripPath(OrientationsFilename) << endl;
		Output << "*Orientation, Name=TexGenOrientations, Definition=coordinates" << endl;
		Output << "TexGenOrientationVectors" << endl;
		Output << "1, 0" << endl;

		// Default orientation
		WriteOrientationsHeader( OriOutput );
		OriOutput <<  ", 1.0, 0.0, 0.0,   0.0, 1.0, 0.0" << endl;

		int i;
		
		WriteElementsHeader( DataOutput );

		map<int, vector<int> > ElementSets;
		vector<POINT_INFO>::const_iterator itData;
		for (itData = pElementInfo->begin(), i=1; itData != pElementInfo->end(); ++itData, ++i)
		{
			if (itData->iYarnIndex != -1)
			{
				if ( GetLength(itData->Up ) )
				{
					XYZ Up = itData->Up;
					XYZ Dir = itData->Orientation;
				
					XYZ Perp = CrossProduct(Dir, Up);
					Normalise(Perp);
					OriOutput << i << ", " << Dir << ",   " << Perp << endl;
				}
				else
				{
					// Default orientation
					OriOutput << i << ", 1.0, 0.0, 0.0,   0.0, 1.0, 0.0" << endl;
				}
			}
			else
			{
				// Default orientation
				OriOutput << i << ", 1.0, 0.0, 0.0,   0.0, 1.0, 0.0" << endl;
			}
			DataOutput << i;
			DataOutput << ", " << itData->iYarnIndex;
			DataOutput << ", " << itData->Location;		// This counts as 2 DepVars
			DataOutput << ", " << itData->dVolumeFraction;
			DataOutput << ", " << itData->dSurfaceDistance;
			DataOutput << endl;
			ElementSets[itData->iYarnIndex].push_back(i);
		}

		// Output element sets
		Output << "********************" << endl;
		Output << "*** ELEMENT SETS ***" << endl;
		Output << "********************" << endl;
		Output << "** TexGen generates a number of element sets:" << endl;
		Output << "** All - Contains all elements" << endl;
		Output << "** Matrix - Contains all elements belonging to the matrix" << endl;
		Output << "** YarnX - Where X represents the yarn index" << endl;
		Output << "*ElSet, ElSet=All, Generate" << endl;
		Output << "1, " << GetNumElements() << ", 1" << endl;
		vector<int>::iterator itIndices;
		map<int, vector<int> >::iterator itElementSet;
		for (itElementSet = ElementSets.begin(); itElementSet != ElementSets.end(); ++itElementSet)
		{
			if (itElementSet->first == -1)
				Output << "*ElSet, ElSet=Matrix" << endl;
			else
				Output << "*ElSet, ElSet=Yarn" << itElementSet->first << endl;
			int iLinePos = 0;
			for (itIndices = itElementSet->second.begin(); itIndices != itElementSet->second.end(); ++itIndices)
			{
				if (iLinePos == 0)
				{
					// Do nothing...
				}
				else if (iLinePos < 16)
				{
					Output << ", ";
				}
				else
				{
					Output << endl;
					iLinePos = 0;
				}
				Output << *itIndices;
				++iLinePos;
			}
			Output << endl;
		}

		// Output materials, this is only here because it is necessary in order to associate
		// orientations and other things to the yarns. It just creates a linear elastic isotropic
		// material with young's modulus of 1.
		if (bCreateMaterial)
		{
			Output << "*****************" << endl;
			Output << "*** MATERIALS ***" << endl;
			Output << "*****************" << endl;
			Output << "*Material, Name=TexGenGenerated" << endl;
			Output << "*Elastic" << endl;
			Output << "1.0, 0.0" << endl;
			Output << "*DepVar" << endl;
			Output << "5" << endl;
	//		Output << "*Solid Section, ElSet=All, Material=TexGenGenerated, Orientation=TexGenOrientations" << endl;
	//		Output << "1.0," << endl;
			for (itElementSet = ElementSets.begin(); itElementSet != ElementSets.end(); ++itElementSet)
			{
				// Let's create 1 section definition for the matrix and each yarn so that it is easy
				// to apply different material properties to different yarns in ABAQUS CAE
				ostringstream SetName;
				if (itElementSet->first == -1)
				{
					SetName << "Matrix";
				}
				else
				{
					SetName << "Yarn" << itElementSet->first;
				}
				Output << "*Solid Section, ElSet=" << SetName.str() << ", Material=TexGenGenerated, Orientation=TexGenOrientations" << endl;
				Output << "1.0," << endl;
			}
			Output << "** Note: Additional element data are stored as a depvars:" << endl;
			Output << "** 1 - Yarn Index (-1 for matrix, first yarn starting at 0)" << endl;
			Output << "** 2/3 - Location (x and y cross-section coordinates of element relative to yarn centerline)" << endl;
			Output << "** 4 - Volume fraction" << endl;
			Output << "** 5 - Distance of element from the surface of the yarn (for yarn elements only, distance is negative)" << endl;
			Output << "*Initial Conditions, Type=Solution, Input=" << StripPath(ElementDataFilename) << endl;
		}

		if (bCreateStep)
		{
			Output << "************" << endl;
			Output << "*** STEP ***" << endl;
			Output << "************" << endl;
			Output << "*Step, Name=TexGenGenerated" << endl;
			Output << "*Static" << endl;
			Output << "*Output, Field, Variable=PRESELECT" << endl;
	//		Output << "*Output, History, Variable=PRESELECT" << endl;
			Output << "*Element Output" << endl;
			Output << "SDV" << endl;
			Output << "*Node Print" << endl;
			Output << "U" << endl;
			Output << "*End Step" << endl;
		}
	}

	return true;
}

bool CMesh::SaveToVTK(string Filename, const vector<CMeshDataBase*> *pMeshData) const
{
	AddExtensionIfMissing(Filename, ".vtu");

	TiXmlDocument doc;
	TiXmlDeclaration decl("1.0", "", "");
	doc.InsertEndChild(decl);
	TiXmlElement VTKFile("VTKFile");
	{
		VTKFile.SetAttribute("type", "UnstructuredGrid");
		VTKFile.SetAttribute("version", "0.1");
		VTKFile.SetAttribute("byte_order", "LittleEndian");
		TiXmlElement UnstructuredGrid("UnstructuredGrid");
		{
			TiXmlElement Piece("Piece");
			{
				TiXmlElement Points("Points");
				int iNumPoints = FillVTKPointData(Points);
				Piece.InsertEndChild(Points);

				TiXmlElement Cells("Cells");
				int iNumCells = FillVTKCellData(Cells);
				Piece.InsertEndChild(Cells);

				if (pMeshData)
				{
					TiXmlElement PointData("PointData");
					TiXmlElement CellData("CellData");
					vector<CMeshDataBase*>::const_iterator itData;
					for (itData = pMeshData->begin(); itData != pMeshData->end(); ++itData)
					{
						if ((*itData)->m_DataType == CMeshDataBase::NODE)
							(*itData)->InsertVTKData(PointData);
						else
							(*itData)->InsertVTKData(CellData);
					}
					TGLOG("Finished iterator");
					Piece.InsertEndChild(PointData);
					Piece.InsertEndChild(CellData);
					TGLOG("End InsertVTKData");
				}

				Piece.SetAttribute("NumberOfPoints", stringify(iNumPoints));
				Piece.SetAttribute("NumberOfCells", stringify(iNumCells));
			}
			UnstructuredGrid.InsertEndChild(Piece);
		}
		VTKFile.InsertEndChild(UnstructuredGrid);
	}

	doc.InsertEndChild(VTKFile);
	TGLOG("Return call SaveFile");
	return doc.SaveFile(Filename);
}
/*
bool CMesh::SaveToVTK(string Filename, const vector<TiXmlElement> *pAdditionalData) const
{
	AddExtensionIfMissing(Filename, ".vtu");

	TiXmlDocument doc;
	TiXmlDeclaration decl("1.0", "", "");
	doc.InsertEndChild(decl);
	TiXmlElement VTKFile("VTKFile");
	{
		VTKFile.SetAttribute("type", "UnstructuredGrid");
		VTKFile.SetAttribute("version", "0.1");
		VTKFile.SetAttribute("byte_order", "LittleEndian");
		TiXmlElement UnstructuredGrid("UnstructuredGrid");
		{
			TiXmlElement Piece("Piece");
			{
				TiXmlElement Points("Points");
				int iNumPoints = FillVTKPointData(Points);
				Piece.InsertEndChild(Points);

				TiXmlElement Cells("Cells");
				int iNumCells = FillVTKCellData(Cells);
				Piece.InsertEndChild(Cells);

				if (pAdditionalData)
				{
					vector<TiXmlElement>::const_iterator itElement;
					for (itElement = pAdditionalData->begin(); itElement != pAdditionalData->end(); ++itElement)
						Piece.InsertEndChild(*itElement);
				}

				Piece.SetAttribute("NumberOfPoints", stringify(iNumPoints));
				Piece.SetAttribute("NumberOfCells", stringify(iNumCells));
			}
			UnstructuredGrid.InsertEndChild(Piece);
		}
		VTKFile.InsertEndChild(UnstructuredGrid);
	}

	doc.InsertEndChild(VTKFile);

	return doc.SaveFile(Filename);
}
*/
int CMesh::FillVTKPointData(TiXmlElement &Points) const
{
	TGLOG("FillVTKPointData");
	ostringstream PointsData;
	vector<XYZ>::const_iterator itNode;
	for (itNode = m_Nodes.begin(); itNode != m_Nodes.end(); ++itNode)
	{
		PointsData << itNode->x << " " << itNode->y << " " << itNode->z << " ";
	}

	TiXmlElement DataArray("DataArray");
	{
		DataArray.SetAttribute("type", "Float32");
		DataArray.SetAttribute("NumberOfComponents", "3");
		DataArray.SetAttribute("format", "ascii");
		DataArray.InsertEndChild(TiXmlText(PointsData.str()));
	}
	Points.InsertEndChild(DataArray);
	TGLOG("return");
	return m_Nodes.size();
}

int CMesh::FillVTKCellData(TiXmlElement &Cells) const
{
	TGLOG("FillVTKCellData");
	ostringstream ConnectivityData;
	ostringstream OffsetsData;
	ostringstream TypesData;

	int i, iOffset = 0;
	int iTotalNumCells = 0;
	list<int>::const_iterator itIndex;
	int j;
	for (j = 0; j < NUM_ELEMENT_TYPES; ++j)
	{
		int iVTKType = 0;
		switch (j)
		{
		case TRI:
			iVTKType = 5;
			break;
		case QUAD:
			iVTKType = 9;
			break;
		case TET:
			iVTKType = 10;
			break;
		case WEDGE:
			iVTKType = 13;
			break;
		case PYRAMID:
			iVTKType = 14;
			break;
		case HEX:
			iVTKType = 12;
			break;
		case LINE:
			iVTKType = 3;
			break;
		case POLYLINE:
			iVTKType = 4;
			break;
		case QUADRATIC_TET:
			iVTKType = 24;
			break;
		}
		if (iVTKType != 0)
		{
			int iNumNodesPerElement = GetNumNodes((ELEMENT_TYPE)j);
			int iNumElements = m_Indices[j].size()/iNumNodesPerElement;
			for (itIndex = m_Indices[j].begin(), i=0; itIndex != m_Indices[j].end(); ++itIndex, ++i)
			{
				ConnectivityData << *itIndex << " ";
			}
			for (i=0; i<iNumElements; ++i)
			{
				TypesData << iVTKType << " ";
			}
			for (i=0; i<iNumElements; ++i)
			{
				iOffset += iNumNodesPerElement;
				OffsetsData << iOffset << " ";
			}
			iTotalNumCells += iNumElements;
		}
	}


	TiXmlElement Connectivity("DataArray");
	{
		Connectivity.SetAttribute("type", "Int32");
		Connectivity.SetAttribute("Name", "connectivity");
		Connectivity.SetAttribute("format", "ascii");
		Connectivity.InsertEndChild(TiXmlText(ConnectivityData.str()));
	}
	Cells.InsertEndChild(Connectivity);

	TiXmlElement Offsets("DataArray");
	{
		Offsets.SetAttribute("type", "Int32");
		Offsets.SetAttribute("Name", "offsets");
		Offsets.SetAttribute("format", "ascii");
		Offsets.InsertEndChild(TiXmlText(OffsetsData.str()));
	}
	Cells.InsertEndChild(Offsets);

	TiXmlElement Types("DataArray");
	{
		Types.SetAttribute("type", "Int32");
		Types.SetAttribute("Name", "types");
		Types.SetAttribute("format", "ascii");
		Types.InsertEndChild(TiXmlText(TypesData.str()));
	}
	Cells.InsertEndChild(Types);
	TGLOG("return");
	return iTotalNumCells;
}

bool CMesh::AddElement(ELEMENT_TYPE Type, const vector<int> &Indices)
{
	if (Type < 0 || Type >= NUM_ELEMENT_TYPES)
	{
		TGERROR("Tried to add element of invalid type: " << Type);
		return false;
	}
	if ((int)Indices.size() != GetNumNodes(Type) && Type != CMesh::POLYGON )
	{
		TGERROR("Tried to add element of type " << Type << " with invalid number of indices: " << Indices.size());
		return false;
	}
	if ( Type == CMesh::POLYGON && *(Indices.begin()) != *(Indices.end()-1) )
	{
		TGERROR("Tried to add unclosed POLYGON to mesh" );
		return false;
	}
	m_Indices[Type].insert(m_Indices[Type].end(), Indices.begin(), Indices.end());
	return true;
}

int CMesh::GetNumElements(ELEMENT_TYPE Type) const
{
	assert(Type >= 0 && Type < NUM_ELEMENT_TYPES);
	return m_Indices[Type].size()/GetNumNodes(Type);
}

int CMesh::GetNumElements() const
{
	int iNumElements = 0;
	int i;
	for (i = 0; i < NUM_ELEMENT_TYPES; ++i)
	{
		if ( (ELEMENT_TYPE)i != CMesh::POLYGON )
			iNumElements += GetNumElements((ELEMENT_TYPE)i);
	}
	return iNumElements;
}

vector<XYZ>::const_iterator CMesh::NodesBegin() const
{
	return m_Nodes.begin();
}

vector<XYZ>::const_iterator CMesh::NodesEnd() const
{
	return m_Nodes.end();
}

vector<XYZ>::iterator CMesh::NodesBegin()
{
	return m_Nodes.begin();
}

vector<XYZ>::iterator CMesh::NodesEnd()
{
	return m_Nodes.end();
}

const int CMesh::AddNode(XYZ Node)
{
	m_Nodes.push_back(Node);
	return (int)m_Nodes.size()-1;
}

void CMesh::SetNode(int iIndex, XYZ Node)
{
	assert(iIndex >= 0 && iIndex<(int)m_Nodes.size());
	m_Nodes[iIndex] = Node; 
} 	 

const XYZ& CMesh::GetNode(int iIndex) const
{
	return m_Nodes[iIndex];
}

vector<XYZ>::iterator CMesh::DeleteNode(vector<XYZ>::iterator it)
{
	return m_Nodes.erase(it);
}

int CMesh::GetNumNodes() const
{
	return (int)m_Nodes.size();
}

const bool CMesh::NodesEmpty() const
{
	return m_Nodes.empty();
}

const vector<XYZ>& CMesh::GetNodes() const
{
	return m_Nodes;
}

vector<XYZ>& CMesh::GetNodes()
{
	return m_Nodes;
}

void CMesh::SetNumNodes(int NumNodes)
{
	m_Nodes.resize(NumNodes);
}

const list<int>& CMesh::GetIndices(ELEMENT_TYPE ElemType) const
{
	assert(ElemType >= 0 && ElemType < NUM_ELEMENT_TYPES);
	return m_Indices[ElemType];
}

list<int>& CMesh::GetIndices(ELEMENT_TYPE ElemType)
{
	assert(ElemType >= 0 && ElemType < NUM_ELEMENT_TYPES);
	return m_Indices[ElemType];
}

void CMesh::ConvertElementListToVector( ELEMENT_TYPE ElementType, vector<int> &Indices )
{
	assert(ElementType >= 0 && ElementType < NUM_ELEMENT_TYPES);

	Indices.insert( Indices.begin(), m_Indices[ElementType].begin(), m_Indices[ElementType].end() );
}

bool CMesh::SaveToSCIRun(string Filename) 
{
	Filename = RemoveExtension( Filename, ".pts" );
	AddExtensionIfMissing(Filename, ".tri.pts");

	ofstream Output(Filename.c_str());

	if (!Output)
	{
		TGERROR("Unable to output mesh to SCIRun .pts file format, could not open file: " << Filename);
		return false;
	}

	TGLOG("Saving mesh data to " << Filename);

	
	OutputNodes(Output, 1, ",", true );

	int iStartIndex = 1;

	Filename = RemoveExtension( Filename, ".pts" );
	AddExtensionIfMissing( Filename, ".fac" );

	ofstream ElementOutput( Filename.c_str());

	if (!ElementOutput)
	{
		TGERROR("Unable to output mesh to SCIRun .fac file format, could not open file: " << Filename);
		return false;
	}

	TGLOG("Saving mesh data to " << Filename);

	ConvertQuadstoTriangles();

	// Tri elements
	if (!m_Indices[TRI].empty())
	{
		iStartIndex = OutputElements(ElementOutput, TRI, iStartIndex, 1, ", ", true);
	}
	return true;
}


