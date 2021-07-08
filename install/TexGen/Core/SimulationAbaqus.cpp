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

#include "PrecompiledHeaders.h"
#include "SimulationAbaqus.h"
#include "AdjustMeshInterference.h"
#include "TexGen.h"
#include "Materials.h"

#define	FIRST_HALF	0
#define SECOND_HALF	1

using namespace TexGen;

CSimulationAbaqus::CSimulationAbaqus(void)
: m_iTotalNumNodes(0)
, m_bIncludePlates(false)
, m_dInitialPlateGap(0)
, m_bWholeSurfaces(false)
{
}

CSimulationAbaqus::~CSimulationAbaqus(void)
{
}

bool CSimulationAbaqus::CreateAbaqusInputFile(CTextile &Textile, string Filename, bool bRegenerateMesh, int iElementType, bool bAdjustMesh, double Tolerance )
{
	//PROFILE_BEGIN(Begin)
	AddExtensionIfMissing(Filename, ".inp");
	TGLOG("Replacing spaces in filename with underscore for ABAQUS compatibility");
	Filename = ReplaceFilenameSpaces( Filename );

	int iNumYarns = Textile.GetNumYarns();
	if (iNumYarns == 0)
		return false;
	const CDomain* pDomain = Textile.GetDomain();
	if (!pDomain)
	{
		TGERROR("Unable to create ABAQUS input file: No domain specified");
		return false;
	}

	m_YarnMeshes.clear();
	int i;
	m_YarnMeshes.resize(iNumYarns);

	
	for (i=0; i<iNumYarns; ++i)  // Create volume mesh for each yarn as MS thesis sections 2.9.1 & 2.9.2
	{
		CYarn* pYarn = Textile.GetYarn(i);
		if ( !pYarn->AddVolumeToMesh(m_YarnMeshes[i], *pDomain))// || m_YarnMeshes[i].NodesEmpty())
		{
			TGERROR("Unable to create ABAQUS input file: Failed to create volume mesh for yarn " << i );
			return false;
		}
	}

	const vector<XYZ> &Repeats = Textile.GetYarn(0)->GetRepeats();
	map<string, vector<ELEMENT_FACE> > SurfaceDefinitions;
	
	if ( !bRegenerateMesh )  // This is time consuming - if regenerating will get surfaces later
	{
		//PROFILE_BLOCK(GetYarnSurfaces)
		CreateSurfaceDefinitions( iNumYarns, Repeats, SurfaceDefinitions);
	}

	vector<POINT_INFO> YarnElementInfo;
	vector< vector<POINT_INFO> > YarnElementTypesInfo;

	if ( bAdjustMesh )
	{
		CAdjustMeshInterference AdjustMesh;
		
		TGLOG("Adjusting mesh");
		if ( !AdjustMesh.AdjustMesh( Textile, m_YarnMeshes, Tolerance ) )
		{
			TGERROR("Unable to create ABAQUS input file with adjusted mesh: Intersection depths too large" );
			return false;
		}

		if ( bRegenerateMesh )
		{
			TGLOG("Regenerating mesh using adjusted points");
			AdjustMesh.AdjustSectionMeshes( Textile, m_YarnMeshes );

			m_YarnMeshes.clear();
			m_YarnMeshes.resize(iNumYarns);
			for (i=0; i<iNumYarns; ++i)  // Create volume mesh for each yarn as MS thesis sections 2.9.1 & 2.9.2
			{
				CYarn* pYarn = Textile.GetYarn(i);
				if ( !pYarn->AddVolumeToMesh(m_YarnMeshes[i], *pDomain))// || m_YarnMeshes[i].NodesEmpty())
				{
					TGERROR("Unable to create ABAQUS input file: Failed to create volume mesh for yarn " << i );
					return false;
				}
			}

			SurfaceDefinitions.clear();
			CreateSurfaceDefinitions( iNumYarns, Repeats, SurfaceDefinitions);
		}
	}
	
	//BuildIndexOffsets();
	m_TextileMesh.Clear();
	vector<POINT_INFO> ElementsInfo;
	TGLOG("Getting point information");
	{
	//	PROFILE_BLOCK(GetPointInformation)
	for ( i = 0; i < iNumYarns; ++i )
	{
		for (int j = 0; j < CMesh::NUM_ELEMENT_TYPES; ++j )
		{
			if ( j == CMesh::POLYGON )
				continue;
			YarnElementInfo.clear();
			Textile.GetPointInformation( m_YarnMeshes[i].GetElementCenters( (CMesh::ELEMENT_TYPE)j ), YarnElementInfo, i, 0.005 );
			if ( i == 0 )  // If first yarn add vector for each element type
				YarnElementTypesInfo.push_back( YarnElementInfo );
			else			// Subsequent yarns add to end of appropriate element vector
			{
				
				YarnElementTypesInfo[j].insert( YarnElementTypesInfo[j].end(), YarnElementInfo.begin(), YarnElementInfo.end() );
			}
		}
	}
	vector< vector<POINT_INFO> >::iterator itYarnElementTypesInfo;
	for ( itYarnElementTypesInfo = YarnElementTypesInfo.begin(); itYarnElementTypesInfo != YarnElementTypesInfo.end(); ++itYarnElementTypesInfo )
	{
		ElementsInfo.insert( ElementsInfo.end(), itYarnElementTypesInfo->begin(), itYarnElementTypesInfo->end() );
	}
	}
	vector<SECTION_VF_DATA> VolFractionData;

	for(i=0; i<iNumYarns; ++i)  // Add meshes into single mesh
	{
		if ( !m_YarnMeshes[i].NodesEmpty() )
		{
			if ( bAdjustMesh && !bRegenerateMesh )  // Can just use volume fractions straight from textile if have regenerated mesh
			{
				TGLOG("Recalculating volume fractions for adjusted mesh");
				GetSectionVolumeFractions( Textile, VolFractionData, i );
				
			}
			m_YarnMeshes[i].RemoveElementType(CMesh::POLYGON);	// Don't need POLYGON sections any more & dont want to copy into global mesh
			m_YarnMeshes[i].RemoveUnreferencedNodes();
			m_TextileMesh.InsertMesh(m_YarnMeshes[i]);
		}
#ifdef _DEBUG
		m_YarnMeshes[i].SaveToVTK( "c:\\Program Files\\TexGen\\AdjustedMesh" + stringify(i) );
#endif
	}
	
	// then assemble

	BuildIndexOffsets();

	if ( bAdjustMesh && !bRegenerateMesh)
	{
		vector<SECTION_VF_DATA> MidVFData;
		CreateMidPointVolumeFractions( VolFractionData, MidVFData );
		GetElementVolumeFractions( ElementsInfo, MidVFData );
	}
	
	TGLOG("Saving to ABAQUS");
	m_TextileMesh.SaveToABAQUS(Filename, &ElementsInfo, false, false, iElementType);
	m_iTotalNumNodes = m_TextileMesh.GetNumNodes();

	ofstream Output(Filename.c_str(), ios::app);

	m_Materials.SetupMaterials( Textile );
	TGLOG("Creating materials");
	CreateMaterials(Output, Filename);
	TGLOG("Creating surfaces");
	CreateSurfaces(Output, SurfaceDefinitions);
	if (m_bIncludePlates)
	{
		TGLOG("Creating compression plates");
		CreateCompressionPlates(Output);
	}

	vector<int> YarnCenterNodes;
	int iNodeIndex;
	for (i=0; i<iNumYarns; ++i)
	{
		if ( !m_YarnMeshes[i].NodesEmpty() )
		{
			iNodeIndex = GetGlobalNodeIndex(i, GetCenterNode(m_YarnMeshes[i]));
			YarnCenterNodes.push_back(iNodeIndex);
		}
	}

	TGLOG("Creating periodic boundaries");
	CreatePeriodicBoundaries(Output, *pDomain);
	vector<CLinearTransformation>::iterator itDeformation;

	
	TGLOG("Creating step");
	CreateStep(Output);

	TGLOG("Creating contacts");
	CreateContacts(Output, Textile);
	for (itDeformation = m_DeformationSteps.begin(); itDeformation != m_DeformationSteps.end(); ++itDeformation)
	{
		CreateBoundary(Output, *itDeformation, YarnCenterNodes, Repeats);
	}
	TGLOG("Completed ABAQUS input file");
	//PROFILE_END();
    //PROFILER_UPDATE();
    //PROFILER_OUTPUT("ProfileDryFibreExportOutput.txt");
	return true;
}

