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
#include "GeometrySolver.h"
#include "TexGen.h"
#include <iterator>
extern "C"
{
#include "../CSparse/Include/cs.h"
}

using namespace TexGen;
CGeometrySolver::CGeometrySolver(void)
: m_dHeightTolerance(1e-3)
, m_dDisabledStiffness(1e-4)
, m_dContactStiffness(1e3)
, m_bAdjustThickness(false)
, m_dLongitudinalBendingModulus(1.0)
, m_dTransverseBendingModulus(1.0)
, m_dTensileStress(1.0)
{
}

CGeometrySolver::~CGeometrySolver(void)
{
}

void CGeometrySolver::SetTensileStress(double dTensileStress)
{
	m_dTensileStress = dTensileStress;
	vector<PLATE>::iterator itPlate;
	for (itPlate = m_PlateElements.begin(); itPlate != m_PlateElements.end(); ++itPlate)
	{
		itPlate->TensionElement.SetTensileStress(m_dTensileStress);
	}
}

void CGeometrySolver::SetLongitudinalBendingModulus(double dBendingModulus)
{
	m_dLongitudinalBendingModulus = dBendingModulus;
	vector<PLATE>::iterator itPlate;
	for (itPlate = m_PlateElements.begin(); itPlate != m_PlateElements.end(); ++itPlate)
	{
		itPlate->BendingElement.SetLongitudinalBendingModulus(m_dLongitudinalBendingModulus);
	}
}

void CGeometrySolver::SetTransverseBendingModulus(double dBendingModulus)
{
	m_dTransverseBendingModulus = dBendingModulus;
	vector<PLATE>::iterator itPlate;
	for (itPlate = m_PlateElements.begin(); itPlate != m_PlateElements.end(); ++itPlate)
	{
		itPlate->BendingElement.SetTransverseBendingModulus(m_dTransverseBendingModulus);
	}
}

void CGeometrySolver::SetContactStiffness(double dContactStiffness)
{
	m_dContactStiffness = dContactStiffness;
}

void CGeometrySolver::SetDisabledStiffness(double dDisabledStiffness)
{
	m_dDisabledStiffness = dDisabledStiffness;
}

bool CGeometrySolver::CreateSystem(string TextileName)
{
	CTextile* pTextile = TEXGEN.GetTextile(TextileName);
	if (pTextile)
		return CreateSystem(*pTextile);
	else
		return false;
}

