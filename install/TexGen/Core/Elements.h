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

#include "Matrix.h"

namespace TexGen
{
	/// Base class for representing finite elements
	/**
	This class contains all the implementation which is common to finite element analysis
	elements. Not to be confused with elements within a CMesh. The CMesh class is purely
	geometrical, while this class is used for FE analysis.
	*/
	class CLASS_DECLSPEC CElement
	{
	public:
		CElement(const CMatrix &P = CMatrix(), int iOrder = 3);
		virtual ~CElement(void) {}

		void SetNodeCoordinates(const CMatrix &P);
		void SetIntegrationOrder(int iOrder);

		void GetKeMatrix(CMatrix& KeMatrix);

		void SetFibreDirection(XYZ FibreDirection) { m_FibreDirection = FibreDirection; }
		XYZ GetFibreDirection() { return m_FibreDirection; }

	protected:
		/// Struct for representing an integration point
		struct INTEGRATION_POINT
		{
			CMatrix Position;
			double dWeight;
		};

		/// This is the dimension matrix which is composed of the A and X matrices (B=X*A^-1)
		void GetBMatrix(CMatrix& BMatrix, CMatrix& Position);
		/// This is the coordinate matrix given nodal coordinates P
		virtual void GetAMatrix(CMatrix& AMatrix) = 0;
		/// This is the matrix which defines strain in terms of the coefficients
		virtual void GetXMatrix(CMatrix& XMatrix, CMatrix& Position) = 0;
		/// This is the material properties matrix (typically involving E and v)
		virtual void GetDMatrix(CMatrix& DMatrix) = 0;
		/// Defines a series of integration points
		/**
		The number of integration points depends on the element shape and integration order
		*/
		virtual void GetIntegrationPoints(vector<INTEGRATION_POINT> &IntegrationPoints) = 0;

		CMatrix m_P;	// These are the nodal coordinates
		int m_iIntegrationOrder;
		XYZ m_FibreDirection;
	};

	/// Base class for representing triangular elements
	/**
	This class contains all the implementation which is common to triangular finite element
	elements. It contains methods for getting the area of the triangle and getting the
	integration point positions based on the integration order.
	*/
	class CLASS_DECLSPEC CElementTri : public CElement
	{
	public:
		CElementTri(const CMatrix &P, int iOrder);
		double GetArea();

	protected:
		void GetIntegrationPoints(vector<INTEGRATION_POINT> &IntegrationPoints);
	};

	/// Triangular element which represents yarn bending.
	/**
	The bending modulus of the yarn can be set independantly for along the length and
	transverse to the yarn. If this where considered to be a regular plate such as a
	sheet of metal, the bending modulus Eb would be equal to E * t^3/12 where E is the
	young's modulus of the material and t is the thickness of the sheet. However it is
	not sensible to represent a yarn as such, thus a value for Eb should be specified
	directly rather than E and t.
	*/
	class CLASS_DECLSPEC CElementTriBending : public CElementTri
	{
	public:
		CElementTriBending(const CMatrix &P = CMatrix(), int iOrder = 3);

		void SetLongitudinalBendingModulus(double E1) { m_E1 = E1; }
		double GetLongitudinalBendingModulus() { return m_E1; }

		void SetTransverseBendingModulus(double E2) { m_E2 = E2; }
		double GetTransverseBendingModulus() { return m_E2; }

	protected:
		void GetAMatrix(CMatrix& AMatrix);
		void GetXMatrix(CMatrix& XMatrix, CMatrix& Position);
		void GetDMatrix(CMatrix& DMatrix);

		/// This is the matrix of displacement v in terms of coefficients C1, C2 ... C10
		CMatrix GetvMatrix(CMatrix& Position);
		/// This is dv/dx
		CMatrix GetTheta_xMatrix(CMatrix& Position);
		/// This is dv/dy
		CMatrix GetTheta_yMatrix(CMatrix& Position);
		/// This is d^2v/dx^2
		CMatrix GetEpsilon_xMatrix(CMatrix& Position);
		/// This is d^2v/dy^2
		CMatrix GetEpsilon_yMatrix(CMatrix& Position);
		/// This is d^2v/dxdy
		CMatrix GetEpsilon_xyMatrix(CMatrix& Position);

		double m_E1;
		double m_E2;
	};

	/// Triangular element which represents fibre tension.
	/**
	This element considers the forces along the z axis when tension is applied along the
	length of the yarns. It is assumed that the yarn tensile stress remains constant and
	can be set as a parameter. The tensile force applied along the z axis is linearly
	proportional to the slope at that point. If the slope is 0 then the force in the z
	direction is 0 (it will all be in-plane, but this force is not simulated). If the slope
	is non-zero then there will be a z component to the force propertional to the slope.
	*/
	class CLASS_DECLSPEC CElementTriTension : public CElementTri
	{
	public:
		CElementTriTension(const CMatrix &P = CMatrix(), int iOrder = 3);

		void SetTensileStress(double E) { m_E = E; }
		double GetTensileStress() { return m_E; }

	protected:
		void GetAMatrix(CMatrix& AMatrix);
		void GetXMatrix(CMatrix& XMatrix, CMatrix& Position);
		void GetDMatrix(CMatrix& DMatrix);

		/// This is the matrix of displacement v in terms of coefficients C1, C2 and C3
		CMatrix GetvMatrix(CMatrix& Position);
		/// This is dv/dx
		CMatrix GetEpsilon_xMatrix(CMatrix& Position);
		/// This is dv/dy
		CMatrix GetEpsilon_yMatrix(CMatrix& Position);

		double m_E;
	};

};	// namespace TexGen









