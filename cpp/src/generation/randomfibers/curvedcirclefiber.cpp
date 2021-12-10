#include "curvedcirclefiber.h"


CurvedCircleFiber::CurvedCircleFiber(double& radius, double& length, puma::Vec3<double>& startPos, puma::Vec3<double>& endPos, double& RadiusOfCurvature, puma::Vec3<double>& center_point, double& accuracy, puma::Vec3<double>& direction_vector) {
    this->radius = radius;
    this->length = length;
    this->startPos = startPos;
    this->endPos = endPos;
    this->RadiusOfCurvature = RadiusOfCurvature;
    this->center_point = center_point;
    this->accuracy = accuracy;
    this->direction_vector = direction_vector;
}


bool CurvedCircleFiber::setValues(double& radius, double& length, puma::Vec3<double>& startPos, puma::Vec3<double>& endPos, double& RadiusOfCurvature, puma::Vec3<double>& center_point, double& accuracy, puma::Vec3<double>& direction_vector) {
    this->radius = radius;
    this->length = length;
    this->startPos = startPos;
    this->endPos = endPos;
    this->RadiusOfCurvature = RadiusOfCurvature;
    this->center_point = center_point;
    this->accuracy = accuracy;
    this->direction_vector = direction_vector;
    return true;
}


short CurvedCircleFiber::getGrayValue(int i, int j, int k) {

    double deltaG = 127.0 / radius;

    puma::Vec3<double> voxelPoint(i,j,k);

    double gray = 255.0 - deltaG* pointCurve(curve,voxelPoint,accuracy, 0);
    if(gray<0) gray=0;

    return (short)gray;
}


long CurvedCircleFiber::getXMin() {
    return (long)(std::min(startPos.x,endPos.x)-radius-curveDistance2-2 - 10*radius);
}


long CurvedCircleFiber::getXMax() {
    return (long)(std::max(startPos.x,endPos.x)+radius+curveDistance2+2 + 10*radius);
}


long CurvedCircleFiber::getYMin() {
    return (long)(std::min(startPos.y,endPos.y)-radius-curveDistance2-2 - 10*radius);
}


long CurvedCircleFiber::getYMax() {
    return (long)(std::max(startPos.y,endPos.y)+radius+curveDistance2+2 + 10*radius);
}


long CurvedCircleFiber::getZMin() {
    return (long)(std::min(startPos.z,endPos.z)-radius-curveDistance2-2 - 10*radius);
}


long CurvedCircleFiber::getZMax() {
    return (long)(std::max(startPos.z,endPos.z)+radius+curveDistance2+2 + 10*radius);
}


double CurvedCircleFiber::approximateVolume(RandomFibersInput& input) {

    double fiberVolume = 0.8*(atan(1)*4.0*input.avgRadius*input.avgRadius*input.avgLength);

    return fiberVolume;
}


bool CurvedCircleFiber::randomParameters(RandomFibersInput& input, sitmo::prng_engine *engine) {
    double dX;
    double dY;
    double dZ;

    this->engine = engine;

    //new radius based on average value and deviation
    double rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    radius = input.avgRadius + rand0*input.dRadius;

    //new length based on average value and deviation
    rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    length = input.avgLength + rand0*input.dLength;

    //new radius of curvature based on average value and deviation
    rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    RadiusOfCurvature = input.avgRadiusOfCurvature + rand0*input.dRadiusOfCurvature;


    //random starting position eqiprobable in XYZ space
    startPos.x=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.xSize);
    startPos.y=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.ySize);
    startPos.z=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.zSize);

    endPos = get_end_position(&input, engine, startPos, length);

    curve = GenerateRandomCurve(startPos,endPos,RadiusOfCurvature);
    accuracy = input.accuracy;

    return true;
}


