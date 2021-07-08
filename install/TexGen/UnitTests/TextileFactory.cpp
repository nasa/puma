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

#include "TextileFactory.h"
#include "../Core/PrecompiledHeaders.h"
#include "../Core/TexGen.h"

CTextileFactory::CTextileFactory()
{

}

CTextileFactory::~CTextileFactory()
{

}

CTextileWeave2D CTextileFactory::SatinWeave()
{
	CTextileWeave2D Textile(4, 2, 2, 1, false);
	Textile.SwapPosition(0, 0);
	Textile.SwapPosition(1, 1);
	Textile.SwapPosition(2, 1);
	Textile.SwapPosition(3, 0);
	Textile.AssignDefaultDomain();
	Textile.SetYarnWidths(1.8);
	return Textile;
}

CTextileWeave3D CTextileFactory::Weave3D8x4()
{
	CTextileWeave3D Textile = CTextileWeave3D(8, 4, 5, 7);

	Textile.AddYLayers(0, 1);
	Textile.AddYLayers(2, 1);
	Textile.AddYLayers(4, 1);
	Textile.AddYLayers(6, 1);
	Textile.AddXLayers();
	Textile.AddYLayers();
	Textile.AddXLayers();
	Textile.AddYLayers();
	Textile.AddXLayers();
	Textile.AddYLayers();

	Textile.PushUp(0, 0);
	Textile.PushUp(1, 0);
	Textile.PushDown(4, 0);
	Textile.PushUp(7, 0);

	Textile.PushUp(1, 1);
	Textile.PushUp(2, 1);
	Textile.PushUp(3, 1);
	Textile.PushDown(6, 1);

	Textile.PushDown(0, 2);
	Textile.PushUp(3, 2);
	Textile.PushUp(4, 2);
	Textile.PushUp(5, 2);

	Textile.PushDown(2, 3);
	Textile.PushUp(5, 3);
	Textile.PushUp(6, 3);
	Textile.PushUp(7, 3);

	Textile.SetYarnWidths(4);
	Textile.SetYarnHeights(1);

	Textile.AssignDefaultDomain();

	return Textile;
}