void CSimulationAbaqus::CreateSurfaceDefinitions( int iNumYarns, const vector<XYZ> &Repeats, map<string, vector<ELEMENT_FACE> > &SurfaceDefinitions)
{
	for (int i=0; i<iNumYarns; ++i)
	{
		TGLOG("Creating yarn " << i << " surface definitions");
		if ( m_bWholeSurfaces )
		{
			vector<ELEMENT_FACE> Faces;
			GetYarnSurface( i, Repeats, Faces );
			SurfaceDefinitions["YarnSurf" + stringify(i)] = Faces;
		}
		else
		{
			vector<ELEMENT_FACE> UpperFaces, LowerFaces;
			GetYarnSurfaces( i, Repeats, UpperFaces, LowerFaces );
			SurfaceDefinitions["Yarn" + stringify(i) + "Lower"] = LowerFaces;
			SurfaceDefinitions["Yarn" + stringify(i) + "Upper"] = UpperFaces;
		}
	}
}

void CSimulationAbaqus::GetYarnSurfaces(int iYarn, const vector<XYZ> &Repeats, vector<ELEMENT_FACE> &UpperFaces, vector<ELEMENT_FACE> &LowerFaces )
{
	CMesh &VolumeMesh = m_YarnMeshes[iYarn];

	CMesh SurfaceMesh = VolumeMesh;
	SurfaceMesh.RemoveElementType(CMesh::POLYGON);
	SurfaceMesh.RemoveUnreferencedNodes();
	SurfaceMesh.ConvertToSurfaceMesh();

	// Make a list of nodes which lie on the yarn's end
	set<int> BoundaryNodes;
	vector<XYZ>::const_iterator itRepeat;
	for (itRepeat=Repeats.begin(); itRepeat!=Repeats.end(); ++itRepeat)
	{
		vector<pair<int, int> > NodePairs;
		SurfaceMesh.GetNodePairs(*itRepeat, NodePairs);
		vector<pair<int, int> >::iterator itNodePair;
		for (itNodePair = NodePairs.begin(); itNodePair != NodePairs.end(); ++itNodePair)
		{
			BoundaryNodes.insert(itNodePair->first);
			BoundaryNodes.insert(itNodePair->second);
		}
	}

	// Get the yarn surface
	int i, iType, iBoundaryNodes;
	list<int>::iterator itIndex;
	
	ELEMENT_FACE Face;
	for (iType = 0; iType < CMesh::NUM_ELEMENT_TYPES; ++iType)
	{
		CMesh::ELEMENT_TYPE Type = (CMesh::ELEMENT_TYPE)iType;
		if ( Type != CMesh::POLYGON )
		{
			list<int> &Indices = SurfaceMesh.GetIndices(Type);
			for (itIndex = Indices.begin(); itIndex != Indices.end(); )
			{
				vector<int> ElemIndices;
				iBoundaryNodes = 0;
				for (i=0; i<CMesh::GetNumNodes(Type); ++i)
				{
					iBoundaryNodes += (int)BoundaryNodes.count(*itIndex);
					ElemIndices.push_back(*itIndex);
					++itIndex;
				}
				// Filter out any faces which lie on the yarn boundary
				if (iBoundaryNodes != (int)ElemIndices.size())
				{
					XYZ N1 = SurfaceMesh.GetNode(ElemIndices[0]);
					XYZ N2 = SurfaceMesh.GetNode(ElemIndices[1]);
					XYZ N3 = SurfaceMesh.GetNode(ElemIndices[2]);
					XYZ Normal = CrossProduct(N2-N1, N3-N1);
					// Let's filter out all the faces based on their normal
					Face = FindFaceIndex(iYarn, ElemIndices);
					if ( Normal.z > 0 ) // Upper surface
					{
						UpperFaces.push_back(Face);
					}
					else // Lower surface
					{
						LowerFaces.push_back(Face);
					}
				}
			}
		}
	}
}

