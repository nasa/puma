/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2010 Louise Brown

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
#include "AdjustMeshInterference.h"
//#include "MeshIntersectionData.h"
//#include "Textile.h"
//#include "Misc.h"
#include "TexGen.h"
#include <time.h>

#define	FIRST_HALF	0
#define SECOND_HALF	1
#define TOL		1e-6

using namespace TexGen;

CAdjustMeshInterference::CAdjustMeshInterference(void)
{
}

CAdjustMeshInterference::~CAdjustMeshInterference(void)
{
}

bool CAdjustMeshInterference::AdjustMesh( CTextile &Textile, vector<CMesh> &YarnMeshes, double Tolerance )
{
	
	int iNumYarns = Textile.GetNumYarns();
	m_IntersectionMeshes.clear();
	m_IntersectionMeshes.resize( iNumYarns );
	m_Tolerance = Tolerance;
	vector<CMesh>::iterator itMesh;
	
	// Mesh.CalculateVolume() before & after gives change in total yarn volume.  May need to use if output to format where
	// using Vf = 1 and just need to know how much mass has been removed
	for ( int i = 0; i < iNumYarns; ++i )
	{
		TGLOG( "Volume of yarn mesh " << i << " = " << YarnMeshes[i].CalculateVolume() << "\n" );
	}

	m_TempYarnMeshes.clear();
	for ( itMesh = YarnMeshes.begin(); itMesh != YarnMeshes.end(); ++itMesh )
	{
		CMesh Mesh = *itMesh;
		m_TempYarnMeshes.push_back( Mesh );
	}

	if ( !CheckInitialIntersections( Textile, YarnMeshes ) ) // Returns false if any outside tolerence so adjustement needed
	{
		if ( AdjustInitialIntersections( YarnMeshes ) )  // Returns false any of intersections larger than element intersecting with
		{
#ifdef _DEBUG
			for ( int i=0; i < (int)YarnMeshes.size(); ++i )
			{
				YarnMeshes[i].SaveToVTK( "c:\\Program Files\\TexGen\\InitialAdjustedMesh" + stringify(i));
			}
#endif
			if ( !AdjustIntersections( YarnMeshes ) )
				return false;
		}
		else
			return false;
	}
	
	/*for ( int i = 0; i < iNumYarns; ++i )
	{
		TGLOG( "Volume of adjusted yarn mesh " << i << " = " << YarnMeshes[i].CalculateVolume() << "\n" );
	}*/

//	AdjustInterpolationNodes( YarnMeshes );
	//SetNodeDisplacements( iNumYarns, YarnMeshes ); // Displacements for if using TextileDeformer
	return true; 
	
}


bool CAdjustMeshInterference::CheckInitialIntersections( CTextile &Textile, vector<CMesh> &YarnMeshes )
{
	vector<float> InterferenceDepths;
	vector<int> YarnIndices;
	CMesh InterferencePoints;
	double dMinDist, dDist, dMaxDist = 0.0;
	int iIndex;

	TGLOG("Checking initial intersections");
	Textile.DetectInterference( InterferenceDepths, YarnIndices, true, &InterferencePoints );

	const vector<XYZ> &Repeats = Textile.GetYarn(0)->GetRepeats();  // Simulation Abaqus only uses repeats from yarn 0 to get surface.
																	// Makes assumption that repeats are same for all yarns (valid?...)

	for ( int i=0; i< (int)InterferenceDepths.size(); ++i )
	{
		dMinDist = -1.0;

		dDist = fabs(InterferenceDepths[i]);
		if ( dDist > dMaxDist ) // Find max interference depth
			dMaxDist = dDist;

		iIndex = YarnMeshes[YarnIndices[i]].GetClosestNode( InterferencePoints.GetNode( i ) );
		
		if ( iIndex != -1 )
		{
			CMeshIntersectionData PointInfo;
			int iPairIndex = -1;
			PointInfo.SetYarn( YarnIndices[i] );
			PointInfo.SetIndex( iIndex );
			PointInfo.SetDepth( InterferenceDepths[i] );
			PointInfo.SetPoint( InterferencePoints.GetNode(i) );
			PointInfo.SetStartPoint( InterferencePoints.GetNode(i) );

			// Check for corresponding point on opposite boundary
			vector<XYZ>::const_iterator itRepeat;   
			for (itRepeat=Repeats.begin(); itRepeat!=Repeats.end(); ++itRepeat)
			{
				iPairIndex = YarnMeshes[YarnIndices[i]].GetClosestNodeDistance( InterferencePoints.GetNode(i) + (*itRepeat), TOL);
				if ( iPairIndex != -1 )
					break;
				iPairIndex = YarnMeshes[YarnIndices[i]].GetClosestNodeDistance( InterferencePoints.GetNode(i) - (*itRepeat), TOL );
				if ( iPairIndex != -1 )
					break;
			}
			PointInfo.SetBoundaryPairIndex( iPairIndex );
			m_Intersections.push_back( PointInfo );
		}
		else
		{
			TGERROR("Index = -1");
		}
	}

	if ( dMaxDist < m_Tolerance )
		return true;  // Intersections all within tolerance
	return false;
}

