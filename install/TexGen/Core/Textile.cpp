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
#include "Textile.h"
#include "Domain.h"
#include "TexGen.h"

using namespace TexGen;

#define TOL 1e-10

CTextile::CTextile(void)
: m_bNeedsBuilding(true)
{
}

CTextile::CTextile(const vector<CYarn> &Yarns)
: m_Yarns(Yarns.size())
{
	copy(Yarns.begin(), Yarns.end(), m_Yarns.begin());
}

CTextile::CTextile(const CTextile &CopyMe)
: CPropertiesTextile(CopyMe)
{
	*this = CopyMe;
}

CTextile::~CTextile(void)
{
}

CTextile &CTextile::operator=(const CTextile& CopyMe)
{
	m_Yarns = CopyMe.m_Yarns;
	m_bNeedsBuilding = CopyMe.m_bNeedsBuilding;
	m_pDomain = CopyMe.m_pDomain;

	// Yarns need to be reparented
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->SetParent(this);
	}
	return *this;
}

CTextile::CTextile(TiXmlElement &Element)
:CPropertiesTextile(Element),m_bNeedsBuilding(true)
{
	TiXmlElement* pDomain = Element.FirstChildElement("Domain");
	if (pDomain)
	{
		const string* pType = pDomain->Attribute(string("type"));
		if (pType)
		{
			if (*pType == "CDomainPlanes")
				m_pDomain = CDomainPlanes(*pDomain);
		}
	}
	m_bNeedsBuilding = valueify<bool>(Element.Attribute("NeedsBuilding"));
	FOR_EACH_TIXMLELEMENT(pYarn, Element, "Yarn")
	{
		AddYarn(CYarn(*pYarn));
	}
}

void CTextile::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType)
{
	CPropertiesTextile::PopulateTiXmlElement(Element, OutputType);

	Element.SetAttribute("type", GetType());
	
	if (m_pDomain)
	{
		TiXmlElement Domain("Domain");
		m_pDomain->PopulateTiXmlElement(Domain, OutputType);
		Element.InsertEndChild(Domain);
	}

	// Output the yarns if minimal output hasn't been selected or
	// if this instance of the textile is not derived from (otherwise
	// the output file will be useless)
	if (OutputType != OUTPUT_MINIMAL || GetType() == "CTextile")
	{
		Element.SetAttribute("NeedsBuilding", 0);
		BuildTextileIfNeeded();
		int i;
		for (i=0; i<(int)m_Yarns.size(); ++i)
		{
			TiXmlElement Yarn("Yarn");
			Yarn.SetAttribute("index", i);
			m_Yarns[i].PopulateTiXmlElement(Yarn, OutputType);
			Element.InsertEndChild(Yarn);
		}
	}
	else
	{
		Element.SetAttribute("NeedsBuilding", 1);
	}
}

int CTextile::AddYarn(const CYarn &Yarn)
{
	// This call may result in the CYarn copy constructor being invoked
	// This is not effecient but more importantly all the parent pointers
	// are reset to NULL
	m_Yarns.push_back(Yarn);
	// Make sure the parent pointers are reset correctly
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->SetParent(this);
	}
	return m_Yarns.size()-1;
}

int CTextile::AddYarn(const CYarn &Yarn) const
{
	// This call may result in the CYarn copy constructor being invoked
	// This is not effecient but more importantly all the parent pointers
	// are reset to NULL
	m_Yarns.push_back(Yarn);
	// Make sure the parent pointers are reset correctly
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->SetParent(this);
	}
	return m_Yarns.size()-1;
}

bool CTextile::DeleteYarn(int iIndex)
{
	if (iIndex < 0 || iIndex >= (int)m_Yarns.size())
		return false;
	m_Yarns.erase(m_Yarns.begin()+iIndex);
	return true;
}

void CTextile::DeleteYarns()
{
	m_Yarns.clear();
}

void CTextile::AddNodesToMesh(CMesh &Mesh)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->AddNodesToMesh(Mesh);
	}
}

void CTextile::AddPathToMesh(CMesh &Mesh)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->AddPathToMesh(Mesh);
	}
}

void CTextile::AddSurfaceToMesh(CMesh &Mesh, bool bTrimToDomain)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		if (bTrimToDomain && m_pDomain)
			itYarn->AddSurfaceToMesh(Mesh, *m_pDomain);
		else
			itYarn->AddSurfaceToMesh(Mesh);
	}
}

