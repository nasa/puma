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
#include "TexGenRenderer.h"
#include "CustomInteractorStyle.h"

//#include "../Core/TexGen.h"
using namespace TexGen;
CTexGenRenderer::CTexGenRenderer(vtkRenderWindowInteractor *pRenderWindowInteractor)
: m_pRenderer(NULL)
, m_pRenderWindow(NULL)
, m_pRenderWindowInteractor(pRenderWindowInteractor)
, m_pOrientationMarkerWidget(NULL)
, m_pCallback(NULL)
, m_bXRay(false)
, m_bTrimToDomain(true)
, m_iBatchCount(0)
{
	m_pRenderer = vtkRenderer::New();

	if (!m_pRenderWindowInteractor)
	{
		m_pRenderWindow = vtkRenderWindow::New();
		m_pRenderWindowInteractor = vtkRenderWindowInteractor::New();

		m_pRenderWindow->AddRenderer(m_pRenderer);
		m_pRenderWindowInteractor->SetRenderWindow(m_pRenderWindow);
	}
	// interactor style
//	vtkInteractorStyleTerrain *pInteractionStyle = vtkInteractorStyleTerrain::New();
	CCustomInteractorStyle *pInteractionStyle = CCustomInteractorStyle::New();
	m_pRenderWindowInteractor->SetInteractorStyle(pInteractionStyle);
	pInteractionStyle->Delete();

	m_pOrientationMarkerWidget = vtkOrientationMarkerWidget::New();
	vtkAxesActor* pAxes = vtkAxesActor::New();
	pAxes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->ShadowOn();
	pAxes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->ShadowOn();
	pAxes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->ShadowOn();

	m_pOrientationMarkerWidget->SetOrientationMarker(pAxes);
	pAxes->Delete();
	m_pOrientationMarkerWidget->SetViewport(0.0, 0.0, 0.15, 0.3);
	m_pOrientationMarkerWidget->SetInteractor(m_pRenderWindowInteractor);
	m_pOrientationMarkerWidget->SetDefaultRenderer(m_pRenderer);

	vtkCamera *camera = m_pRenderer->GetActiveCamera();

	camera->SetFocalPoint(0, 1, -0.5);
	camera->SetPosition(0, 0, 0);
	camera->SetViewUp(0, 0, 1);
}

CTexGenRenderer::~CTexGenRenderer(void)
{
	if (m_pRenderer)
	{
		m_pRenderer->Delete();
		m_pRenderer = NULL;
	}
	if (m_pRenderWindow)		// If render window was created then window interactor was also created, otherwise it was created by an external source and should not be deleted
	{
		m_pRenderWindow->Delete();
		m_pRenderWindow = NULL;
		if (m_pRenderWindowInteractor)
		{
			m_pRenderWindowInteractor->Delete();
			m_pRenderWindowInteractor = NULL;
		}
	}
	if (m_pOrientationMarkerWidget)
	{
		m_pOrientationMarkerWidget->Delete();
	}
}

void CTexGenRenderer::SetBackgroundColor(COLOR Color)
{
	m_pRenderer->SetBackground(Color.Array());
	UpdateAxesColor();
	RefreshView();
}

void CTexGenRenderer::SetParallelProjection(bool bParallelProjection)
{
	if (bParallelProjection)
		m_pRenderer->GetActiveCamera()->ParallelProjectionOn();
	else
		m_pRenderer->GetActiveCamera()->ParallelProjectionOff();
	RefreshView();
}

bool CTexGenRenderer::GetParallelProjection() const
{
	return m_pRenderer->GetActiveCamera()->GetParallelProjection()?true:false;
}

void CTexGenRenderer::SetXRay(bool bXRay)
{
	m_bXRay = bXRay;
}

void CTexGenRenderer::SetTrimToDomain(bool bTrim)
{
	m_bTrimToDomain = bTrim;
}

void CTexGenRenderer::Start(bool bTakeControl, int iWidth, int iHeight, bool bFullScreen)
{
	m_pRenderWindow->SetSize(iWidth, iHeight);
	if (bFullScreen)
		m_pRenderWindow->FullScreenOn();
	m_pRenderWindow->Render();

	if (bTakeControl)
	{
		TGLOG("Starting rendering");
		m_pRenderWindowInteractor->Start();
	}
}

void CTexGenRenderer::TakeScreenShot(string FileName, int iMagnification)
{
	vtkRenderLargeImage* pRenderLarge = vtkRenderLargeImage::New(); 
	pRenderLarge->SetInput(m_pRenderer);
	pRenderLarge->SetMagnification(iMagnification);

	// If magnifying need to turn off axes otherwise they are tiled across image
	int iAxes = m_pOrientationMarkerWidget->GetEnabled();
	if ( iMagnification > 1 && iAxes )
		m_pOrientationMarkerWidget->SetEnabled(0);

/*	vtkImageCast* pCast = vtkImageCast::New();
	pCast->SetInputConnection(pRenderLarge->GetOutputPort());
	pCast->SetOutputScalarType(pRenderLarge->GetOutput()->GetScalarType());
	pCast->ClampOverflowOn();

	vtkImageResample* pResample = vtkImageResample::New();
	pResample->SetInputConnection(pCast->GetOutputPort());
	pResample->SetDimensionality(2);
	pResample->SetAxisMagnificationFactor(0, 2);
    pResample->SetAxisMagnificationFactor(1, 2);*/

	// We write out the image which causes the rendering to occur. If you
	// watch your screen you might see the pieces being rendered right
	// after one another.
	vtkPNGWriter* pPNGWriter = vtkPNGWriter::New();
	pPNGWriter->SetInputConnection(pRenderLarge->GetOutputPort());
	pPNGWriter->SetFileName(FileName.c_str());
	pPNGWriter->Write();

	// Restore axes
	m_pOrientationMarkerWidget->SetEnabled(iAxes);

	pRenderLarge->Delete();
//	pCast->Delete();
//	pResample->Delete();
	pPNGWriter->Delete();
}

void CTexGenRenderer::ClearScene()
{
/*	vtkActorCollection *Actors = m_pRenderer->GetActors();
	vtkActor *Actor;
	for (Actors->InitTraversal(); (Actor = Actors->GetNextItem())!=NULL; )
	{
		m_pRenderer->RemoveActor(Actor);
	}*/
//	multimap<PROP_TYPE, vtkProp*>::iterator itProp;
	while (!m_Props.empty())
		RemoveProp(m_Props.begin()->second);
/*	for (itProp = m_Props.begin(); itProp != m_Props.end(); ++itProp)
	{
		RemoveProp(itProp->second);
//		m_pRenderer->RemoveViewProp(itProp->second);
	}*/
//	m_Props.clear();
	RefreshView();
}

