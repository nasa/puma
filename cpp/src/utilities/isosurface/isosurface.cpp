#include "isosurface.h"


bool puma::isosurface_MarchingCubes(std::vector< puma::Triangle<float> > *triangles, Workspace *work, puma::Cutoff cutoff, bool closedEdges, int factor, bool interpolateVerts, int numThreads) {
    IsoSurface iso(triangles,work,cutoff,closedEdges,factor,interpolateVerts,false,numThreads);
    return iso.compute();
}

bool puma::isosurface_LewinerMarchingCubes(std::vector< puma::Triangle<float> > *triangles, Workspace *work, puma::Cutoff cutoff, bool closedEdges, int factor, bool interpolateVerts, int numThreads) {
    IsoSurface iso(triangles,work,cutoff,closedEdges,factor,interpolateVerts,true,numThreads);
    return iso.compute();
}


IsoSurface::IsoSurface(std::vector< puma::Triangle<float> > *triangles, puma::Workspace *work, puma::Cutoff cutoff, bool closedEdges, int factor, bool interpolateVerts, bool lewiner, int numThreads) {
    this->triangles = triangles;
    this->work=work;
    this->cutoff=cutoff;
    this->closedEdges=closedEdges;
    this->factor=factor;
    this->interpolateVerts=interpolateVerts;
    this->numThreads=numThreads;
    this->lewiner = lewiner;
}

bool IsoSurface::compute() {
    triangles->clear();

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        std::cout << "Marching Cubes: " << errorMessage << std::endl;
        return false;
    }

    puma::Matrix<float> newMatrix;
    copyGrayscaleMatrix(&newMatrix);
    downscale(&newMatrix);
    double isoVal = manipulateGrayscaleMatrix(&newMatrix);

    if(lewiner) {
        if(closedEdges) {
            puma::Matrix<float> closedMatrix(newMatrix.X()+2,newMatrix.Y()+2, newMatrix.Z()+2, -1e10);
#pragma omp parallel for
            for(long i = 0; i < newMatrix.X(); i++) {
                for(long j = 0; j < newMatrix.Y(); j++) {
                    for(long k = 0; k < newMatrix.Z(); k++) {
                        closedMatrix(i+1,j+1,k+1) = newMatrix(i,j,k);
                    }
                }
            }

            if(closedMatrix.size() > 5e8) {

                // putting into batches of 400
                long nX = std::ceil(closedMatrix.X() / 400.0);
                long nY = std::ceil(closedMatrix.Y() / 400.0);
                long nZ = std::ceil(closedMatrix.Z() / 400.0);

                std::vector<long> sX(nX);
                for( int iX = 0; iX < nX - 1; iX++) {
                    sX[iX] = 401;
                }
                sX[nX-1] = closedMatrix.X() - (nX-1) * 400;

                std::vector<long> sY(nY);
                for( int iY = 0; iY < nY - 1; iY++) {
                    sY[iY] = 401;
                }
                sY[nY-1] = closedMatrix.Y() - (nY-1) * 400;

                std::vector<long> sZ(nZ);
                for( int iZ = 0; iZ < nZ - 1; iZ++) {
                    sZ[iZ] = 401;
                }
                sZ[nZ-1] = closedMatrix.Z() - (nZ-1) * 400;


                for( int iX = 0; iX < nX; iX++) {
                    for (int iY = 0; iY < nY; iY++) {
                        for (int iZ = 0; iZ < nZ; iZ++) {
                            long startX = iX * 400;
                            long startY = iY * 400;
                            long startZ = iZ * 400;
                            long endX = startX + sX[iX] - 1;
                            long endY = startY + sY[iY] - 1;
                            long endZ = startZ + sZ[iZ] - 1;

                            std::cout << "Marching Cubes Batch: " << startX << " " << endX << " - " << startY << " "
                                      << endY << " - " << startZ << " " << endZ << std::endl;

                            puma::Matrix<float> batchMatrix(sX[iX], sY[iY], sZ[iZ], -1e10);

                            for (long i = 0; i < batchMatrix.X(); i++) {
                                for (long j = 0; j < batchMatrix.Y(); j++) {
                                    for (long k = 0; k < batchMatrix.Z(); k++) {
                                        batchMatrix(i, j, k) = closedMatrix(i + startX, j + startY, k + startZ);
                                    }
                                }
                            }

                            std::vector<puma::Triangle<float> > batchTris;

                            MarchingCubes mc(&batchMatrix, &batchTris);
                            mc.run(isoVal);

                            for (int t = 0; t < batchTris.size(); t++) {
                                batchTris[t] = batchTris[t] + puma::Vec3<float>(startX, startY, startZ);
                            }

                            (*triangles).insert((*triangles).end(), batchTris.begin(), batchTris.end());

                        }
                    }
                }



            } else{
                MarchingCubes mc(&closedMatrix, triangles);
                mc.run(isoVal);


            }

        } else {
            MarchingCubes mc(&newMatrix, triangles);
            mc.run( isoVal ) ;
        }

    } else {
        return IsoSurfaceHelper::MarchingCubes(triangles,&newMatrix,isoVal, closedEdges,numThreads);
    }

    return true;