bool CTextile::AddSurfaceToMesh(CMesh &Mesh, vector<CMesh> &DomainMeshes, bool bTrimToDomain)
{
	if (!BuildTextileIfNeeded())
		return false;
	if ( !m_pDomain )
	{
		TGERROR("Textile has no domain assigned");
		return false;
	}
	CMesh DomainMesh = m_pDomain->GetMesh();
	// For the most part domain will be box in which case want the quad elements not the tris
	DomainMesh.ConvertTriToQuad();

	list<int>::const_iterator itIter;
	int iNumNodes;

	// Save each domain face as separate mesh & add to DomainMeshes vector
	for ( int i = 0; i < CMesh::NUM_ELEMENT_TYPES; ++i)
	{
		const list<int> &Indices = DomainMesh.GetIndices((CMesh::ELEMENT_TYPE)i);
		iNumNodes = CMesh::GetNumNodes((CMesh::ELEMENT_TYPE)i);
		for (itIter = Indices.begin(); itIter != Indices.end(); )
		{
			if ( i != CMesh::POLYGON  )
			{
				CMesh FaceMesh;
				vector<int> index;
				for ( int j = 0; j < iNumNodes; ++j )
				{
					FaceMesh.AddNode( DomainMesh.GetNode(*(itIter++)));
					index.push_back(j);
				}
				FaceMesh.AddElement( (CMesh::ELEMENT_TYPE)i, index );
				DomainMeshes.push_back(FaceMesh);
			}		
		}
	}

	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		if (bTrimToDomain && m_pDomain)
		{
			if ( !itYarn->AddSurfaceToMesh(Mesh, *m_pDomain, DomainMeshes ) )
				return false;
		}
		else
			itYarn->AddSurfaceToMesh(Mesh);
	}
	return true;
}

void CTextile::AddVolumeToMesh(CMesh &Mesh, bool bTrimToDomain)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		if (bTrimToDomain && m_pDomain)
			itYarn->AddVolumeToMesh(Mesh, *m_pDomain);
		else
			itYarn->AddVolumeToMesh(Mesh);
	}
}

void CTextile::AddCentrePlaneToMesh(CMesh &Mesh, bool bTrimToDomain)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		if (bTrimToDomain && m_pDomain)
			itYarn->AddCentrePlaneToMesh(Mesh, *m_pDomain);
		else
			itYarn->AddCentrePlaneToMesh(Mesh);
	}
}

void CTextile::Rotate(WXYZ Rotation, XYZ Origin)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->Rotate(Rotation, Origin);
	}
}

void CTextile::Translate(XYZ Vector)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->Translate(Vector);
	}
}