void CTexGenRenderer::RemoveProps(PROP_TYPE Type)
{
	multimap<PROP_TYPE, vtkProp*>::iterator itProp;
	while ((itProp = m_Props.find(Type)) != m_Props.end())
		RemoveProp(itProp->second);
/*	for (itProp = m_Props.lower_bound(Type); itProp != m_Props.upper_bound(Type); ++itProp)
	{
		RemoveProp(itProp->second);
//		m_pRenderer->RemoveViewProp(itProp->second);
	}*/
//	m_Props.erase(m_Props.lower_bound(Type), m_Props.upper_bound(Type));
	RefreshView();
}

void CTexGenRenderer::SetPropsColor(PROP_TYPE Type, COLOR Color)
{
	multimap<PROP_TYPE, vtkProp*>::iterator itProp;
	for (itProp = m_Props.lower_bound(Type); itProp != m_Props.upper_bound(Type); ++itProp)
	{
		vtkActor* pActor = dynamic_cast<vtkActor*>(itProp->second);
		if (pActor)
		{
			ApplyColor(pActor, Color);
		}
	}
	RefreshView();
}

int CTexGenRenderer::GetNumProps(PROP_TYPE Type) const
{
	// Simulate the fact that the axes are a prop inside the scene
	if (Type == PROP_AXES)
		return m_pOrientationMarkerWidget->GetEnabled();
	return m_Props.count(Type);
}

vector<vtkProp*> CTexGenRenderer::GetProps(set<PROP_TYPE> Types)
{
	vector<vtkProp*> Props;
	multimap<PROP_TYPE, vtkProp*>::iterator itProp;
	for (itProp = m_Props.begin(); itProp != m_Props.end(); ++itProp)
	{
		if (Types.count(itProp->first))
		{
			Props.push_back(itProp->second);
		}
	}
	return Props;
}

vector<vtkProp*> CTexGenRenderer::GetProps(PROP_TYPE Type)
{
	vector<vtkProp*> Props;
	pair<multimap<PROP_TYPE, vtkProp*>::iterator, multimap<PROP_TYPE, vtkProp*>::iterator> Range = m_Props.equal_range(Type);
	multimap<PROP_TYPE, vtkProp*>::iterator itProp;
	for (itProp = Range.first; itProp != Range.second; ++itProp)
	{
		Props.push_back(itProp->second);
	}
	return Props;
}

const PROP_YARN_INFO *CTexGenRenderer::GetYarnPropInfo(vtkProp* pProp) const
{
	map<vtkProp*, PROP_YARN_INFO>::const_iterator itProp = m_YarnProps.find(pProp);
	if (itProp != m_YarnProps.end())
	{
		return &itProp->second;
	}
	return NULL;
}

const PROP_NODE_INFO *CTexGenRenderer::GetNodePropInfo(vtkProp* pProp) const
{
	map<vtkProp*, PROP_NODE_INFO>::const_iterator itProp = m_NodeProps.find(pProp);
	if (itProp != m_NodeProps.end())
	{
		return &itProp->second;
	}
	return NULL;
}

const PROP_IMAGE_INFO *CTexGenRenderer::GetImagePropInfo(vtkProp* pProp) const
{
	map<vtkProp*, PROP_IMAGE_INFO>::const_iterator itProp = m_ImageProps.find(pProp);
	if (itProp != m_ImageProps.end())
	{
		return &itProp->second;
	}
	return NULL;
}

vector<vtkProp*> CTexGenRenderer::GetProps(PROP_INFO* pInfo, bool bIgnoreTextileName)
{
	vector<vtkProp*> Props;
	PROP_YARN_INFO* pYarnProp = dynamic_cast<PROP_YARN_INFO*>(pInfo);
	PROP_NODE_INFO* pNodeProp = dynamic_cast<PROP_NODE_INFO*>(pInfo);
	PROP_IMAGE_INFO* pImageProp = dynamic_cast<PROP_IMAGE_INFO*>(pInfo);
	if (pYarnProp)
	{
		map<vtkProp*, PROP_YARN_INFO>::const_iterator itProp;
		for (itProp = m_YarnProps.begin(); itProp != m_YarnProps.end(); ++itProp)
		{
			if (pYarnProp->iYarn == itProp->second.iYarn &&
				(bIgnoreTextileName || pYarnProp->TextileName == itProp->second.TextileName))
				Props.push_back(itProp->first);
		}
	}
	if (pNodeProp)
	{
		map<vtkProp*, PROP_NODE_INFO>::const_iterator itProp;
		for (itProp = m_NodeProps.begin(); itProp != m_NodeProps.end(); ++itProp)
		{
			if (pNodeProp->iNode == itProp->second.iNode &&
				pNodeProp->iYarn == itProp->second.iYarn &&
				(bIgnoreTextileName || pNodeProp->TextileName == itProp->second.TextileName))
				Props.push_back(itProp->first);
		}
	}
	if (pImageProp)
	{
		map<vtkProp*, PROP_IMAGE_INFO>::const_iterator itProp;
		for (itProp = m_ImageProps.begin(); itProp != m_ImageProps.end(); ++itProp)
		{
			if (pImageProp->iImageID == itProp->second.iImageID)
				Props.push_back(itProp->first);
		}
	}
	return Props;
}

vector<vtkProp*> CTexGenRenderer::GetNodeProps(PROP_YARN_INFO Info, bool bIgnoreTextileName)
{
	vector<vtkProp*> Props;
	map<vtkProp*, PROP_NODE_INFO>::const_iterator itProp;
	for (itProp = m_NodeProps.begin(); itProp != m_NodeProps.end(); ++itProp)
	{
		if (Info.iYarn == itProp->second.iYarn &&
			Info.TextileName == itProp->second.TextileName)
			Props.push_back(itProp->first);
	}
	return Props;
}

void CTexGenRenderer::RefreshTextile(string TextileName)
{
	BeginBatch();
	bool bSurface = (GetNumProps(PROP_SURFACE)>0);
	bool bPath = (GetNumProps(PROP_PATH)>0);
	bool bNodes = (GetNumProps(PROP_NODE)>0);
	bool bVolume = (GetNumProps(PROP_VOLUME)>0);
	set<PROP_TYPE> Types;
	Types.insert(PROP_NODE);
	Types.insert(PROP_PATH);
	Types.insert(PROP_SURFACE);
	Types.insert(PROP_VOLUME);
	vector<vtkProp*> Props = GetProps(Types);
	vector<vtkProp*>::iterator itProp;
	for (itProp = Props.begin(); itProp != Props.end(); ++itProp)
	{
		RemoveProp(*itProp);
	}
	if (bSurface)
		RenderTextile(TextileName, 1);
	if (bPath)
		RenderPaths(TextileName);
	if (bNodes)
		RenderNodes(TextileName);
	if (bVolume)
		RenderMesh(TextileName);
	EndBatch();
}

