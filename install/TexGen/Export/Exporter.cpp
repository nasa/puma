#include "Exporter.h"

#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>

#include <TopoDS_Wire.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TColgp_HArray1OfPnt.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GeomAPI_Interpolate.hxx>

#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_Copy.hxx>

#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <Interface_Static.hxx>
#include <STEPControl_Writer.hxx>
#include <STEPControl_StepModelType.hxx>

using namespace TexGen;

CExporter::CExporter()
: m_bFaceted(false)
, m_bSubtractYarns(false)
, m_bExportDomain(true)
{
//	Interface_Static::SetIVal("write.step.assembly",1);
}

CExporter::~CExporter()
{
}

bool CExporter::OutputDomainToIGES(char szFileName[], CDomain &Domain)
{
	TGLOGINDENT("Saving Domain to IGES file: " << szFileName);
	TopTools_HSequenceOfShape Shapes;
	Shapes.Append(ConvertDomain(Domain));
	return SaveToIGES(szFileName, Shapes);
}

bool CExporter::OutputDomainToSTEP(char szFileName[], CDomain &Domain)
{
	TGLOGINDENT("Saving Domain to STEP file: " << szFileName);
	TopTools_HSequenceOfShape Shapes;
	Shapes.Append(ConvertDomain(Domain));
	return SaveToSTEP(szFileName, Shapes);
}

bool CExporter::OutputTextileToIGES(char szFileName[], CTextile &Textile)
{
	TGLOGINDENT("Saving Textile to IGES file: " << szFileName);
	TopTools_HSequenceOfShape Shapes;
	ConvertTextile(Shapes, Textile);
	return SaveToIGES(szFileName, Shapes);
}

bool CExporter::OutputTextileToSTEP(char szFileName[], CTextile &Textile)
{
	TGLOGINDENT("Saving Textile to STEP file: " << szFileName);
	TopTools_HSequenceOfShape Shapes;
	ConvertTextile(Shapes, Textile);
	return SaveToSTEP(szFileName, Shapes);
}

bool CExporter::OutputTextileToIGES(char szFileName[], string TextileName)
{
	CTextile *pTextile = CTexGen::GetInstance().GetTextile(TextileName);
	if (!pTextile)
		return false;

	return OutputTextileToIGES(szFileName, *pTextile);
}

bool CExporter::OutputTextileToSTEP(char szFileName[], string TextileName)
{
	CTextile *pTextile = CTexGen::GetInstance().GetTextile(TextileName);
	if (!pTextile)
		return false;

	return OutputTextileToSTEP(szFileName, *pTextile);
}

bool CExporter::SaveToSTEP(char szFileName[], TopTools_HSequenceOfShape &Shapes)
{
	STEPControl_Writer aWriter;

	IFSelect_ReturnStatus status;
	for (Standard_Integer i=1; i<=Shapes.Length(); i++)  
	{
		status = aWriter.Transfer(Shapes.Value(i), STEPControl_AsIs);
		if (status != IFSelect_RetDone)
		{
			return false;
		}
	}
	status = aWriter.Write(szFileName);
	return (status == IFSelect_RetDone);
}

bool CExporter::SaveToIGES(char szFileName[], TopTools_HSequenceOfShape &Shapes)
{
	IGESControl_Controller::Init();
	//IGESControl_Writer ICW(Interface_Static::CVal("XSTEP.iges.unit"), 1);  // Value 1 for 2nd parameter would give BRep write mode - which should it be??
	IGESControl_Writer ICW(Interface_Static::CVal("XSTEP.iges.unit"),
				Interface_Static::IVal("XSTEP.iges.writebrep.mode"));

	for (Standard_Integer i=1;i<=Shapes.Length();i++)
	{
		ICW.AddShape(Shapes.Value(i));
	}

	ICW.ComputeModel();
	Standard_Boolean result = ICW.Write(szFileName);
	return result?true:false;
}

