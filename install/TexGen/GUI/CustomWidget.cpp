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
#include "CustomWidget.h"
#include "../Renderer/PrecompiledHeaders.h"
#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkCellPicker.h"

vtkStandardNewMacro(vtkCustomWidget);

vtkCustomWidget::vtkCustomWidget()
: m_pXArrow(NULL)
, m_pYArrow(NULL)
, m_pZArrow(NULL)
, m_pHandlePicker(NULL)
, m_iSelectAxis(-1)
, m_State(START)
, m_dSnapDistance(0.1)
, m_bRelativeSnap(true)
, m_bSnap(true)
{
	EventCallbackCommand->SetCallback(vtkCustomWidget::ProcessEvents);
	for (int i=0; i<3; ++i)
	{
		vtkArrowSource* pArrow = vtkArrowSource::New();
//		pArrow->SetShaftResolution(8);
//		pArrow->SetTipResolution(8);
		vtkPolyDataMapper* pPolyDataMapper = vtkPolyDataMapper::New();
		pPolyDataMapper->SetInputConnection(pArrow->GetOutputPort());
		vtkActor* pActor = vtkActor::New();
		pActor->SetMapper(pPolyDataMapper);
		pActor->SetScale(2);
		switch (i)
		{
		case 0:
			m_pXArrow = pActor;
			pActor->GetProperty()->SetDiffuseColor(COLOR(1, 0, 0).Array());
			break;
		case 1:
			m_pYArrow = pActor;
			pActor->RotateZ(90);
			pActor->GetProperty()->SetDiffuseColor(COLOR(0, 1, 0).Array());
			break;
		case 2:
			m_pZArrow = pActor;
			pActor->RotateY(-90);
			pActor->GetProperty()->SetDiffuseColor(COLOR(0, 0, 1).Array());
			break;
		}
		pArrow->Delete();
		pPolyDataMapper->Delete();
	}
	m_pHandlePicker = vtkCellPicker::New();
	m_pHandlePicker->SetTolerance(0.001);
	m_pHandlePicker->AddPickList(m_pXArrow);
	m_pHandlePicker->AddPickList(m_pYArrow);
	m_pHandlePicker->AddPickList(m_pZArrow);
	m_pHandlePicker->PickFromListOn();
}

vtkCustomWidget::~vtkCustomWidget()
{
	if (m_pXArrow)
		m_pXArrow->Delete();
	if (m_pYArrow)
		m_pYArrow->Delete();
	if (m_pZArrow)
		m_pZArrow->Delete();
	if (m_pHandlePicker)
		m_pHandlePicker->Delete();
}

void vtkCustomWidget::SetEnabled(int enabling)
{
	if (!Interactor)
	{
		vtkErrorMacro(<<"The interactor must be set prior to enabling/disabling widget");
		return;
	}
	if (enabling)
	{
		if (Enabled)
			return;
		if (!CurrentRenderer)
		{
			SetCurrentRenderer(Interactor->FindPokedRenderer(
				Interactor->GetLastEventPosition()[0],
				Interactor->GetLastEventPosition()[1]));
			if (CurrentRenderer == NULL)
			{
				return;
			}
		}

		// listen to the following events
		Interactor->AddObserver(vtkCommand::MouseMoveEvent, EventCallbackCommand, Priority);
		Interactor->AddObserver(vtkCommand::LeftButtonPressEvent, EventCallbackCommand, Priority);
		Interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, EventCallbackCommand, Priority);
		Interactor->AddObserver(vtkCommand::MiddleButtonPressEvent, EventCallbackCommand, Priority);
		Interactor->AddObserver(vtkCommand::MiddleButtonReleaseEvent, EventCallbackCommand, Priority);
		Interactor->AddObserver(vtkCommand::RightButtonPressEvent, EventCallbackCommand, Priority);
		Interactor->AddObserver(vtkCommand::RightButtonReleaseEvent, EventCallbackCommand, Priority);

		CurrentRenderer->AddActor(m_pXArrow);
		CurrentRenderer->AddActor(m_pYArrow);
		CurrentRenderer->AddActor(m_pZArrow);

		Enabled = 1;
	}
	else
	{
		if (!Enabled)
			return;
		Enabled = 0;

		Interactor->RemoveObserver(EventCallbackCommand);

		CurrentRenderer->RemoveActor(m_pXArrow);
		CurrentRenderer->RemoveActor(m_pYArrow);
		CurrentRenderer->RemoveActor(m_pZArrow);

		InvokeEvent(vtkCommand::DisableEvent, NULL);

		SetCurrentRenderer(NULL);
	}
	Interactor->Render();
}