void CTexGenRenderer::RefreshYarn(PROP_YARN_INFO YarnInfo)
{
	BeginBatch();
	bool bSurface = (GetNumProps(PROP_SURFACE)>0);
	bool bPath = (GetNumProps(PROP_PATH)>0);
	bool bNodes = (GetNumProps(PROP_NODE)>0);
	bool bVolume = (GetNumProps(PROP_VOLUME)>0);
	vector<vtkProp*> YarnProps = GetProps(&YarnInfo);
	vector<vtkProp*> NodeProps = GetNodeProps(YarnInfo);
	vector<vtkProp*> Props;
	Props.insert(Props.end(), YarnProps.begin(), YarnProps.end());
	Props.insert(Props.end(), NodeProps.begin(), NodeProps.end());
	vector<vtkProp*>::iterator itProp;
	for (itProp = Props.begin(); itProp != Props.end(); ++itProp)
	{
		RemoveProp(*itProp);
	}
	CTextile* pTextile = TEXGEN.GetTextile(YarnInfo.TextileName);
	if (pTextile)
	{
		const CDomain* pDomain = NULL;
		if (m_bTrimToDomain)
			pDomain = pTextile->GetDomain();
		CYarn &Yarn = pTextile->GetYarns()[YarnInfo.iYarn];
		if (bSurface)
		{
			vtkProp* pYarnProp = RenderYarn(Yarn, pDomain, GetIndexedColor(YarnInfo.iYarn), 1);
			if (pYarnProp)
			{
				m_YarnProps[pYarnProp] = YarnInfo;
			}
		}
		if (bPath)
		{
			RenderPath(*pTextile, YarnInfo.iYarn);
		}
		if (bNodes)
		{
			RenderNodes(*pTextile, YarnInfo.iYarn);
		}
		if( bVolume )
		{
			vtkProp* pYarnProp = RenderYarnMesh(Yarn, pDomain, GetIndexedColor(YarnInfo.iYarn));
			if (pYarnProp)
			{
				m_YarnProps[pYarnProp] = YarnInfo;
			}
		}
	}
	EndBatch();
}

void CTexGenRenderer::AddProp(PROP_TYPE Type, vtkProp *pProp)
{
//	if (bRefreshView)
//	{
		int iNumPropsBefore = m_Props.size();
		double Bounds[6];
		m_pRenderer->ComputeVisiblePropBounds(Bounds);
		double dDiagonalBefore = sqrt((Bounds[1]-Bounds[0])*(Bounds[1]-Bounds[0]) +
								(Bounds[3]-Bounds[2])*(Bounds[3]-Bounds[2]) +
								(Bounds[5]-Bounds[4])*(Bounds[5]-Bounds[4]));
		m_pRenderer->AddViewProp(pProp);
		m_Props.insert(make_pair(Type, pProp));
		pProp->Delete();
		m_pRenderer->ComputeVisiblePropBounds(Bounds);
		double dDiagonalAfter = sqrt((Bounds[1]-Bounds[0])*(Bounds[1]-Bounds[0]) +
								(Bounds[3]-Bounds[2])*(Bounds[3]-Bounds[2]) +
								(Bounds[5]-Bounds[4])*(Bounds[5]-Bounds[4]));
		if (abs(dDiagonalBefore-dDiagonalAfter) > 0.1*dDiagonalBefore || iNumPropsBefore==0)
			ResetCamera();
		else
			RefreshView();
/*	}
	else
	{
		m_pRenderer->AddViewProp(pProp);
		m_Props.insert(make_pair(Type, pProp));
		pProp->Delete();
	}*/
}

void CTexGenRenderer::RemoveProp(vtkProp *pProp)
{
	m_pRenderer->RemoveViewProp(pProp);
	m_YarnProps.erase(pProp);
	m_NodeProps.erase(pProp);
	m_ImageProps.erase(pProp);
	multimap<PROP_TYPE, vtkProp*>::iterator itProp;
	for (itProp = m_Props.begin(); itProp != m_Props.end(); )
	{
		if (itProp->second == pProp)
			m_Props.erase(itProp++);
		else
			++itProp;
	}
}

vtkActor *CTexGenRenderer::ConvertToActor(vtkPolyData *pPolyData)
{
	vtkActor *pActor = vtkActor::New();
	vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
	pPolyDataMapper->SetInputData(pPolyData);
	pActor->SetMapper(pPolyDataMapper);

	pPolyDataMapper->Delete();
	pPolyData->Delete();

	return pActor;
}

vtkActor *CTexGenRenderer::ConvertToActor(vtkAlgorithm *pAlgorithm)
{
	vtkActor *pActor = vtkActor::New();
	vtkPolyDataMapper *pPolyDataMapper = vtkPolyDataMapper::New();
	pPolyDataMapper->SetInputConnection(pAlgorithm->GetOutputPort());
	pActor->SetMapper(pPolyDataMapper);

	pPolyDataMapper->Delete();
	pAlgorithm->Delete();

	return pActor;
}

void CTexGenRenderer::RenderTextile(string TextileName, double dOpacity)
{
	if (TextileName == "")
	{
		map<string, CTextile*>::const_iterator itTextile;
		for (itTextile = CTexGen::GetInstance().GetTextiles().begin(); itTextile != CTexGen::GetInstance().GetTextiles().end(); ++itTextile)
		{
			assert(itTextile->first != "");
			RenderTextile(itTextile->first, dOpacity);
		}
		return;
	}
	TGLOGINDENT("Adding textile \"" << TextileName << "\" to renderer");
	CTextile *pTextile = CTexGen::GetInstance().GetTextile(TextileName);
	if (!pTextile)
		return;
	RenderTextile(*pTextile, dOpacity);
}

void CTexGenRenderer::RenderTextile(CTextile &Textile, double dOpacity)
{
	const CDomain *pDomain = NULL;
	if (m_bTrimToDomain)
		pDomain = Textile.GetDomain();

	vector<CYarn>::iterator itYarn;

	int i;
	vtkProp* pProp;
	BeginBatch();
	for (itYarn = Textile.GetYarns().begin(), i=0; itYarn != Textile.GetYarns().end(); ++itYarn, ++i)
	{
		pProp = RenderYarn(*itYarn, pDomain, GetIndexedColor(i), dOpacity);
		if (pProp)
		{
			PROP_YARN_INFO Info;
			Info.iYarn = i;
			Info.TextileName = Textile.GetName();
			m_YarnProps[pProp] = Info;
		}
	}
	EndBatch();
}

vtkProp* CTexGenRenderer::RenderYarn(CYarn &Yarn, const CDomain *pDomain, COLOR Color, double dOpacity)
{
	CMesh Mesh;

	TGLOGINDENT("Adding yarn surface to renderer");
	if (pDomain)
		Yarn.AddSurfaceToMesh(Mesh, *pDomain);
	else
		Yarn.AddSurfaceToMesh(Mesh);

	if (Mesh.NodesEmpty())
		return NULL;

	vtkActor *pActor;
	vtkAlgorithm *pAlgorithm = CalculateNormals(GetPolyData(Mesh));

	pActor = ConvertToActor(pAlgorithm);

	pActor->GetProperty()->BackfaceCullingOn();
	if (m_bXRay)
		pActor->GetProperty()->SetRepresentationToWireframe();
//		pActor->GetProperty()->SetOpacity(0.75);
//	else
		pActor->GetProperty()->SetOpacity(dOpacity);
	ApplyColor(pActor, Color);


	AddProp(PROP_SURFACE, pActor);

	return pActor;
}

