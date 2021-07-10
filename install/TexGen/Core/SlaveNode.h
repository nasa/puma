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
#include "Node.h"
#include "Mesh.h"
namespace TexGen
{

	using namespace std;

	/// A derivation of the CNode class which contains data specific to slave nodes such as sections
	class CLASS_DECLSPEC CSlaveNode : public CNode
	{
	public:
		CSlaveNode(XYZ Position = XYZ(), XYZ Tangent = XYZ(), XYZ Up = XYZ());
		CSlaveNode(TiXmlElement &Element);
		~CSlaveNode(void);

		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		/// Populate m_SectionPoints from m_2DSectionPoints, Setting m_2DSectionPoints at the same time
		/**
		This function is used to create the global 3D coordinates of the section points
		from the local 2D coordinates of the section points
		\param p2DSectionPoints Set m_Section to be equal to this, or NULL to leave m_Section unmodified
		*/
		void UpdateSectionPoints(const vector<XY> *p2DSectionPoints = NULL);

		/// Calculate the 3D coordinates of a point on the cross-section
		/**
		This function is used to create the global 3D coordinates of a section point
		from the local 2D coordinates
		\param p2DSectionPoints The point to be converted to the 3D coordinates
		\return The 3D point calculated
		*/
		XYZ GetPointOnSection(const XY &p2DSectionPoints);

		/// Populate m_SectionMesh from m_2DSectionMesh, Setting m_2DSectionMesh at the same time
		/**
		This function is used to create the global 3D coordinates of the section mesh
		from the local 2D coordinates of the section mesh
		\param p2DSectionMesh Set m_SectionMesh to be equal to this, or NULL to leave m_SectionMesh unmodified
		*/
		void UpdateSectionMesh(const CMesh *p2DSectionMesh = NULL);

		/// Rotate the Node by given quaternion
		void Rotate(WXYZ Rotation);

		/// Translate the Node by given vector
		void Translate(XYZ Vector);

		// Accessor functions
		double GetT() const { return m_T; }
		void SetT(double t) { m_T = t; }
		int GetIndex() const { return m_iIndex; }
		void SetIndex(int iIndex) { m_iIndex = iIndex; }
		const vector<XYZ> &GetSectionPoints() const { return m_SectionPoints; }
		const vector<XY> &Get2DSectionPoints() const { return m_2DSectionPoints; }
		const CMesh &Get2DSectionMesh() const { return *m_2DSectionMesh; }
		const CMesh &GetSectionMesh() const { return *m_SectionMesh; }

		inline bool operator < ( const CSlaveNode &right)
		{
			return m_T < right.m_T;
		}

	protected:
		/// Coordinates of the cross-section edge points in 2D
		vector<XY> m_2DSectionPoints;
		/// Coordinates of the cross-section edge points in 3D
		vector<XYZ> m_SectionPoints;
		/// Section mesh in 2D
		CMesh* m_2DSectionMesh;
		/// Section mesh in 3D
		CMesh* m_SectionMesh;
		/// T is the parameter which varies from 0 to 1 which tells us how far along the link the node is.
		double m_T;
		/// Index which determines between which master nodes this slave node lies, varies from 0 to number of nodes - 1
		int m_iIndex;
	};

	

};	// namespace TexGen
