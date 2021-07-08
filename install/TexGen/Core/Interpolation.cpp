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
#include "Interpolation.h"
#include "InterpolationBezier.h"
#include "InterpolationCubic.h"
#include "InterpolationAdjusted.h"
#include "InterpolationLinear.h"

using namespace TexGen;

CInterpolation::CInterpolation(bool bPeriodic, bool bForceInPlaneTangent, bool bForceMasterNodeTangent)
: m_bPeriodic(bPeriodic)
, m_bForceInPlaneTangent(bForceInPlaneTangent)
, m_bForceMasterNodeTangent(bForceMasterNodeTangent)
{
}

CInterpolation::~CInterpolation(void)
{
}

CInterpolation::CInterpolation(TiXmlElement &Element)
: m_bPeriodic(true)
, m_bForceInPlaneTangent(false)
, m_bForceMasterNodeTangent(false)
{
	const string* pPeriodic = Element.Attribute(string("Periodic"));
	if (pPeriodic)
		m_bPeriodic = valueify<bool>(*pPeriodic);
	const string* pForceInPlaneTangent = Element.Attribute(string("ForceInPlaneTangent"));
	if (pForceInPlaneTangent)
		m_bForceInPlaneTangent = valueify<bool>(*pForceInPlaneTangent);
	const string* pForceMasterNodeTangent = Element.Attribute(string("ForceMasterNodeTangent"));
	if (pForceMasterNodeTangent)
		m_bForceMasterNodeTangent = valueify<bool>(*pForceMasterNodeTangent);
}

void CInterpolation::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	Element.SetAttribute("Periodic", m_bPeriodic);
	Element.SetAttribute("ForceInPlaneTangent", m_bForceInPlaneTangent);
	Element.SetAttribute("ForceMasterNodeTangent", m_bForceMasterNodeTangent);
	Element.SetAttribute("type", GetType());
}

CObjectContainer<CInterpolation> CInterpolation::CreateInterpolation(TiXmlElement &Element)
{
	const string* pType = Element.Attribute(string("type"));
	if (pType)
	{
		if (*pType == "CInterpolationBezier")
			return CInterpolationBezier(Element);
		else if (*pType == "CInterpolationCubic")
			return CInterpolationCubic(Element);
		else if (*pType == "CInterpolationAdjusted")
			return CInterpolationAdjusted(Element);
		else if (*pType == "CInterpolationLinear")
			return CInterpolationLinear(Element);
	}
	// If the interpolation was not recognised for some reason, revert to using bezier
	// interpolation to avoid further problems
	return CInterpolationBezier();
}

void CInterpolation::InterpolateUp(const CNode &Node1, const CNode &Node2, CSlaveNode &SlaveNode, double t)
{
	XYZ U1, U2, SlaveUp;
	U1 = Node1.GetUp();
	U2 = Node2.GetUp();
	if (!U1)
		U1.z = 1;
	if (!U2)
		U2.z = 1;
	SlaveUp = U1 + (U2-U1) * t;
	Normalise(SlaveUp);
	SlaveNode.SetUp(SlaveUp);
	SlaveNode.ProjectUp();
}

void CInterpolation::InterpolateAngle(const CNode &Node1, const CNode &Node2, CSlaveNode &SlaveNode, double t)
{
	double A1, A2, SlaveA;
	A1 = Node1.GetAngle();
	A2 = Node2.GetAngle();
	if ( A1 == 0.0 && A2 == 0.0 )
		return;
	
	SlaveA = A1 + (A2-A1) * t;
	SlaveNode.SetAngle(SlaveA);
}


/*
double CInterpolation::GetLength(vector<CNode> &MasterNodes)
{
	int i, j;
	const int iDivisions = 1000;
	double t, dLength = 0;
	CSlaveNode CurNode, PrevNode;
	PrevNode = GetNode(MasterNodes, 0, 0);
	for (i=0; i<(int)MasterNodes.size()-1; ++i)
	{
		for (j=0; j<iDivisions; ++j)
		{
			t = (double)(j+1)/(double)(iDivisions);
			CurNode = GetNode(MasterNodes, i, t);
			dLength += ::GetLength(PrevNode.GetPosition(), CurNode.GetPosition());
			PrevNode = CurNode;
		}
	}
	return dLength;
}
*/
CSlaveNode CInterpolation::GetNode(const vector<CNode> &MasterNodes, double t) const
{
	int iNumSections = (int)MasterNodes.size()-1;
	t *= iNumSections;
	int iIndex = int(t);
	t -= iIndex;
	if (iIndex >= iNumSections)
	{
		iIndex = iNumSections-1;
		t = 1;
	}
	return GetNode(MasterNodes, iIndex, t);
}