CTextile CTextileFactory::InterpTest()
{
	// Create a textile
	CTextile Textile;
	{
		// Create a yarn
		CYarn Yarn;

		// Add some nodes
		Yarn.AddNode(CNode(XYZ(0, 0, 0)));
		Yarn.AddNode(CNode(XYZ(5, 0, 2)));
		Yarn.AddNode(CNode(XYZ(10, 0, 0)));

		// Set the interpolation function
		CInterpolationAdjusted AdjustedInterp = CInterpolationCubic();
		AdjustedInterp.AddAdjustment(0, 0.0, XYZ(1, 2, 3));
		AdjustedInterp.AddAdjustment(0, 0.5, XYZ(0, 1, 0));
		AdjustedInterp.AddAdjustment(0, 1.0, XYZ(1, 2, 3));
		AdjustedInterp.AddAdjustment(1, 0.0, XYZ(1, 2, 3));
		AdjustedInterp.AddAdjustment(1, 0.5, XYZ(0, 0, 1));
		AdjustedInterp.AddAdjustment(1, 1.0, XYZ(0, 1, 0));
		Yarn.AssignInterpolation(AdjustedInterp);

		// Assign a constant cross-section all along the yarn
		Yarn.AssignSection(CYarnSectionConstant(CSectionEllipse(2, 1)));

		// Constant fibre distrib
		Yarn.AssignFibreDistribution(CFibreDistributionConst());
		Yarn.SetFibreArea(1, "mm^2");

		// Set the resolution
		Yarn.SetResolution(20);

		// Create some repeats for the yarn
		Yarn.AddRepeat(XYZ(10, 0, 0));
		Yarn.AddRepeat(XYZ(0, 10, 0));
		Yarn.AddRepeat(XYZ(0, 0, 3));

		// Add the yarn to our textile
		Textile.AddYarn(Yarn);
	}
	{
		// Create a new Yarn
		CYarn Yarn;

		// Add some nodes
		Yarn.AddNode(CNode(XYZ(0,0,2)));
		Yarn.AddNode(CNode(XYZ(0,5,0)));
		Yarn.AddNode(CNode(XYZ(0,10,2)));

		// Set the interpolation function
		Yarn.AssignInterpolation(CInterpolationBezier(false));

		// Assign a cross-section interpolated between the nodes
		CYarnSectionInterpNode YarnSection;
		YarnSection.AddSection(CSectionEllipse(2.2, 1.1));
		YarnSection.AddSection(CSectionEllipse(1.8, 0.9));
		YarnSection.AddSection(CSectionEllipse(2.4, 1.3));
		YarnSection.InsertSection(0, 0.25, CSectionLenticular(1.5, 0.4));
		YarnSection.InsertSection(1, 0.75, CSectionLenticular(2.3, 0.86));
		Yarn.AssignSection(YarnSection);

		// Constant fibre distrib
		Yarn.AssignFibreDistribution(CFibreDistribution1DQuad(0.5));
		Yarn.SetFibreArea(1, "mm^2");

		// Set the resolution
		Yarn.SetResolution(10);

		// Create some repeats for the yarn
		Yarn.AddRepeat(XYZ(5, 5, 0));
		Yarn.AddRepeat(XYZ(0, 10, 0));

		// Add the yarn to our textile
		Textile.AddYarn(Yarn);
	}
	{
		// Create a new Yarn
		CYarn Yarn;

		// Add some nodes
		Yarn.AddNode(CNode(XYZ(5,0,2)));
		Yarn.AddNode(CNode(XYZ(5,5,0)));
		Yarn.AddNode(CNode(XYZ(5,10,2)));

		// Set the interpolation function
		Yarn.AssignInterpolation(CInterpolationCubic(false));

		// Assign a cross-section interpolated between the nodes
		CYarnSectionInterpPosition YarnSection;
		YarnSection.AddSection(0, CSectionEllipse(2.2, 1.1));
		YarnSection.AddSection(0.23, CSectionEllipse(1.8, 0.9));
		YarnSection.AddSection(0.67, CSectionEllipse(2.4, 1.3));
		Yarn.AssignSection(YarnSection);

		// Set the resolution
		Yarn.SetResolution(40);

		// Create some repeats for the yarn
		Yarn.AddRepeat(XYZ(0, 0, 4));

		// Add the yarn to our textile
		Textile.AddYarn(Yarn);
	}
	{
		// Create a new Yarn
		CYarn Yarn;

		// Add some nodes
		Yarn.AddNode(CNode(XYZ(10,0,2)));
		Yarn.AddNode(CNode(XYZ(10,5,0)));
		Yarn.AddNode(CNode(XYZ(10,10,2)));

		// Set the interpolation function
		Yarn.AssignInterpolation(CInterpolationBezier());

		// Assign a cross-section interpolated between the nodes
		CYarnSectionInterpPosition YarnSection;
		YarnSection.AddSection(0, CSectionEllipse(2.2, 1.1));
		YarnSection.AddSection(0.23, CSectionEllipse(1.8, 0.9));
		YarnSection.AddSection(0.67, CSectionEllipse(2.4, 1.3));
		CYarnSectionAdjusted AdjustedYarnSec(YarnSection);
		vector<pair<double, XY> > Circ;
		Circ.push_back(make_pair(0, XY(0.1, 0.2)));
		Circ.push_back(make_pair(0.5, XY(0.3, 0.4)));
		Circ.push_back(make_pair(1.0, XY(0.5, 0.6)));
		AdjustedYarnSec.AddAdjustment(1, 0, Circ);
		Yarn.AssignSection(AdjustedYarnSec);

		// Set the resolution
		Yarn.SetResolution(40);

		// Create some repeats for the yarn
		Yarn.AddRepeat(XYZ(0, 0, 4));

		// Add the yarn to our textile
		Textile.AddYarn(Yarn);
	}

	return Textile;
}

