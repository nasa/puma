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

#include "BasicVolumes.h"
#include "TextileDeformer.h"
#include "Elements.h"

namespace TexGen
{ 
	using namespace std;

	/// Class for creating realistic fabric geometry using FE
	/**
	This class implements a method where the mid-surface of the yarns are represented
	by triangular plate elements with bending stiffness and tension components.
	*/
	class CLASS_DECLSPEC CGeometrySolver : public CBasicVolumes, public CTextileDeformer
	{
	public:
		CGeometrySolver(void);
		~CGeometrySolver(void);

		/// Create the finite element system of equations for a given textile
		bool CreateSystem(CTextile &Textile);
		/// Create the finite element system of equations for a given textile
		bool CreateSystem(string TextileName);
/*		void OutputSystem(string Filename);*/
		/// Solve the system of equations
		/**
		\return The number of iterations taken, a value of 0 indicates a failure
		*/
		int SolveSystem();
		/// Save the results to a VTK file
		void SaveToVTK(string Filename);
		/// Deform the textile with the results
		void DeformTextile();
		/// Create a copy of the textile and deform it, leaving the original textile intact
		CTextile* GetDeformedCopyOfTextile();

		// Get and set the various mechanical properties
		void SetTensileStress(double dTensileStress);
		void SetLongitudinalBendingModulus(double dBendingModulus);
		void SetTransverseBendingModulus(double dBendingModulus);
		void SetContactStiffness(double dContactStiffness);
		void SetDisabledStiffness(double dDisabledStiffness);
		double GetTensileStress() { return m_dTensileStress; }
		double GetLongitudinalBendingModulus() { return m_dLongitudinalBendingModulus; }
		double GetTransverseBendingModulus() { return m_dTransverseBendingModulus; }
		double GetContactStiffness() { return m_dContactStiffness; }
		double GetDisabledStiffness() { return m_dDisabledStiffness; }

	private:
		void CreateDebugSystem();

	protected:
		struct RAISED_NODE
		{
			int iGlobalIndex;
			int iYarnIndex;
			double dHeight;
			double dThickness;
			bool operator < (const RAISED_NODE &right) const
			{
				return dHeight < right.dHeight;
			}
			RAISED_NODE():iGlobalIndex(0), iYarnIndex(0), dHeight(0), dThickness(0) {}
		};

		struct PROJECTED_NODE
		{
			vector<RAISED_NODE> RaisedNodes;
			XYZ Position;
		};

		struct NODE
		{
			XYZ Position;
			double dThickness;
			double dDisplacement;
			double dThetaX;
			double dThetaY;
			int iYarnIndex;
			double GetAdjustedThickness()
			{
				return dThickness*sqrt(1+dThetaX*dThetaX+dThetaY*dThetaY);
			}
			NODE():dThickness(0), dDisplacement(0), iYarnIndex(0), dThetaX(0), dThetaY(0) {}
		};

		struct SPRING
		{
			int iNode1, iNode2;
			double dArea;
			bool bEnabled;
			SPRING():iNode1(0), iNode2(0), dArea(0), bEnabled(false) {}
		};

		struct PLATE
		{
			CElementTriBending BendingElement;
			CElementTriTension TensionElement;
			int iNode1, iNode2, iNode3;
			int iYarnIndex;
			PLATE():iNode1(0), iNode2(0), iNode3(0), iYarnIndex(-1) {}
		};


		void RaiseNodes(int iIndex);
		double GetAverageLength(int iIndex);
		double GetArea(int iIndex);
		void CreateSurfaceMesh();
		void CreatePlateElements();
		void AssignFibreDirectionToElements();
		void ApplyPeriodicBoundaryConditions(vector<XYZ> Repeats);
		double GetDisplacement(XYZ Pos, int iYarn, XYZ &Disp) const;
//		void DeformTextile(CTextile* pTextile);

		vector<SPRING> m_Springs;
		vector<PLATE> m_PlateElements;
		vector<NODE> m_Nodes;
		vector<PROJECTED_NODE> m_ProjectedNodes;
		vector<pair<int, double> > m_DOFConstraints;
		vector<pair<int, int> > m_DOFLinks;
		double m_dHeightTolerance;
		double m_dDisabledStiffness;
		double m_dContactStiffness;
		double m_dLongitudinalBendingModulus;
		double m_dTransverseBendingModulus;
		double m_dTensileStress;
		CMesh m_SurfaceMesh;
		bool m_bAdjustThickness;
	};


};	// namespace TexGen