void vtkCustomWidget::SetScale(double dScale)
{
	m_pXArrow->SetScale(dScale);
	m_pYArrow->SetScale(dScale);
	m_pZArrow->SetScale(dScale);
}

void vtkCustomWidget::PlaceWidget(double bounds[6])
{
	
}

void vtkCustomWidget::ProcessEvents(vtkObject* vtkNotUsed(object), 
                                 unsigned long event,
                                 void* clientdata, 
                                 void* vtkNotUsed(calldata))
{
	vtkCustomWidget* self = reinterpret_cast<vtkCustomWidget *>( clientdata );

	//okay, let's do the right thing
	switch(event)
	{
	case vtkCommand::LeftButtonPressEvent:
		self->OnLeftButtonDown();
		break;
	case vtkCommand::LeftButtonReleaseEvent:
		self->OnLeftButtonUp();
		break;
	case vtkCommand::MiddleButtonPressEvent:
		self->OnMiddleButtonDown();
		break;
	case vtkCommand::MiddleButtonReleaseEvent:
		self->OnMiddleButtonUp();
		break;
	case vtkCommand::RightButtonPressEvent:
		self->OnRightButtonDown();
		break;
	case vtkCommand::RightButtonReleaseEvent:
		self->OnRightButtonUp();
		break;
	case vtkCommand::MouseMoveEvent:
		self->OnMouseMove();
		break;
	}
}

void vtkCustomWidget::HighlightHandle(vtkActor *pActor)
{
	// Reset default colors
	m_pXArrow->GetProperty()->SetDiffuseColor(COLOR(1, 0, 0).Array());
	m_pYArrow->GetProperty()->SetDiffuseColor(COLOR(0, 1, 0).Array());
	m_pZArrow->GetProperty()->SetDiffuseColor(COLOR(0, 0, 1).Array());
	// Set yellow for the selected
	if (pActor)
	{
		pActor->GetProperty()->SetDiffuseColor(COLOR(1, 1, 0).Array());
	}
	if (pActor == m_pXArrow)
		m_iSelectAxis = 0;
	else if (pActor == m_pYArrow)
		m_iSelectAxis = 1;
	else if (pActor == m_pZArrow)
		m_iSelectAxis = 2;
	else
		m_iSelectAxis = -1;
}

void vtkCustomWidget::SetPosition(XYZ Position, bool DeselectAxis)
{
	m_StartPosition = m_UnsnappedPosition = m_CurrentPosition = Position;
	UpdateHandlePositions();
	if (DeselectAxis)
	{
		HighlightHandle(NULL);
	}
}

void vtkCustomWidget::UpdateHandlePositions()
{
	m_pXArrow->SetPosition(m_CurrentPosition.x, m_CurrentPosition.y, m_CurrentPosition.z);
	m_pYArrow->SetPosition(m_CurrentPosition.x, m_CurrentPosition.y, m_CurrentPosition.z);
	m_pZArrow->SetPosition(m_CurrentPosition.x, m_CurrentPosition.y, m_CurrentPosition.z);
}

XYZ vtkCustomWidget::GetPosition()
{
	return m_CurrentPosition;
}

