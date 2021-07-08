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
#include "VoxelMesh.h"
#include "TexGen.h"
#include "PeriodicBoundaries.h"
#include "ShearedPeriodicBoundaries.h"
#include "StaggeredPeriodicBoundaries.h"
#include <iterator>
//#define SHINY_PROFILER TRUE

using namespace TexGen;

CVoxelMesh::CVoxelMesh(string Type)
{
	if ( Type == "CShearedPeriodicBoundaries" )
		m_PeriodicBoundaries = new CShearedPeriodicBoundaries;
	else if ( Type == "CStaggeredPeriodicBoundaries" )
		m_PeriodicBoundaries = new CStaggeredPeriodicBoundaries;
	else if ( Type == "CRotatedPeriodicBoundaries" )
		m_PeriodicBoundaries = new CRotatedPeriodicBoundaries;
	else
		m_PeriodicBoundaries = new CPeriodicBoundaries;
}

CVoxelMesh::~CVoxelMesh(void)
{
	delete m_PeriodicBoundaries;
}

void CVoxelMesh::SaveVoxelMesh(CTextile &Textile, string OutputFilename, int XVoxNum, int YVoxNum, int ZVoxNum,
	bool bOutputMatrix, bool bOutputYarns, int iBoundaryConditions, int iElementType, int FileType)
{
	//PROFILE_SHARED_DEFINE(ProfileTest)
	//PROFILE_FUNC()

	const CDomain* pDomain = Textile.GetDomain();
	if (!pDomain)
	{
		TGERROR("Unable to create ABAQUS input file: No domain specified");
		return;
	}
	//PROFILE_SHARED_BEGIN(ProfileTest);
	m_XVoxels = XVoxNum;
	m_YVoxels = YVoxNum;
	m_ZVoxels = ZVoxNum;
	TGLOG("Calculating voxel sizes");
	if (!CalculateVoxelSizes(Textile))
	{
		TGERROR("Unable to create ABAQUS input file: Error calculating voxel sizes");
		return;
	}
	TGLOG("Replacing spaces in filename with underscore for ABAQUS compatibility");
	OutputFilename = ReplaceFilenameSpaces(OutputFilename);
	//GetYarnGridIntersections(Textile);
	if (FileType == INP_EXPORT)
	{
		CTimer timer;
		timer.start("Timing SaveToAbaqus");
		SaveToAbaqus(OutputFilename, Textile, bOutputMatrix, bOutputYarns, iBoundaryConditions, iElementType);
		timer.check("End of SaveToAbaqus");
		timer.stop();
	}
	else
		SaveVoxelMeshToVTK(OutputFilename, Textile, bOutputYarns);

   // PROFILE_END();
   // PROFILER_UPDATE();
   // PROFILER_OUTPUT("ProfileOutput.txt");
	//SaveToSCIRun( OutputFilename, Textile );
}

/*void CVoxelMesh::SaveShearedVoxelMesh( CTextile &Textile, string OutputFilename, int XVoxNum, int YVoxNum, int ZVoxNum, 
							    bool bOutputMatrix, bool bOutputYarns, int iBoundariesUntiedZ, int iElementType )
{
	const CDomain* pDomain = Textile.GetDomain();
	if (!pDomain)
	{
		TGERROR("Unable to create ABAQUS input file: No domain specified");
		return;
	}
	//PROFILE_BEGIN(Begin);
	m_XVoxels = XVoxNum;
	m_YVoxels = YVoxNum;
	m_ZVoxels = ZVoxNum;
	TGLOG("Calculating voxel sizes");
	
	CalculateShearedVoxelSizes( Textile );
	TGLOG("Replacing spaces in filename with underscore for ABAQUS compatibility");
	OutputFilename = ReplaceFilenameSpaces( OutputFilename );
	//GetYarnGridIntersections(Textile);
	CTimer timer;
	timer.start("Timing SaveToAbaqus");
	SaveToAbaqus( OutputFilename, Textile, bOutputMatrix, bOutputYarns, iBoundariesUntiedZ, iElementType );
	timer.check("End of SaveToAbaqus");
	timer.stop();

    //PROFILE_END();
    //PROFILER_UPDATE();
    //PROFILER_OUTPUT("ProfileOutput.txt");
	//SaveToSCIRun( OutputFilename, Textile );
}*/

