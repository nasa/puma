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
#include "InterpolationCubic.h"
#include "Matrix.h"
using namespace TexGen;

CInterpolationCubic::CInterpolationCubic(bool bPeriodic, bool bForceInPlaneTangent, bool bForceMasterNodeTangent)
: CInterpolation(bPeriodic, bForceInPlaneTangent, bForceMasterNodeTangent)
{
}

CInterpolationCubic::~CInterpolationCubic(void)
{
}

CInterpolationCubic::CInterpolationCubic(TiXmlElement &Element)
: CInterpolation(Element)
{
	// TODO: IMPLEMENT ME
}

/*
void CInterpolationCubic::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
}*/

CSlaveNode CInterpolationCubic::GetNode(const vector<CNode> &MasterNodes, int iIndex, double t) const
{
	assert(iIndex >= 0 && iIndex < int(MasterNodes.size()-1));
	// Errors may occur here if the Initialise function was not called before
	// calling this function. For performance reasons it is necessary to call
	// initialise once at the start before making calls to this function
	assert(m_XCubics.size() == MasterNodes.size()-1);
	assert(m_YCubics.size() == MasterNodes.size()-1);
	assert(m_ZCubics.size() == MasterNodes.size()-1);

	XYZ Pos, Tangent;

	Pos.x = m_XCubics[iIndex].Evaluate(t);
	Pos.y = m_YCubics[iIndex].Evaluate(t);
	Pos.z = m_ZCubics[iIndex].Evaluate(t);

	Tangent.x = m_XCubics[iIndex].EvaluateDerivative(t);
	Tangent.y = m_YCubics[iIndex].EvaluateDerivative(t);
	Tangent.z = m_ZCubics[iIndex].EvaluateDerivative(t);

	if (m_bForceInPlaneTangent)
		Tangent.z = 0;

	Normalise(Tangent);

	CSlaveNode NewNode(Pos, Tangent);

	InterpolateUp(MasterNodes[iIndex], MasterNodes[iIndex+1], NewNode, t);
	InterpolateAngle(MasterNodes[iIndex], MasterNodes[iIndex+1], NewNode, t);

	NewNode.SetIndex(iIndex);
	NewNode.SetT(t);

	return NewNode;
}

void CInterpolationCubic::Initialise(const vector<CNode> &MasterNodes) const
{
	vector<double> X;
	vector<double> Y;
	vector<double> Z;
	vector<CNode>::const_iterator itNode;
	for (itNode = MasterNodes.begin(); itNode != MasterNodes.end(); ++itNode)
	{
		X.push_back(itNode->GetPosition().x);
		Y.push_back(itNode->GetPosition().y);
		Z.push_back(itNode->GetPosition().z);
	}
	if (m_bPeriodic)
	{
		GetPeriodicCubicSplines(X, m_XCubics);
		GetPeriodicCubicSplines(Y, m_YCubics);
		GetPeriodicCubicSplines(Z, m_ZCubics);
	}
	else
	{
		GetNaturalCubicSplines(X, m_XCubics);
		GetNaturalCubicSplines(Y, m_YCubics);
		GetNaturalCubicSplines(Z, m_ZCubics);
	}
}

