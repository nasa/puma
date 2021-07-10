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
#include "YarnSection.h"
#include "YarnSectionConstant.h"
#include "YarnSectionInterpNode.h"
#include "YarnSectionInterpPosition.h"
#include "YarnSectionAdjusted.h"
#include "Interpolation.h"
#include "InterpolationBezier.h"
#include "InterpolationCubic.h"
#include "InterpolationLinear.h"
#include "InterpolationAdjusted.h"
#include "FibreDistribution.h"
#include "FibreDistributionConst.h"
#include "FibreDistribution1DQuad.h"
#include "Mesh.h"
#include "Domain.h"
#include "PropertiesYarn.h"

namespace TexGen
{
	class CDomain;
	class CTextile;

	using namespace std;

	/// Represents a yarn consisting of master nodes, section and interpolation function
	class CLASS_DECLSPEC CYarn : public CPropertiesYarn
	{
	friend class CTextile;
	public:
		CYarn(void);
		CYarn(TiXmlElement &Element);
		~CYarn(void);

		enum BUILD_TYPE
		{
			NONE = 0,
			LINE = 1<<0,
			SURFACE = 1<<1,
			VOLUME = 1<<2,
			ALL = LINE | SURFACE | VOLUME,
			CENTRE_PLANE = 1<<3,
		};

		/// Used for saving data to XML
		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		/// Add a node to the end of the list of nodes (note the nodes must be ordered)
		/**
		\param Node The Node to add
		*/
		void AddNode(const CNode &Node);

		/// Insert a node before node pBefore
		/**
		\param Node The Node to add
		\param pBefore Insert node before this node
		\return false if it couldn't find pBefore in the list
		*/
		bool InsertNode(const CNode &Node, const CNode* pBefore);

		/// Insert a node at given index
		/**
		\param Node The Node to add
		\param iIndex The index which the new node will take
		\return false if the index is invalid
		*/
		bool InsertNode(const CNode &Node, int iIndex);

		/// Replace an existing node with given node
		/**
		\param NewNode The new node which will be replacing the existing node
		\param iIndex The index of the node to replace
		\return false if the index is out of range
		*/
		bool ReplaceNode(int iIndex, CNode NewNode);

		/// Delete an existing node
		/**
		\param iIndex The index of the node to delete
		\return false if the index is out of range
		*/
		bool DeleteNode(int iIndex);

		/// Get a master node by index
		const CNode* GetNode(int iIndex) const;

		/// Set the nodes for this yarn given as an ordered list of nodes
		void SetNodes(const vector<CNode> &Nodes);

		/// Assign an interpolation function to the yarn
		/**
		The interpolation function determines what the exact path of the yarns will be
		between the master nodes. For example bezier curves or cubic splines.
		*/
		void AssignInterpolation(const CInterpolation &Interpolation);

		/// Assign a fibre distribution 
		void AssignFibreDistribution(const CFibreDistribution &Distribution);

		/// Set the resolution of the mesh created
		/**
		\param iNumSlaveNodes The number of slave nodes to create
		\param iNumSectionPoints The number of points defining the section
		*/
		void SetResolution(int iNumSlaveNodes, int iNumSectionPoints);

		/// Set the resolution of the mesh created
		/**
		The number of slave nodes is calculated such that the distance between nodes is equal
		to the average distance between section points. This is done in an iterative manner,
		since the average distance between section points and length of the yarn is not known
		until the yarn is built.
		\param iNumSectionPoints The number of points defining the section
		*/
		bool SetResolution(int iNumSectionPoints);

		/// Meshes of the yarn should be equispaced if true
		/**
		When this is set to true, volume meshes of the yarn will be constructed
		such that the points on a cross-section are equispaced. This should
		result in higher quality volume meshes. Otherwise the spacing between
		the nodes is determined by the CSection parametric function.
		*/
		void SetEquiSpacedSectionMesh(bool bEquiSpacedSectionMesh);

		/// Assign a section to the yarn
		void AssignSection(const CYarnSection &YarnSection);

		/// Rotate the Yarn by given quaternion
		void Rotate(WXYZ Rotation, XYZ Origin = XYZ(0,0,0));

		/// Translate the Yarn by given vector
		void Translate(XYZ Vector);

		/// Add the axis aligned bounding box of the yarn to the Mesh
		bool AddAABBToMesh(CMesh &Mesh) const;
		
		/// Add the repeated axis aligned bounding box of the yarn to the Mesh
		bool AddAABBToMesh(CMesh &Mesh, const vector<pair<int, int> > &RepeatLimits) const;

