class TopoDS_Shape;
class TopTools_HSequenceOfShape;
class TopoDS_Wire;

#include "../Core/PrecompiledHeaders.h"
#include "../Core/TexGen.h"

using namespace std;

namespace TexGen
{
	class CTextile;	// This makes sure that swig knows CTextile is part of the TexGen namespace...
	/// Class to re-create geometry in OpenCASCADE and in turn export to IGES or STEP
	class CExporter
	{
	public:
		CExporter();
		~CExporter();

		/// Output the domain to IGES file format
		bool OutputDomainToIGES(char szFileName[], CDomain &Domain);
		/// Output the domain to STEP file format
		bool OutputDomainToSTEP(char szFileName[], CDomain &Domain);

		/// Output the textile to IGES file format
		bool OutputTextileToIGES(char szFileName[], CTextile &Textile);
		/// Output the textile to STEP file format
		bool OutputTextileToSTEP(char szFileName[], CTextile &Textile);

		/// Output the textile to IGES file format
		bool OutputTextileToIGES(char szFileName[], string TextileName);
		/// Output the textile to STEP file format
		bool OutputTextileToSTEP(char szFileName[], string TextileName);

		bool GetFaceted() { return m_bFaceted; }
		bool GetExportDomain() { return m_bExportDomain; }
		bool GetSubtractYarns() { return m_bSubtractYarns; }

		void SetFaceted(bool bFaceted) { m_bFaceted = bFaceted; }
		void SetExportDomain(bool bExportDomain) { m_bExportDomain = bExportDomain; }
		void SetSubtractYarns(bool bSubtractYarns) { m_bSubtractYarns = bSubtractYarns; }
		void SetJoinYarns(bool bJoinYarns) { m_bJoinYarns = bJoinYarns; }

	protected:
		/// Copy the shapes to given range with given repeat vector
		/**
		for i in range lowerlimit - upperlimit
		   All the shapes are copied and translated by i * Vector
        Shapes are then replaced with the newly generated shapes
		*/
		static void CopyShapesToRange(TopTools_HSequenceOfShape &Shapes, XYZ Vector, int iLowerLimit, int iUpperLimit);
		/// Convert textile to a sequence of OpenCascade TopoDS_Shape
		void ConvertTextile(TopTools_HSequenceOfShape &Shapes, CTextile &Textile);
		/// Convert yarn to a sequence of OpenCascade TopoDS_Shape
		void ConvertYarn(TopTools_HSequenceOfShape &Shapes, CYarn &Yarn, const CDomain *pDomain = NULL);
		/// Convert a yarn section to a OpenCascade wire
		TopoDS_Wire ConvertSection(const vector<XYZ> &SectionPoints);
		/// Convert domain to a OpenCascade TopoDS_Shape
		TopoDS_Shape ConvertDomain(const CDomain &Domain);
		/// Subtract yarns from the domain (NOTE: THIS IS UNSTABLE AND VERY SLOW)
		void SubtractYarnsFromDomain(TopoDS_Shape &Domain, const TopTools_HSequenceOfShape &Yarns);
		/// Trim the yarns such they fit within the domain
		bool ConfineYarnsToDomain(TopTools_HSequenceOfShape &Yarns, const TopoDS_Shape &Domain);
		/// Save sequence of shapes to IGES file
		bool SaveToIGES(char szFileName[], TopTools_HSequenceOfShape &Shapes);
		/// Save sequence of shapes to STEP file
		bool SaveToSTEP(char szFileName[], TopTools_HSequenceOfShape &Shapes);

		bool m_bFaceted;
		bool m_bExportDomain;
		bool m_bSubtractYarns;
		bool m_bJoinYarns;
	};
}