string CTextile::GetName() const
{
	return TEXGEN.GetName(this);
}
/*
bool CTextile::OutputAbaqus(string FileName, double dInitialStrains)
{
	TGLOGINDENT("Outputing volume mesh to abaqus");
	if (!BuildTextileIfNeeded())
		return false;

	CMesh Mesh, UndeformedMesh;
	CYarn UndeformedYarn;
	vector<CYarn>::iterator itYarn;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		itYarn->AddVolumeToMesh(Mesh);
		if (dInitialStrains > 0)
		{
			UndeformedYarn = *itYarn;
			UndeformedYarn.StraightenYarn(dInitialStrains);
			UndeformedYarn.AddVolumeToMesh(UndeformedMesh);
		}
	}

	ofstream Output(FileName.c_str());

	if (!Output)
	{
		TGERROR("Unable to output mesh to abaqus file format, could not open file: " + FileName);
		return false;
	}

	Output << "*HEADING" << endl;
	Output << "TexGen generated file" << endl;

	Output << "*NODE" << endl;
	Mesh.OutputNodes(Output, 1);

	int iStartIndex = 1;
	Output << "*ELEMENT,TYPE=C3D6" << endl;
	iStartIndex = Mesh.OutputElements(Output, CMesh::WEDGE, iStartIndex, 1);

	Output << "*ELEMENT,TYPE=C3D8" << endl;
	iStartIndex = Mesh.OutputElements(Output, CMesh::HEX, iStartIndex, 1);

	if (dInitialStrains > 0)
	{
		Output << "*ORIGINALPOSITIONS" << endl;
		UndeformedMesh.OutputNodes(Output, 1);
	}

	return true;
}
*/
void CTextile::GetPointInformation(const vector<XYZ> &Points, vector<POINT_INFO> &PointsInfo, double dTolerance)
{
	//TGLOGINDENT("Getting information for " << (int)Points.size() << " points");
	if (Points.empty())
		return;
	if (!BuildTextileIfNeeded())
		return;

	PointsInfo.clear();
	PointsInfo.resize(Points.size());
	vector<XYZ>::const_iterator itPoint;
	std::vector<XY>::iterator itLoc; 	
	vector<POINT_INFO>::iterator itInfo;
	POINT_INFO Info;

	XYZ Min,Max;
	for (itPoint = Points.begin(); itPoint != Points.end(); ++itPoint)
	{
		if (itPoint == Points.begin())
		{
			Min = Max = *itPoint;
		}
		else
		{
			Min = ::Min(Min, *itPoint);
			Max = ::Max(Max, *itPoint);
		}
	}
	XYZ Tangent;
	XY Loc;
	CDomainPlanes DomainPlanes(Min, Max);
	vector<CYarn>::iterator itYarn;
	int iIndex;
	for (itYarn = m_Yarns.begin(), iIndex=0; itYarn != m_Yarns.end(); ++itYarn, ++iIndex)
	{
		vector<XYZ> Translations = DomainPlanes.GetTranslations(*itYarn);
		for (itPoint = Points.begin(), itInfo = PointsInfo.begin(); itPoint != Points.end(); ++itPoint, ++itInfo)
		{
			if (itYarn->PointInsideYarn(*itPoint, Translations, &Info.YarnTangent, 
				&Info.Location, &Info.dVolumeFraction, &Info.dSurfaceDistance, dTolerance, &Info.Orientation, &Info.Up))
			{
				// If the point is inside several yarns, either because the yarns overlap or because
				// the tolerance is set too high, the point is assigned the yarn which it lies deepest
				// within. I.e. the one with the lowest surface distance (note that negative surface
				// distance represents a point lying below the yarn surface).
				if (itInfo->iYarnIndex == -1 || Info.dSurfaceDistance < itInfo->dSurfaceDistance)
				{
					*itInfo = Info;
					itInfo->iYarnIndex = iIndex;
				}
			}
		}
	}
}

void CTextile::GetPointInformation(const vector<XYZ> &Points, vector<POINT_INFO> &PointsInfo, int iYarn, double dTolerance, bool bSurface)
{
	//TGLOGINDENT("Getting information for " << (int)Points.size() << " points");
	if (Points.empty())
		return;
	if (!BuildTextileIfNeeded())
		return;

	PointsInfo.clear();
	PointsInfo.resize(Points.size());
	vector<XYZ>::const_iterator itPoint;
	std::vector<XY>::iterator itLoc; 	
	vector<POINT_INFO>::iterator itInfo;
	POINT_INFO Info;

	XYZ Min,Max;
	for (itPoint = Points.begin(); itPoint != Points.end(); ++itPoint)
	{
		if (itPoint == Points.begin())
		{
			Min = Max = *itPoint;
		}
		else
		{
			Min = ::Min(Min, *itPoint);
			Max = ::Max(Max, *itPoint);
		}
	}
	XYZ Tangent;
	XY Loc;
	CDomainPlanes DomainPlanes(Min, Max);
	vector<CYarn>::iterator itYarn;
	
	vector<XYZ> Translations = DomainPlanes.GetTranslations(m_Yarns[iYarn]);
	for (itPoint = Points.begin(), itInfo = PointsInfo.begin(); itPoint != Points.end(); ++itPoint, ++itInfo)
	{
		if (m_Yarns[iYarn].PointInsideYarn(*itPoint, Translations, &Info.YarnTangent, 
			&Info.Location, &Info.dVolumeFraction, &Info.dSurfaceDistance, dTolerance, &Info.Orientation, &Info.Up, bSurface))
		{
			// If the point is inside several yarns, either because the yarns overlap or because
			// the tolerance is set too high, the point is assigned the yarn which it lies deepest
			// within. I.e. the one with the lowest surface distance (note that negative surface
			// distance represents a point lying below the yarn surface).
			if (itInfo->iYarnIndex == -1 )//|| Info.dSurfaceDistance < itInfo->dSurfaceDistance)
			{
				*itInfo = Info;
				itInfo->iYarnIndex = iYarn;
			}
		}
		else
		{
		}
	}
	
}