bool CAdjustMeshInterference::AdjustInitialIntersections( vector<CMesh> &YarnMeshes )
{
	
	vector<CMeshIntersectionData>::iterator itIntersections;
	vector<CMesh>::iterator itMesh;

	// Convert element lists to vectors so that have random access into data
	vector< vector<int> > WedgeElementIndices;
	vector< vector<int> > HexElementIndices;

	TGLOG("Adjusting initial intersections");
	for ( itMesh = YarnMeshes.begin(); itMesh != YarnMeshes.end(); ++itMesh )
	{
		vector<int> ElementIndices;
		ElementIndices.clear();
		
		itMesh->ConvertElementListToVector( CMesh::WEDGE, ElementIndices );
		WedgeElementIndices.push_back( ElementIndices );
		ElementIndices.clear();
		itMesh->ConvertElementListToVector( CMesh::HEX, ElementIndices );
		HexElementIndices.push_back( ElementIndices );
	}
	
	for ( itIntersections = m_Intersections.begin(); itIntersections != m_Intersections.end(); ++itIntersections )
	{
		// Find all the elements in the mesh containing the intersection point
		
		int iYarn = itIntersections->GetYarn();
		bool	bFoundNode;
		itIntersections->FindElements( WedgeElementIndices[iYarn], CMesh::WEDGE);
		itIntersections->FindElements( HexElementIndices[iYarn], CMesh::HEX );
		itIntersections->FindPolygonPoints( YarnMeshes[iYarn].GetIndices(CMesh::POLYGON) );
		bFoundNode = itIntersections->FindInterpolationNode( YarnMeshes[iYarn]);

		if ( bFoundNode )
		{
			if ( !itIntersections->MoveNode( YarnMeshes[iYarn] ) ) // Adjust the point	
				return false;   // Adjustment size > element size
		}
	}

	for ( itIntersections = m_Intersections.begin(); itIntersections != m_Intersections.end(); ++itIntersections ) // Can't do this in previous loop because need to wait until all nodes adjusted
	{
		AddIntersectElementsToMesh( itIntersections->GetElements(), itIntersections->GetYarn(), YarnMeshes[itIntersections->GetYarn()] );  // Add the elements to a new mesh for the appropriate yarn to be used for further intersection testing
	}

	int i;
	for ( itMesh = m_IntersectionMeshes.begin(), i=0; itMesh != m_IntersectionMeshes.end(); ++itMesh, ++i )
	{
#ifdef _DEBUG		
		itMesh->SaveToVTK( "c:\\Program Files\\TexGen\\IntersectionMesh" + stringify(i) );
#endif
		itMesh->MergeNodes();
		itMesh->Convert3Dto2D();
		itMesh->RemoveDuplicateElements(CMesh::QUAD);
		itMesh->ConvertQuadstoTriangles();
	}
	return true;
}