// Leaving this in case want to utilise for rendering mesh at some stage (memory allowing)
/*void CVoxelMesh::SaveVoxelMesh(CTextile &Textile, string OutputFilename, int XVoxNum, int YVoxNum, int ZVoxNum, bool bOutputMatrix, bool bOutputYarns )
{
	vector<POINT_INFO> ElementsInfo;
	vector<POINT_INFO>::iterator itElementInfo;
	
	list<int>::iterator itIndex;
	list<int>::iterator itStartIndex;
	int i;
	int iNumNodes = m_Mesh.GetNumNodes(CMesh::HEX);
	int iNumElements;

	m_XVoxels = XVoxNum;
	m_YVoxels = YVoxNum;
	m_ZVoxels = ZVoxNum;
	TGLOG("Calculating voxel sizes");
	CalculateVoxelSizes( Textile );
	TGLOG("Adding nodes");
	
	AddNodes();
	TGLOG("Adding elements");
	AddElements();
	Textile.GetPointInformation(m_Mesh.GetElementCenters(), ElementsInfo);
	list<int> &Indices = m_Mesh.GetIndices(CMesh::HEX);
	TGLOG("Deleting fibre elements");
	iNumElements = ElementsInfo.size();
	if ( !bOutputYarns )
	{
		for ( i = 0, itIndex = Indices.begin(); i < iNumElements && itIndex != Indices.end(); ++i)
		{
			if ( ElementsInfo[i].iYarnIndex == -1 )
			{
				advance( itIndex, iNumNodes );
			}
			else
			{
				itStartIndex = itIndex;
				advance( itIndex, iNumNodes);
				itIndex = Indices.erase( itStartIndex, itIndex );
			}
		}
	}
	else if ( !bOutputMatrix )
	{
		for ( i = 0, itIndex = Indices.begin(), itElementInfo = ElementsInfo.begin(); i < iNumElements && itIndex != Indices.end(); ++i)
		{
			if ( itElementInfo->iYarnIndex == -1 )
			{
				itStartIndex = itIndex;
				advance( itIndex, iNumNodes);
				itIndex = Indices.erase( itStartIndex, itIndex );
				itElementInfo = ElementsInfo.erase( itElementInfo );
			}
			else
			{
				advance( itIndex, iNumNodes );	
				++itElementInfo;
			}
		}
	}
	// else outputting both so don't need to delete anything
	
	TGLOG("Save to ABAQUS");
	//SaveVoxelMeshToVTK( "TestVoxelMesh", ElementsInfo );
	
	m_Mesh.SaveToABAQUS(OutputFilename, bOutputYarns ? &ElementsInfo : NULL, false, false);
	TGLOG("Finished save voxel mesh");
}





void CVoxelMesh::AddNodes()
{
	int x,y,z;
	
	for ( z = 0; z <= m_ZVoxels; ++z )
	{
		for ( y = 0; y <= m_YVoxels; ++y )
		{
			for ( x = 0; x <=m_XVoxels; ++x )
			{
				XYZ Point;
				Point.x = m_DomainAABB.first.x + m_VoxSize[0] * x;
				Point.y = m_DomainAABB.first.y + m_VoxSize[1] * y;
				Point.z = m_DomainAABB.first.z + m_VoxSize[2] * z;
				m_Mesh.AddNode( Point );
			}
		}
	}
}

void CVoxelMesh::AddElements()
{
	int numx = m_XVoxels + 1;
	int numy = m_YVoxels + 1;
	int x,y,z;

	for ( z = 0; z < m_ZVoxels; ++z )
	{
		for ( y = 0; y < m_YVoxels; ++y )
		{
			for ( x = 0; x < m_XVoxels; ++x )
			{
				vector<int> Indices;
				Indices.push_back(x + y*numx + z*numx*numy);
				Indices.push_back((x+1) + y*numx + z*numx*numy);
				Indices.push_back((x+1) +y*numx + (z+1)*numx*numy);
				Indices.push_back(x +y*numx + (z+1)*numx*numy);
				Indices.push_back(x + (y+1)*numx + z*numx*numy);
				Indices.push_back((x+1) + (y+1)*numx + z*numx*numy);
				Indices.push_back((x+1) +(y+1)*numx + (z+1)*numx*numy);
				Indices.push_back(x +(y+1)*numx + (z+1)*numx*numy);
				m_Mesh.AddElement(CMesh::HEX, Indices );
			}
		}
	}
}*/