void CTextile::SavePointInformationToVTK(string Filename, const CMesh &Mesh, double dTolerance)
{
	vector<POINT_INFO> PointsInfo;
	GetPointInformation(Mesh.GetNodes(), PointsInfo, dTolerance);

	CMeshData<char> YarnIndex("YarnIndex", CMeshDataBase::ELEMENT);
	CMeshData<XYZ> YarnTangent("YarnTangent", CMeshDataBase::ELEMENT);
	CMeshData<XY> Location("Location", CMeshDataBase::ELEMENT);
	CMeshData<double> VolumeFraction("VolumeFraction", CMeshDataBase::ELEMENT);
	CMeshData<double> SurfaceDistance("SurfaceDistance", CMeshDataBase::ELEMENT);
	CMeshData<XYZ> Orientation("Orientation", CMeshDataBase::ELEMENT);

	vector<POINT_INFO>::const_iterator itPointInfo;
	for (itPointInfo = PointsInfo.begin(); itPointInfo != PointsInfo.end(); ++itPointInfo)
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

	Mesh.SaveToVTK(Filename, &MeshData);
}

double CTextile::GetApproximateSize()
{
	if (!BuildTextileIfNeeded())
		return 0;

	XYZ Min, Max, Pos;
	vector<CYarn>::const_iterator itYarn;
	vector<CNode>::const_iterator itNode;
	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		const vector<CNode> &Nodes = itYarn->GetMasterNodes();
		for (itNode = Nodes.begin(); itNode != Nodes.end(); ++itNode)
		{
			Pos = itNode->GetPosition();
			Min = ::Min(Min, Pos);
			Max = ::Max(Max, Pos);
		}
	}
	return GetLength(Min, Max);
}

int CTextile::DetectInterference(vector<float> &DistanceToSurface, vector<int> &YarnIndices, bool bTrimToDomain, CMesh *pInterferingPoints )
{
	if (!BuildTextileIfNeeded())
		return -1;

	if ( bTrimToDomain && !m_pDomain )
	{
		TGERROR( "Cannot trim interference points to domain - no domain specified" );
		return -1;
	}

	if (pInterferingPoints)
		pInterferingPoints->Clear();

	TGLOGINDENT("Detecting interference between yarns in textile \"" << GetName() << "\"");
	int iIntersections = 0;
	vector<CYarn>::iterator itYarn;
	vector<CYarn>::iterator itYarnCompare;
	vector<CSlaveNode>::const_iterator itSlaveNode;
	vector<XYZ>::const_iterator itPoint;
	pair<XYZ, XYZ> AABB1, AABB2;
	// Parameters to send to PointInsideYarn. Only need because default parameters before pDistanceToSurface
	XYZ pTangent;
	XY pLoc;
	double pVolumeFraction, pDistanceToSurface;
	int iIndex;

	for (itYarn = m_Yarns.begin(), iIndex = 0; itYarn != m_Yarns.end(); ++itYarn, ++iIndex)
	{
		for (itYarnCompare = m_Yarns.begin(); itYarnCompare != m_Yarns.end(); ++itYarnCompare)
		{
			if (itYarn == itYarnCompare)
				continue;
			if ( bTrimToDomain )
			{
				CMesh YarnMesh, CompareMesh;
				
				itYarn->AddSurfaceToMesh(YarnMesh, m_pDomain);
				AABB1 = YarnMesh.GetAABB();
				itYarnCompare->AddSurfaceToMesh( CompareMesh, m_pDomain );
				AABB2 = CompareMesh.GetAABB();
			}
			else
			{
				AABB1 = itYarn->GetAABB();
				AABB2 = itYarnCompare->GetAABB();
			}
			if (!BoundingBoxIntersect(AABB1.first, AABB1.second, AABB2.first, AABB2.second, 1e-9))
				continue;

			vector<XYZ> YarnTranslations;
			vector<XYZ> CompareTranslations;
			if( bTrimToDomain )
			{
				YarnTranslations = m_pDomain->GetTranslations(*itYarn);
				CompareTranslations = m_pDomain->GetTranslations(*itYarnCompare);
			}
			else
			{
				XYZ Translation(0,0,0);
				YarnTranslations.push_back(Translation);
				CompareTranslations.push_back(Translation);
			}

			const vector<CSlaveNode> &SlaveNodes = itYarn->GetSlaveNodes(CYarn::SURFACE);
			for (itSlaveNode = SlaveNodes.begin(); itSlaveNode != SlaveNodes.end(); ++itSlaveNode)
			{
				for (itPoint = itSlaveNode->GetSectionPoints().begin(); itPoint != itSlaveNode->GetSectionPoints().end(); ++itPoint)
				{
					vector<XYZ>::const_iterator itXYZ;
					for( itXYZ = YarnTranslations.begin(); itXYZ != YarnTranslations.end(); ++itXYZ )
					{
						if ( !bTrimToDomain || m_pDomain->PointInDomain( *itPoint + *itXYZ ) ) // Don't need to check for intersection if point outside domain
						{
							if (itYarnCompare->PointInsideYarn(*itPoint + *itXYZ, CompareTranslations, &pTangent, &pLoc, &pVolumeFraction, &pDistanceToSurface))
							{
								if (pInterferingPoints)
									pInterferingPoints->AddNode(*itPoint + *itXYZ);
								DistanceToSurface.push_back( (float)pDistanceToSurface );
								YarnIndices.push_back( iIndex );
								
								++iIntersections;
							}
						}
					}
				}
			}
		}
	}
	TGLOG("Found " << iIntersections << " intersections between yarns in textile \"" << GetName() << "\"");
	return iIntersections;
}

