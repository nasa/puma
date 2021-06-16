#include "isosurfacehelper.h"


bool IsoSurfaceHelper::MarchingCubes(std::vector< puma::Triangle<float> > *triangles, puma::Matrix<short> *gMatrix, double isoValue, bool closedEdges, int numThreads) {

    if( gMatrix->size()==0 ) {
        return false;
    }

    long ncellsX;
    long ncellsY;
    long ncellsZ;

    long xRange = gMatrix->X();
    long yRange = gMatrix->Y();
    long zRange = gMatrix->Z();

    long bufX = xRange;
    long bufY = yRange;
    long bufZ = zRange;

    if(closedEdges){

        bufX = xRange+2;
        bufY = yRange+2;
        bufZ = zRange+2;

        ncellsX = xRange+1;
        ncellsY = yRange+1;
        ncellsZ = zRange+1;
    }else{
        ncellsX = xRange-1;
        ncellsY = yRange-1;
        ncellsZ = zRange-1;
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    std::vector< std::vector< puma::Triangle<float> > > xTriangles(ncellsX);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0; i < ncellsX; i++) {
        int iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
        int indicies[12]={1,2,3,0,5,6,7,4,4,5,6,7};
        puma::Vec4<float> afCubeValue[8];
        puma::Vec3<float> asEdgeVertex[12];
        std::vector< puma::Vec4<float> > mcPoints2;

        for(int j=0; j < ncellsY; j++) {
            for(int k=0; k < ncellsZ; k++){

                if(!closedEdges){
                    puma::Vec4<float> vert(i,j,k,gMatrix->operator ()(i,j,k));
                    afCubeValue[0]=vert;
                    puma::Vec4<float> vert2(i+1,j,k,gMatrix->operator ()(i+1,j,k));
                    afCubeValue[1]=vert2;
                    puma::Vec4<float> vert3(i+1,j,k+1,gMatrix->operator ()(i+1,j,k+1));
                    afCubeValue[2]=vert3;
                    puma::Vec4<float> vert4(i,j,k+1,gMatrix->operator ()(i,j,k+1));
                    afCubeValue[3]=vert4;
                    puma::Vec4<float> vert5(i,j+1,k,gMatrix->operator ()(i,j+1,k));
                    afCubeValue[4]=vert5;
                    puma::Vec4<float> vert6(i+1,j+1,k,gMatrix->operator ()(i+1,j+1,k));
                    afCubeValue[5]=vert6;
                    puma::Vec4<float> vert7(i+1,j+1,k+1,gMatrix->operator ()(i+1,j+1,k+1));
                    afCubeValue[6]=vert7;
                    puma::Vec4<float> vert8(i,j+1,k+1,gMatrix->operator ()(i,j+1,k+1));
                    afCubeValue[7]=vert8;
                }else{

                    mcPoints2.resize(8);
                    for(int i2=i;i2<i+2;i2++){
                        for(int j2=j;j2<j+2;j2++){
                            for(int k2=k;k2<k+2;k2++){
                                if(i2==0||i2==bufX-1||j2==0||j2==bufY-1||k2==0||k2==bufZ-1){
                                    puma::Vec4<float> vert(i2,j2,k2,-1.7e38);
                                    mcPoints2[(i2-i)*((long)2)*((long)2) + (j2-j)*((long)2) + (k2-k)] = vert;

                                }else{
                                    puma::Vec4<float> vert(i2,j2,k2,gMatrix->operator ()((i2-1),(j2-1),(k2-1)));
                                    mcPoints2[(i2-i)*((long)2)*((long)2) + (j2-j)*((long)2) + (k2-k)] = vert;
                                }
                            }
                        }
                    }

                    afCubeValue[0] = mcPoints2[0];
                    afCubeValue[1] = mcPoints2[4];
                    afCubeValue[2] = mcPoints2[5];
                    afCubeValue[3] = mcPoints2[1];
                    afCubeValue[4] = mcPoints2[2];
                    afCubeValue[5] = mcPoints2[6];
                    afCubeValue[6] = mcPoints2[7];
                    afCubeValue[7] = mcPoints2[3];
                }




                //Find which vertices are inside of the surface and which are outside
                iFlagIndex = 0;
                for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
                {
                    if(afCubeValue[iVertexTest].v < isoValue) iFlagIndex |= 1<<iVertexTest;
                }

                //Find which edges are intersected by the surface
                iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

                //If the cube is entirely inside or outside of the surface, then there will be no intersections
                if(iEdgeFlags == 0){continue;}

                //Find the point of intersection of the surface with each edge
                //Then find the normal to the surface at those points
                int vIndex=0;
                for(iEdge = 0; iEdge < 12; iEdge++,vIndex++)
                {
                    if(vIndex>7){vIndex=iEdge-8;}
                    if(iEdgeFlags & (1<<iEdge))
                    {
                        if(fabs(afCubeValue[vIndex].v - afCubeValue[indicies[iEdge]].v) > 1e-30){
                            asEdgeVertex[iEdge] = (puma::Vec3<float> )(afCubeValue[vIndex] + (afCubeValue[indicies[iEdge]] - afCubeValue[vIndex])*((isoValue - (afCubeValue[vIndex].v))/((afCubeValue[indicies[iEdge]].v) - (afCubeValue[vIndex].v))));
                        } else{
                            asEdgeVertex[iEdge] =  (puma::Vec3<float>)afCubeValue[vIndex];
                        }
                    }
                }

                for (iTriangle=0; a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] != -1; iTriangle++) {
                    int index[3] = {a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+2], a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+1], a2iTriangleConnectionTable[iFlagIndex][3*iTriangle]};
                    puma::Triangle<float> t;

                    t.p0 = asEdgeVertex[index[0]];
                    t.p1 = asEdgeVertex[index[1]];
                    t.p2 = asEdgeVertex[index[2]];
                    t.normZ = ((((t.p1 - t.p0).cross(t.p2 - t.p0)).norm()).x)*2/sqrt(14)+((((t.p1 - t.p0).cross(t.p2 - t.p0)).norm()).y)/sqrt(14)+((((t.p1 - t.p0).cross(t.p2 - t.p0)).norm()).z)*3/sqrt(14);

                    xTriangles[i].push_back(t);
                }
            }	//END OF FOR LOOP ON Z AXIS
        }	//END OF FOR LOOP ON Y AXIS
    }	//END OF FOR LOOP ON X AXIS


    std::vector<int> startTriangles(xTriangles.size());

    long numTriangles=0;
    for(int i=0;i<ncellsX;i++){
        startTriangles[i]=numTriangles;
        numTriangles+=xTriangles[i].size();
    }
    int startPos = triangles->size();
    triangles->resize(startPos+numTriangles);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<ncellsX;i++){
        for(long j=0;j<(long)xTriangles[i].size();j++){
            triangles->operator [](startPos+startTriangles[i]+j)=xTriangles[i][j];
        }
    }

    return true;
}