bool CGeometrySolver::CreateSystem(CTextile &Textile)
{
	SetPeriodic(true);

	int i, j;
	if (m_bAdjustThickness)
	{
		int iNumYarns = Textile.GetNumYarns();
		for (i=0; i<iNumYarns; ++i)
		{
			CYarn* pYarn = Textile.GetYarn(i);
			if (pYarn && pYarn->GetInterpolation())
			{
				CObjectContainer<CInterpolation> Interp = *pYarn->GetInterpolation();
				Interp->SetForceInPlaneTangent(true);
				pYarn->AssignInterpolation(*Interp);
			}
		}
	}

	if (!CreateBasicVolumes(Textile))
		return false;

	int iNumNodes = (int)m_ProjectedMesh.GetNumNodes();
	m_ProjectedNodes.clear();
	m_ProjectedNodes.resize(iNumNodes);
	for (i=0; i<iNumNodes; ++i)
	{
		m_ProjectedNodes[i].Position = m_ProjectedMesh.GetNode(i);
		RaiseNodes(i);
	}
	m_SurfaceMesh.Clear();
	m_Nodes.clear();
	NODE Node;
	int iGlobalIndexCount = 0;
	for (i=0; i<iNumNodes; ++i)
	{
		Node.Position = m_ProjectedNodes[i].Position;
		for (j=0; j<(int)m_ProjectedNodes[i].RaisedNodes.size(); ++j)
		{
			RAISED_NODE &RaisedNode = m_ProjectedNodes[i].RaisedNodes[j];
			RaisedNode.iGlobalIndex = iGlobalIndexCount++;
			Node.Position.z = RaisedNode.dHeight;
			Node.dThickness = RaisedNode.dThickness;
			Node.iYarnIndex = RaisedNode.iYarnIndex;
			m_SurfaceMesh.AddNode(Node.Position);
			m_Nodes.push_back(Node);
		}
	}

	CreateSurfaceMesh();

	m_ProjectedMesh.ConvertToSegmentMesh();

	// Create yarn connection springs
//	list<int> &LineIndices = m_ProjectedMesh.m_Indices[CMesh::LINE];
//	list<int>::iterator itIter;
/*	int i1, i2;
	vector<RAISED_NODE>::iterator itRaised1, itRaised2;
	for (itIter = LineIndices.begin(); itIter != LineIndices.end(); )
	{
		i1 = *(itIter++);
		i2 = *(itIter++);
		PROJECTED_NODE &ProjNode1 = m_ProjectedNodes[i1];
		PROJECTED_NODE &ProjNode2 = m_ProjectedNodes[i2];
		Spring.dLength = GetLength(ProjNode1.Position, ProjNode2.Position);
		for (itRaised1 = ProjNode1.RaisedNodes.begin(); itRaised1 != ProjNode1.RaisedNodes.end(); ++itRaised1)
		{
			for (itRaised2 = ProjNode2.RaisedNodes.begin(); itRaised2 != ProjNode2.RaisedNodes.end(); ++itRaised2)
			{
				if (itRaised1->iYarnIndex == itRaised2->iYarnIndex)
				{
					if (itRaised1->dThickness < m_dHeightTolerance && itRaised2->dThickness < m_dHeightTolerance)
						continue;
					Spring.iNode1 = itRaised1->iGlobalIndex;
					Spring.iNode2 = itRaised2->iGlobalIndex;
					m_Springs.push_back(Spring);
				}
			}
		}
	}*/

	// Create plate elements from surface mesh
	CreatePlateElements();
	AssignFibreDirectionToElements();

	// Add yarn contact springs
	m_Springs.clear();
	SPRING Spring;
	Spring.bEnabled = true;
	vector<PROJECTED_NODE>::iterator itProjectedNode;
	vector<RAISED_NODE>::iterator itRaised;
	for (itProjectedNode = m_ProjectedNodes.begin(), i = 0; itProjectedNode != m_ProjectedNodes.end(); ++itProjectedNode, ++i)
	{
		Spring.dArea = GetArea(i);
		for (j=0; j < (int)itProjectedNode->RaisedNodes.size()-1; ++j)
		{
			Spring.iNode1 = itProjectedNode->RaisedNodes[j].iGlobalIndex;
			Spring.iNode2 = itProjectedNode->RaisedNodes[j+1].iGlobalIndex;
			m_Springs.push_back(Spring);
		}
	}

	// Build the constraints
	m_DOFConstraints.clear();

	// Fix the first node's vertical displacement DOF to ensure the system is properly constrained
	m_DOFConstraints.push_back(make_pair(0, 0));
//	m_DOFConstraints.push_back(make_pair(1, 0));
//	m_DOFConstraints.push_back(make_pair(2, 0));

	// Add periodic boundary conditions
	pair<XYZ, XYZ> AABB = m_pTextile->GetDomain()->GetMesh().GetAABB();
	double dWidth = AABB.second.x - AABB.first.x;
	double dHeight = AABB.second.y - AABB.first.y;
	vector<XYZ> Repeats;
	Repeats.push_back(XYZ(dWidth, 0, 0));
	Repeats.push_back(XYZ(0, dHeight, 0));

	ApplyPeriodicBoundaryConditions(Repeats);

	return true;
}

