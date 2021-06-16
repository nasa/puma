#include "fiber.h"


double Fiber::pointSegment( puma::Vec3<double> &voxel_point, puma::Vec3<double> &point_1, puma::Vec3<double> &point_2) {
    //vector v = s2 to s1;
    puma::Vec3<double> v = point_2 - point_1;

    //vector w = p to s1;
    puma::Vec3<double> w = voxel_point - point_1;

    double c1 = v.dot(w);

    if(c1<=0) return voxel_point.distance(point_1);
    double c2 = v.dot(v);
    if(c2<=c1) return voxel_point.distance(point_2);

    double b = c1 / c2;

    //    Point Pb = S.P0 + b * v;
    puma::Vec3<double> p_b = point_1 + v*(b);

    double distance = sqrt((voxel_point.x-p_b.x)*(voxel_point.x-p_b.x)+(voxel_point.y-p_b.y)*(voxel_point.y-p_b.y)+(voxel_point.z-p_b.z)*(voxel_point.z-p_b.z));

    return distance;
}

double Fiber::pointSegment_Uncapped( puma::Vec3<double> &voxel_point, puma::Vec3<double> &point_1, puma::Vec3<double> &point_2) {
    //vector v = s2 to s1;
    puma::Vec3<double> v = point_2 - point_1;

    //vector w = p to s1;
    puma::Vec3<double> w = voxel_point - point_1;

    double c1 = v.dot(w);

    if(c1<=0) return 1e6;
    double c2 = v.dot(v);
    if(c2<=c1) return 1e6;

    double b = c1 / c2;

    //    Point Pb = S.P0 + b * v;
    puma::Vec3<double> p_b = point_1 + v*(b);

    double distance = sqrt((voxel_point.x-p_b.x)*(voxel_point.x-p_b.x)+(voxel_point.y-p_b.y)*(voxel_point.y-p_b.y)+(voxel_point.z-p_b.z)*(voxel_point.z-p_b.z));

    return distance;
}



double Fiber::pointSegment( double px, double py, double pz, double s1x, double s1y, double s1z, double s2x, double s2y, double s2z) {
    //vector v = s2 to s1;
    double v1 = s2x - s1x;
    double v2 = s2y - s1y;
    double v3 = s2z - s1z;

    //vector w = p to s1;
    double w1 = px - s1x;
    double w2 = py - s1y;
    double w3 = pz - s1z;

    double c1 = v1*w1+v2*w2+v3*w3;

    if(c1<=0) return 255;
    double c2 = v1*v1+v2*v2+v3*v3;
    if(c2<=c1) return 255;

    double b = c1 / c2;

    //    Point Pb = S.P0 + b * v;

    double pbx = s1x+b*v1;
    double pby = s1y+b*v2;
    double pbz = s1z+b*v3;

    double distance = sqrt((px-pbx)*(px-pbx)+(py-pby)*(py-pby)+(pz-pbz)*(pz-pbz));

    return distance;
}