		/// Add the master nodes to the mesh
		bool AddNodesToMesh(CMesh &Mesh) const;

		/// Add yarn centerline path to mesh
		bool AddPathToMesh(CMesh &Mesh) const;

		/// Create surface mesh for this yarn and add it to the surface mesh object
		/**
		\param Mesh Mesh to add elements to
		\param bAddEndCaps If true then the ends of the yarns will be closed with triangles
		*/
		bool AddSurfaceToMesh(CMesh &Mesh, bool bAddEndCaps = true) const;

		/// Create surface mesh for this yarn and add it to the surface mesh object
		/**
		\param Mesh Mesh to add elements to
		\param RepeatLimits A vector of pairs of doubles that represent the repeat limits
			the number of items in the vector should be equal to the number of infinite repeats
		*/
		bool AddSurfaceToMesh(CMesh &Mesh, const vector<pair<int, int> > &RepeatLimits) const;

		/// Create surface mesh for this yarn and add it to the surface mesh object
		/**
		\param Mesh Mesh to add elements to
		\param TranslationVectors A number of translations be to applied to the yarn
		\param bAddEndCaps If true then the ends of the yarns will be closed with triangles
		*/
		bool AddSurfaceToMesh(CMesh &Mesh, const vector<XYZ> &TranslationVectors, bool bAddEndCaps = true) const;

		/// Create surface mesh for this yarn bounded by the given domain and add it to the surface mesh object
		/**
		\param Mesh Mesh to add elements to
		\param Domain Domain to restrict the mesh to
		\param bAddEndCaps If true then the ends of the yarns will be closed with triangles
		*/
		bool AddSurfaceToMesh(CMesh &Mesh, const CDomain &Domain, bool bAddEndCaps = true, bool bFillGaps = true) const;

		/// Create surface mesh for this yarn bounded by the given domain and add it to the surface mesh object
		/// Polygons created by the intersection of yarns with domain are added to the DomainMeshes object
		/**
		\param Mesh Mesh to add elements to
		\param Domain Domain to restrict the mesh to
		*/
		bool AddSurfaceToMesh( CMesh &Mesh, const CDomain &Domain, vector<CMesh> &DomainMeshes ) const;

		/// Create volume mesh for this yarn and add it to the volume mesh object
		/**
		\param Mesh Mesh to add elements to
		*/
		bool AddVolumeToMesh(CMesh &Mesh) const;

		/// Create volume mesh for this yarn and add it to the volume mesh object
		/**
		\param Mesh Mesh to add elements to
		\param RepeatLimits A vector of pairs of doubles that represent the repeat limits
			the number of items in the vector should be equal to the number of infinite repeats
		*/
		bool AddVolumeToMesh(CMesh &Mesh, const vector<pair<int, int> > &RepeatLimits) const;

		/// Create volume mesh for this yarn and add it to the volume mesh object
		/**
		\param Mesh Mesh to add elements to
		\param TranslationVectors A number of translations be to applied to the yarn
		*/
		bool AddVolumeToMesh(CMesh &Mesh, const vector<XYZ> &TranslationVectors) const;

		/// Create volume mesh for this yarn bounded by the given domain and add it to the volume mesh object
		/**
		\param Mesh Mesh to add elements to
		\param Domain Domain to restrict the mesh to
		*/
		bool AddVolumeToMesh(CMesh &Mesh, const CDomain &Domain) const;

		/// Create mesh of the centre plane for this yarn and add it to the plane mesh object
		/**
		\param Mesh Mesh to add elements to
		*/
		bool AddCentrePlaneToMesh(CMesh &Mesh) const;

		/// Create mesh of the centre plane for this yarn and add it to the plane mesh object
		/**
		\param Mesh Mesh to add elements to
		\param TranslationVectors A number of translations be to applied to the yarn
		*/
		bool AddCentrePlaneToMesh(CMesh &Mesh, const vector<XYZ> &TranslationVectors) const;

		/// Create centre plane mesh for this yarn bounded by the given domain and add it to the plane mesh object
		/**
		\param Mesh Mesh to add elements to
		\param Domain Domain to restrict the mesh to
		*/
		bool AddCentrePlaneToMesh(CMesh &Mesh, const CDomain &Domain) const;

		/// Add a repeat vector
		void AddRepeat(XYZ Repeat);

		/// Remove all repeat vectors
		void ClearRepeats();

