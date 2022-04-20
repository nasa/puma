#include "curvedflowerfiber.h"


double CurvedFlowerFiber::approximateVolume(RandomFibersInput& input) {

    double fiberVolume = (atan(1)*4.0*input.avgRadius*input.avgRadius*input.avgLength)*(input.AvgNumSmallFibers + 1)*2;
    return fiberVolume;
}


bool CurvedFlowerFiber::randomParameters(RandomFibersInput& input, sitmo::prng_engine *engine) {

    double dX;
    double dY;
    double dZ;

    this->engine = engine;

    double rand0= std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8);
    if(input.hollow){
        if(rand0 < input.fractionOfHollowFibers){
            thisFiberIsHollow = true;
        }
        else{
            thisFiberIsHollow = false;
        }
    }
    else{
        thisFiberIsHollow = false;
    }

    //new NumSmallFibers based on average value and deviation
    rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    int NumSmallFibers = input.AvgNumSmallFibers + rand0*input.dNumSmallFibers;

    if(thisFiberIsHollow) {
        fibers.resize(NumSmallFibers+3);
    }
    else{
        fibers.resize(NumSmallFibers+2);
    }

    //new radius based on average value and deviation
    rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    fibers[0].radius = input.avgRadius + rand0*input.dRadius;

    //new small radius based on average value and deviation
    for(int i=1;i<(int)fibers.size();i++) {
        rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
        fibers[i].radius = input.AvgSmallRadius + rand0*input.dSmallRadius;
    }

    //new radius of curvature based on average value and deviation
    rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    fibers[0].RadiusOfCurvature = input.avgRadiusOfCurvature + rand0*input.dRadiusOfCurvature;

    //new length based on average value and deviation
    rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    fibers[0].length = input.avgLength + rand0*input.dLength;

    //random starting position eqiprobable in XYZ space
    fibers[0].startPos.x=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.xSize);
    fibers[0].startPos.y=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.ySize);
    fibers[0].startPos.z=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.zSize);

    fibers[0].endPos = get_end_position(&input, engine, fibers[0].startPos, fibers[0].length);

    // Generate the curve for the main fiber. Only the main fiber is random
    fibers[0].engine = engine;
    fibers[0].accuracy = input.accuracy;
    fibers[0].curve = fibers[0].GenerateRandomCurve(fibers[0].startPos, fibers[0].endPos, fibers[0].RadiusOfCurvature);

    fibers[0].direction_vector = fibers[0].curve.direction_vector;
    fibers[0].center_point = fibers[0].curve.center;

    if(thisFiberIsHollow) {
        fibers[fibers.size()-1].curve.p1 = fibers[0].curve.p1;
        fibers[fibers.size()-1].curve.p2 = fibers[0].curve.p2;
        fibers[fibers.size()-1].curve.radiusOfCurvature = fibers[0].curve.radiusOfCurvature;
        fibers[fibers.size()-1].curve.direction_vector = fibers[0].curve.direction_vector;
        fibers[fibers.size()-1].curve.center = fibers[0].curve.center;
        fibers[fibers.size()-1].length = fibers[0].length;

        fibers[fibers.size()-1].startPos          = fibers[fibers.size()-1].curve.p1;
        fibers[fibers.size()-1].endPos            = fibers[fibers.size()-1].curve.p2;
        fibers[fibers.size()-1].center_point      = fibers[fibers.size()-1].curve.center;
        fibers[fibers.size()-1].RadiusOfCurvature = fibers[fibers.size()-1].curve.radiusOfCurvature;
        fibers[fibers.size()-1].direction_vector  = fibers[fibers.size()-1].curve.direction_vector;

        //new hollow radius based on average value and deviation
        rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
        fibers[fibers.size()-1].radius = input.avgHollowRadius + rand0*input.dHollowRadius;
    }

    //Calculate properties for the second fiber

    //   std::cout << "------------------" << std::endl;

    if(fibers.size()>1) {
        fibers[1].accuracy = input.accuracy;
        fibers[1].curve.p1 = calculatePointsOfSecondFiber(fibers[0].curve.p1, fibers[0].curve.center, fibers[0].radius);
        fibers[1].curve.p2 = calculatePointsOfSecondFiber(fibers[0].curve.p2, fibers[0].curve.center, fibers[0].radius);

        fibers[1].curve.radiusOfCurvature = getNewRadiusOfCurvature(fibers[0].curve.p1, fibers[1].curve.p1, fibers[0].curve.radiusOfCurvature, fibers[0].curve.center, fibers[0].radius);
        fibers[1].curve.direction_vector = fibers[0].curve.direction_vector;
        fibers[1].curve.center = getNewCenterPoint(fibers[1].curve.p1, fibers[1].curve.p2, fibers[1].curve.radiusOfCurvature, fibers[1].curve.direction_vector);
        fibers[1].length = getNewLength(fibers[1].curve.p1, fibers[1].curve.p2, fibers[1].curve.center, fibers[1].curve.radiusOfCurvature);

        fibers[1].startPos = fibers[1].curve.p1;
        fibers[1].endPos = fibers[1].curve.p2;
        fibers[1].center_point = fibers[1].curve.center;
        fibers[1].RadiusOfCurvature = fibers[1].curve.radiusOfCurvature;
        fibers[1].direction_vector = fibers[1].curve.direction_vector;
    }

    //Moving the fibers[1] information into its Curve Class
    //fibers[1].curve = Curve(fibers[1].startPos, fibers[1].endPos, fibers[1].center_point, fibers[1].RadiusOfCurvature, fibers[0].curve.direction_vector);

    //Create Rotation axis
    puma::Vec3<double> vector_A = getVectorAForRotationAxis(fibers[0].curve.p1, fibers[0].curve.p2, fibers[0].curve.center);
    puma::Vec3<double> v_1 = fibers[0].curve.p1 - fibers[0].curve.center;
    puma::Vec3<double> v_2 = fibers[0].curve.p2 - fibers[0].curve.center;
    puma::Vec3<double> rotation_vector_1 = v_1.cross(vector_A);
    puma::Vec3<double> rotation_vector_2 = v_2.cross(vector_A);
    rotation_vector_1 = rotation_vector_1.norm();
    rotation_vector_2 = rotation_vector_2.norm();

    //Rotate point to define start point and end point of the other fibers
    if(thisFiberIsHollow) {
        for(int i=2;i<(int)fibers.size()-1;i++) {
            rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
            double angle = (i-1.0)*360.0/NumSmallFibers + rand0*input.dPlacement;
            angle *= 3.14159265 / 180; //converting degrees to radians

            fibers[i].accuracy = input.accuracy;
            fibers[i].curve.p1 = rotateCrossSectionalVectorCurved(fibers[1].curve.p1, angle, fibers[0].curve.p1, rotation_vector_1);
            fibers[i].curve.p2 = rotateCrossSectionalVectorCurved(fibers[1].curve.p2, angle, fibers[0].curve.p2, rotation_vector_2);
            fibers[i].curve.radiusOfCurvature = getNewRadiusOfCurvature(fibers[0].curve.p1, fibers[i].curve.p1, fibers[0].curve.radiusOfCurvature, fibers[0].curve.center, fibers[0].radius);
            fibers[i].curve.direction_vector = fibers[0].curve.direction_vector;
            fibers[i].curve.center = getNewCenterPoint(fibers[i].curve.p1, fibers[i].curve.p2, fibers[i].curve.radiusOfCurvature, fibers[i].curve.direction_vector);
            fibers[i].length = getNewLength(fibers[i].curve.p1, fibers[i].curve.p2, fibers[i].curve.center, fibers[i].curve.radiusOfCurvature);

            fibers[i].startPos = fibers[i].curve.p1;
            fibers[i].endPos = fibers[i].curve.p2;
            fibers[i].center_point = fibers[i].curve.center;
            fibers[i].RadiusOfCurvature = fibers[i].curve.radiusOfCurvature;
            fibers[i].direction_vector = fibers[i].curve.direction_vector;

            //Moving the fibers[i] information into its Curve Class
            //fibers[i].curve = Curve(fibers[i].startPos, fibers[i].endPos, fibers[i].center_point, fibers[i].RadiusOfCurvature, fibers[0].curve.direction_vector);
        }
    }
    else {
        for(int i=2;i<(int)fibers.size();i++) {
            rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
            double angle = (i-1.0)*360.0/NumSmallFibers + rand0*input.dPlacement;
            angle *= 3.14159265 / 180; //converting degrees to radians

            fibers[i].accuracy = input.accuracy;
            fibers[i].curve.p1 = rotateCrossSectionalVectorCurved(fibers[1].curve.p1, angle, fibers[0].curve.p1, rotation_vector_1);
            fibers[i].curve.p2 = rotateCrossSectionalVectorCurved(fibers[1].curve.p2, angle, fibers[0].curve.p2, rotation_vector_2);
            fibers[i].curve.radiusOfCurvature = getNewRadiusOfCurvature(fibers[0].curve.p1, fibers[i].curve.p1, fibers[0].curve.radiusOfCurvature, fibers[0].curve.center, fibers[0].radius);
            fibers[i].curve.direction_vector = fibers[0].curve.direction_vector;
            fibers[i].curve.center = getNewCenterPoint(fibers[i].curve.p1, fibers[i].curve.p2, fibers[i].curve.radiusOfCurvature, fibers[i].curve.direction_vector);
            fibers[i].length = getNewLength(fibers[i].curve.p1, fibers[i].curve.p2, fibers[i].curve.center, fibers[i].curve.radiusOfCurvature);

            fibers[i].startPos = fibers[i].curve.p1;
            fibers[i].endPos = fibers[i].curve.p2;
            fibers[i].center_point = fibers[i].curve.center;
            fibers[i].RadiusOfCurvature = fibers[i].curve.radiusOfCurvature;
            fibers[i].direction_vector = fibers[i].curve.direction_vector;

            //Moving the fibers[i] information into its Curve Class
            //  fibers[i].curve = Curve(fibers[i].startPos,fibers[i].endPos,fibers[i].center_point,fibers[i].RadiusOfCurvature, fibers[0].curve.direction_vector);
        }
    }



    //    fibers[1].length = 132;
    //    fibers[2].length = 120;
    //    fibers[3].length = 108;
    //    fibers[4].length = 120;

    //    fibers[1].length = 200;
    //    fibers[2].length = 200;
    //    fibers[3].length = 200;
    //    fibers[4].length = 200;

    //    for(int i=0;i<(int)fibers.size();i++) {
    //        std::cout << "Fibers[" << i << "] Radius Curv = " << fibers[i].curve.radiusOfCurvature << std::endl;
    //        std::cout << "Fibers[" << i << "] Length = " << fibers[i].length << std::endl;
    //        std::cout << "Fibers[" << i << "] Accuracy = " << fibers[i].accuracy << std::endl;
    //    }

    return true;
}