void CTexGenRenderer::RenderDomain(string TextileName, COLOR Color, double dOpacity)
{
	if (TextileName == "")
	{
		map<string, CTextile*>::const_iterator itTextile;
		for (itTextile = CTexGen::GetInstance().GetTextiles().begin(); itTextile != CTexGen::GetInstance().GetTextiles().end(); ++itTextile)
		{
			assert(itTextile->first != "");
			RenderDomain(itTextile->first, Color, dOpacity);
		}
		return;
	}
	TGLOGINDENT("Adding domain of \"" << TextileName << "\" to renderer");
	CTextile *pTextile = CTexGen::GetInstance().GetTextile(TextileName);
	if (!pTextile)
		return;

	const CDomain *pDomain = pTextile->GetDomain();
	if (!pDomain)
		return;

	RenderDomain(*pDomain, Color, dOpacity);
}

void CTexGenRenderer::RenderMesh(string TextileName)
{
	if (TextileName == "")
	{
		map<string, CTextile*>::const_iterator itTextile;
		for (itTextile = CTexGen::GetInstance().GetTextiles().begin(); itTextile != CTexGen::GetInstance().GetTextiles().end(); ++itTextile)
		{
			assert(itTextile->first != "");
			RenderMesh(itTextile->first);
		}
		return;
	}
	TGLOGINDENT("Adding textile mesh\"" << TextileName << "\" to renderer");
	CTextile *pTextile = CTexGen::GetInstance().GetTextile(TextileName);
	if (!pTextile)
		return;
	RenderMesh(*pTextile);
}

void CTexGenRenderer::RenderMesh(CTextile &Textile)
{
	const CDomain *pDomain = NULL;
	if (m_bTrimToDomain)
		pDomain = Textile.GetDomain();

	vector<CYarn>::iterator itYarn;

	int i;
	vtkProp* pProp;
	BeginBatch();
	for (itYarn = Textile.GetYarns().begin(), i=0; itYarn != Textile.GetYarns().end(); ++itYarn, ++i)
	{
		pProp = RenderYarnMesh(*itYarn, pDomain, GetIndexedColor(i));
		if (pProp)
		{
			PROP_YARN_INFO Info;
			Info.iYarn = i;
			Info.TextileName = Textile.GetName();
			m_YarnProps[pProp] = Info;
		}
	}
	EndBatch();
}

vtkProp* CTexGenRenderer::RenderYarnMesh(CYarn &Yarn, const CDomain *pDomain, COLOR Color)
{
	CMesh Mesh;

	TGLOGINDENT("Adding yarn volume mesh to renderer");
	if (pDomain)
		Yarn.AddVolumeToMesh(Mesh, *pDomain);
	else
		Yarn.AddVolumeToMesh(Mesh);

	if (Mesh.NodesEmpty())
		return NULL;

	vtkActor *pActor;
	vtkAlgorithm *pAlgorithm = CalculateNormals(GetPolyData(Mesh));

	pActor = ConvertToActor(pAlgorithm);	
	pActor->GetProperty()->SetRepresentationToWireframe();
	ApplyColor(pActor, Color);


	AddProp(PROP_VOLUME, pActor);

	return pActor;
}

void CTexGenRenderer::RenderDomain(const CDomain &Domain, COLOR Color, double dOpacity)
{
	TGLOGINDENT("Adding domain surface to renderer");

	CDomain* pCopy = Domain.Copy();

	// Get rid of flicker between the ends of the yarns and the domain by growing the domain a little
	pair<XYZ, XYZ> Limits = Domain.GetMesh().GetAABB();
	double extend = GetLength(Limits.second - Limits.first)*1e-3;
	pCopy->Grow(extend);
	CMesh Mesh = pCopy->GetMesh();

	delete pCopy;

	if (Mesh.NodesEmpty())
		return;

	vtkActor *pActor;
	vtkAlgorithm *pAlgorithm = CalculateNormals(GetPolyData(Mesh));

	pActor = ConvertToActor(pAlgorithm);

	pActor->GetProperty()->SetColor(Color.Array());
	pActor->GetProperty()->BackfaceCullingOn();
	pActor->GetProperty()->SetOpacity(dOpacity);

	AddProp(PROP_DOMAIN, pActor);
}

void CTexGenRenderer::RenderDomainAxes(string TextileName)
{
	if (TextileName == "")
	{
		map<string, CTextile*>::const_iterator itTextile;
		for (itTextile = CTexGen::GetInstance().GetTextiles().begin(); itTextile != CTexGen::GetInstance().GetTextiles().end(); ++itTextile)
		{
			assert(itTextile->first != "");
			RenderDomainAxes(itTextile->first);
		}
		return;
	}
	TGLOGINDENT("Adding domain of \"" << TextileName << "\" to renderer");
	CTextile *pTextile = CTexGen::GetInstance().GetTextile(TextileName);
	if (!pTextile)
		return;

	const CDomain *pDomain = pTextile->GetDomain();
	if (!pDomain)
		return;

	RenderDomainAxes(*pDomain);
}

void CTexGenRenderer::RenderDomainAxes(const CDomain &Domain)
{
	pair<XYZ, XYZ> Limits = Domain.GetMesh().GetAABB();

	COLOR AnnotationColor = GetBestAnnotationColor();

	// Create a text property for both cube axes
	vtkTextProperty *tprop = vtkTextProperty::New();
	tprop->SetColor(AnnotationColor.Array());
	tprop->ShadowOn();

	// Create a vtkCubeAxesActor2D.  Use the outer edges of the bounding box to
	// draw the axes.  Add the actor to the renderer.
	vtkCubeAxesActor2D* axes = vtkCubeAxesActor2D::New();
	axes->SetBounds(Limits.first.x, Limits.second.x, Limits.first.y, Limits.second.y, Limits.first.z, Limits.second.z);
	axes->SetCamera(m_pRenderer->GetActiveCamera());
	axes->SetLabelFormat("%6.4g");
	axes->SetFlyModeToOuterEdges();
	axes->SetFontFactor(1.5);
	axes->SetAxisTitleTextProperty(tprop);
	axes->SetAxisLabelTextProperty(tprop);
	axes->SetNumberOfLabels(2);
	axes->GetProperty()->SetColor(AnnotationColor.Array());

	tprop->Delete();

	AddProp(PROP_DOMAINAXES, axes);
}