		/// Set the repeat vectors
		void SetRepeats(const vector<XYZ> &Repeats);

		/// Check if the repeat vector is the same as the last master node - first master node
		/**
		This function determines if the repeat vector will result in the origin of the repeated yarn to
		be connected to the end of the non-repeated yarn.
		*/
		bool RepeatMatchesEnds(XYZ Repeat) const;

		/// Set the tangents and up vectors of all the master nodes to null
		void ClearMasterNodeOrientations();

		/// Determine if the given point lies within the yarn (this function doesn't take the repeats into account).
		/**
		A limitation with this function is that it only work for convex sections because
		of the way the it determines if a point lies within a section or not. Could be updated to work
		with non-convex sections at the expensive of performance.
		*/
		bool PointInsideYarn( const XYZ &Point, XYZ *pTangent = NULL, XY *pLoc = NULL, double *pVolumeFraction = NULL, double* pDistanceToSurface = NULL, double dTolerance = 1e-9, XYZ *pOrientation = NULL, XYZ *pUp = NULL, bool bSurface = false ) const;

		/// Similar to the above function except that this one takes into account repeated yarns.
		/**
		The translations can be calculated by creating a domain that encompases this point alone
		and call the calculate translation from the domain. However this would be in-efficient
		on a point by point basis. Consider using GetPointTangents from CTextile instead.
		A limitation with this function is that it only work for convex sections because
		of the way the it determines if a point lies within a section or not. Could be updated to work
		with non-convex sections at the expensive of performance.
		pTangent, pLoc, pVolumeFraction and pDistanceToSurface are output pointers, they can be
		NULL where the information is not needed.
		\param bSurface Defaults to false. Set to true where points are known to be on surface and simply want to return information without PointInside check
		*/
		bool PointInsideYarn(const XYZ &Point, const vector<XYZ> &Translations, XYZ *pTangent = NULL, XY* pLoc = NULL, double* pVolumeFraction = NULL, double* pDistanceToSurface = NULL, double dTolerance = 1e-9, XYZ *pOrientation = NULL, XYZ *pUp = NULL, bool bSurface = false) const;

		/// Find the plane normal to the yarn which contains a specified point
		/**
		Returns false if iterative method doesn't find a plane
		Returns Planes, a vector of all instances of the normalised distance along the yarn where the plane crosses 
		the yarn centreline. Where the yarn undulates there may be more than one plane
		*/
		bool FindPlaneContainingPoint( const XYZ &Point, double &u, double dTolerance, int iSeg) const;

		/// Finds the closest point on the yarn surface to Point
		/**
		\param Point The point for which the closest point on the surface is being found
		\param SurfacePoint Used to return the closest point found
		\param iNumSectionPoints Set this to the number of points to be generated around the cross-section
		       The function returns one of these points so increasing this value will improve the accuracy of the point found
		\param dTolerance Tolerance for plane containing the search point
		\return The distance of the point to the surface
		*/
		double FindClosestSurfacePoint(const XYZ &Point, XYZ &SurfacePoint, int iNumSectionPoints, double dTolerance = 1e-9);

		/// Finds the closest point on the yarn surface to Point
		/**
		\param Point The point for which the closest point on the surface is being found
		\param Translations for if repeat specified
		\param SurfacePoint Used to return the closest point found
		\param iNumSectionPoints Set this to the number of points to be generated around the cross-section
		       The function returns one of these points so increasing this value will improve the accuracy of the point found
		\param dTolerance Tolerance for plane containing the search point
		\return True if point found, False if not
		*/
		bool FindClosestSurfacePoint(const XYZ &Point, const vector<XYZ> &Translations, XYZ &SurfacePoint, int iNumSectionPoints, double dTolerance = 1e-9);

		/// Straighten the yarn
		/**	
		The master nodes are adjusted to vary between a completely straight yarn and yarn that is crimped
		as much as before calling this function.
		This is used for finite element code to adjust initial strains stored in the yarn.
		\param dStraightness A value of 1 represents a completely straight yarn, a value of 0 represents
			an unchanged yarn. Values in between represent various degrees of flatness.
		*/
		void StraightenYarn(double dStraightness = 1);

		/// Get axis aligned bounding box for the yarn (building the yarn if necessary)
		pair<XYZ, XYZ> GetAABB() const;

		/// Get axis aligned bounding box for a yarn section (building the yarn if necessary)
		pair<XYZ, XYZ> GetSectionAABB(int iIndex) const;