bool IsoSurfaceHelper::MarchingCubes(std::vector< puma::Triangle<float> > *triangles, puma::Matrix<float> *gMatrix, double isoValue, bool closedEdges, int numThreads) {

    if( gMatrix->size()==0 ) {
        return false;
    }

    long ncellsX;
    long ncellsY;
    long ncellsZ;

    long xRange = gMatrix->X();
    long yRange = gMatrix->Y();
    long zRange = gMatrix->Z();

    long bufX = xRange;
    long bufY = yRange;
    long bufZ = zRange;

    if(closedEdges){

        bufX = xRange+2;
        bufY = yRange+2;
        bufZ = zRange+2;

        ncellsX = xRange+1;
        ncellsY = yRange+1;
        ncellsZ = zRange+1;
    }else{
        ncellsX = xRange-1;
        ncellsY = yRange-1;
        ncellsZ = zRange-1;
    }


    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    std::vector< std::vector< puma::Triangle<float> > > xTriangles(ncellsX);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0; i < ncellsX; i++) {
        int iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
        int indicies[12]={1,2,3,0,5,6,7,4,4,5,6,7};
        puma::Vec4<float> afCubeValue[8];
        puma::Vec3<float> asEdgeVertex[12];
        std::vector< puma::Vec4<float> > mcPoints2;

        for(int j=0; j < ncellsY; j++) {
            for(int k=0; k < ncellsZ; k++){

                if(!closedEdges){
                    puma::Vec4<float> vert(i,j,k,gMatrix->operator ()(i,j,k));
                    afCubeValue[0]=vert;
                    puma::Vec4<float> vert2(i+1,j,k,gMatrix->operator ()(i+1,j,k));
                    afCubeValue[1]=vert2;
                    puma::Vec4<float> vert3(i+1,j,k+1,gMatrix->operator ()(i+1,j,k+1));
                    afCubeValue[2]=vert3;
                    puma::Vec4<float> vert4(i,j,k+1,gMatrix->operator ()(i,j,k+1));
                    afCubeValue[3]=vert4;
                    puma::Vec4<float> vert5(i,j+1,k,gMatrix->operator ()(i,j+1,k));
                    afCubeValue[4]=vert5;
                    puma::Vec4<float> vert6(i+1,j+1,k,gMatrix->operator ()(i+1,j+1,k));
                    afCubeValue[5]=vert6;
                    puma::Vec4<float> vert7(i+1,j+1,k+1,gMatrix->operator ()(i+1,j+1,k+1));
                    afCubeValue[6]=vert7;
                    puma::Vec4<float> vert8(i,j+1,k+1,gMatrix->operator ()(i,j+1,k+1));
                    afCubeValue[7]=vert8;
                }else{

                    mcPoints2.resize(8);
                    for(int i2=i;i2<i+2;i2++){
                        for(int j2=j;j2<j+2;j2++){
                            for(int k2=k;k2<k+2;k2++){
                                if(i2==0||i2==bufX-1||j2==0||j2==bufY-1||k2==0||k2==bufZ-1){
                                    puma::Vec4<float> vert(i2,j2,k2,-1.7e6);
                                    mcPoints2[(i2-i)*((long)2)*((long)2) + (j2-j)*((long)2) + (k2-k)] = vert;

                                }else{
                                    puma::Vec4<float> vert(i2,j2,k2,gMatrix->operator ()((i2-1),(j2-1),(k2-1)));
                                    mcPoints2[(i2-i)*((long)2)*((long)2) + (j2-j)*((long)2) + (k2-k)] = vert;

                                }
                            }
                        }
                    }

                    afCubeValue[0] = mcPoints2[0];
                    afCubeValue[1] = mcPoints2[4];
                    afCubeValue[2] = mcPoints2[5];
                    afCubeValue[3] = mcPoints2[1];
                    afCubeValue[4] = mcPoints2[2];
                    afCubeValue[5] = mcPoints2[6];
                    afCubeValue[6] = mcPoints2[7];
                    afCubeValue[7] = mcPoints2[3];
                }




                //Find which vertices are inside of the surface and which are outside
                iFlagIndex = 0;
                for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
                {
                    if(afCubeValue[iVertexTest].v < isoValue) iFlagIndex |= 1<<iVertexTest;
                }

                //Find which edges are intersected by the surface
                iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

                //If the cube is entirely inside or outside of the surface, then there will be no intersections
                if(iEdgeFlags == 0){continue;}

                //Find the point of intersection of the surface with each edge
                //Then find the normal to the surface at those points
                int vIndex=0;
                for(iEdge = 0; iEdge < 12; iEdge++,vIndex++)
                {
                    if(vIndex>7){vIndex=iEdge-8;}
                    if(iEdgeFlags & (1<<iEdge))
                    {
                        if(fabs(afCubeValue[vIndex].v - afCubeValue[indicies[iEdge]].v) > 1e-30){
                            asEdgeVertex[iEdge] = (puma::Vec3<float> )(afCubeValue[vIndex] + (afCubeValue[indicies[iEdge]] - afCubeValue[vIndex])*((isoValue - (afCubeValue[vIndex].v))/((afCubeValue[indicies[iEdge]].v) - (afCubeValue[vIndex].v))));
                        } else{
                            asEdgeVertex[iEdge] =  (puma::Vec3<float>)afCubeValue[vIndex];
                        }
                    }
                }

                for (iTriangle=0; a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] != -1; iTriangle++) {
                    int index[3] = {a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+2], a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+1], a2iTriangleConnectionTable[iFlagIndex][3*iTriangle]};
                    puma::Triangle<float> t;

                    t.p0 = asEdgeVertex[index[0]];
                    t.p1 = asEdgeVertex[index[1]];
                    t.p2 = asEdgeVertex[index[2]];
                    t.normZ = ((((t.p1 - t.p0).cross(t.p2 - t.p0)).norm()).x)*2/sqrt(14)+((((t.p1 - t.p0).cross(t.p2 - t.p0)).norm()).y)/sqrt(14)+((((t.p1 - t.p0).cross(t.p2 - t.p0)).norm()).z)*3/sqrt(14);

                    xTriangles[i].push_back(t);
                }
            }	//END OF FOR LOOP ON Z AXIS
        }	//END OF FOR LOOP ON Y AXIS
    }	//END OF FOR LOOP ON X AXIS


    std::vector<int> startTriangles(xTriangles.size());

    long numTriangles=0;
    for(int i=0;i<ncellsX;i++){
        startTriangles[i]=numTriangles;
        numTriangles+=xTriangles[i].size();
    }
    int startPos = triangles->size();
    triangles->resize(startPos+numTriangles);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<ncellsX;i++){
        for(long j=0;j<(long)xTriangles[i].size();j++){
            triangles->operator [](startPos+startTriangles[i]+j)=xTriangles[i][j];
        }
    }

    return true;
}

