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

    if(c1<0) return 1e6;
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

puma::Vec3<double> Fiber::get_end_position(RandomFibersInput *input, sitmo::prng_engine *engine, puma::Vec3<double> startPos, double length) {
    puma::Vec3<double> dir_vec(1,1,1);

    if(input->angleType == 0) { //isotropic
        double theta = std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8) * 2.0 * M_PI;
        double phi = std::acos(1 - 2 * std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8));

        dir_vec.x = std::sin(phi) * std::cos(theta);
        dir_vec.y = std::sin(phi) * std::sin(theta);
        dir_vec.z = std::cos(phi);

    } else if(input->angleType == 1) { //transverse isotropic
        double theta = std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8) * 2.0 * M_PI;
        double phi = 0;

        if(input->angle_variability <= 0.5) { // 0.5 is a bit arbitrary, but basically if it's close to or equal to 0, i set it to 0
            phi = M_PI/2.0;
        } else {
            phi = std::acos(1 - 2 * std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8));
            while( std::fabs( M_PI / 2.0 - phi ) > input->angle_variability * M_PI / 180.0  ) {
                phi = std::acos(1 - 2 * std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8));
            }
        }

        puma::Vec3<double> temp_vec(std::sin(phi) * std::cos(theta), std::sin(phi) * std::sin(theta),std::cos(phi));

        if(input->var_direction == 0) { // 1D in x
            dir_vec = puma::Vec3<double>(temp_vec.z,temp_vec.y,temp_vec.x);
        } else if(input->var_direction == 1) { // 1D in y
            dir_vec = puma::Vec3<double>(temp_vec.x,temp_vec.z,temp_vec.y);
        } else if(input->var_direction == 2) { // 1D in z
            dir_vec = puma::Vec3<double>(temp_vec.x,temp_vec.y,temp_vec.z);
        }

    } else if(input->angleType == 2) { // 1D
        if(input->var_direction == 0) { // 1D in x
            dir_vec = puma::Vec3<double>(1,0,0);
        } else if(input->var_direction == 1) { // 1D in y
            dir_vec = puma::Vec3<double>(0,1,0);
        } else if(input->var_direction == 2) { // 1D in z
            dir_vec = puma::Vec3<double>(0,0,1);
        }
    }

    return startPos + dir_vec * length;
}