void CExporter::ConvertTextile(TopTools_HSequenceOfShape &Shapes, CTextile &Textile)
{
	Shapes.Clear();

	TGLOGINDENT("Converting Textile to Open CASCADE format");
	int i, j;
	for (i=0; i<(int)Textile.GetYarns().size(); ++i)
	{
		TopTools_HSequenceOfShape Yarns;
		
		ConvertYarn(Yarns, Textile.GetYarns()[i], Textile.GetDomain());
		for (j=1; j<=Yarns.Length(); ++j)
		{
			Shapes.Append(Yarns.Value(j));
		}
	}
	if (Textile.GetDomain())
	{
		bool bConfineSuccess;
		TopoDS_Shape Domain = ConvertDomain(*Textile.GetDomain());
		bConfineSuccess = ConfineYarnsToDomain(Shapes, Domain);
		if (m_bExportDomain)
		{
			if (m_bSubtractYarns)
			{
				if ( bConfineSuccess )
					SubtractYarnsFromDomain(Domain, Shapes);
				else
					TGERROR("Cannot subtract yarn from domain: errors confining yarns");
			}
			Shapes.Append(Domain);
		}
	}
}

bool CExporter::ConfineYarnsToDomain(TopTools_HSequenceOfShape &Yarns, const TopoDS_Shape &Domain)
{
	TGLOGINDENT("Confining yarns to the domain");
	int i;
	bool bSuccess = true;

	for (i=1; i<=Yarns.Length(); ++i)
	{
		TGLOG("Confining yarn: " << i << "/" << Yarns.Length());
		try
		{
			Yarns.SetValue(i, BRepAlgoAPI_Common(Yarns.Value(i), Domain));
		}
		catch(...)
		{
			TGERROR( "Failed to confine yarn " << i << " to domain" );
			bSuccess = false;
		}
	}
	return bSuccess;
}

void CExporter::SubtractYarnsFromDomain(TopoDS_Shape &Domain, const TopTools_HSequenceOfShape &Yarns)
{
	TGLOGINDENT("Cutting yarns out of the domain");
	int i;
	for (i=1; i<=Yarns.Length(); ++i)
	{
		TGLOG("Cutting domain with yarn: " << i << "/" << Yarns.Length());
		Domain = BRepAlgoAPI_Cut(Domain, Yarns.Value(i));
	}
}

void CExporter::ConvertYarn(TopTools_HSequenceOfShape &Shapes, CYarn &Yarn, const CDomain *pDomain)
{
	Shapes.Clear();
	TopTools_HSequenceOfShape YarnShapes;

	TGLOG("Converting Yarn to Open CASCADE format");

	vector<CSlaveNode>::const_iterator itSlaveNode;
	const vector<CSlaveNode> &SlaveNodes = Yarn.GetSlaveNodes(CYarn::SURFACE);

	BRepOffsetAPI_ThruSections SectionsSolid(true, m_bFaceted);
	for (itSlaveNode = SlaveNodes.begin(); itSlaveNode != SlaveNodes.end(); ++itSlaveNode)
	{
		SectionsSolid.AddWire(ConvertSection(itSlaveNode->GetSectionPoints()));
	}
	TopoDS_Shape Shape = SectionsSolid.Shape();

	YarnShapes.Append(Shape);

	if (pDomain)
	{
		TGLOGINDENT("Repeating Yarn to fully encompass domain");
		vector<pair<int, int> > RepeatLimits = pDomain->GetRepeatLimits(Yarn);
		int i;
		for (i = 0; i < (int)RepeatLimits.size(); ++i)
		{
			CopyShapesToRange(YarnShapes, Yarn.GetRepeats()[i], RepeatLimits[i].first, RepeatLimits[i].second);
		}
	}

	if ( m_bJoinYarns )
	{
		TGLOG("Fusing yarn sections");
		Shapes.Append( YarnShapes.Value(1) );
		
		for ( int i = 2; i <= YarnShapes.Length(); )
		{
			BRepAlgoAPI_Fuse Fused( Shapes.Value(1), YarnShapes.Value(i) );
			if ( !Fused.ErrorStatus() )  // Error status = 0 if ok
			{
				Shapes.SetValue( 1, Fused.Shape() );
				YarnShapes.Remove(i);
			}
			else
			{
				++i;
			}
		}
	}
	else
		Shapes = YarnShapes;
}

