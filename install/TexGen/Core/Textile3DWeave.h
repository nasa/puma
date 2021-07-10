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
#include "Textile.h"
#include "DomainPlanes.h"
#include "SectionPowerEllipse.h"
#include "PatternDraft.h"


namespace TexGen
{ 
	using namespace std;

	class CPatternDraft;

	enum
	{
		PATTERN3D_YYARN,
		PATTERN3D_XYARN,
		PATTERN3D_NOYARN
	};

	enum
	{
		WARP,
		BINDER,
		WEFT
	};

	enum
	{
		BOTTOM_TO_TOP,
		TOP_TO_BOTTOM,
		ALTERNATE_WEFT_STACK
	};

	typedef int PATTERN3D;

	/// Data structure to keep track of yarn parameters
	struct YARNDATA
	{
		double dWidth;
		double dHeight;
		double dSpacing;
		//int iFibresPerYarn;
		//double dFibreDiameter;
	};

	/// Represents a 3D woven textile
	class CLASS_DECLSPEC CTextile3DWeave : public CTextile
	{
	public:

		/// Build a weave unit cell of given width, height, yarn spacing and fabric thickness
		/**
		\param iNumXYarns Total number of warp and binder yarns parallel to x-axis
		\param iNumYYarns Total number of weft yarns parallel to y-axis
		\param dXSpacing Spacing of x yarns, centred on yarn
		\param dYSpacing Spacing of y yarns, centred on yarn
		\param dXHeight Warp yarn height
		\param dYHeight Weft yarn height
		\param bRefine True if refinement selected
		*/ 
		CTextile3DWeave(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing,
							double dXHeight, double dYHeight, bool bRefine = false);
		CTextile3DWeave(TiXmlElement &Element);
		virtual ~CTextile3DWeave(void);

