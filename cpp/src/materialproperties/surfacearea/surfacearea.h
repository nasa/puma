#ifndef MATERIALPROPERTYSURF_H
#define MATERIALPROPERTYSURF_H

#include "materialproperty.h"
#include "workspace.h"
#include "triangle.h"
#include "isosurface.h"

#include <numeric>
#include <vector>


namespace puma {

std::pair<double, double> compute_SurfaceAreaMarchingCubes(Workspace *grayWS, puma::Cutoff cutoff, bool interpVerts, int numThreads = 0);
std::pair<double, double> compute_SurfaceAreaVoxels(Workspace *grayWS, puma::Cutoff cutoff, int numThreads = 0);

}

class SurfaceArea : MaterialProperty
{
public:

    SurfaceArea(puma::Workspace *work, puma::Cutoff cutoff, bool interpVerts, int numThreads);
    SurfaceArea(puma::Workspace *work, puma::Cutoff cutoff, int numThreads);

    std::pair<double,double> compute();


private:

    //surfaceArea.first = raw surface area (m^2)
    //surfaceArea.second = specific surface are (1/m)
    std::pair<double,double> surfaceArea;
    double volume{};

    puma::Workspace *work;
    puma::Cutoff cutoff;
    bool interpVerts;
    int numThreads;

    //true is using voxel grid
    //false if using marching cubes
    bool voxel;

    bool surfaceAreaHelper();
    void getVolume(bool smallDomain);
    void getSurfaceArea_Voxel();
    void getSurfaceArea_MarchingCubes();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};



#endif // MATERIALPROPERTYVF_H
