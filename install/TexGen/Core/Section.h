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
#include "SectionMeshTriangulate.h"
#include "SectionMeshRectangular.h"
#include "SectionMeshRectangleSection.h"

namespace TexGen
{ 
	using namespace std;

	/// Abstract base class respresenting a yarn cross-section
	class CLASS_DECLSPEC CSection
	{
	friend class CSectionHybrid;
	friend class CSectionRotated;
	friend class CSectionScaled;
	public:
		CSection();
		CSection(TiXmlElement &Element);
		virtual ~CSection(void);

		/// Overloaded equality operator to determine if two sections are the same
		virtual bool operator == (const CSection &CompareMe) const = 0;
		
		/// Create a copy of the derived section and return a pointer to the newly created instance
		virtual CSection* Copy() const = 0;

		/// Create a section from TiXmlElement
		static CObjectContainer<CSection> CreateSection(TiXmlElement &Element);

		/// Used for saving data to XML
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		/// Get a section with given number of points on the perimeter
		/**
		The section is created and saved to m_EdgePoints which is then returned.
		If this function is called multiple times with the same	parameter the section
		need not be recreated. Note that the returned vector is only valid until
		the next call to this function, a copy of the vector should be made if it
		needs to be kept for any period of time.
		\param iNumPoints Number of points the section is made up of
		\param bEquiSpaced If set to true, the code will attempt to space the nodes at equal distances apart
		*/
		virtual const vector<XY> &GetPoints(int iNumPoints, bool bEquiSpaced = false) const;

		/// Get a mesh with given number of points on the perimeter, a mesh must be assigned
		/// to the section before this function is called.
		const CMesh &GetMesh(int iNumPoints, bool bEquiSpaced = true) const;

		/// Get the area of a section
		static double GetArea(const vector<XY> &Section);

		/// Get the circumference of a section
		static double GetCircumference(const vector<XY> &Section);

	//	double GetCircumference();

		/// Assign a mesh to the section
		void AssignSectionMesh(const CSectionMesh &SectionMesh);

		/// Get the default name to assign to a section
		virtual string GetDefaultName() const = 0;

		/// Derived class should return the class name
		virtual string GetType() const = 0;

		virtual string GetBaseType() const { return "";}

		/// Specify number of layers for associated section mesh
		void SetSectionMeshLayers( int iNum );

		/// Get the number of layers for associated section mesh
		int GetSectionMeshLayers();

		/// Get a point lying on the perimeter correspending to parametric value t
		/**
		Each section should be defined as a parametric function where t varies from 0 to 1
		the function should start from the positive x axis at t=0 and go counter-clockwise
		through 360 degrees returning to its original position at t=1. In order for the
		CreateEquiSpacedSection function to work correctly, the parametric equation's
		first derivative with respect to t should be continuous.
		*/
		virtual XY GetPoint(double t) const = 0;
	protected:
		

		/// Create a section with given number of points on the perimeter
		/**
		m_EdgePoints will be populated with given number of points
		\param iNumPoints Number of points the section is made up of
		*/
		void CreateSection(int iNumPoints) const;
		/// Same as CreateSection except all the points will be spaced the same distance apart
		/**
		This should work fine as long as the first derivative of the parametric equation with
		respect to t is continuous and the number of points making up the section is not too small
		*/
		bool CreateEquiSpacedSection(int iNumPoints) const;

		/// List of 2d points creating the outline of the cross-section
		mutable vector<XY> m_EdgePoints;

		/// Keep this variable to determine whether the section was created with equidistant points or not
		mutable bool m_bEquiSpaced;

		/// Pointer to a derived class of SectionMesh, this class is in charge of creating the section mesh
		CObjectContainer<CSectionMesh> m_pSectionMesh;

	private:
		void AssignDefaults();
	};

};	// namespace TexGen

