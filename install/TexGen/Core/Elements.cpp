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
#include "Elements.h"

using namespace TexGen;

CElement::CElement(const CMatrix &P, int iOrder)
: m_P(P)
, m_iIntegrationOrder(iOrder)
{
}

void CElement::SetNodeCoordinates(const CMatrix &P)
{
	m_P = P;
}

void CElement::SetIntegrationOrder(int iOrder)
{
	m_iIntegrationOrder = iOrder;
}

void CElement::GetKeMatrix(CMatrix& KeMatrix)
{
	vector<INTEGRATION_POINT> IntegrationPoints;
	GetIntegrationPoints(IntegrationPoints);
	vector<INTEGRATION_POINT>::iterator itIP;
	KeMatrix.ZeroMatrix();
	CMatrix B, D, Kei;
	GetDMatrix(D);
	for (itIP = IntegrationPoints.begin(); itIP != IntegrationPoints.end(); ++itIP)
	{
		GetBMatrix(B, itIP->Position);

		// Kei = B.T * D * B
		Kei.EqualsTransposeMultiple(B, D * B);

		KeMatrix += Kei * itIP->dWeight;
	}
}

void CElement::GetBMatrix(CMatrix& BMatrix, CMatrix& Position)
{
	// B = X * A^-1
	CMatrix A, X, AInv;
	GetAMatrix(A);
	GetXMatrix(X, Position);
	A.GetInverse(AInv);
	BMatrix.EqualsMultiple(X, AInv);
}

void CElementTri::GetIntegrationPoints(vector<INTEGRATION_POINT> &IntegrationPoints)
{
	IntegrationPoints.clear();
	const int iOrder = m_iIntegrationOrder;
	INTEGRATION_POINT IP;
	IP.Position.Initialise(1, 3);
	switch (iOrder)
	{
	case 1:
		IP.Position(0, 0) = IP.Position(0, 1) = IP.Position(0, 2) = 1.0/3.0;
		IP.dWeight = 1;
		IntegrationPoints.push_back(IP);
		break;
	case 2:
		IP.dWeight = 1.0/3.0;
		IP.Position(0, 1) = IP.Position(0, 2) = 0.5;
		IP.Position(0, 0) = 0;
		IntegrationPoints.push_back(IP);
		IP.Position(0, 0) = IP.Position(0, 2) = 0.5;
		IP.Position(0, 1) = 0;
		IntegrationPoints.push_back(IP);
		IP.Position(0, 0) = IP.Position(0, 1) = 0.5;
		IP.Position(0, 2) = 0;
		IntegrationPoints.push_back(IP);
		break;
	case 3:
		IP.dWeight = -27.0/48.0;
		IP.Position(0, 0) = IP.Position(0, 1) = IP.Position(0, 2) = 1.0/3.0;
		IntegrationPoints.push_back(IP);

		IP.dWeight = 25.0/48.0;
		IP.Position(0, 1) = IP.Position(0, 2) = 0.2;
		IP.Position(0, 0) = 0.6;
		IntegrationPoints.push_back(IP);
		IP.Position(0, 0) = IP.Position(0, 2) = 0.2;
		IP.Position(0, 1) = 0.6;
		IntegrationPoints.push_back(IP);
		IP.Position(0, 0) = IP.Position(0, 1) = 0.2;
		IP.Position(0, 2) = 0.6;
		IntegrationPoints.push_back(IP);
		break;
	case 5:
		IP.dWeight = 0.255;
		IP.Position(0, 0) = IP.Position(0, 1) = IP.Position(0, 2) = 1.0/3.0;
		IntegrationPoints.push_back(IP);

		IP.dWeight = 0.13239415;
		IP.Position(0, 1) = IP.Position(0, 2) = 0.47014206;
		IP.Position(0, 0) = 0.05961587;
		IntegrationPoints.push_back(IP);
		IP.Position(0, 0) = IP.Position(0, 2) = 0.47014206;
		IP.Position(0, 1) = 0.05961587;
		IntegrationPoints.push_back(IP);
		IP.Position(0, 0) = IP.Position(0, 1) = 0.47014206;
		IP.Position(0, 2) = 0.05961587;
		IntegrationPoints.push_back(IP);

		IP.dWeight = 0.12593918;
		IP.Position(0, 1) = IP.Position(0, 2) = 0.10128651;
		IP.Position(0, 0) = 0.79742699;
		IntegrationPoints.push_back(IP);
		IP.Position(0, 0) = IP.Position(0, 2) = 0.10128651;
		IP.Position(0, 1) = 0.79742699;
		IntegrationPoints.push_back(IP);
		IP.Position(0, 0) = IP.Position(0, 1) = 0.10128651;
		IP.Position(0, 2) = 0.79742699;
		IntegrationPoints.push_back(IP);
		break;
	}
	vector<INTEGRATION_POINT>::iterator itIP;
	for (itIP = IntegrationPoints.begin(); itIP != IntegrationPoints.end(); ++itIP)
	{
		itIP->Position = itIP->Position * m_P;
	}
}

