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
#include "CustomInteractorStyle.h"

#include "vtkMath.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(CCustomInteractorStyle);

void CCustomInteractorStyle::Rotate()
{
	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;

	int dx = - (rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0]);
	int dy = - (rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1]);

	int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

	double a = (double)dx / (double)size[0] * (double)180.0;
	double e = (double)dy / (double)size[1] * (double)180.0;

	if (rwi->GetShiftKey()) 
	{
		if (fabs((double)dx) >= fabs((double)dy))
		{
			e = (double)0.0;
		}
		else
		{
			a = (double)0.0;
		}
	}

	// Move the camera. 
	// Make sure that we don't hit the north pole singularity.

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	camera->Azimuth(a);

	double dop[3], vup[3];

	camera->GetDirectionOfProjection(dop);
	vtkMath::Normalize(dop);
	camera->GetViewUp(vup);
	vtkMath::Normalize(vup);

	const double dAngleLimit = 5.0;

	double angle = acos(vtkMath::Dot(dop, vup)) / vtkMath::RadiansFromDegrees( 1.0 );
	if ((angle + e) > 180.0-dAngleLimit)
	{
		e = 180.0-dAngleLimit-angle;
	}
	else if ((angle + e) < dAngleLimit)
	{
		e = dAngleLimit-angle;
	}

	camera->Elevation(e);
	if (this->AutoAdjustCameraClippingRange)
	{
		this->CurrentRenderer->ResetCameraClippingRange();
	}

	rwi->Render();
}








