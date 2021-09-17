#ifndef ORIENTATION_H
#define ORIENTATION_H

#include "workspace.h"
#include "vector.h"
#include "matrix.h"


namespace puma {

/*! Function to compute the error between the correct local orientation (tangents puma matric) of an artificially generated sample and the estimated one (dirs puma matrix).
Error is between 0 - 90 degrees.
Set error to 100 if NaN inside dirs is found.
Set error to 200 if NaN inside tangents is found.
Note that these voxels are not considered when computing the Mean angular difference and Standard Deviation (SD)
\param ws the input workspace to serve as a mask according to the cutoff
\param dirs first orientation field
\param tangents second orientation field
\param error matrix containing the angular difference (in degrees)
\param cutoff the grayscale range to consider, according to the input ws
\param numThreads number of threads to use
\return pair containing the mean (first) and std (second)
*/
std::pair <double, double> compute_orientationComparison(Workspace *ws, puma::MatVec3<double> *dirs, puma::MatVec3<double> *tangents, puma::Matrix<double> *error, puma::Cutoff cutoff, int numThreads = 0);

}


#endif // ORIENTATION_H
