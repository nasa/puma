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
#include "YarnSectionInterp.h"

namespace TexGen
{ 
	using namespace std;

	/// Creates a section which is linearly interpolated between sections defined at the nodes.
	/**
	Additional sections can be defined between the nodes using the AddSection function. If sections
	need not be defined at the nodes then CYarnSectionInterpPosition should be used instead.
	*/
	class CLASS_DECLSPEC CYarnSectionInterpNode : public CYarnSectionInterp
	{
	public:
		/**
		\param bRamped, bPolar  Specify how points are interpolated between master nodes using
			CYarnSectionInterp::InterpolatePoints()
			If bRamped is set to true, the interpolation fraction given to functions in GetInterpedSection
			and GetInterpedSectionMesh will be ramped using a cubic equation. Where du = 0 at u = 0, and du = 0
			at u = 1. This provides a smoother transition between sections when the interpolation begins and ends.
		\param bConstMesh Defines whether all section meshes are forced to have same configuration
			ie number of rows and columns in mesh
		*/
		CYarnSectionInterpNode(bool bRamped = true, bool bPolar = false, bool bConstMesh = true);
		CYarnSectionInterpNode(TiXmlElement &Element);
		~CYarnSectionInterpNode(void);

		vector<XY> GetSection(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced = false) const;
		CMesh GetSectionMesh(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced) const;
		CYarnSection* Copy() const { return new CYarnSectionInterpNode(*this); }
		string GetType() const { return "CYarnSectionInterpNode"; }
		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		/// Add a section at a node
		/**
		Nodes should be added sequentially, one for each master node, the number of nodes must
		be equal to the number of nodes on the yarn it is applied to
		*/
		void AddSection(const CSection &Section);

		/// Insert a section at a node
		/**
		If a node is added to the yarn after, it may be necessary to insert a section at a specific
		index. The section will be added at the given index, and all other sections will be pushed back.
		*/
		void InsertSection(int iIndex, const CSection &Section);

		/// Replace a section at a node
		/**
		Replace an existing section at the node. Note that the old one will not be deleted until
		the YarnSectionInterpNode is deleted.
		*/
		void ReplaceSection(int iIndex, const CSection &Section);

		/// Replace a mid node section
		void ReplaceMidSection(int iNodeIndex, int iIndex, const CSection &Section);

		/// Delete a section at a node
		void DeleteSection(int iIndex);

		/// Insert a section in between two nodes
		/**
		The sections must be added at the nodes before midnode sections are added
		\param iIndex The index of the segment between two nodes
		\param u The position between the two nodes where to add the section 0 < u < 1
		\param Section The section to add at this point
		*/
		void InsertSection(int iIndex, double u, const CSection &Section);

		/// Function to set the number of layers equal for all sections in a given yarn
		void SetSectionMeshLayersEqual( int iNumPoints ) const;

		// Accessor methods
		int GetNumNodeSections() const { return (int)m_Sections.size(); }
		const CSection &GetNodeSection(int iIndex) const;
		int GetNumMidNodeSections(int iIndex) const;
		double GetMidNodeSectionPos( int iNodeIndex, int iIndex ) const;

	protected:
		void GetSectionsToInterp(const YARN_POSITION_INFORMATION PositionInfo, const CSection* &pSection1, const CSection* &pSection2, double &u) const;

		vector<CObjectContainer<CSection> > m_Sections;
		vector<vector<pair<double, CObjectContainer<CSection> > > > m_MidNodeSections;

	};


};	// namespace TexGen
