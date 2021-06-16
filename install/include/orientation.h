#ifndef ORIENTATION_H
#define ORIENTATION_H

#include "workspace.h"
#include "vector.h"
#include "matrix.h"


namespace puma {

std::pair <double, double> compute_orientationComparison(Workspace *ws, puma::MatVec3<double> *dirs, puma::MatVec3<double> *tangents, puma::Matrix<double> *error, puma::Cutoff cutoff, int numThreads = 0);

}


#endif // ORIENTATION_H