//Determines whether the inputFiber intersects with an already created fiber
bool CurvedFlowerFiber::doesFiberIntersect(puma::Workspace *work, short id) {

    for(int i=0;i<(int)fibers.size();i++) {
        if(fibers[i].doesFiberIntersect(work, id)) {
            return true;
        }
    }

    return false;
}


short CurvedFlowerFiber::getGrayValue(int i, int j, int k) {
    return 0;
}


bool CurvedFlowerFiber::addFiberToDomain(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, short id) {

    if(thisFiberIsHollow) {
        for(int i=0;i<(int)fibers.size()-1;i++) {
            fibers[i].addFiberToDomain(work,dirMatrix, id);
        }

        fibers[fibers.size()-1].addHollowFiberToDomain(work,dirMatrix, id);

    }
    else {
        for(int i=0;i<(int)fibers.size();i++) {
            fibers[i].addFiberToDomain(work,dirMatrix, id);
        }
    }

    return true;
}


puma::Vec3<double> CurvedFlowerFiber::calculatePointsOfSecondFiber(puma::Vec3<double> &point_first_fiber, puma::Vec3<double> &center_first_fiber, double bigRadius) {

    puma::Vec3<double> v_fiber_to_center = point_first_fiber - center_first_fiber;
    v_fiber_to_center = v_fiber_to_center.norm();

    puma::Vec3<double> point_of_second_fiber = point_first_fiber + v_fiber_to_center*bigRadius;

    return point_of_second_fiber;
}


