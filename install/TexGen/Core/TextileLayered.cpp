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

#include "PrecompiledHeaders.h"
#include "TextileLayered.h"

using namespace TexGen;

#define START_DIST -1.0e+6

CTextileLayered::CTextileLayered()
: m_iResolution(40)
{
}

CTextileLayered::~CTextileLayered(void)
{
}

CTextileLayered::CTextileLayered(TiXmlElement &Element)
: CTextile(Element)
{
	Element.Attribute("Resolution", &m_iResolution);
	FOR_EACH_TIXMLELEMENT(pLayer, Element, "Layer")
	{
		vector<int> Indices;
		FOR_EACH_TIXMLELEMENT(pYarn, *pLayer, "Yarn")
		{
			Indices.push_back(valueify<int>(pYarn->Attribute("yarnindex")));
		}
		m_LayerYarnIndices.push_back(Indices);
	}
	FOR_EACH_TIXMLELEMENT(pOffset, Element, "Offset")
	{
		m_LayerOffset.push_back(valueify<XY>(pOffset->Attribute("value")));
	}
}

void CTextileLayered::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CTextile::PopulateTiXmlElement(Element, OutputType);

	Element.SetAttribute("Resolution", m_iResolution);

	for (int i=0; i<(int)m_LayerYarnIndices.size(); ++i)
	{
		TiXmlElement Layer("Layer");
		//YYarns.SetAttribute("index", i);
		for (int j=0; j<(int)m_LayerYarnIndices[i].size(); ++j)
		{
			TiXmlElement Yarn("Yarn");
			Yarn.SetAttribute("yarnindex", m_LayerYarnIndices[i][j]);
			Layer.InsertEndChild(Yarn);
		}
		Element.InsertEndChild(Layer);
	}
	vector<XY>::iterator itXY;
	for (itXY = m_LayerOffset.begin(); itXY!=m_LayerOffset.end(); ++itXY)
	{
		TiXmlElement Offset("Offset");
		Offset.SetAttribute("value", stringify(*itXY));
		Element.InsertEndChild(Offset);
	}
}

void CTextileLayered::AddLayer(CTextile& Textile, XYZ& Offset)
{
	if ( Textile.GetType() == "CTextileLayered" )
	{
		const vector< vector<int> > YarnIndices = dynamic_cast<CTextileLayered*>(&Textile)->GetLayerYarnIndices();
		vector<XY> Offsets = dynamic_cast<CTextileLayered*>(&Textile)->GetOffsets();
		vector< vector<int> >::const_iterator itYarnIndices;
		int i;

		//  Add each layer in turn to the new layered textile
		for( itYarnIndices = YarnIndices.begin(), i = 0; itYarnIndices != YarnIndices.end(); ++itYarnIndices, ++i )
		{
			vector<int>::const_iterator itIndex;
			vector<CYarn> Yarns;
			for ( itIndex = itYarnIndices->begin(); itIndex != itYarnIndices->end(); ++itIndex )
			{
				CYarn* pYarn = Textile.GetYarn( *itIndex );
				Yarns.push_back( *pYarn );
			}
			XY LayerOffset(Offsets[i].x, Offsets[i].y);
			AddLayer( Yarns, Offset );
			m_LayerOffset.push_back( LayerOffset );
		}
	}
	else
	{
		vector<CYarn> &Yarns = Textile.GetYarns();
		AddLayer( Yarns, Offset );
		m_LayerOffset.push_back( XY(Offset.x, Offset.y) );
	}
}

void CTextileLayered::AddLayer( vector<CYarn> &Yarns, XYZ& Offset )
{
	vector<CYarn>::iterator itYarn;
	vector<int> YarnIndices;  // Vector of yarn indices (within the textile) for the layer
	XY LayerOffset(Offset.x, Offset.y);

	// Add each yarn with specified offset
	for ( itYarn = Yarns.begin(); itYarn != Yarns.end(); ++itYarn )
	{
		CYarn Yarn = *itYarn;
		Yarn.Translate( Offset );

		int index = AddYarn( Yarn );
		YarnIndices.push_back( index );  
	}
	m_LayerYarnIndices.push_back( YarnIndices );
}