void CVoxelMesh::SaveVoxelMeshToVTK(string Filename, CTextile &Textile, bool bOutputYarns)
{
	AddExtensionIfMissing(Filename, ".vtu");
	ofstream Output(Filename.c_str());

	OutputNodes(Output, Textile, VTU_EXPORT);

	OutputHexElements(Output, true, true, VTU_EXPORT);

	CMeshData<int> YarnIndex("YarnIndex", CMeshDataBase::ELEMENT);
	CMeshData<XYZ> YarnTangent("YarnTangent", CMeshDataBase::ELEMENT);
	CMeshData<XY> Location("Location", CMeshDataBase::ELEMENT);
	CMeshData<double> VolumeFraction("VolumeFraction", CMeshDataBase::ELEMENT);
	CMeshData<double> SurfaceDistance("SurfaceDistance", CMeshDataBase::ELEMENT);
	CMeshData<XYZ> Orientation("Orientation", CMeshDataBase::ELEMENT);

	vector<POINT_INFO>::iterator itElementInfo;
	
	for (itElementInfo = m_ElementsInfo.begin(); itElementInfo != m_ElementsInfo.end(); ++itElementInfo)
	{
		YarnIndex.m_Data.push_back(itElementInfo->iYarnIndex);
		YarnTangent.m_Data.push_back(itElementInfo->YarnTangent);
		Location.m_Data.push_back(itElementInfo->Location);
		VolumeFraction.m_Data.push_back(itElementInfo->dVolumeFraction);
		SurfaceDistance.m_Data.push_back(itElementInfo->dSurfaceDistance);
		Orientation.m_Data.push_back(itElementInfo->Orientation);
	}
	

	vector<CMeshDataBase*> MeshData;

	MeshData.push_back(&YarnIndex);
	// MeshData.push_back(&YarnTangent);
	// MeshData.push_back(&Location);
	// MeshData.push_back(&VolumeFraction);
	// MeshData.push_back(&SurfaceDistance);
	if (bOutputYarns) {
		MeshData.push_back(&Orientation);
	}

	m_Mesh.SaveToVTK(Filename, &MeshData);
}

void CVoxelMesh::SaveToAbaqus( string Filename, CTextile &Textile, bool bOutputMatrix, bool bOutputYarn, int iBoundaryConditions, int iElementType )
{
	//PROFILE_FUNC();
	AddExtensionIfMissing(Filename, ".inp");

	ofstream Output(Filename.c_str());

	if (!Output)
	{
		TGERROR("Unable to output voxel mesh to ABAQUS file format, could not open file: " << Filename);
		return;
	}

	TGLOG("Saving voxel mesh data to " << Filename);
	
	Output << "*Heading" << "\n";
	Output << "File generated by TexGen v" << TEXGEN.GetVersion() << "\n";

	Output << "************" << "\n";
	Output << "*** MESH ***" << "\n";
	Output << "************" << "\n";
	Output << "*Node" << "\n";
	//PROFILE_BEGIN(OutputNodes);
		OutputNodes(Output, Textile);
	//PROFILE_END();
	TGLOG("Outputting hex elements");
	//Output the voxel HEX elements
	int iNumHexElements = 0;
	if ( !iElementType )
	{
		Output << "*Element, Type=C3D8R" << "\n";
	}
	else
	{
		Output << "*Element, Type=C3D8" << "\n";
	}
	//PROFILE_BEGIN(OutputHexElements);
	iNumHexElements = OutputHexElements( Output, bOutputMatrix, bOutputYarn );
	//PROFILE_END();
	bool bMatrixOnly = false;
	if ( bOutputMatrix && !bOutputYarn )
		bMatrixOnly = true;

	if ( bOutputYarn )
	{
		TGLOG("Outputting orientations & element sets");
		//PROFILE_BEGIN(OutputOrientations);
		OutputOrientationsAndElementSets( Filename, Output );
		//PROFILE_END();
	}
	else if ( bMatrixOnly )
	{
		OutputMatrixElementSet( Filename, Output, iNumHexElements, bMatrixOnly );
	}
	//PROFILE_BEGIN(OutputNodeSets);
	OutputAllNodesSet( Filename, Output );

	// Output material properties
	m_Materials.SetupMaterials( Textile );
	m_Materials.OutputMaterials( Output, Textile.GetNumYarns(), bMatrixOnly );

	//PROFILE_END();
	if ( iBoundaryConditions != NO_BOUNDARY_CONDITIONS )
	{
		//PROFILE_BEGIN(OutputPBCs);
		OutputPeriodicBoundaries( Output, Textile, iBoundaryConditions, bMatrixOnly );
		//PROFILE_END();
	}
	TGLOG("Finished saving to Abaqus");
}