puma::Vec3<double> CurvedFlowerFiber::getVectorAForRotationAxis(puma::Vec3<double> &startPos_first_fiber, puma::Vec3<double> &endPos_first_fiber, puma::Vec3<double> &center_first_fiber) {

    puma::Vec3<double> v_1 = startPos_first_fiber - center_first_fiber;
    puma::Vec3<double> v_2 = endPos_first_fiber - center_first_fiber;
    puma::Vec3<double> vector_A = v_1.cross(v_2);

    return vector_A;
}


double CurvedFlowerFiber::getNewRadiusOfCurvature(puma::Vec3<double> &startPos_fiber_1, puma::Vec3<double> &startPos_fiber_n, double &RadiusOfCurvature_fiber_1, puma::Vec3<double> &center_fiber_1, double radiusCross_fiber_1) {

    puma::Vec3<double> vector_n = startPos_fiber_n - startPos_fiber_1;
    puma::Vec3<double> vector_1 = startPos_fiber_1 - center_fiber_1;
    vector_1 = vector_1.norm();
    vector_1 = vector_1*radiusCross_fiber_1;

    double cos_theta = (vector_1.dot(vector_n)) / (vector_1.magnitude()*vector_n.magnitude());

    double newRadiusOfCurvature = RadiusOfCurvature_fiber_1 + radiusCross_fiber_1*cos_theta;

    return newRadiusOfCurvature;
}