void CSimulationAbaqus::GetYarnSurface(int iYarn, const vector<XYZ> &Repeats, vector<ELEMENT_FACE> &Faces )
{
	// This function assumes that the yarn is repeating. If no repeats are specified, or the domain does not
	// match a whole number of repeats then no surface will be saved
	CMesh &VolumeMesh = m_YarnMeshes[iYarn];

	CMesh SurfaceMesh = VolumeMesh;
	SurfaceMesh.RemoveElementType(CMesh::POLYGON);
	SurfaceMesh.RemoveUnreferencedNodes();
	//SurfaceMesh.SaveToVTK("C://Users//epzlpb//TexGen//Knit//VolumeMesh");
	SurfaceMesh.ConvertToSurfaceMesh();
	//SurfaceMesh.SaveToVTK("C://Users//epzlpb//TexGen//Knit//SurfaceMesh");

	// Make a list of nodes which lie on the yarn's end
	set<int> BoundaryNodes;
	vector<XYZ>::const_iterator itRepeat;
	for (itRepeat=Repeats.begin(); itRepeat!=Repeats.end(); ++itRepeat)
	{
		vector<pair<int, int> > NodePairs;
		SurfaceMesh.GetNodePairs(*itRepeat, NodePairs);
		vector<pair<int, int> >::iterator itNodePair;
		for (itNodePair = NodePairs.begin(); itNodePair != NodePairs.end(); ++itNodePair)
		{
			BoundaryNodes.insert(itNodePair->first);
			BoundaryNodes.insert(itNodePair->second);
		}
	}

	// Get the yarn surface
	int i, iType, iBoundaryNodes;
	list<int>::iterator itIndex;
	
	ELEMENT_FACE Face;
	for (iType = 0; iType < CMesh::NUM_ELEMENT_TYPES; ++iType)
	{
		CMesh::ELEMENT_TYPE Type = (CMesh::ELEMENT_TYPE)iType;
		if ( Type != CMesh::POLYGON )
		{
			list<int> &Indices = SurfaceMesh.GetIndices(Type);
			for (itIndex = Indices.begin(); itIndex != Indices.end(); )
			{
				vector<int> ElemIndices;
				iBoundaryNodes = 0;
				for (i=0; i<CMesh::GetNumNodes(Type); ++i)
				{
					iBoundaryNodes += (int)BoundaryNodes.count(*itIndex);
					ElemIndices.push_back(*itIndex);
					++itIndex;
				}
				// Filter out any faces which lie on the yarn boundary
				if (iBoundaryNodes != (int)ElemIndices.size())
				{
					Face = FindFaceIndex(iYarn, ElemIndices);
					Faces.push_back(Face);
				}
			}
		}
	}
}

CSimulationAbaqus::ELEMENT_FACE CSimulationAbaqus::FindFaceIndex(int iYarn, const vector<int> &SurfIndices)
{
	CMesh VolumeMesh = m_YarnMeshes[iYarn];

	VolumeMesh.RemoveElementType(CMesh::POLYGON);
	VolumeMesh.RemoveUnreferencedNodes();

	list<int>::iterator itIndex;
	int i, iElementIndex, iType;
	for (iType = 0; iType < CMesh::NUM_ELEMENT_TYPES; ++iType)
	{
		if ( iType == CMesh::POLYGON )
			continue;
		CMesh::ELEMENT_TYPE Type = (CMesh::ELEMENT_TYPE)iType;
		list<int> &Indices = VolumeMesh.GetIndices(Type);
		for (itIndex = Indices.begin(), iElementIndex=0; itIndex != Indices.end(); ++iElementIndex)
		{
			vector<int> VolIndices;
			for (i=0; i<CMesh::GetNumNodes(Type); ++i)
			{
				VolIndices.push_back(*(itIndex++));
			}
			set<int> CommonIndices = GetCommonIndices(SurfIndices, VolIndices);
			if (CommonIndices.size() == SurfIndices.size())
			{
				// Bingo, we have a match...
				ELEMENT_FACE Face;
				Face.ElementType = Type;
				Face.iElementIndex = iElementIndex;
				Face.iFaceIndex = GetFaceIndex(Type, CommonIndices);
				Face.iYarn = iYarn;
				return Face;
			}
		}
	}
	// We should never reach this point
	assert(false);
	return ELEMENT_FACE();
}

int CSimulationAbaqus::GetFaceIndex(CMesh::ELEMENT_TYPE ElemType, const set<int> &NodeIndices)
{
	// Face indices taken from abaqus manual 22.1.4 Three-dimensional solid element library
	switch (ElemType)
	{
	case CMesh::HEX:
		if (NodeIndices.count(0) && NodeIndices.count(1) && NodeIndices.count(2) && NodeIndices.count(3))
			return 0;
		if (NodeIndices.count(4) && NodeIndices.count(5) && NodeIndices.count(6) && NodeIndices.count(7))
			return 1;
		if (NodeIndices.count(0) && NodeIndices.count(1) && NodeIndices.count(4) && NodeIndices.count(5))
			return 2;
		if (NodeIndices.count(1) && NodeIndices.count(2) && NodeIndices.count(5) && NodeIndices.count(6))
			return 3;
		if (NodeIndices.count(2) && NodeIndices.count(3) && NodeIndices.count(6) && NodeIndices.count(7))
			return 4;
		if (NodeIndices.count(3) && NodeIndices.count(0) && NodeIndices.count(7) && NodeIndices.count(4))
			return 5;
		break;
	case CMesh::WEDGE:
		// Note face indices 0 and 1 are switched because of the diference in local node numbering in
		// ABAQUS and VTK
		if (NodeIndices.count(0) && NodeIndices.count(1) && NodeIndices.count(2))
			return 1;
		if (NodeIndices.count(3) && NodeIndices.count(4) && NodeIndices.count(5))
			return 0;
		if (NodeIndices.count(0) && NodeIndices.count(1) && NodeIndices.count(3) && NodeIndices.count(4))
			return 2;
		if (NodeIndices.count(1) && NodeIndices.count(2) && NodeIndices.count(4) && NodeIndices.count(5))
			return 3;
		if (NodeIndices.count(2) && NodeIndices.count(0) && NodeIndices.count(5) && NodeIndices.count(3))
			return 4;
		break;
	}
	assert(false);
	return -1;
}