COLOR CTexGenRenderer::GetBestAnnotationColor()
{
	double BackGroundColor[3];
	m_pRenderer->GetBackground(BackGroundColor);
	double dIntensity = (BackGroundColor[0]+BackGroundColor[1]+BackGroundColor[2])/3.0;
	if (dIntensity > 0.5)
		return COLOR(0, 0, 0);
	else
		return COLOR(1, 1, 1);
}

void CTexGenRenderer::UpdateAxesColor()
{
	COLOR AnnotationColor = GetBestAnnotationColor();
	typedef multimap<PROP_TYPE, vtkProp*>::iterator PropMapIter;
	PropMapIter itProp;
	pair<PropMapIter, PropMapIter> Range = m_Props.equal_range(PROP_DOMAINAXES);
	for (itProp = Range.first; itProp != Range.second; ++itProp)
	{
		vtkCubeAxesActor2D* pAxes = dynamic_cast<vtkCubeAxesActor2D*>(itProp->second);
		if (pAxes)
		{
			pAxes->GetProperty()->SetColor(AnnotationColor.Array());
			pAxes->GetAxisTitleTextProperty()->SetColor(AnnotationColor.Array());
			pAxes->GetAxisLabelTextProperty()->SetColor(AnnotationColor.Array());
		}
	}

	vtkAxesActor* pAxes = dynamic_cast<vtkAxesActor*>(m_pOrientationMarkerWidget->GetOrientationMarker());
	if (pAxes)
	{
		pAxes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(AnnotationColor.Array());
		pAxes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(AnnotationColor.Array());
		pAxes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(AnnotationColor.Array());
	}
}

void CTexGenRenderer::RenderNodes(string TextileName)
{
	if (TextileName == "")
	{
		map<string, CTextile*>::const_iterator itTextile;
		for (itTextile = CTexGen::GetInstance().GetTextiles().begin(); itTextile != CTexGen::GetInstance().GetTextiles().end(); ++itTextile)
		{
			RenderNodes(*itTextile->second);
		}
		return;
	}

	CTextile *pTextile = CTexGen::GetInstance().GetTextile(TextileName);
	if (!pTextile)
		return;

	RenderNodes(*pTextile);
}

void CTexGenRenderer::RenderNodes(CTextile &Textile)
{
	BeginBatch();
	int i;
	vector<CYarn>::iterator itYarn;
	for (i=0; i<(int)Textile.GetYarns().size(); ++i)
	{
		RenderNodes(Textile, i);
	}
	EndBatch();
}

void CTexGenRenderer::RenderNodes(CTextile &Textile, int iYarn)
{
//	pair<XYZ, XYZ> Bounds;
	int i;
	XYZ Pos;
	double dRadius = 0.01*Textile.GetApproximateSize();
	CYarn &Yarn = Textile.GetYarns()[iYarn];
	const vector<CNode> &Nodes = Yarn.GetMasterNodes();
	vector<CNode>::const_iterator itNode;
	BeginBatch();
	for (itNode = Nodes.begin(), i=0; itNode != Nodes.end(); ++itNode, ++i)
	{
		vtkSphereSource* pSphere = vtkSphereSource::New();
		pSphere->SetRadius(dRadius);
		pSphere->SetPhiResolution(10);
		pSphere->SetThetaResolution(10);
		Pos = itNode->GetPosition();
		pSphere->SetCenter(Pos.x, Pos.y, Pos.z);

		vtkPolyDataMapper* pSphereMapper = vtkPolyDataMapper::New();
		pSphereMapper->SetInputConnection(pSphere->GetOutputPort());

		vtkActor* pSphereActor = vtkActor::New();
		pSphereActor->SetMapper(pSphereMapper);
		ApplyColor(pSphereActor, GetIndexedColor(iYarn));

		pSphere->Delete();
		pSphereMapper->Delete();

		PROP_NODE_INFO Info;
		Info.iYarn = iYarn;
		Info.iNode = i;
		Info.TextileName = Textile.GetName();
		m_NodeProps[pSphereActor] = Info;

		AddProp(PROP_NODE, pSphereActor);
	}
	EndBatch();
}


void CTexGenRenderer::RenderPaths(string TextileName)
{
	if (TextileName == "")
	{
		map<string, CTextile*>::const_iterator itTextile;
		for (itTextile = CTexGen::GetInstance().GetTextiles().begin(); itTextile != CTexGen::GetInstance().GetTextiles().end(); ++itTextile)
		{
			RenderPaths(*itTextile->second);
		}
		return;
	}

	CTextile *pTextile = CTexGen::GetInstance().GetTextile(TextileName);
	if (!pTextile)
		return;

	RenderPaths(*pTextile);
}

void CTexGenRenderer::RenderPaths(CTextile &Textile)
{
	int i, iNumYarns = (int)Textile.GetYarns().size();
	BeginBatch();
	for (i=0; i < iNumYarns; ++i)
	{
		RenderPath(Textile, i);
	}
	EndBatch();
}

void CTexGenRenderer::RenderPath(CTextile &Textile, int iYarn)
{
	double dRadius;
//	pair<XYZ, XYZ> Bounds;
	CYarn &Yarn = Textile.GetYarns()[iYarn];
	CMesh Mesh;
	Yarn.AddPathToMesh(Mesh);
	vtkPolyData *pPolyData = GetPolyData(Mesh);

//	Bounds = Mesh.GetAABB();

	dRadius = 0.005*Textile.GetApproximateSize();

	vtkTubeFilter* pProfileTubes = vtkTubeFilter::New();
	pProfileTubes->SetNumberOfSides(8);
	pProfileTubes->SetInputData(pPolyData);
	pProfileTubes->SetRadius(dRadius);
	pProfileTubes->CappingOn();

	vtkPolyDataMapper* pProfileMapper = vtkPolyDataMapper::New();
	pProfileMapper->SetInputConnection(pProfileTubes->GetOutputPort());

	vtkActor* pProfile = vtkActor::New();
	pProfile->SetMapper(pProfileMapper);
	ApplyColor(pProfile, GetIndexedColor(iYarn));

	pPolyData->Delete();
	pProfileTubes->Delete();
	pProfileMapper->Delete();

	PROP_YARN_INFO Info;
	Info.iYarn = iYarn;
	Info.TextileName = Textile.GetName();
	m_YarnProps[pProfile] = Info;

	AddProp(PROP_PATH, pProfile);
}

void CTexGenRenderer::RenderInterference(string TextileName, bool bDepth)
{
	if (TextileName == "")
	{
		map<string, CTextile*>::const_iterator itTextile;
		for (itTextile = CTexGen::GetInstance().GetTextiles().begin(); itTextile != CTexGen::GetInstance().GetTextiles().end(); ++itTextile)
		{
			RenderInterference(*itTextile->second, bDepth);
		}
		return;
	}

	CTextile *pTextile = CTexGen::GetInstance().GetTextile(TextileName);
	if (!pTextile)
		return;

	RenderInterference(*pTextile, bDepth);
}

