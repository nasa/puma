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
#pragma once

class vtkActor;
class vtkPointWidget;
class vtkCustomWidget;
class vtkProp;
class vtkCellPicker;
class vtkPropPicker;

#include "vtkCommand.h"

namespace TexGen
{
	class CTexGenRenderer;
	struct PROP_INFO;
	struct PROP_YARN_INFO;
	struct PROP_NODE_INFO;
	struct PROP_IMAGE_INFO;
}

enum TOOLS
{
	TOOL_NONE,
	TOOL_SELECT,
	TOOL_MOVE,
	TOOL_ROTATE,
	TOOL_SCALE,
};

class CModeller;
class vtkAbstractPropPicker;
class CTexGenMainFrame;

class vtkMyCallback : public vtkCommand
{
public:
	vtkMyCallback(CModeller* pModeller) : m_pModeller(pModeller) {;}
	void Execute(vtkObject *caller, unsigned long eventId, void* callData);
protected:
	CModeller* m_pModeller;
};

class CModeller
{
public:
	CModeller(CTexGenRenderer *pRenderer, string TextileName);
	~CModeller(void);

	void OnButtonDown(wxMouseEvent &event);
	void OnButtonUp(wxMouseEvent &event);
    void OnKeyDown(wxKeyEvent &event);
	void OnKeyUp(wxKeyEvent &event);

//	void SelectActor(vtkActor* pActor);
	void CreateYarn();
	void DeselectObject(PROP_INFO* pObject, bool bUpdateWidget = true, bool bUpdateOutliner = true);
	void DeselectAll(bool bUpdateOutliner = true);
	void SelectObject(PROP_INFO* pObject, bool bUpdateOutliner = true);
	void SetActiveTool(TOOLS Tool);
	TOOLS GetActiveTool() const {return m_ActiveTool;}
	void OnWidgetStartMove();
	void OnWidgetMove();
	void OnWidgetEndMove();
	void OnOutlinerSelectionChanged(wxPanel* pOutliner);
	void AssignSectionToSelectedObjects();
	void AssignInterpolationToSelectedObjects();
	void AssignRepeatsToSelectedObjects();
	void AssignPropertiesToSelectedObjects();
	void DisplayVolumeFractionForSelectedObjects();
	void AssignPropertiesToTextile();
	void AssignMatrixProperties();
	void DeleteSelectedObjects();
	void InsertNodes();
	void DuplicateYarns();
//	void ShowOutliner(bool bShow);
	void UpdateOutlinerItems();
	void UpdateOutlinerSelection();

	void SetPosition(int iCoordinate, double Value);
	XYZ GetPosition() const;

	void SetScale(XYZ Scale);

	void SetSelectNodes(bool bSelect) { m_bSelectNodes = bSelect; }
	void SetSelectPaths(bool bSelect) { m_bSelectPaths = bSelect; }
	void SetSelectSurfaces(bool bSelect) { m_bSelectSurfaces = bSelect; }
	void SetSelectImages(bool bSelect) { m_bSelectImages = bSelect; }
	bool GetSelectNodes() const { return m_bSelectNodes; }
	bool GetSelectPaths() const { return m_bSelectPaths; }
	bool GetSelectSurfaces() const { return m_bSelectSurfaces; }
	bool GetSelectImages() const { return m_bSelectImages; }

	vtkCustomWidget* GetWidget() { return m_pCustomWidget; }
	const vtkCustomWidget* GetWidget() const { return m_pCustomWidget; }

protected:
	void CreateWidgets();
	void CreatePicker();
	void UpdateWidget();
	void MoveSelectedObjects(XYZ DeltaPos);
	void ScaleSelectedObjects(XYZ DeltaScale);
	void HighlightSelectedObjects();
	vector<PROP_YARN_INFO> GetSelectedYarns() const;
	vector<PROP_NODE_INFO> GetSelectedNodes() const;
	CYarn *GetYarn(PROP_YARN_INFO YarnInfo) const;
	const CNode *GetNode(PROP_NODE_INFO NodeInfo) const;
	void RefreshSelectedYarns();
	bool HitWidget(double dX, double dY);
	bool SelectNode(double dX, double dY);
	bool SelectPath(double dX, double dY);
	bool SelectSurface(double dX, double dY);
	bool SelectImage(double dX, double dY);
	void SetPickFilter(const vector<vtkProp*> &Props);
	void DeleteInvalidYarns();
	void SetDefaultRenderState();
	wxTreeItemId GetOutlinerItem(wxTreeCtrl* pTreeCtrl, int iYarn);
	wxTreeItemId GetOutlinerItem(wxTreeCtrl* pTreeCtrl, int iYarn, int iNode);
	void ScaleWidget();
	static void CreatePropertiesGrid(wxGrid* pGrid, vector<pair<wxString, CProperty> > &Properties);
	static void SavePropertiesFromGrid(wxGrid* pGrid, vector<pair<wxString, CProperty> > &Properties);

	string m_TextileName;
	CTexGenRenderer *m_pRenderer;
	vector<PROP_INFO*> m_SelectedObjects;
//	PROP_NODE_INFO *m_pSelectedNode;
//	vector<vtkActor*> m_SelectedActors;
	vtkCustomWidget* m_pCustomWidget;
	vtkMyCallback* m_pCallBack;
	XYZ m_PrevWidgetPos;
	vtkCellPicker* m_pCellPicker;
	vtkPropPicker* m_pPropPicker;

	TOOLS m_ActiveTool;
	bool m_bSelectNodes;
	bool m_bSelectPaths;
	bool m_bSelectSurfaces;
	bool m_bSelectImages;
	bool m_bIgnoreSelectionEvent;
};


inline void vtkMyCallback::Execute(vtkObject *caller, unsigned long eventId, void* callData)
{
	switch (eventId)
	{
	case vtkCommand::StartInteractionEvent:
		m_pModeller->OnWidgetStartMove();
		break;
	case vtkCommand::InteractionEvent:
		m_pModeller->OnWidgetMove();
		break;
	case vtkCommand::EndInteractionEvent:
		m_pModeller->OnWidgetEndMove();
		break;
	}
}














