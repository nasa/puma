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

%module(directors="1") Core

%include <stl.i>
%include <std_list.i>
%include <std_vector.i>
%include <std_string.i>
%{
	#include "../Core/PrecompiledHeaders.h"
	#include "../Core/TexGen.h"
	#pragma warning ( disable : 4224 4049 )
	using namespace TexGen;
	namespace TexGen
	{
		CTextile* GetTextile(string TextileName = "") { return TEXGEN.GetTextile(TextileName);}
		bool AddTextile(string TextileName, const CTextile &Textile, bool bOverwrite = false) { return TEXGEN.AddTextile(TextileName, Textile, bOverwrite);}
		string AddTextile(const CTextile &Textile) { return TEXGEN.AddTextile(Textile);}
		bool DeleteTextile(string TextileName) { return TEXGEN.DeleteTextile(TextileName);}
		void SaveToXML(string FileName, string TextileName = "", OUTPUT_TYPE OutputType = OUTPUT_STANDARD) { return TEXGEN.SaveToXML(FileName, TextileName, OutputType);}
		bool ReadFromXML(string FileName) { return TEXGEN.ReadFromXML(FileName);}
		void DeleteTextiles() { return TEXGEN.DeleteTextiles();}
		const map<string, CTextile*> &GetTextiles() { return TEXGEN.GetTextiles();}
	}
%}

// Generate a copy constructor wrapper for all classes
%feature("copyctor");

namespace TexGen {}

using namespace std;
using namespace TexGen;

// Create a director for the selected virtual methods, this enables
// python classes that are derived from C++ classes to override virtual
// methods and have it work as you would hope it to work
%feature("director") CTextileDeformer;
%feature("director") CTextileDeformerVolumeMesh;
//%feature("director") CTextile;

#define CLASS_DECLSPEC
%import "../Core/Singleton.h"
%include "../Core/Plane.h"
%include "../Core/MeshData.h"

/*using namespace TexGen;*/
%template(TexGenSingleton) TexGen::CSingleton<TexGen::CTexGen>;

namespace std
{
	%template(StringVector) vector<string>;
	%template(PlaneVector) vector<TexGen::PLANE>;
	%template(XYZPair) pair<TexGen::XYZ, TexGen::XYZ>;
	%template(YarnList) list<TexGen::CYarn>;
	%template(YarnVector) vector<TexGen::CYarn>;
	%template(YarnVectorPtr) vector<TexGen::CYarn*>;
	%template(TextileVector) vector<TexGen::CTextile*>;		// This template seem to make the map compile without error?!?
	%template(TextileMap) map<string, TexGen::CTextile*>;	// This template causes an error in the c++ code if vector template is not defined?!?
	%template(NodeVector) vector<TexGen::CNode>;
	%template(SlaveNodeVector) vector<TexGen::CSlaveNode>;
	%template(XYZVector) vector<TexGen::XYZ>;
	%template(XYVector) vector<TexGen::XY>;
	%template(DoubleVector) vector<double>;
	%template(IntPair) pair<int, int>;
	%template(IntPairVector) vector<pair<int, int> >;
	%template(PointInfoVector) vector<TexGen::POINT_INFO>;
	%template(DoubleXYZPair) pair<double, TexGen::XYZ>;
	%template(DoubleXYZPairVector) vector< pair<double, TexGen::XYZ> >;
	%template(BoolPair) pair<bool, bool>;
	%template(IntList) list<int>;
	%template(IntVector) vector<int>;
	//%template(ElementTypeMap) map<TexGen::CMesh::ELEMENT_TYPE, list<int> >;  // Only used in unoptimised version of MergeNodes. Causes compilation error with Swig 2
	%template(BoolVector) vector<bool>;
	%template(ElementDataList) list<TexGen::MESHER_ELEMENT_DATA>;
	%template(MeshDataVector) vector<TexGen::CMeshDataBase*>;
	%template(LinearTransformationVector) vector<CLinearTransformation>;
	%template(FloatVector) vector<float>;
	%template(MeshVector) vector<TexGen::CMesh>;
}
%template(XYZMeshData) TexGen::CMeshData<TexGen::XYZ>;

// Wrap all the classes that will be used in the python scripts
// Ignore some of the warnings for the mymath header
%warnfilter(503, 362, 361, 389);
%include "../Core/mymath.h"
%warnfilter(+503, +362, +361, +389);
%include "../Core/Matrix.h"
%include "../Core/LinearTransformation.h"


//%warnfilter(509);
//%include "../Core/units.h"
//%warnfilter(+509);

%include "../Core/Property.h"
%include "../Core/Properties.h"
%include "../Core/PropertiesTextile.h"
%include "../Core/PropertiesYarn.h"

%warnfilter(362);

%include "../Core/Mesh.h"
%include "../Core/BasicVolumes.h"
%include "../Core/Mesher.h"
%include "../Core/Node.h"
%include "../Core/SlaveNode.h"
%include "../Core/TexGen.h"
%include "../Core/Yarn.h"
%warnfilter(401);
%include "../Core/Misc.h"
%warnfilter(+401);

%warnfilter(+362);

%include "../Core/Textile.h"
%include "../Core/TextileWeave.h"
%include "../Core/TextileWeave2D.h"
%include "../Core/TextileWeave3D.h"
%include "../Core/Textile3DWeave.h"
%include "../Core/TextileOrthogonal.h"
%include "../Core/TextileAngleInterlock.h"
%include "../Core/TextileLayerToLayer.h"
%include "../Core/TextileOffsetAngleInterlock.h"
%include "../Core/ShearedTextileWeave2D.h"
%include "../Core/TextileLayered.h"

%include "../Core/TextileDeformer.h"
%include "../Core/TextileDeformerVolumeMesh.h"
%include "../Core/GeometrySolver.h"