vector<CYarn> &CTextile::GetYarns()
{
	BuildTextileIfNeeded();
	return m_Yarns;
}

const vector<CYarn> &CTextile::GetYarns() const
{
	BuildTextileIfNeeded();
	return m_Yarns;
}

CYarn *CTextile::GetYarn(int iIndex)
{
	BuildTextileIfNeeded();
	if (iIndex < 0 || iIndex >= (int)m_Yarns.size())
	{
		TGERROR("Unable to get yarn, invalid index: " << iIndex);
		return NULL;
	}
	return &m_Yarns[iIndex];
}

const CYarn *CTextile::GetYarn(int iIndex) const
{
	BuildTextileIfNeeded();
	if (iIndex < 0 || iIndex >= (int)m_Yarns.size())
	{
		TGERROR("Unable to get yarn, invalid index: " << iIndex);
		return NULL;
	}
	return &m_Yarns[iIndex];
}

int CTextile::GetNumYarns() const
{
	BuildTextileIfNeeded();
	return (int)m_Yarns.size();
}

bool CTextile::BuildTextileIfNeeded() const
{
	if (!m_bNeedsBuilding)
		return true;
	else
	{
		// Even if the build fails, we set this flag to false because if nothing happens
		// which changes the textile between this call and the next call. The next call
		// will also fail, thus there is no point to call it again.
		m_bNeedsBuilding = false;
		return BuildTextile();
	}
}

void CTextile::AssignDomain(const CDomain &Domain)
{
	m_pDomain = Domain;
}

void CTextile::RemoveDomain()
{
	m_pDomain.Clear();
}

double CTextile::GetYarnLength(string Units)
{
	if (!BuildTextileIfNeeded())
		return 0;
	int i;
	double dYarnLength = 0;
	for (i=0; i<(int)m_Yarns.size(); ++i)
	{
		dYarnLength += m_Yarns[i].GetRealYarnLength(Units);
	}
	return dYarnLength;
}

double CTextile::GetYarnLengthPerUnitArea(string Units)
{
	if (!BuildTextileIfNeeded())
		return 0;
	int i;
	double dYarnLength = 0;
	for (i=0; i<(int)m_Yarns.size(); ++i)
	{
		// If one of the yarns doesn't have correct repeat vectors then we
		// cannot calculate an accurate value of length per unit area for the
		// whole textile
		if (m_Yarns[i].GetRawRepeatArea() == 0)
			return 0;
		dYarnLength += m_Yarns[i].GetYarnLengthPerUnitArea(Units);
	}
	return dYarnLength;
}