void CGeometrySolver::CreateDebugSystem()
{
	int i, j, k;
	const int iXDivs = 10, iYDivs = 10;
	double u, v;
	for (k=0; k<2; ++k)
	{
		for (i=0; i<iXDivs; ++i)
		{
			u = i/double(iXDivs-1);
			for (j=0; j<iYDivs; ++j)
			{
				v = j/double(iYDivs-1);
				m_SurfaceMesh.AddNode(XYZ(u, v, /*u*u-u - v*v+v+*/0.2*k));
				if (i<iXDivs-1 && j<iYDivs-1)
				{
					m_SurfaceMesh.GetIndices(CMesh::TRI).push_back(i*iYDivs+j + k*iXDivs*iYDivs);
					m_SurfaceMesh.GetIndices(CMesh::TRI).push_back((i+1)*iYDivs+j + k*iXDivs*iYDivs);
					m_SurfaceMesh.GetIndices(CMesh::TRI).push_back(i*iYDivs+j+1 + k*iXDivs*iYDivs);

					m_SurfaceMesh.GetIndices(CMesh::TRI).push_back((i+1)*iYDivs+j + k*iXDivs*iYDivs);
					m_SurfaceMesh.GetIndices(CMesh::TRI).push_back((i+1)*iYDivs+j+1 + k*iXDivs*iYDivs);
					m_SurfaceMesh.GetIndices(CMesh::TRI).push_back(i*iYDivs+j+1 + k*iXDivs*iYDivs);
				}
			}
		}
	}
/*	m_SurfaceMesh.AddNode(XYZ(0, 0, 0));
	m_SurfaceMesh.AddNode(XYZ(1, 0, 0));
	m_SurfaceMesh.AddNode(XYZ(0, 1, 0));
	m_SurfaceMesh.AddNode(XYZ(1, 1, 0));

	m_SurfaceMesh.GetIndices(CMesh::TRI).push_back(0);
	m_SurfaceMesh.GetIndices(CMesh::TRI).push_back(1);
	m_SurfaceMesh.GetIndices(CMesh::TRI).push_back(2);

	m_SurfaceMesh.GetIndices(CMesh::TRI).push_back(1);
	m_SurfaceMesh.GetIndices(CMesh::TRI).push_back(3);
	m_SurfaceMesh.GetIndices(CMesh::TRI).push_back(2);*/

	NODE Node;
	for (i=0; i<m_SurfaceMesh.GetNumNodes(); ++i)
	{
		Node.Position = m_SurfaceMesh.GetNode(i);
		Node.dThickness = 0.2;
		m_Nodes.push_back(Node);
	}

	SPRING Spring;
	Spring.bEnabled = true;
	Spring.dArea = 0.1;
	for (i=0; i<iXDivs*iYDivs; ++i)
	{
		Spring.iNode1 = i;
		Spring.iNode2 = i + iXDivs*iYDivs;
		m_Springs.push_back(Spring);
	}

	// Create plate elements from surface mesh
	CreatePlateElements();

	m_DOFConstraints.clear();
	m_DOFConstraints.push_back(make_pair(0, 0));
	m_DOFConstraints.push_back(make_pair(1, 0.5));
	m_DOFConstraints.push_back(make_pair(2, 0.5));
/*	m_DOFConstraints.push_back(make_pair(0, 0));
	m_DOFConstraints.push_back(make_pair(1, 0));
	m_DOFConstraints.push_back(make_pair(2, 0));*/
//	m_DOFConstraints.push_back(make_pair((iXDivs*iYDivs-1)*3, 0));
/*	m_DOFConstraints.push_back(make_pair(3*iXDivs*iYDivs, 0));
	m_DOFConstraints.push_back(make_pair(3*iXDivs*iYDivs+1, 0.5));
	m_DOFConstraints.push_back(make_pair(3*iXDivs*iYDivs+2, 0.5));*/

	// Apply periodic BCs
	vector<XYZ> Repeats;
	Repeats.push_back(XYZ(1, 0, 0));
	Repeats.push_back(XYZ(0, 1, 0));
	ApplyPeriodicBoundaryConditions(Repeats);
}

void CGeometrySolver::RaiseNodes(int iIndex)
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

	// Create the raised nodes
	XYZ Point = m_ProjectedMesh.GetNode(iIndex);
	RAISED_NODE Node;
	double dMin, dMax;
	for (itYarnIndex=YarnIndices.begin(); itYarnIndex!=YarnIndices.end(); ++itYarnIndex)
	{
		bool bFound = GetMeshVerticalBounds(m_YarnMeshes[*itYarnIndex], Point, dMin, dMax, true);
		assert(bFound);
		Node.dHeight = 0.5*(dMin+dMax);
		Node.dThickness = dMax - dMin;
		Node.iYarnIndex = *itYarnIndex;
		m_ProjectedNodes[iIndex].RaisedNodes.push_back(Node);
	}
	sort(m_ProjectedNodes[iIndex].RaisedNodes.begin(), m_ProjectedNodes[iIndex].RaisedNodes.end());
}

double CGeometrySolver::GetArea(int iIndex)
{
	double dArea = 0;
	vector<PLATE>::iterator itPlate;
	for (itPlate = m_PlateElements.begin(); itPlate != m_PlateElements.end(); ++itPlate)
	{
		if (itPlate->iNode1 == iIndex || itPlate->iNode2 == iIndex || itPlate->iNode3 == iIndex)
		{
			dArea += itPlate->BendingElement.GetArea();
			// same as itPlate->TensionElement.GetArea();
		}
	}
	return dArea / 3.0;
}

double CGeometrySolver::GetAverageLength(int iIndex)
{
	// Create yarn connection springs
	list<int> &LineIndices = m_ProjectedMesh.GetIndices(CMesh::LINE);
	list<int>::iterator itIter;
	int i1, i2;
	double dAverageLength = 0;
	int iNumConnected = 0;
	for (itIter = LineIndices.begin(); itIter != LineIndices.end(); )
	{
		i1 = *(itIter++);
		i2 = *(itIter++);
		if (i1 == iIndex || i2 == iIndex)
		{
			dAverageLength += GetLength(m_ProjectedMesh.GetNode(i1), m_ProjectedMesh.GetNode(i2));
			++iNumConnected;
		}
	}
	dAverageLength /= iNumConnected;
	return dAverageLength;
}

