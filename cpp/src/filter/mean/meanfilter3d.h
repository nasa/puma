#ifndef MeanFilter3D_H
#define MeanFilter3D_H

#include "filter.h"
#include "workspace.h"


namespace puma {

/*! Executes a mean filter on a workspace
\param work: the workspace to filter
\param window_radius: the voxel radius in 3d space. typically 1-5
\param numThreads: the number of threads used in the operation. 0 for maximum number of threads
\return true for success or false for fail 
*/
bool filter_Mean3D(Workspace *work, int window_radius, int numThreads = 0);
}

class MeanFilter3D : Filter
{
public:

    MeanFilter3D(puma::Workspace *work, int window_radius, int numThreads = 0);

    bool execute() override;


private:

    puma::Workspace *work;
    int window_radius;
    int numThreads;

    bool filterHelper();
    short meanValue(long x, long y, long z);

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


#endif // MeanFilter3D_H