set<int> CSimulationAbaqus::GetCommonIndices(const vector<int> &SurfIndices, const vector<int> &VolIndices)
{
	set<int> Common;
	vector<int>::const_iterator itSurf;
	vector<int>::const_iterator itVol;
	int i;
	for (itSurf = SurfIndices.begin(); itSurf != SurfIndices.end(); ++itSurf)
	{
		for (itVol = VolIndices.begin(), i=0; itVol != VolIndices.end(); ++itVol, ++i)
		{
			if (*itSurf == *itVol)
			{
				Common.insert(i);
			}
		}
	}
	return Common;
}

void CSimulationAbaqus::BuildIndexOffsets()
{
	// In TexGen we have 1 mesh for each yarn and each mesh has
	// a seperate list for each element type.
	// In ABAQUS we have 1 mesh for the entire model and element
	// indices are shared for all element types.
	// We need to convert TexGen indices to ABAQUS indices. In
	// order to do this we need an index offset for each element
	// type and yarn number. This is what this code below does
	int i, iNumYarns = (int)m_YarnMeshes.size();
	int iType;
	int iElemIndexOffset = 0;
	m_ElementIndexOffsets.clear();
	for (iType = 0; iType < CMesh::NUM_ELEMENT_TYPES; ++iType)
	{
		CMesh::ELEMENT_TYPE Type = (CMesh::ELEMENT_TYPE)iType;
		for (i=0; i<iNumYarns; ++i)
		{
			m_ElementIndexOffsets[i][Type] = iElemIndexOffset;
			iElemIndexOffset += m_YarnMeshes[i].GetNumElements(Type);
		}
	}
	m_NodeIndexOffsets.clear();
	int iNodeIndexOffset = 0;
	for (i=0; i<iNumYarns; ++i)
	{
		m_NodeIndexOffsets[i] = iNodeIndexOffset;
		iNodeIndexOffset += m_YarnMeshes[i].GetNumNodes();
	}
}

int CSimulationAbaqus::GetGlobalElementIndex(int iYarn, CMesh::ELEMENT_TYPE ElemType, int iIndex)
{
	return m_ElementIndexOffsets[iYarn][ElemType] + iIndex;
}

int CSimulationAbaqus::GetGlobalElementIndex(ELEMENT_FACE Face)
{
	return GetGlobalElementIndex(Face.iYarn, Face.ElementType, Face.iElementIndex);
}

int CSimulationAbaqus::GetGlobalNodeIndex(int iYarn, int iIndex)
{
	return m_NodeIndexOffsets[iYarn] + iIndex;
}

void CSimulationAbaqus::CreateMaterials(ostream &Output, string Filename)
{
	string ElementDataFilename = Filename;
	ElementDataFilename.replace(ElementDataFilename.end()-4, ElementDataFilename.end(), ".eld");
	Output << "*****************" << endl;
	Output << "*** MATERIALS ***" << endl;
	Output << "*****************" << endl;
	
	map<string, pair<CObjectContainer<CMaterial>, CObjectContainer<CMaterial> > > Materials = m_Materials.GetMaterials();
	map<int, string> MaterialAssignements = m_Materials.GetMaterialAssignements();
	
	map<string, pair<CObjectContainer<CMaterial>, CObjectContainer<CMaterial> > >::iterator itMaterial;
	for (itMaterial = Materials.begin(); itMaterial != Materials.end(); ++itMaterial)
	{
		Output << "*Material, Name=" << itMaterial->first << endl;
		if ( itMaterial->second.first->GetConstants().size() == 2 )
			Output << itMaterial->second.first->GetAbaqusCommands();	
		else
			Output << itMaterial->second.first->GetAbaqusCommands( "ENGINEERING CONSTANTS" );
		Output << "*Damping, alpha= 3.75e+6" << endl;
		Output << "*Density" << endl;
		Output << "1.37e-09," << endl;
		Output << "*DepVar" << endl;
		Output << "5" << endl;
	}
	int i, iNumYarns = (int)m_YarnMeshes.size();
	string MatName;
	for (i = 0; i < iNumYarns; ++i)
	{
		if (MaterialAssignements.count(i))
			MatName = MaterialAssignements[i];
		else
			MatName = Materials.begin()->first;
		Output << "*Solid Section, ElSet=Yarn" << i << ", Material=" << MatName << ", Orientation=TexGenOrientations, controls=HourglassEnhanced" << endl;
		Output << "1.0," << endl;
	}

	Output << "*Section Controls, Name=HourglassEnhanced, Hourglass=Enhanced, SECOND ORDER ACCURACY=YES" << endl;
	Output << "** Note: Additional element data are stored as a depvars:" << endl;
	WriteElementsHeader( Output );
	Output << "*Initial Conditions, Type=Solution, Input=" << StripPath(ElementDataFilename) << endl;
}