void CTexGenRenderer::RenderInterference(CTextile &Textile, bool bDepth)
{
	CMesh InterferingPoints;
	vector<float> DistToSurface;
	vector<int> YarnIndices;
	vector<float>::iterator itDistToSurface;
	float fMaxDist = 0.0;
	bool TrimToDomain = Textile.GetDomain() && m_bTrimToDomain ? true : false;

	Textile.DetectInterference(DistToSurface, YarnIndices, TrimToDomain, &InterferingPoints);

	vtkPolyData *pPolyData = GetPolyData(InterferingPoints);

	if ( bDepth )
	{
		// Create array containing interference depths
		vtkFloatArray* pDistance = vtkFloatArray::New();
		pDistance->SetNumberOfValues( DistToSurface.size() );

		int i;
		
		for ( itDistToSurface = DistToSurface.begin(), i = 0; itDistToSurface != DistToSurface.end(); ++itDistToSurface, ++i )
		{
			float fDist = fabs( *itDistToSurface );
			if ( fDist > fMaxDist )
				fMaxDist = fDist;  // Get max interference to give display range
			pDistance->SetValue( i, fDist );
		}
		TGLOG("Maximum intersection depth to surface " << fMaxDist << "\"");
		// Set the scalar values of the points to the distance array - the spheres will be scaled to these values
		pPolyData->GetPointData()->SetScalars( pDistance );
	}

//	pair<XYZ, XYZ> Bounds = InterferingPoints.GetAABB();

//	double dRadius = 0.0025*GetLength(Bounds.first, Bounds.second);
	const double dRadius = 0.0025*Textile.GetApproximateSize();

	vtkSphereSource* pBalls = vtkSphereSource::New();
	pBalls->SetRadius(dRadius);
	pBalls->SetPhiResolution(10);
	pBalls->SetThetaResolution(10);

	vtkGlyph3D* pGlyphPoints = vtkGlyph3D::New();
	pGlyphPoints->SetInputData(pPolyData);
	pGlyphPoints->SetSourceConnection(pBalls->GetOutputPort());

	if ( bDepth )
	{
		// Set the glyphs to be scaled to the intereference range
		pGlyphPoints->SetScaleModeToScaleByScalar();
		pGlyphPoints->SetColorModeToColorByScale();
		pGlyphPoints->SetRange( 0.0, fMaxDist );
		pGlyphPoints->ClampingOn();
		pGlyphPoints->SetScaleFactor( 1 );
	}

	vtkPolyDataMapper* pGlyphMapper = vtkPolyDataMapper::New();
	pGlyphMapper->SetInputConnection(pGlyphPoints->GetOutputPort());

	vtkActor* pGlyph = vtkActor::New();
	pGlyph->SetMapper(pGlyphMapper);

	ApplyColor(pGlyph, COLOR(1,1,1));

	pPolyData->Delete();
	pBalls->Delete();
	pGlyphPoints->Delete();
	pGlyphMapper->Delete();

	AddProp(bDepth ? PROP_INTERFERENCE_DEPTH : PROP_INTERFERENCE, pGlyph);
}

void CTexGenRenderer::RenderGrid(string TextileName, int iResX, int iResY, int iResZ, double dLengthModifier)
{
	if (TextileName == "")
	{
		map<string, CTextile*>::const_iterator itTextile;
		for (itTextile = CTexGen::GetInstance().GetTextiles().begin(); itTextile != CTexGen::GetInstance().GetTextiles().end(); ++itTextile)
		{
			RenderGrid(*itTextile->second, iResX, iResY, iResZ, dLengthModifier);
		}
		return;
	}
	
	CTextile *pTextile = CTexGen::GetInstance().GetTextile(TextileName);
	if (!pTextile)
		return;

	RenderGrid(*pTextile, iResX, iResY, iResZ, dLengthModifier);
}

// Render a grid of points within given box domain, each point is rendered as a tangent representing the
// yarn tangent at that point
void CTexGenRenderer::RenderGrid(CTextile &Textile, int iResX, int iResY, int iResZ, double dLengthModifier)
{
	const CDomain *pDomain = Textile.GetDomain();

	pair<XYZ, XYZ> Limits;

	if ( pDomain ) // Use the domain
	{
		Limits = pDomain->GetMesh().GetAABB();
	}
	else  // Find the limits of the yarns
	{
		vector<CYarn> Yarns = Textile.GetYarns();
		vector<CYarn>::iterator itYarns;

		for ( itYarns = Yarns.begin(); itYarns != Yarns.end(); ++itYarns )
		{
			pair<XYZ, XYZ> YarnLimits = itYarns->GetAABB();
			Limits.first = Min( YarnLimits.first, Limits.first );
			Limits.second = Max( YarnLimits.second, Limits.second );
		}
	}

	XYZ Min = Limits.first;
	XYZ Max = Limits.second;

	vector<XYZ> Points;
	vector<POINT_INFO> PointInfo;

	XYZ Pos;
	int i, j, k;
	for (i=0; i<iResX; ++i)
	{
		for (j=0; j<iResY; ++j)
		{
			for (k=0; k<iResZ; ++k)
			{
				Pos.x = ((double)i+0.5)/(double(iResX)) * (Max.x-Min.x) + Min.x;
				Pos.y = ((double)j+0.5)/(double(iResY)) * (Max.y-Min.y) + Min.y;
				Pos.z = ((double)k+0.5)/(double(iResZ)) * (Max.z-Min.z) + Min.z;
				Points.push_back(Pos);
			}
		}
	}

	Textile.GetPointInformation(Points, PointInfo);

	double dLength = 0.02*GetLength(Min, Max)*dLengthModifier;

	vtkPoints* pPoints = vtkPoints::New();
	vtkFloatArray* pNormals = vtkFloatArray::New();
	pNormals->SetNumberOfComponents(3);
	for (i=0; i<(int)Points.size(); ++i)
	{
//		if (PointInfo[i].iYarnIndex != -1)
//			RenderDirection(Points[i], PointInfo[i].YarnTangent, PROP_ORIENTATION, dLength, GetIndexedColor(PointInfo[i].iYarnIndex), false);
//		else
//			RenderPoint(Points[i]);
		if (PointInfo[i].iYarnIndex != -1)
		{
			pPoints->InsertNextPoint(Points[i].x, Points[i].y, Points[i].z);
			//pNormals->InsertNextTuple3(PointInfo[i].YarnTangent.x, PointInfo[i].YarnTangent.y, PointInfo[i].YarnTangent.z);
			pNormals->InsertNextTuple3(PointInfo[i].Orientation.x, PointInfo[i].Orientation.y, PointInfo[i].Orientation.z);
		}
	}
	vtkPolyData* pPolyData = vtkPolyData::New();
	pPolyData->SetPoints(pPoints);
	pPolyData->GetPointData()->SetVectors(pNormals);

	vtkArrowSource *pArrow = vtkArrowSource::New();
//	pArrow->SetTipRadius(0.2);
//	pArrow->SetShaftRadius(0.1);
//	pArrow->SetTipLength(0.1);
//	pArrow->SetTipResolution(8);
//	pArrow->SetShaftResolution(8);

	vtkGlyph3D* pGlyphPoints = vtkGlyph3D::New();
	pGlyphPoints->SetScaleFactor(dLength);
	pGlyphPoints->SetVectorModeToUseVector();
	pGlyphPoints->SetSourceConnection(pArrow->GetOutputPort());
	pGlyphPoints->SetInputData(pPolyData);

	vtkPolyDataMapper* pGlyphMapper = vtkPolyDataMapper::New();
	pGlyphMapper->SetInputConnection(pGlyphPoints->GetOutputPort());

	vtkActor* pGlyph = vtkActor::New();
	pGlyph->SetMapper(pGlyphMapper);
	ApplyColor(pGlyph, COLOR(1,1,1));

	pPolyData->Delete();
	pArrow->Delete();
	pGlyphPoints->Delete();
	pGlyphMapper->Delete();

	AddProp(PROP_ORIENTATION, pGlyph);
}

