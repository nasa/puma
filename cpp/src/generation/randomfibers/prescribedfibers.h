#ifndef Generate_PrescribedFibers_H
#define Generate_PrescribedFibers_H

#include "generate.h"
#include "workspace.h"
#include "prng_engine.h"
#include "vector.h"
#include "fiber.h"
#include "straightcirclefiber.h"

#include <string>
#include <cstdlib>
#include <iostream>

namespace puma {
    
    bool generatePrescribedFibers(puma::Workspace *work, std::vector< double > *positions, double radius, double scale);
    bool generatePrescribedFibers(puma::Workspace *mat, std::vector< double > *positions, double radius , double scale, short val);

    bool generatePrescribedFibers(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, std::vector< double > *positions, double radius, double scale);
    bool generatePrescribedFibers(puma::Workspace *mat, puma::MatVec3<double> *dirMatrix, std::vector< double > *positions, double radius , double scale, short val);

}


class Generate_PrescribedFibers : public Generate
{
public:
    Generate_PrescribedFibers(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, std::vector< double > *positions, double radius, double scale);
    bool generate();


private:

    puma::Workspace *work;

    std::vector< double > *positions;
    double radius;
    double scale;
    int buffer{};

    StraightCircleFiber fiber;

    puma::MatVec3<double> *dirMatrix;

    bool fibersGenerationHelper();
    bool normalizeScale();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};



#endif // Generate_PrescribedFibers

