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
#include "LinearTransformation.h"

namespace TexGen
{ 
	using namespace std;

	class CYarn;

	/// Abstract base class representing the domain in which a textile cell may lie
	class CLASS_DECLSPEC CDomain
	{
	public:
		CDomain(void);
		CDomain(TiXmlElement &Element);
		virtual ~CDomain(void);

		virtual CDomain* Copy() const = 0;

		/// Used for saving data to XML
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType = OUTPUT_STANDARD) const;

		/// Get the limits associated with each repeat vector, that is the upper and lower bounds multiplied to
		/// the repeat vector for the entire yarn to lie just outside out of the domain
		virtual vector<pair<int, int> > GetRepeatLimits(const CYarn &Yarn) const = 0;
		/// Get the translation vectors necessary to fully fill the domain
		virtual vector<XYZ> GetTranslations(const CYarn &Yarn) const = 0;
		/// Clip the surface elements to the domain
		/**
		For surface elements (TRI and QUAD elements) the elements will be either kept, discarded
		or trimmed depending on their location relative to the domain.
		For volume elements (TET, HEX) the elements will be either kept or discarded. No trimming
		occurs in this case to avoid poorly shaped elements. The center of the element is used to
		determine whether the element should be kept or discarded. For certain finite element analyses
		simulations can be performed without any trimming.
		\param Mesh The mesh instance to be clipped, this will be modified in place
		\param bFillGaps If set to true the resulting holes will be filled with a triangulated mesh
		*/
		virtual void ClipMeshToDomain(CMesh &Mesh, bool bFillGaps = true) const = 0;
		/// Clip the surface elements to the domain
		/**
		\param Mesh The mesh instance to be clipped, this will be modified in place
		\param DomainMeshes One mesh for each plane which domain/mesh intersection polygons are added
		\param bFillGaps If set to true the resulting holes will be filled with a triangulated mesh
		*/
		virtual bool ClipMeshToDomain(CMesh &Mesh, vector<CMesh> &DomainMeshes, bool bFillGaps = true) const = 0;
		/// Derived class should return the class name
		virtual string GetType() const = 0;
		/// Get the mesh representing the domain as a surface mesh
		const CMesh &GetMesh() const { return m_Mesh; }
		/// Get the volume of the domain
		virtual double GetVolume() const;
		/// Make the domain larger (used mainly for rendering purposes)
		virtual void Grow(double dDistance) = 0;
		/// Rotate the domain by given rotation quaternion
		virtual void Rotate(WXYZ Rotation) = 0;
		/// Translate the domain by given vector
		virtual void Translate(XYZ Vector) = 0;
		/// Deform the domain by given linear transformation
		virtual void Deform(CLinearTransformation Transformation) = 0;

		/// Check if a point lies inside the domain
		virtual bool PointInDomain(const XYZ &Point) const = 0;

	protected:
		static vector<pair<int, int> > ConvertLimitsToInt(const vector<pair<double, double> > &RepeatLimits);

		/// A mesh representing the domain as a surface mesh
		CMesh m_Mesh;

	};

};	// namespace TexGen