CElementTri::CElementTri(const CMatrix &P, int iOrder)
: CElement(P, iOrder)
{
}

double CElementTri::GetArea()
{
	CMatrix A(3, 3);
	int i, j;
	for (i=0; i<3; ++i)
	{
		for (j=0; j<3; ++j)
		{
			if (i!=2)
				A(i, j) = m_P(j, i);
			else
				A(i, j) = 1;
		}
	}
	return 0.5*abs(A.GetDeterminant());
}

//////////////////////////
/// CElementTriBending ///
//////////////////////////

CElementTriBending::CElementTriBending(const CMatrix &P, int iOrder)
: CElementTri(P, iOrder)
, m_E1(1.0)
, m_E2(1.0)
{
}

void CElementTriBending::GetAMatrix(CMatrix& AMatrix)
{
	vector<CMatrix> Nodes;
	CMatrix Node(1, 2);
	CMatrix Center(1, 2);
	int i, j;
	for (i=0; i<3; ++i)
	{
		Node(0, 0) = m_P(i, 0);
		Node(0, 1) = m_P(i, 1);
		Center += Node;
		Nodes.push_back(Node);
	}
	Center /= 3;

	AMatrix.Initialise(10, 10);
	vector<CMatrix> Rows;
	Rows.push_back(GetvMatrix(Nodes[0]));
	Rows.push_back(GetTheta_xMatrix(Nodes[0]));
	Rows.push_back(GetTheta_yMatrix(Nodes[0]));
	Rows.push_back(GetvMatrix(Nodes[1]));
	Rows.push_back(GetTheta_xMatrix(Nodes[1]));
	Rows.push_back(GetTheta_yMatrix(Nodes[1]));
	Rows.push_back(GetvMatrix(Nodes[2]));
	Rows.push_back(GetTheta_xMatrix(Nodes[2]));
	Rows.push_back(GetTheta_yMatrix(Nodes[2]));
	Rows.push_back(GetvMatrix(Center));
	for (i=0; i<10; ++i)
	{
		for (j=0; j<10; ++j)
		{
			AMatrix(i, j) = Rows[i](0, j);
		}
	}
}

void CElementTriBending::GetXMatrix(CMatrix& XMatrix, CMatrix& Position)
{
	XMatrix.Initialise(3, 10);
	CMatrix R1 = GetEpsilon_xMatrix(Position);
	CMatrix R2 = GetEpsilon_yMatrix(Position);
	CMatrix R3 = GetEpsilon_xyMatrix(Position);
	int j;
	for (j=0; j<10; ++j)
	{
		XMatrix(0, j) = R1(0, j);
		XMatrix(1, j) = R2(0, j);
		XMatrix(2, j) = 2*R3(0, j);		// Using engineering shear strain here, make sure D matrix also uses this
	}
}