// Create meshes containing just the elements with intersection points (to reduce data to check for intersections)
void CAdjustMeshInterference::AddIntersectElementsToMesh( const vector<ELEMENT_INDICES> &Elements, int iYarn, CMesh &YarnMesh )
{
	vector<ELEMENT_INDICES>::const_iterator itElements;
	int i;
	for ( i = 0, itElements = Elements.begin(); itElements != Elements.end(); ++i, ++itElements )
	{
		vector<int> iNewIndices;
		int iNumNodes = CMesh::GetNumNodes( itElements->ElementType );
		for ( int j = 0; j < iNumNodes; ++j )
		{
			int iNodeIndex = m_IntersectionMeshes[iYarn].AddNode( YarnMesh.GetNode( itElements->Index[j] ));
			iNewIndices.push_back( iNodeIndex );
		}
		m_IntersectionMeshes[iYarn].AddElement( itElements->ElementType, iNewIndices );
	}
}
bool CAdjustMeshInterference::AdjustIntersections( vector<CMesh> &YarnMeshes )
{
	vector<CMeshIntersectionData>::iterator itIntersections;
	vector<CMesh>::iterator itIntersectionMeshes;
	int iNumIntersectPoints = 0, iIterations = 0;

	// Find AABB of meshes so that can do quick initial test of whether to check for intersections
	vector< pair<XYZ, XYZ> > MeshAABB;
	for ( itIntersectionMeshes = m_IntersectionMeshes.begin(); itIntersectionMeshes != m_IntersectionMeshes.end(); itIntersectionMeshes++ )
	{
		pair<XYZ, XYZ> AABB = itIntersectionMeshes->GetAABB();
		MeshAABB.push_back( AABB );
	}

	// Keep adjusting points until all within tolerance
	do 
	{
		iNumIntersectPoints = 0;
		for ( itIntersections = m_Intersections.begin(); itIntersections != m_Intersections.end(); ++itIntersections )
		{
			if ( fabs(itIntersections->GetDepth()) < m_Tolerance ) // if already within tolerance move onto next point
				continue;
			for ( int i = 0; i < (int)m_IntersectionMeshes.size(); ++i )
			{
				if( itIntersections->GetYarn() == i || !BoundingBoxIntersect(MeshAABB[i].first, MeshAABB[i].second, MeshAABB[itIntersections->GetYarn()].first, MeshAABB[itIntersections->GetYarn()].second ))  // Don't need to test against points in the same yarn
					continue;	// Don't need to check for intersections if AABBs don't intersect
				
				vector< pair<double, XYZ> > IntersectionPoints;
				int numIntersect = m_IntersectionMeshes[i].IntersectLine( itIntersections->GetPoint(), itIntersections->GetInterpPoint(), IntersectionPoints, make_pair(true,true) );
				if ( numIntersect > 0 )
				{
					itIntersections->SetDepth( GetLength( itIntersections->GetPoint(), itIntersections->GetInterpPoint() )* IntersectionPoints[0].first );
					if ( itIntersections->GetDepth() > m_Tolerance )
					{
						if ( !itIntersections->MoveNode(YarnMeshes[itIntersections->GetYarn()]) )
							return false;  // Intersection too large to correct 
						iNumIntersectPoints++;
						break;  // Found it - can move onto next point
					}
				}
			}
		}
		++iIterations;
		TGLOG("Adjusting points: Iteration" << iIterations );
	} while ( iNumIntersectPoints );
	return true;
}

void CAdjustMeshInterference::SetNodeDisplacements( int iNumYarns, vector<CMesh> &YarnMeshes )
{
	vector<CMeshIntersectionData>::iterator itMeshData;
	for ( int i = 0; i < iNumYarns; ++i )
	{
		vector<XYZ> YarnNodeDisplacements;
		YarnNodeDisplacements.resize( YarnMeshes[i].GetNumNodes() );
		m_NodeDisplacements.push_back( YarnNodeDisplacements );
	}
	for ( itMeshData = m_Intersections.begin(); itMeshData != m_Intersections.end(); ++itMeshData )
	{
		m_NodeDisplacements[itMeshData->GetYarn()][itMeshData->GetIndex()] = -itMeshData->GetDisplacement();
		
	}
}

