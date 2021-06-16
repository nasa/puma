#include "fv_anisotropic_AMatrix.h"


FV_anisotropic_AMatrix::FV_anisotropic_AMatrix(puma::Matrix<std::vector<double>> *E_Matrix, puma::Matrix<std::vector<double>> *flux_Matrix, std::vector<long> startend, std::string sideBC, int numThreads) {
    this->E_Matrix = E_Matrix;
    this->flux_Matrix = flux_Matrix;
    this->numThreads = numThreads;
    this->X = (int)E_Matrix->X()-1;
    this->Y = (int)E_Matrix->Y()-1;
    this->Z = (int)E_Matrix->Z()-1;
    this->startend = std::move(startend);
    this->sideBC = std::move(sideBC);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool FV_anisotropic_AMatrix::computeQuadrantFluxes(puma::Matrix<double> *x) {

    //     Execute q=E*T
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long i=startend[0];i<startend[1];i++){
        std::vector<double> N(8);
        std::vector<double> q(12);
        for(long j=startend[2];j<startend[3];j++){
            for(long k=startend[4];k<startend[5];k++){

                // 12x8 * 8x1
                N[0] = (*x).at(indexAt(i-1,X),indexAt(j-1,Y),indexAt(k-1,Z));  // Cell P
                N[1] = (*x).at(indexAt(i,X),indexAt(j-1,Y),indexAt(k-1,Z));    // Cell E
                N[2] = (*x).at(indexAt(i-1,X),indexAt(j,Y),indexAt(k-1,Z));    // Cell N
                N[3] = (*x).at(indexAt(i,X),indexAt(j,Y),indexAt(k-1,Z));      // Cell NE
                N[4] = (*x).at(indexAt(i-1,X),indexAt(j-1,Y),indexAt(k,Z));    // Cell T
                N[5] = (*x).at(indexAt(i,X),indexAt(j-1,Y),indexAt(k,Z));      // Cell TE
                N[6] = (*x).at(indexAt(i-1,X),indexAt(j,Y),indexAt(k,Z));      // Cell TN
                N[7] = (*x).at(indexAt(i,X),indexAt(j,Y),indexAt(k,Z));        // Cell TNE

                // Multiply EMatrix[Z*(Y*i+j)+k] by the N vector of temperatures defined above
                for(int i2=0;i2<12;i2++) {
                    q[i2] = 0;
                    for(int j2=0;j2<8;j2++) {
                        if(!std::isnan(E_Matrix->at(i,j,k)[8*i2+j2]) && !std::isinf(E_Matrix->at(i,j,k)[8*i2+j2])) { q[i2] += E_Matrix->at(i,j,k)[8*i2+j2] * N[j2]; }
                    }
                }

                // Assigning fluxes to the flux_Matrix
                long i2 = i-1; long j2 = j-1; long k2 = k-1; // Cell P
                if(i2>=0 && j2>=0 && k2>=0 && i2<X && j2<Y && k2<Z) {
                    flux_Matrix->at(i2,j2,k2)[0] = q[0]; // qxPtne
                    flux_Matrix->at(i2,j2,k2)[0+8] = q[4]; // qyPtne
                    flux_Matrix->at(i2,j2,k2)[0+16] = q[8]; // qzPtne
                }

                i2 = i; j2 = j-1; k2 = k-1; // Cell E
                if(j2>=0 && k2>=0 && i2<X && j2<Y && k2<Z) {
                    flux_Matrix->at(i2,j2,k2)[1] = q[0]; // qxPtne
                    flux_Matrix->at(i2,j2,k2)[1+8] = q[5]; // qyEtnw
                    flux_Matrix->at(i2,j2,k2)[1+16] = q[9]; // qzEtnw
                }

                i2 = i-1; j2 = j; k2 = k-1; // Cell N
                if(i2>=0  && k2>=0 && i2<X && j2<Y && k2<Z) {
                    flux_Matrix->at(i2,j2,k2)[2] = q[1]; // qxNtse
                    flux_Matrix->at(i2,j2,k2)[2+8] = q[4]; // qyPtne
                    flux_Matrix->at(i2,j2,k2)[2+16] = q[10]; // qzNtse
                }

                i2 = i; j2 = j; k2 = k-1; // Cell NE
                if(k2>=0 && i2<X && j2<Y && k2<Z) {
                    flux_Matrix->at(i2,j2,k2)[3] = q[1]; // qxNtse
                    flux_Matrix->at(i2,j2,k2)[3+8] = q[5]; // qyEtnw
                    flux_Matrix->at(i2,j2,k2)[3+16] = q[11]; // qzNEtsw
                }

                i2 = i-1; j2 = j-1; k2 = k; // Cell T
                if(i2>=0 && j2>=0 && i2<X && j2<Y && k2<Z) {
                    flux_Matrix->at(i2,j2,k2)[4] = q[2]; // qxTbne
                    flux_Matrix->at(i2,j2,k2)[4+8] = q[6]; // qyTbne
                    flux_Matrix->at(i2,j2,k2)[4+16] = q[8]; // qzPtne
                }

                i2 = i; j2 = j-1; k2 = k; // Cell TE
                if(j2>=0 && i2<X && j2<Y && k2<Z) {
                    flux_Matrix->at(i2,j2,k2)[5] = q[2]; // qxTbne
                    flux_Matrix->at(i2,j2,k2)[5+8] = q[7]; // qyTEbnw
                    flux_Matrix->at(i2,j2,k2)[5+16] = q[9]; // qzEtnw
                }

                i2 = i-1; j2 = j; k2 = k; // Cell TN
                if(i2>=0 && i2<X && j2<Y && k2<Z) {
                    flux_Matrix->at(i2,j2,k2)[6] = q[3]; // qxTNbse
                    flux_Matrix->at(i2,j2,k2)[6+8] = q[6]; // qyTbne
                    flux_Matrix->at(i2,j2,k2)[6+16] = q[10]; // qzNtse
                }

                i2 = i; j2 = j; k2 = k; // Cell TNE
                if( i2<X && j2<Y && k2<Z) {
                    flux_Matrix->at(i2,j2,k2)[7] = q[3]; // qxTNbse
                    flux_Matrix->at(i2,j2,k2)[7+8] = q[7]; // qyTEbnw
                    flux_Matrix->at(i2,j2,k2)[7+16] = q[11]; // qzNEtsw
                }
            }
        }
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* Description: Performs A*x multiplication. A is finite differencing matrix, which is too large to
 *      explicitly create. Instead, this function applies A*x and stores the value in the residual, r
 * Inputs:
 *      x - Matrix to be multiplied by A
 *      r - Matrix to store the result of Ax
 * Outputs:
 *      The multiplication of Ax is stored into the matrix r
 */
bool FV_anisotropic_AMatrix::A_times_X(puma::Matrix<double> *x, puma::Matrix<double> *r) {

    //step 1. compute all of the fluxes in the quadrant
    computeQuadrantFluxes(&(*x));

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long i=startend[6];i<startend[7];i++) {
        for(long j=startend[8]; j<startend[9]; j++) {
            for(long k=startend[10]; k<startend[11]; k++) {
                r->at(i,j,k) = (flux_Matrix->at(i,j,k)[1] + flux_Matrix->at(i,j,k)[3] + flux_Matrix->at(i,j,k)[5] + flux_Matrix->at(i,j,k)[7]) -
                               (flux_Matrix->at(i,j,k)[0] + flux_Matrix->at(i,j,k)[2] + flux_Matrix->at(i,j,k)[4] + flux_Matrix->at(i,j,k)[6]) +
                               (flux_Matrix->at(i,j,k)[2+8] + flux_Matrix->at(i,j,k)[3+8] + flux_Matrix->at(i,j,k)[6+8] + flux_Matrix->at(i,j,k)[7+8]) -
                               (flux_Matrix->at(i,j,k)[0+8] + flux_Matrix->at(i,j,k)[1+8] + flux_Matrix->at(i,j,k)[4+8] + flux_Matrix->at(i,j,k)[5+8]) +
                               (flux_Matrix->at(i,j,k)[4+16] + flux_Matrix->at(i,j,k)[5+16] + flux_Matrix->at(i,j,k)[6+16] + flux_Matrix->at(i,j,k)[7+16]) -
                               (flux_Matrix->at(i,j,k)[0+16] + flux_Matrix->at(i,j,k)[1+16] + flux_Matrix->at(i,j,k)[2+16] + flux_Matrix->at(i,j,k)[3+16]);
            }
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


long FV_anisotropic_AMatrix::indexAt(long index, long size) {

    // Symmetric BC
    if (sideBC == "s") {
        if(index==-1)   { index=0; }
        if(index==size) { index=size-1; }
    } else { // Periodic BC
        if(index==-1)   { index=size-1; }
        if(index==size) { index=0; }
    }

    return index;
}
