#include "orientation.h"


// Function to compute the error between the correct local orientation (tangents puma matric) of an artificially generated sample and the estimated one (dirs puma matrix)
// error is between 0 - 90 degrees
// Set error to 100 if NaN inside dirs is found
// Set error to 200 if NaN inside tangents is found
// Note that these voxels are not considered when computing the Mean angular difference and Standard Deviation (SD)
std::pair <double, double> puma::compute_orientationComparison(Workspace *ws, puma::MatVec3<double> *dirs, puma::MatVec3<double> *tangents, puma::Matrix<double> *error, puma::Cutoff cutoff, int numThreads){

    std::pair <double, double> MeanSD(0.,0.);

    // Error Checks
    if(!(ws->X() == dirs->X() && ws->Y() == dirs->Y() && ws->Z() == dirs->Z())){
        std::cout << "Orientation Comparison Error: Dimensions of Workspace and second directions provided (tangents) don't correspond." << std::endl;
        return std::pair<double, double>{-1.,-1.};
    }
    if(!(ws->X() == tangents->X() && ws->Y() == tangents->Y() && ws->Z() == tangents->Z())) {
        std::cout << "Orientation Comparison Error: Dimensions of Workspace and first directions provided (dirs) don't correspond." << std::endl;
        return std::pair <double, double>{-1.,-1.};
    }
    if(!(ws->X() == tangents->X() && ws->Y() == tangents->Y() && ws->Z() == tangents->Z())) {
        std::cout << "Orientation Comparison Error: No solid voxels" << std::endl;
        return std::pair <double, double>{-1.,-1.};
    }
    if(cutoff.first < -0.5 || cutoff.second > 32767 || cutoff.first > cutoff.second){  // maximum signed short value 32767
        std::cout << "Orientation Comparison Error: Invalid Cutoff Ranges" << std::endl;
        return std::pair <double, double>{-1.,-1.};
    }

    // If it passed all the checks
    error->resize(dirs->X(), dirs->Y(), dirs->Z(), NAN);

    // Compute scalar product between unit vectors
    // assigns warnings if inconsistent data is found with error codes noted above
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long i=0;i<dirs->X();i++){
        for(long j=0;j<dirs->Y();j++){
            for(long k=0;k<dirs->Z();k++){
                if(ws->at(i,j,k) >= cutoff.first && ws->at(i,j,k) <= cutoff.second){
                    if(dirs->at(i,j,k).x!=dirs->at(i,j,k).x || dirs->at(i,j,k).y!=dirs->at(i,j,k).y || dirs->at(i,j,k).z!=dirs->at(i,j,k).z) { // checking if any Dir is NaN
                        error->at(i,j,k) = 100.;
                    } else if(tangents->at(i,j,k).x!=tangents->at(i,j,k).x || tangents->at(i,j,k).y!=tangents->at(i,j,k).y || tangents->at(i,j,k).z!=tangents->at(i,j,k).z){  // checking if any tangent is NaN
                        error->at(i,j,k) = 200.;
                    } else if(dirs->at(i,j,k).magnitude() > 1.0001 || dirs->at(i,j,k).magnitude() < 0.9999){  // checking if any tangent is NaN
                        error->at(i,j,k) = 300.;
                    } else if(tangents->at(i,j,k).magnitude() > 1.0001 || tangents->at(i,j,k).magnitude() < 0.9999){  // checking if any tangent is NaN
                        error->at(i,j,k) = 400.;
                    } else {  // if no NaN, then compute the error (smaller angle between unit vectors, computed by calculating the vectors scalar product)
                        double theta = acos(dirs->at(i,j,k).x * tangents->at(i,j,k).x + dirs->at(i,j,k).y * tangents->at(i,j,k).y + dirs->at(i,j,k).z * tangents->at(i,j,k).z) * (180./M_PI);
                        double phi = acos(dirs->at(i,j,k).x * (-tangents->at(i,j,k).x) + dirs->at(i,j,k).y * (-tangents->at(i,j,k).y) + dirs->at(i,j,k).z * (-tangents->at(i,j,k).z)) * (180./M_PI);
                        if(theta>phi){ error->at(i,j,k) = phi; }
                        else { error->at(i,j,k) = theta; }
                    }
                }
            }
        }
    }

    // Counting solid voxels and inconsistencies and summing error over the domain
    int solidCount = 0, totalSolidCount = 0, errorsDirsNANS = 0, errorsTangNANs = 0, errorsDirsMagn = 0, errorsTangMagn = 0;
    for(long i=0;i<dirs->X();i++){
        for(long j=0;j<dirs->Y();j++){
            for(long k=0;k<dirs->Z();k++){
                if(ws->at(i,j,k) >= cutoff.first && ws->at(i,j,k) <= cutoff.second){
                    if (error->at(i,j,k) != error->at(i,j,k)) {
                        errorsTangNANs++;
                    } else if(error->at(i,j,k) < 91.){
                        MeanSD.first += error->at(i,j,k);
                        solidCount++;

                        // Creating Warnings for faulty data
                    } else if (error->at(i,j,k) > 91. && error->at(i,j,k) < 150.){
                        errorsDirsNANS++;
                    } else if (error->at(i,j,k) > 150. && error->at(i,j,k) < 250.) {
                        errorsTangNANs++;
                    } else if (error->at(i,j,k) > 250. && error->at(i,j,k) < 350.){
                        errorsDirsMagn++;
                    } else if (error->at(i,j,k) > 350.) {
                        errorsTangMagn++;
                    }
                    totalSolidCount++;
                }
            }
        }
    }

    // Faulty Data Warnings
    if(errorsDirsNANS){ std::cout << "Warning: Identified " << errorsDirsNANS << " NaNs in first directions provided (dirs) out of " << totalSolidCount <<
                                     " solid voxels (" << ((double)errorsDirsNANS/(double)totalSolidCount)*100. << "%)" << std::endl; }
    if(errorsTangNANs){ std::cout << "Warning: Identified " << errorsTangNANs << " NaNs in second directions provided (tangents) out of " << totalSolidCount <<
                                     " solid voxels (" << ((double)errorsTangNANs/(double)totalSolidCount)*100. << "%)" << std::endl; }
    if(errorsDirsMagn){ std::cout << "Warning: Identified " << errorsDirsMagn << " voxels having magnitude !=1 in first directions provided (dirs) out of " << totalSolidCount <<
                                     " solid voxels (" << ((double)errorsDirsMagn/(double)totalSolidCount)*100. << "%)" << std::endl; }
    if(errorsTangMagn){ std::cout << "Warning: Identified " << errorsTangMagn << " voxels having magnitude !=1 in second directions provided (tangents) out of " << totalSolidCount <<
                                     " solid voxels (" << ((double)errorsTangMagn/(double)totalSolidCount)*100. << "%)" << std::endl; }

    if(solidCount){ // if there are solid voxels
        MeanSD.first = MeanSD.first/(double)solidCount; // Computing Mean angular separation

        for(long i=0;i<dirs->X();i++){
            for(long j=0;j<dirs->Y();j++){
                for(long k=0;k<dirs->Z();k++){
                    if(ws->at(i,j,k) >= cutoff.first && ws->at(i,j,k) <= cutoff.second && error->at(i,j,k) < 91 &&
                       error->at(i, j, k) == error->at(i, j, k)){
                        MeanSD.second += pow(error->at(i,j,k) - MeanSD.first, 2);
                    }
                }
            }
        }
        MeanSD.second = sqrt(MeanSD.second/(double)solidCount); // Computing Standard Deviation (SD)

        std::cout << "Mean: " << MeanSD.first << " SD: " << MeanSD.second << std::endl;
    }

    return MeanSD;
}