void CAdjustMeshInterference::AdjustInterpolationNodes( vector<CMesh> &YarnMeshes )
{
	vector<CMeshIntersectionData>::iterator itIntersections;

	for ( itIntersections = m_Intersections.begin(); itIntersections != m_Intersections.end(); ++itIntersections )
	{
		itIntersections->AdjustInterpolationNode(YarnMeshes[itIntersections->GetYarn()]);
	}
}

bool CAdjustMeshInterference::CreateVolumeMeshes( CTextile &Textile, vector<CMesh> &YarnMeshes )
{
	int iNumYarns = Textile.GetNumYarns();
	YarnMeshes.clear();
	YarnMeshes.resize(iNumYarns);
	const CDomain* pDomain = Textile.GetDomain();
	if (!pDomain)
	{
		TGERROR("Unable to set up volume meshes: No domain specified");
		return false;
	}

	for (int i=0; i<iNumYarns; ++i)  // Create volume mesh for each yarn as MS thesis sections 2.9.1 & 2.9.2
	{
		CYarn* pYarn = Textile.GetYarn(i);
		if ( !pYarn->AddVolumeToMesh(YarnMeshes[i], *pDomain))// || m_YarnMeshes[i].NodesEmpty())
		{
			TGERROR("Failed to create volume mesh for yarn " << i );
			return false;
		}
	}
	return true;
}

void CAdjustMeshInterference::AdjustSectionMeshes( CTextile &Textile, vector<CMesh> &YarnMeshes )
{
	int iNumYarns = Textile.GetNumYarns();
	const CDomain* pDomain = Textile.GetDomain();
	vector< vector<int> > PolygonIndices;
	vector< vector<int> > AdjustedPolygonIndices;

	
	vector<CMesh>::iterator itMesh;
	vector<CMesh>::iterator itTempMesh;


	// Convert polygon element lists to vectors so that can use indices to access matching elements in original and adjusted meshes
	

	for ( itMesh = YarnMeshes.begin(), itTempMesh = m_TempYarnMeshes.begin(); itMesh != YarnMeshes.end(), itTempMesh != m_TempYarnMeshes.end(); ++itMesh, ++itTempMesh )
	{
		vector<int> ElementIndices;
		ElementIndices.clear();
		
		itMesh->ConvertElementListToVector( CMesh::POLYGON, ElementIndices );
		AdjustedPolygonIndices.push_back( ElementIndices );
		ElementIndices.clear();
		itTempMesh->ConvertElementListToVector( CMesh::POLYGON, ElementIndices );
		PolygonIndices.push_back( ElementIndices );
	}
	

	for ( int i = 0; i < iNumYarns; ++i )
	{
		CYarn* Yarn = Textile.GetYarn(i);
		vector<XYZ> Translations;
		Translations = pDomain->GetTranslations( *Yarn );
		
		const vector<CSlaveNode> &Nodes = Yarn->GetSlaveNodes(CYarn::SURFACE);
		//vector<CSlaveNode> TempNodes = Nodes;
		vector<CSlaveNode>::const_iterator itNodes;
		CYarnSectionInterpPosition SectionInterp;

		YARN_POSITION_INFORMATION YarnPositionInfo;
		YarnPositionInfo.SectionLengths = Yarn->GetSectionLengths();
		
		//sort( TempNodes.begin(), TempNodes.end() );
		
		//list<int> &PolygonIndices = m_YarnMeshes[i].GetIndices( CMesh::POLYGON );
		//list<int>::iterator itInt;
		//for ( itNodes = TempNodes.begin(); itNodes != TempNodes.end(); ++itNodes )
		for ( itNodes = Nodes.begin(); itNodes != Nodes.end(); ++itNodes )
		{
			YarnPositionInfo.iSection = itNodes->GetIndex();
			double T = itNodes->GetT();  // Get position of slave node along section
			YarnPositionInfo.dSectionPosition = T;
			T = YarnPositionInfo.GetYarnPosition();  // Get position along yarn

			vector<XYZ> SlaveNodePoints;
			SlaveNodePoints = itNodes->GetSectionPoints();
			int iPolygonIndex = -1;
			int iOffsetIndex = -1;
			int iTrans;

			iPolygonIndex = FindMeshPolygonSection( SlaveNodePoints, m_TempYarnMeshes[i], PolygonIndices[i], Translations, iOffsetIndex, iTrans );
			//iPolygonIndex = FindMeshPolygonSection( SlaveNodePoints, m_YarnMeshes[i], PolygonIndices[i], Translations, iOffsetIndex );
			if ( iPolygonIndex == -1 )
			{
				// Add slave node section
				SectionInterp.AddSection( T, CSectionPolygon( itNodes->Get2DSectionPoints() ));
			}
			else
			{
				vector<XYZ> SectionPoints;
				vector<XYZ> TempPoints;
				XYZ Point;
				
				//vector<int> Indices = AdjustedPolygonIndices[i];
				vector<int> Indices = PolygonIndices[i];
				int iStartIndex = Indices[iPolygonIndex];
				int j = 0;
				// Get the nodes around the section at this point
				// iOffsetIndex accounts for the difference in the start of the section mesh edge points and the points stored in the polygon
				// sections because of the way the section meshes are generated
				do {
						Point = YarnMeshes[i].GetNode( Indices[iPolygonIndex++] );
						//Point = TempMeshes[i].GetNode( Indices[iPolygonIndex++] );
						if ( j < iOffsetIndex )
							TempPoints.push_back(Point);
						else
							SectionPoints.push_back( Point );
						j++;
				} while ( Indices[iPolygonIndex] != iStartIndex );
				
				/*Point = m_YarnMeshes[i].GetNode( Indices[iPolygonIndex++] );
				if ( j < iOffsetIndex )
					TempPoints.push_back( Point );
				else
					SectionPoints.push_back( Point );*/

				SectionPoints.insert( SectionPoints.end(), TempPoints.begin(), TempPoints.end() );
				//SectionPoints.push_back( SectionPoints[0] );

				vector<XYZ>::iterator itSectionPoints;
				for ( itSectionPoints = SectionPoints.begin(); itSectionPoints != SectionPoints.end(); ++itSectionPoints )
				{
					*(itSectionPoints) -= Translations[iTrans];
				}

				// Convert the 3D section points to local 2D ones
				vector<XY> Points2D;
				XYZ SlaveNodePos = itNodes->GetPosition();
				XYZ Up = itNodes->GetUp();
				XYZ Side = itNodes->GetSide();

				for ( itSectionPoints = SectionPoints.begin(); itSectionPoints != SectionPoints.end(); ++itSectionPoints )
				{
					// Calculate the location of the point projected on to the cross section plane
					XYZ Relative = *itSectionPoints - SlaveNodePos;
					XY Point2D;
					Point2D.x = DotProduct(Side, Relative);
					Point2D.y = DotProduct(Up, Relative);
					Points2D.push_back( Point2D );
				}
				// Create polygon section using 2D points and add to CYarnSectionInterpPosition
				SectionInterp.AddSection( T, CSectionPolygon(Points2D) );
			}
		}

		Yarn->AssignSection( SectionInterp );
	}
}

