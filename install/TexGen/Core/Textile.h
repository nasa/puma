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
#include "Yarn.h"
#include "PropertiesTextile.h"
namespace TexGen
{ 
	class CDomain;
	class CTextileWeave;
	class CTextile3DWeave;
	class CTextileLayered;
	class CTextileOrthogonal;
	class CTextileLayerToLayer;
	class CTextileAngleInterlock;
	class CTextileWeave2D;

	using namespace std;

	/// Represents a textile cell containing yarns
	class CLASS_DECLSPEC CTextile : public CPropertiesTextile
	{
	public:
		CTextile(void);
		CTextile(const vector<CYarn> &Yarns);
		CTextile(const CTextile &CopyMe);
		CTextile(TiXmlElement &Element);
		virtual ~CTextile(void);

		// NOTE: IF DERIVING FROM THIS CLASS BE SURE TO IMPLEMENT THIS FUNCTION
		virtual CTextile* Copy() const { return new CTextile(*this); }

		/// Derived class should return the class name
		virtual string GetType() const { return "CTextile"; }

		CTextile &operator=(const CTextile& CopyMe);

		/// Used for saving data to XML
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		/// Add a Yarn to the textile
		int AddYarn(const CYarn &Yarn);

		/// Delete an Yarn from the textile
		/**
		\param iIndex The index of the yarn to delete
		\return false if the index is out of range
		*/
		bool DeleteYarn(int iIndex);

		/// Delete all Yarns in the textile
		void DeleteYarns();

		/// Add the master nodes to the mesh
		void AddNodesToMesh(CMesh &Mesh);

		/// Add yarn centerline path to mesh
		void AddPathToMesh(CMesh &Mesh);

		/// Create surface mesh for this textile and add it to the mesh object
		/**
		\param Mesh Mesh to add elements to
		\param bTrimToDomain Mesh inserted will be clipped to the domain size
		*/
		void AddSurfaceToMesh(CMesh &Mesh, bool bTrimToDomain = false);

		/// Create surface mesh for this textile and add it to the mesh object
		/// Create meshes for domain plane faces with yarn intersection polygons included
		/**
		\param Mesh Mesh to add elements to
		\param bTrimToDomain Mesh inserted will be clipped to the domain size
		\param DomainMeshes Vector of meshes, one for each face of the domain
		*/
		bool AddSurfaceToMesh(CMesh &Mesh, vector<CMesh> &DomainMeshes, bool bTrimToDomain = false);

		/// Create volume mesh for this textile and add it to the mesh object
		/**
		\param Mesh Mesh to add elements to
		\param bTrimToDomain Mesh inserted will be clipped to the domain size
		*/
		void AddVolumeToMesh(CMesh &Mesh, bool bTrimToDomain = false);

		/// Create centre plane mesh for this textile and add it to the mesh object
		/**
		\param Mesh Mesh to add elements to
		\param bTrimToDomain Mesh inserted will be clipped to the domain size
		*/
		void AddCentrePlaneToMesh(CMesh &Mesh, bool bTrimToDomain = false);

/*		/// Create Mesh and output to abaqus file format
		bool OutputAbaqus(string FileName, double dInitialStrains = 0);*/

		/// Get usefull information of a list of points
		/**
		Pass in a vector of point coordinates, for each point it will be determined if
		it lies within a yarn. If it does then the yarn index along with the tangent of the
		yarn at that point will be returned along with its 2D location relative to the yarn
		center line. If the point doesn't lie within the yarn an index of -1 will be returned.
		\param Points List of points for which the tangents is required
		\param PointsInfo Information about the point as described in the POINT_INFO structure
		\param dTolerance Points outside the yarn by a distance less than this tolerance will be considered part of the yarn
		*/
		void GetPointInformation(const vector<XYZ> &Points, vector<POINT_INFO> &PointsInfo, double dTolerance = 1e-9);

