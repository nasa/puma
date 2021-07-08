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
#include "TexGen.h"

extern "C"
{
#include "../Triangle/triangle.h"
#include "../Triangle/triangle_api.h"
}

using namespace TexGen;
using namespace std;

CTetgenMesh::CTetgenMesh(double Seed) : CMeshDomainPlane(Seed)
{
	
}

CTetgenMesh::~CTetgenMesh(void)
{
}

void CTetgenMesh::SaveTetgenMesh( CTextile &Textile, string OutputFilename, string Parameters, bool bPeriodic, int FileType )
{
	tetgenio::facet *f;
	tetgenio::polygon *p;

	pair<XYZ, XYZ> DomainAABB;
	XYZ P;

	if ( !Textile.AddSurfaceToMesh( m_Mesh, m_DomainMeshes, true ) )
	{
		TGERROR("Error creating surface mesh. Cannot generate tetgen mesh");
		return;
	}
	m_Mesh.ConvertQuadstoTriangles(true);

	MeshDomainPlanes( bPeriodic );
	
	m_in.numberoffacets = (int)m_Mesh.GetNumElements() + (int)m_DomainMeshes.size();
	m_in.facetlist = new tetgenio::facet[m_in.numberoffacets];

	// Add facets for yarn elements
	list<int>::const_iterator itIter;
	
	int i = 0;
	for ( int j = 0; j < CMesh::NUM_ELEMENT_TYPES; ++j)
	{
		const list<int> &Indices = m_Mesh.GetIndices((CMesh::ELEMENT_TYPE)j);
		int iNumNodes = CMesh::GetNumNodes((CMesh::ELEMENT_TYPE)j);
		for (itIter = Indices.begin(); itIter != Indices.end(); )
		{
			if ( j == CMesh::QUAD || j == CMesh::TRI )  // For the moment assume that all surface elements are quad or tri
			{
				  f = &m_in.facetlist[i];
				  f->numberofpolygons = 1;
				  f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
				  f->numberofholes = 0;
				  f->holelist = NULL;
				  p = &f->polygonlist[0];
				  p->numberofvertices = iNumNodes;
				  p->vertexlist = new int[p->numberofvertices];
				  for ( int iNode = 0; iNode < iNumNodes; ++iNode )
				  {
					  p->vertexlist[iNode] = *(itIter++) + 1;
				  }
				  ++i;
			}	
			else
			{
				break;
			}
		}
	}

	// Add facets for domain planes
	if ( bPeriodic )
	{
		vector<CMesh>::iterator itTriangulatedMeshes;
		for ( itTriangulatedMeshes = m_TriangulatedMeshes.begin(); itTriangulatedMeshes != m_TriangulatedMeshes.end(); ++itTriangulatedMeshes )
		{
			const list<int> &TriIndices = itTriangulatedMeshes->GetIndices(CMesh::TRI);
			list<int>::const_iterator itTriIndices;

			int iNodeOffset = m_Mesh.GetNumNodes();  // Adding domain plane points to m_Mesh so need to continue from current max index
			int iNumNodes = 3;

			f = &m_in.facetlist[i];
			f->numberofpolygons = (int)TriIndices.size()/iNumNodes;
			f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
			f->numberofholes = 0;
			f->holelist = NULL;

			int iList = 0;
			
			int PolyInd = 0;
			for (itTriIndices = TriIndices.begin(); itTriIndices != TriIndices.end(); )
			{
				p = &f->polygonlist[PolyInd++];
				p->numberofvertices = iNumNodes;
				p->vertexlist = new int[p->numberofvertices];

				for ( int iNode = 0; iNode < p->numberofvertices; ++iNode )
				{
					XYZ Point = itTriangulatedMeshes->GetNode( *(itTriIndices++) );
					int ind = m_Mesh.GetClosestNodeDistance( Point, 0.000001);
					if ( ind == -1 ) // Add node if not in mesh yet
					{
						m_Mesh.AddNode( Point );
						p->vertexlist[iNode] = m_Mesh.GetNumNodes();
					}
					else  // Add existing node index to vertex list
	  					p->vertexlist[iNode] = ind + 1;
				}
			}
			++i;
		}
	}
	else // if not periodic need to add quad for domain edge and point for any polygons intersecting domain plane
	{
		int iFace = 0;
		vector<CMesh>::iterator itDomainMeshes;
		for ( itDomainMeshes = m_DomainMeshes.begin(); itDomainMeshes != m_DomainMeshes.end(); ++itDomainMeshes )
		{
			const list<int> &QuadIndices = itDomainMeshes->GetIndices(CMesh::QUAD);
			const list<int> &PolygonIndices = itDomainMeshes->GetIndices(CMesh::POLYGON);
			list<int>::const_iterator itQuadIndices;
			list<int>::const_iterator itPolygonIndices;


			int iNodeOffset = m_Mesh.GetNumNodes();

			f = &m_in.facetlist[i];
			if ( PolygonIndices.empty() )
				f->numberofpolygons = 1;
			else
				f->numberofpolygons = (int)m_PolygonNumVertices[iFace].size() + 1;  // Number of polygons + the quad
			f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
			f->numberofholes = 0;
			f->holelist = NULL;

			int iList = 0;
			
			int PolyInd = 0;
			// Add quad points
			for (itQuadIndices = QuadIndices.begin(); itQuadIndices != QuadIndices.end(); )
			{
				p = &f->polygonlist[PolyInd++];
				p->numberofvertices = 4;
				p->vertexlist = new int[p->numberofvertices];

				for ( int iNode = 0; iNode < p->numberofvertices; ++iNode )
				{
					XYZ Point = itDomainMeshes->GetNode( *(itQuadIndices++) );
					int ind = m_Mesh.GetClosestNodeDistance( Point, 0.000001);
					if ( ind == -1 ) // Add node if not in mesh yet
					{
						m_Mesh.AddNode( Point );
						p->vertexlist[iNode] = m_Mesh.GetNumNodes();
					}
					else  // Add existing node index to vertex list
	  					p->vertexlist[iNode] = ind + 1;
				}
			}

			// Add polygon points
			if ( !PolygonIndices.empty() )
			{
				vector<int>::iterator itNumVertices = m_PolygonNumVertices[iFace].begin();
				for (itPolygonIndices = PolygonIndices.begin(); itPolygonIndices != PolygonIndices.end(); )
				{
					p = &f->polygonlist[PolyInd++];
					p->numberofvertices = *(itNumVertices++);
					p->vertexlist = new int[p->numberofvertices];
				
					for ( int iNode = 0; iNode < p->numberofvertices; ++iNode )
					{
						XYZ Point = itDomainMeshes->GetNode( *(itPolygonIndices++) );
						int ind = m_Mesh.GetClosestNodeDistance( Point, 0.000001);
						if ( ind == -1 ) // Add node if not in mesh yet
						{
							m_Mesh.AddNode( Point );
							p->vertexlist[iNode] = m_Mesh.GetNumNodes();
						}
						else  // Add existing node index to vertex list
	  						p->vertexlist[iNode] = ind + 1;
					}
				}
				++iFace;
			}
			++i;
			
		}
	}

	// Add the mesh nodes to the tetgen point list
	// All indices start from 1.
	m_in.firstnumber = 1;
	m_in.numberofpoints = m_Mesh.GetNumNodes();

	m_in.pointlist = new REAL[m_in.numberofpoints * 3];
	
	vector<XYZ>::iterator itNode;
	int iNodeInd = 0;
	for ( itNode = m_Mesh.NodesBegin(); itNode != m_Mesh.NodesEnd(); ++itNode )
	{
		m_in.pointlist[iNodeInd++] = (*itNode).x;
		m_in.pointlist[iNodeInd++] = (*itNode).y;
		m_in.pointlist[iNodeInd++] = (*itNode).z;
	}
	
	string strOutput;
	string strInput;
	int size = (int)OutputFilename.length();
	
	char* TetgenOutput = new char[size];
	char* TetgenInput = new char[size+5];
	if (FileType == INP_EXPORT)
		strOutput = RemoveExtension( OutputFilename, ".inp" );
	else
		strOutput = RemoveExtension(OutputFilename, ".vtu");
	strInput = strOutput + "Input";
	strcpy(TetgenOutput, strOutput.c_str());
	strcpy( TetgenInput, strInput.c_str());
	
	m_in.save_nodes(TetgenInput);
	m_in.save_poly(TetgenInput);
	delete [] TetgenInput;

	// Check the input mesh first
	try
	{
		tetrahedralize((char *)"d", &m_in, &m_out);
	}
	catch(...)
	{
		TGERROR("Tetrahedralize failed.  Intersections in PLC");
		return;
	}
	// Then create the mesh
	try
	{
		tetrahedralize((char*)Parameters.c_str(), &m_in, &m_out);
	}
	catch(...)
	{
		TGERROR("Tetrahedralize failed.  No mesh generated");
		TGERROR(Parameters);
		return;
	}
			
	// Output mesh to files 'barout.node', 'barout.ele' and 'barout.face'.
	m_out.save_nodes(TetgenOutput);
	m_out.save_elements(TetgenOutput);
	m_out.save_faces(TetgenOutput);
	delete [] TetgenOutput;

	SaveMesh( Textile );
	if (FileType == INP_EXPORT)
		SaveToAbaqus(OutputFilename, Textile);
	else
		SaveToVTK(OutputFilename);
}