		/// Get the length of the yarn (building the yarn if necessary)
		/**
		This function doesn't take care of units, it is the raw unconverted yarn length.
		*/
		double GetRawYarnLength() const;

		/// Get the length of a yarn section (building the yarn if necessary)
		/**
		This function doesn't take care of units, it is the raw unconverted yarn section length.
		*/
		double GetRawYarnSectionLength(int iIndex) const;

		/// Get the volume of the yarn (building the yarn if necessary)
		/**
		This function doesn't take care of units, it is the raw unconverted yarn volume.
		*/
		double GetRawYarnVolume() const;

		/// Get repeat area
		/**
		This area corresponds to the area of a parallelogram formed by two repeat vectors.
		Note that in order for this to work the number of repeat vectors must be exactly
		equal to two otherwise the area returned is 0.
		*/
		double GetRawRepeatArea() const;

		/// Get the lengths of all the yarn sections
		vector<double> GetYarnSectionLengths() const;

		/// Get the slave nodes and build them if necessary
		/**
		\param Usage Specify what the slave nodes will be used for, this will determine what
			type of build is performed (LINE, SURFACE or VOLUME). If only the position of the
			slave nodes is required select LINE, if access to the slave node's sections is required
			select SURFACE, if access to the slave node's section mesh is required select VOLUME.
		*/
		const vector<CSlaveNode> &GetSlaveNodes(BUILD_TYPE Usage) const;


		/// Calculate the total yarn length
		/**
		This is the total length of yarn without taking in account of repeat vectors or
		unit cell sizes. It is calculated from the geometry alone and uses the m_GeometryScale
		to give the value in the requested units.
		*/
		double GetRealYarnLength(string Units = "m") const;

		/// Calculate the total yarn length per unit area
		/**
		Calculates the total length of yarn in the unit cell taking into account the repeat vectors.
		This function only works when each yarn has two repeat vectors assigned to it, otherwise
		it does not make sense to obtain a length per unit AREA.
		*/
		double GetYarnLengthPerUnitArea(string Units = "/m") const;

		/// Calculate the yarn volume
		/**
		This is the volume without taking account of repeat vectors or unit cell sizes.
		It is calculated from the geometry alone and uses the m_GeometryScale to give the value
		in the requested units.
		*/
		double GetRealYarnVolume(string Units = "m^3") const;

		/// Calculate the total yarn volume per unit area
		/**
		Calculates the volume of yarn in the unit cell taking into account the repeat vectors.
		This function only works when each yarn has two repeat vectors assigned to it, otherwise
		it does not make sense to obtain a volume per unit AREA. If 1 repeat vector is given then
		a volume per unit length would be more suitable, similarly 2 repeat vectors would give a
		volume per unit volume (i.e. a volume fraction of region occupied by yarns).
		*/
		double GetYarnVolumePerUnitArea(string Units = "m") const;

		/**
		Calculates the volume of fibre without taking account of repeat vectors or unit cell sizes.
		This only works when the linear density of the yarns and fibre density are specified.
		*/
		double GetFibreVolume(string Units = "m^3") const;

		/**
		Calculates the volume of fibre per unit area of fabric. This only works when the areal density and
		fibre density is specified. Alternatively if linear density and fibre density is specified calculation
		will be based on these values.
		*/
		double GetFibreVolumePerUnitArea(string Units = "m") const;

		/**
		Calculates the volume of fibre divided by the volume of yarn. See GetFibreVolume and
		GetYarnVolume functions for requirements.
		*/
		double GetFibreYarnVolumeFraction() const;

		/// Get number of section points
		int GetNumSectionPoints() const { return m_iNumSectionPoints; }

		/// Get number of slave nodes
		int GetNumSlaveNodes() const { return m_iNumSlaveNodes; }

		/// Converts a yarn with constant cross-section to interpolate sections between nodes
		/// Sets the section at each node to the original section
		bool ConvertToInterpNodes();

		// Accessor methods
		const vector<CNode> &GetMasterNodes() const { return m_MasterNodes; }
		const vector<XYZ> &GetRepeats() const { return m_Repeats; }
		const CYarnSection* GetYarnSection() const { return m_pYarnSection; }
		const CInterpolation* GetInterpolation() const { return m_pInterpolation; }
		const CFibreDistribution* GetFibreDistribution() const { return m_pFibreDistribution;}
		int GetNumNodes() const { return (int)m_MasterNodes.size(); }
		vector<double> GetSectionLengths() const { return m_SectionLengths; }

