#include "mp_volumefraction.h"


double puma::compute_VolumeFraction(Workspace *work, puma::Cutoff cutoff, int numThreads) {
    MP_VolumeFraction vf(work,cutoff,numThreads);
    return vf.compute();
}

double puma::compute_VolumeFraction(Workspace *work, int value, int numThreads) {
    MP_VolumeFraction vf(work,puma::Cutoff(value,value),numThreads);
    return vf.compute();
}

MP_VolumeFraction::MP_VolumeFraction(puma::Workspace *work, puma::Cutoff cutoff, int numThreads) {
    this->work = work;
    this-> cutoff = cutoff;
    this->numThreads = numThreads;

}

double MP_VolumeFraction::compute() {

    logInput();
    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        std::cout << "Volume Fraction Error: " <<  errorMessage << std::endl;
        return -1;
    }

    volumeFraction = MP_VolumeFractionHelper::volumeFraction(&work->matrix,cutoff,numThreads);


    logOutput();

    return volumeFraction;
}

bool MP_VolumeFraction::logInput() {
    puma::Logger *logger;
    logger = work->log;

    logger->appendLogSection("Compute Volume Fraction");
    logger->appendLogLine(" -- Inputs:");
    logger->appendLogItem("Domain Size: ");

    logger->appendLogItem("Grayscale range (inclusive): ");
    logger->appendLogItem(cutoff.first);
    logger->appendLogItem(", ");
    logger->appendLogItem(cutoff.second);
    logger->appendLogItem(work->X());
    logger->appendLogItem(", ");
    logger->appendLogItem(work->Y());
    logger->appendLogItem(", ");
    logger->appendLogItem(work->Z());
    logger->newLine();


    logger->newLine();
    logger->writeLog();

    return true;
}

bool MP_VolumeFraction::logOutput() {

    puma::Logger *logger;
    logger = work->log;

    logger->appendLogLine(" -- Output:");
    logger->appendLogItem("Volume Fraction: ");
    logger->appendLogItem(volumeFraction);
    logger->newLine();
    logger->writeLog();

    return true;
}

bool MP_VolumeFraction::errorCheck(std::string *errorMessage) {

    if(work->matrix.size() == 0) {
        *errorMessage = "Empty Grayscale Matrix";
        return false;
    }

    if(cutoff.first<=-1 || cutoff.second<=-1) {
        *errorMessage = "puma::Cutoff cannot be negative";
        return false;
    }

    if(cutoff.first > cutoff.second) {
        *errorMessage = "lowpuma::Cutoff > highpuma::Cutoff. lowpuma::Cutoff must be less than highpuma::Cutoff";
        return false;
    }

    if(cutoff.second > 32767) {
        *errorMessage = "highpuma::Cutoff > 32767. Must be <= 32767";
        return false;
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    return true;
}
