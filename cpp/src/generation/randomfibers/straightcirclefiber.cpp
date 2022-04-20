#include "straightcirclefiber.h"
#include <math.h>

StraightCircleFiber::StraightCircleFiber(double& radius, double& length, puma::Vec3<double>& startPos, puma::Vec3<double>& endPos) {
    this->radius = radius;
    this->length = length;
    this->startPos = startPos;
    this->endPos = endPos;
}


bool StraightCircleFiber::setValues(double& radius, double& length, puma::Vec3<double>& startPos, puma::Vec3<double>& endPos) {
    this->radius = radius;
    this->length = length;
    this->startPos = startPos;
    this->endPos = endPos;
    return true;
}


short StraightCircleFiber::getGrayValue(int i, int j, int k) {

    double deltaG = 127.0 / radius;

    puma::Vec3<double> voxelPoint(i,j,k);
    double gray = 255.0 - deltaG*pointSegment(voxelPoint,startPos,endPos);
    if(gray<0) gray=0;

    return (short)gray;
}

short StraightCircleFiber::getGrayValue_Uncapped(int i, int j, int k) {

    double deltaG = 127.0 / radius;

    puma::Vec3<double> voxelPoint(i,j,k);
    double gray = 255.0 - deltaG*pointSegment_Uncapped(voxelPoint,startPos,endPos);
    if(gray<0) gray=0;

    return (short)gray;
}



long StraightCircleFiber::getXMin() {
    return std::min(startPos.x,endPos.x)-2*radius;
}


long StraightCircleFiber::getXMax() {
    return std::max(startPos.x,endPos.x)+2*radius;
}


long StraightCircleFiber::getYMin() {
    return std::min(startPos.y,endPos.y)-2*radius;
}


long StraightCircleFiber::getYMax() {
    return std::max(startPos.y,endPos.y)+2*radius;
}


long StraightCircleFiber::getZMin() {
    return std::min(startPos.z,endPos.z)-2*radius;
}


long StraightCircleFiber::getZMax() {
    return std::max(startPos.z,endPos.z)+2*radius;
}


double StraightCircleFiber::approximateVolume(RandomFibersInput& input) {

    double fiberVolume = (atan(1)*4.0*input.avgRadius*input.avgRadius*input.avgLength);

    return fiberVolume;
}


bool StraightCircleFiber::randomParameters(RandomFibersInput& input, sitmo::prng_engine *engine) {
    //new radius based on average value and deviation
    double rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    radius = input.avgRadius + rand0*input.dRadius;

    //new length based on average value and deviation
    rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    length = input.avgLength + rand0*input.dLength;

    //random starting position eqiprobable in XYZ space
    startPos.x=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.xSize);
    startPos.y=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.ySize);
    startPos.z=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.zSize);

    endPos = get_end_position(&input, engine, startPos, length);

    return true;
}


//Determines whether the inputFiber intersects with an already created fiber
bool StraightCircleFiber::doesFiberIntersect(puma::Workspace *work, short id) {
    bool intersected = false;

    int iMin=getXMin();
    int iMax=getXMax();
    int jMin=getYMin();
    int jMax=getYMax();
    int kMin=getZMin();
    int kMax=getZMax();

#pragma omp parallel for
    for(int i=iMin;i<iMax;i++){
        for(int j=jMin;j<jMax;j++){
            for(int k=kMin;k<kMax;k++){

                //periodic boundary conditions on cylinders
                int i2=i;
                int j2=j;
                int k2=k;

                while(i2<0||j2<0||k2<0||i2>=work->X()||j2>=work->Y()||k2>=work->Z()){
                    if(i2<0) i2=i2+(work->X());
                    if(j2<0) j2=j2+(work->Y());
                    if(k2<0) k2=k2+(work->Z());
                    if(i2>=work->X()) i2=i2-(work->X());
                    if(j2>=work->Y()) j2=j2-(work->Y());
                    if(k2>=work->Z()) k2=k2-(work->Z());
                }

                short gray = getGrayValue(i,j,k);

                if(id==-1) {
                    if(gray>=128&&work->matrix(i2,j2,k2)>=128){
                        intersected = true;
                        break;
                    }
                }
                else {
                    if (gray >= 128 && work->matrix(i2, j2, k2) > 0) {
                        intersected = true;
                        break;
                    }
                }


            }
            if(intersected) break;
        }

    }

    return intersected;
}