long IsoSurfaceHelper::indexAt(long i, long X) {

    while( i<0 || i>X-1 ) {
        if(i < 0) {
            i = -i;
        }
        if(i > X-1) {
            i = 2*X-i-1;
        }
    }

    return i;
}

bool IsoSurfaceHelper::DownScale(puma::Matrix<short> *originalMatrix, puma::Matrix<float> *newMatrix, int factor, int numThreads) {

    if(originalMatrix->size()==0) {
        return false;
    }

    if(factor <= 0) {
        return false;
    }

    long X = originalMatrix->X();
    long Y = originalMatrix->Y();
    long Z = originalMatrix->Z();

    long scaledX = X/factor;
    long scaledY = Y/factor;
    long scaledZ = Z/factor;

    newMatrix->resize(scaledX,scaledY,scaledZ);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for( long i=0; i<scaledX; i++ ) {
        for( long j=0; j<scaledY; j++ ) {
            for( long k=0; k<scaledZ; k++ ) {
                newMatrix->operator ()(i,j,k) = originalMatrix->operator ()(i*factor,j*factor,k*factor);
            }
        }
    }

    return true;
}

bool IsoSurfaceHelper::DownScale(puma::Matrix<float> *newMatrix, int factor, int numThreads) {

    if(newMatrix->size()==0) {
        return false;
    }

    if(factor <= 0) {
        return false;
    }

    long X = newMatrix->X();
    long Y = newMatrix->Y();
    long Z = newMatrix->Z();

    long scaledX = X/factor;
    long scaledY = Y/factor;
    long scaledZ = Z/factor;

    puma::Matrix<float> scaledMatrix(scaledX,scaledY,scaledZ);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for( long i=0; i<scaledX; i++ ) {
        for( long j=0; j<scaledY; j++ ) {
            for( long k=0; k<scaledZ; k++ ) {
                scaledMatrix(i,j,k) = newMatrix->operator ()(i*factor,j*factor,k*factor);
            }
        }
    }

    newMatrix->copy(&scaledMatrix);

    return true;
}