void CElementTriBending::GetDMatrix(CMatrix& DMatrix)
{
/*
	// This is the isotropic case!
	const double v = 0.2;
	const double E = m_E;
	const double t = 1;
	DMatrix.Initialise(3, 3);
	DMatrix(0, 0) = 1;
	DMatrix(1, 1) = 1;
	DMatrix(1, 0) = v;
	DMatrix(0, 1) = v;
	DMatrix(2, 2) = 0.5*(1-v);	// Using engineering shear strain here, make sure X matrix also uses this
	DMatrix *= (E*t*t*t)/(12*(1-v*v));*/
	const double v12 = 0.0, v21 = 0.0;
	const double E1 = m_E1, E2 = m_E2;
	const double G12 = 0.5*(E1+E2)/(2*(1+0.5*(v12+v21)));
	const double t = 1;
	DMatrix.Initialise(3, 3);
	DMatrix(0, 0) = E1/(1-v12*v21);
	DMatrix(1, 1) = E2/(1-v12*v21);
	DMatrix(1, 0) = v12*E2/(1-v12*v21);
	DMatrix(0, 1) = v12*E2/(1-v12*v21);
	DMatrix(2, 2) = G12;	// Using engineering shear strain here, make sure X matrix also uses this
//	DMatrix *= t*t*t/12;

	// Let's rotate this to the fibre direction
	XY MatOrient(m_FibreDirection.x, m_FibreDirection.y);
	if (MatOrient)
	{
		Normalise(MatOrient);
		// Transformation matrix to rotate to fibre direction
		// From "Mechanics of composite materials" by Robert M. Jones, pg 74
		// section "2.6. Stress-strain relations for a lamina of arbitrary orientation"
		CMatrix T(3, 3);
		T(0, 0) = MatOrient.x*MatOrient.x;
		T(0, 1) = MatOrient.y*MatOrient.y;
		T(0, 2) = -2*MatOrient.x*MatOrient.y;
		T(1, 0) = MatOrient.y*MatOrient.y;
		T(1, 1) = MatOrient.x*MatOrient.x;
		T(1, 2) = 2*MatOrient.x*MatOrient.y;
		T(2, 0) = MatOrient.x*MatOrient.y;
		T(2, 1) = -MatOrient.x*MatOrient.y;
		T(2, 2) = MatOrient.x*MatOrient.x-MatOrient.y*MatOrient.y;
		CMatrix TInv;
		CMatrix RotatedD;
		T.GetInverse(TInv);

		// RotatedD = T^-1 * D * T^-T
		RotatedD.EqualsMultipleTranspose(TInv*DMatrix, TInv);

		DMatrix = RotatedD;
	}
}

CMatrix CElementTriBending::GetvMatrix(CMatrix& Position)
{
	double x = Position(0, 0), y = Position(0, 1);
	CMatrix v(1, 10);
	v(0, 0) = 1;
	v(0, 1) = x;
	v(0, 2) = y;
	v(0, 3) = x*x;
	v(0, 4) = x*y;
	v(0, 5) = y*y;
	v(0, 6) = x*x*x;
	v(0, 7) = x*x*y;
	v(0, 8) = x*y*y;
	v(0, 9) = y*y*y;
	return v;
}

CMatrix CElementTriBending::GetTheta_xMatrix(CMatrix& Position)
{
	double x = Position(0, 0), y = Position(0, 1);
	CMatrix Theta_x(1, 10);
	Theta_x(0, 0) = 0;
	Theta_x(0, 1) = 1;
	Theta_x(0, 2) = 0;
	Theta_x(0, 3) = 2*x;
	Theta_x(0, 4) = y;
	Theta_x(0, 5) = 0;
	Theta_x(0, 6) = 3*x*x;
	Theta_x(0, 7) = 2*x*y;
	Theta_x(0, 8) = y*y;
	Theta_x(0, 9) = 0;
	return Theta_x;
}

CMatrix CElementTriBending::GetTheta_yMatrix(CMatrix& Position)
{
	double x = Position(0, 0), y = Position(0, 1);
	CMatrix Theta_y(1, 10);
	Theta_y(0, 0) = 0;
	Theta_y(0, 1) = 0;
	Theta_y(0, 2) = 1;
	Theta_y(0, 3) = 0;
	Theta_y(0, 4) = x;
	Theta_y(0, 5) = 2*y;
	Theta_y(0, 6) = 0;
	Theta_y(0, 7) = x*x;
	Theta_y(0, 8) = 2*x*y;
	Theta_y(0, 9) = 3*y*y;
	return Theta_y;
}

CMatrix CElementTriBending::GetEpsilon_xMatrix(CMatrix& Position)
{
	double x = Position(0, 0), y = Position(0, 1);
	CMatrix Epsilon_x(1, 10);
	Epsilon_x(0, 0) = 0;
	Epsilon_x(0, 1) = 0;
	Epsilon_x(0, 2) = 0;
	Epsilon_x(0, 3) = 2;
	Epsilon_x(0, 4) = 0;
	Epsilon_x(0, 5) = 0;
	Epsilon_x(0, 6) = 6*x;
	Epsilon_x(0, 7) = 2*y;
	Epsilon_x(0, 8) = 0;
	Epsilon_x(0, 9) = 0;
	return Epsilon_x;
}