/*bool CTexGenRenderer::RenderImage(string FileName, double dMaxDim, XYZ Orientation)
{
	vtkImageReader2* pReader = vtkImageReader2Factory::CreateImageReader2(FileName.c_str());
	if (!pReader)
		return false;
//	vtkPNGReader* pReader = vtkPNGReader::New();
	pReader->SetFileName(FileName.c_str());
	vtkImageActor* pImageActor = vtkImageActor::New();
	pImageActor->SetInputData(pReader->GetOutputDataObject(());
	double Bounds[6];
	pImageActor->GetBounds(Bounds);
	double dMaxBound = Bounds[1] - Bounds[0];
	dMaxBound = max(dMaxBound, Bounds[2] - Bounds[1]);
	dMaxBound = max(dMaxBound, Bounds[3] - Bounds[2]);
	pImageActor->SetScale(dMaxDim/dMaxBound);
//	pImageActor->RotateWXYZ(Orientation.w, Orientation.x, Orientation.y, Orientation.z);
	pImageActor->SetOrientation(Orientation.x, Orientation.y, Orientation.z);
	pReader->Delete();
	PROP_IMAGE_INFO Info;
	Info.iImageID = GetUniqueImageID();
	m_ImageProps[pImageActor] = Info;
	AddProp(PROP_IMAGE, pImageActor);
	return true;
}*/

void CTexGenRenderer::ResetCamera(XYZ LookDirection)
{
	vtkCamera *camera = m_pRenderer->GetActiveCamera();

	if (!LookDirection)
	{
		XYZ Position, FocalPoint;
		camera->GetPosition(Position.x, Position.y, Position.z);
		camera->GetFocalPoint(FocalPoint.x, FocalPoint.y, FocalPoint.z);
		LookDirection = FocalPoint - Position;
	}
	XYZ ViewUp(0, 0, 1);
	if (!CrossProduct(ViewUp, LookDirection))
	{
		LookDirection = XYZ(0, 1, -0.5);
	}
	camera->SetFocalPoint(LookDirection.x, LookDirection.y, LookDirection.z);
	camera->SetPosition(0, 0, 0);
	camera->SetViewUp(ViewUp.x, ViewUp.y, ViewUp.z);
//	camera->ComputeViewPlaneNormal();

	m_pRenderer->ResetCamera();

	RefreshView();
}

void CTexGenRenderer::RefreshView()
{
//	vtkCamera *camera = m_pRenderer->GetActiveCamera();
//	camera->ResetCameraClippingRange();
	if (m_iBatchCount != 0)
		return;

	m_pRenderer->Render();
	m_pRenderWindowInteractor->Render();

	if (m_pCallback)
		m_pCallback();
}

void CTexGenRenderer::RenderAxes()
{
/*	double Bounds[6];
	m_pRenderer->ComputeVisiblePropBounds(Bounds);
	double dRadius = sqrt((Bounds[1]-Bounds[0])*(Bounds[1]-Bounds[0]) +
							(Bounds[3]-Bounds[2])*(Bounds[3]-Bounds[2]) +
							(Bounds[5]-Bounds[4])*(Bounds[5]-Bounds[4])) /2.0;*/
/*	vtkOrientationMarkerWidget* pOrient = vtkOrientationMarkerWidget::New();
	vtkAxesActor* pAxes = vtkAxesActor::New();

	pOrient->SetOrientationMarker(pAxes);
	pOrient->SetInteractor(m_pRenderWindowInteractor);
	pOrient->SetOutlineColor(0.93, 0.57, 0.13);
	pOrient->SetViewport(0.0, 0.0, 0.15, 0.2);
	pOrient->InteractiveOff();*/
	m_pOrientationMarkerWidget->SetEnabled(1);
	m_pOrientationMarkerWidget->InteractiveOff();
	RefreshView();

//	pOrient->Delete();
//	pAxes->Delete();

//	RenderDirection(XYZ(0, 0, 0), XYZ(1, 0, 0), PROP_AXES, 0.1*dRadius, COLOR_RED);
//	RenderDirection(XYZ(0, 0, 0), XYZ(0, 1, 0), PROP_AXES, 0.1*dRadius, COLOR_GREEN);
//	RenderDirection(XYZ(0, 0, 0), XYZ(0, 0, 1), PROP_AXES, 0.1*dRadius, COLOR_BLUE);
}

vtkPolyData *CTexGenRenderer::GetPolyData(const CMesh &Mesh)
{
	int i, j;

	// We'll create the building blocks of polydata including data attributes.
	vtkPolyData *pPolyData = vtkPolyData::New();
	vtkPoints *pPoints = vtkPoints::New();
	vtkCellArray *pPolys = vtkCellArray::New();
	vtkCellArray *pLines = vtkCellArray::New();

	// Load the point, cell, and data attributes.
	vector<XYZ>::const_iterator itNode;
	for (itNode = Mesh.NodesBegin(), i=0; itNode != Mesh.NodesEnd(); ++itNode, ++i)
	{
		pPoints->InsertPoint(i, itNode->x, itNode->y, itNode->z);
	}
	list<int>::const_iterator itIter;
	int iNumNodes, iStartIndex, iEndIndex;
	for (j = 0; j < CMesh::NUM_ELEMENT_TYPES; ++j)
	{
		const list<int> &Indices = Mesh.GetIndices((CMesh::ELEMENT_TYPE)j);
		iNumNodes = CMesh::GetNumNodes((CMesh::ELEMENT_TYPE)j);
		for (itIter = Indices.begin(); itIter != Indices.end(); )
		{
			if (j == CMesh::POLYLINE)
			{
				iStartIndex = *(itIter++);
				iEndIndex = *(itIter++);
				pLines->InsertNextCell(iEndIndex-iStartIndex);
				for (i=iStartIndex; i<iEndIndex; ++i)
				{
					pLines->InsertCellPoint(i);
				}
			}
			else if (j == CMesh::HEX || j == CMesh::WEDGE )
			{
				vector<int> iFirst;
				for ( int side = 0; side < 2; ++side )
				{
					pPolys->InsertNextCell(iNumNodes/2);
					for ( i = 0; i < iNumNodes/2; ++i )
					{
						if ( side == 0 )
						{
							iFirst.push_back( *itIter );
						}
						else
						{
							pLines->InsertNextCell(2);
							pLines->InsertCellPoint( iFirst[i] );
							pLines->InsertCellPoint( *itIter );
						}
						pPolys->InsertCellPoint(*itIter);
						++itIter;
					}
				}
			}
			else if ( j != CMesh::POLYGON ) // Don't want to render polygons (at the moment)
			{
				pPolys->InsertNextCell(iNumNodes);
				for (i=0; i<iNumNodes; ++i)
				{
					pPolys->InsertCellPoint(*itIter);
					++itIter;
				}
			}
			else
			{
				break; // If POLYGON just continue to next element type
			}
		}
	}
	// We now assign the pieces to the vtkPolyData.
	pPolyData->SetPoints(pPoints);
	pPoints->Delete();
	pPolyData->SetPolys(pPolys);
	pPolys->Delete();
	pPolyData->SetLines(pLines);
	pLines->Delete();

	return pPolyData;
}

