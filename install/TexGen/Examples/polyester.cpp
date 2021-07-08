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
	CSectionEllipse Section(0.18, 0.04);
	Section.AssignSectionMesh(CSectionMeshTriangulate(20));

	// Create 4 yarns
	std::vector<CYarn> Yarns(4); 

	// Add nodes to the yarns to describe their paths
	Yarns[0].AddNode(CNode(XYZ(0, 0, 0)));
	Yarns[0].AddNode(CNode(XYZ(0.22, 0, 0.05)));
	Yarns[0].AddNode(CNode(XYZ(0.44, 0, 0)));

	Yarns[1].AddNode(CNode(XYZ(0, 0.22, 0.05)));
	Yarns[1].AddNode(CNode(XYZ(0.22, 0.22, 0)));
	Yarns[1].AddNode(CNode(XYZ(0.44, 0.22, 0.05)));

	Yarns[2].AddNode(CNode(XYZ(0, 0, 0.05)));
	Yarns[2].AddNode(CNode(XYZ(0, 0.22, 0)));
	Yarns[2].AddNode(CNode(XYZ(0, 0.44, 0.05)));

	Yarns[3].AddNode(CNode(XYZ(0.22, 0, 0)));
	Yarns[3].AddNode(CNode(XYZ(0.22, 0.22, 0.05)));
	Yarns[3].AddNode(CNode(XYZ(0.22, 0.44, 0)));

	// We want the same interpolation and section shape for all the yarns so loop over them all
	//for Yarn in Yarns:
	for(unsigned int i=0;i<Yarns.size();++i){
		// Set the interpolation function
		Yarns[i].AssignInterpolation(CInterpolationCubic());
		// Assign a constant cross-section all along the yarn
		Yarns[i].AssignSection(CYarnSectionConstant(Section));
		// Set the resolution
		Yarns[i].SetResolution(20);
		// Add repeats to the yarn
		Yarns[i].AddRepeat(XYZ(0.44, 0, 0));
		Yarns[i].AddRepeat(XYZ(0, 0.44, 0));
		// Add the yarn to our textile
		Textile.AddYarn(Yarns[i]);
	} 

	// Create a domain
	CDomainPlanes domain(XYZ(0, 0, -1), XYZ(0.44, 0.44, 1));
	Textile.AssignDomain(domain);

	// Add the textile to our TexGen singleton with the name "polyester"
	TEXGEN.AddTextile("polyester", Textile);

	TEXGEN.SaveToXML("polyester.tg3");

#ifdef USE_RENDERER
	// Get an instance of the CTexGenRenderer to render the textile
	CTexGenRenderer Renderer;

	// Render our textile "polyester"
	Renderer.RenderTextile("polyester");

	// Start rendering
	Renderer.Start();
#endif 
	return 0;
};