bool StraightCircleFiber::addHollowFiberToDomain(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, short id) {
    //adds the input fiber to the domain

    int iMin=getXMin();
    int iMax=getXMax();
    int jMin=getYMin();
    int jMax=getYMax();
    int kMin=getZMin();
    int kMax=getZMax();

#pragma omp parallel for
    for(int i=iMin;i<iMax;i++){
        for(int j=jMin;j<jMax;j++){
            for(int k=kMin;k<kMax;k++){

                //periodic boundary conditions on cylinders
                int i2=i;
                int j2=j;
                int k2=k;

                while(i2<0||j2<0||k2<0||i2>=work->X()||j2>=work->Y()||k2>=work->Z()){
                    if(i2<0) i2=i2+(work->X());
                    if(j2<0) j2=j2+(work->Y());
                    if(k2<0) k2=k2+(work->Z());
                    if(i2>=work->X()) i2=i2-(work->X());
                    if(j2>=work->Y()) j2=j2-(work->Y());
                    if(k2>=work->Z()) k2=k2-(work->Z());
                }

                double deltaG = 127.0 / radius;
                puma::Vec3<double> voxelPoint(i,j,k);
                double gray = deltaG*pointSegment(voxelPoint,startPos,endPos);
                if(gray> 32767) gray=255;



                if(id==-1) {
                    if(gray < work->matrix(i2,j2,k2)){ // only modify the value at (i2,j2,k2) if the new value is higher than the previous value that was stored there
                        work->matrix(i2,j2,k2)=(short)gray;
                        if(dirMatrix) {
                            dirMatrix->at(i2,j2,k2) = puma::Vec3<double>(0,0,0);
                        }
                    }
                }
                else {
                    if(gray < 128){ // only modify the value at (i2,j2,k2) if the new value is higher than the previous value that was stored there
                        work->matrix(i2,j2,k2) = 0;
                        if(dirMatrix) {
                            dirMatrix->at(i2,j2,k2) = puma::Vec3<double>(0,0,0);
                        }
                    }
                }


            }
        }
    }

    return true;
}


bool StraightCircleFiber::addFiberToDomain(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, short id) {
    //adds the input fiber to the domain

    int iMin=getXMin();
    int iMax=getXMax();
    int jMin=getYMin();
    int jMax=getYMax();
    int kMin=getZMin();
    int kMax=getZMax();

#pragma omp parallel for
    for(int i=iMin;i<iMax;i++){
        for(int j=jMin;j<jMax;j++){
            for(int k=kMin;k<kMax;k++){

                //periodic boundary conditions on cylinders
                int i2=i;
                int j2=j;
                int k2=k;

                while(i2<0||j2<0||k2<0||i2>=work->X()||j2>=work->Y()||k2>=work->Z()){
                    if(i2<0) i2=i2+(work->X());
                    if(j2<0) j2=j2+(work->Y());
                    if(k2<0) k2=k2+(work->Z());
                    if(i2>=work->X()) i2=i2-(work->X());
                    if(j2>=work->Y()) j2=j2-(work->Y());
                    if(k2>=work->Z()) k2=k2-(work->Z());
                }

                short gray = getGrayValue(i,j,k);




                if( id == -1) {
                    if(gray > work->matrix(i2,j2,k2)){ // only modify the value at (i2,j2,k2) if the new value is higher than the previous value that was stored there
                        work->matrix(i2,j2,k2)=(short)gray;
                        if(dirMatrix && gray >= 128) {
                            // If fibers intersecting, double the magnitude to signal this for anisotropic material properties computation.
                            if(dirMatrix->at(i2, j2, k2).magnitude() > 0) {
                                dirMatrix->at(i2, j2, k2) = dirMatrix->at(i2, j2, k2).norm();
                                dirMatrix->at(i2, j2, k2) = puma::Vec3<double>(2*dirMatrix->at(i2, j2, k2).x,2*dirMatrix->at(i2, j2, k2).y,2*dirMatrix->at(i2, j2, k2).z);
                            } else {
                                dirMatrix->at(i2, j2, k2) = (this->endPos - this->startPos).norm();
                            }
                        }
                    }
                }
                else {
                    if(gray >= 128){ // only modify the value at (i2,j2,k2) if the new value is higher than the previous value that was stored there
                        work->matrix(i2,j2,k2) = id ;
                        if(dirMatrix) {
                            // If fibers intersecting, double the magnitude to signal this for anisotropic material properties computation.
                            if(dirMatrix->at(i2, j2, k2).magnitude() > 0) {
                                dirMatrix->at(i2, j2, k2) = dirMatrix->at(i2, j2, k2).norm();
                                dirMatrix->at(i2, j2, k2) = puma::Vec3<double>(2*dirMatrix->at(i2, j2, k2).x,2*dirMatrix->at(i2, j2, k2).y,2*dirMatrix->at(i2, j2, k2).z);
                            } else {
                                dirMatrix->at(i2, j2, k2) = (this->endPos - this->startPos).norm();
                            }
                        }
                    }
                }

            }
        }
    }

    return true;
}

