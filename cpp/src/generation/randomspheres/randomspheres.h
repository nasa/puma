#ifndef GENERATE_SPHERES_H
#define GENERATE_SPHERES_H

#include "generate.h"
#include "randomspheresinput.h"
#include "workspace.h"
#include "prng_engine.h"
#include "vector.h"
#include "sphere.h"
#include "mp_volumefraction.h"

#include <iostream>
#include <cstdlib>


namespace puma {
/*! Generates random spheres based upon the given input and stores into the grayscale workspace. After generation, void is contained in
 *      grayscale values 0-127 and material is contained in grayscale values 128-255
 \param sphereWS the output workspace
 \param input the RandomSpheresInput details for the generation
 \return true if successful, false if it failing
 */
bool generateRandomSpheres(puma::Workspace *sphereWS, RandomSpheresInput input);

/*! Generates random fibers based upon the given input and stores into the segmented workspace. After generation, void is contained in
 *      segmented value 0 and material is contained in segmented value 1
 \param sphereWS the output workspace
 \param input the RandomSpheresInput details for the generation
 \param val the value to assign to the fibers
 \return true if successful, false if it failing
 */
bool generateRandomSpheres(puma::Workspace *mat, RandomSpheresInput input, short val);
}

class Generate_RandomSpheres : public Generate
{
public:


    Generate_RandomSpheres(puma::Workspace *sphereWS, RandomSpheresInput input);
    bool generate();


private:

    RandomSpheresInput input;
    puma::Workspace *sphereWS;
    sitmo::prng_engine engine;

    Sphere *sphere{};

    double approximateNumSpheres(double porosity);

    bool fibersGenerationHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};



#endif // GENERATE_SPHERES_H