//Determines whether the inputFiber intersects with an already created fiber
bool CurvedCircleFiber::doesFiberIntersect(puma::Workspace *work, short id) {
    bool intersected = false;

    int iMin=(int)getXMin();
    int iMax=(int)getXMax();
    int jMin=(int)getYMin();
    int jMax=(int)getYMax();
    int kMin=(int)getZMin();
    int kMax=(int)getZMax();


#pragma omp parallel for
    for(int i=iMin;i<iMax;i++){
        for(int j=jMin;j<jMax;j++){
            for(int k=kMin;k<kMax;k++){

                //periodic boundary conditions on cylinders
                long i2=i;
                long j2=j;
                long k2=k;

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


bool CurvedCircleFiber::addHollowFiberToDomain(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, short id) {
    //adds the input fiber to the domain

    int iMin=(int)getXMin();
    int iMax=(int)getXMax();
    int jMin=(int)getYMin();
    int jMax=(int)getYMax();
    int kMin=(int)getZMin();
    int kMax=(int)getZMax();

#pragma omp parallel for
    for(int i=iMin;i<iMax;i++){
        for(int j=jMin;j<jMax;j++){
            for(int k=kMin;k<kMax;k++){

                //periodic boundary conditions on cylinders
                long i2=i;
                long j2=j;
                long k2=k;

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
                double gray = deltaG*pointCurve(curve,voxelPoint,accuracy, 0);
                if(gray > 32767) gray=255;

                if(id==-1) {
                    if (gray < work->matrix(i2, j2, k2)) { // only modify the value at (i2,j2,k2) if the new value is higher than the previous value that was stored there
                        work->matrix(i2, j2, k2) = (short) gray;
                    }
                }
                else {
                    if (gray < 128) {
                        work->matrix(i2, j2, k2) = 0;
                    }
                }

            }
        }
    }

    return true;
}



bool CurvedCircleFiber::addFiberToDomain(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, short id) {
    //adds the input fiber to the domain

    int iMin=(int)getXMin();
    int iMax=(int)getXMax();
    int jMin=(int)getYMin();
    int jMax=(int)getYMax();
    int kMin=(int)getZMin();
    int kMax=(int)getZMax();

#pragma omp parallel for
    for(int i=iMin;i<iMax;i++){
        for(int j=jMin;j<jMax;j++){
            for(int k=kMin;k<kMax;k++){

                //periodic boundary conditions on cy+linders
                long i2=i;
                long j2=j;
                long k2=k;

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
                    }
                }
                else {
                    if(gray >= 128){
                        work->matrix(i2,j2,k2)= id ;
                    }
                }

            }
        }
    }

    return true;
}


puma::Vec3<double> CurvedCircleFiber::RandomPointInSphere(sitmo::prng_engine *engine) {

    puma::Vec3<double> random_point;
    double distance = 2;

    while(distance>1) {
        random_point.x = std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*2-1; //random num between -1 and 1
        random_point.y = std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*2-1;
        random_point.z = std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*2-1;
        distance = sqrt(random_point.x*random_point.x+random_point.y*random_point.y+random_point.z*random_point.z);
    }

    return random_point;
}


puma::Vec3<double> CurvedCircleFiber::GetCenterPoint(puma::Vec3<double> &point_1, puma::Vec3<double> &vector_u, double dist_midpoint_to_center) {

    return point_1 + vector_u*(dist_midpoint_to_center);
}


puma::Vec3<double> CurvedCircleFiber::GetBisectionPoint(puma::Vec3<double> &point_1, puma::Vec3<double> &vector_u, double dist_midpoint_to_center, double radius) {

    return point_1 - vector_u*(radius - dist_midpoint_to_center);
}


Curve CurvedCircleFiber::GenerateRandomCurve(puma::Vec3<double> &point_1, puma::Vec3<double> &point_2, double RadiusOfCurvature) {

    Curve c;
    c.p1 = point_1;
    c.p2 = point_2;
    c.radiusOfCurvature = RadiusOfCurvature;

    puma::Vec3<double> point_3 = puma::Vec3<double>::midpoint(point_1,point_2);
    puma::Vec3<double> dir_12 = puma::Vec3<double>::direction(point_1,point_2);
    dir_12 = dir_12.norm();

    puma::Vec3<double> random_vector = RandomPointInSphere(engine);

    puma::Vec3<double> vector_u = random_vector - dir_12 * ( random_vector.dot(dir_12) );
    vector_u = vector_u.norm();

    c.direction_vector = vector_u;

    double dist_p1_to_midpoint = puma::Vec3<double>::distance(point_1,point_3);
    double dist_midpoint_to_center = sqrt(RadiusOfCurvature*RadiusOfCurvature - dist_p1_to_midpoint*dist_p1_to_midpoint);

    center_point = GetCenterPoint(point_3, vector_u, dist_midpoint_to_center);

    c.center = center_point;

    puma::Vec3<double> dir_midpoint_to_center = puma::Vec3<double>::direction(c.center,point_3);
    dir_midpoint_to_center = dir_midpoint_to_center.norm();

    curveDistance2 = c.radiusOfCurvature - dist_midpoint_to_center;

    return c;
}


std::pair<Curve,Curve> CurvedCircleFiber::SplitCurve(Curve &c) {

    puma::Vec3<double> point_3 = puma::Vec3<double>::midpoint(c.p1,c.p2);

    puma::Vec3<double> dir_midpoint_to_center = puma::Vec3<double>::direction(c.center,point_3);
    dir_midpoint_to_center = dir_midpoint_to_center.norm();

    double dist_midpoint_to_center = puma::Vec3<double>::distance(point_3,c.center);

    puma::Vec3<double> bisection_point = GetBisectionPoint(point_3, dir_midpoint_to_center, dist_midpoint_to_center, c.radiusOfCurvature);

    std::pair<Curve,Curve> splitCurves;
    splitCurves.first.p1 = c.p1;
    splitCurves.first.p2 = bisection_point;
    splitCurves.first.center = c.center;
    splitCurves.first.radiusOfCurvature = c.radiusOfCurvature;

    splitCurves.second.p1 = bisection_point;
    splitCurves.second.p2 = c.p2;
    splitCurves.second.center = c.center;
    splitCurves.second.radiusOfCurvature = c.radiusOfCurvature;

    return splitCurves;
}


double CurvedCircleFiber::pointSegment_Curved_Fiber( puma::Vec3<double> &voxel_point, puma::Vec3<double> &point_1, puma::Vec3<double> &point_2, bool *outside_normal) {

    *outside_normal = false;

    //vector v = s2 to s1;
    puma::Vec3<double> v = point_2 - point_1;

    //vector w = p to s1;
    puma::Vec3<double> w = voxel_point - point_1;

    double c1 = v.dot(w);

    if(c1<=0) {
        *outside_normal = true;
        return voxel_point.distance(point_1);
    }
    double c2 = v.dot(v);
    if(c2<=c1) {
        *outside_normal = true;
        return voxel_point.distance(point_2);
    }

    double b = c1 / c2;

    //    Point Pb = S.P0 + b * v;
    puma::Vec3<double> p_b = point_1 + v*(b);

    double distance = sqrt((voxel_point.x-p_b.x)*(voxel_point.x-p_b.x)+(voxel_point.y-p_b.y)*(voxel_point.y-p_b.y)+(voxel_point.z-p_b.z)*(voxel_point.z-p_b.z));

    return distance;
}


double CurvedCircleFiber::pointCurve(Curve &c, puma::Vec3<double> &voxel_point, double accuracy, int level) {

    std::pair<Curve, Curve> splitCurves = SplitCurve(c);
    bool curve1_outside_normal;
    bool curve2_outside_normal;

    double distance_to_curve_1 = pointSegment_Curved_Fiber(voxel_point, splitCurves.first.p1, splitCurves.first.p2, &curve1_outside_normal);
    double distance_to_curve_2 = pointSegment_Curved_Fiber(voxel_point, splitCurves.second.p1, splitCurves.second.p2, &curve2_outside_normal);

    if(fabs(distance_to_curve_1 - distance_to_curve_2) < accuracy) {

        if(curve1_outside_normal && curve2_outside_normal && level<50) {
            double distanceCurve1 = voxel_point.distance(splitCurves.first.p1) + voxel_point.distance(splitCurves.first.p2);
            double distanceCurve2 = voxel_point.distance(splitCurves.second.p1) + voxel_point.distance(splitCurves.second.p2);

            if(distanceCurve1 < distanceCurve2) {
                level++;
                return pointCurve(splitCurves.first, voxel_point, accuracy, level);
            }
            level++;
            return pointCurve(splitCurves.second, voxel_point, accuracy, level);

        } else {
            if(distance_to_curve_1 < distance_to_curve_2){
                return distance_to_curve_1;
            }
            return distance_to_curve_2;
        }

    }else {

        if( level > 50 ) {
            if(distance_to_curve_1 < distance_to_curve_2){
                return distance_to_curve_1;
            }
            return distance_to_curve_2;
        }

        if(distance_to_curve_1 < distance_to_curve_2){
            level++;
            return pointCurve(splitCurves.first, voxel_point, accuracy, level);
        }
        level++;
        return pointCurve(splitCurves.second, voxel_point, accuracy, level);
    }

}

