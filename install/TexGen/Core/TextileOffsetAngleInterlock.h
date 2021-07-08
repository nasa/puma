/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2011 Louise Brown
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
#include "Textile3DWeave.h"
#include "SectionPowerEllipse.h"

namespace TexGen
{ 
	using namespace std;

	
	/// Represents a 3D angle interlock woven textile
	class CLASS_DECLSPEC CTextileOffsetAngleInterlock : public CTextile3DWeave
	{
	public:

		/// Build a weave unit cell of given width, height, number of layers, yarn spacing and fabric thickness
		CTextileOffsetAngleInterlock(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing,
							double dXHeight, double dYHeight);
		CTextileOffsetAngleInterlock(TiXmlElement &Element);
		virtual ~CTextileOffsetAngleInterlock(void);

		virtual CTextile* Copy() const { return new CTextileOffsetAngleInterlock(*this); }
		virtual string GetType() const { return "CTextileOffsetAngleInterlock"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		void SetBinderYarnPositions( int x, int y );
		void AddYLayer( bool bOffset );

		/// Find min and max z values for the textile
		virtual void FindMinMaxZ( double& dMinZ, double& dMaxZ );
		virtual void SetupLayers( int iNumWarpLayers, int iNumWeftLayers, int iNumBinderLayers = 1 );
		virtual bool BuildTextile() const;
		virtual double GetWidth() const;	///< Get the width of the unit cell

		virtual double GetAverageWeftYarnSpacing() const { return (GetWidth()/m_iNumYYarns) * 2.0; }

	protected:
		/// Shape the binder yarns around the adjacent weft yarns
		void ShapeBinderYarns() const;
		/// Add extra nodes to binder yarns to match shape of adjacent weft yarns
		int AddBinderNodes( int CurrentNode, int i, int j ) const;

		void SetBinderPosition( int z, vector<PATTERN3D>& Cell );
		int FindBinderHeight( const vector<PATTERN3D>& Cell ) const;
	};
};	// namespace TexGen