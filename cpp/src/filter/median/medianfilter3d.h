#ifndef MedianFilter3D_H
#define MedianFilter3D_H

#include "filter.h"
#include "workspace.h"


namespace puma {

/*! Executes a median filter on a grayscale workspace
\param work: the workspace to filter
\param window_radius: the voxel radius in 3d space. typically 1-5
\param numThreads: the number of threads used in the operation. 0 for maximum number of threads
\return true for success or false for fail
*/
bool filter_Median3D(puma::Workspace *work, int window_radius, int numThreads = 0);
}

class MedianFilter3D : Filter
{
public:

    MedianFilter3D(puma::Workspace *work, int window_radius, int numThreads = 0);

    bool execute() override;


private:

    puma::Workspace *work;
    int window_radius;
    int numThreads;

    //functions for the quicksort algorithm. Needed for getMedian
    void quicksort(short *arIntegers, int left, int right);
    short median3(short *arIntegers,int left,int right);
    short partition(short *arIntegers, int left, int right, int pivot);
    void swap(short &val1, short &val2);

    bool filterHelper();

    short getMedian(short* daArray, int iSize);
    short medianValue(long x, long y, long z);

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


#endif // MedianFilter3D_H