void CVoxelMesh::SaveToSCIRun( string Filename, CTextile &Textile )
{
	Filename = RemoveExtension( Filename, ".inp" );
	Filename += ".hx.pts";

	ofstream NodesFile(Filename.c_str());

	if (!NodesFile)
	{
		TGERROR("Unable to output voxel mesh to SCIRun file format, could not open file: " << Filename);
		return;
	}

	TGLOG("Saving voxel mesh data points to " << Filename);
	
	OutputNodes(NodesFile, Textile, SCIRUN_EXPORT );
	
	Filename = RemoveExtension( Filename, ".pts" );
	Filename += ".hex";

	ofstream OutputElements(Filename.c_str());

	if (!OutputElements)
	{
		TGERROR("Unable to output voxel mesh to SCIRun file format, could not open file: " << Filename);
		return;
	}

	TGLOG("Outputting hex elements");
	//Output the voxel HEX elements
	int iNumHexElements = 0;
	
	iNumHexElements = OutputHexElements( OutputElements, true, true, SCIRUN_EXPORT );
	
	TGLOG("Finished saving to SCIRun format");
}





int CVoxelMesh::OutputHexElements(ostream &Output, bool bOutputMatrix, bool bOutputYarn, int Filetype )
{
	int numx = m_XVoxels + 1;
	int numy = m_YVoxels + 1;
	int x,y,z;
	vector<POINT_INFO>::iterator itElementInfo = m_ElementsInfo.begin();
	int iElementNumber = 1;

	vector<POINT_INFO> NewElementInfo;

	if ( Filetype == SCIRUN_EXPORT )
		Output << m_XVoxels*m_YVoxels*m_ZVoxels << "\n";
	
	for ( z = 0; z < m_ZVoxels; ++z )
	{
		for ( y = 0; y < m_YVoxels; ++y )
		{
			for ( x = 0; x < m_XVoxels; ++x )
			{
				if ( (itElementInfo->iYarnIndex == -1 && bOutputMatrix) 
					 || (itElementInfo->iYarnIndex >=0 && bOutputYarn) )
				{
					if ( Filetype == INP_EXPORT )
					{
						Output << iElementNumber << ", ";
						Output << (x+1) +y*numx + z*numx*numy + 1 << ", " << (x+1) + (y+1)*numx + z*numx*numy + 1 << ", ";
						Output << x + (y+1)*numx + z*numx*numy + 1 << ", " << x + y*numx + z*numx*numy + 1 << ", ";
						Output << (x+1) +y*numx + (z+1)*numx*numy + 1 << ", " << (x+1) +(y+1)*numx + (z+1)*numx*numy + 1 << ", ";
						Output << x +(y+1)*numx + (z+1)*numx*numy + 1 << ", " << x +y*numx + (z+1)*numx*numy + 1 << "\n";
					}
					else if ( Filetype == SCIRUN_EXPORT)
					{
						Output << x +y*numx + z*numx*numy + 1 << ", " << (x+1) + y*numx + z*numx*numy + 1 << ", ";
						Output << x + y*numx + (z+1)*numx*numy + 1 << ", " << (x+1) + y*numx + (z+1)*numx*numy + 1 << ", ";
						Output << x + (y+1)*numx + z*numx*numy + 1 << ", " << (x+1) +(y+1)*numx + z*numx*numy + 1 << ", ";
						Output << x +(y+1)*numx + (z+1)*numx*numy + 1 << ", " << (x+1) + (y+1)*numx + (z+1)*numx*numy + 1 << "\n";
					}
					else  // VTU export
					{
						vector<int> Indices;
						Indices.push_back(x + y*numx + z*numx*numy);
						Indices.push_back((x + 1) + y*numx + z*numx*numy);
						Indices.push_back((x + 1) + y*numx + (z + 1)*numx*numy);
						Indices.push_back(x + y*numx + (z + 1)*numx*numy);
						Indices.push_back(x + (y + 1)*numx + z*numx*numy);
						Indices.push_back((x + 1) + (y + 1)*numx + z*numx*numy);
						Indices.push_back((x + 1) + (y + 1)*numx + (z + 1)*numx*numy);
						Indices.push_back(x + (y + 1)*numx + (z + 1)*numx*numy);
						m_Mesh.AddElement(CMesh::HEX, Indices);
					}
					++iElementNumber;
					if ( bOutputYarn && !bOutputMatrix ) // Just saving yarn so need to make element array with just yarn info
					{
						NewElementInfo.push_back( *itElementInfo );
					}					
					
				}
				++itElementInfo;
			}
		}
	}


	if ( bOutputYarn && !bOutputMatrix )
	{
		m_ElementsInfo.clear();
		m_ElementsInfo = NewElementInfo;
	}
	return ( iElementNumber-1 );
}

