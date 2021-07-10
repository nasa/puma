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
#include "Domain.h"
#include "Plane.h"

namespace TexGen
{
	using namespace std;

	/// Domain implementation described using planes, the simplest of which would be a box
	/**
	The domain must describe an enclosed area in which the textile lies. If not enough planes are
	specified the domain may be infinite, in which case bad things may happen.
	A plane is described by a normal and a value that represents the shortest distance from the plane
	to the origin (0, 0, 0). The normal should point towards the inside of the domain.
	*/

	class CLASS_DECLSPEC CDomainPlanes : public CDomain
	{
	public:
		CDomainPlanes();
		CDomainPlanes(const vector<PLANE> &Planes);
		CDomainPlanes(const XYZ &Min, const XYZ &Max);
		CDomainPlanes(TiXmlElement &Element);
		~CDomainPlanes(void);

		CDomain* Copy() const { return new CDomainPlanes(*this); }

		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType = OUTPUT_STANDARD) const;

		/// Get a vector of repeat limits for a given yarn. The number of limit pairs is the same
		/// as the number of infinite repeats in the yarn.
		vector<pair<int, int> > GetRepeatLimits(const CYarn &Yarn) const;
		/// Get the translation vectors necessary to fully fill the domain
		vector<XYZ> GetTranslations(const CYarn &Yarn) const;
		void ClipMeshToDomain(CMesh &Mesh, bool bFillGaps = true) const;
		bool ClipMeshToDomain(CMesh &Mesh, vector<CMesh> &DomainMeshes, bool bFillGaps = true) const;
		string GetType() const { return "CDomainPlanes"; }

		void AddPlane(const PLANE &Plane);

		/// Accessor method to get the planes making up this domain
		const vector<PLANE> &GetPlanes() const { return m_Planes; }
		/// If the limits describe a box return the minimum and maximum otherwise return false
		bool GetBoxLimits(XYZ &Min, XYZ &Max);
		/// Move all the planes by given distance along their normal
		void Grow(double dDistance);
		/// Rotate the domain by given rotation quaternion
		void Rotate(WXYZ Rotation);
		/// Translate the domain by given vector
		void Translate(XYZ Vector);
		/// Deform the domain by given linear transformation
		void Deform(CLinearTransformation Transformation);
		/// Get plane with given normal.  Returns both the plane and its index
		int GetPlane( XYZ &Normal, PLANE &Plane );
		/// Set plane at given index
		void SetPlane( int index, PLANE &Plane ); 

		/// Check if a point lies within the domain
		bool PointInDomain(const XYZ &Point) const;

	protected:
		/// Get the limits for a single given repeat vector and surface mesh
		/**
		The limits are the points where the whole surface mesh lies outside the domain
		The limit is represented as the factor multiplied by the repeat vector that represents
		the translation of the surface mesh to the limiting case.
		\param RepeatVector The repeat vector to get limit for
		\param Mesh The surface mesh that needs to lie behind the plane
		\return A pair of doubles representing the upper and lower limits
		*/
		pair<double, double> GetLimits(XYZ RepeatVector, const CMesh &Mesh) const;

		/// Determine if a mesh intersects with the domain
		/**
		This is a very crude approximation of whether a mesh intersects the domain or not.
		It is based soley on the axis aligned bounding boxes of the two regions.
		*/
		bool MeshIntersectsDomain(const CMesh &Mesh) const;

		/// Populate m_PlaneIntersections and m_Mesh, note this only works for closed domains
		void BuildMesh();

		/// Given a mesh and a plane, the holes found in the plane will be filled with triangles
		/// Intersection of mesh and plane returned as closed loop
		static bool FillGaps(CMesh &Mesh, const PLANE &Plane, vector<int> &Polygon, bool bMeshGaps = true);

		/// List of planes that define the limits of the cell
		vector<PLANE> m_Planes;

		/// A list of lines for each plane representing the intersections between other planes
		vector<vector<pair<XYZ, XYZ> > > m_PlaneIntersections;
	};

};	// namespace TexGen