double CTextile::GetYarnVolume(string Units)
{
	if (!BuildTextileIfNeeded())
		return 0;
	int i;
	double dVolume = 0;
	for (i=0; i<(int)m_Yarns.size(); ++i)
	{
		dVolume += m_Yarns[i].GetRealYarnVolume(Units);
	}
	return dVolume;
}

double CTextile::GetYarnVolumePerUnitArea(string Units)
{
	if (!BuildTextileIfNeeded())
		return 0;
	int i;
	double dVolPerUnitArea = 0;
	for (i=0; i<(int)m_Yarns.size(); ++i)
	{
		// If one of the yarns doesn't have correct repeat vectors then we
		// cannot calculate an accurate value of volume per unit area for the
		// whole textile
		if (m_Yarns[i].GetRawRepeatArea() == 0)
			return 0;
		dVolPerUnitArea += m_Yarns[i].GetYarnVolumePerUnitArea(Units);
	}
	return dVolPerUnitArea;
}

double CTextile::GetFibreVolume(string Units)
{
	if (!BuildTextileIfNeeded())
		return 0;
	int i;
	double dVolume = 0;
	for (i=0; i<(int)m_Yarns.size(); ++i)
	{
		dVolume += m_Yarns[i].GetFibreVolume(Units);
	}
	return dVolume;
}

double CTextile::GetFibreVolumePerUnitArea(string Units)
{
	if (!BuildTextileIfNeeded())
		return 0;
	// Try to calculate the fibre volume per unit area based on areal density
	if (m_ArealDensity.IsSet() && m_FibreDensity.IsSet())
	{
		double dVolPerUnitArea = m_ArealDensity.GetSIValue()/m_FibreDensity.GetSIValue();
		return ConvertUnits(dVolPerUnitArea, "m", Units);
	}
	// If areal density is missing then calculate it based on linear density of the yarns
	else
	{
		int i;
		double dVolPerUnitArea = 0;
		for (i=0; i<(int)m_Yarns.size(); ++i)
		{
			// If one of the yarns doesn't have correct repeat vectors then we
			// cannot calculate an accurate value fibre volume per unit area for the
			// whole textile
			if (m_Yarns[i].GetRawRepeatArea() == 0)
				return 0;
			dVolPerUnitArea += m_Yarns[i].GetFibreVolumePerUnitArea(Units);
		}
		return dVolPerUnitArea;
	}
	return 0;
}

double CTextile::GetFibreYarnVolumeFraction()
{
	if (!BuildTextileIfNeeded())
		return 0;
	double dYarnVolumePerUnitArea = GetYarnVolumePerUnitArea();
	if (dYarnVolumePerUnitArea)
		return GetFibreVolumePerUnitArea()/dYarnVolumePerUnitArea;
	else if (double dYarnVolume = GetYarnVolume())
	{
		return GetFibreVolume()/dYarnVolume;
	}
	return 0;
}

CTextileWeave* CTextile::GetWeave()
{
	return dynamic_cast<CTextileWeave*>(this);
}

CTextileWeave2D* CTextile::GetWeave2D()
{
	return dynamic_cast<CTextileWeave2D*>(this);
}

CTextileLayered* CTextile::GetLayeredTextile()
{
	return dynamic_cast<CTextileLayered*>(this);
}

CTextile3DWeave* CTextile::Get3DWeave()
{
	return dynamic_cast<CTextile3DWeave*>(this);
}

CTextileOrthogonal* CTextile::GetOrthogonalWeave()
{
	return dynamic_cast<CTextileOrthogonal*>(this);
}

CTextileLayerToLayer* CTextile::GetLayerToLayerWeave()
{
	return dynamic_cast<CTextileLayerToLayer*>(this);
}

CTextileAngleInterlock* CTextile::GetAngleInterlockWeave()
{
	return dynamic_cast<CTextileAngleInterlock*>(this);
}