string CTextileLayered::GetDefaultName() const
{
	return "LayeredTextile(Layers:" + stringify(m_LayerYarnIndices.size()) + ")";
}

void CTextileLayered::SetOffsets( vector<XY> &Offsets )
{
	int size = m_LayerYarnIndices.size();
	if ( Offsets.size() != size )
	{
		TGERROR("Can't assign offsets: number of offsets does not match number of layers");
		return;
	}

	if ( m_LayerOffset.empty() )
	{
		m_LayerOffset.assign( Offsets.begin(), Offsets.end() );
		ApplyOffsets( m_LayerOffset );
	}
	else
	{
		vector<XY> MoveOffset;
		// Calculate relative offset and translate yarns by that offset
		for ( int i = 0; i < size; ++i )
		{
			MoveOffset.push_back( Offsets[i] - m_LayerOffset[i] );
		}
		ApplyOffsets( MoveOffset );
		// Set offsets to new values
		m_LayerOffset.assign( Offsets.begin(), Offsets.end() );
	}
}

void CTextileLayered::SetRelativeOffsets( vector<XY> &Offsets )
{
	int size = m_LayerYarnIndices.size();
	if ( Offsets.size() != size )
	{
		TGERROR("Can't assign offsets: number of offsets does not match number of layers");
		return;
	}

	if ( m_LayerOffset.empty() )
	{
		m_LayerOffset.assign( Offsets.begin(), Offsets.end() );
		ApplyOffsets( m_LayerOffset );
	}
	else
	{
		vector<XY> TotalOffset;
		// Calculate relative offset and translate yarns by
		for ( int i = 0; i < size; ++i )
		{
			TotalOffset.push_back( Offsets[i] + m_LayerOffset[i] );
		}
		ApplyOffsets( Offsets );
		// Set offsets to new values
		m_LayerOffset.assign( TotalOffset.begin(), TotalOffset.end() );
	}
}

void CTextileLayered::SetOffsets( XY &Offset )
{
	int size = m_LayerYarnIndices.size();
	vector<XY> Offsets;
	XY SumOffset;

	for( int i = 0; i < size; ++i )
	{
		Offsets.push_back( SumOffset );
		SumOffset += Offset;
	}

	SetOffsets( Offsets );
}

void CTextileLayered::ApplyOffsets( vector<XY> &Offsets )
{
	int iNumLayers = m_LayerYarnIndices.size();
	if ( Offsets.size() != iNumLayers )
	{
		TGERROR("Can't apply offsets: number of offsets does not match number of layers");
	}

	for ( int i = 0; i < iNumLayers; ++i )
	{
		vector<int>::iterator itIndices;
		XYZ Offset( Offsets[i].x, Offsets[i].y, 0 );
		ApplyLayerOffset( Offset, i );
	}
}

void CTextileLayered::ApplyLayerOffset( XYZ &Offset, int iLayer )
{
	if ( iLayer < 0 || iLayer >= m_LayerYarnIndices.size() )
		return;

	vector<int>::iterator itIndices;

	for ( itIndices = m_LayerYarnIndices[iLayer].begin(); itIndices != m_LayerYarnIndices[iLayer].end(); ++itIndices )
	{
		CYarn* Yarn = GetYarn( *itIndices );
		Yarn->Translate( Offset );
	}
}

int CTextileLayered::GetNumLayers()
{
	return (int)m_LayerYarnIndices.size();
}