CTextile CTextileFactory::SectionsTest()
{
	CTextile Textile;
	CYarn Yarn;
	Yarn.AddNode(CNode(XYZ(0, 0, 0)));
	Yarn.AddNode(CNode(XYZ(10, 0, 0)));
	Yarn.AssignInterpolation(CInterpolationBezier());
	CYarnSectionInterpPosition YarnSection;
	
	vector<XY> Points;
	Points.push_back(XY(1, 0));
	Points.push_back(XY(1, 0.5));
	Points.push_back(XY(0.5, 1));
	Points.push_back(XY(0, 1));
	YarnSection.AddSection(0, CSectionBezier(Points, true));
	YarnSection.AddSection(0.1, CSectionEllipse(2, 1));
	YarnSection.AddSection(0.2, CSectionLenticular(2, 1));

	Points.push_back(XY(-1, 0));
	Points.push_back(XY(0, -1));
	YarnSection.AddSection(0.3, CSectionPolygon(Points));

	YarnSection.AddSection(0.4, CSectionPowerEllipse(2, 1, 0.5));
	YarnSection.AddSection(0.5, CSectionRotated(CSectionEllipse(2, 1), 0.34));
	YarnSection.AddSection(0.6, CSectionScaled(CSectionEllipse(2, 1), XY(2, 1.5)));
	YarnSection.AddSection(0.7, CSectionHybrid(CSectionEllipse(2, 1), CSectionLenticular(2, 1)));
	YarnSection.AddSection(0.8, CSectionLenticular(2, 1, 0.2));

	Yarn.AssignSection(YarnSection);

	Yarn.SetResolution(40);

	Textile.AddYarn(Yarn);

	return Textile;
}

CTextile CTextileFactory::GetSingleYarn(int iNumMasterNodes, int iResolution)
{
	CTextile Textile;
	CYarn Yarn;
	int i;
	double u;
	for (i=0; i<iNumMasterNodes; ++i)
	{
		u = double(i)/(iNumMasterNodes-1);
		Yarn.AddNode(CNode(XYZ(u, 0, 0)));
	}
	Yarn.AssignSection(CYarnSectionConstant(CSectionEllipse(1, 1)));
	Yarn.AssignInterpolation(CInterpolationBezier());
	Yarn.SetResolution(iResolution);
	Yarn.AddRepeat(XYZ(1, 0, 0));
	Yarn.AddRepeat(XYZ(0, 1, 0));
	Yarn.AddRepeat(XYZ(0, 0, 1));
	CDomainPlanes Domain(XYZ(0.5, 0.5, 0.5), XYZ(1.5, 1.5, 1.5));
	Textile.AssignDomain(Domain);
	Textile.AddYarn(Yarn);
	return Textile;
}

CTextileWeave2D CTextileFactory::MeshedWeave()
{
	CTextileWeave2D Textile(2, 2, 2, 1, false);
	Textile.SwapPosition(0, 0);
	Textile.SwapPosition(1, 1);
	Textile.AssignSectionMesh(CSectionMeshRectangular());
	return Textile;
}

CTextileWeave2D CTextileFactory::CottonWeave()
{
	CTextileWeave2D Textile(2, 2, 0.35, 0.3);

	Textile.SwapPosition(0, 0);
	Textile.SwapPosition(1, 1);

	Textile.SetXYarnWidths(0.3);
	Textile.SetYYarnWidths(0.26);

	Textile.SetXYarnSpacings(0.36);
	Textile.SetYYarnSpacings(0.33);

	Textile.AssignDefaultDomain();

	Textile.SetArealDensity(160, "g/m^2");
//	Textile.SetYarnLinearDensity(0.028, 'g/m');
	Textile.SetFibreDensity(1.54, "g/cm^3");

	return Textile;
}

CTextileWeave2D CTextileFactory::PlainWeave()
{
	CTextileWeave2D Textile(2, 2, 1, 1, false);

	Textile.SwapPosition(0, 0);
	Textile.SwapPosition(1, 1);

	Textile.SetYarnWidths(0.8);
	Textile.SetYarnHeights(0.4);

	Textile.AssignDefaultDomain();

	return Textile;
}

CTextileWeave2D CTextileFactory::PlainWeaveWithGap()
{
	CTextileWeave2D Textile(2, 2, 1, 0.2, true);

	Textile.SetGapSize(0.01);
	Textile.SwapPosition(0, 0);
	Textile.SwapPosition(1, 1);

	Textile.SetYarnWidths(0.8);

	Textile.AssignDefaultDomain();

	return Textile;
}