vector<CSlaveNode> CInterpolation::GetSlaveNodes(const vector<CNode> &MasterNodes, int iNumPoints, bool bEquiSpaced) const
{
	vector<CSlaveNode> SlaveNodes;
	Initialise(MasterNodes);
	if (!bEquiSpaced || !CreateEquiSpacedSlaveNodes(SlaveNodes, MasterNodes, iNumPoints))
		CreateSlaveNodes(SlaveNodes, MasterNodes, iNumPoints);
	return SlaveNodes;
}

void CInterpolation::CreateSlaveNodes(vector<CSlaveNode> &SlaveNodes, const vector<CNode> &MasterNodes, int iNumPoints) const
{
	double t;
	int i;
	SlaveNodes.clear();
	for (i=0; i<iNumPoints; ++i)
	{
		t = double(i)/double(iNumPoints-1);
		SlaveNodes.push_back(GetNode(MasterNodes, t));
	}
}

bool CInterpolation::CreateEquiSpacedSlaveNodes(vector<CSlaveNode> &SlaveNodes, const vector<CNode> &MasterNodes, int iNumPoints) const
{
	if (iNumPoints <= 1)
	{
		TGERROR("Unable to create equispaced slave nodes, not enough points: " << iNumPoints);
		assert(false);
		return false;
	}

	// Approximate the total length of the yarn and length of yarn sections
	// from master nodes assuming linear interpolation
	vector<double> SectionLengths;
	int i, j;
	int iNumNodes = (int)MasterNodes.size();
	double dLength;
	for (i=0; i<iNumNodes-1; ++i)
	{
		dLength = GetLength(MasterNodes[i+1].GetPosition() - MasterNodes[i].GetPosition());
		SectionLengths.push_back(dLength);
	}
	double dTotalLength = accumulate(SectionLengths.begin(), SectionLengths.end(), 0.0);

	// For each master node store the T value and L values
	// The T value varies from 0 to 1 across the master nodes at
	// constant intervals, whereas the L value represent the
	// length along the yarn of the master node
	vector<double> LValues;
	vector<double> TValues;
	double t, L;

	double dt = 1/double(iNumNodes-1);
	for (i=0, t=0, L=0; i<iNumNodes; ++i, t+=dt)
	{
		TValues.push_back(t);
		LValues.push_back(L);
		if (i<iNumNodes-1)
			L+=SectionLengths[i];
	}

	// We want the slave nodes to be equispaced with length along the yarn
	// so calculate an L value for each slave node then convert it to the T value
	double dl = dTotalLength/double(iNumPoints-1);
	SlaveNodes.clear();
	SlaveNodes = CalcSlaveNodePositions( MasterNodes, LValues, TValues, dl, iNumNodes, iNumPoints );
	

	// If there is a high curvature in the yarn the initial estimate of length will be too small
	// leading to uneven distances between points when the t values are calculated for the actual curved lines
	// Calculate the new section length by summing the distances between the two master nodes
	// on each end and all the slave nodes in between. 
	SectionLengths.clear();
	SectionLengths.resize(iNumNodes-1);
	XYZ PrevPos;
	
	for (i=0; i<iNumNodes-1; ++i)
	{
		SectionLengths[i] = 0;
		PrevPos = MasterNodes[i].GetPosition();
		for (j=0; j<int(SlaveNodes.size()); ++j)
		{
			if (SlaveNodes[j].GetIndex() == i)
			{
				SectionLengths[i] += GetLength(PrevPos, SlaveNodes[j].GetPosition());
				PrevPos = SlaveNodes[j].GetPosition();
			}
		}
		SectionLengths[i] += GetLength(PrevPos, MasterNodes[i+1].GetPosition());
	}
	double dNewTotalLength = accumulate(SectionLengths.begin(), SectionLengths.end(), 0.0);

	// If more than 5% difference between original length estimate and length using slave nodes, recalculate t values
	if ( fabs( (dNewTotalLength - dTotalLength)/dTotalLength ) > 0.05 )  
	{
		LValues.clear();
		TValues.clear();

		for (i=0, t=0, L=0; i<iNumNodes; ++i, t+=dt)
		{
			TValues.push_back(t);
			LValues.push_back(L);
			if (i<iNumNodes-1)
				L+=SectionLengths[i];
		}

		// We want the slave nodes to be equispaced with length along the yarn
		// so calculate an L value for each slave node then convert it to the T value
		double dl = dNewTotalLength/double(iNumPoints-1);
		SlaveNodes.clear();
		SlaveNodes = CalcSlaveNodePositions( MasterNodes, LValues, TValues, dl, iNumNodes, iNumPoints );
	}

	return true;
}