void CTextileLayered::NestLayers()
{
	if ( !m_pDomain )
	{
		TGERROR( "Cannot nest layers: no domain specified" );
		return;
	}
	int iNumLayers = (int)m_LayerYarnIndices.size();
	vector<CMesh> LayerMeshes;
	int iNumX, iNumY;
	// Create mesh for each layer
	iNumX = GetLayerMeshes( LayerMeshes );
	iNumY = iNumX;

#ifdef _DEBUG
	int i = 0;
	vector<CMesh>::iterator itMeshes;
	for ( itMeshes = LayerMeshes.begin(); itMeshes != LayerMeshes.end(); ++itMeshes )
	{
		string str = "LayerMeshes" + stringify(i++);
		itMeshes->SaveToVTK(str.c_str() );
	}
#endif

	pair<XYZ,XYZ> AABB = m_pDomain->GetMesh().GetAABB();
	double XInc = (AABB.second.x - AABB.first.x) / iNumX;
	double YInc = (AABB.second.y - AABB.first.y) / iNumY;

	vector< vector<pair<double,double> > > LayerIntersections;
	LayerIntersections.resize((iNumX+1) * (iNumY+1));
	XY Pos(AABB.first.x, AABB.first.y);
	double Height = GetLength( XYZ(Pos.x, Pos.y, AABB.first.z), XYZ(Pos.x, Pos.y, AABB.second.z) );

	// Find intersections of grid of vertical lines with yarn meshes
	for ( int j = 0; j <= iNumY; ++j )
	{
		for ( int i = 0; i <= iNumX; ++i )
		{
			vector<CMesh>::iterator itMeshes;
			XYZ P1(Pos.x,Pos.y,AABB.first.z);
			XYZ P2(Pos.x,Pos.y,AABB.second.z);
			for ( itMeshes = LayerMeshes.begin(); itMeshes != LayerMeshes.end(); ++itMeshes )
			{
				vector<pair<double,XYZ> > IntersectionPoints;
				int iNum = itMeshes->IntersectLine(P1, P2, IntersectionPoints, make_pair(true,true));
				if ( iNum >= 2 )
				{
					pair<double,double> Intersects;
					Intersects.first = IntersectionPoints[0].first * Height;  // Bottom mesh intersection for layer
					Intersects.second = IntersectionPoints[iNum-1].first * Height;  // Top mesh intersection for layer
					LayerIntersections[j*(iNumX+1)+i].push_back(Intersects);
				}
				else
				{
					LayerIntersections[j*(iNumX+1)+i].push_back(make_pair(START_DIST,START_DIST));  // Set to -1 if no intersections in layer
				}
			}
			Pos.x += XInc;
		}
		Pos.y += YInc;
		Pos.x = AABB.first.x;
	}

	vector<double> MinDist(iNumLayers-1,START_DIST);

	// Find the smallest distance between adjacent layers
	for ( int j = 0; j < LayerIntersections.size(); ++j )
	{	
		for ( int i = 0; i < LayerIntersections[j].size()-1; ++i )
		{
			if ( LayerIntersections[j][i].first > 0.0 && LayerIntersections[j][i+1].second > 0.0 )
			{
				double dist = LayerIntersections[j][i+1].first - LayerIntersections[j][i].second;
				/*if ( MinDist[i] < 0.0 )
				{
					MinDist[i] = dist;
				}*/
				//else if ( dist < MinDist[i] )
				if ( MinDist[i] == START_DIST || dist < MinDist[i] )
				{
					MinDist[i] = dist;
				}
			}
		}
	}

	// Move layers together by smallest distances between layers
	XYZ Offset;
	for ( int i = 1; i < iNumLayers; ++i )
	{
		Offset.z -= MinDist[i-1];
		ApplyLayerOffset( Offset, i );
	}

	// Resize domain
	PLANE Plane;
	CDomainPlanes *DomainPlanes = (CDomainPlanes*)&(*m_pDomain);  // Convoluted - need to see if better way of doing this
	XYZ RefPlane(0,0,-1);
	int index = DomainPlanes->GetPlane( RefPlane, Plane );
	Plane.d -= Offset.z;
	DomainPlanes->SetPlane( index, Plane );
	m_bNeedsBuilding = true;
}