bool IsoSurfaceHelper::DownScale(puma::Matrix<short> *newMatrix, int factor, int numThreads) {

    if(newMatrix->size()==0) {
        return false;
    }

    if(factor <= 0) {
        return false;
    }

    long X = newMatrix->X();
    long Y = newMatrix->Y();
    long Z = newMatrix->Z();

    long scaledX = X/factor;
    long scaledY = Y/factor;
    long scaledZ = Z/factor;

    puma::Matrix<short> scaledMatrix(scaledX,scaledY,scaledZ);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for( long i=0; i<scaledX; i++ ) {
        for( long j=0; j<scaledY; j++ ) {
            for( long k=0; k<scaledZ; k++ ) {
                scaledMatrix(i,j,k) = newMatrix->operator ()(i*factor,j*factor,k*factor);
            }
        }
    }

    newMatrix->copy(&scaledMatrix);

    return true;
}

puma::Cutoff IsoSurfaceHelper::InterpolateVerts(puma::Matrix<short> *originalMatrix, puma::Matrix<float> *newMatrix, puma::Cutoff cutoff, int numThreads) {

    if( originalMatrix->size()==0 ) {
        return cutoff;
    }


    newMatrix->resize(originalMatrix->X()+1,originalMatrix->Y()+1,originalMatrix->Z()+1);

    int newX = newMatrix->X();
    int newY = newMatrix->Y();
    int newZ = newMatrix->Z();

    int X = originalMatrix->X();
    int Y = originalMatrix->Y();
    int Z = originalMatrix->Z();

    puma::Cutoff newCutoff = cutoff;


    if( std::fabs( 1 - (cutoff.second - cutoff.first)) < 0.001 ) {

        puma::Matrix<float> oldMatrixCopy(X,Y,Z,0);
        for(long i=0; i<X; i++) {
            for(long j=0; j<Y; j++) {
                for (long k = 0; k < Z; k++) {
                    if(originalMatrix->operator ()(i,j,k) > cutoff.first &&  originalMatrix->operator ()(i,j,k) < cutoff.second ) {
                        oldMatrixCopy(i,j,k) = 1;
                    }
                }
            }
        }


        newCutoff.first = 0.5;
        newCutoff.second = 1.5;
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0; i<newX; i++) {
            for(long j=0; j<newY; j++) {
                for(long k=0; k<newZ; k++) {
                    newMatrix->operator ()(i,j,k) = (
                                                            oldMatrixCopy(indexAt(i,X),indexAt(j,Y),indexAt(k,Z))
                                                            +oldMatrixCopy(indexAt(i,X),indexAt(j,Y),indexAt(k-1,Z))
                                                            +oldMatrixCopy(indexAt(i,X),indexAt(j-1,Y),indexAt(k,Z))
                                                            +oldMatrixCopy(indexAt(i,X),indexAt(j-1,Y),indexAt(k-1,Z))
                                                            +oldMatrixCopy(indexAt(i-1,X),indexAt(j,Y),indexAt(k,Z))
                                                            +oldMatrixCopy(indexAt(i-1,X),indexAt(j,Y),indexAt(k-1,Z))
                                                            +oldMatrixCopy(indexAt(i-1,X),indexAt(j-1,Y),indexAt(k,Z))
                                                            +oldMatrixCopy(indexAt(i-1,X),indexAt(j-1,Y),indexAt(k-1,Z)) )/8.0;
                }
            }
        }

    } else {
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0; i<newX; i++) {
            for(long j=0; j<newY; j++) {
                for(long k=0; k<newZ; k++) {
                    newMatrix->operator ()(i,j,k) = (
                                                            (float)originalMatrix->operator ()(indexAt(i,X),indexAt(j,Y),indexAt(k,Z))
                                                            +originalMatrix->operator ()(indexAt(i,X),indexAt(j,Y),indexAt(k-1,Z))
                                                            +originalMatrix->operator ()(indexAt(i,X),indexAt(j-1,Y),indexAt(k,Z))
                                                            +originalMatrix->operator ()(indexAt(i,X),indexAt(j-1,Y),indexAt(k-1,Z))
                                                            +originalMatrix->operator ()(indexAt(i-1,X),indexAt(j,Y),indexAt(k,Z))
                                                            +originalMatrix->operator ()(indexAt(i-1,X),indexAt(j,Y),indexAt(k-1,Z))
                                                            +originalMatrix->operator ()(indexAt(i-1,X),indexAt(j-1,Y),indexAt(k,Z))
                                                            +originalMatrix->operator ()(indexAt(i-1,X),indexAt(j-1,Y),indexAt(k-1,Z)) )/8.0;
                }
            }
        }
    }





    return newCutoff;
}

