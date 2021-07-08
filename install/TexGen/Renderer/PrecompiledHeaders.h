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

#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#define TIXML_USE_STL
#include "../tinyxml/tinyxml.h"
#include "../Core/mymath.h"

#include "vtkOrientationMarkerWidget.h"
#include "vtkAxesActor.h"
#include "vtkTubeFilter.h"
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkCylinderSource.h"
#include "vtkArrowSource.h"
#include "vtkAxes.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtkInteractorStyleTerrain.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkCamera.h"
#include "vtkPolyDataNormals.h"
#include "vtkMatrix4x4.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkAxisActor2D.h"
#include "vtkCaptionActor2D.h"
#include "vtkCubeAxesActor2D.h"
#include "vtkRenderLargeImage.h"
#include "vtkPNGWriter.h"
//#include "vtkImageResample.h"
//#include "vtkImageCast.h"
#include "vtkAbstractPropPicker.h"
#include "vtkAssemblyPath.h"
#include "vtkPointWidget.h"
#include "vtkBoxWidget.h"
#include "vtkCommand.h"
#include "vtkPropPicker.h"
#include "vtkPropCollection.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkPNGReader.h"
#include "vtkImageActor.h"
#include "vtkImageReader2Factory.h"
#include "vtkDataArrayTemplate.h"


using namespace std;
