#include "mp_volumefractionhelper.h"


double MP_VolumeFractionHelper::volumeFraction(puma::Matrix<short> *gMatrix, puma::Cutoff cutoff, int numThreads) {
    return (double)count(gMatrix,cutoff,numThreads)/(double)gMatrix->size();
}

double MP_VolumeFractionHelper::volumeFraction(puma::Matrix<long> *gMatrix, puma::Cutoff cutoff, int numThreads) {
    return (double)count(gMatrix,cutoff,numThreads)/(double)gMatrix->size();
}

double MP_VolumeFractionHelper::volumeFraction(puma::Matrix<short> *gMatrix, int value, int numThreads) {
    return (double)count(gMatrix,puma::Cutoff(value,value),numThreads)/(double)gMatrix->size();
}

long MP_VolumeFractionHelper::count(puma::Matrix<short> *gMatrix, puma::Cutoff cutoff, int numThreads) {

    long size=gMatrix->size();

    int splitSize = (int)pow(omp_get_num_procs(),2);
    if(splitSize==0) splitSize=1;

    long iteration = size/splitSize;

    std::vector<long> cStart(splitSize);
    std::vector<long> cEnd(splitSize);

    for(long i=0; i<splitSize-1; i++) {
        cStart[i]=i*iteration;
        cEnd[i]=(i+1)*iteration;
    }
    cStart[splitSize-1]=(splitSize-1)*iteration;
    cEnd[splitSize-1]=size;

    std::vector<long> splitCount(splitSize,0);
    long count=0;

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long c=0; c<splitSize; c++) {
        for(long i=cStart[c]; i<cEnd[c]; i++) {
            if(gMatrix->operator ()(i)>=cutoff.first&&gMatrix->operator ()(i)<=cutoff.second) { splitCount[c]++; }
        }
    }

    for(int i=0; i<splitSize; i++) {
        count+=splitCount[i];
    }

    return count;
}

long MP_VolumeFractionHelper::count(puma::Matrix<long> *gMatrix, puma::Cutoff cutoff, int numThreads) {

    long size=gMatrix->size();

    int splitSize = (int)pow(omp_get_num_procs(),2);
    if(splitSize==0) splitSize=1;

    long iteration = size/splitSize;

    std::vector<long> cStart(splitSize);
    std::vector<long> cEnd(splitSize);

    for(long i=0; i<splitSize-1; i++) {
        cStart[i]=i*iteration;
        cEnd[i]=(i+1)*iteration;
    }
    cStart[splitSize-1]=(splitSize-1)*iteration;
    cEnd[splitSize-1]=size;

    std::vector<long> splitCount(splitSize,0);
    long count=0;

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long c=0; c<splitSize; c++) {
        for(long i=cStart[c]; i<cEnd[c]; i++) {
            if(gMatrix->operator ()(i)>=cutoff.first&&gMatrix->operator ()(i)<=cutoff.second) { splitCount[c]++; }
        }
    }

    for(int i=0; i<splitSize; i++) {
        count+=splitCount[i];
    }

    return count;
}