void CVoxelMesh::OutputOrientationsAndElementSets( string Filename )
{
	AddExtensionIfMissing(Filename, ".inp");

	ofstream Output(Filename.c_str(), ofstream::app);
	OutputOrientationsAndElementSets( Filename, Output );
}

void CVoxelMesh::OutputOrientationsAndElementSets( string Filename, ostream &Output )
{
	string OrientationsFilename = Filename;
	OrientationsFilename.replace(OrientationsFilename.end()-4, OrientationsFilename.end(), ".ori");
	ofstream OriOutput(OrientationsFilename.c_str());
	string ElementDataFilename = Filename;
	ElementDataFilename.replace(ElementDataFilename.end()-4, ElementDataFilename.end(), ".eld");
	ofstream DataOutput(ElementDataFilename.c_str());

	if (!OriOutput)
	{
		TGERROR("Unable to output orientations, could not open file: " << OrientationsFilename);
		return;
	}
	if (!DataOutput)
	{
		TGERROR("Unable to output additional element data, could not open file: " << ElementDataFilename);
		return;
	}

	TGLOG("Saving element orientations data to " << OrientationsFilename);
	TGLOG("Saving additional element data to " << ElementDataFilename);

	WriteOrientationsHeader( Output );
	Output << "*Distribution Table, Name=TexGenOrientationVectors" << "\n";
	Output << "COORD3D,COORD3D" << "\n";
	Output << "*Distribution, Location=Element, Table=TexGenOrientationVectors, Name=TexGenOrientationVectors, Input=" << StripPath(OrientationsFilename) << "\n";
	Output << "*Orientation, Name=TexGenOrientations, Definition=coordinates" << "\n";
	Output << "TexGenOrientationVectors" << "\n";
	Output << "1, 0" << "\n";

	// Default orientation
	WriteOrientationsHeader( OriOutput );
	OriOutput <<  ", 1.0, 0.0, 0.0,   0.0, 1.0, 0.0" << "\n";

	WriteElementsHeader( DataOutput );

	int i;
	
	map<int, vector<int> > ElementSets;
	vector<POINT_INFO>::iterator itData;
	for (itData = m_ElementsInfo.begin(), i=1; itData != m_ElementsInfo.end(); ++itData, ++i)
	{
		if (itData->iYarnIndex != -1)
		{
			XYZ Up = itData->Up;
			XYZ Dir = itData->Orientation;
			
			XYZ Perp = CrossProduct(Dir, Up);
			Normalise(Perp);
			OriOutput << i << ", " << Dir << ",   " << Perp << "\n";
		}
		else
		{
			// Default orientation
			OriOutput << i << ", 1.0, 0.0, 0.0,   0.0, 1.0, 0.0" << "\n";
		}
		DataOutput << i;
		DataOutput << ", " << itData->iYarnIndex;
		DataOutput << ", " << itData->Location;		// This counts as 2 DepVars
		DataOutput << ", " << itData->dVolumeFraction;
		DataOutput << ", " << itData->dSurfaceDistance;
		DataOutput << "\n";
		ElementSets[itData->iYarnIndex].push_back(i);
	}

	// Output element sets
	Output << "********************" << "\n";
	Output << "*** ELEMENT SETS ***" << "\n";
	Output << "********************" << "\n";
	Output << "** TexGen generates a number of element sets:" << "\n";
	Output << "** All - Contains all elements" << "\n";
	Output << "** Matrix - Contains all elements belonging to the matrix" << "\n";
	Output << "** YarnX - Where X represents the yarn index" << "\n";
	Output << "*ElSet, ElSet=AllElements, Generate" << "\n";
	Output << "1, " << m_ElementsInfo.size() << ", 1" << "\n";
	vector<int>::iterator itIndices;
	map<int, vector<int> >::iterator itElementSet;
	for (itElementSet = ElementSets.begin(); itElementSet != ElementSets.end(); ++itElementSet)
	{
		if (itElementSet->first == -1)
			Output << "*ElSet, ElSet=Matrix" << "\n";
		else
			Output << "*ElSet, ElSet=Yarn" << itElementSet->first << "\n";

		WriteValues(Output, itElementSet->second, 16);
	}	
}