void CTextileLayered::MaxNestLayers()
{
	if ( !m_pDomain )
	{
		TGERROR( "Cannot nest layers: no domain specified" );
		return;
	}
	int iNumLayers = (int)m_LayerYarnIndices.size();
	vector<CMesh> LayerMeshes;
	int iNumX, iNumY;
	// Initial grid resolution is based on number of slave nodes
	int iGridRes = GetLayerMeshes( LayerMeshes );

#ifdef _DEBUG
	int i = 0;
	vector<CMesh>::iterator itMeshes;
	for ( itMeshes = LayerMeshes.begin(); itMeshes != LayerMeshes.end(); ++itMeshes )
	{
		string str = "LayerMeshes" + stringify(i++);
		itMeshes->SaveToVTK(str.c_str() );
	}
#endif

	pair<XYZ,XYZ> AABB = m_pDomain->GetMesh().GetAABB();

	vector<XY> Repeats;
	GetLayerRepeats( Repeats );

	vector<XY> Offsets;
	XY Offset;
	Offsets.push_back( Offset );

	// Store the offset position which gives the minimum distance between each pair of layers
	vector<pair<int,double> > MinDist(iNumLayers-1,make_pair(-1,START_DIST));

	// Find min dist for each pair of layers
	for ( int iLayer = 0; iLayer < LayerMeshes.size()-1; ++iLayer )
	{

		double X1 = Repeats[iLayer].x;
		double X2 = Repeats[iLayer+1].x;
		double Y1 = Repeats[iLayer].y;
		double Y2 = Repeats[iLayer+1].y;

		// Need to have integer number of offsets for the algorithm to work
		double gcd = GreatestCommonDenominator( X1, X2 );
		int fact = (int)(iGridRes/((AABB.second.x - AABB.first.x)/gcd));
		double XInc = gcd/fact;

		gcd = GreatestCommonDenominator( Y1, Y2 );
		fact = (int)(iGridRes/((AABB.second.y - AABB.first.y)/gcd));
		double YInc = gcd/fact;

		iNumX = (AABB.second.x - AABB.first.x)/XInc;
		iNumY = (AABB.second.y - AABB.first.y)/YInc;

		// Set the upper and lower intersection points of the mesh with a grid of vertical lines for each of the pair of meshes
		vector< vector<pair<double,double> > > LayerIntersections;
		LayerIntersections.resize((iNumX+1) * (iNumY+1));
		XY Pos(AABB.first.x, AABB.first.y);
		double Height = GetLength( XYZ(Pos.x, Pos.y, AABB.first.z), XYZ(Pos.x, Pos.y, AABB.second.z) );

		for ( int j = 0; j <= iNumY; ++j )
		{
			for ( int i = 0; i <= iNumX; ++i )
			{
				vector<CMesh>::iterator itMeshes;
				XYZ P1(Pos.x,Pos.y,AABB.first.z);
				XYZ P2(Pos.x,Pos.y,AABB.second.z);
				
				for ( int k = 0; k < 2; ++k )
				{
					vector<pair<double,XYZ> > IntersectionPoints;
					int iNum = LayerMeshes[iLayer+k].IntersectLine(P1, P2, IntersectionPoints, make_pair(true,true));
					if ( iNum >= 2 )
					{
						pair<double,double> Intersects;
						Intersects.first = IntersectionPoints[0].first * Height;
						Intersects.second = IntersectionPoints[iNum-1].first * Height;
						LayerIntersections[j*(iNumX+1)+i].push_back(Intersects);
					}
					else
					{
						LayerIntersections[j*(iNumX+1)+i].push_back(make_pair(START_DIST,START_DIST));
					}
				}
				Pos.x += XInc;
			}
			Pos.y += YInc;
			Pos.x = AABB.first.x;
		}
		
		// Which mesh is offset depends on which has the larger repeat
		int XSize = (int)(min(Repeats[iLayer].x, Repeats[iLayer+1].x)/XInc);
		int YSize = (int)(min(Repeats[iLayer].y, Repeats[iLayer+1].y)/YInc);
		bool bOffsetTop = Repeats[iLayer].x < Repeats[iLayer+1].x ? true : false;
		
		// Move the layers relative to each other for each of the set of grid positions and find minimum distance between the meshes
		for ( int j = 0; j < (int)YSize; ++j )
		{
			for ( int i = 0; i < (int)XSize; ++i )
			{
				GetOffsetMinDist(i, j, iLayer, LayerIntersections, MinDist, iNumX+1, iNumY+1, XSize, YSize, bOffsetTop);
			}
		}
	
		int X = MinDist[iLayer].first % (iNumX+1);
		int Y = (MinDist[iLayer].first - X) / (iNumX+1);

		if ( bOffsetTop )
		{
			X *= -1.0;
			Y *= -1.0;
		}
		Offset.x += X * XInc;
		Offset.y += Y * YInc;
		Offsets.push_back( Offset );
	}

	SetRelativeOffsets( Offsets );

	// Move layers together by smallest distances between layers
	XYZ ZOffset;
	for ( int i = 1; i < iNumLayers; ++i )
	{
		TGLOG("Mindist = " << MinDist[i-1].second );
		ZOffset.z -= MinDist[i-1].second;
		ApplyLayerOffset( ZOffset, i );
	}

	PLANE Plane;
	CDomainPlanes *DomainPlanes = (CDomainPlanes*)&(*m_pDomain);  // Convoluted - need to see if better way of doing this
	XYZ RefPlane(0,0,-1);
	int index = DomainPlanes->GetPlane( RefPlane, Plane );
	Plane.d -= ZOffset.z;
	DomainPlanes->SetPlane( index, Plane );
	m_bNeedsBuilding = true;
}