bool IsoSurfaceHelper::InterpolateVerts(puma::Matrix<short> *originalMatrix, puma::Matrix<float> *newMatrix, int numThreads) {

    if( originalMatrix->size()==0 ) {
        return false;
    }

    newMatrix->resize(originalMatrix->X()+1,originalMatrix->Y()+1,originalMatrix->Z()+1);

    int newX = newMatrix->X();
    int newY = newMatrix->Y();
    int newZ = newMatrix->Z();

    int X = originalMatrix->X();
    int Y = originalMatrix->Y();
    int Z = originalMatrix->Z();

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long i=0; i<newX; i++) {
        for(long j=0; j<newY; j++) {
            for(long k=0; k<newZ; k++) {
                newMatrix->operator ()(i,j,k) = (
                        (float)originalMatrix->operator ()(indexAt(i,X),indexAt(j,Y),indexAt(k,Z))
                            +originalMatrix->operator ()(indexAt(i,X),indexAt(j,Y),indexAt(k-1,Z))
                            +originalMatrix->operator ()(indexAt(i,X),indexAt(j-1,Y),indexAt(k,Z))
                            +originalMatrix->operator ()(indexAt(i,X),indexAt(j-1,Y),indexAt(k-1,Z))
                            +originalMatrix->operator ()(indexAt(i-1,X),indexAt(j,Y),indexAt(k,Z))
                            +originalMatrix->operator ()(indexAt(i-1,X),indexAt(j,Y),indexAt(k-1,Z))
                            +originalMatrix->operator ()(indexAt(i-1,X),indexAt(j-1,Y),indexAt(k,Z))
                            +originalMatrix->operator ()(indexAt(i-1,X),indexAt(j-1,Y),indexAt(k-1,Z)) )/8.0;
            }
        }
    }

    return true;
}

