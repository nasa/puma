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
#include "Misc.h"
#include "Logger.h"
#include "ObjectContainer.h"
#include "Singleton.h"
#include "Section.h"
#include "SectionEllipse.h"
#include "SectionPowerEllipse.h"
#include "SectionLenticular.h"
#include "SectionBezier.h"
#include "SectionPolygon.h"
#include "SectionHybrid.h"
#include "SectionRotated.h"
#include "SectionScaled.h"
#include "SectionRectangle.h"
#include "Textile.h"
#include "TextileWeave.h"
#include "TextileWeave2D.h"
#include "TextileWeave3D.h"
#include "Domain.h"
#include "DomainPlanes.h"
#include "Mesher.h"
#include "TextileDeformer.h"
#include "TextileDeformerVolumeMesh.h"
#include "GeometrySolver.h"
#include "Simulation.h"
#include "SimulationAbaqus.h"
#include "VoxelMesh.h"
#include "RectangularVoxelMesh.h"
#include "ShearedVoxelMesh.h"
#include "StaggeredVoxelMesh.h"
// #include "OctreeVoxelMesh.h"
#include "RotatedVoxelMesh.h"
#include "TetgenMesh.h"
#include "SurfaceMesh.h"
#include "PeriodicBoundaries.h"
#include "Textile3DWeave.h"
#include "TextileOrthogonal.h"
#include "TextileAngleInterlock.h"
#include "TextileLayerToLayer.h"
#include "TextileOffsetAngleInterlock.h"
#include "ShearedPeriodicBoundaries.h"
#include "StaggeredPeriodicBoundaries.h"
#include "RotatedPeriodicBoundaries.h"
#include "ShearedTextileWeave2D.h"
#include "TextileLayered.h"
#include "PatternDraft.h"
#include "AdjustMeshInterference.h"
#include "Materials.h"
#include "ShellElementExport.h"

/// Helper macro to get the texgen instance
#define TEXGEN (CTexGen::GetInstance())

namespace TexGen
{
	class CLogger;

	using namespace std;

	/// Singleton class holding the Textiles in a database
	class CLASS_DECLSPEC CTexGen : public CSingleton<CTexGen>
	{
	public:

		CTexGen(void);
		~CTexGen(void);

		/// Get the name of the textile with given pointer
		/**
		\return the name of the textile or "" if the textile is not in the list
		*/
		string GetName(const CTextile* pTextile) const;

		/// Get a textile with given name
		/**
		\param TextileName Name of the textile to return
		return Pointer to the textile or NULL if the textile doesn't exist
		*/
		CTextile* GetTextile(string TextileName = "");
		/// Add Textile
		/**
		\param TextileName Name assigned to the textile, it must be unique
		\param Textile Textile object to add, a copy of it will be created and stored
		\param bOverwrite If true an existing textile with the same name will be overwritten
		\return false if the name already exists else true
		*/
		bool AddTextile(string TextileName, const CTextile &Textile, bool bOverwrite = false);
		/// Add Textile
		/**
		\param Textile Textile object to add, a copy of it will be created and stored
		\return Name of the textile created
		*/
		string AddTextile(const CTextile &Textile);
		/// Delete a textile
		/**
		\param TextileName Name of the textile to delete
		\return true if successful
		*/
		bool DeleteTextile(string TextileName);
		/// Used for saving data to XML
		void PopulateTiXmlElement(TiXmlElement &Element, string TextileName = "", OUTPUT_TYPE OutputType = OUTPUT_STANDARD);
		/// Used for loading data from XML
		bool LoadTiXmlElement(TiXmlElement &Element);
		/// Save TexGen XML file
		/**
		\param FileName The name of the XML file on disk
		\param TextileName The name of the textile to save, if left blank will save all textiles
		\param OutputType The amount of information to save to disk
		*/
		void SaveToXML(string FileName, string TextileName = "", OUTPUT_TYPE OutputType = OUTPUT_STANDARD);
		/// Read TexGen XML file
		bool ReadFromXML(string FileName);
		/// Clear Textiles
		void DeleteTextiles();
		/// Set the logger
		void SetLogger(const CLogger &Logger);
		/// Set the callback function when a textile is added or deleted
		void SetTextileCallback(void (*pTextileCallback)(string TextileName, bool bAdded));
		/// Get the version of TexGen
		string GetVersion() const;
		/// Switch messages on/off
		void SetMessages( bool bMessagesOn, const CLogger &Logger );
		/// Switch messages on/off - use from scripts
		void SetMessages( bool bMessagesOn );
		/// Get messages on/off
		bool GetMessagesOn( ) const { return m_bMessagesOn; }
		/// Get list of textile names
		void GetTextileNames( vector<string> &Names );

		// Accessor methods
		const map<string, CTextile*> &GetTextiles() const { return m_Textiles; }
		CLogger &GetLogger() const { return *m_pLogger; }

	protected:
		map<string, CTextile*> m_Textiles;	///< List of textiles created
		CLogger* m_pLogger;	///< Logger used to keep track of how error messages and log messages displayed or stored
		bool m_bMessagesOn; // Messages on/off
		void (*m_pTextileCallback)(string TextileName, bool bAdded);
		int m_iMajorVersion;
		int m_iMinorVersion;
		int m_iRevision;
	};

};	// namespace TexGen