		virtual CTextile* Copy() const { return new CTextile3DWeave(*this); }
		virtual string GetType() const { return "CTextile3DWeave"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		/// Set the thickness of the fabric
		/**
		\param dThickness New fabric thickness to assign
		*/
		void SetThickness(double dThickness);
		/// Set the maximum yarn volume fraction for use in refinement
		void SetMaxVolFraction( double dVolFraction );
		/// Set the width of the yarn parallel to the X axis, with given index
		void SetXYarnWidths(int iIndex, double dWidth);
		/// Set the width of the yarn parallel to the Y axis, with given index
		void SetYYarnWidths(int iIndex, double dWidth);
		/// Set the height of the yarn parallel to the X axis, with given index
		void SetXYarnHeights(int iIndex, double dHeight);
		/// Set the height of the yarn parallel to the Y axis, with given index
		void SetYYarnHeights(int iIndex, double dHeight);
		/// Set the spacing of the yarn parallel to the X axis, with given index
		void SetXYarnSpacings(int iIndex, double dSpacing);
		/// Set the spacing of the yarn parallel to the Y axis, with given index
		void SetYYarnSpacings(int iIndex, double dSpacing);


		/// Get the width of the yarn parallel to the X axis, with given index
		double GetXYarnWidths(int iIndex) const;
		/// Get the width of the yarn parallel to the Y axis, with given index
		double GetYYarnWidths(int iIndex) const;
		/// Get the height of the yarn parallel to the X axis, with given index
		double GetXYarnHeights(int iIndex) const;
		/// Get the height of the yarn parallel to the Y axis, with given index
		double GetYYarnHeights(int iIndex) const;
		/// Get the spacing of the yarn parallel to the X axis, with given index
		double GetXYarnSpacings(int iIndex) const;
		/// Get the spacing of the yarn parallel to the Y axis, with given index
		double GetYYarnSpacings(int iIndex) const;
		/// Get the gap size between two yarns parallel to the X axis, with given index
		double GetXYarnGapSize(int iIndex) const;
		/// Get the gap size between two yarns parallel to the Y axis, with given index
		double GetYYarnGapSize(int iIndex) const;

		/// Set the width of all the yarns parallel to the X axis
		void SetXYarnWidths(double dWidth);
		/// Set the width of all the yarns parallel to the Y axis
		void SetYYarnWidths(double dWidth);
		/// Set the width of yarns parallel to the X axis in warp yarn positions
		void SetWarpYarnWidths( double dWidth );
		/// Set the width of yarns parallel to the X axis in binder yarn positions
		void SetBinderYarnWidths( double dWidth );
		/// Set the height of all the yarns parallel to the X axis
		void SetXYarnHeights(double dHeight);
		/// Set the height of all the yarns parallel to the Y axis
		void SetYYarnHeights(double dHeight);
		/// Set the height of yarns parallel to the X axis in warp yarn positions
		void SetWarpYarnHeights( double dHeight );
		/// Set the height of yarns parallel to the X axis in binder yarn positions
		void SetBinderYarnHeights( double dHeight );
		/// Set the spacing of all the yarns parallel to the X axis
		void SetXYarnSpacings(double dSpacing);
		/// Set the spacing of all the yarns parallel to the Y axis
		void SetYYarnSpacings(double dSpacing);
		/// Set the spacing of yarns parallel to the X axis in warp yarn positions
		void SetWarpYarnSpacings( double dWidth );
		/// Set the spacing of yarns parallel to the X axis in binder yarn positions
		void SetBinderYarnSpacings( double dWidth );
		/// Set the power for power ellipse section for yarns parallel to Y axis
		void SetWeftYarnPower( double dPower ){ m_dWeftPower = dPower;}
		/// Set the power for power ellipse section for yarns parallel to X axis in warp yarn positions
		void SetWarpYarnPower( double dPower ){ m_dWarpPower = dPower;}
		/// Set the power for power ellipse section for yarns parallel to X axis in binder yarn positions
		void SetBinderYarnPower( double dPower ){ m_dBinderPower = dPower;}

		/// Set the fibre linear density, i.e. the mass per unit length of fibre
		/**
		Suppose 1 meter length of yarn weighs X kg, it's linear density is X kg/m
		\param iYarnType WARP,WEFT or BINDER 
		*/
		void SetYarnLinearDensity( int iYarnType, double dValue, string Units = "kg/m" );

		/// Set the fibre density, i.e. the mass per unit volume of fibre
		/**
		\param iYarnType WARP,WEFT or BINDER
		*/
		void SetFibreDensity(int iYarnType, double dValue, string Units = "kg/m^3" );
		/// Set the area occupied by fibres in a yarn cross-section
		/**
		If not set it can be calculated from the yarn linear density and fibre density.
		\param iYarnType WARP,WEFT or BINDER
		*/
		void SetFibreArea(int iYarnType, double dValue, string Units = "m^2" );
		/// Set the number of fibres per yarn
		/**
		\param iYarnType WARP,WEFT or BINDER
		*/
		void SetFibresPerYarn( int iYarnType, int iNumFibres );
		/// Set the fibre diameter
		/// Set the number of fibres per yarn
		/**
		\param iYarnType WARP,WEFT or BINDER
		*/
		void SetFibreDiameter( int iYarnType, double dValue, string Units = "m" );
		/// Get the area occupied by fibres given fibre diameter and number of fibres
		/**
		Either returns the value stored in m_dFibreArea or calculates it from 
		m_iFibresPerYarn and m_FibreDiameter if they are set OR from
		m_dYarnLinearDensity and m_dFibreDensity if they are set.
		*/
		double GetFibreArea( int iYarnType, string Units );

		/// Set the width of all the yarns in the textile
		void SetYarnWidths(double dWidth);
		/// Set the height of all the yarns in the textile
		void SetYarnHeights(double dHeight);
		/// Set the spacing of all the yarns in the textile
		void SetYarnSpacings(double dSpacing);


		/// Add given number of yarns parallel to the Y axis, with given index x
		void AddYLayers(int x, int iNumberLayers);
		/// Add given number of yarns parallel to the X axis, with given index y
		void AddXLayers(int y, int iNumberLayers);
		/// Add given number of yarns parallel to the Y axis
		void AddYLayers(int iNumberLayers = 1);
		/// Add given number of yarns parallel to the X axis
		void AddXLayers(int iNumberLayers = 1);
		/// Add yarn of type XYARN or NOYARN parallel to the X axis
		void AddXYarn(int y, PATTERN3D Pattern );
		/// Add yarn of type XYARN or NOTYARN parallel to the Y axis
		void AddYYarn( int x, PATTERN3D Pattern );
		/// Add yarns parallel to the X axis in warp yarn positions, no yarns in binder positions
		void AddWarpLayer();
		/// Add yarns parallel to the X axis in binder yarn positions, no yarns in warp positions
		void AddBinderLayer();
		/// Add empty layer
		void AddNoYarnLayer();
		/// Delete given number of yarns parallel to the Y axis, with given index x
		void DeleteYLayers(int x, int iNumberLayers);
		/// Delete given number of yarns parallel to the X axis, with given index y
		void DeleteXLayers(int y, int iNumberLayers);
		/// Delete given number of yarns parallel to the Y axis
		void DeleteYLayers(int iNumberLayers = 1);
		/// Delete given number of yarns parallel to the X axis
		void DeleteXLayers(int iNumberLayers = 1);
		/// Swap the positions of two yarns in the patter with given index x, y and level iLevel1, iLevel2
		void SwapPosition(int x, int y, int iLevel1, int iLevel2);

		/// Retreive the number of yarns parallel to the Y axis, with given index x
		int GetNumYLayers(int x) const;
		/// Retreive the number of yarns parallel to the X axis, with given index y
		int GetNumXLayers(int y) const;
		/// Retreive the maximum number of layers at any crossover
		int GetMaxNumLayers() const;
		/// Retrieve the number of binder layers
		virtual int GetNumBinderLayers() const;

		/// Retrieve number of warp and binder yarns and ratios
		void GetWarpYarnInfo( int& iNumBinderYarns, int& iNumWarpYarns, int& iBinderRatio, int& iWarpRatio, YARNDATA& AveWarp, YARNDATA& AveBinder ) const;
		
		/// Check if X yarn is binder or warp. Returns true if binder
		bool IsBinderYarn( int index ) const;
		/// Get the XYarn index (ie the y position) of a yarn given the yarn index. 
		int GetXYarnIndex( int YarnInd );
		
		/// Find next cell in y direction which isn't a binder yarn
		int FindNextCellIndex( int index ) const;
		/// Find next cell in x direction which isn't a noyarn
		int FindNextYCellIndex( int Starti, int j, int k ) const;
		/// Find next cell in x direction which isn't a noyarn, searching backwards
		int FindPrevYCellIndex( int Starti, int j, int k ) const;

		/// Set the number of warp yarns grouped together
		void SetWarpRatio( int iWarpRatio ); 
		/// Set the number of binder yarns grouped together
		void SetBinderRatio( int iBinderRatio ); 
		/// Setup the binder pattern from the warp and binder ratios
		void SetBinderPattern();
		/// Setup the binder pattern from a pre-specified vector
		void SetBinderPattern( vector<bool> &BinderPattern );

		/// Setup the layers given a specified number of layers
		virtual void SetupLayers( int iNumWarpLayers, int iNumWeftLayers, int iNumBinderLayers = 1 );

		/// Set the size of the gap to leave between yarns when correcting interference
		void SetGapSize(double dGapSize);

		/// Set the resolution of the yarns
		void SetResolution(int iResolution);

		/// Assign a section mesh to the weave
		void AssignSectionMesh(const CSectionMesh &SectionMesh);

		/// Get a domain which describes 6 planes triming the textile to a unit cell
		CDomainPlanes GetDefaultDomain( bool bAddedDomainHeight = true );

		/// Get the default domain and assign it as the domain
		/**
		This is the same as AssignDomain(GetDefaultDomain))
		*/
		void AssignDefaultDomain( bool bAddedDomainHeight = true );

		void SetDomainZValues();
		/// Find max and min Z values for textile
		virtual void FindMinMaxZ( double& dMinZ, double& dMaxZ );

		/// Iterates along yarn checking the angle between alternate nodes and setting up vector appropriately
		/// bYarnsIndex = true indicates WarpIndex gives index into textile yarns vector. If false gives index into x/y yarn vector
		void CheckUpVectors( int WarpIndex, bool Yarn = PATTERN3D_XYARN, bool bYarnsIndex = false ) const;

		virtual string GetDefaultName() const;
		/// Set up row of pattern cells for one weft pattern using one row of weave pattern data and the layers pattern for the weave
		/**
		\param Layers Vector defining pattern of warp layers and binder yarns
		\param Row The warp lift pattern for one weft insertion
		\param Weft Defines which weft insertion is being defines (numbered from bottom of pattern draft) 
		*/
		virtual void SetupWeftRow( vector<int>& Layers, vector<int>& Row, int NumWarps, int Weft );

		/// Adds nodes to weft yarns to avoid intersections where they pass around warps
		/// Only useful where Textile3DWeave base class is used. Inherited classes assume weft yarns are straight
		void ShapeWeftYarns() const;

		virtual void ConvertToPatternDraft( int iWeftOrder = BOTTOM_TO_TOP );
		bool NoYarnCell( vector<PATTERN3D> &Cell );

		// Accessor methods
		virtual double GetWidth() const;	///< Get the width of the unit cell
		double GetHeight() const;	///< Get the height of the unit cell
		int GetNumXYarns() const { return m_iNumXYarns; }
		int GetNumYYarns() const { return m_iNumYYarns; }
		double GetFabricThickness() const { return m_dFabricThickness; }
		double GetMaxVolFraction() const { return m_dMaxVolumeFraction; }
		double GetGapSize() const { return m_dGapSize; }
		double GetAverageYarnSpacing() const { return (GetWidth()+GetHeight())/(m_iNumXYarns+m_iNumYYarns); }
		double GetAverageYarnWidth() const;
		const vector<PATTERN3D> &GetCell(int x, int y) const;
		int GetResolution() const { return m_iResolution; }
		vector<CYarn*> GetXYarns(int iIndex);
		vector<CYarn*> GetYYarns(int iIndex);
		int GetYarnIndex(int x, int y, int z) const;
		double GetWarpPower() const { return m_dWarpPower; }
		double GetWeftPower() const { return m_dWeftPower; }
		double GetBinderPower() const { return m_dBinderPower; }
		bool GetRefine() const { return m_bRefine; }
		virtual double GetAverageWeftYarnSpacing() const { return GetWidth()/m_iNumYYarns; }
		double GetAverageWeftYarnWidth() const;
		double GetAverageWeftYarnHeight() const;
		vector<PATTERN3D> &GetCell(int x, int y);
		void SetWeftRepeat( bool WeftRepeat ) { m_bWeftRepeat = WeftRepeat; }
		CPatternDraft &GetPatternDraft() { return m_PatternDraft; }

		void CalculateReedDesign( string Filename );

	protected:
		/// Check that the weave pattern contained in m_Pattern is valid
		bool Valid() const;

		/// Build the textile
		/**
		The weave is built based on the pattern contained in m_Pattern and the data
		in m_YYarnData and m_XYarnData. The pattern is essentially a 3D array of ints,
		where 1 represents a yarn going parallel to the x axis, 0 represents
		a yarn going parallel to the y axis and 2 represents a space with no yarn. The Yarn data contains yarn width and height,
		and horizontal spacing between yarns. The spacing value corresponds to the width of the yarn plus an equal space on either side.
		There is one set of width, height and spacing variables for each set of vertically
		grouped yarns. The assumption is that all vertically grouped yarns will have the same
		values, this can easily be extended however. 

		This function is made virtual so that sub-classes may define a completely new build function
		or more likely extend this one.
		*/
		virtual bool BuildTextile() const;
		/// Adjust cross section shapes to correct interference
		/**
		Cross section shapes will be adjusted in order to remove interference, this is done
		by shrinking the position of the nodes where they interfere. The cross sections are
		replaced with polygons to provide an interference free shape.
		*/
		//void CorrectInterference() const;
		//bool NeedsMidSection(int iYarn, int iSection) const;

		
		/// Gets the cross section of the weft
		/**  Currently assumes is a constant power elliptical cross section 
		*/
		CSectionPowerEllipse* GetWeftCrossSection( int WeftYarnIndex ) const;
		/// Gets the cross-section of a yarn, either constant or interp node, power elliptical sections
		CSection* GetCrossSection( int YarnIndex, int Node = -1 ) const;
		
		/// Gets an up vector for a pair of nodes
		XYZ GetUpVector( XYZ CheckNodes[], bool bYarn ) const;
		/// Calculates a point at given point on power ellipse cross section and inserts into binder yarn
		/**
		\param YarnSection The cross section of the weft yarn
		\param t Varies from 0 to 1 and gives the distance around the circumference 
		\param bInsert Insert node if true, replace node if false
		*/
		void InsertBinderNode( CSectionPowerEllipse* YarnSection, double t, XYZ& WeftNode, int& CurrentNode, int BinderYarnIndex, double Offset, bool bInsert = true ) const;

		/// Calculates a point at given point on cross section and inserts into binder yarn
		/**	
		\param YarnSection The cross section of the weft yarn
		\param t Varies from 0 to 1 and gives the distance around the circumference 
		\param bInsert Insert node if true, replace node if false
		*/
		void InsertBinderNode( CSection* YarnSection, double t, XYZ& WeftNode, int& CurrentNode, int BinderYarnIndex, double Offset, bool bInsert = true ) const;

		/// Replace last node with node offset to match node 0
		void ReplaceLastNode( int BinderYarnIndex, XYZ& NewNode, XYZ& BinderNode ) const;
		/// Adjust section to target height whilst maintaining cross-sectional area
		/**
		If width required to maintain area exceeds MaxWidth, sets Width to MaxWidth and returns false
		Returns true if achieve area and width < MaxWidth
		*/
		bool AdjustPowerEllipseSectionWidth( double &HeightReduction, double &MaxWidth, double TargetArea, CSectionPowerEllipse* YarnSection ) const;

		void FindPowerEllipseSectionHeight( double& MaxHeight, double& TargetArea, CSectionPowerEllipse* YarnSection ) const;

		bool AdjustPowerEllipsePower( double &HeightReduction, double TargetArea, CSectionPowerEllipse* YarnSection, double MinPower ) const;

		void ReducePowerEllipseHeight( double& TargetArea, CSectionPowerEllipse* YarnSection ) const;

		void SetYarnProperties( CYarn& Yarn, int iType ) const;

		pair<int, int> GetCellCoordinates(int iIndex) const;

		int FindWeftHeight( const vector<PATTERN3D>& Cell ) const;

		int AddWeftNodes( int CurrentNode, int XNode, int i, int j ) const;

		void InsertWeftNode( CSection* YarnSection, double t, XYZ& WarpNode, int& CurrentNode, int WeftYarnIndex, double Offset, bool bInsert = true ) const;

		int FindWarpAboveIndex( const vector<PATTERN3D> &Cell, int iIndex ) const;
		int FindWarpBelowIndex( const vector<PATTERN3D> &Cell, int iIndex ) const;

		void RemoveCell( int x, int y);
		/// Check that all cells are populated
		bool CheckCells() const;

		/// Check if any binder yarns in weave
		bool BinderYarns() const;
		
		/// Class for generating pattern draft from weave pattern
		CPatternDraft m_PatternDraft;

		int m_iNumXYarns, m_iNumYYarns;
		int m_iWarpRatio, m_iBinderRatio;
		vector<bool> m_BinderPattern;
		int m_iTotalXYarns;
		vector<vector<PATTERN3D> > m_Pattern;
		double m_dGapSize;
		double m_dFabricThickness;
		double m_dMaxVolumeFraction;
		int m_iResolution;
		CObjectContainer<CSectionMesh> m_pSectionMesh;
		mutable double m_dMinZ;
		mutable double m_dMaxZ;
		double m_dWarpPower, m_dWeftPower, m_dBinderPower;
		bool m_bRefine;
		CProperties m_Properties[3];
		bool m_bWeftRepeat;

		mutable vector<YARNDATA> m_XYarnData;
		mutable vector<YARNDATA> m_YYarnData;
		mutable vector<vector<int> > m_XYarns;
		mutable vector<vector<int> > m_YYarns;
	};
};	// namespace TexGen