puma::Vec3<double> CurvedFlowerFiber::getNewCenterPoint(puma::Vec3<double> &startPos_fiber_n, puma::Vec3<double> &endPos_fiber_n, double &RadiusOfCurvature_fiber_n, puma::Vec3<double> &direction_vector) {

    puma::Vec3<double> midpoint_fiber_n = startPos_fiber_n.midpoint(endPos_fiber_n);

    double d_1_fiber_n = startPos_fiber_n.distance(midpoint_fiber_n);
    double d_2_fiber_n = sqrt(RadiusOfCurvature_fiber_n*RadiusOfCurvature_fiber_n - d_1_fiber_n*d_1_fiber_n);

    puma::Vec3<double> center_point_fiber_n = midpoint_fiber_n + direction_vector*d_2_fiber_n;

    return center_point_fiber_n;
}


double CurvedFlowerFiber::getNewLength(puma::Vec3<double> &startPos_fiber_n, puma::Vec3<double> &endPos_fiber_n, puma::Vec3<double> &center_fiber_n, double &RadiusOfCurvature_fiber_n) {

    puma::Vec3<double> w_1 = startPos_fiber_n - center_fiber_n;
    puma::Vec3<double> w_n = endPos_fiber_n - center_fiber_n;

    double cos_theta_arc = (w_1.dot(w_n)) / (w_1.magnitude()*w_n.magnitude());
    double theta_arc = acos(cos_theta_arc); //NOTE: theta is calculated here in radians
    //    std::cout << "theta_arc = " << theta_arc << std::endl;
    double new_length = theta_arc*RadiusOfCurvature_fiber_n;

    return new_length;
}


puma::Vec3<double> CurvedFlowerFiber::rotateCrossSectionalVectorCurved(puma::Vec3<double> &Previous_SmallFiber_Vector, double theta, puma::Vec3<double> &point_1, puma::Vec3<double> Rotation_Axis) {

    // unit vector of rotation axis
    puma::Vec3<double> u = Rotation_Axis;
    // New Small Fiber Vector
    puma::Vec3<double> q1;
    puma::Vec3<double> q2;
    double d;

    /* Step 1 */
    q1 = Previous_SmallFiber_Vector - point_1;

    d = sqrt(u.y*u.y + u.z*u.z);

    /* Step 2 */
    if (d != 0){
        q2.x = q1.x;
        q2.y = q1.y * u.z / d - q1.z * u.y / d;
        q2.z = q1.y * u.y / d + q1.z * u.z / d;
    }else{
        q2 = q1;
    }

    /* Step 3 */
    q1.x = q2.x * d - q2.z * u.x;
    q1.y = q2.y;
    q1.z = q2.x * u.x + q2.z * d;

    /* Step 4 */
    q2.x = q1.x * cos(theta) - q1.y * sin(theta);
    q2.y = q1.x * sin(theta) + q1.y * cos(theta);
    q2.z = q1.z;

    /* Inverse of step 3 */
    q1.x =   q2.x * d + q2.z * u.x;
    q1.y =   q2.y;
    q1.z = - q2.x * u.x + q2.z * d;

    /* Inverse of step 2 */
    if (d != 0){
        q2.x =   q1.x;
        q2.y =   q1.y * u.z / d + q1.z * u.y / d;
        q2.z = - q1.y * u.y / d + q1.z * u.z / d;
    }else{
        q2 = q1;
    }

    /* Inverse of step 1 */
    q1.x = q2.x + point_1.x;
    q1.y = q2.y + point_1.y;
    q1.z = q2.z + point_1.z;

    return q1;
}