void CGeometrySolver::CreateSurfaceMesh()
{
	list<int>::iterator itIndex;
	list<int> &Indices = m_ProjectedMesh.GetIndices(CMesh::TRI);
	int i1, i2, i3;
	vector<RAISED_NODE>::iterator itRaised1, itRaised2, itRaised3;
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		i1 = *(itIndex++);
		i2 = *(itIndex++);
		i3 = *(itIndex++);
		PROJECTED_NODE &ProjNode1 = m_ProjectedNodes[i1];
		PROJECTED_NODE &ProjNode2 = m_ProjectedNodes[i2];
		PROJECTED_NODE &ProjNode3 = m_ProjectedNodes[i3];
		for (itRaised1 = ProjNode1.RaisedNodes.begin(); itRaised1 != ProjNode1.RaisedNodes.end(); ++itRaised1)
		{
			for (itRaised2 = ProjNode2.RaisedNodes.begin(); itRaised2 != ProjNode2.RaisedNodes.end(); ++itRaised2)
			{
				for (itRaised3 = ProjNode3.RaisedNodes.begin(); itRaised3 != ProjNode3.RaisedNodes.end(); ++itRaised3)
				{
					if (itRaised1->iYarnIndex == itRaised2->iYarnIndex &&
						itRaised2->iYarnIndex == itRaised3->iYarnIndex)
					{
						if (itRaised1->dThickness < m_dHeightTolerance &&
							itRaised2->dThickness < m_dHeightTolerance &&
							itRaised3->dThickness < m_dHeightTolerance)
							continue;
						m_SurfaceMesh.GetIndices(CMesh::TRI).push_back(itRaised1->iGlobalIndex);
						m_SurfaceMesh.GetIndices(CMesh::TRI).push_back(itRaised2->iGlobalIndex);
						m_SurfaceMesh.GetIndices(CMesh::TRI).push_back(itRaised3->iGlobalIndex);
					}
				}
			}
		}
	}
}

void CGeometrySolver::CreatePlateElements()
{
	m_PlateElements.clear();
	PLATE Plate;
	list<int>::iterator itIndex;
	list<int> &Indices = m_SurfaceMesh.GetIndices(CMesh::TRI);
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		Plate.iNode1 = *(itIndex++);
		Plate.iNode2 = *(itIndex++);
		Plate.iNode3 = *(itIndex++);
		Plate.iYarnIndex = m_Nodes[Plate.iNode1].iYarnIndex;
		CMatrix P(3, 2);
		P(0,0) = m_SurfaceMesh.GetNode(Plate.iNode1).x;		P(0,1) = m_SurfaceMesh.GetNode(Plate.iNode1).y;
		P(1,0) = m_SurfaceMesh.GetNode(Plate.iNode2).x;		P(1,1) = m_SurfaceMesh.GetNode(Plate.iNode2).y;
		P(2,0) = m_SurfaceMesh.GetNode(Plate.iNode3).x;		P(2,1) = m_SurfaceMesh.GetNode(Plate.iNode3).y;
		Plate.BendingElement.SetNodeCoordinates(P);
		Plate.TensionElement.SetNodeCoordinates(P);
		Plate.BendingElement.SetLongitudinalBendingModulus(m_dLongitudinalBendingModulus);
		Plate.BendingElement.SetTransverseBendingModulus(m_dTransverseBendingModulus);
		Plate.TensionElement.SetTensileStress(m_dTensileStress);
		m_PlateElements.push_back(Plate);
	}
}

