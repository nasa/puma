#include "tortuosity_unified.h"


puma::TortuosityUnifiedReturn puma::compute_TortuosityUnified_particle(puma::Workspace *grayWS, puma::Cutoff cutoff, int numParticles, int randomSeed, double totalLength, bool print, int numThreads) {

    if (cutoff.first < -0.5 || cutoff.second > 32767.5 || cutoff.first > cutoff.second) {
        grayWS->printer->print("DiffusivityError: Invalid Cutoff Ranges");
        puma::TortuosityUnifiedReturn returnVals;
        returnVals.tortuosity = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.porosity = -1;
        returnVals.characteristicLength = puma::Vec3<double>(-1.,-1.,-1.);
        return returnVals;
    }

    // Intercept length here is not the same definition as in the isosurface tortuosity. However, it does provide a rough estimate of the mean pore diameter
    puma::Vec3<double> porediameter = puma::compute_MeanInterceptLength(grayWS, cutoff, 0);
    double poreDiameter_voxel = std::max(std::max(porediameter.x, porediameter.y), porediameter.z) / grayWS->voxelLength;

    double knudsenNumber = 0.01; // Chosen because this is in continuum
    double meanFreePath = poreDiameter_voxel * knudsenNumber;  // setting the mean free path such that the simulation is continuum
    double velocity = 500.; // Arbitrary since it's canceled out

    std::cout << "Max mean pore diameter (voxels): " << poreDiameter_voxel << std::endl;
    std::cout << "Continuum mean free path (voxels): " << meanFreePath << std::endl;

    // First doing a continuum particle solve
    TortuosityParticles_Isosurface continuumSolver(grayWS,cutoff,numParticles,meanFreePath,velocity,randomSeed,totalLength,numThreads);
    puma::TortuosityReturn continuumReturn = continuumSolver.compute();
    puma::Vec3<double> tortuosity = continuumReturn.tortuosity;  // Tortuosity value is the continuum tortuosity


    // Next doing the rarefied particle solve
    meanFreePath = totalLength; // to force into free-molecular
    TortuosityParticles_Isosurface rarefiedSolver(grayWS,cutoff,numParticles,meanFreePath,velocity,randomSeed,totalLength,numThreads);
    puma::TortuosityReturn rarefiedReturn = rarefiedSolver.compute();

    double porosity = puma::compute_VolumeFraction(grayWS,cutoff, numThreads);
    std::cout << "Porosity: " << porosity << std::endl;

    puma::TortuosityUnifiedReturn returnVals;
    returnVals.tortuosity = tortuosity;
    returnVals.porosity = porosity;
    returnVals.legacy_knudsen_tortuosity = rarefiedReturn.tortuosity;
    returnVals.legacy_meanInterceptLength = rarefiedReturn.meanInterceptLength;
    returnVals.characteristicLength =  tortuosity * rarefiedReturn.diffusionCoefficient / ( porosity * velocity ) * 3.;

    return returnVals;
}

puma::TortuosityUnifiedReturn puma::compute_TortuosityUnified(puma::Workspace *grayWS, puma::Matrix<double> *C, puma::Cutoff cutoff,
                                             const std::string& sideBC, const std::string& solverType, double solverTol,
                                             int solverMaxIt , int numParticles,
                                             int randomSeed, double totalLength, bool print, int numThreads) {

    if (cutoff.first < -0.5 || cutoff.second > 32767.5 || cutoff.first > cutoff.second) {
        grayWS->printer->print("DiffusivityError: Invalid Cutoff Ranges");
        puma::TortuosityUnifiedReturn returnVals;
        returnVals.tortuosity = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.porosity = -1;
        returnVals.characteristicLength = puma::Vec3<double>(-1.,-1.,-1.);
        return returnVals;
    }

    puma::Workspace segWS(grayWS->getShape(),grayWS->log);
    segWS.setPrinter(grayWS->printer);
    segWS.setMaterialID(grayWS,cutoff,1);
    FV_Tortuosity condX(&segWS,C,1,sideBC,solverType,'x',solverTol,solverMaxIt,print,numThreads);
    puma::Vec3<double> tortuosityX = condX.compute();
    FV_Tortuosity condY(&segWS,C,1,sideBC,solverType,'y',solverTol,solverMaxIt,print,numThreads);
    puma::Vec3<double> tortuosityY = condY.compute();
    FV_Tortuosity condZ(&segWS,C,1,sideBC,solverType,'z',solverTol,solverMaxIt,print,numThreads);
    puma::Vec3<double> tortuosityZ = condZ.compute();

    std::cout << tortuosityX << " " << tortuosityY << " " << tortuosityZ << std::endl;

    puma::Vec3<double> tortuosity(tortuosityX.x, tortuosityY.y, tortuosityZ.z);

    double velocity = 500.;
    double meanFreePath = totalLength; // to force into free-molecular

    TortuosityParticles_Isosurface solver(grayWS,cutoff,numParticles,meanFreePath,velocity,randomSeed,totalLength,numThreads);
    puma::TortuosityReturn rarefiedReturn = solver.compute();

    double porosity = puma::compute_VolumeFraction(grayWS,cutoff, numThreads);
    std::cout << "Porosity: " << porosity << std::endl;

    puma::TortuosityUnifiedReturn returnVals;
    returnVals.tortuosity = tortuosity;
    returnVals.porosity = porosity;
    returnVals.legacy_knudsen_tortuosity = rarefiedReturn.tortuosity;
    returnVals.legacy_meanInterceptLength = rarefiedReturn.meanInterceptLength;
    returnVals.characteristicLength =  tortuosity * rarefiedReturn.diffusionCoefficient / ( porosity * velocity ) * 3.;

    return returnVals;
}

puma::Vec3<double> puma::compute_EffectiveDiffusionCoefficient(const puma::Vec3<double>& tortuosity, puma::Vec3<double> characteristicLength, double porosity, puma::Vec3<double> bulkDiffusionCoefficient, double meanThermalVelocity){

    return puma::Vec3<double>(1,1,1) * porosity / 3. / tortuosity * (characteristicLength * meanThermalVelocity * bulkDiffusionCoefficient) / (bulkDiffusionCoefficient + characteristicLength * meanThermalVelocity / 3.);
}