/*int CTextile::AdjustInterference(vector<float> &DistanceToSurface, CMesh *pInterferingPoints)
{
	if (!BuildTextileIfNeeded())
		return -1;

	if (pInterferingPoints)
		pInterferingPoints->Clear();
	TGLOGINDENT("Detecting interference between yarns in textile \"" << GetName() << "\"");
	int iIntersections = 0;
	vector<CYarn>::iterator itYarn;
	vector<CYarn>::iterator itYarnCompare;
	vector<CSlaveNode>::const_iterator itSlaveNode;
	vector<XYZ>::const_iterator itPoint;
	pair<XYZ, XYZ> AABB1, AABB2;
	// Parameters to send to PointInsideYarn. Only need because default parameters before pDistanceToSurface
	XYZ pTangent;
	XY pLoc;
	double pVolumeFraction, pDistanceToSurface;

	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		for (itYarnCompare = m_Yarns.begin(); itYarnCompare != m_Yarns.end(); ++itYarnCompare)
		{
			if (itYarn == itYarnCompare)
				continue;
			AABB1 = itYarn->GetAABB();
			AABB2 = itYarnCompare->GetAABB();
			if (!BoundingBoxIntersect(AABB1.first, AABB1.second, AABB2.first, AABB2.second, 1e-9))
				continue;
/*			if (AABB1.second.x < AABB2.first.x || AABB1.second.y < AABB2.first.y || AABB1.second.z < AABB2.first.z ||
				AABB2.second.x < AABB1.first.x || AABB2.second.y < AABB1.first.y || AABB2.second.z < AABB1.first.z)
				continue;*/
/*			CDomainPlanes DomainPlanes(AABB1.first, AABB1.second);
			vector<XYZ> Translations = DomainPlanes.GetTranslations(*itYarnCompare);
			const vector<CSlaveNode> &SlaveNodes = itYarn->GetSlaveNodes(CYarn::SURFACE);
			for (itSlaveNode = SlaveNodes.begin(); itSlaveNode != SlaveNodes.end(); ++itSlaveNode)
			{
				for (itPoint = itSlaveNode->GetSectionPoints().begin(); itPoint != itSlaveNode->GetSectionPoints().end(); ++itPoint)
				{
					if (itYarnCompare->PointInsideYarn(*itPoint, Translations, &pTangent, &pLoc, &pVolumeFraction, &pDistanceToSurface))
					{
						if (pInterferingPoints)
						{
							pInterferingPoints->AddNode(*itPoint);
							DistanceToSurface.push_back( (float)pDistanceToSurface );
						}
						++iIntersections;
					}
				}
			}
		}
	}
	TGLOG("Found " << iIntersections << " intersections between yarns in textile \"" << GetName() << "\"");
	return iIntersections;
}*/

void CTextile::SetAllYarnsYoungsModulusX( double dValue, string Units)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn )
	{
		itYarn->SetYoungsModulusX( dValue, Units );
	}
}

void CTextile::SetAllYarnsYoungsModulusY( double dValue, string Units)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn )
	{
		itYarn->SetYoungsModulusY( dValue, Units );
	}
}

void CTextile::SetAllYarnsYoungsModulusZ( double dValue, string Units)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn )
	{
		itYarn->SetYoungsModulusZ( dValue, Units );
	}
}

void CTextile::SetAllYarnsShearModulusXY(double dValue, string Units)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn )
	{
		itYarn->SetShearModulusXY( dValue, Units );
	}
}

void CTextile::SetAllYarnsShearModulusXZ(double dValue, string Units)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn )
	{
		itYarn->SetShearModulusXZ( dValue, Units );
	}
}

void CTextile::SetAllYarnsShearModulusYZ(double dValue, string Units)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn )
	{
		itYarn->SetShearModulusYZ( dValue, Units );
	}
}

void CTextile::SetAllYarnsAlphaX( double dValue, string Units)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn )
	{
		itYarn->SetAlphaX( dValue, Units );
	}
}

void CTextile::SetAllYarnsAlphaY( double dValue, string Units)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn )
	{
		itYarn->SetAlphaY( dValue, Units );
	}
}

void CTextile::SetAllYarnsAlphaZ( double dValue, string Units)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn )
	{
		itYarn->SetAlphaZ( dValue, Units );
	}
}

void CTextile::SetAllYarnsPoissonsRatioX(double dValue)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn )
	{
		itYarn->SetPoissonsRatioX( dValue );
	}
}

void CTextile::SetAllYarnsPoissonsRatioY(double dValue)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn )
	{
		itYarn->SetPoissonsRatioY( dValue );
	}
}

void CTextile::SetAllYarnsPoissonsRatioZ(double dValue)
{
	if (!BuildTextileIfNeeded())
		return;
	vector<CYarn>::iterator itYarn;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn )
	{
		itYarn->SetPoissonsRatioZ( dValue );
	}
}