int CAdjustMeshInterference::FindMeshPolygonSection( vector<XYZ> &SlaveNodePoints, CMesh &TempMesh, vector<int> &PolygonIndices, vector<XYZ> &Translations, int &OffsetIndex, int &iTrans )
{
	vector<XYZ>::iterator itSlavePoints;
	vector< vector<XYZ> > TranslatedSections;
	vector<XYZ>::iterator itTranslations;

	// Create section points for each translation of section in the domain
	for ( itTranslations = Translations.begin(); itTranslations != Translations.end(); ++itTranslations )
	{
		vector<XYZ> SectionPoints;
		SectionPoints.clear();
		for ( itSlavePoints = SlaveNodePoints.begin(); itSlavePoints != SlaveNodePoints.end(); ++itSlavePoints )
		{
			SectionPoints.push_back( *(itSlavePoints) + *(itTranslations) );
		}
		TranslatedSections.push_back( SectionPoints );
	}

	
	vector<int>::iterator itPolygonIndices;
	vector<XYZ> PolygonPoints;
	int iElementIndex = 0;

	for ( itPolygonIndices = PolygonIndices.begin(); itPolygonIndices != PolygonIndices.end(); )
	{
		int StartIndex = *(itPolygonIndices);
		int StartElementIndex = iElementIndex;
		XYZ Point;
		PolygonPoints.clear();
		// Get the nodes around the section at this point
		do {
				int index = *(itPolygonIndices);
				Point = TempMesh.GetNode( index );
				//Point = TempMesh.GetNode( *(itPolygonIndices++) );
				++itPolygonIndices;
				PolygonPoints.push_back( Point );
				iElementIndex++;
		} while ( *(itPolygonIndices)!= StartIndex );
		
		++itPolygonIndices;
		++iElementIndex;
		
		//GetPolygonSectionPoints( itPolygonIndices, TempMesh, PolygonPoints );

		OffsetIndex = CompareSections( TranslatedSections, PolygonPoints, iTrans );
		if ( OffsetIndex >= 0 )
		{
			//return StartIndex;
			return StartElementIndex;
		}
	}
	return -1;
}