TopoDS_Wire CExporter::ConvertSection(const vector<XYZ> &SectionPoints)
{
	if (m_bFaceted)
	{
		BRepBuilderAPI_MakePolygon PolygonalWire;
		vector<XYZ>::const_iterator itSectionPoint;
		for (itSectionPoint = SectionPoints.begin(); itSectionPoint != SectionPoints.end(); ++itSectionPoint)
		{
			gp_Pnt Point(itSectionPoint->x, itSectionPoint->y, itSectionPoint->z);
			PolygonalWire.Add(Point);
		}
		PolygonalWire.Close();
		return PolygonalWire.Wire();
	}
	else
	{
		vector<int> Discontinuities;
		const double dMaxAngle = 90*(PI/180.0);
		double dAngle;
		int i, j, iPrev, iNext;
		XYZ V1, V2, TanV;
		// Determine where discontinuities occur in the cross section occur (typically this will be at the edges
		// of the yarn). Check the angle between three consecutive points, if the angle is greater than the set
		// MaxAngle then define that middle point as a discontinuity.
		for (i=0; i<(int)SectionPoints.size(); ++i)
		{
			iNext = (i+1)%SectionPoints.size();
			if (i==0)
				iPrev = SectionPoints.size()-1;
			else
				iPrev = (i-1)%SectionPoints.size();
			V1 = SectionPoints[iNext] - SectionPoints[i];
			V2 = SectionPoints[i] - SectionPoints[iPrev];
			Normalise(V1);
			Normalise(V2);
			// Create a vector with the average of the first and last sections to specify the tangent for the spline
			if ( i == 0 )
				TanV = (V1 + V2)/2;
			dAngle = acos(DotProduct(V1, V2));
			if (dAngle > dMaxAngle)
			{
				Discontinuities.push_back(i);
			}
		}
		// If there are no discontinuities then create a closed periodic loop
		if (Discontinuities.empty())
		{
			Handle(TColgp_HArray1OfPnt) pPoints = new TColgp_HArray1OfPnt(1, SectionPoints.size()+1);
			for (i=0; i<(int)SectionPoints.size(); ++i)
			{
				pPoints->SetValue(i+1, gp_Pnt(SectionPoints[i].x, SectionPoints[i].y, SectionPoints[i].z));
			}
			pPoints->SetValue(i+1, gp_Pnt(SectionPoints[0].x, SectionPoints[0].y, SectionPoints[0].z));
			GeomAPI_Interpolate Interpolation(pPoints, Standard_False, 0.001); 

			// Set up the tangent for the start and end sections
			gp_Vec Tangent = gp_Vec(TanV.x, TanV.y, TanV.z);
			Interpolation.Load(Tangent,Tangent, Standard_False);

			Interpolation.Perform();
			BRepBuilderAPI_MakeEdge Edge(Interpolation.Curve());
			BRepBuilderAPI_MakeWire Wire(Edge.Edge());
			return Wire.Wire();
		}
		else
		{
			// Otherwise the wire will be built up of several sub-wires, 1 wire per discontinuity
			int iStart, iEnd, iRange;
			TopoDS_Wire CrossSectionWire;
			for (j=0; j<(int)Discontinuities.size(); ++j)
			{
				// Find the start and end point of the sub-wire
				iStart = Discontinuities[j];
				iEnd = Discontinuities[(j+1)%Discontinuities.size()];
				if (iEnd <= iStart)
					iEnd += SectionPoints.size();
				iRange = iEnd - iStart + 1;
				// Create the sub-wire
				Handle(TColgp_HArray1OfPnt) pPoints = new TColgp_HArray1OfPnt(1, iRange);
				for (i=iStart; i<=iEnd; ++i)
				{
					XYZ Pt = SectionPoints[i%SectionPoints.size()];
					pPoints->SetValue(i-iStart+1, gp_Pnt(Pt.x, Pt.y, Pt.z));
				}
				GeomAPI_Interpolate Interpolation(pPoints, Standard_False, 0.001);  
				Interpolation.Perform();
				BRepBuilderAPI_MakeEdge Edge(Interpolation.Curve());
				// Join the sub-wire with the rest of the CrossSectionWire
				if (j==0)
				{
					BRepBuilderAPI_MakeWire Wire(Edge.Edge());
					CrossSectionWire = Wire.Wire();
				}
				else
				{
					BRepBuilderAPI_MakeWire Wire(CrossSectionWire, Edge.Edge());
					CrossSectionWire = Wire.Wire();
				}
			}
			return CrossSectionWire;
		}
	}
}