void CGeometrySolver::AssignFibreDirectionToElements()
{
	if (!m_pTextile)
		return;
	vector<PLATE>::iterator itPlate;
	vector<vector<pair<int, XYZ> > > PlateCenters;
	int i, j, iNumYarns = m_pTextile->GetNumYarns();
	PlateCenters.resize(iNumYarns);
	for (itPlate = m_PlateElements.begin(), i=0; itPlate != m_PlateElements.end(); ++itPlate, ++i)
	{
		XYZ Center;
		Center += m_SurfaceMesh.GetNode(itPlate->iNode1);
		Center += m_SurfaceMesh.GetNode(itPlate->iNode2);
		Center += m_SurfaceMesh.GetNode(itPlate->iNode3);
		Center /= 3.0;
		PlateCenters[itPlate->iYarnIndex].push_back(make_pair(i, Center));
	}
	XYZ Tangent;
	for (i=0; i<iNumYarns; ++i)
	{
		CYarn* pYarn = m_pTextile->GetYarn(i);
		vector<XYZ> Translations = m_pTextile->GetDomain()->GetTranslations(*pYarn);
		for (j=0; j<(int)PlateCenters[i].size(); ++j)
		{
			bool bInside = pYarn->PointInsideYarn(PlateCenters[i][j].second, Translations, &Tangent);
			assert(bInside);
			PLATE &Plate = m_PlateElements[PlateCenters[i][j].first];
			Plate.BendingElement.SetFibreDirection(Tangent);
			Plate.TensionElement.SetFibreDirection(Tangent);
		}
	}
/*	vector<POINT_INFO> PointInfo;
	vector<POINT_INFO>::iterator itPointInfo;
	m_pTextile->GetPointInformation(PlateCenters, PointInfo);
	for (itPlate = m_PlateElements.begin(), itPointInfo = PointInfo.begin();
		itPlate != m_PlateElements.end() && itPointInfo != PointInfo.end();
		++itPlate, ++itPointInfo)
	{
		itPlate->BendingElement.SetFibreDirection(itPointInfo->YarnTangent);
		itPlate->TensionElement.SetFibreDirection(itPointInfo->YarnTangent);
	}
	assert(itPlate == m_PlateElements.end() && itPointInfo == PointInfo.end());*/
}