double CTextile::GetQuickDomainVolumeFraction()
{
	if( !m_pDomain )
	{
		TGERROR("Cannot calculate volume fraction. No domain specified");
		return 0.0;
	}

	if (!BuildTextileIfNeeded())
		return 0.0;

	vector<CYarn>::iterator itYarn;
	double FibreVolume = 0.0;
	
	int i = 0;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn, ++i )
	{
		CMesh Mesh;
		double YarnVolume, YarnFibreVf;
		// Calculate volume of yarn within the domain
		itYarn->AddSurfaceToMesh( Mesh, m_pDomain );
		YarnVolume = Mesh.CalculateVolume();
		
		YarnFibreVf = itYarn->GetFibreYarnVolumeFraction(); //Based on whole yarn
		if ( YarnFibreVf < TOL )
		{
			stringstream Message;
			Message << "No fibre data specified for yarn " << i << ". Using Vf = 1.0\n";
			TGERROR( Message.str() );
			YarnFibreVf = 1.0;
		}
		FibreVolume += YarnFibreVf * YarnVolume;
	}

	return FibreVolume/m_pDomain->GetVolume();
}

double CTextile::GetDomainVolumeFraction()
{
	if( !m_pDomain )
	{
		TGERROR("Cannot calculate volume fraction. No domain specified");
		return 0.0;
	}

	if (!BuildTextileIfNeeded())
		return 0.0;

	vector<CYarn>::iterator itYarn;
	double FibreVolume = 0.0;
	int i = 0;

	for ( itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn, ++i )
	{
		CMesh Mesh;
		double YarnFibreVf;
		vector<XYZ> Centres;
		vector<XYZ>::iterator  itCentres;
		XYZ Tangent;
		XY Loc;
		int i1, i2, i3, i4;
		const XYZ *p1, *p2, *p3, *p4;
		bool bDefaultVf = false;

		// Create volume mesh for domain
		itYarn->AddVolumeToMesh( Mesh, *m_pDomain );
		Mesh.ConvertToTetMesh();

		vector<XYZ> Translations = m_pDomain->GetTranslations(*itYarn);
		Centres = Mesh.GetElementCenters();
		list<int> &TetIndices = Mesh.GetIndices(CMesh::TET);
		list<int>::iterator itTetInd;

		YarnFibreVf = itYarn->GetFibreYarnVolumeFraction();
		if ( YarnFibreVf < TOL )
		{
			stringstream Message;
			Message << "No fibre data specified for yarn " << i << ". Using Vf = 1.0\n";
			TGERROR( Message.str() );
			YarnFibreVf = 1.0;
			bDefaultVf = true;
		}

		// Get fibre volume for each tet based on its area and local volume fraction
		for ( itCentres = Centres.begin(), itTetInd = TetIndices.begin(); itCentres != Centres.end(); ++itCentres )
		{
			XYZ Centre = *itCentres;
			if ( !bDefaultVf )
			{
				itYarn->PointInsideYarn( *itCentres, Translations, &Tangent, &Loc, &YarnFibreVf );
			}
			i1 = *(itTetInd++);
			i2 = *(itTetInd++);
			i3 = *(itTetInd++);
			i4 = *(itTetInd++);

			p1 = &Mesh.GetNode(i1);
			p2 = &Mesh.GetNode(i2);
			p3 = &Mesh.GetNode(i3);
			p4 = &Mesh.GetNode(i4);
			double TetVolume = fabs( DotProduct(CrossProduct(((*p1)-(*p2)),((*p1)-(*p3))),((*p1)-(*p4))))/6.0;
			FibreVolume += YarnFibreVf * TetVolume;
		}
	}

	return FibreVolume/m_pDomain->GetVolume();
}

bool CTextile::ConvertToInterpNodes() const
{
	vector<CYarn>::iterator itYarn;

	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		if ( !itYarn->ConvertToInterpNodes() )
			return false;
	}
	return true;
}

bool CTextile::SetResolution(int Resolution)
{
	BuildTextileIfNeeded();
	vector<CYarn>::iterator itYarn;

	for (itYarn = m_Yarns.begin(); itYarn != m_Yarns.end(); ++itYarn)
	{
		if (!itYarn->SetResolution(Resolution))
			return false;
	}
	return true;
}
















