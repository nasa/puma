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
#include "SlaveNode.h"

namespace TexGen
{
	using namespace std;

	/// Abstract base class for describing the yarn path interpolations
	/**
	Given the master nodes of a yarn, this class should be able to interpolated
	points along the path of the yarn that contain the tangent and up vector
	*/
	class CLASS_DECLSPEC CInterpolation
	{
	public:
		CInterpolation(bool bPeriodic, bool bForceInPlaneTangent, bool bForceMasterNodeTangent);
		CInterpolation(TiXmlElement &Element);
		virtual ~CInterpolation(void);

		/// This is a function to allow copying of derived classes correctly
		/**
		Derived classes should implement this as CInterpolation* Copy() const { return new CInterpolationDerived(*this); }
		where CInterpolationDerived should be replaced by the name of the derived class
		*/
		virtual CInterpolation* Copy() const = 0;

		/// Derived class should return the class name
		virtual string GetType() const = 0;

		/// Create an interpolation from TiXmlElement
		static CObjectContainer<CInterpolation> CreateInterpolation(TiXmlElement &Element);

		/// Used for saving data to XML
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		/// Get a list of nodes along the centre line of the yarn
		vector<CSlaveNode> GetSlaveNodes(const vector<CNode> &MasterNodes, int iNumPoints, bool bEquiSpaced = true) const;

		/// If the interpolation function needs to calculate some things just once for
		/// the whole yarn it should be done here
		virtual void Initialise(const vector<CNode> &MasterNodes) const {};

		/// Get a node from parametric function. Initialise should be called first.
		/**
		Any function which calls this needs to make sure the yarn has been initialised first
		or trouble will ensue.
		\param MasterNodes The nodes which need to be interpolated
		\param iIndex The section between which the interpolation should be made
		\param t The interpolation parameter which is 0 to 1 from the start of the section to the end of the section
		*/
		virtual CSlaveNode GetNode(const vector<CNode> &MasterNodes, int iIndex, double t) const = 0;

		/// Get a node from parametric function. Initialise should be called first.
		/**
		Any function which calls this needs to make sure the yarn has been initialised first
		or trouble will ensue.
		\param MasterNodes The nodes which need to be interpolated
		\param t The interpolation parameter which is 0 to 1 from the start of the yarn to the end of the yarn
		*/
		CSlaveNode GetNode(const vector<CNode> &MasterNodes, double t) const;

	//	/// Get the length of the centreline by spliting up the interpolation function with straight lines
	//	double GetLength(vector<CNode> &MasterNodes);

		// Accessor methods
		void SetPeriodic(bool bPeriodic) { m_bPeriodic = bPeriodic; }
		void SetForceInPlaneTangent(bool bForceInPlaneTangent) { m_bForceInPlaneTangent = bForceInPlaneTangent; }
		bool GetPeriodic() const { return m_bPeriodic; }
		bool GetForceInPlaneTangent() const { return m_bForceInPlaneTangent; }

	protected:
		/// Create slave nodes with specified number of points between master nodes
		void CreateSlaveNodes(vector<CSlaveNode> &SlaveNodes, const vector<CNode> &MasterNodes, int iNumPoints) const;

		/// Create slave nodes equispaced with total specified number of points
		bool CreateEquiSpacedSlaveNodes(vector<CSlaveNode> &SlaveNodes, const vector<CNode> &MasterNodes, int iNumPoints) const;

		static void InterpolateUp(const CNode &Node1, const CNode &Node2, CSlaveNode &SlaveNode, double t);
		static void InterpolateAngle(const CNode &Node1, const CNode &Node2, CSlaveNode &SlaveNode, double t);

		/// This function will calculate the Tangent and Up vectors automatically to sensible values
		/// without user input for the parent nodes within the current yarn. It may be desirable to give more control to the user.
		void CalculateNodeCoordinateSystem(const vector<CNode> &MasterNodes, vector<XYZ> &Tangents) const;

		/// 
		vector<CSlaveNode> CalcSlaveNodePositions( const vector<CNode> &MasterNodes, vector<double> &LValues, vector<double> &TValues, double dL, int iNumNodes, int iNumPoints ) const;

		bool m_bPeriodic;
		bool m_bForceInPlaneTangent;  // Forces both slave and master nodes to have in-plane tangents
		bool m_bForceMasterNodeTangent;  // Forces master nodes to have in-plane tangents
	};

};	// namespace TexGen
