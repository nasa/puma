/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2014 Louise Brown

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

namespace TexGen
{ 
	using namespace std;

	/// Represents a textile made up from several layers of weaves
	class CLASS_DECLSPEC CTextileLayered : public CTextile
	{
	public:

		/// Build a weave unit cell of given width, height, yarn spacing and fabric thickness
		CTextileLayered();
		CTextileLayered(TiXmlElement &Element);
		virtual ~CTextileLayered(void);

		virtual CTextile* Copy() const { return new CTextileLayered(*this); }
		virtual string GetType() const { return "CTextileLayered"; }
		virtual void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType);
		string GetDefaultName() const;

		const vector< vector<int> > GetLayerYarnIndices() { return m_LayerYarnIndices; }
		/// Sets an xy offset for each layer
		void SetOffsets( vector<XY> &Offsets );
		/// Sets an xy offset for the layer, relative to the existing offset
		void SetRelativeOffsets( vector<XY> &Offsets );
		/// Sets a constant offset between layers
		void SetOffsets( XY &Offset );
		const vector<XY> &GetOffsets() const {return m_LayerOffset; } 

		int GetNumLayers();

		/// Adds the layers from the textile passed, maintaining the existing offset of each layer
		void AddLayer( CTextile& Textile, XYZ& Offset );
		void AddLayer( vector<CYarn> &Yarns, XYZ& Offset );

		void NestLayers();
		void MaxNestLayers();

	protected:
		/// Applies a set of offsets to the yarns. This is an absolute value - if there is already
		/// an offset in place this will be removed before the new offset is applied
		void ApplyOffsets( vector<XY> &Offsets );
		void ApplyLayerOffset( XYZ &Offset, int iLayer );
		void GetOffsetMinDist( int iOffset, vector< vector<pair<double,double> > >& LayerIntersections, vector<pair<int,double> >& MinDist, int iNumX, int iNumY);
		void GetOffsetMinDist( int x, int y, int iLayer, vector< vector<pair<double,double> > >& LayerIntersections, vector<pair<int,double> >& MinDist, int iNumX, int iNumY, int XSize, int YSize, bool bOffsetTop);
		int GetLayerMeshes( vector<CMesh>& LayerMeshes );
		void GetLayerRepeats( vector< XY >& Repeats );

		double GreatestCommonDenominator( double X1, double X2 );

		vector<vector<int> > m_LayerYarnIndices;
		vector<XY> m_LayerOffset;
		int m_iResolution;
	};
};	// namespace TexGen