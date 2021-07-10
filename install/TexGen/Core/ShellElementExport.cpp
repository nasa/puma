/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2019 Louise Brown

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
#include "TexGen.h"



using namespace TexGen;
CShellElementExport::CShellElementExport( bool bWholeSurfaces, bool bTrimSurface )
: m_bWholeSurfaces( bWholeSurfaces )
, m_bTrimSurface( bTrimSurface )
{
}

CShellElementExport::~CShellElementExport(void)
{
}

bool CShellElementExport::SaveShellElementToABAQUS(string Filename, CTextile& Textile )
{
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

	
	for (i=0; i<iNumYarns; ++i)  // Create surface mesh for each yarn 
	{
		CYarn* pYarn = Textile.GetYarn(i);
		bool bMesh = false;
		if (m_bTrimSurface)
			bMesh = pYarn->AddSurfaceToMesh( m_YarnMeshes[i], *pDomain );
		else
			bMesh = pYarn->AddSurfaceToMesh( m_YarnMeshes[i]);
					
		if ( !bMesh )
		{
			TGERROR("Unable to create ABAQUS input file: Failed to create surface mesh for yarn " << i );
			return false;
		}
	}

	GetElementInfo( Textile );

	for(i=0; i<iNumYarns; ++i)  // Add meshes into single mesh
	{
		if ( !m_YarnMeshes[i].NodesEmpty() )
		{
			m_YarnMeshes[i].RemoveUnreferencedNodes();
			m_SurfaceMesh.InsertMesh(m_YarnMeshes[i]);
		}
	}
	
	// then assemble

	BuildIndexOffsets();

	m_SurfaceMesh.SaveToABAQUS(Filename, &m_ElementInfo, false, false ); 

	ofstream Output(Filename.c_str(), ofstream::app );
	// Output material properties
	m_Materials = new CTextileMaterials;
	m_Materials->SetupMaterials( Textile );
	m_Materials->OutputMaterials( Output, Textile.GetNumYarns(), false );
	delete( m_Materials );

	const vector<XYZ> &Repeats = Textile.GetYarn(0)->GetRepeats();
	map<string, vector<pair<int, int> > > SurfaceDefinitions;

	CreateSurfaceDefinitions( iNumYarns, Repeats, SurfaceDefinitions);
	TGLOG("Creating surfaces");
	CreateSurfaces(Output, SurfaceDefinitions);

	CreateInteractions( Output );
	CreateContacts(Output, Textile);

	/*if ( m_iBoundaryConditions != NO_BOUNDARY_CONDITIONS )
	{
		m_PeriodicBoundaries->SetDomainSize( Textile.GetDomain()->GetMesh() );
		if (SaveNodeSets() )
		{
			//ofstream Output(Filename.c_str(), ofstream::app );
			Output << "*****************" << endl;
			Output << "*** NODE SETS ***" << endl;
			Output << "*****************" << endl;
			Output << "** AllNodes - Node set containing all elements" << endl;
			Output << "*NSet, NSet=AllNodes, Generate" << endl;
			Output << "1, " << m_VolumeMesh.GetNumNodes() << ", 1" << endl;
			m_PeriodicBoundaries->CreatePeriodicBoundaries( Output, m_VolumeMesh.GetNumNodes() + 1, Textile, m_iBoundaryConditions, false );
		}
		else
			TGERROR("Unable to generate node sets");
	}*/
	return true;
}

void CShellElementExport::CreateSurfaceDefinitions( int iNumYarns, const vector<XYZ> &Repeats, map<string, vector<pair<int, int> > > &SurfaceDefinitions)
{
	for (int i=0; i<iNumYarns; ++i)
	{
		TGLOG("Creating yarn " << i << " surface definitions");
		if ( m_bWholeSurfaces )
		{
			vector<pair<YarnNumber,int> > Elements;
			GetYarnSurface( i, Elements );
			SurfaceDefinitions["YarnSurf" + stringify(i)] = Elements;
		}
		else
		{
			vector<pair<int,int> > UpperFaces, LowerFaces;
			GetYarnSurfaces( i, Repeats, UpperFaces, LowerFaces );
			SurfaceDefinitions["Yarn" + stringify(i) + "Lower"] = LowerFaces;
			SurfaceDefinitions["Yarn" + stringify(i) + "Upper"] = UpperFaces;
		}
	}
}

void CShellElementExport::GetYarnSurfaces(int iYarn, const vector<XYZ> &Repeats, vector<pair<int,int> > &UpperFaces, vector<pair<int,int> > &LowerFaces )
{
	CMesh SurfaceMesh = m_YarnMeshes[iYarn];
	SurfaceMesh.RemoveElementType(CMesh::POLYGON);
	SurfaceMesh.RemoveUnreferencedNodes();

	// Get the yarn surface
	int i, ElementNum;
	list<int>::iterator itIndex;
	
	pair<int,int> Element;
	Element.first = iYarn;

	
	list<int> &Indices = SurfaceMesh.GetIndices(CMesh::QUAD);
	for (itIndex = Indices.begin(), ElementNum = 0; itIndex != Indices.end(); ++ElementNum)
	{
		vector<int> ElemIndices;
	
		for (i=0; i<CMesh::GetNumNodes(CMesh::QUAD); ++i)
		{
			ElemIndices.push_back(*itIndex);
			++itIndex;
		}
		
		XYZ N1 = SurfaceMesh.GetNode(ElemIndices[0]);
		XYZ N2 = SurfaceMesh.GetNode(ElemIndices[1]);
		XYZ N3 = SurfaceMesh.GetNode(ElemIndices[2]);
		XYZ Normal = CrossProduct(N2-N1, N3-N1);

		// Filter out all the elements based on their normal

		Element.second = ElementNum;
		if ( Normal.z > 0 ) // Upper surface
		{
			UpperFaces.push_back(Element);
		}
		else // Lower surface
		{
			LowerFaces.push_back(Element);
		}
		
	}
	
	
}

