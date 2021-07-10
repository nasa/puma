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

#include "Mesh.h"
#include "Section.h"

namespace TexGen
{
	using namespace std;

	class CSection;
	class CYarnSectionConstant;
	class CYarnSectionInterpNode;
	class CYarnSectionInterpPosition;

	/// Structure used to represent the position along the length of a yarn
	struct CLASS_DECLSPEC YARN_POSITION_INFORMATION
	{
		double dSectionPosition;	///< This variables varies linearly with distance from 0 to 1 from the start to the end of the current link
		int iSection;	///< This variable represents the index of the current section (where a section is defined as the part between two master nodes)
		vector<double> SectionLengths;	///< This contains a list of lengths representing the length of each section

		double GetYarnPosition() const;
		/// Constructor used to initialise all variables to zero on creation
		YARN_POSITION_INFORMATION()
		{
			dSectionPosition = 0;
			iSection = 0;
		}
	};

	/// Abstract base class used to define the sections along the length of a yarn
	/** The section may vary in different ways, it is an abstract base class which can
	be derived from to implement specific types of section variations. Derived classes must implement
	GetSection function and provide a list of points in 2D that represent the section
	at a given point along the fibre. The position along the fibre is given by the
	YARN_POSITION_INFORMATION structure
	*/
	class CLASS_DECLSPEC CYarnSection
	{
	public:
		CYarnSection(bool bConstMesh = true);
		CYarnSection(TiXmlElement &Element);
		virtual ~CYarnSection(void);
		bool m_bForceEqualMeshLayers; 

		/// This is a function to allow copying of derived classes correctly
		/**
		Derived classes should implement this as CYarnSection* Copy() const { return new CYarnSectionDerived(*this); }
		where CYarnSectionDerived should be replaced by the name of the derived class
		*/
		virtual CYarnSection* Copy() const = 0;

		/// Derived class should return the class name
		virtual string GetType() const = 0;

		/// Create a yarn section from TiXmlElement
		static CObjectContainer<CYarnSection> CreateYarnSection(TiXmlElement &Element);

		/// Used for saving data to XML
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		/// This function must be implemented by derived classes
		/**
		\param PositionInfo Structure which contains information about the position along the yarn
		\param iNumPoints Number of points on the perimeter of the section to return
		\param bEquiSpaced Create the points such that they are all equispaced or not
		*/
		virtual vector<XY> GetSection(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced = false) const = 0;
		/// This function must be implemented by derived classes
		/**
		\param PositionInfo Structure which contains information about the position along the yarn
		\param iNumPoints Number of points on the perimeter of the section to return
		\param bEquiSpaced Create the points such that they are all equispaced or not
		*/
		virtual CMesh GetSectionMesh(const YARN_POSITION_INFORMATION PositionInfo, int iNumPoints, bool bEquiSpaced) const = 0;

		/// Find max value for iNumLayers in section meshes and set iNumLayers to that value
		virtual void SetSectionMeshLayersEqual( int iNumPoints ) const {return;}

		bool GetForceMeshLayers() const {return m_bForceEqualMeshLayers;}

		/// Get a pointer of that the inherited class
		/**
		Internally the function just returns dynamic_cast<CYarnSectionConstant*>(this)  etc
		This is mainly useful in the python interface because type casting is not supported
		in this language.
		*/
		CYarnSectionConstant* GetSectionConstant();
		CYarnSectionInterpNode* GetSectionInterpNode();
		CYarnSectionInterpPosition* GetSectionInterpPosition();

	protected:
	};

};	// namespace TexGen