void vtkCustomWidget::OnMouseMove()
{
	// See whether we're active
	if (m_State == START/* || m_State == OUTSIDE*/)
		return;

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	double displayStartPt[3], currentPt[4], prevPt[4];

	vtkCamera *camera = CurrentRenderer->GetActiveCamera();
	if (!camera)
		return;

	ComputeWorldToDisplay(m_CurrentPosition.x, m_CurrentPosition.y, m_CurrentPosition.z, displayStartPt);
	ComputeDisplayToWorld(double(X), double(Y), displayStartPt[2], currentPt);
	ComputeDisplayToWorld(double(Interactor->GetLastEventPosition()[0]), double(Interactor->GetLastEventPosition()[1]), displayStartPt[2], prevPt);

	XYZ CurrentPos = currentPt;
	XYZ PrevPos = prevPt;
	XYZ Delta;
	if (m_iSelectAxis>=0 && m_iSelectAxis<3)
	{
		XYZ Axis;
		Axis[m_iSelectAxis] = 1;

		Delta = Axis*DotProduct(CurrentPos-PrevPos, Axis);
	}
	else
	{
		Delta = CurrentPos-PrevPos;
	}
	m_UnsnappedPosition += Delta;
	m_CurrentPosition = m_UnsnappedPosition;
	if (m_bSnap && m_dSnapDistance>1e-6)
	{
		if (m_bRelativeSnap)
			m_CurrentPosition -= m_StartPosition;
		m_CurrentPosition /= m_dSnapDistance;
		m_CurrentPosition.x = RoundValue(m_CurrentPosition.x);
		m_CurrentPosition.y = RoundValue(m_CurrentPosition.y);
		m_CurrentPosition.z = RoundValue(m_CurrentPosition.z);
		m_CurrentPosition *= m_dSnapDistance;
		if (m_bRelativeSnap)
			m_CurrentPosition += m_StartPosition;
	}
	UpdateHandlePositions();

	// Interact, if desired
	EventCallbackCommand->SetAbortFlag(1);
	InvokeEvent(vtkCommand::InteractionEvent, NULL);
	Interactor->Render();
}

void vtkCustomWidget::OnLeftButtonDown()
{
	if (Interactor->GetControlKey())
		return;
	int X = Interactor->GetEventPosition()[0];
	int Y = Interactor->GetEventPosition()[1];

	// Okay, we can process this. Try to pick handles first;
	// if no handles picked, then pick the bounding box.
	if (!CurrentRenderer || !CurrentRenderer->IsInViewport(X, Y))
	{
		m_State = OUTSIDE;
		return;
	}

//	vtkAssemblyPath *path;
	m_pHandlePicker->Pick(X, Y, 0.0, CurrentRenderer);
//	path = m_pHandlePicker->GetPath();
	vtkActor* pActor = m_pHandlePicker->GetActor();
	if (pActor)
	{
		HighlightHandle(pActor);
		m_State = ACTIVE;
//		m_pHandlePicker->GetPickPosition(this->LastPickPosition);
//		this->ValidPick = 1;
	}
	else
	{
		m_State = OUTSIDE;
		return;
	}
	EventCallbackCommand->SetAbortFlag(1);
	StartInteraction();
	InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	Interactor->Render();
}

void vtkCustomWidget::OnLeftButtonUp()
{
	if (/*m_State == OUTSIDE ||*/ m_State == START)
		return;

	m_State = START;
//	HighlightHandle(NULL);

	EventCallbackCommand->SetAbortFlag(1);
	EndInteraction();
	InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
	Interactor->Render();
}

void vtkCustomWidget::OnMiddleButtonDown()
{
	if (Interactor->GetControlKey())
		return;
	int X = Interactor->GetEventPosition()[0];
	int Y = Interactor->GetEventPosition()[1];

	// Okay, we can process this. Try to pick handles first;
	// if no handles picked, then pick the bounding box.
	if (!CurrentRenderer || !CurrentRenderer->IsInViewport(X, Y))
	{
		m_State = OUTSIDE;
		return;
	}

	m_State = ACTIVE;

	EventCallbackCommand->SetAbortFlag(1);
	StartInteraction();
	InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
	Interactor->Render();
}

void vtkCustomWidget::OnMiddleButtonUp()
{
	if (/*m_State == OUTSIDE ||*/ m_State == START)
		return;

	m_State = START;
//	HighlightHandle(NULL);

	EventCallbackCommand->SetAbortFlag(1);
	EndInteraction();
	InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
	Interactor->Render();
}

void vtkCustomWidget::OnRightButtonDown()
{

}

void vtkCustomWidget::OnRightButtonUp()
{

}

vector<vtkProp*> vtkCustomWidget::GetProps()
{
	vector<vtkProp*> Props;
	Props.push_back(m_pXArrow);
	Props.push_back(m_pYArrow);
	Props.push_back(m_pZArrow);
	return Props;
}









