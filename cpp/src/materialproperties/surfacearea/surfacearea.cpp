#include "surfacearea.h"


std::pair<double, double> puma::compute_SurfaceAreaMarchingCubes(Workspace *work, puma::Cutoff cutoff, bool interpVerts, int numThreads) {
    SurfaceArea surfArea(work,cutoff,interpVerts,numThreads);
    return surfArea.compute();
}

std::pair<double, double> puma::compute_SurfaceAreaVoxels(Workspace *work, puma::Cutoff cutoff, int numThreads) {
    SurfaceArea surfArea(work,cutoff,numThreads);
    return surfArea.compute();
}

SurfaceArea::SurfaceArea(puma::Workspace *work, puma::Cutoff cutoff, bool interpVerts, int numThreads) {
    this->work = work;
    this->cutoff = cutoff;
    this->interpVerts = interpVerts;
    this->voxel = false;
    this->numThreads = numThreads;
}

SurfaceArea::SurfaceArea(puma::Workspace *work,puma::Cutoff cutoff, int numThreads) {
    this->work = work;
    this->cutoff = cutoff;
    this->voxel = true;
    this->interpVerts = false;
    this->numThreads = numThreads;
}

std::pair<double,double> SurfaceArea::compute() {
    logInput();
    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        std::cout << "Surface Area Error: " <<  errorMessage << std::endl;
        return std::pair<double,double>{-1,-1};
    }

    surfaceAreaHelper();

    logOutput();

    return surfaceArea;
}

bool SurfaceArea::surfaceAreaHelper() {

    if(voxel) {
        getVolume(false);
        getSurfaceArea_Voxel();
    }
    else {
        {
            getVolume(!interpVerts);
            getSurfaceArea_MarchingCubes();
        }
    }

    return true;
}

void SurfaceArea::getVolume(bool smallDomain) {
    if(smallDomain) {
        volume = (double)( (work->X()-1) * (work->Y()-1) * (work->Z()-1) ) * std::pow(work->voxelLength,3);
    }
    else {
        volume = (double)( (work->X()) * (work->Y()) * (work->Z()) ) * std::pow(work->voxelLength,3);
    }
}

void SurfaceArea::getSurfaceArea_MarchingCubes() {
    puma::TriangleVector tris;
    puma::isosurface_MarchingCubes(&tris,work,cutoff,false,1,interpVerts,numThreads);

    std::vector<double> bufferArray(tris.size());
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long i=0;i<(long)tris.size();i++){
        double u1,u2,u3,v1,v2,v3;
        u1=tris[i].p1.x-tris[i].p0.x;
        u2=tris[i].p1.y-tris[i].p0.y;
        u3=tris[i].p1.z-tris[i].p0.z;
        v1=tris[i].p2.x-tris[i].p0.x;
        v2=tris[i].p2.y-tris[i].p0.y;
        v3=tris[i].p2.z-tris[i].p0.z;
        bufferArray[i]=(sqrt((u2*v3-u3*v2)*(u2*v3-u3*v2)+(u3*v1-u1*v3)*(u3*v1-u1*v3)+(u1*v2-u2*v1)*(u1*v2-u2*v1))/2);
        bufferArray[i]*=work->voxelLength*work->voxelLength;
    }


    double sA = accumulate(bufferArray.begin(), bufferArray.end(),0.0);

    surfaceArea.first = sA;
    surfaceArea.second = sA/volume;

}

void SurfaceArea::getSurfaceArea_Voxel() {

    long double sA=0;
    for(long i=0;i<work->X();i++){
        for(long j=0;j<work->Y();j++){
            for(long k=0;k<work->Z();k++){
                if(work->operator () (i,j,k) >= cutoff.first && work->operator () (i,j,k) <= cutoff.second){
                    if(i!=0){ if(work->operator () (i-1,j,k) < cutoff.first || work->operator () (i-1,j,k) > cutoff.second){ sA+=work->voxelLength*work->voxelLength;}}
                    if(j!=0){ if(work->operator () (i,j-1,k) < cutoff.first || work->operator () (i,j-1,k) > cutoff.second){ sA+=work->voxelLength*work->voxelLength;}}
                    if(k!=0){ if(work->operator () (i,j,k-1) < cutoff.first || work->operator () (i,j,k-1) > cutoff.second){ sA+=work->voxelLength*work->voxelLength;}}
                    if(i!=work->X()-1){ if(work->operator () (i+1,j,k) < cutoff.first || work->operator () (i+1,j,k) > cutoff.second){ sA+=work->voxelLength*work->voxelLength;}}
                    if(j!=work->Y()-1){ if(work->operator () (i,j+1,k) < cutoff.first || work->operator () (i,j+1,k) > cutoff.second){ sA+=work->voxelLength*work->voxelLength;}}
                    if(k!=work->Z()-1){ if(work->operator () (i,j,k+1) < cutoff.first || work->operator () (i,j,k+1) > cutoff.second){ sA+=work->voxelLength*work->voxelLength;}}
                }
            }
        }
    }

    surfaceArea.first = sA;
    surfaceArea.second = sA/volume;

}

bool SurfaceArea::logInput() {
    puma::Logger *logger = work->log;

    logger->appendLogSection("Compute Surface Area");
    logger->appendLogLine(" -- Inputs:");
    logger->appendLogItem("interior Size: ");

    logger->appendLogItem(work->X());
    logger->appendLogItem(", ");
    logger->appendLogItem(work->Y());
    logger->appendLogItem(", ");
    logger->appendLogItem(work->Z());
    logger->newLine();

    logger->appendLogItem("Method: ");
    if (voxel)
    {
        logger->appendLogItem("Voxel Based");
    }
    else
    {
        logger->appendLogItem("Triangle Based");
        logger->newLine();
        logger->appendLogItem("Interpolated Vertices: ");
        if (interpVerts)
        {
            logger->appendLogItem("Yes");
        }
        else
        {
            logger->appendLogItem("No");
        }
    }
    logger->newLine();
    logger->appendLogItem("Low puma::Cutoff: ");
    logger->appendLogItem(cutoff.first);
    logger->appendLogItem(", High puma::Cutoff: ");
    logger->appendLogItem(cutoff.second);
    logger->newLine();
    logger->appendLogItem("Number of Threads: ");
    logger->appendLogItem(numThreads);

    logger->newLine();
    logger->writeLog();

    return true;
}

bool SurfaceArea::logOutput() {
    puma::Logger *logger = work->log;

    logger->appendLogLine(" -- Output:");
    logger->appendLogItem("Surface Area: ");
    logger->appendLogItem(surfaceArea.first);
    logger->newLine();
    logger->appendLogItem("Specific Surface Area: ");
    logger->appendLogItem(surfaceArea.second);

    logger->newLine();
    logger->writeLog();
    return true;
}

bool SurfaceArea::errorCheck(std::string *errorMessage) {
    if(work->matrix.size() == 0) {
        *errorMessage = "Empty Material Matrix";
        return false;
    }

    if(cutoff.first<=-1 || cutoff.second<=-1) {
        *errorMessage = "puma::Cutoff cannot be negative";
        return false;
    }

    if(cutoff.second > 32767) {
        *errorMessage = "value > 32767. Must be <= 255";
        return false;
    }

    if(cutoff.first > cutoff.second) {
        *errorMessage = "Low cutoff must be less than high cutoff";
        return false;
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }
    return true;
}



//SurfaceArea_MarchignCubes;

//SurfaceArea_Voxel