%include "../Core/Simulation.h"
%include "../Core/SimulationAbaqus.h"
%include "../Core/AdjustMeshInterference.h"

%include "../Core/Section.h"
%include "../Core/SectionBezier.h"
%include "../Core/SectionEllipse.h"
%include "../Core/SectionHybrid.h"
%include "../Core/SectionLenticular.h"
%include "../Core/SectionPolygon.h"
%include "../Core/SectionPowerEllipse.h"
%include "../Core/SectionRotated.h"
%include "../Core/SectionScaled.h"
%include "../Core/SectionRectangle.h"

%include "../Core/YarnSection.h"
%include "../Core/YarnSectionConstant.h"
%include "../Core/YarnSectionInterp.h"
%include "../Core/YarnSectionInterpNode.h"
%include "../Core/YarnSectionInterpPosition.h"

%include "../Core/SectionMesh.h"
%include "../Core/SectionMeshTriangulate.h"
%include "../Core/SectionMeshRectangular.h"
%include "../Core/SectionMeshRectangleSection.h"

%include "../Core/Interpolation.h"
%include "../Core/InterpolationBezier.h"
%include "../Core/InterpolationCubic.h"
%include "../Core/InterpolationLinear.h"
%include "../Core/InterpolationAdjusted.h"

%include "../Core/Domain.h"
%include "../Core/DomainPlanes.h"

%include "../Core/FibreDistribution.h"
%include "../Core/FibreDistributionConst.h"
%include "../Core/FibreDistribution1DQuad.h"

%include "../Core/VoxelMesh.h"
%include "../Core/RectangularVoxelMesh.h"
%include "../Core/ShearedVoxelMesh.h"
%include "../Core/StaggeredVoxelMesh.h"
%include "../Core/RotatedVoxelMesh.h"
%include "../Core/TetgenMesh.h"
%include "../Core/PeriodicBoundaries.h"
%include "../Core/ShearedPeriodicBoundaries.h"
%include "../Core/StaggeredPeriodicBoundaries.h"
%include "../Core/Materials.h"
#%include "../Core/OctreeVoxelMesh.h"
%include "../Core/ShellElementExport.h"
%include "../Core/SurfaceMesh.h"

%include "../Core/PatternDraft.h"

namespace TexGen
{
	CTextile* GetTextile(string TextileName = "");
	bool AddTextile(string TextileName, const CTextile &Textile, bool bOverwrite = false);
	string AddTextile(const CTextile &Textile);
	bool DeleteTextile(string TextileName);
	void SaveToXML(string FileName, string TextileName = "", OUTPUT_TYPE OutputType = OUTPUT_STANDARD);
	bool ReadFromXML(string FileName);
	void DeleteTextiles();
	const map<string, CTextile*> &GetTextiles();
}

%extend TexGen::WXYZ
{
	char *__repr__()
	{
		static string str;
		ostringstream out;
		out << *self;
		str = out.str();
		return const_cast<char *>(str.c_str());
	}
	WXYZ __add__(const WXYZ &Vector)
	{
		return *self + Vector;
	}
	WXYZ __sub__(const WXYZ &Vector)
	{
		return *self - Vector;
	}
	WXYZ __mul__(const WXYZ &Quat)
	{
		return *self * Quat;
	}
	XYZ __mul__(const XYZ &Vector)
	{
		return *self * Vector;
	}
}

%extend TexGen::XYZ
{
	char *__repr__()
	{
		static string str;
		ostringstream out;
		out << *self;
		str = out.str();
		return const_cast<char *>(str.c_str());
	}
	XYZ __add__(const XYZ &Vector)
	{
		return *self + Vector;
	}
	XYZ __sub__(const XYZ &Vector)
	{
		return *self - Vector;
	}
	XYZ __mul__(const XYZ &Vector)
	{
		return *self * Vector;
	}
	XYZ __mul__(double dScalar)
	{
		return *self * dScalar;
	}
}

%extend TexGen::XY
{
	char *__repr__()
	{
		static string str;
		ostringstream out;
		out << *self;
		str = out.str();
		return const_cast<char *>(str.c_str());
	}
	XY __add__(const XY &Vector)
	{
		return *self + Vector;
	}
	XY __sub__(const XY &Vector)
	{
		return *self - Vector;
	}
	XY __mul__(const XY &Vector)
	{
		return *self * Vector;
	}
	XY __mul__(double dScalar)
	{
		return *self * dScalar;
	}
}

%extend TexGen::CMatrix
{
	char *__repr__()
	{
		static string str;
		ostringstream out;
		out << *self;
		str = out.str();
		return const_cast<char *>(str.c_str());
	}
	CMatrix __add__(const CMatrix &Matrix)
	{
		return *self + Matrix;
	}
	CMatrix __sub__(const CMatrix &Matrix)
	{
		return *self - Matrix;
	}
	CMatrix __mul__(const CMatrix &Matrix)
	{
		return *self * Matrix;
	}
	CMatrix __mul__(double dScalar)
	{
		return *self * dScalar;
	}
}

%extend TexGen::CLinearTransformation
{
	char *__repr__()
	{
		static string str;
		ostringstream out;
		out << *self;
		str = out.str();
		return const_cast<char *>(str.c_str());
	}
	CLinearTransformation __add__(const CLinearTransformation &Matrix)
	{
		return *self + Matrix;
	}
	CLinearTransformation __sub__(const CLinearTransformation &Matrix)
	{
		return *self - Matrix;
	}
	CLinearTransformation __mul__(const CLinearTransformation &Matrix)
	{
		return *self * Matrix;
	}
	CLinearTransformation __mul__(double dScalar)
	{
		return *self * dScalar;
	}
}