void CSimulationAbaqus::CreateSurfaces(ostream &Output, map< string, vector<ELEMENT_FACE> > &SurfaceDefinitions)
{
	// Now output the surface definitions
	Output << "***************************" << endl;
	Output << "*** SURFACE DEFINITIONS ***" << endl;
	Output << "***************************" << endl;
	vector<ELEMENT_FACE>::iterator itFace;
	
	map<string, vector<ELEMENT_FACE> >::iterator itSurfDef;
	
	for (itSurfDef = SurfaceDefinitions.begin(); itSurfDef != SurfaceDefinitions.end(); ++itSurfDef)
	{
		Output << "*Surface, Name=" << itSurfDef->first << ", Type=Element" << endl;
		for (itFace = itSurfDef->second.begin(); itFace != itSurfDef->second.end(); ++itFace)
		{
			// ABAQUS is 1 based, so add 1
			Output << GetGlobalElementIndex(*itFace)+1 << ", S" << itFace->iFaceIndex+1 << endl;
		}
	}
}

void CSimulationAbaqus::CreateContacts(ostream &Output, const CTextile &Textile)
{
	Output << "***************************" << endl;
	Output << "*** CONTACT DEFINITIONS ***" << endl;
	Output << "***************************" << endl;
	
	const CTextileWeave* pWeave = dynamic_cast<const CTextileWeave*>(&Textile);
	const CTextile3DWeave* p3DWeave = dynamic_cast<const CTextile3DWeave*>(&Textile);
	if (pWeave)
	{
		CreateContacts(Output, *pWeave);
	}
	else if (p3DWeave)
	{
		Output << "*** WARNING: Contacts are only created for upper and lower surfaces." << endl;
		Output << "***          Any contacts between binder yarns and sides of warp and weft" << endl;
		Output << "***          yarns must be specified manually" << endl;
		CreateContacts(Output, *p3DWeave);
	}
	else
	{
		Output << "*** WARNING: TexGen was unable to create contact definitions automatically" << endl;
		Output << "***          because the textile is not of type CTextileWeave" << endl;
	}

	// Create contacts with the plate
	if (m_bIncludePlates)
	{
		Output << "*Surface Interaction, Name=Plate" << endl;
		if (m_PlateInteraction)
			Output << m_PlateInteraction->GetAbaqusCommands();
		int i;
		for (i=0; i<Textile.GetNumYarns(); ++i)
		{
			if ( m_bWholeSurfaces )
			{
				CreateContact(Output, "YarnSurf" + stringify(i), "TopPlate", "Plate");
				CreateContact(Output, "YarnSurf" + stringify(i), "BottomPlate", "Plate");			  
			}
			else
			{
				CreateContact(Output, "Yarn" + stringify(i) + "Upper", "TopPlate", "Plate");
				CreateContact(Output, "Yarn" + stringify(i) + "Lower", "BottomPlate", "Plate");
			}
		}
	}
}

void CSimulationAbaqus::CreateContacts(ostream &Output, const CTextileWeave &Weave)
{
	if (m_YarnInteraction)
		Output << m_YarnInteraction->GetAbaqusCommands();
	set<pair<int, int> > Contacts;
	int i, j, k;
	for (i=0; i<Weave.GetNumYYarns(); ++i)
	{
		for (j=0; j<Weave.GetNumXYarns(); ++j)
		{
			const vector<PATTERN2D> &Cell = Weave.GetCell(i, j);
			for (k=1; k<(int)Cell.size(); ++k)
			{
				Contacts.insert(make_pair(Weave.GetYarnIndex(i, j, k-1), Weave.GetYarnIndex(i, j, k)));
			}
		}
	}
	OutputContacts( Output, Contacts );
}

void CSimulationAbaqus::CreateContacts(ostream &Output, const CTextile3DWeave &Weave)
{
	if (m_YarnInteraction)
		Output << m_YarnInteraction->GetAbaqusCommands();
	set<pair<int, int> > Contacts;
	int i, j, k;
	for (i=0; i<Weave.GetNumYYarns(); ++i)
	{
		for (j=0; j<Weave.GetNumXYarns(); ++j)
		{
			const vector<PATTERN3D> &Cell = Weave.GetCell(i, j);
			for (k=1; k<(int)Cell.size(); ++k)
			{
				if ( Cell[k-1] != PATTERN3D_NOYARN && Cell[k] != PATTERN3D_NOYARN )
					Contacts.insert(make_pair(Weave.GetYarnIndex(i, j, k-1), Weave.GetYarnIndex(i, j, k)));
			}
		}
	}
	OutputContacts( Output, Contacts );
}

void CSimulationAbaqus::OutputContacts( ostream &Output, set<pair<int, int> > &Contacts )
{
	set<pair<int, int> >::iterator itContact;
	for (itContact = Contacts.begin(); itContact != Contacts.end(); ++itContact)
	{
		if ( m_bWholeSurfaces )
			CreateContact(Output, "YarnSurf" + stringify(itContact->first),
							  "YarnSurf" + stringify(itContact->second) , "Yarn");
		else
			CreateContact(Output, "Yarn" + stringify(itContact->first) + "Upper",
							  "Yarn" + stringify(itContact->second) + "Lower", "Yarn");
	}
}

