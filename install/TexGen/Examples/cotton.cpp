#include "../Core/PrecompiledHeaders.h"
#include "../Core/TexGen.h"
#ifdef USE_RENDERER
#include "../Renderer/PrecompiledHeaders.h"
#include "../Renderer/TexGenRenderer.h"
#endif 
using namespace TexGen;

int main()
{
	// Create a textile
	CTextile Textile;

	// Create a lenticular section and add it to our TexGen singleton

	CSectionLenticular Section(0.3, 0.14);
	Section.AssignSectionMesh(CSectionMeshTriangulate(30));

	// Create 4 yarns
	std::vector<CYarn> Yarns(4);		

	// Add nodes to the yarns to describe their paths
	Yarns[0].AddNode(CNode(XYZ(0, 0, 0)));
	Yarns[0].AddNode(CNode(XYZ(0.35, 0, 0.15)));
	Yarns[0].AddNode(CNode(XYZ(0.7, 0, 0)));

	Yarns[1].AddNode(CNode(XYZ(0, 0.35, 0.15)));
	Yarns[1].AddNode(CNode(XYZ(0.35, 0.35, 0)));
	Yarns[1].AddNode(CNode(XYZ(0.7, 0.35, 0.15)));

	Yarns[2].AddNode(CNode(XYZ(0, 0, 0.15)));
	Yarns[2].AddNode(CNode(XYZ(0, 0.35, 0)));
	Yarns[2].AddNode(CNode(XYZ(0, 0.7, 0.15)));

	Yarns[3].AddNode(CNode(XYZ(0.35, 0, 0)));
	Yarns[3].AddNode(CNode(XYZ(0.35, 0.35, 0.15)));
	Yarns[3].AddNode(CNode(XYZ(0.35, 0.7, 0)));

	// We want the same interpolation and section shape for all the yarns so loop over them all
	unsigned int i;
	for(i=0; i<Yarns.size(); ++i)
	{		
		//     Set the interpolation function
		Yarns[i].AssignInterpolation(CInterpolationCubic());
		//     Assign a constant cross-section all along the yarn
		Yarns[i].AssignSection(CYarnSectionConstant(Section));
		//     Set the resolution
		Yarns[i].SetResolution(8);
		//     Add repeats to the yarn
		Yarns[i].AddRepeat(XYZ(0.7, 0, 0));
		Yarns[i].AddRepeat(XYZ(0, 0.7, 0));
		//     Add the yarn to our textile
		Textile.AddYarn(Yarns[i]);
	} 

	CDomainPlanes dmpl(XYZ(0, 0, -1), XYZ(0.7, 0.7, 1));
	// Create a domain and add it our Textile
	Textile.AssignDomain(dmpl);

	// Add the textile to our TexGen singleton with the name "cotton"
	TEXGEN.AddTextile("cotton", Textile);

	TEXGEN.SaveToXML("cotton.tg3");

#ifdef USE_RENDERER
	// Get an instance of the CTexGenRenderer to render the textile
	CTexGenRenderer Renderer;

	// Render our textile "cotton"
	Renderer.RenderTextile("cotton");

	// Start rendering
	Renderer.Start();
#endif

	return 0;
};