		CMesh::ELEMENT_TYPE GetMeshPoint( CMesh &Mesh, const XY &Point, int &Index ) const;

		//int GetMeshPoint( const XY &Point, int& Index );

	protected:
		/// Create slave nodes and apply yarn section to them
		/**
		Before this function is called, a yarn section must be applied to this yarn along with
		an interpolation function. The resolution of the yarn created must also be set. The order
		of these is not important, but they must all be done before building the yarn.
		This function should be called by any function that needs a built yarn (i.e. AddSurfaceToMesh).
		m_iNeedsBuilding should be set to ALL whenever a change to the geometry is made which would
		require a rebuild of the yarn. If iBuildType is set to VOLUME any sections used in this yarn
		must have section meshes assigned to them. Furthermore, all sections in the yarn must have
		compatible section meshes. What	this means is that the number of nodes in the section meshes
		must be the same and the node linkage must be the same.
		\param iBuildType Can be a value from the BUILD_TYPE enum. Create either a "SURFACE" mesh or
			"VOLUME" mesh (Note: surface mesh is automatically created along with the volume mesh).
		@see SetResolution
		@see AssignInterpolation
		@see AssignSection
		@see BUILD_TYPE
		\return false if the yarn building failed
		*/
		bool BuildYarnIfNeeded(int iBuildType) const;

		bool BuildSlaveNodes() const;
		bool BuildSections() const;
		bool BuildSectionMeshes() const;

		/// Add end caps to the mesh
		void AddEndCapsToMesh(CMesh &Mesh) const;

		/// Create the section Axis aligned bounding boxes
		void CreateSectionAABBs() const;

		/// Set the yarn parent
		void SetParent(const CTextile *pParent);

		vector<CNode> m_MasterNodes;	///< Ordered list of nodes belonging to this Yarn
		CObjectContainer<CInterpolation> m_pInterpolation;	///< Interpolation applied to smooth the yarn paths
		CObjectContainer<CYarnSection> m_pYarnSection;	///< Section applied to this yarn, with possibility of a varying cross-section
		CObjectContainer<CFibreDistribution> m_pFibreDistribution;	///< Fibre distribution given to the yarn, used for getting Fibre Volume Fraction

		vector<XYZ> m_Repeats;	///< List of infinite repeat vectors, yarns will be repeated to infinite displaced by the specified vector multiplied integer values
		int m_iNumSlaveNodes;	///< Number of slave nodes to create
		int m_iNumSectionPoints;	///< Number of section points to create
		bool m_bEquiSpacedSectionMesh;	///< Whether or not volume meshes of the yarn should be equispaced

		/// Find whether point is inside polygon specified by Nodes
		bool PointInside( const XY &Point, vector<XYZ> &Nodes ) const;
		bool PointInside( const XY &Point, const vector<XY> &Nodes ) const;

		/// Find closest perpendicular distance from point to polygon specified by SectionPoints
		double FindClosestEdgeDistance( XY &Loc, const vector<XY> &SectionPoints, double dTolerance ) const;

		mutable int m_iNeedsBuilding;	///< Variable used to keep track of wether the yarn needs to be rebuilt or not and what part needs rebuilding
		mutable vector<CSlaveNode> m_SlaveNodes;	///< Ordered list of interpolated slave nodes belonging to this Yarn

		/// An axis aligned bounding box containing the full unrepeated yarn
		/**
		Will be populated when the yarn surface is built.
		*/
		mutable pair<XYZ, XYZ> m_AABB;

		/// An axis aligned bounding box containing each section of the yarn
		/**
		A section is defined as the part of the yarn between master nodes. The first
		section refers to the section between the first two nodes and so on.
		The size of the vector is one less than the number of master nodes.
		Will be populated when the yarn surface is built.
		*/
		mutable vector<pair<XYZ, XYZ> > m_SectionAABBs;

		/// The length of each of the sections in the yarn
		/**
		Will be populated when yarn surface is built.
		*/
		mutable vector<double> m_SectionLengths;

		/// Stores a pointer to the CTextile it belongs to
		/**
		Note: This can be dangerous when a yarn is copy constructed. The copied
		version will retain a pointer to the parent textile. If that textile
		then gets deleted the pointer will become invalid.
		*/
		// Copy constructer for the yarn may be needed which sets this value to
		// NULL on the copied yarn
		CWeakPointer<const CTextile> m_pParent;
//		CTextile* m_pParent;

	private:
		void AssignDefaults();
	};


};	// namespace TexGen


