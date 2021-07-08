// ProfileTests.cpp : Defines the entry point for the console application.
//
#include "../Core/PrecompiledHeaders.h"
#include "../Renderer/PrecompiledHeaders.h"
#include "../Core/TexGen.h"
#include "../Core/RectangularVoxelMesh.h"
#include "../Renderer/TexGenRenderer.h"

#include "TextileFactory.h"

//#define SHINY_PROFILER TRUE

using namespace TexGen;

//PROFILE_SHARED_EXTERN( ProfileTest )

//int _tmain(int argc, _TCHAR* argv[])
int main( int argc, char** argv)
{
	//PROFILE_SHARED_DEFINE( ProfileTest)
	//PROFILE_FUNC();

	CTextileFactory TextileFactory;
	
	// Test voxel output for plain weave
	CTextileWeave2D Textile = TextileFactory.PlainWeave(13,13);
	CRectangularVoxelMesh Mesh("CPeriodicBoundaries");
	Mesh.SaveVoxelMesh(Textile, "ProfileVoxels", 100, 100, 30, true, true, MATERIAL_CONTINUUM);

	// Test octree smoothed output for single yarn
	//CTextile Textile = TextileFactory.GetSingleYarn(2, 20);
	//COctreeVoxelMesh  Mesh;
	//Mesh.SaveVoxelMesh(Textile, "Straight", 3, 5, true, 10, 0.3, 0.3, true, false);
	
	TEXGEN.AddTextile(Textile);
	CTexGenRenderer* Renderer = new CTexGenRenderer;
	Renderer->RenderTextile(Textile);
	Renderer->Start();
	//PROFILE_END();
    //PROFILER_UPDATE();
    //PROFILER_OUTPUT("ProfileOutput.txt");
	//TEXGEN.SaveToXML("ProfilePlainWeave.tg3");
	return 0;
}