void CGeometrySolver::ApplyPeriodicBoundaryConditions(vector<XYZ> Repeats)
{
	int i;
	vector<pair<int, int> > NodePairs;
	vector<XYZ>::iterator itRepeat;
	for (itRepeat = Repeats.begin(); itRepeat != Repeats.end(); ++itRepeat)
	{
		vector<pair<int, int> > Temp = m_SurfaceMesh.GetNodePairs(*itRepeat);
		NodePairs.insert(NodePairs.end(), Temp.begin(), Temp.end());
	}

	m_DOFLinks.clear();
	vector<pair<int, int> >::iterator itNodePair;
	for (itNodePair = NodePairs.begin(); itNodePair != NodePairs.end(); ++itNodePair)
	{
		if (m_Nodes[itNodePair->first].iYarnIndex != m_Nodes[itNodePair->second].iYarnIndex)
		{
			// Ignore node pairs where nodes have different yarn indices
			continue;
		}
		for (i=0; i<3; ++i)
		{
			pair<int, int> Pair = make_pair(itNodePair->first*3+i, itNodePair->second*3+i);
//			swap(Pair.first, Pair.second);
			m_DOFLinks.push_back(Pair);
		}
	}
}
/*
void CGeometrySolver::OutputSystem(string Filename)
{
	ofstream Output(Filename.c_str());
	int i, j;
	int iNumNodes = (int)m_ProjectedMesh.GetNumNodes();
	Output << "[Nodes]" << endl;
	for (i=0; i<iNumNodes; ++i)
	{
		XYZ Pos = m_ProjectedNodes[i].Position;
		for (j=0; j<(int)m_ProjectedNodes[i].RaisedNodes.size(); ++j)
		{
			RAISED_NODE &Node = m_ProjectedNodes[i].RaisedNodes[j];
			Output << Pos.x << " " << Pos.y << " " << Node.dHeight
				<< " " << Node.dThickness << " " << Node.iYarnIndex << endl;
		}
	}
	Output << "[Springs]" << endl;
	vector<SPRING>::iterator itSpring;
	for (itSpring = m_Springs.begin(); itSpring != m_Springs.end(); ++itSpring)
	{
		Output << itSpring->iNode1 << " " << itSpring->iNode2 << " " << itSpring->dArea << endl;
	}
	Output << "[Surface]" << endl;
	list<int>::iterator itIndex;
	list<int> &Indices = m_SurfaceMesh.GetIndices(CMesh::TRI);
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		Output << *(itIndex++) << " " << *(itIndex++) << " " << *(itIndex++) << endl;
	}
}
*/
int CGeometrySolver::SolveSystem()
{
//	int i, j, iNumProjectedNodes = (int)m_ProjectedNodes.size();
	int iNumNodes = (int)m_Nodes.size();
	int iNumPlates = (int)m_PlateElements.size();
	int iDOFs = iNumNodes*3 + iNumPlates;
	map<pair<int, int>, double> A;
	map<pair<int, int>, double>::iterator itA;
	vector<double> x(iDOFs/*, 0.0*/);
	vector<double> b(iDOFs/*, 0.0*/);
	int iNumSwitches, iIteration = 0;
	double dInitialLength, dCurrentLength;
	double dContactDistance, dSeperationDistance;
	double dStiffness;
	bool bEnabled;
	int i, j, i1, i2, iPlateCount;
	int iGlob, jGlob;
	CMatrix KeB, KeT;
	vector<SPRING>::iterator itSpring;
	vector<PLATE>::iterator itPlate;

//	m_bDebug = true;

	TGLOGINDENT("Solving geometry with " << iNumNodes << " nodes and " << iDOFs << " degrees of freedom");
	do
	{
		++iIteration;
		TGLOG("Iteration " << iIteration);
		iNumSwitches = 0;
		A.clear();
		fill(x.begin(), x.end(), 0.0);
		fill(b.begin(), b.end(), 0.0);
		// Populate the matrix A with plate elements
		for (itPlate = m_PlateElements.begin(), iPlateCount=0; itPlate != m_PlateElements.end(); ++itPlate, ++iPlateCount)
		{
			int iNodes[3];
			iNodes[0] = itPlate->iNode1;
			iNodes[1] = itPlate->iNode2;
			iNodes[2] = itPlate->iNode3;
			// Fill in bending element
			itPlate->BendingElement.GetKeMatrix(KeB);
			for (i=0; i<10; ++i)
			{
				iGlob = iNodes[i/3]*3+i%3;
				if (i/3==3)
					iGlob = iNumNodes*3+iPlateCount;
				assert(iGlob < iDOFs);
				for (j=0; j<10; ++j)
				{
					jGlob = iNodes[j/3]*3+j%3;
					if (j/3==3)
						jGlob = iNumNodes*3+iPlateCount;
					assert(jGlob < iDOFs);
					A[make_pair(iGlob, jGlob)] += KeB(i, j);
				}
			}
			// Fill in tension element
			itPlate->TensionElement.GetKeMatrix(KeT);
			for (i=0; i<3; ++i)
			{
				iGlob = iNodes[i]*3;
				assert(iGlob < iDOFs);
				for (j=0; j<3; ++j)
				{
					jGlob = iNodes[j]*3;
					assert(jGlob < iDOFs);
					A[make_pair(iGlob, jGlob)] += KeT(i, j);
				}
			}
		}
		// Adjust the b matrix to get rid of initial strains
		// This can be done by doing b = -A*x0 where x0 is the matrix of initial 
		// node positions
		for (i=0; i<iNumNodes; ++i)
		{
			x[i*3] = m_Nodes[i].Position.z;
		}
		// This screws things up big time, not sure why...
/*		for (i=0; i<iNumPlates; ++i)
		{
			x[iNumNodes+i] = (m_Nodes[m_PlateElements[i].iNode1].Position.z + 
							  m_Nodes[m_PlateElements[i].iNode2].Position.z + 
							  m_Nodes[m_PlateElements[i].iNode3].Position.z) / 3.0;
		}*/
		for (itA = A.begin(); itA != A.end(); ++itA)
		{
			b[itA->first.first] -= itA->second * x[itA->first.second];
		}

		// Populate the matrix A with contact springs
		for (itSpring = m_Springs.begin(); itSpring != m_Springs.end(); ++itSpring)
		{
			i1 = itSpring->iNode1;
			i2 = itSpring->iNode2;
			NODE &Node1 = m_Nodes[i1];
			NODE &Node2 = m_Nodes[i2];
			dInitialLength = Node2.Position.z - Node1.Position.z;
			if (m_bAdjustThickness)
				dContactDistance = 0.5*(Node1.GetAdjustedThickness()+Node2.GetAdjustedThickness());
			else
				dContactDistance = 0.5*(Node1.dThickness+Node2.dThickness);
			dCurrentLength = (Node2.Position.z+Node2.dDisplacement)-(Node1.Position.z+Node1.dDisplacement);
			dSeperationDistance = dCurrentLength-dContactDistance;
			/*if (abs(dSeperationDistance) < 1e-3)
				bEnabled = itSpring->bEnabled;
			else*/ if (dSeperationDistance > 0)
				bEnabled = false;
			else
				bEnabled = true;
			if (bEnabled != itSpring->bEnabled)
			{
				++iNumSwitches;
			}
			itSpring->bEnabled = bEnabled;
			if (!bEnabled)
			{
				dStiffness = m_dDisabledStiffness*itSpring->dArea;
			}
			else
			{
				dStiffness = m_dContactStiffness*itSpring->dArea;
			}
			b[i1*3] += dStiffness*(dInitialLength-dContactDistance);
			b[i2*3] -= dStiffness*(dInitialLength-dContactDistance);
			A[make_pair(i1*3, i1*3)] += dStiffness;
			A[make_pair(i2*3, i2*3)] += dStiffness;
			A[make_pair(i1*3, i2*3)] -= dStiffness;
			A[make_pair(i2*3, i1*3)] -= dStiffness;
		}

		set<int> ConstrainedDOFs;

		// Apply constraints
		vector<pair<int, double> >::iterator itConstraint;
		for (itConstraint=m_DOFConstraints.begin(); itConstraint != m_DOFConstraints.end(); ++itConstraint)
		{
			if (ConstrainedDOFs.count(itConstraint->first))
			{
				TGERROR("Warning: Equation " << itConstraint->first << " is already constrained! Ignoring this boundary condition.");
				continue;
			}
			for (i=0; i<iDOFs; ++i)
			{
				pair<int, int> Index = make_pair(itConstraint->first, i);
				if (i==itConstraint->first)
					A[Index] = 1;
				else
					A.erase(Index);
			}
			b[itConstraint->first] = itConstraint->second;
			ConstrainedDOFs.insert(itConstraint->first);
		}

		// Apply periodic boundary conditions
		vector<pair<int, int> >::iterator itDOFLink;
		for (itDOFLink=m_DOFLinks.begin(); itDOFLink != m_DOFLinks.end(); ++itDOFLink)
//		if (0)
		{
			int iRemovedDOF = itDOFLink->first;
			int iCombinedDOF = itDOFLink->second;
			if (ConstrainedDOFs.count(iRemovedDOF))
			{
				swap(iRemovedDOF, iCombinedDOF);
			}
			if (ConstrainedDOFs.count(iRemovedDOF))
			{
				TGERROR("Warning: Degrees of freedom " << itDOFLink->first << " and " << itDOFLink->second << 
					" are already constrained! Ignoring this boundary condition.");
				continue;
			}
			for (i=0; i<iDOFs; ++i)
			{
				pair<int, int> Index = make_pair(iRemovedDOF, i);
				if (!ConstrainedDOFs.count(iCombinedDOF) && A.count(Index))
				{
					// Combine the stiffnesses from the removed DOF and the remaining one
					A[make_pair(iCombinedDOF, i)] += A[Index];
				}
				if (i==itDOFLink->first)
					A[Index] = 1;
				else if (i==itDOFLink->second)
					A[Index] = -1;
				else
					A.erase(Index);
			}
			// Combine the forces from the removed DOF and the remaining one
			if (!ConstrainedDOFs.count(iCombinedDOF))
				b[iCombinedDOF] += b[iRemovedDOF];
			b[iRemovedDOF] = 0;
			ConstrainedDOFs.insert(iRemovedDOF);
		}

		/*if (m_bDebug && iIteration == 1)
		{
			SaveToVTK("c://Program Files//TexGen//GeometrySolver//Initial");
		}*/

		// Solve this sucker with CSparse
		cs* csT = cs_spalloc(iNumNodes, iNumNodes, A.size(), 1, 1);
		for (itA = A.begin(); itA != A.end(); ++itA)
		{
			cs_entry(csT, itA->first.first, itA->first.second, itA->second);
		}
		cs* csA = cs_compress(csT);
		cs_spfree(csT);
		x = b;
		int iSuccess = cs_lusol(1, csA, &x.front(), 1);
		cs_spfree(csA);

		if (!iSuccess)
		{
			TGERROR("Solve failed");
			return 0;
		}

		for (i=0; i<iNumNodes; ++i)
		{
			m_Nodes[i].dDisplacement = x[i*3];
			m_Nodes[i].dThetaX = x[i*3+1];
			m_Nodes[i].dThetaY = x[i*3+2];
		}
		TGLOG("Number of switches: " << iNumSwitches);
//		break;
		if (m_bDebug)
		{
			stringstream IterFileName;
			IterFileName << "c://Program Files//TexGen//GeometrySolver//iter." << setfill('0') << setw(4) << iIteration;
			SaveToVTK(IterFileName.str());
		}
	} while (iIteration == 1 || iNumSwitches > 0);

	return iIteration;
}