void CVoxelMesh::OutputMatrixElementSet( string Filename, ostream &Output, int iNumHexElements, bool bMatrixOnly )
{
	if ( !bMatrixOnly )
		return;

	// Output element sets
	Output << "********************" << "\n";
	Output << "*** ELEMENT SETS ***" << "\n";
	Output << "********************" << "\n";
	Output << "** TexGen generates a number of element sets:" << "\n";
	Output << "** All - Contains all elements" << "\n";
	Output << "** Matrix - Contains all elements belonging to the matrix" << "\n";
	Output << "** YarnX - Where X represents the yarn index" << "\n";
	Output << "*ElSet, ElSet=Matrix, Generate" << "\n";
	Output << "1, " << iNumHexElements << ", 1" << "\n";
}

void CVoxelMesh::OutputAllNodesSet( string Filename, ostream &Output )
{
	Output << "*****************" << "\n";
	Output << "*** NODE SETS ***" << "\n";
	Output << "*****************" << "\n";
	Output << "** AllNodes - Node set containing all elements" << "\n";
	Output << "*NSet, NSet=AllNodes, Generate" << "\n";
	Output << "1, " << (m_XVoxels+1)*(m_YVoxels+1)*(m_ZVoxels+1) << ", 1" << "\n";
}

void CVoxelMesh::OutputPeriodicBoundaries(ostream &Output, CTextile& Textile, int iBoundaryConditions, bool bMatrixOnly )
{
	m_PeriodicBoundaries->SetDomainSize( Textile.GetDomain()->GetMesh() );

	vector<int> GroupA;
	vector<int> GroupB;
	pair< vector<int>, vector<int> > Faces;

	int numx = m_XVoxels + 1;
	int numy = m_YVoxels + 1;
	int numz = m_ZVoxels + 1;
	//int iDummyNodeNum = numx*numy*numz + 1;
	
	// Create a set of nodes for opposite faces of the domain, then output
	for ( int z = 1; z < numz-1; ++z )
	{
		for ( int y = 1; y < numy-1; ++y )
		{
			GroupA.push_back( numx-1 + y*numx + z*numx*numy +1 );
			GroupB.push_back( y*numx + z*numx*numy + 1);
			
		}
	}
	m_PeriodicBoundaries->SetFaceA( GroupA, GroupB );

	GroupA.clear();
	GroupB.clear();
	for ( int z = 1; z < numz-1; ++z )
	{
		for ( int x = 1; x < numx-1; ++x )
		{
			GroupA.push_back( x + (numy-1)*numx + z*numx*numy +1 );
			GroupB.push_back( x + z*numx*numy + 1);
			
		}
	}
	m_PeriodicBoundaries->SetFaceB( GroupA, GroupB );

	GroupA.clear();
	GroupB.clear();
	for ( int y = 1; y < numy-1; ++y )
	{
		for ( int x = 1; x < numx-1; ++x )
		{
			GroupA.push_back( x + y*numx + (numz-1)*numx*numy + 1);
			GroupB.push_back( x + y*numx + 1 );
			
		}
	}
	m_PeriodicBoundaries->SetFaceC( GroupA, GroupB );

	// Create edge node sets
	vector<int> Edges[4];

	for ( int z = 1; z < numz-1; ++z )
	{
		Edges[0].push_back( z*numx*numy + 1 );
		Edges[3].push_back( numx*(numy-1) + z*numx*numy +1 );
		Edges[2].push_back( (z+1)*numx*numy );
		Edges[1].push_back( numx + z*numx*numy );
	}

	for ( int i = 0; i < 4; ++i )
	{
		m_PeriodicBoundaries->SetEdges( Edges[i] );
		Edges[i].clear();
	}
	
	for ( int y = 1; y < numy-1; ++y )
	{
		Edges[3].push_back( y*numx + (numz-1)*numx*numy + 1 ); 
		Edges[2].push_back( (y+1)*numx +(numz-1)*numx*numy );
		Edges[1].push_back( (y+1)*numx );
		Edges[0].push_back( y*numx + 1 );
	}

	for ( int i = 0; i < 4; ++i )
	{
		m_PeriodicBoundaries->SetEdges( Edges[i] );
		Edges[i].clear();
	}

	for ( int x = 1; x < numx-1; ++x )
	{
		Edges[3].push_back( x + (numz-1)*numx*numy + 1 );
		Edges[2].push_back( x + numx*(numy-1) + (numz-1)*numx*numy +1 );
		Edges[1].push_back( x + numx*(numy-1) + 1 );
		Edges[0].push_back( x + 1 );
	}

	for ( int i = 0; i < 4; ++i )
	{
		m_PeriodicBoundaries->SetEdges( Edges[i] );
		Edges[i].clear();
	}

	// Create vertex sets
	// Vertices rearranged to tie up with Li notation (previous numbering in comments)
	m_PeriodicBoundaries->SetVertex( 1 );//5
	m_PeriodicBoundaries->SetVertex( numx );//8
	m_PeriodicBoundaries->SetVertex( numx*numy );//7
	m_PeriodicBoundaries->SetVertex( (numy-1)*numx + 1 );//6
	m_PeriodicBoundaries->SetVertex( (numz-1)*numx*numy + 1 );//1
	m_PeriodicBoundaries->SetVertex( numx + (numz-1)*numx*numy );//4
	m_PeriodicBoundaries->SetVertex( numx*numy*numz );//3
	m_PeriodicBoundaries->SetVertex( numx*(numy-1) + (numz-1)*numx*numy + 1 );//2
	
	m_PeriodicBoundaries->CreatePeriodicBoundaries( Output, numx*numy*numz + 1, Textile, iBoundaryConditions, bMatrixOnly );
}