		/// Get information when know which yarn point is in.  Saves iterating through entire textile
		/**
		\param bSurface default is False. Set to True if getting point information for surface mesh export. Assumes Points are on yarn surface and doesn't do PointInside check
		*/
		void GetPointInformation(const vector<XYZ> &Points, vector<POINT_INFO> &PointsInfo, int iYarn, double dTolerance = 1e-9, bool bSurface = false);

		/// Output point information to VTK
		/**
		Makes a call to GetPointInformation and stores the result in a VTK unstructured grid file
		*/
		void SavePointInformationToVTK(string Filename, const CMesh &Mesh, double dTolerance = 1e-9);

		/// Get an approximate size for the textile
		/**
		This is useful for rendering where the size of certain objects rendered
		should be relative to the approximate size of the textile.
		*/
		double GetApproximateSize();

		/// Find out if any of the yarns are interfering with each other
		int DetectInterference(vector<float> &DistanceToSurface, vector<int> &YarnIndex, bool bTrimToDomain, CMesh *pInterferingPoints = NULL );

		/// Rotate the Textile by given quaternion
		void Rotate(WXYZ Rotation, XYZ Origin = XYZ(0,0,0));

		/// Translate the Textile by given vector
		void Translate(XYZ Vector);

		/// Get the default name to assign to a textile
		/**
		Derived classes should have different default names based on the type of textile
		*/
		virtual string GetDefaultName() const { return "Textile"; }

		/// Get the name associated with this textile
		string GetName() const;

		/// Assign a domain to the textile, will be used by default to trim the domain
		void AssignDomain(const CDomain &Domain);

		/// Remove the domain
		void RemoveDomain();

		/// Set the resolution for all yarns in textile
		bool SetResolution(int Resolution);



		/// Calculate the total yarn length
		/**
		This is the total length of yarn without taking in account of repeat vectors or
		unit cell sizes. It is calculated from the geometry alone and uses the m_GeometryScale
		to give the value in the requested units.
		*/
		double GetYarnLength(string Units = "m");

		/// Calculate the total yarn length per unit area
		/**
		Calculates the total length of yarn in the unit cell taking into account the repeat vectors.
		This function only works when each yarn has two repeat vectors assigned to it, otherwise
		it does not make sense to obtain a length per unit AREA.
		*/
		double GetYarnLengthPerUnitArea(string Units = "/m");

		/// Calculate the yarn volume
		/**
		This is the raw volume without taking account of repeat vectors or unit cell sizes.
		It is calculated from the geometry alone and uses the m_GeometryScale to give the value
		in the requested units.
		*/
		double GetYarnVolume(string Units = "m^3");

		/// Calculate the total yarn volume per unit area
		/**
		Calculates the volume of yarn in the unit cell taking into account the repeat vectors.
		This function only works when each yarn has two repeat vectors assigned to it, otherwise
		it does not make sense to obtain a volume per unit AREA. If 1 repeat vector is given then
		a volume per unit length would be more suitable, similarly 2 repeat vectors would give a
		volume per unit volume (i.e. a volume fraction of region occupied by yarns).
		*/
		double GetYarnVolumePerUnitArea(string Units = "m");

		/// Calculate the volume of fibre in the yarn, for just the nodes specified
		/**
		Calculates the raw volume of fibre without taking account of repeat vectors or unit cell sizes.
		This only works when the linear density of the yarns and fibre density are specified.
		*/
		double GetFibreVolume(string Units = "m^3");

		/// Calculate the volume of fibre per unit area
		/**
		Calculates the volume of fibre per unit area of fabric. This only works when the areal density and
		fibre density is specified. Alternatively if linear density and fibre density is specified calculation
		will be based on these values.
		*/
		double GetFibreVolumePerUnitArea(string Units = "m");

		/// Calculates the total fibre volume fraction for all yarns
		/**
		Calculates the volume of fibre divided by the volume of yarn. See GetFibreVolume and
		GetYarnVolume functions for requirements.
		*/
		double GetFibreYarnVolumeFraction();

		/// Calculates the fibre volume fraction for the domain.
		/**
		Uses the fibre volume fraction for whole yarns which may give inaccuracies where the yarn cross-section
		varies along its length. Useful for quick approximation as the more accurate GetDomainVolumeFraction may be slow.
		*/
		double GetQuickDomainVolumeFraction();