void CInterpolationCubic::GetPeriodicCubicSplines(const vector<double> &Knots, vector<CUBICEQUATION> &Cubics)
{
/*
	We solve the equation
	[4 1      1] [ D[0] ]   [ 3((x[1] - x[0])+(x[n]-x[n-1])) ]
	|1 4 1     | | D[1] |   |         3(x[2] - x[0])         |
	|  1 4 1   | |  .   | = |               .                |
	|    ..... | |  .   |   |               .                |
	|     1 4 1| |  .   |   |       3(x[n-1] - x[n-3])       |
	[1      1 4] [D[n-1]]   [        3(x[n] - x[n-2])        ]

	Where x is the knots and D is the derivatives at the knots
	Adapted from: http://www.cse.unsw.edu.au/~lambert/splines/NatCubicClosed.java

	Also see http://mathworld.wolfram.com/CubicSpline.html
*/
	Cubics.clear();

	int n = int(Knots.size()-1);

	if (n <= 0)
		return;

	CMatrix Left(n, n);
	CMatrix Right(n, 1);
	CMatrix D;

	int i;

	// Populate Left Matrix as shown above
	for (i=0; i<n; ++i)
	{
		Left(i, i) = 4;
		if (i < n-1)
		{
			Left(i+1, i) = 1;
			Left(i, i+1) = 1;
		}
	}
	Left(n-1, 0) += 1;
	Left(0, n-1) += 1;

	// Populate the Right Matrix as shown above
	Right(0, 0) = 3*((Knots[1] - Knots[0])+(Knots[n] - Knots[n-1]));
	for (i=1; i<n; ++i)
	{
		Right(i, 0) = 3*(Knots[i+1] - Knots[i-1]);
	}

	// Get the Inverse of the left matrix
	CMatrix LeftInverse;
	Left.GetInverse(LeftInverse);
	D.EqualsMultiple(LeftInverse, Right);

	// D now contains the derivatives at the knots

	double a, b, c, d;

	// Calculate the coefficients of the cubics
	for (i=0; i<n-1; ++i)
	{
		a = Knots[i];
		b = D(i, 0);
		c = 3*(Knots[i+1] - Knots[i]) - 2*D(i, 0) - D(i+1, 0);
		d = 2*(Knots[i] - Knots[i+1]) + D(i, 0) + D(i+1, 0);

		Cubics.push_back(CUBICEQUATION(a, b, c, d));
	}
	a = Knots[n-1];
	b = D(n-1, 0);
	c = 3*(Knots[n] - Knots[n-1]) - 2*D(n-1, 0) - D(0, 0);
	d = 2*(Knots[n-1] - Knots[n]) + D(n-1, 0) + D(0, 0);

	Cubics.push_back(CUBICEQUATION(a, b, c, d));
}

void CInterpolationCubic::GetNaturalCubicSplines(const vector<double> &Knots, vector<CUBICEQUATION> &Cubics)
{
/*  
	We solve the equation
	[2 1       ] [ D[0] ]   [  3(x[1] - x[0])  ]
	|1 4 1     | | D[1] |   |  3(x[2] - x[0])  |
	|  1 4 1   | |  .   | = |        .         |
	|    ..... | |  .   |   |        .         |
	|     1 4 1| |  .   |   |3(x[n-1] - x[n-3])|
	[       1 2] [D[n-1]]   [3(x[n-1] - x[n-2])]

	Where x is the knots and D is the derivatives at the knots
	Adapted from: http://www.cse.unsw.edu.au/~lambert/splines/NatCubic.java

	Also see http://mathworld.wolfram.com/CubicSpline.html
*/
	Cubics.clear();

	int n = int(Knots.size());

	if (n <= 0)
		return;

	CMatrix Left(n, n);
	CMatrix Right(n, 1);
	CMatrix D;

	int i;

	// Populate Left Matrix as shown above
	for (i=0; i<n-1; ++i)
	{
		Left(i, i) = 4;
		Left(i+1, i) = 1;
		Left(i, i+1) = 1;
	}
	Left(0, 0) = 2;
	Left(n-1, n-1) = 2;

	// Populate the Right Matrix as shown above
	Right(0, 0) = 3*((Knots[1] - Knots[0]));
	for (i=1; i<n-1; ++i)
	{
		Right(i, 0) = 3*(Knots[i+1] - Knots[i-1]);
	}
	Right(n-1, 0) = 3*(Knots[n-1] - Knots[n-2]);

	// Get the Inverse of the left matrix
	CMatrix LeftInverse;
	Left.GetInverse(LeftInverse);
	D.EqualsMultiple(LeftInverse, Right);

	// D now contains the derivatives at the knots

	double a, b, c, d;

	// Calculate the coefficients of the cubics
	for (i=0; i<n-1; ++i)
	{
		a = Knots[i];
		b = D(i, 0);
		c = 3*(Knots[i+1] - Knots[i]) - 2*D(i, 0) - D(i+1, 0);
		d = 2*(Knots[i] - Knots[i+1]) + D(i, 0) + D(i+1, 0);

		Cubics.push_back(CUBICEQUATION(a, b, c, d));
	}
}