void CShellElementExport::GetYarnSurface(int iYarn, vector<pair<int,int> > &Elements )
{
	// Get the yarn surface
	int ElementNum;
	
	pair<int,int> Element;
	Element.first = iYarn;

	
	list<int> &Indices = m_YarnMeshes[iYarn].GetIndices(CMesh::QUAD);
	int numElements = m_YarnMeshes[iYarn].GetNumElements(CMesh::QUAD);
	for ( ElementNum = 0; ElementNum < numElements ; ++ElementNum )
	{
		Element.second = ElementNum;
		Elements.push_back(Element);
	}
	
}

void CShellElementExport::CreateSurfaces(ostream &Output, map< string, vector<pair<YarnNumber,int> > > &SurfaceDefinitions)
{
	// Now output the surface definitions
	Output << "***************************" << endl;
	Output << "*** SURFACE DEFINITIONS ***" << endl;
	Output << "***************************" << endl;
	vector<pair<YarnNumber,int> >::iterator itElements;
	
	map<string, vector<pair<YarnNumber,int> > >::iterator itSurfDef;
	
	for (itSurfDef = SurfaceDefinitions.begin(); itSurfDef != SurfaceDefinitions.end(); ++itSurfDef)
	{
		Output << "*Surface, Name=" << itSurfDef->first << ", Type=Element" << endl;
		for (itElements = itSurfDef->second.begin(); itElements != itSurfDef->second.end(); ++itElements)
		{
			// ABAQUS is 1 based, so add 1
			Output << GetGlobalElementIndex(*itElements)+1 << endl;
		}
	}
}


int CShellElementExport::GetGlobalElementIndex(pair<YarnNumber,int> Element)
{
	return m_ElementIndexOffsets[Element.first] + Element.second;  // m_ElementOffsets[Yarn] + ElementIndex;
}

void CShellElementExport::GetElementInfo( CTextile& Textile )
{	
	TGLOG("Getting point information");
	vector<POINT_INFO> YarnElementInfo;
	for ( int i = 0; i < Textile.GetNumYarns(); ++i )
	{
		if (!m_YarnMeshes[i].GetIndices(CMesh::QUAD).empty() )
		{
			YarnElementInfo.clear();
			Textile.GetPointInformation( m_YarnMeshes[i].GetElementCenters( (CMesh::QUAD) ), YarnElementInfo, i, 0.005, true );		
			m_ElementInfo.insert( m_ElementInfo.end(), YarnElementInfo.begin(), YarnElementInfo.end() );
		}
	}
}

void CShellElementExport::BuildIndexOffsets()
{
	// Where yarn meshes are combined to create ABAQUS file
	// need to create offsets for node and element numbering
	int i, iNumYarns = (int)m_YarnMeshes.size();
	
	int iElemIndexOffset = 0;
	m_ElementIndexOffsets.clear();
	
	for (i=0; i<iNumYarns; ++i)
	{
		m_ElementIndexOffsets[i] = iElemIndexOffset;
		iElemIndexOffset += m_YarnMeshes[i].GetNumElements(CMesh::QUAD);
	}
	
	m_NodeIndexOffsets.clear();
	int iNodeIndexOffset = 0;
	for (i=0; i<iNumYarns; ++i)
	{
		m_NodeIndexOffsets[i] = iNodeIndexOffset;
		iNodeIndexOffset += m_YarnMeshes[i].GetNumNodes();
	}
}

void CShellElementExport::CreateContacts(ostream &Output, const CTextile &Textile)
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

}

void CShellElementExport::CreateContacts(ostream &Output, const CTextileWeave &Weave)
{
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

void CShellElementExport::CreateContacts(ostream &Output, const CTextile3DWeave &Weave)
{
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

void CShellElementExport::OutputContacts( ostream &Output, set<pair<int, int> > &Contacts )
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

void CShellElementExport::CreateContact(ostream &Output, string Name1, string Name2, string InteractionName)
{
	Output << "*Contact Pair, Interaction=" << InteractionName << endl;
	Output << Name1 << ", " << Name2 << endl;
}

void CShellElementExport::CreateInteractions( ostream &Output )
{
	Output << "************" << endl;
	Output << "*** INTERACTION PROPERTIES ***" << endl;
	Output << "************" << endl;
	Output << "*Surface Interaction, name=YARN" << endl;
	Output << "*Friction" << endl;
	Output << "0.112," << endl;
	Output << "*Surface Behavior, pressure-overclosure=HARD" << endl;
}