void CInterpolation::CalculateNodeCoordinateSystem(const vector<CNode> &MasterNodes, vector<XYZ> &Tangents) const
{
	int i, iNumNodes = (int)MasterNodes.size();
	Tangents.resize(iNumNodes, XYZ());
	if (iNumNodes <= 1)
	{
		// We need at least 2 nodes to calculate tangents
		TGERROR("Unable to calculate node coordinate system, not enough master nodes specified");
		assert(false);
		return;
	}
	XYZ YarnRepeat;
	XYZ PrevPos, NextPos;
	XYZ Tangent, Up;
	YarnRepeat = MasterNodes[iNumNodes-1].GetPosition() - MasterNodes[0].GetPosition();
	for (i=0; i<iNumNodes; ++i)
	{
		// Get the position of the previous node in the list of parent nodes
		// if the previous node is non-existant then use equivalent repeated node position - yarn repeat
		if (i==0)
		{
			if (m_bPeriodic)
				PrevPos = MasterNodes[iNumNodes-2].GetPosition() - YarnRepeat;
			else
				PrevPos = MasterNodes[0].GetPosition();
		}
		else
			PrevPos = MasterNodes[i-1].GetPosition();
		// Get the position of the next node in the list of parent nodes
		// if the next node is non-existent then use the equivalent repeated node position + yarn repeat
		if (i==iNumNodes-1)
		{
			if (m_bPeriodic)
				NextPos = MasterNodes[1].GetPosition() + YarnRepeat;
			else
				NextPos = MasterNodes[iNumNodes-1].GetPosition();
		}
		else
			NextPos = MasterNodes[i+1].GetPosition();

		// The tangent is calculated as the normalisation of the next parent node position - the previous
		Tangent = NextPos - PrevPos;
		assert(::GetLength(Tangent) != 0);
		Normalise(Tangent);

		if (MasterNodes[i].GetTangent())
			Tangents[i] = MasterNodes[i].GetTangent();
		else
			Tangents[i] = Tangent;
		if ( m_bForceMasterNodeTangent )  // Force master node tangents to be in-plane
			Tangents[i].z = 0;
	}
}

vector<CSlaveNode> CInterpolation::CalcSlaveNodePositions( const vector<CNode> &MasterNodes, vector<double> &LValues, vector<double> &TValues, double dL, int iNumNodes, int iNumPoints ) const
{
	
	vector<CSlaveNode> SlaveNodes;
	int j = 0, i;
	double t, u, L;
	for (i=0, L=0.0; i<iNumPoints; ++i, L += dL)
	{
		// Convert L value to t value
		if (L < LValues[0])
		{
			t = 0;
		}
		else if (L > LValues[iNumNodes-1])
		{
			t = 1;
		}
		else
		{
			while ( j < iNumNodes-1 )
			{
				if (LValues[j] <= L && L <= LValues[j+1])
				{
					u = (L-LValues[j])/(LValues[j+1]-LValues[j]);
					t = TValues[j] + u*(TValues[j+1]-TValues[j]);
					break;
				}
				j++;
			}
		}
		CSlaveNode SlaveNode = GetNode(MasterNodes, t);             
		SlaveNodes.push_back(SlaveNode);
	}
	return SlaveNodes;
}