int CAdjustMeshInterference::CompareSections( vector< vector<XYZ> > &TranslatedSections, vector<XYZ> &PolygonPoints, int &iTrans )
{
	bool bMatch;
	vector< vector<XYZ> >::iterator itTransSections;
	double tolerance = 0.0000001;  // Assign properly
	int iStartIndex;
	iTrans = 0;

	for ( itTransSections = TranslatedSections.begin(); itTransSections != TranslatedSections.end(); ++ itTransSections )
	{
		vector<XYZ>::iterator itTransPoints;
		vector<XYZ>::iterator itPolyPoints;
		bMatch = true;

		if ( itTransSections->size() != PolygonPoints.size() )  // Only try & compare sections with equal number of points
			continue;
		vector<XYZ> TransSection = *itTransSections;

		// Find matching start point
		iStartIndex = 0;
		for( itPolyPoints = PolygonPoints.begin(); itPolyPoints != PolygonPoints.end(); ++itPolyPoints )
		{
			if( GetLength(TransSection[0] - *(itPolyPoints)) < tolerance )
				break;  // Found matching start point
			++iStartIndex;
		}
		if ( itPolyPoints == PolygonPoints.end() )
		{
			bMatch = false;  // No matching points, go to next translation
			++iTrans;
			continue;
		}
		
		for( itTransPoints = TransSection.begin(); itTransPoints != TransSection.end(); ++itTransPoints )
		{
			if ( GetLength((*itTransPoints) - (*itPolyPoints)) > tolerance )
			{
				bMatch = false;
				break;
			}
			++itPolyPoints;
			if ( itPolyPoints == PolygonPoints.end() )
				itPolyPoints = PolygonPoints.begin();
		}
		if ( bMatch )
			return iStartIndex;
		++iTrans;
	}
	return -1;
}

CMeshIntersectionData::CMeshIntersectionData(void)
{
	m_Wedge = false;
	m_InterpIndex = -1;
}

CMeshIntersectionData::~CMeshIntersectionData(void)
{
}

