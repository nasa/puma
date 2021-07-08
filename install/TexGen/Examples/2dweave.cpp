#include "../Core/PrecompiledHeaders.h"
#include "../Core/TexGen.h"
#ifdef USE_RENDERER
#include "../Renderer/PrecompiledHeaders.h"
#include "../Renderer/TexGenRenderer.h"
#endif

using namespace TexGen;
int main()
{
	CTextileWeave2D Textile(4, 4, 5, 2);
	Textile.SwapPosition(3, 0);
	Textile.SwapPosition(2, 1);
	Textile.SwapPosition(1, 2);
	Textile.SwapPosition(0, 3);

	Textile.SetYarnWidths(4);
	Textile.SetYarnHeights(0.8);
	
	TEXGEN.AddTextile(Textile);
	
	CDomainPlanes Domain(XYZ(0, 0, 0), XYZ(30, 30, 2));
	
	// Create a domain and assign it to the textile
	Textile.AssignDomain(Domain);
	
#ifdef USE_RENDERER
	// Get an instance of the CTexGenRenderer to render the textile
	CTexGenRenderer Renderer;
	// Render our textile clipped to the domain
	Renderer.RenderTextile();
	Renderer.Start();
#endif
	return 0;
};
