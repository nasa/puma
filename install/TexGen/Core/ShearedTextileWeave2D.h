/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2013 Louise Brown

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
#include "TextileWeave2D.h"

namespace TexGen
{ 
	class CSectionPolygon;
	using namespace std;

	/// Respresents a 2d woven textile with shear applied
	/** Parameters are input for the unsheared textile and the shear is then applied.
		This results in reduction in spacing in the y direction
	**/
	class CLASS_DECLSPEC CShearedTextileWeave2D : public CTextileWeave2D
	{
	public:
		/// Build a 2d weave unit cell of given width, height, yarn spacing and fabric thickness
		/**
		\param dThickness  Textile thickness
		\param ShearAngle  Angle in radians
		*/
		CShearedTextileWeave2D(int iWidth, int iHeight, double dSpacing, double dThickness, double ShearAngle, bool bRefine = true, bool bInPlaneTangents = true);
		CShearedTextileWeave2D(TiXmlElement &Element);
		~CShearedTextileWeave2D(void);

		CTextile* Copy() const { return new CShearedTextileWeave2D(*this); }
		string GetType() const { return "CShearedTextileWeave2D"; }
		void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		/// Creates the domain as a box if bSheared = false
		/// Planes parallel to the y yarns (ie oriented at the shear angle) if bSheared = true
		CDomainPlanes GetDefaultDomain( bool bSheared = false, bool bAddedDomainHeight = true );

		double GetShearAngle() const { return m_ShearAngle; }
		bool GetShearedDomain() const { return m_bShearedDomain; }

	protected:
		bool BuildTextile() const;
		/// Get the x and y components of spacings along y yarns
		void GetYSpacings() const;
		/// Sums all y components of spacings to get y repeat
		XY GetYRepeat() const;

		void Refine() const;
		bool AdjustSectionsForRotation() const;
		double CalcSinAngle( double R, double Width, double Height ) const;

		/// Adjust cross section shapes to correct interference
		/**
		Cross section shapes will be adjusted in order to remove interference, this is done
		by shrinking the position of the nodes where they interfere. The cross sections are
		replaced with polygons to provide an interference free shape.
		\param bMaxCorrection Corrects by whole of modifier if true, by half of modifier if false (Creates gaps with large intersections if set to true)
		*/
		void CorrectInterference( bool bMaxCorrection = false ) const;
		void CalculateModifiers( const CInterpolation *pInterpolation, int Yarn, YARN_POSITION_INFORMATION YarnPosInfo, CMesh& Mesh, vector<vector<vector<double> > >& YarnSectionModifiers, bool bMaxCorrection) const;
		void ModifySection(CYarnSectionInterpNode* pYarnSection, YARN_POSITION_INFORMATION& YarnPosInfo, vector<double>& Modifiers, int iNodeIndex, int iMidIndex = -1) const;

		void AdjustSections() const;
		/// Create meshes for top and bottom surfaces of domain
		void CreateTopAndBottomMeshes( CMesh& TopMesh, CMesh& BottomMesh ) const;
		void CalculateModifiers( const CInterpolation *pInterpolation, int Yarn, YARN_POSITION_INFORMATION YarnPosInfo, CMesh& Mesh, vector<double>& YarnSectionModifiers) const;
		XY Get2DRefPoint( XY& Point, double dAngle ) const;
		XY RotatePoint( XY& Point, double dAngle ) const;
		void ModifySection(CYarnSectionInterpNode* pYarnSection, YARN_POSITION_INFORMATION& YarnPosInfo, vector<double>& Modifiers, double dTargetArea, double dOriginalHeight, int iNodeIndex, int iMidIndex = -1) const;
		double GetArea( vector<XY>& Points ) const;
		void ReplaceSection( vector<XY>& Points, CYarnSectionInterpNode* pYarnSection, int iNodeIndex, int iMidIndex ) const;
		/// Swap x and y in GetCell call if transverse yarn is x yarn
		const vector<PATTERN2D> &GetCellDir( int x, int y, int iDir ) const;
		int ModifyPoints( vector<XY>& Points, vector<double>& Modifiers, vector<XY>& MaxHeight, int iModCount, double dy, vector<XY>& NewPoints ) const;

		double m_ShearAngle;
		/// Vector containing the dx and dy components for spacings along y yarns
		mutable vector<XY> m_YSpacing;
		bool m_bShearedDomain;
	};
};	// namespace TexGen