		/// Calculates the fibre volume fraction for the domain
		double GetDomainVolumeFraction();

		/// Set the Youngs Modulus for all yarns in textile
		void SetAllYarnsYoungsModulusX( double dValue, string Units = "MPa");
		void SetAllYarnsYoungsModulusY( double dValue, string Units = "MPa");
		void SetAllYarnsYoungsModulusZ( double dValue, string Units = "MPa");
		/// Set the shear modulus for all yarns in textile
		void SetAllYarnsShearModulusXY(double dValue, string Units = "MPa");
		void SetAllYarnsShearModulusXZ(double dValue, string Units = "MPa");
		void SetAllYarnsShearModulusYZ(double dValue, string Units = "MPa");
		/// Set the coefficient of thermal expansion for all yarns in textile
		void SetAllYarnsAlphaX( double dValue, string Units = "/K");
		void SetAllYarnsAlphaY( double dValue, string Units = "/K");
		void SetAllYarnsAlphaZ( double dValue, string Units = "/K");
		/// Set the Poisson's ratio for all yarns in textile
		void SetAllYarnsPoissonsRatioX(double dValue);
		void SetAllYarnsPoissonsRatioY(double dValue);
		void SetAllYarnsPoissonsRatioZ(double dValue);

		/// Converts each yarn with constant cross-section to interpolate sections between nodes
		/// Sets the section at each node to the original section
		bool ConvertToInterpNodes() const;


		/// If this textile is a woven textile get a pointer of that type else return NULL
		/**
		Internally the function just returns dynamic_cast<CTextileWeave*>(this)
		This is mainly usefull in the python interface because type casting is not supported
		in this language.
		*/
		CTextileWeave* GetWeave();
		CTextileWeave2D* GetWeave2D();
		CTextile3DWeave* Get3DWeave();
		CTextileOrthogonal* GetOrthogonalWeave();
		CTextileLayered* GetLayeredTextile();
		CTextileLayerToLayer* GetLayerToLayerWeave();
		CTextileAngleInterlock* GetAngleInterlockWeave();

		// Accessor methods
		int GetNumYarns() const;
		const CYarn *GetYarn(int iIndex) const;
		CYarn *GetYarn(int iIndex);
		const vector<CYarn> &GetYarns() const;
		vector<CYarn> &GetYarns();
		const CDomain* GetDomain() const {return m_pDomain;}
		CDomain* GetDomain() {return m_pDomain;}

	protected:
		/// Build the textile only if needed
		/**
		Note: This is only relavant for classes which derive from CTextile and handle the
		construction of yarns automatically. Note that it may seem strange that this
		function should be declared as const, but it is to allow dynamic building of
		the textile when necessary even from within const functions.
		*/
		bool BuildTextileIfNeeded() const;

		/// Build the textile even if it is already built (virtual function which does nothing by default)
		/**
		Note: This is only relavant for classes which derive from CTextile and handle the
		construction of yarns automatically. Note that it may seem strange that this
		function should be declared as const, but it is to allow dynamic building of
		the textile when necessary even from within const functions.
		*/
		virtual bool BuildTextile() const { return true; }

		/// Remove all dynamic memory (called by destructor and copy constructor)
		/// Not to be used to empty the yarn data for re-use
		void CleanUp();

		int AddYarn(const CYarn &Yarn) const;

		/// Vector of yarns contained within this cell
		/**
		Note that this variable has been made mutable to enable building of the
		textile when necessary. For example, the function GetNumYarns should be
		declared as const but before the number of yarns can be retreived it
		must build the textile which cannot be done front a const function unless
		this variable is declared as mutable.
		*/
		mutable vector<CYarn> m_Yarns;

		/// Variable which keeps track of wether the textile needs building or not
		/**
		Note: This is only relavant for classes which derive from CTextile and handle the
		construction of yarns automatically
		*/
		mutable bool m_bNeedsBuilding;

		CObjectContainer<CDomain> m_pDomain;
	};
};	// namespace TexGen
