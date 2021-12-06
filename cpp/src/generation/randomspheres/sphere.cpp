#include "sphere.h"


Sphere::Sphere(double& sphere_radius, puma::Vec3<double>& center_point) {
    this->sphere_radius = sphere_radius;
    this->center_point = center_point;
}


bool Sphere::setValues(double& sphere_radius, puma::Vec3<double>& center_point) {
    this->sphere_radius = sphere_radius;
    this->center_point = center_point;
    return true;
}


short Sphere::getGrayValue(int i, int j, int k) {

    double deltaG = 127.0 / sphere_radius;

    puma::Vec3<double> voxelPoint(i,j,k);
//    double gray = 255.0 - deltaG*distanceFromCenter(voxelPoint, center_point);
    double gray = (255.-127.5*(1+std::tanh(( 0.5 * ( distanceFromCenter(voxelPoint, center_point) - sphere_radius)))));
    if(gray<0) gray=0;

    return (short)gray;
}


long Sphere::getXMin() {
    return center_point.x - sphere_radius - 2;
}

long Sphere::getXMax() {
    return center_point.x + sphere_radius + 2;
}

long Sphere::getYMin() {
    return center_point.y - sphere_radius - 2;
}

long Sphere::getYMax() {
    return center_point.y + sphere_radius + 2;
}

long Sphere::getZMin() {
    return center_point.z - sphere_radius - 2;
}

long Sphere::getZMax() {
    return center_point.z + sphere_radius + 2;
}


double Sphere::approximateVolume(RandomSpheresInput& input) {

    //Overestimate the sphere volume
    double SphereVolume = 1.1 * (atan(1)*4.0 ) * 4/3 * input.avgRadius * input.avgRadius * input.avgRadius;

    return SphereVolume;
}


bool Sphere::randomParameters(RandomSpheresInput& input, sitmo::prng_engine *engine) {

    //new sphere radius based on average value and deviation
    double rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    sphere_radius = input.avgRadius + rand0*input.dRadius;

    //random center point eqiprobable in XYZ space
    center_point.x=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.xSize);
    center_point.y=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.ySize);
    center_point.z=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.zSize);

    return true;
}


bool Sphere::doesSphereIntersect(puma::Workspace *sphereWS) {
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

                //periodic boundary conditions on spheres
                int i2=i;
                int j2=j;
                int k2=k;

                while(i2<0||j2<0||k2<0||i2>=sphereWS->X()||j2>=sphereWS->Y()||k2>=sphereWS->Z()){
                    if(i2<0) i2=i2+(sphereWS->X());
                    if(j2<0) j2=j2+(sphereWS->Y());
                    if(k2<0) k2=k2+(sphereWS->Z());
                    if(i2>=sphereWS->X()) i2=i2-(sphereWS->X());
                    if(j2>=sphereWS->Y()) j2=j2-(sphereWS->Y());
                    if(k2>=sphereWS->Z()) k2=k2-(sphereWS->Z());
                }

                short gray = getGrayValue(i,j,k);

                if(gray>=128&&sphereWS->matrix(i2,j2,k2)>=128){
                    intersected = true;
                    break;
                }
            }
            if(intersected) break;
        }
    }

    return intersected;
}


bool Sphere::addSphereToDomain(puma::Workspace *sphereWS) {
    //adds the input sphere to the domain

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

                //periodic boundary conditions on sphere
                int i2=i;
                int j2=j;
                int k2=k;

                while(i2<0||j2<0||k2<0||i2>=sphereWS->X()||j2>=sphereWS->Y()||k2>=sphereWS->Z()){
                    if(i2<0) i2=i2+(sphereWS->X());
                    if(j2<0) j2=j2+(sphereWS->Y());
                    if(k2<0) k2=k2+(sphereWS->Z());
                    if(i2>=sphereWS->X()) i2=i2-(sphereWS->X());
                    if(j2>=sphereWS->Y()) j2=j2-(sphereWS->Y());
                    if(k2>=sphereWS->Z()) k2=k2-(sphereWS->Z());
                }

                short gray = getGrayValue(i,j,k);

                if(gray > sphereWS->matrix(i2,j2,k2)){ // only modify the value at (i2,j2,k2) if the new value is higher than the previous value that was stored there
                    sphereWS->matrix(i2,j2,k2)=(short)gray;
                }


            }
        }
    }

    return true;
}


double Sphere::distanceFromCenter( puma::Vec3<double> &voxel_point, puma::Vec3<double> &center_point) {

    double distance = sqrt((voxel_point.x-center_point.x)*(voxel_point.x-center_point.x)+(voxel_point.y-center_point.y)*(voxel_point.y-center_point.y)+(voxel_point.z-center_point.z)*(voxel_point.z-center_point.z));

    return distance;
}