bool StraightCircleFiber::addFiberToDomain_Uncapped(puma::Workspace *work, puma::MatVec3<double> *dirMatrix) {
    //adds the input fiber to the domain

    int iMin=getXMin();
    int iMax=getXMax();
    int jMin=getYMin();
    int jMax=getYMax();
    int kMin=getZMin();
    int kMax=getZMax();

#pragma omp parallel for
    for(int i=iMin;i<iMax;i++){
        for(int j=jMin;j<jMax;j++){
            for(int k=kMin;k<kMax;k++){

                //periodic boundary conditions on cylinders
                int i2=i;
                int j2=j;
                int k2=k;

                while(i2<0||j2<0||k2<0||i2>=work->X()||j2>=work->Y()||k2>=work->Z()){
                    if(i2<0) i2=i2+(work->X());
                    if(j2<0) j2=j2+(work->Y());
                    if(k2<0) k2=k2+(work->Z());
                    if(i2>=work->X()) i2=i2-(work->X());
                    if(j2>=work->Y()) j2=j2-(work->Y());
                    if(k2>=work->Z()) k2=k2-(work->Z());
                }

                short gray = getGrayValue_Uncapped(i,j,k);

                if(gray > work->matrix(i2,j2,k2)){ // only modify the value at (i2,j2,k2) if the new value is higher than the previous value that was stored there
                    work->matrix(i2,j2,k2)=(short)gray;
                    if(dirMatrix && gray >= 128) {
                        // If fibers intersecting, double the magnitude to signal this for anisotropic material properties computation.
                        if(dirMatrix->at(i2, j2, k2).magnitude() > 0) {
                            dirMatrix->at(i2, j2, k2) = dirMatrix->at(i2, j2, k2).norm();
                            dirMatrix->at(i2, j2, k2) = puma::Vec3<double>(2*dirMatrix->at(i2, j2, k2).x,2*dirMatrix->at(i2, j2, k2).y,2*dirMatrix->at(i2, j2, k2).z);
                        } else {
                            dirMatrix->at(i2, j2, k2) = (this->endPos - this->startPos).norm();
                        }
                    }
                }


            }
        }
    }

    return true;
}


double StraightCircleFiber::getAverageValue(puma::Workspace *work) {

    int iMin=getXMin();
    int iMax=getXMax();
    int jMin=getYMin();
    int jMax=getYMax();
    int kMin=getZMin();
    int kMax=getZMax();

    double sum = 0;
    double count = 0;

#pragma omp parallel for
    for(int i=iMin;i<iMax;i++){
        for(int j=jMin;j<jMax;j++){
            for(int k=kMin;k<kMax;k++){

                //periodic boundary conditions on cylinders
                int i2=i;
                int j2=j;
                int k2=k;

                while(i2<0||j2<0||k2<0||i2>=work->X()||j2>=work->Y()||k2>=work->Z()){
                    if(i2<0) i2=i2+(work->X());
                    if(j2<0) j2=j2+(work->Y());
                    if(k2<0) k2=k2+(work->Z());
                    if(i2>=work->X()) i2=i2-(work->X());
                    if(j2>=work->Y()) j2=j2-(work->Y());
                    if(k2>=work->Z()) k2=k2-(work->Z());
                }

                short gray = getGrayValue(i,j,k);

                if( gray>=128 ){
                    sum += work->matrix(i2,j2,k2);
                    count += 1.0;
                }
            }
        }
    }

    return sum/count;
}