void CTextileLayered::GetOffsetMinDist( int iOffset, vector< vector<pair<double,double> > >& LayerIntersections, vector<pair<int,double> >& MinDist, int iNumX, int iNumY)
{
	vector<double> OffsetMinDist(MinDist.size(),START_DIST);

	int incX = iOffset % iNumX;
	int incY = (iOffset - incX) / iNumX;
	for ( int j = 0; j < iNumY; ++j )
	{	
		for ( int i = 0; i < iNumX; ++i )
		{
			int iInd = j*iNumX+i;
			int iOffsetInd = ((j+incY)%iNumY)*iNumX + ((i+incX)%iNumX);
			for ( int LayerInd = 0; LayerInd < LayerIntersections[iInd].size()-1; ++LayerInd )
			{
				if ( LayerIntersections[iInd][LayerInd].second > 0.0 && LayerIntersections[iOffsetInd][LayerInd+1].first > 0.0 )
				{
					double dist = LayerIntersections[iOffsetInd][LayerInd+1].first - LayerIntersections[iInd][LayerInd].second;
					if ( OffsetMinDist[LayerInd] == START_DIST || dist < OffsetMinDist[LayerInd] )
					{
						OffsetMinDist[LayerInd] = dist;
					}
				}
			}
		}
	}
	
	// Check if min dist for this offset is greater than previous stored for each inter layer gap (ie will give greater nesting)
	for ( int index = 0; index < MinDist.size(); ++index )  // size of MinDist = number of layers - 1
	{
		if ( MinDist[index].second == START_DIST || OffsetMinDist[index] > MinDist[index].second )
		{
			MinDist[index].second = OffsetMinDist[index];
			MinDist[index].first = iOffset;
		}
	}
}