void CTetgenMesh::SaveMesh(CTextile &Textile)
{
	m_OutputMesh.Clear();
	
	// Store output mesh in CMesh
	for (int i = 0; i < m_out.numberofpoints * 3; ) // Three REALs in pointlist for each point
	{
		XYZ Point;
		Point.x = m_out.pointlist[i++];
		Point.y = m_out.pointlist[i++];
		Point.z = m_out.pointlist[i++];
		m_OutputMesh.AddNode(Point);
	}

	CMesh::ELEMENT_TYPE ElementType = m_out.numberofcorners == 4 ? CMesh::TET : CMesh::QUADRATIC_TET;

	for (int i = 0; i < m_out.numberoftetrahedra; i++)
	{
		vector<int> Indices;
		for (int j = 0; j < m_out.numberofcorners; j++)
		{
			Indices.push_back(m_out.tetrahedronlist[i*m_out.numberofcorners + j] - 1);  // Tetgen indices start from 1
		}
		m_OutputMesh.AddElement(ElementType, Indices);
	}

	Textile.GetPointInformation(m_OutputMesh.GetElementCenters(ElementType), m_ElementsInfo);
}

void CTetgenMesh::SaveToAbaqus( string Filename, CTextile &Textile )
{
	m_OutputMesh.SaveToABAQUS( Filename, &m_ElementsInfo, false, false );
	
}