bool CMeshIntersectionData::FindInterpolationNode( CMesh &YarnMesh )
{
	int interpNodeIndex = -1;
	vector<int>::iterator itElementIndex;
	
	if ( m_Elements.size() == 4  || m_Elements.size() == 3 ) // Size may be 3 where yarn has been cut at an angle on domain edge
	{
		// Find index to interpolate between.  It's the one that occurs in all 4 elements
		
		for( itElementIndex = m_Elements[0].Index.begin(); itElementIndex != m_Elements[0].Index.end(); ++itElementIndex )
		{
			if ( *itElementIndex == m_Index ) // Don't need to check for the one you're looking for!
				continue;
			vector<ELEMENT_INDICES>::iterator itElementArrays = m_Elements.begin()+1;
			while ( itElementArrays != m_Elements.end() // Will drop out of loop if didn't find index in one of arrays
				&& find(itElementArrays->Index.begin(), itElementArrays->Index.end(), *itElementIndex) != itElementArrays->Index.end() )
			{
				itElementArrays++;
			}
			if ( itElementArrays == m_Elements.end() )  
			{
				break;  // If got to end of arrays then have found index in all of them - it's the one
			}
		}
		interpNodeIndex = *itElementIndex;
	}
	else if ( m_Elements.size() == 2 || m_Elements.size() == 1 )  
	{			
		// When 2 elements need to find the common index at the same end of the element
									// Occurs when node at end of mesh
		bool ElementEnd[2];
		
		for ( int i = 0; i < (int)m_Elements.size(); ++i )  // Find which end of the elements the node is
		{
			int j;
			for (itElementIndex = m_Elements[i].Index.begin(), j = 0; itElementIndex != m_Elements[i].Index.end(); ++itElementIndex, ++j )
			{
				if ( *itElementIndex == m_Index )
				{
					if ( j < CMesh::GetNumNodes( m_Elements[i].ElementType )/2 )
						ElementEnd[i] = FIRST_HALF;
					else
						ElementEnd[i] = SECOND_HALF;
					break;
				}
			}
		}
		
		// Find iterator start & end for appropriate half of element
		vector<int>::iterator itElementStart[2], itElementEnd[2];
		for ( int i = 0; i < (int)m_Elements.size(); ++i )
		{
			int iNumNodes = CMesh::GetNumNodes(m_Elements[i].ElementType)/2;
			itElementStart[i] = m_Elements[i].Index.begin() + ElementEnd[i] * iNumNodes;
			if ( ElementEnd[i] == FIRST_HALF )
				itElementEnd[i] = m_Elements[i].Index.begin() + iNumNodes;
			else
				itElementEnd[i] = m_Elements[i].Index.end();
		}

		if( m_Elements.size() == 2 )
		{
			

			// Find 2nd node which is at same end of both elements as search node
			for ( itElementIndex = itElementStart[0]; itElementIndex != itElementEnd[0]; ++itElementIndex )		
			{
				if ( *itElementIndex == m_Index )  // Don't search for the one already know
					continue;
				
				if ( find(itElementStart[1], itElementEnd[1], *itElementIndex) != itElementEnd[1] )
				{
					break;  // Found it!
				}
			}
			interpNodeIndex = *itElementIndex;
		}
		else if ( m_Elements.size() == 1 )
		{
			TGLOG( "Number of elements = 1.  Need to test this code" );
			int Index1 = -1;
			int PolygonIndex = -1;
			int i = 0;
			for ( itElementIndex = itElementStart[0] ; itElementIndex != itElementEnd[0]; ++itElementIndex )
			{
				if ( *itElementIndex == m_Index )
				{
					Index1 = i;
					continue;
				}
				if ( *itElementIndex == m_PolygonPoints[0] || *itElementIndex == m_PolygonPoints[2] )
				{
					PolygonIndex = i;
					continue;
				}
				++i;
			}
			
			if ( m_Wedge )
			{
				for ( int i = 0; i < 2; ++i )
				{
					if ( i != Index1 && i != PolygonIndex ) 
					{
						interpNodeIndex = *(itElementStart[0]+i);
						break;
					}
				}
			}
			else
			{
				int iNumNodes = CMesh::GetNumNodes(m_Elements[0].ElementType)/2;
				if ( PolygonIndex > Index1 )
				{
					interpNodeIndex = *(itElementStart[0]+((Index1+iNumNodes-1)%(iNumNodes)));
				}
				else
				{
					interpNodeIndex = *( itElementStart[0]+((Index1+1)%(iNumNodes)));
				}
			}
		}
	}
	
	
	if ( interpNodeIndex != -1 )
	{
		m_InterpPoint = YarnMesh.GetNode(interpNodeIndex);
		SetInterpIndex( interpNodeIndex );
		return true;
	}
	TGERROR("Find interpolation node, number of elements = " << m_Elements.size() );
	return false;
}

