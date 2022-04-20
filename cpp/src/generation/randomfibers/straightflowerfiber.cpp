#include "straightflowerfiber.h"


double StraightFlowerFiber::approximateVolume(RandomFibersInput& input) {

    double fiberVolume = (atan(1)*4.0*input.avgRadius*input.avgRadius*input.avgLength)*(input.AvgNumSmallFibers + 1)*2;

    return fiberVolume;
}


bool StraightFlowerFiber::randomParameters(RandomFibersInput& input, sitmo::prng_engine *engine) {

    double dX;
    double dY;
    double dZ;

    this->engine = engine;


    double rand0= std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8);
    if(input.hollow){
        thisFiberIsHollow = rand0 < input.fractionOfHollowFibers;
    }
    else{
        thisFiberIsHollow = false;
    }

    //new NumSmallFibers based on average value and deviation
    rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    int NumSmallFibers = (int)(input.AvgNumSmallFibers + rand0*input.dNumSmallFibers);

    if(thisFiberIsHollow) {
        fibers.resize(NumSmallFibers+2);
    }
    else{
        fibers.resize(NumSmallFibers+1);
    }

    //new radius based on average value and deviation
    rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    fibers[0].radius = input.avgRadius + rand0*input.dRadius;

    //new small radius based on average value and deviation
    for(int i=1;i<(int)fibers.size();i++) {
        rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
        fibers[i].radius = input.AvgSmallRadius + rand0*input.dSmallRadius;
    }

    //new length based on average value and deviation
    rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
    double length = input.avgLength + rand0*input.dLength;

    //random starting position eqiprobable in XYZ space
    fibers[0].startPos.x=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.xSize);
    fibers[0].startPos.y=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.ySize);
    fibers[0].startPos.z=(int)(std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)*input.zSize);


    fibers[0].endPos = get_end_position(&input, engine, fibers[0].startPos, length);


    if(thisFiberIsHollow) {
        fibers[fibers.size()-1].startPos = fibers[0].startPos;
        fibers[fibers.size()-1].endPos = fibers[0].endPos;

        //new hollow radius based on average value and deviation
        rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
        fibers[fibers.size()-1].radius = input.avgHollowRadius + rand0*input.dHollowRadius;
    }

    puma::Vec3<double> vector_u = calculateVectorU(fibers[0].startPos, fibers[0].endPos, fibers[0].radius);
    if(fibers.size()>1) {
        if( ! ( fibers.size() == 2 && thisFiberIsHollow) ) {
            fibers[1].startPos = fibers[0].startPos + vector_u;
            fibers[1].endPos = fibers[0].endPos + vector_u;
        }
    }

    if(thisFiberIsHollow) {
        for(int i=2;i<(int)fibers.size()-1;i++) {
            rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
            double angle = (i-1.0)*360.0/NumSmallFibers + rand0*input.dPlacement;
            angle *= 3.14159265 / 180; //converting degrees to radians

            fibers[i].startPos = rotateCrossSectionalVector(fibers[1].startPos, angle, fibers[0].startPos, fibers[0].endPos);
            fibers[i].endPos = fibers[i].startPos-fibers[0].startPos+fibers[0].endPos;
        }
    }
    else {
        for(int i=2;i<(int)fibers.size();i++) {
            rand0 = 2*std::max((double)engine->operator ()()/sitmo::prng_engine::max(),1e-8)-1;
            double angle = (i-1.0)*360.0/NumSmallFibers + rand0*input.dPlacement;
            angle *= 3.14159265 / 180; //converting degrees to radians

            fibers[i].startPos = rotateCrossSectionalVector(fibers[1].startPos, angle, fibers[0].startPos, fibers[0].endPos);
            fibers[i].endPos = fibers[i].startPos-fibers[0].startPos+fibers[0].endPos;
        }
    }

    return true;
}


//Determines whether the inputFiber intersects with an already created fiber
bool StraightFlowerFiber::doesFiberIntersect(puma::Workspace *work, short id) {

    for(auto & fiber : fibers) {
        if(fiber.doesFiberIntersect(work, id)) {
            return true;
        }
    }

    return false;
}


short StraightFlowerFiber::getGrayValue(int i, int j, int k) {
    return 0;
}


bool StraightFlowerFiber::addFiberToDomain(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, short id) {

    if(thisFiberIsHollow) {
        for(int i=0;i<(int)fibers.size()-1;i++) {
            fibers[i].addFiberToDomain(work,dirMatrix, id);
        }
        fibers[fibers.size()-1].addHollowFiberToDomain(work,dirMatrix, id);

    }
    else {
        for(auto & fiber : fibers) {
            fiber.addFiberToDomain(work,dirMatrix, id);
        }
    }

    return true;
}


puma::Vec3<double> StraightFlowerFiber::getRandomPointInSphere(sitmo::prng_engine *engine) {

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


puma::Vec3<double> StraightFlowerFiber::calculateVectorU(puma::Vec3<double> &point_1, puma::Vec3<double> &point_2, double bigRadius) {

    puma::Vec3<double> dir_12 = puma::Vec3<double>::direction(point_1,point_2);
    dir_12 = dir_12.norm();

    puma::Vec3<double> random_vector = getRandomPointInSphere(engine);

    puma::Vec3<double> vector_u = random_vector - dir_12 * ( random_vector.dot(dir_12) );
    vector_u = vector_u.norm();
    vector_u = vector_u * bigRadius;

    return vector_u;
}


puma::Vec3<double> StraightFlowerFiber::rotateCrossSectionalVector(puma::Vec3<double> Previous_SmallFiber_Vector, double theta, const puma::Vec3<double>& point_1, puma::Vec3<double> point_2) {

    // unit vector of rotation axis
    puma::Vec3<double> u;
    // New Small Fiber Vector
    puma::Vec3<double> q1;
    puma::Vec3<double> q2;
    double d;

    /* Step 1 */
    q1 = Previous_SmallFiber_Vector - point_1;

    u = point_2-point_1;
    u = u.norm();

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