void CTetgenMesh::SaveToVTK(string Filename)
{
	CMeshData<char> YarnIndex("YarnIndex", CMeshDataBase::ELEMENT);
	CMeshData<XYZ> YarnTangent("YarnTangent", CMeshDataBase::ELEMENT);
	CMeshData<XY> Location("Location", CMeshDataBase::ELEMENT);
	CMeshData<double> VolumeFraction("VolumeFraction", CMeshDataBase::ELEMENT);
	CMeshData<double> SurfaceDistance("SurfaceDistance", CMeshDataBase::ELEMENT);
	CMeshData<XYZ> Orientation("Orientation", CMeshDataBase::ELEMENT);

	vector<POINT_INFO>::const_iterator itPointInfo;
	for (itPointInfo = m_ElementsInfo.begin(); itPointInfo != m_ElementsInfo.end(); ++itPointInfo)
	{
		YarnIndex.m_Data.push_back(itPointInfo->iYarnIndex);
		YarnTangent.m_Data.push_back(itPointInfo->YarnTangent);
		Location.m_Data.push_back(itPointInfo->Location);
		VolumeFraction.m_Data.push_back(itPointInfo->dVolumeFraction);
		SurfaceDistance.m_Data.push_back(itPointInfo->dSurfaceDistance);
		Orientation.m_Data.push_back(itPointInfo->Orientation);
	}

	vector<CMeshDataBase*> MeshData;

	MeshData.push_back(&YarnIndex);
	MeshData.push_back(&YarnTangent);
	MeshData.push_back(&Location);
	MeshData.push_back(&VolumeFraction);
	MeshData.push_back(&SurfaceDistance);
	MeshData.push_back(&Orientation);

	m_OutputMesh.SaveToVTK( Filename, &MeshData );
}