void CSimulationAbaqus::CreateCompressionPlates(ostream &Output)
{
	Output << "**************************" << endl;
	Output << "*** COMPRESSION PLATES ***" << endl;
	Output << "**************************" << endl;

	vector<CMesh>::iterator itMesh;
	pair<XYZ, XYZ> YarnAABB;
	pair<XYZ, XYZ> TextileAABB;
	for (itMesh = m_YarnMeshes.begin(); itMesh != m_YarnMeshes.end(); ++itMesh)
	{
		YarnAABB = itMesh->GetAABB(m_dInitialPlateGap);
		if (itMesh == m_YarnMeshes.begin())
		{
			TextileAABB = YarnAABB;
		}
		else
		{
			TextileAABB.first = Min(TextileAABB.first, YarnAABB.first);
			TextileAABB.second = Max(TextileAABB.second, YarnAABB.second);
		}
	}
	double dWidth = TextileAABB.second.x-TextileAABB.first.x;
	double dMinX = TextileAABB.first.x - dWidth;
	double dMaxX = TextileAABB.second.x + dWidth;
	double dLocalY, dZ;
	int i, iDummyNodeNum;
	string Name;
	m_TopPlatePos.z = TextileAABB.second.z;
	m_BottomPlatePos.z = TextileAABB.first.z;
	XYZ GeneratorDir;
	for (i=0; i<2; ++i)
	{
		if (i==0)
		{
			Name = "TopPlate";
			dZ = m_TopPlatePos.z;
			GeneratorDir = XYZ(0, -1, 0);		// Make sure the normal is correct
			dLocalY = -dZ;
		}
		else
		{
			Name = "BottomPlate";
			dZ = m_BottomPlatePos.z;
			GeneratorDir = XYZ(0, 1, 0);		// Make sure the normal is correct
			dLocalY = dZ;
		}
		iDummyNodeNum = ++m_iTotalNumNodes;	// Note this is now a 1-based node index
		Output << "*Node" << endl;
		Output << iDummyNodeNum << ", 0, 0, " << dZ << endl;
		Output << "*NSet, NSet=" << Name << endl;
		Output << iDummyNodeNum << endl;
		Output << "*Surface, Type=Cylinder, Name=" << Name << endl;
		Output << "0, 0, 0, 1, 0, 0" << endl;
		Output << GeneratorDir << endl;
		Output << "Start, " << dMinX << ", " << dLocalY << endl;
		Output << "Line, " << dMaxX << ", " << dLocalY << endl;
		Output << "*Rigid Body, Analytical Surface=" << Name << ", Ref Node=" << Name << endl;
	}
}

int CSimulationAbaqus::GetCenterNode(const CMesh &Mesh)
{
	pair<XYZ, XYZ> AABB = Mesh.GetAABB();

	XYZ Center = 0.5*(AABB.first + AABB.second);

	return Mesh.GetClosestNode(Center);
}

void CSimulationAbaqus::CreatePeriodicBoundaries(ostream &Output, const CDomain &TextileDomain)
{
	//PROFILE_FUNC()
	Output << "************************************" << endl;
	Output << "*** PERIODIC BOUNDARY CONDITIONS ***" << endl;
	Output << "************************************" << endl;

	vector<pair<int, int> > NodePairs;
	int i, j;

	pair<XYZ, XYZ> DomainAABB = TextileDomain.GetMesh().GetAABB();
	vector<XYZ> DomainSize;
	DomainSize.push_back(XYZ(DomainAABB.second.x - DomainAABB.first.x, 0,0));
	DomainSize.push_back(XYZ(0, DomainAABB.second.y - DomainAABB.first.y, 0));
	DomainSize.push_back(XYZ(0, 0, DomainAABB.second.z - DomainAABB.first.z));
	
	for ( i = 0; i < (int)DomainSize.size(); ++i )
	{
		NodePairs.clear();
		m_TextileMesh.GetNodePairs(DomainSize[i], NodePairs);
		if ( NodePairs.size() > 0 )
		{
			CreateSet(Output, "Bound" + stringify(i), NodePairs);
			int iDummyNodeNum = ++m_iTotalNumNodes;	// Note this is now a 1-based node index
			Output << "*Node" << endl;
			Output << iDummyNodeNum << ", 0, 0, 0" << endl;
			Output << "*NSet, NSet=Dummy" << i << endl;
			Output << iDummyNodeNum << endl;
			for (j=0; j<3; ++j)
			{
				Output << "*Equation\n3\n";
				Output << "Bound" << i << "A, " << j+1 << ", 1.0, Bound" << i << "B, " << j+1 << ", -1.0, Dummy" << i << ", " << j+1 << ", 1.0" << endl;
			}
		}
	}
	
	pair<XYZ, XYZ> Domain = m_TextileMesh.GetAABB();
	
	Output << "*** Nodes containing domain AABB ***" << endl;
	Output << "*Node" << endl;
	Output << ++m_iTotalNumNodes << ", " << Domain.first.x << ", " << Domain.first.y << ", " << Domain.first.z << endl;
	Output << ++m_iTotalNumNodes << ", " << Domain.second.x << ", " << Domain.second.y << ", " << Domain.second.z << endl;
	Output << "*NSet, NSet=Domain" << endl;
	Output << m_iTotalNumNodes-1 << ", " << m_iTotalNumNodes << endl;
}

void CSimulationAbaqus::CreateStep(ostream &Output)
{
	Output << "************" << endl;
	Output << "*** INTERACTION PROPERTIES ***" << endl;
	Output << "************" << endl;
	Output << "*Surface Interaction, name=YARN" << endl;
	Output << "*Friction" << endl;
	Output << "0.112," << endl;
	Output << "*Surface Behavior, pressure-overclosure=HARD" << endl;

	Output << "************" << endl;
	Output << "*** STEP ***" << endl;
	Output << "************" << endl;
	Output << "*Step, NLGeom=Yes" << endl;
	Output << "*Dynamic, Explicit" << endl;
	Output << ",2.5e-5" << endl;
	Output << "*Bulk Viscosity" << endl;
	Output << "0.06, 1.2" << endl;
	if (!m_StaticStepParameters.empty())
		Output << m_StaticStepParameters << endl;
}