void CExporter::CopyShapesToRange(TopTools_HSequenceOfShape &Shapes, XYZ Vector, int iLowerLimit, int iUpperLimit)
{
	int i, j;
	XYZ Translation;
	gp_Trsf Transform;
	TopTools_HSequenceOfShape NewShapes;
	for (i=iLowerLimit; i<=iUpperLimit; ++i)
	{
		Translation = i*Vector;
		Transform.SetTranslation(gp_Vec(Translation.x, Translation.y, Translation.z));
		for (j=1; j<=Shapes.Length(); ++j)
		{
			BRepBuilderAPI_Transform Translated(Shapes.Value(j), Transform, Standard_True);
			NewShapes.Append(Translated.Shape());
		}
	}
	Shapes = NewShapes;
}

TopoDS_Shape CExporter::ConvertDomain(const CDomain &Domain)
{
	TGLOGINDENT("Converting Domain to Open CASCADE format");
	if (Domain.GetType() == "CDomainPlanes")
	{
		CDomainPlanes &DomainPlanes = *((CDomainPlanes*)&Domain);
		// If the domain is a box then just make a box with MakeBox and return it
		XYZ Min, Max;
		if (DomainPlanes.GetBoxLimits(Min, Max))
		{
			return BRepPrimAPI_MakeBox(gp_Pnt(Min.x, Min.y, Min.z), gp_Pnt(Max.x, Max.y, Max.z));
		}

		// Otherwise make a AABB and then cut it up using the planes that define the boundary
		Min = DomainPlanes.GetMesh().GetAABB().first;
		Max = DomainPlanes.GetMesh().GetAABB().second;
		Min -= XYZ(1, 1, 1);
		Max += XYZ(1, 1, 1);
		TopoDS_Shape DomainShape = BRepPrimAPI_MakeBox(gp_Pnt(Min.x, Min.y, Min.z), gp_Pnt(Max.x, Max.y, Max.z));
		vector<PLANE>::const_iterator itPlane;
		for (itPlane = DomainPlanes.GetPlanes().begin(); itPlane != DomainPlanes.GetPlanes().end(); ++itPlane)
		{
			gp_Pln Plane(itPlane->Normal.x, itPlane->Normal.y, itPlane->Normal.z, -itPlane->d);
			BRepBuilderAPI_MakeFace Face(Plane);
			XYZ RefPoint = (itPlane->d+1)*itPlane->Normal;
			BRepPrimAPI_MakeHalfSpace HalfSpace(Face.Face(), gp_Pnt(RefPoint.x, RefPoint.y, RefPoint.z));
			DomainShape = BRepAlgoAPI_Common(HalfSpace.Solid(), DomainShape);
		}
		return DomainShape;
	}
	return TopoDS_Shape();
}


