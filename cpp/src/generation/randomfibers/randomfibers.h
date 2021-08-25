#ifndef GENERATE_FIBERS_H
#define GENERATE_FIBERS_H

#include "generate.h"
#include "randomfibersinput.h"
#include "medianfilter3d.h"
#include "mp_volumefraction.h"
#include "workspace.h"
#include "prng_engine.h"
#include "vector.h"
#include "fiber.h"
#include "straightcirclefiber.h"
#include "curvedcirclefiber.h"
#include "straightflowerfiber.h"
#include "curvedflowerfiber.h"

#include <iostream>
#include <cstdlib>


namespace puma {

/*! Generates random fibers based upon the given input and stores into the grayscale workspace. After generation, void is contained in
 *      grayscale values 0-127 and material is contained in grayscale values 128-255
 * \param work output workspace
 * \param input the RandomFibersInput details for the generation
 \return true if successful, false if it failing
 */
bool generateRandomFibers(puma::Workspace *work, RandomFibersInput input);

/*! Generates random fibers based upon the given input and stores into the segmented workspace. After generation, void is contained in
 *      segmented value 0 and material is contained in segmented value 1
 * \param mat output workspace
 * \param input the RandomFibersInput details for the generation
 * \param val the value to assign to the fibers
 \return true if successful, false if it failing
 */
bool generateRandomFibers(puma::Workspace *mat, RandomFibersInput input, short val);

/*! Generates random fibers based upon the given input and stores into the grayscale workspace. After generation, void is contained in
 *      grayscale values 0-127 and material is contained in grayscale values 128-255
 * \param work
 * \param dirMatrix object holds the direction vector for each point inside the material, aligned with the centerline of the cylinder
 * \param input the RandomFibersInput details for the generation
 \return true if successful, false if it failing
 */
bool generateRandomFibers(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, RandomFibersInput input);

/*! Generates random fibers based upon the given input and stores into the workspace. After generation, void is contained in
 *      segmented value 0 and material is contained in segmented value, val
 * \param mat output workspace
 * \param dirMatrix object holds the direction vector for each point inside the material, aligned with the centerline of the cylinder
 * \param input the RandomFibersInput details for the generation
  * \param val the value to assign to the fibers
  \return true if successful, false if it failing
 */
bool generateRandomFibers(puma::Workspace *mat, puma::MatVec3<double> *dirMatrix, RandomFibersInput input, short val);

}



class Generate_RandomFibers : public Generate
{
public:
    Generate_RandomFibers(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, RandomFibersInput input);
    bool generate();


private:

    RandomFibersInput input;
    puma::Workspace *work;
    sitmo::prng_engine engine;

    puma::MatVec3<double> *dirMatrix;

    Fiber *fiber{};

    double approximateNumFibers(double porosity);

    bool fibersGenerationHelper();

    bool bindFibers();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};



#endif // GENERATE_FIBERS_H