vtkAlgorithm *CTexGenRenderer::CalculateNormals(vtkPolyData *pInput)
{
	vtkPolyDataNormals *pPolyDataNormals = vtkPolyDataNormals::New();

	pPolyDataNormals->SetFeatureAngle(45);
	// The following options don't seem to operate the way they should?!
	pPolyDataNormals->ConsistencyOff();
	pPolyDataNormals->NonManifoldTraversalOn();
	pPolyDataNormals->SplittingOn();
	pPolyDataNormals->ComputePointNormalsOn();
	pPolyDataNormals->ComputeCellNormalsOn();
	pPolyDataNormals->FlipNormalsOff();

	pPolyDataNormals->SetInputData(pInput);

	pInput->Delete();

	return pPolyDataNormals;
}

void CTexGenRenderer::SetCallback(void (*pCallback)())
{
	m_pCallback = pCallback;
}

void CTexGenRenderer::ApplyDefaultColor(vtkActor *pActor)
{
	COLOR Color;
	const PROP_YARN_INFO *pYarnInfo = GetYarnPropInfo(pActor);
	if (pYarnInfo)
	{
		Color = GetIndexedColor(pYarnInfo->iYarn);
	}
	const PROP_NODE_INFO *pNodeInfo = GetNodePropInfo(pActor);
	if (pNodeInfo)
	{
		Color = GetIndexedColor(pNodeInfo->iYarn);
	}
	ApplyColor(pActor, Color);
}

void CTexGenRenderer::SetYarnColor(int iYarn, COLOR Color)
{
	PROP_YARN_INFO YarnInfo;
	YarnInfo.iYarn = iYarn;
	vector<vtkProp*> Props = GetProps(&YarnInfo, true);
	vector<vtkProp*>::iterator itProp;
	for (itProp = Props.begin(); itProp != Props.end(); ++itProp)
	{
		vtkActor* pActor = dynamic_cast<vtkActor*>(*itProp);
		if (pActor)
			ApplyColor(pActor, Color);
	}
	RefreshView();
}

void CTexGenRenderer::ApplyColor(vtkActor *pActor, COLOR Color)
{
	pActor->GetProperty()->SetDiffuse(0.7);
	pActor->GetProperty()->SetDiffuseColor(Color.Array());
	pActor->GetProperty()->SetSpecular(0.3);
	pActor->GetProperty()->SetSpecularPower(30);
	pActor->GetProperty()->SetAmbient(0.3);
	pActor->GetProperty()->SetAmbientColor(Color.Array());
}

bool CTexGenRenderer::SaveTextileToVTK(string FileName, string TextileName, int iExportFlags)
{
	CMesh Mesh;

	vector<CTextile*> Textiles;
	if (TextileName == "")
	{
		map<string, CTextile*>::const_iterator itTextile;
		for (itTextile = TEXGEN.GetTextiles().begin(); itTextile != TEXGEN.GetTextiles().end(); ++itTextile)
		{
			Textiles.push_back(itTextile->second);
		}
	}
	else
	{
		CTextile *pTextile = TEXGEN.GetTextile(TextileName);
		if (pTextile)
			Textiles.push_back(pTextile);
	}
	if (Textiles.empty())
		return false;
	vector<CTextile*>::iterator itTextile;
	for (itTextile = Textiles.begin(); itTextile != Textiles.end(); ++itTextile)
	{
		if (iExportFlags & EXPORT_NODES)
			(*itTextile)->AddNodesToMesh(Mesh);
		if (iExportFlags & EXPORT_PATH)
			(*itTextile)->AddPathToMesh(Mesh);
		if (iExportFlags & EXPORT_SURFACE)
			(*itTextile)->AddSurfaceToMesh(Mesh, m_bTrimToDomain);
		if (iExportFlags & EXPORT_DOMAIN)
		{
			const CDomain* pDomain = (*itTextile)->GetDomain();
			if (pDomain)
			{
				Mesh.InsertMesh(pDomain->GetMesh());
			}
		}
	}
	return SaveMeshToVTK(FileName, Mesh);
}

bool CTexGenRenderer::SaveMeshToVTK(string FileName, const CMesh &Mesh)
{
	if (FileName.size()<4 || FileName.rfind(".vtp") != FileName.size()-4)
		FileName += ".vtp";
	vtkPolyData* pPolyData = GetPolyData(Mesh);
	vtkXMLPolyDataWriter* pPolyDataWriter = vtkXMLPolyDataWriter::New(); 
	pPolyDataWriter->SetInputData(pPolyData);
	pPolyDataWriter->SetDataModeToBinary();
	pPolyDataWriter->SetFileName(FileName.c_str());
	bool bSuccess = pPolyDataWriter->Write()?true:false;

	pPolyData->Delete();
	pPolyDataWriter->Delete();

	return bSuccess;
}

void CTexGenRenderer::BeginBatch()
{
	++m_iBatchCount;
}

void CTexGenRenderer::EndBatch()
{
	--m_iBatchCount;
	if (m_iBatchCount == 0)
		RefreshView();
}

int CTexGenRenderer::GetUniqueImageID()
{
	map<vtkProp*, PROP_IMAGE_INFO>::iterator itImage;
	int iPrevID = 0, iID = 0;
	do
	{
		iPrevID = iID;
		for (itImage = m_ImageProps.begin(); itImage != m_ImageProps.end(); ++itImage)
		{
			if (iID == itImage->second.iImageID)
			{
				++iID;
			}
		}
	} while (iPrevID != iID);
	return iID;
}