CMatrix CElementTriBending::GetEpsilon_yMatrix(CMatrix& Position)
{
	double x = Position(0, 0), y = Position(0, 1);
	CMatrix Epsilon_y(1, 10);
	Epsilon_y(0, 0) = 0;
	Epsilon_y(0, 1) = 0;
	Epsilon_y(0, 2) = 0;
	Epsilon_y(0, 3) = 0;
	Epsilon_y(0, 4) = 0;
	Epsilon_y(0, 5) = 2;
	Epsilon_y(0, 6) = 0;
	Epsilon_y(0, 7) = 0;
	Epsilon_y(0, 8) = 2*x;
	Epsilon_y(0, 9) = 6*y;
	return Epsilon_y;
}

CMatrix CElementTriBending::GetEpsilon_xyMatrix(CMatrix& Position)
{
	double x = Position(0, 0), y = Position(0, 1);
	CMatrix Epsilon_xy(1, 10);
	Epsilon_xy(0, 0) = 0;
	Epsilon_xy(0, 1) = 0;
	Epsilon_xy(0, 2) = 0;
	Epsilon_xy(0, 3) = 0;
	Epsilon_xy(0, 4) = 1;
	Epsilon_xy(0, 5) = 0;
	Epsilon_xy(0, 6) = 0;
	Epsilon_xy(0, 7) = 2*x;
	Epsilon_xy(0, 8) = 2*y;
	Epsilon_xy(0, 9) = 0;
	return Epsilon_xy;
}

//////////////////////////
/// CElementTriTension ///
//////////////////////////

CElementTriTension::CElementTriTension(const CMatrix &P, int iOrder)
: CElementTri(P, iOrder)
, m_E(1.0)
{
}

void CElementTriTension::GetAMatrix(CMatrix& AMatrix)
{
	vector<CMatrix> Nodes;
	CMatrix Node(1, 2);
	int i, j;
	for (i=0; i<3; ++i)
	{
		Node(0, 0) = m_P(i, 0);
		Node(0, 1) = m_P(i, 1);
		Nodes.push_back(Node);
	}

	AMatrix.Initialise(3, 3);
	vector<CMatrix> Rows;
	Rows.push_back(GetvMatrix(Nodes[0]));
	Rows.push_back(GetvMatrix(Nodes[1]));
	Rows.push_back(GetvMatrix(Nodes[2]));
	for (i=0; i<3; ++i)
	{
		for (j=0; j<3; ++j)
		{
			AMatrix(i, j) = Rows[i](0, j);
		}
	}
}

void CElementTriTension::GetXMatrix(CMatrix& XMatrix, CMatrix& Position)
{
	XMatrix.Initialise(2, 3);
	CMatrix R1 = GetEpsilon_xMatrix(Position);
	CMatrix R2 = GetEpsilon_yMatrix(Position);
	int j;
	for (j=0; j<3; ++j)
	{
		XMatrix(0, j) = R1(0, j);
		XMatrix(1, j) = R2(0, j);
	}
}

void CElementTriTension::GetDMatrix(CMatrix& DMatrix)
{
	const double E = m_E;
	DMatrix.Initialise(2, 2);
	DMatrix(0, 0) = m_E;
	DMatrix(1, 1) = 0;

	// Let's rotate this to the fibre direction
	XY MatOrient(m_FibreDirection.x, m_FibreDirection.y);
	if (MatOrient)
	{
		Normalise(MatOrient);
		// Transformation matrix to rotate to fibre direction
		CMatrix T(2, 2);
		T(0, 0) = MatOrient.x;
		T(0, 1) = MatOrient.y;
		T(1, 0) = -MatOrient.y;
		T(1, 1) = MatOrient.x;
		CMatrix TInv;
		CMatrix RotatedD;
		T.GetInverse(TInv);

		// RotatedD = T^-1 * D * T
		RotatedD.EqualsMultiple(TInv, DMatrix * T);

		DMatrix = RotatedD;
	}
}

CMatrix CElementTriTension::GetvMatrix(CMatrix& Position)
{
	double x = Position(0, 0), y = Position(0, 1);
	CMatrix v(1, 3);
	v(0, 0) = 1;
	v(0, 1) = x;
	v(0, 2) = y;
	return v;
}

CMatrix CElementTriTension::GetEpsilon_xMatrix(CMatrix& Position)
{
	double x = Position(0, 0), y = Position(0, 1);
	CMatrix v(1, 3);
	v(0, 0) = 0;
	v(0, 1) = 1;
	v(0, 2) = 0;
	return v;
}

CMatrix CElementTriTension::GetEpsilon_yMatrix(CMatrix& Position)
{
	double x = Position(0, 0), y = Position(0, 1);
	CMatrix v(1, 3);
	v(0, 0) = 0;
	v(0, 1) = 0;
	v(0, 2) = 1;
	return v;
}

