void CSimulationAbaqus::CreateBoundary(ostream &Output, CLinearTransformation Deformation, const vector<int> &YarnCenterNodes, const vector<XYZ> &Repeats)
{
	Output << "*Output, History, Variable=PRESELECT" << endl;
	Output << "*Output, Field, Variable=PRESELECT" << endl;
	Output << "*Element Output" << endl;
	Output << "SDV" << endl;
//	Output << "*Node Print" << endl;
//	Output << "U1, U2, U3" << endl;

	Output << "*Boundary" << endl;
	Output << "*Amplitude, name=Amp-1" << endl;
	Output << "0., 0., 2.5e-05, 1." << endl;
	Output << "*Boundary, amplitude=Amp-1" << endl;
	Output << "*** Constrain the nodes closest to the geometrical center of each yarns" << endl;
//	Output << GetCenterNode(m_TextileMesh)+1 << ", 1, 3, 0" << endl;
	XYZ Position, Displacement;
	vector<int>::const_iterator itCenterNode;
	int i, j;
	for (itCenterNode = YarnCenterNodes.begin(); itCenterNode != YarnCenterNodes.end(); ++itCenterNode)
	{
		Position = m_TextileMesh.GetNode(*itCenterNode);
		Displacement = Deformation * Position - Position;
		for (j=0; j<3; ++j)
		{
			Output << (*itCenterNode)+1 << ", " << j+1 << ", " << j+1 << ", " << Displacement[j] << endl;
		}
	}
	if (m_bIncludePlates)
	{
		Output << "*** Constrain the plates" << endl;
		XYZ TopDisp = Deformation * m_TopPlatePos - m_TopPlatePos;
		XYZ BottomDisp = Deformation * m_BottomPlatePos - m_BottomPlatePos;
		for (j=0; j<3; ++j)
		{
			Output << "TopPlate" << ", " << j+1 << ", " << j+1 << ", " << TopDisp[j] << endl;
			Output << "BottomPlate" << ", " << j+1 << ", " << j+1 << ", " << BottomDisp[j] << endl;
		}
		Output << "TopPlate" << ", 4, 6, 0" << endl;
		Output << "BottomPlate" << ", 4, 6, 0" << endl;
	}
	XYZ Relative;
	Output << "*** Apply the repeat vector constraints" << endl;
	for (i=0; i<(int)Repeats.size(); ++i)
	{
		Relative = Deformation * Repeats[i] - Repeats[i];
		for (j=0; j<3; ++j)
		{
			Output << "Dummy" << i << ", " << j+1 << ", " << j+1 << ", " << Relative[j] << endl;
		}
	}
	Output << "*End Step" << endl;
}
void CSimulationAbaqus::CreateContact(ostream &Output, string Name1, string Name2, string InteractionName)
{
	Output << "*Contact Pair, Interaction=" << InteractionName << endl;
	Output << Name1 << ", " << Name2 << endl;
}
void CSimulationAbaqus::CreateSet(ostream &Output, string Name, const vector<pair<int, int> > &NodePairs)
{
	vector<pair<int, int> >::const_iterator itPair;
	vector<int> GroupA;
	vector<int> GroupB;
	for (itPair = NodePairs.begin(); itPair != NodePairs.end(); ++itPair)
	{
		GroupA.push_back(itPair->first);
		GroupB.push_back(itPair->second);
	}
	CreateSet(Output, NODE_SET, Name + "A", GroupA, true);
	CreateSet(Output, NODE_SET, Name + "B", GroupB, true);
}

void CSimulationAbaqus::CreateSet(ostream &Output, SET_TYPE Type, string Name, vector<int> &Indices, bool bUnSorted)
{
	switch (Type)
	{
	case NODE_SET:
		Output << "*NSet, NSet=" << Name;
		break;
	case ELEMENT_SET:
		Output << "*ElSet, ElSet=" << Name;
		break;
	}
	if (bUnSorted)
	{
		Output << ", Unsorted";
	}

	Output << endl;

	// Increase the indices by 1 because abaqus is 1 based
	vector<int>::iterator itIndex;
	for (itIndex = Indices.begin(); itIndex != Indices.end(); ++itIndex)
	{
		*itIndex += 1;
	}
	WriteValues(Output, Indices, 16);
}

void CSimulationAbaqus::SetYarnSurfaceInteraction(string AbaqusCommands)
{
	m_YarnInteraction = CKeywordInteraction(AbaqusCommands);
}

void CSimulationAbaqus::SetPlateSurfaceInteraction(string AbaqusCommands)
{
	m_PlateInteraction = CKeywordInteraction(AbaqusCommands);
}

void CSimulationAbaqus::SetStaticStepParameters(double dInitTimeInc, double dTimePeriod, double dMinTimeInc, double dMaxTimeInc)
{
	m_StaticStepParameters = stringify(dInitTimeInc) + ", ";
	m_StaticStepParameters += stringify(dTimePeriod) + ", ";
	m_StaticStepParameters += stringify(dMinTimeInc) + ", ";
	m_StaticStepParameters += stringify(dMaxTimeInc);
}


double CSimulationAbaqus::GetSectionArea( vector<int> &Section, CMesh &Mesh )
{
	// area3D_Polygon(): computes the area of a 3D planar polygon
	// From: http://softsurfer.com/Archive/algorithm_0101/algorithm_0101.htm#area3D_Polygon
//    Input:  int n = the number of vertices in the polygon
//            Point* V = an array of n+2 vertices in a plane
//                       with V[n]=V[0] and V[n+1]=V[1]
//            Point N = unit normal vector of the polygon's plane
//    Return: the (float) area of the polygon

	vector<int>::iterator itSection;
	vector<XYZ> Points;
	
	for ( itSection = Section.begin(); itSection != Section.end(); ++itSection )
	{
		Points.push_back( Mesh.GetNode( *itSection ) );
	}

	int iNumNodes = (int)Points.size()-1;
	Points.push_back( Points[1] );   

	double an, ax, ay, az;  // abs value of normal and its coords
	XYZ Normal = CrossProduct( (Points[0] - Points[1]), (Points[2] - Points[1]) );
	an = GetLength( Normal );
	Normal /= an;  // Normalise

    double area = 0;
    int   i, j, k;         // loop indices
    int   coord;           // coord to ignore: 1=x, 2=y, 3=z
    

	
    // select largest abs coordinate to ignore for projection
    ax = fabs(Normal.x); // abs x-coord
    ay = fabs(Normal.y); // abs y-coord
    az = fabs(Normal.z); // abs z-coord

    coord = 3;                     // ignore z-coord
    if (ax > ay) {
        if (ax > az) coord = 1;    // ignore x-coord
    }
    else if (ay > az) coord = 2;   // ignore y-coord

    // compute area of the 2D projection
    for (i=1, j=2, k=0; i<=iNumNodes; i++, j++, k++)
        switch (coord) {
        case 1:
            area += (Points[i].y * (Points[j].z - Points[k].z));
            continue;
        case 2:
            area += (Points[i].x * (Points[j].z - Points[k].z));
            continue;
        case 3:
            area += (Points[i].x * (Points[j].y - Points[k].y));
            continue;
        }

    // scale to get area before projection
    switch (coord) {
    case 1:
		area /= (2*ax);  
        //area *= (an / (2*ax));
        break;
    case 2:
		area /= (2*ay);
        //area *= (an / (2*ay));
        break;
    case 3:
		area /= (2*az);
        //area *= (an / (2*az));
    }
    return fabs(area);
}