//    }
}

bool IsoSurface::copyGrayscaleMatrix(puma::Matrix<float> *newMatrix) {
    if(interpolateVerts) {
        cutoff =  IsoSurfaceHelper::InterpolateVerts(&work->matrix, newMatrix, cutoff, numThreads);
        return true;
    }else {
        newMatrix->resize( work->matrix.X(), work->matrix.Y(), work->matrix.Z() );
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<work->matrix.size(); i++) {
            newMatrix->operator()(i) = work->matrix(i);
        }
        return true;
    }
}

bool IsoSurface::manipulateSegmentedMatrix(puma::Matrix<short> *newMatrix) {
    newMatrix->resize( work->matrix.X(), work->matrix.Y(), work->matrix.Z() );
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long i=0;i<work->matrix.size(); i++) {
        if (work->matrix(i) >= cutoff.first && work->matrix(i) <= cutoff.second) {
            newMatrix->operator()(i) = 1;
        }
        else {
            newMatrix->operator()(i) = 0;
        }
    }
    if(interpolateVerts) {
        puma::Matrix<short> tempMatrix;
        tempMatrix.copy(newMatrix);
        return IsoSurfaceHelper::InterpolateVertsSegmented(&tempMatrix, newMatrix, numThreads);
    }
    return true;
}

bool IsoSurface::downscale(puma::Matrix<float> *Matrix) {
    if(factor > 1) {
        return IsoSurfaceHelper::DownScale(Matrix,factor,numThreads);
    }
    else if (factor == 1) {
        return true;
    }
    return false;
}

bool IsoSurface::downscale(puma::Matrix<short> *Matrix) {
    if(factor > 1) {
        return IsoSurfaceHelper::DownScale(Matrix,factor,numThreads);
    }
    else if (factor == 1) {
        return true;
    }
    return false;
}

double IsoSurface::manipulateGrayscaleMatrix(puma::Matrix<float> *Matrix){

    double average = (double)(cutoff.first+cutoff.second)/2;
    Matrix->flipAroundValue((float)average);
    return cutoff.first;
}

bool IsoSurface::errorCheck(std::string *errorMessage) {
    if( work->matrix.size()==0) {
        *errorMessage = "Marching Cube Error: Empty Grayscale Matrix";
        return false;
    }

    if( cutoff.first<= -1 || cutoff.first>cutoff.second || cutoff.second> 32767 ) {
        *errorMessage = "Marching Cube Error: Invalid low and high ranges";
        return false;
    }
    if( factor < 1 ) {
        *errorMessage = "Marching Cubes Error: Invalid factor";
        return false;
    }
    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }
    return true;
}