bool IsoSurfaceHelper::InterpolateVertsSegmented(puma::Matrix<short> *originalMatrix, puma::Matrix<short> *newMatrix, int numThreads) {

    if( originalMatrix->size()==0 ) {
        return false;
    }

    newMatrix->resize(originalMatrix->X()+1,originalMatrix->Y()+1,originalMatrix->Z()+1);

    int newX = newMatrix->X();
    int newY = newMatrix->Y();
    int newZ = newMatrix->Z();

    int X = originalMatrix->X();
    int Y = originalMatrix->Y();
    int Z = originalMatrix->Z();

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long i=0; i<newX; i++) {
        for(long j=0; j<newY; j++) {
            for(long k=0; k<newZ; k++) {
                newMatrix->operator ()(i,j,k) = (
                            originalMatrix->operator ()(indexAt(i,X),indexAt(j,Y),indexAt(k,Z))
                            +originalMatrix->operator ()(indexAt(i,X),indexAt(j,Y),indexAt(k-1,Z))
                            +originalMatrix->operator ()(indexAt(i,X),indexAt(j-1,Y),indexAt(k,Z))
                            +originalMatrix->operator ()(indexAt(i,X),indexAt(j-1,Y),indexAt(k-1,Z))
                            +originalMatrix->operator ()(indexAt(i-1,X),indexAt(j,Y),indexAt(k,Z))
                            +originalMatrix->operator ()(indexAt(i-1,X),indexAt(j,Y),indexAt(k-1,Z))
                            +originalMatrix->operator ()(indexAt(i-1,X),indexAt(j-1,Y),indexAt(k,Z))
                            +originalMatrix->operator ()(indexAt(i-1,X),indexAt(j-1,Y),indexAt(k-1,Z)) );
            }
        }
    }

    return true;
}