double CSimulationAbaqus::GetSectionVolumeFraction( double Area, CTextile &Textile, int Yarn )
{
	double dVolumeFraction;
	CYarn* pYarn = Textile.GetYarn(Yarn);
	if (pYarn->GetFibreDistribution())
	{
		double dFibreArea = pYarn->GetFibreArea(Textile.GetGeometryScale()+"^2");
		if (dFibreArea == 0)
			dFibreArea = Textile.GetFibreArea(Textile.GetGeometryScale()+"^2");
		dVolumeFraction = pYarn->GetFibreDistribution()->GetVolumeFraction(Area, dFibreArea, Yarn);
	}
	else
	{
		dVolumeFraction = -1;
	}
	return dVolumeFraction;
}

void CSimulationAbaqus::CreateMidPointVolumeFractions( vector<SECTION_VF_DATA> &VFData, vector<SECTION_VF_DATA> &VFMidData )
{
	vector<SECTION_VF_DATA>::iterator itData;

	SECTION_VF_DATA PrevData = VFData[0];
	for ( itData = VFData.begin(); itData != VFData.end(); ++itData )
	{
		if ( itData == VFData.begin())
		{
			PrevData = *(itData);
		}
		else
		{
			SECTION_VF_DATA Data;
			Data.iMin = PrevData.iMin; // Set minimum and maximum indices of sections bounding the mid-point
			Data.iMax = itData->iMax;
			Data.dVolumeFraction = (PrevData.dVolumeFraction + itData->dVolumeFraction) / 2.0;
			VFMidData.push_back( Data );
			PrevData = *(itData);
		}
	}
}

void CSimulationAbaqus::GetElementVolumeFractions( vector<POINT_INFO> &ElementsInfo, vector<SECTION_VF_DATA> &MidVFData )
{
	list<int>::const_iterator itIndex;
	vector<SECTION_VF_DATA>::iterator itData;
	int i, ElementIndex = 0;
	for (i = 0; i < CMesh::NUM_ELEMENT_TYPES; ++i)
	{
		int iNumNodes = CMesh::GetNumNodes((CMesh::ELEMENT_TYPE)i);
		if ( iNumNodes != -1 ) // Don't want to get volume fraction of POLYGON sections
		{
			for ( itIndex = m_TextileMesh.GetIndices((CMesh::ELEMENT_TYPE)i).begin(); itIndex != m_TextileMesh.GetIndices((CMesh::ELEMENT_TYPE)i).end(); )
			{
				vector<int> Indices;
				int j;
				for ( j = 0; j < iNumNodes; ++j )
				{
					Indices.push_back( *(itIndex++) );
				}
				// Find which mid-section corresponds to current element
				for ( itData = MidVFData.begin(); itData != MidVFData.end(); ++itData )
				{
					int iNum = 0;
					vector<int>::iterator itIndices;
					for ( itIndices = Indices.begin(); itIndices != Indices.end(); ++itIndices )
					{
						iNum++;
						if ( *itIndices < itData->iMin || *itIndices > itData->iMax )
							break;
					}
					
					if ( iNum == iNumNodes )
					{
						ElementsInfo[ElementIndex++].dVolumeFraction = itData->dVolumeFraction;
						break;
					}
				}
				
			}
		}
	}
}

void CSimulationAbaqus::GetSectionVolumeFractions(CTextile &Textile, vector<SECTION_VF_DATA> &VolFractionData, int iYarn )
{
	list<int> &PolygonIndices = m_YarnMeshes[iYarn].GetIndices( CMesh::POLYGON );
	list<int>::iterator itInt;
	for ( itInt = PolygonIndices.begin(); itInt != PolygonIndices.end(); )
	{
		SECTION_VF_DATA VolFData;
		vector<int> Section;
		Section.clear();
		int StartIndex = *(itInt);
		double Area;
		VolFData.iMin = VolFData.iMax = StartIndex;
		
		// Get the min & max indices of the section and store the indices
		do {
			if ( *(itInt) < VolFData.iMin )
				VolFData.iMin = *(itInt);
			if ( *(itInt) > VolFData.iMax )
				VolFData.iMax = *(itInt);
			Section.push_back( *(itInt++) );
		} while ( *(itInt)!= StartIndex );
		
		if ( *(itInt) < VolFData.iMin )
				VolFData.iMin = *(itInt);
		if ( *(itInt) > VolFData.iMax )
			VolFData.iMax = *(itInt);
		Section.push_back( *(itInt++) );

		Area = GetSectionArea( Section, m_YarnMeshes[iYarn] );
		VolFData.dVolumeFraction = GetSectionVolumeFraction( Area, Textile, iYarn );
		VolFData.iMin = GetGlobalNodeIndex( iYarn, VolFData.iMin );
		VolFData.iMax = GetGlobalNodeIndex( iYarn, VolFData.iMax );
		VolFractionData.push_back( VolFData );
	}
}
