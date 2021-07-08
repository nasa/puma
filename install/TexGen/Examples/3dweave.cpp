#include "../Core/PrecompiledHeaders.h"
#include "../Core/TexGen.h"
#ifdef USE_RENDERER
#include "../Renderer/PrecompiledHeaders.h"
#include "../Renderer/TexGenRenderer.h"
#endif

using namespace TexGen;

int main()
{

	CTextileWeave3D Textile(8, 4, 5, 7);

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

	//# Add the textile to our TexGen singleton
	TEXGEN.AddTextile(Textile);

	//# Create a domain and add it our TexGen singleton
	Textile.AssignDefaultDomain();

#ifdef USE_RENDERER
	//# Get and instance of the CTexGenRenderer to render the textile
	CTexGenRenderer Renderer;

	//# Render our textile
	Renderer.RenderTextile();

//    # Start rendering
	Renderer.Start();
#endif
	return 0;
};
