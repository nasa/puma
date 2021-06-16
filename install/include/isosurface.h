#ifndef PUMA_ISOSURFACE_H
#define PUMA_ISOSURFACE_H

#include "workspace.h"
#include "triangle.h"
#include "MarchingCubes.h"
#include "isosurfacehelper.h"

#include <vector>


namespace puma {
bool isosurface_MarchingCubes(std::vector< puma::Triangle<float> > *triangles, Workspace *work, puma::Cutoff cutoff, bool closedEdges, int factor, bool interpolateVerts, int numThreads = 0);
bool isosurface_LewinerMarchingCubes(std::vector< puma::Triangle<float> > *triangles, Workspace *work, puma::Cutoff cutoff, bool closedEdges, int factor, bool interpolateVerts, int numThreads = 0);
}

class IsoSurface {
public:
    IsoSurface(std::vector< puma::Triangle<float> > *triangles, puma::Workspace *work, puma::Cutoff cutoff, bool closedEdges, int factor, bool interpolateVerts, bool lewiner, int numThreads);
    bool compute();
private:
    std::vector< puma::Triangle<float> > *triangles;
    puma::Workspace *work;
    puma::Cutoff cutoff;
    bool closedEdges;
    int factor;
    bool interpolateVerts;
    int numThreads;
    bool lewiner;

    bool copyGrayscaleMatrix(puma::Matrix<float> *newMatrix);
    bool downscale(puma::Matrix<float> *Matrix);
    double manipulateGrayscaleMatrix(puma::Matrix<float> *newMatrix);
    bool manipulateSegmentedMatrix(puma::Matrix<short> *newMatrix);
    bool downscale(puma::Matrix<short> *Matrix);
    double determineIsoVal();

    bool errorCheck(std::string *errorMessage);
};


#endif // PUMA_ISOSURFACE_H
