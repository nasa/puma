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
#include "PatternDraft.h"

namespace TexGen
{ 
	using namespace std;

	class CPatternDraft;

	enum
	{
		PATTERN_XYARN = 1,
		PATTERN_YYARN = 0,
	};

	typedef bool PATTERN2D;

	/// Represents a woven textile
	class CLASS_DECLSPEC CTextileWeave : public CTextile
	{
	public:

		/// Build a weave unit cell of given width, height, yarn spacing and fabric thickness
		CTextileWeave(int iNumYYarns, int iNumXYarns, double dSpacing, double dThickness);
		CTextileWeave(TiXmlElement &Element);
		virtual ~CTextileWeave(void);

		virtual CTextile* Copy() const { return new CTextileWeave(*this); }
		virtual string GetType() const { return "CTextileWeave"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		/// Set the thickness of the fabric
		/**
		\param dThickness New fabric thickness to assign
		\param bAdjustYarnHeights If true then yarn heights will be set to half the new thickness
		*/
		void SetThickness(double dThickness, bool bAdjustYarnHeights = true);
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
		/// Set the height of all the yarns parallel to the X axis
		void SetXYarnHeights(double dHeight);
		/// Set the height of all the yarns parallel to the Y axis
		void SetYYarnHeights(double dHeight);
		/// Set the spacing of all the yarns parallel to the X axis
		void SetXYarnSpacings(double dSpacing);
		/// Set the spacing of all the yarns parallel to the Y axis
		void SetYYarnSpacings(double dSpacing);

		/// Set the width of all the yarns in the textile
		void SetYarnWidths(double dWidth);
		/// Set the height of all the yarns in the textile
		void SetYarnHeights(double dHeight);
		/// Set the spacing of all the yarns in the textile
		void SetYarnSpacings(double dSpacing);

		/// Set the size of the gap to leave between yarns when correcting interference
		void SetGapSize(double dGapSize);

		/// Set the resolution of the yarns
		void SetResolution(int iResolution);

		/// Assign a section mesh to the weave
		void AssignSectionMesh(const CSectionMesh &SectionMesh);

		/// Get a domain which describes 6 planes triming the textile to a unit cell
		virtual CDomainPlanes GetDefaultDomain( bool bSheared = false, bool bAddedHeight = true );

		/// Get the default domain and assign it as the domain
		/**
		This is the same as AssignDomain(GetDefaultDomain))
		*/
		void AssignDefaultDomain( bool bSheared = false, bool bAddedHeight = true );

		// Flatten the yarn
		/**
		 * Move the points of the cross section that lie
		 * more than FlatLevel away from the yarn center (and away from the
		 * weave center)
		 * \param dFlatLevel distance from the yarn center 
		 * \param iUpDownBoth -1 if only flattening lower passages, 0 for both, 1
		 * for upper passages
		 */
		bool FlattenYarns(double dFlatLevel, int iUpDownBoth=0);

		virtual string GetDefaultName() const;

		/// Call refine functions for textile weave.  Currently only defined for 2D weave to allow refinement after node adjustment in textile
		virtual void RefineTextile( bool bCorrectWidths = true, bool bCorrectInterference = true, bool bPeriodic = true ){};
		void CorrectEdgeInterference();

		virtual void ConvertToPatternDraft( /*CPatternDraft& PatternDraft*/ ) {};

		// Accessor methods
		double GetWidth() const;	///< Get the width of the unit cell
		double GetHeight() const;	///< Get the height of the unit cell
		int GetNumXYarns() const { return m_iNumXYarns; }
		int GetNumYYarns() const { return m_iNumYYarns; }
		double GetFabricThickness() const { return m_dFabricThickness; }
		double GetGapSize() const { return m_dGapSize; }
		double GetAverageYarnSpacing() const { return (GetWidth()+GetHeight())/(m_iNumXYarns+m_iNumYYarns); }
		double GetAverageYarnWidth() const;
		const vector<PATTERN2D> &GetCell(int x, int y) const;
		int GetResolution() const { return m_iResolution; }
		vector<CYarn*> GetXYarns(int iIndex);
		vector<CYarn*> GetYYarns(int iIndex);
		int GetYarnIndex(int x, int y, int z) const;
		CPatternDraft &GetPatternDraft() { return m_PatternDraft; }

	protected:
		/// Check that the weave pattern contained in m_Pattern is valid
		bool Valid() const;

		/// Build the textile
		/**
		The weave is built based on the pattern contained in m_Pattern and the data
		in m_YYarnData and m_XYarnData. The pattern is essentially a 3D array of bools,
		where true represents a yarn going parallel to the x axis and false representing
		a yarn going parallel to the y axis. The Yarn data contains yarn width and height,
		and horizontal spacing between yarns. The spacing value corresponds to the spacing
		between the centerline of the yarn and the centerline of the next yarn after it.
		There is one set of width, height and spacing variables for each set of vertically
		grouped yarns. The assumption is that all vertically grouped yarns will have the same
		values, this can easily be extended however. The fabric thickness determines the vertical
		spacing, the spacing between yarns vertically is the fabric thickness divided by the number
		of yarns crossing over each other. Another limitation here is that the vertical spacing
		is the same between all vertical yarns. Again something that could easily be extended.

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
		virtual void CorrectInterference() const;
		bool NeedsMidSection(int iYarn, int iSection) const;

		/// Adjust cross section widths to avoid interference
		/**
		Find out the maximum yarn width to avoid interference leaving a gap of given size.
		This should be called before correct interference, if correct interference is called
		with too large yarn widths the results will not be good.
		*/
		void CorrectYarnWidths() const;

		/// Class for generating pattern draft from weave pattern
		CPatternDraft m_PatternDraft;

		pair<int, int> GetCellCoordinates(int iIndex) const;
		vector<PATTERN2D> &GetCell(int x, int y);

		/// Data structure to keep track of yarn parameters
		struct YARNDATA
		{
			double dWidth;
			double dHeight;
			double dSpacing;
		};

		int m_iNumXYarns, m_iNumYYarns;
		vector<vector<PATTERN2D> > m_Pattern;
		double m_dGapSize;
		double m_dFabricThickness;
		int m_iResolution;
		CObjectContainer<CSectionMesh> m_pSectionMesh;

		mutable vector<YARNDATA> m_XYarnData;
		mutable vector<YARNDATA> m_YYarnData;
		mutable vector<vector<int> > m_XYarns;
		mutable vector<vector<int> > m_YYarns;
	};
};	// namespace TexGen