void CGeometrySolver::SaveToVTK(string Filename)
{
	CMesh CopiedMesh = m_SurfaceMesh;
	vector<TiXmlElement> AdditionalData;

	CMeshData<double> Displacements("Displacements", CMeshDataBase::NODE);
	CMeshData<double> ThetaX("ThetaX", CMeshDataBase::NODE);
	CMeshData<double> ThetaY("ThetaY", CMeshDataBase::NODE);
	CMeshData<double> Thicknesses("Thicknesses", CMeshDataBase::NODE);
	CMeshData<double> AdjustedThicknesses("AdjustedThicknesses", CMeshDataBase::NODE);
	CMeshData<unsigned char> YarnIndex("YarnIndex", CMeshDataBase::NODE);

	int i;
	for (i=0; i<(int)m_Nodes.size(); ++i)
	{
		Displacements.m_Data.push_back(m_Nodes[i].dDisplacement);
		ThetaX.m_Data.push_back(m_Nodes[i].dThetaX);
		ThetaY.m_Data.push_back(m_Nodes[i].dThetaY);
		Thicknesses.m_Data.push_back(m_Nodes[i].dThickness);
		AdjustedThicknesses.m_Data.push_back(m_Nodes[i].GetAdjustedThickness());
		YarnIndex.m_Data.push_back(m_Nodes[i].iYarnIndex);
	}

	CMeshData<XYZ> YarnTangent("YarnTangent", CMeshDataBase::ELEMENT);
	CMeshData<int> CellYarnIndex("YarnIndex", CMeshDataBase::ELEMENT);
	vector<PLATE>::iterator itPlate;
	for (itPlate = m_PlateElements.begin(); itPlate != m_PlateElements.end(); ++itPlate)
	{
		YarnTangent.m_Data.push_back(itPlate->BendingElement.GetFibreDirection());
		CellYarnIndex.m_Data.push_back(itPlate->iYarnIndex);
	}

	vector<SPRING>::iterator itSpring;
	for (itSpring = m_Springs.begin(); itSpring != m_Springs.end(); ++itSpring)
	{
		if (itSpring->bEnabled)
		{
			CopiedMesh.GetIndices(CMesh::LINE).push_back(itSpring->iNode1);
			CopiedMesh.GetIndices(CMesh::LINE).push_back(itSpring->iNode2);
		}
		YarnTangent.m_Data.push_back(XYZ());
		CellYarnIndex.m_Data.push_back(-1);
	}

	vector<CMeshDataBase*> MeshData;

	MeshData.push_back(&Displacements);
	MeshData.push_back(&ThetaX);
	MeshData.push_back(&ThetaY);
	MeshData.push_back(&Thicknesses);
	if (m_bAdjustThickness)
		MeshData.push_back(&AdjustedThicknesses);
	MeshData.push_back(&YarnIndex);
	MeshData.push_back(&YarnTangent);
	MeshData.push_back(&CellYarnIndex);

	CopiedMesh.SaveToVTK(Filename, &MeshData);
}