void CTextileLayered::GetOffsetMinDist( int x, int y, int iLayer, vector< vector<pair<double,double> > >& LayerIntersections, vector<pair<int,double> >& MinDist, int iNumX, int iNumY, int XSize, int YSize, bool bOffsetTop)
{
	//vector<double> OffsetMinDist(MinDist.size(),START_DIST);
	double OffsetMinDist = START_DIST;

	//int incX = x % iNumX;
	//int incY = y % iNumY;
	/*for ( int j = 0; j < iNumY; ++j )
	{	
		for ( int i = 0; i < iNumX; ++i )
		{
			int iInd = j*iNumX+i;
			int iOffsetInd = ((j+y)%iNumY)*iNumX + ((i+x)%iNumX);
			
			if ( LayerIntersections[iInd][Layer].second > 0.0 && LayerIntersections[iOffsetInd][Layer+1].first > 0.0 )
			{
				double dist = LayerIntersections[iOffsetInd][Layer+1].first - LayerIntersections[iInd][Layer].second;
				if ( OffsetMinDist[Layer] == START_DIST || dist < OffsetMinDist[Layer] )
				{
					OffsetMinDist[Layer] = dist;
				}
			}
			
		}
	}*/
	for ( int j = 0; j < iNumY; ++j )
	{	
		for ( int i = 0; i < iNumX; ++i )
		{
			int iTopInd, iBottomInd;
			if ( bOffsetTop )
			{
				iBottomInd = j*iNumX+i;
				iTopInd = ((j+y)%iNumY)*iNumX + ((i+x)%iNumX);
			}
			else
			{
				iTopInd = j*iNumX+i;
				iBottomInd = ((j+y)%iNumY)*iNumX + ((i+x)%iNumX);
			}
			
			if ( LayerIntersections[iBottomInd][0].second > 0.0 && LayerIntersections[iTopInd][1].first > 0.0 )
			{
				double dist = LayerIntersections[iTopInd][1].first - LayerIntersections[iBottomInd][0].second;
				if ( OffsetMinDist == START_DIST || dist < OffsetMinDist )
				{
					OffsetMinDist = dist;
				}
			}
			
		}
	}
	
	// Check if min dist for this offset is greater than previous stored for this inter layer gap (ie will give greater nesting)
	
	if ( MinDist[iLayer].second == START_DIST || OffsetMinDist > MinDist[iLayer].second )
	{
		MinDist[iLayer].second = OffsetMinDist;
		MinDist[iLayer].first = y*iNumX + x;
	}
	
}

int CTextileLayered::GetLayerMeshes( vector<CMesh>& LayerMeshes )
{
	int iNumSlaveNodes = 0;
	for ( int i = 0; i < m_LayerYarnIndices.size(); ++i )
	{
		vector<int>::iterator itIndices;
		CMesh Mesh;
		for ( itIndices = m_LayerYarnIndices[i].begin(); itIndices != m_LayerYarnIndices[i].end(); ++itIndices )
		{
			CYarn* Yarn = GetYarn( *itIndices );
			int Num = Yarn->GetNumSlaveNodes();  // Use the number of slave nodes to give resolution for grid
			if ( Num > iNumSlaveNodes )
			{
				iNumSlaveNodes = Num;
			}
			Yarn->AddSurfaceToMesh( Mesh, *m_pDomain, false, false );
		}
		Mesh.ConvertQuadstoTriangles();
		LayerMeshes.push_back( Mesh );
	}
	return iNumSlaveNodes;
}

void CTextileLayered::GetLayerRepeats( vector< XY >& Repeats )
{
	for ( int i = 0; i < m_LayerYarnIndices.size(); ++i )
	{
		XY Repeat;
		CYarn* Yarn = GetYarn( m_LayerYarnIndices[i][0] );  // Just get first - assume repeats for all yarns in layer are the same (valid??)
		vector<XYZ> YarnRepeats;
		vector<XYZ>::iterator itYarnRepeats;
		YarnRepeats = Yarn->GetRepeats();
		for ( itYarnRepeats = YarnRepeats.begin(); itYarnRepeats != YarnRepeats.end(); ++itYarnRepeats )
		{
			if ( itYarnRepeats->x > Repeat.x )
			{
				Repeat.x = itYarnRepeats->x;
			}
			if ( itYarnRepeats->y > Repeat.y )
			{
				Repeat.y = itYarnRepeats->y;
			}
		}
		Repeats.push_back( Repeat );
	}
}

double CTextileLayered::GreatestCommonDenominator( double X1, double X2 )
{
	int iX1 = (int)(X1 * 1e+6);
	int iX2 = (int)(X2 * 1e+6);

	if ( iX2 > iX1 )
	{
		int temp = iX1;
		iX1 = iX2;
		iX2 = temp;
	}

	while ( iX2 != 0 )
	{
		int temp = iX2;
		iX2 = iX1 % iX2;
		iX1 = temp;
	}
	return( (double)(iX1/1e+6) );
}
