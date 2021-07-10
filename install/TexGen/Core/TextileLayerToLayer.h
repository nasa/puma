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

	
	/// Represents a 3D layer to layer woven textile
	class CLASS_DECLSPEC CTextileLayerToLayer : public CTextile3DWeave
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
		\param iNumBinderLayers  The number of binder layers alternating with wefts in the stack
		*/ 
		CTextileLayerToLayer(int iNumXYarns, int iNumYYarns, double dXSpacing, double dYSpacing,
							double dXHeight, double dYHeight, int iNumBinderLayers, bool bShapeBinders = true);
		CTextileLayerToLayer(TiXmlElement &Element);
		virtual ~CTextileLayerToLayer(void);

		virtual CTextile* Copy() const { return new CTextileLayerToLayer(*this); }
		virtual string GetType() const { return "CTextileLayerToLayer"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);

		/// Set up the sequence of layers in the textile for the required number of warp, weft and binder yarn layers 
		/**
		\param iNumWarpLayers Number of warps in the stack
		\param iNumWeftLayers Number of wefts in the stack
		\param iNumBinderLayers Number of binder yarns layered between weft yarns
		*/
		virtual void SetupLayers( int iNumWarpLayers, int iNumWeftLayers, int iNumBinderLayers = 1 );
		/// Build the textile
		virtual bool BuildTextile() const;
		/// Sets the vertical positions of the binder yarns.  
		/**\param zOffset The number of binder positions offset from the top of the textile
		*/
		void SetBinderPosition( int x, int y, int zOffset );
		/// Move the stack of binder yarns by one position
		/**
		Parameters iLevel1 and iLevel2 are used to determine whether the yarn is 
		being moved up or down.
		*/
		void MoveBinderYarns( int x, int y, int iLevel1, int iLevel2 );
		/// Finds the offset position of the top binder yarn at the given x,y position
		int GetBinderOffset( int x, int y );
		/// Returns number of binder layers in textile
		virtual int GetNumBinderLayers() const;
		/// Shape the binder yarns around the adjacent weft yarns
		void ShapeBinderYarns() const;

		virtual void ConvertToPatternDraft( int iWeftOrder = BOTTOM_TO_TOP );

	protected:
		/// Finds cell index of the top binder yarn
		int FindTopBinderYarn( vector<PATTERN3D>& Cell );
		
		/// Add extra nodes to binder yarns to match shape of adjacent weft yarns
		int AddBinderNodes( int CurrentNode, int i, int j, int Height ) const;

		int FindBinderHeight( const vector<PATTERN3D>& Cell, int Height ) const;
		
		int m_iNumBinderLayers;
		bool m_bShapeBinders;
	};
};	// namespace TexGen