CTextileLayerToLayer CTextileFactory::LayerToLayerWeave()
{
	CTextileLayerToLayer Textile(2, 4, 1.4, 1, 0.2, 0.1, 2);
	Textile.SetWarpRatio( 1 );
	Textile.SetBinderRatio( 1);

	Textile.SetupLayers( 3, 4, 2);
	Textile.SetGapSize( 0 );
	Textile.SetBinderPosition(0, 1, 0);
	Textile.SetBinderPosition(1, 1, 1);
	Textile.SetBinderPosition(2, 1, 3);
	Textile.SetBinderPosition(3, 1, 2);

	Textile.SetWarpYarnPower( 0.6);
	Textile.SetWeftYarnPower( 0.6);
	Textile.SetWarpYarnWidths(1.2);
	Textile.SetWarpYarnHeights(0.2);
	Textile.SetWarpYarnSpacings(1.4);
	Textile.SetBinderYarnWidths(0.5);
	Textile.SetBinderYarnHeights(0.05);
	Textile.SetBinderYarnSpacings(0.6);

	Textile.SetYYarnWidths(0.8);
	Textile.SetYYarnHeights(0.1);
	Textile.SetYYarnSpacings(1);

	Textile.AssignDefaultDomain();
	Textile.BuildTextile();
	return Textile;
}

CTextile CTextileFactory::StraightYarns()
{
	// Create a textile
	CTextile Textile;
	{
		// Create a yarn
		CYarn Yarn;

		// Add some nodes
		Yarn.AddNode(CNode(XYZ(0, 0, 0)));
		Yarn.AddNode(CNode(XYZ(10, 0, 0)));

		// Assign a constant cross-section all along the yarn
		Yarn.AssignSection(CYarnSectionConstant(CSectionEllipse(1, 1)));

		// Create some repeats for the yarn
		Yarn.AddRepeat(XYZ(10, 0, 0));
		Yarn.AddRepeat(XYZ(0, 10, 0));

		Yarn.SetResolution(2, 100);

		// Volume of this yarn is 2.5 pi mm^3

		Yarn.SetYarnLinearDensity(0.1, "g/mm");

		// Mass is 1 gram

		Yarn.SetFibreDensity(1, "g/mm^3");

		// Assuming Vf of 1, mass is 2.5 pi grams

		// Vf is thus 1/2.5pi = 0.12732

		// Add the yarn to our textile
		Textile.AddYarn(Yarn);
	}
	{
		// Create a yarn
		CYarn Yarn;

		// Add some nodes
		Yarn.AddNode(CNode(XYZ(0, 5, 0)));
		Yarn.AddNode(CNode(XYZ(10, 5, 0)));

		// Assign a constant cross-section all along the yarn
		Yarn.AssignSection(CYarnSectionConstant(CSectionEllipse(1, 1)));

		// Create some repeats for the yarn
//		Yarn.AddRepeat(XYZ(10, 0, 0));
//		Yarn.AddRepeat(XYZ(0, 10, 0));

		Yarn.SetResolution(2, 100);

		// Volume of this yarn is 2.5 pi mm^3

		Yarn.SetYarnLinearDensity(0.2, "kg/mm");

		// Mass is 2 kilograms

		Yarn.SetFibreDensity(1, "kg/mm^3");

		// Assuming Vf of 1, mass is 2.5 pi kilograms

		// Vf is thus 2/2.5pi = 0.25465

		// Add the yarn to our textile
		Textile.AddYarn(Yarn);
	}
	return Textile;
}

CTextileOrthogonal CTextileFactory::OrthogonalWeave()
{
	CTextileOrthogonal Textile( 2, 2, 1.0, 1.0, 0.1, 0.1, false);

	// Set the ratio of warp/binder yarns
	Textile.SetWarpRatio( 1 );
	Textile.SetBinderRatio( 1 );

	// Add yarn layers.  There must always be one NoYarn layer and one Bindery layer
	Textile.AddNoYarnLayer();
	Textile.AddYLayers();
	Textile.AddWarpLayer();
	Textile.AddYLayers();
	Textile.AddBinderLayer();

	// Adjust the yarn widths, heights and spacings
	Textile.SetWarpYarnWidths( 0.8 );

	Textile.SetBinderYarnWidths( 0.4 );
	Textile.SetBinderYarnHeights( 0.05 );
	Textile.SetBinderYarnSpacings( 0.55 );

	// Weft yarns
	Textile.SetYYarnWidths(0.8);  

	// Set the power of the power ellipses used
	Textile.SetWarpYarnPower(0.6);
	Textile.SetWeftYarnPower(0.6);
	Textile.SetBinderYarnPower(0.6);

	Textile.SetResolution(20);

	// Set binder pattern
	Textile.SwapBinderPosition( 0,1);

	// Create a default domain to fit the textile
	Textile.AssignDefaultDomain();

	return Textile;
}