void CVoxelMesh::AddElementInfo(vector<POINT_INFO> &RowInfo)
{
	m_ElementsInfo.insert(m_ElementsInfo.end(), RowInfo.begin(), RowInfo.end() );
}

/// Output data with iMaxPerLine elements per line
/*template <typename T>
void CVoxelMesh::WriteValues( ostream &Output, T &Values, int iMaxPerLine)
{
	int iLinePos = 0;
	typename T::const_iterator itValue;
	for (itValue = Values.begin(); itValue != Values.end(); ++itValue)
	{
		if (iLinePos == 0)
		{
			// Do nothing...
		}
		else if (iLinePos < iMaxPerLine)
		{
			Output << ", ";
		}
		else
		{
			Output << endl;
			iLinePos = 0;
		}
		Output << *itValue;
		++iLinePos;
	}
	Output << endl;
}*/

/*void CVoxelMesh::GetYarnGridIntersections( CTextile &Textile )
{
	int x,y,z;
	int iNodeIndex = 1;
	vector<XYZ> CentrePoints;
	vector<POINT_INFO> RowInfo;

	vector<pair<XYZ, XYZ> > xyLines;
	vector<pair<XYZ, XYZ> > xzLines;
	vector<pair<XYZ, XYZ> > yzLines;
	
	// Create lines in z direction to test for intersections
	for ( y = 0; y <= m_YVoxels; ++y )
	{
		for ( x = 0; x <=m_XVoxels; ++x )
		{
			XYZ Point1, Point2;
			Point1.x = Point2.x = m_DomainAABB.first.x + m_VoxSize[0] * x;
			Point1.y = Point2.y = m_DomainAABB.first.y + m_VoxSize[1] * y;
			Point1.z = 0.0;
			Point2.z = 1.0;
			xyLines.push_back( make_pair(Point1,Point2) );
		}
	}

	// Create lines in x direction to test for intersections
	for ( z = 0; z <= m_ZVoxels; ++z )
	{
		for ( y = 0; y <=m_YVoxels; ++y )
		{
			XYZ Point1, Point2;
			Point1.x = 0.0;
			Point2.x = 1.0;
			Point1.y = Point2.y = m_DomainAABB.first.y + m_VoxSize[1] * y;
			Point1.z = Point2.z = m_DomainAABB.first.z + m_VoxSize[2] * z;
			yzLines.push_back( make_pair(Point1,Point2) );
		}
	}

	// Create lines in y direction to test for intersections
	for ( z = 0; z <= m_ZVoxels; ++z )
	{
		for ( x = 0; x <=m_XVoxels; ++x )
		{
			XYZ Point1, Point2;
			Point1.x = Point2.x = m_DomainAABB.first.x + m_VoxSize[0] * x;
			Point1.y = 0.0;
			Point2.y = 1.0;
			Point1.z = Point2.z = m_DomainAABB.first.z + m_VoxSize[2] * z;
			xzLines.push_back( make_pair(Point1,Point2) );
		}
	}
		
	vector<CYarn> &Yarns = Textile.GetYarns();
	vector<CYarn>::iterator itYarn;
	const CDomain* pDomain = Textile.GetDomain();

	//vector< pair<int, pair<double,double>>> xyIntersections;
	vector<vector<pair<int, double> > > xyIntersections;
	vector<vector<pair<int, double> > > xzIntersections;
	vector<vector<pair<int, double> > > yzIntersections;
	xyIntersections.clear();
	xyIntersections.resize(xyLines.size());
	xzIntersections.clear();
	xzIntersections.resize(xzLines.size());
	yzIntersections.clear();
	yzIntersections.resize(yzLines.size());

	int i = 0;
	for ( itYarn = Yarns.begin(); itYarn != Yarns.end(); ++itYarn, ++i)
	{
		CMesh Mesh;
		vector<pair<XYZ, XYZ> >::iterator itLines;

		itYarn->AddSurfaceToMesh(Mesh, *pDomain);
		Mesh.ConvertQuadstoTriangles();

		CalculateIntersections( Mesh, xyLines, i, xyIntersections );
		CalculateIntersections( Mesh, xzLines, i, xzIntersections );
		CalculateIntersections( Mesh, yzLines, i, yzIntersections );
	}
}

void CVoxelMesh::CalculateIntersections( CMesh &Mesh, vector<pair<XYZ,XYZ> > &Lines, int YarnIndex, vector<vector<pair<int, double> > > &Intersections )
{
	vector< pair<double, XYZ> > IntersectionPoints;
	vector< pair<double, XYZ> >::iterator itIntersectionPoints;
	vector<pair<XYZ, XYZ>>::iterator itLines;
	int j = 0;
	double dTol = 1e-8; 

	for( itLines = Lines.begin(); itLines != Lines.end(); ++itLines, ++j )
	{
		int numIntersections;
		IntersectionPoints.clear();
		numIntersections = Mesh.IntersectLine( itLines->first, itLines->second, IntersectionPoints );
		
		pair<int,double>	Intersection;
		Intersection.first = YarnIndex;
		double PrevDist = 0.0;
		for( itIntersectionPoints = IntersectionPoints.begin(); itIntersectionPoints != IntersectionPoints.end(); ++itIntersectionPoints )
		{	
			if ( itIntersectionPoints != IntersectionPoints.begin() && fabs(PrevDist - itIntersectionPoints->first) < dTol )
			{
				continue;
			}
			Intersection.second = itIntersectionPoints->first;
			Intersections[j].push_back(Intersection);
			PrevDist = itIntersectionPoints->first;
		}
	}
}*/