void CGeometrySolver::DeformTextile()
{
	if (m_pTextile)
		CTextileDeformer::DeformTextile(*m_pTextile);
}

CTextile* CGeometrySolver::GetDeformedCopyOfTextile()
{
	if (m_pTextile)
		return CTextileDeformer::GetDeformedCopyOfTextile(*m_pTextile);
	else
		return NULL;
}

double CGeometrySolver::GetDisplacement(XYZ Pos, int iYarn, XYZ &Disp) const
{
	// Find out what the displacement is of the yarn at given position
	const list<int> &Indices = m_SurfaceMesh.GetIndices(CMesh::TRI);
	list<int>::const_iterator itIndex;
	int i1, i2, i3;
	double dMin;
	double dAccuracy = -1;
	bool bFirst = true;
	for (itIndex = Indices.begin(); itIndex != Indices.end(); )
	{
		i1 = *(itIndex++);
		i2 = *(itIndex++);
		i3 = *(itIndex++);
		const NODE &N1 = m_Nodes[i1];
		const NODE &N2 = m_Nodes[i2];
		const NODE &N3 = m_Nodes[i3];
		if (N1.iYarnIndex != iYarn || N2.iYarnIndex != iYarn || N3.iYarnIndex != iYarn)
			continue;
		XYZ Weights = GetBarycentricCoordinates(Convert(N1.Position),
			Convert(N2.Position), Convert(N3.Position), Convert(Pos));
		dMin = min(Weights.x, Weights.y);
		dMin = min(dMin, Weights.z);
		if (bFirst || dMin > dAccuracy)
		{
			// If all barycentric coordinates are non-negative the point lies inside the triangle
			dAccuracy = dMin;
			Disp.z = Weights.x * N1.dDisplacement;
			Disp.z += Weights.y * N2.dDisplacement;
			Disp.z += Weights.z * N3.dDisplacement;
			bFirst = false;
		}
	}
	return dAccuracy;
}

