bool CMeshIntersectionData::MoveNode( CMesh &YarnMesh )
{
	XYZ NewPoint;
	XYZ Offset;
	double absDepth = fabs(m_Depth);
	double length = GetLength( m_Point, m_InterpPoint );
	double CheckLength = length;
	if ( m_Wedge )  // If wedge don't want to get case where two corners of element moved to same point
	{
		double MinLength = 0.02 * GetLength( m_StartPoint, m_InterpPoint );
		CheckLength -= MinLength;
	}
	if ( (absDepth * 0.5) > CheckLength )
	{
		TGERROR("Intersection depth in yarn " << m_Yarn << " too large for adjustment");
		return false;
	}

	Offset.x = ((m_InterpPoint.x - m_Point.x) * absDepth * 0.5 / length );
	Offset.y = ((m_InterpPoint.y - m_Point.y) * absDepth * 0.5 / length );
	Offset.z = ((m_InterpPoint.z - m_Point.z) * absDepth * 0.5 / length );
	NewPoint = m_Point + Offset;
	/*NewPoint.x = m_Point.x + Offset.x;
	NewPoint.y = m_Point.y + Offset.y;
	NewPoint.z = m_Point.z + Offset.z;*/
	
	YarnMesh.SetNode(m_Index, NewPoint );
	m_Point = NewPoint;
	if ( m_BoundaryPairIndex != -1 )
	{
		NewPoint = YarnMesh.GetNode( m_BoundaryPairIndex );
		NewPoint += Offset;
		YarnMesh.SetNode( m_BoundaryPairIndex, NewPoint );
	}
	return true;
}

void CMeshIntersectionData::FindElements( vector<int> &IndexArray, CMesh::ELEMENT_TYPE ElementType )
{
	vector<int>::iterator itIndexArray = IndexArray.begin();
	ELEMENT_INDICES			Element;
	Element.ElementType = ElementType;

	while ( itIndexArray != IndexArray.end() )
	{
		itIndexArray = find( itIndexArray, IndexArray.end(), m_Index );  
		if ( itIndexArray != IndexArray.end() )  // Found an entry 
		{
			int iIndex = itIndexArray - IndexArray.begin();
				
			vector<int>::iterator itStartVertex = itIndexArray - (iIndex % CMesh::GetNumNodes(ElementType));
			// Add the block of indices for the element
			Element.Index.assign( itStartVertex, itStartVertex + CMesh::GetNumNodes(ElementType) ); 
			m_Elements.push_back( Element );
			if ( ElementType == CMesh::WEDGE )
				m_Wedge = true;
			Element.Index.clear();
			itIndexArray++;  // Move to next item to continue searching
		}
	}
}

void CMeshIntersectionData::FindPolygonPoints( list<int> &Polygons )
{
	list<int>::iterator itPolygons;
	int iFoundIndex = -1;

	for ( itPolygons = Polygons.begin(); itPolygons != Polygons.end(); )
	{
		int StartIndex = *itPolygons;
		vector<int> Polygon;
		int i = 0;
		do{
			Polygon.push_back( *itPolygons );
			if ( *itPolygons == m_Index )
				iFoundIndex = i;
			++itPolygons;
			++i;
		} while ( (*itPolygons) != StartIndex );
		++itPolygons;
		if ( iFoundIndex != -1 )
		{
			m_PolygonPoints[1] = Polygon[iFoundIndex];
			if ( iFoundIndex == 0 )  
				m_PolygonPoints[0] = Polygon[i-1]; // Point before end
			else
				m_PolygonPoints[0] = Polygon[iFoundIndex-1];
			if ( iFoundIndex == i-1 ) 
				m_PolygonPoints[2] = Polygon[0];
			else
				m_PolygonPoints[2] = Polygon[iFoundIndex+1];
			break;
		}
	}
}

void CMeshIntersectionData::AdjustInterpolationNode( CMesh &YarnMesh )
{
	XYZ Offset;
	XYZ NewPoint;
	if ( m_InterpIndex == -1 )
		return;

	Offset = m_StartPoint - m_Point;

	NewPoint = YarnMesh.GetNode( m_InterpIndex );  //  Get node rather than offsetting interpPoint to get correct offsets 
										// on points used more than once
	NewPoint -= Offset/2.0;

	YarnMesh.SetNode( m_InterpIndex, NewPoint );
	
	// Sort out boundaries
	/*if ( m_BoundaryPairIndex != -1 )
	{
		NewPoint = YarnMesh.GetNode( m_BoundaryPairIndex );
		NewPoint += Offset;
		YarnMesh.SetNode( m_BoundaryPairIndex, NewPoint );
	}*/
}

