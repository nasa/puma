#include "ej_AMatrix.h"


EJ_AMatrix::EJ_AMatrix(puma::Matrix<double> *kMat, char dir, int numThreads)
{
    this->kMat = kMat;
    this->dir = dir;
    this->numThreads = numThreads;
    this->X=kMat->X();
    this->Y=kMat->Y();
    this->Z=kMat->Z();
    H = (dir=='x')*1.0/X +(dir=='y')*1.0/Y+(dir=='z')*1.0/Z;
    N = X*Y*Z;
    pi = 3.14159265358979323846264338327950288;
}

bool EJ_AMatrix::A_times_X(puma::Matrix<double> *x, puma::Matrix<double> *r)
{

    Psi_times_X(x,&rN);
    AInv_times_X(&rN);
    D_times_X(&rN,r);
    X_minus_R(x,r);

    return true;
}

void EJ_AMatrix::computeHomogenizedSolution(puma::Matrix<double> *T, puma::Matrix<double> *J)
{
    T->resize(X,Y,Z,0);
    omp_set_num_threads(numThreads);
    int start = 0;
    int end = numXInterfaces;
#pragma omp parallel for
    for (int l = start; l < end; l++)
    {
        (*T)(PsiRowInds[l]) += 0.5/H*(*J)(PsiColDRowInds[l]);
    }
    start = end;
    end = start + numXInterfaces;
#pragma omp parallel for
    for (int l = start; l < end; l++)
    {
        (*T)(PsiRowInds[l]) += 0.5/H*(*J)(PsiColDRowInds[l]);
    }
    start = end;
    end = start + numYInterfaces;
#pragma omp parallel for
    for (int l = start; l < end; l++)
    {
        (*T)(PsiRowInds[l]) += 0.5/H*(*J)(PsiColDRowInds[l]);
    }
    start = end;
    end = start + numYInterfaces;
#pragma omp parallel for
    for (int l = start; l < end; l++)
    {
        (*T)(PsiRowInds[l]) += 0.5/H*(*J)(PsiColDRowInds[l]);
    }
    start = end;
    end = start + numZInterfaces;
#pragma omp parallel for
    for (int l = start; l < end; l++)
    {
        (*T)(PsiRowInds[l]) += 0.5/H*(*J)(PsiColDRowInds[l]);
    }
    start = end;
    end = start + numZInterfaces;
#pragma omp parallel for
    for (int l = start; l < end; l++)
    {
        (*T)(PsiRowInds[l]) += 0.5/H*(*J)(PsiColDRowInds[l]);
    }
    AInv_times_X(T);
}

double EJ_AMatrix::getValueAt(int i, int j, int k, puma::Matrix<double> *x) {
    if (i == -1) {i = X-1;}
    else if (i == X) {i = 0;}
    if (j == -1) {j = Y-1;}
    else if (j == Y) {j = 0;}
    if (k == -1) {k = Z-1;}
    else if (k == Z) {k = 0;}
    return (*x)(i,j,k);
}

puma::Vec3<double> EJ_AMatrix::computeConductivity(puma::Matrix<double> *T, puma::Matrix<double> *J)
{

    puma::Matrix<double> BSX(X,Y,Z);
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                BSX(i,j,k) = (*kMat)(i,j,k)*((getValueAt(i+1,j,k,T) - getValueAt(i-1,j,k,T))/2/H + 1.0*(dir=='x'));
            }
        }
    }

    if (indexXInterfacesBack.size() != 0) {
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (int l = 0; l < numXInterfaces; l++) {
            BSX(indexXInterfacesBack[l]) -= (*kMat)(indexXInterfacesBack[l])*(*J)(jumpCountsX[l])/4;
        }
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (int l = 0; l < numXInterfaces; l++) {
            BSX(indexXInterfacesFront[l]) += (*kMat)(indexXInterfacesFront[l])*(*J)(jumpCountsX[l])/4;
        }

    }
    double B_X = 0;
    for (int n = 0; n < N; n++) {B_X += BSX(n);}
    B_X = B_X/N;

    puma::Matrix<double> BSY(X,Y,Z);
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                BSY(i,j,k) = (*kMat)(i,j,k)*((getValueAt(i,j+1,k,T) - getValueAt(i,j-1,k,T))/2/H + 1.0*(dir=='y'));
            }
        }
    }
    if (indexYInterfacesBack.size() != 0) {
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (int l = 0; l < numYInterfaces; l++) {
            BSY(indexYInterfacesBack[l]) -= (*kMat)(indexYInterfacesBack[l])*(*J)(jumpCountsY[l])/4;
        }
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (int l = 0; l < numYInterfaces; l++) {
            BSY(indexYInterfacesFront[l]) += (*kMat)(indexYInterfacesFront[l])*(*J)(jumpCountsY[l])/4;
        }

    }
    double B_Y = 0;
    for (int n = 0; n < N; n++) {B_Y += BSY(n);}
    B_Y = B_Y/N;

    puma::Matrix<double> BSZ(X,Y,Z);
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                BSZ(i,j,k) = (*kMat)(i,j,k)*((getValueAt(i,j,k+1,T) - getValueAt(i,j,k-1,T))/2/H + 1.0*(dir=='z'));
            }
        }
    }
    if (indexZInterfacesBack.size() != 0) {
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (int l = 0; l < numZInterfaces; l++) {
            BSZ(indexZInterfacesBack[l]) -= (*kMat)(indexZInterfacesBack[l])*(*J)(jumpCountsZ[l])/4;
        }
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (int l = 0; l < numZInterfaces; l++) {
            BSZ(indexZInterfacesFront[l]) += (*kMat)(indexZInterfacesFront[l])*(*J)(jumpCountsZ[l])/4;
        }

    }
    double B_Z = 0;
    for (int n = 0; n < N; n++) {B_Z += BSZ(n);}
    B_Z = B_Z/N;

    void fftw_cleanup_threads(void);
    return puma::Vec3<double>(B_X,B_Y,B_Z);
}

bool EJ_AMatrix::setup()
{
    preallocateMatrices();
    findInterfaceFrontsAndBacks();
    findNumInterfaces();
    setupJumpCounts();
    setupBMatrices();
    setupF();
    setupPDM();
    setupSparseMatrixIndexing();

    return true;
}

bool EJ_AMatrix::free() {
    fftw_free(inData);
    fftw_free(fourierData);
    fftw_free(outData);

    fftw_destroy_plan(pf);
    fftw_destroy_plan(pb);

    return true;
}

long EJ_AMatrix::getNumInterfaces()
{
    return numInterfacesTotal;
}

void EJ_AMatrix::getF(puma::Matrix<double> *FOut)
{
    FOut->resize(F.size(),1,1);
    omp_set_num_threads(numThreads);
    // #pragma omp parallel for
    for (long m = 0; m < (long)F.size(); m++)
    {
        (*FOut)(m) = F[m];
    }
}

void EJ_AMatrix::findInterfaceFrontsAndBacks()
{
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                long frontIndex = Z*(Y*i + j) + k;
                long backIndex;
                if (i == 0) {
                    backIndex = Z*(Y*(X-1) + (j)) + (k);
                }
                else {
                    backIndex = Z*(Y*(i-1) + (j)) + (k);
                }

                if ((*kMat)(frontIndex) != (*kMat)(backIndex)){
                    indexXInterfacesFront.push_back(frontIndex);
                    indexXInterfacesBack.push_back(backIndex);
                }
            }
        }
    }
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                long frontIndex = Z*(Y*i + j) + k;
                long backIndex;
                if (j == 0) {
                    backIndex = Z*(Y*(i) + (Y-1)) + (k);
                }
                else {
                    backIndex = Z*(Y*(i) + (j-1)) + (k);
                }

                if ((*kMat)(frontIndex) != (*kMat)(backIndex)){
                    indexYInterfacesFront.push_back(frontIndex);
                    indexYInterfacesBack.push_back(backIndex);
                }
            }
        }
    }
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                long frontIndex = Z*(Y*i + j) + k;
                long backIndex;
                if (k == 0) {
                    backIndex = Z*(Y*(i) + (j)) + (Z-1);
                }
                else {
                    backIndex = Z*(Y*(i) + (j)) + (k-1);
                }

                if ((*kMat)(frontIndex) != (*kMat)(backIndex)){
                    indexZInterfacesFront.push_back(frontIndex);
                    indexZInterfacesBack.push_back(backIndex);
                }
            }
        }
    }
}

void EJ_AMatrix::findNumInterfaces()
{
    numXInterfaces = indexXInterfacesFront.size();
    numYInterfaces = indexYInterfacesFront.size();
    numZInterfaces = indexZInterfacesFront.size();
    numInterfacesTotal = numXInterfaces+numYInterfaces+numZInterfaces;
}

void EJ_AMatrix::preallocateMatrices(){
    // Z dimension cut in half in Fourier Space
    ZFourierSpace = Z/2+1;

    int fftw_init_threads(void);

    rN.resize(X,Y,Z);

    // FFTW Arrays
    inData = fftw_alloc_real(N);
    outData = fftw_alloc_real(N);
    fourierData = fftw_alloc_complex(X*Y*ZFourierSpace);

}

void EJ_AMatrix::setupJumpCounts()
{
    jumpCountsX.resize(numXInterfaces);
    jumpCountsY.resize(numYInterfaces);
    jumpCountsZ.resize(numZInterfaces);
    for (long n = 0; n < numXInterfaces; n++) {
        jumpCountsX[n] = n;
    }
    for (long n = 0; n < numYInterfaces; n++) {
        jumpCountsY[n] = numXInterfaces+n;
    }
    for (long n = 0; n < numZInterfaces; n++) {
        jumpCountsZ[n] = numYInterfaces+numXInterfaces+n;
    }
}

void EJ_AMatrix::setupBMatrices()
{
    BI.resize(numXInterfaces);
    BJ.resize(numYInterfaces);
    BK.resize(numZInterfaces);
    for (long n = 0; n < numZInterfaces; n++) {
        BK[n] = ((*kMat)(indexZInterfacesFront[n])-(*kMat)(indexZInterfacesBack[n]))/((*kMat)(indexZInterfacesFront[n])+(*kMat)(indexZInterfacesBack[n]));
    }
    for (long n = 0; n < numYInterfaces; n++) {
        BJ[n] = ((*kMat)(indexYInterfacesFront[n])-(*kMat)(indexYInterfacesBack[n]))/((*kMat)(indexYInterfacesFront[n])+(*kMat)(indexYInterfacesBack[n]));
    }
    for (long n = 0; n < numXInterfaces; n++) {
        BI[n] = ((*kMat)(indexXInterfacesFront[n])-(*kMat)(indexXInterfacesBack[n]))/((*kMat)(indexXInterfacesFront[n])+(*kMat)(indexXInterfacesBack[n]));
    }

    BINegative.resize(numXInterfaces); //These are just making alternate, negative-valued std::vectors for defining 'D'
    BJNegative.resize(numYInterfaces);
    BKNegative.resize(numZInterfaces);
    for (long n = 0; n < numXInterfaces; n++) {
        BINegative[n] = -BI[n];
    }
    for (long n = 0; n < numYInterfaces; n++) {
        BJNegative[n] = -BJ[n];
    }
    for (long n = 0; n < numZInterfaces; n++) {
        BKNegative[n] = -BK[n];
    }
}

void EJ_AMatrix::setupF()
{
    for (long n = 0; n < numXInterfaces; n++) {F.push_back(-2.0*(dir=='x')*BI[n]);}
    for (long n = 0; n < numYInterfaces; n++) {F.push_back(-2.0*(dir=='y')*BJ[n]);}
    for (long n = 0; n < numZInterfaces; n++) {F.push_back(-2.0*(dir=='z')*BK[n]);}
}

void EJ_AMatrix::setupPDM()
{
    PDM.resize(N);
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < Z; k++) {
                int n = Z*(Y*i + j) + k;
                PDM[n] = 2*((cos((2*pi/X)*i)-1) + (cos((2*pi/Y)*j)-1) + (cos((2*pi/Z)*k)-1))/H/H;
            }
        }
    }
    PDM[0] = 1;
}

void EJ_AMatrix::setupSparseMatrixIndexing()
{
    PsiColDRowInds.reserve(2*(jumpCountsX.size() + jumpCountsY.size() + jumpCountsZ.size())); // preallocate memory
    PsiColDRowInds.insert(PsiColDRowInds.end(), jumpCountsX.begin(), jumpCountsX.end() );
    PsiColDRowInds.insert(PsiColDRowInds.end(), jumpCountsX.begin(), jumpCountsX.end() );
    PsiColDRowInds.insert(PsiColDRowInds.end(), jumpCountsY.begin(), jumpCountsY.end() );
    PsiColDRowInds.insert(PsiColDRowInds.end(), jumpCountsY.begin(), jumpCountsY.end() );
    PsiColDRowInds.insert(PsiColDRowInds.end(), jumpCountsZ.begin(), jumpCountsZ.end() );
    PsiColDRowInds.insert(PsiColDRowInds.end(), jumpCountsZ.begin(), jumpCountsZ.end() );

    PsiRowInds.reserve(indexXInterfacesBack.size() + indexXInterfacesFront.size() + indexYInterfacesBack.size() + indexYInterfacesFront.size() + indexZInterfacesBack.size() + indexZInterfacesFront.size());
    PsiRowInds.insert(PsiRowInds.end(), indexXInterfacesBack.begin(), indexXInterfacesBack.end() );
    PsiRowInds.insert(PsiRowInds.end(), indexXInterfacesFront.begin(), indexXInterfacesFront.end() );
    PsiRowInds.insert(PsiRowInds.end(), indexYInterfacesBack.begin(), indexYInterfacesBack.end() );
    PsiRowInds.insert(PsiRowInds.end(), indexYInterfacesFront.begin(), indexYInterfacesFront.end() );
    PsiRowInds.insert(PsiRowInds.end(), indexZInterfacesBack.begin(), indexZInterfacesBack.end() );
    PsiRowInds.insert(PsiRowInds.end(), indexZInterfacesFront.begin(), indexZInterfacesFront.end() );

    DColInds.reserve(indexXInterfacesBack.size() + indexXInterfacesFront.size() + indexYInterfacesBack.size() + indexYInterfacesFront.size() + indexZInterfacesBack.size() + indexZInterfacesFront.size()); // preallocate memory
    DColInds.insert(DColInds.end(), indexXInterfacesFront.begin(), indexXInterfacesFront.end() );
    DColInds.insert(DColInds.end(), indexXInterfacesBack.begin(), indexXInterfacesBack.end() );
    DColInds.insert(DColInds.end(), indexYInterfacesFront.begin(), indexYInterfacesFront.end() );
    DColInds.insert(DColInds.end(), indexYInterfacesBack.begin(), indexYInterfacesBack.end() );
    DColInds.insert(DColInds.end(), indexZInterfacesFront.begin(), indexZInterfacesFront.end() );
    DColInds.insert(DColInds.end(), indexZInterfacesBack.begin(), indexZInterfacesBack.end() );

    BForDMatrix.reserve(2*(BI.size() + BJ.size() + BK.size())); // preallocate memory
    BForDMatrix.insert(BForDMatrix.end(), BI.begin(), BI.end() );
    BForDMatrix.insert(BForDMatrix.end(), BINegative.begin(), BINegative.end() );
    BForDMatrix.insert(BForDMatrix.end(), BJ.begin(), BJ.end() );
    BForDMatrix.insert(BForDMatrix.end(), BJNegative.begin(), BJNegative.end() );
    BForDMatrix.insert(BForDMatrix.end(), BK.begin(), BK.end() );
    BForDMatrix.insert(BForDMatrix.end(), BKNegative.begin(), BKNegative.end() );
}

void EJ_AMatrix::Psi_times_X(puma::Matrix<double> *x, puma::Matrix<double> *r)
{
    omp_set_num_threads(numThreads);
    r->set(0);
    int start = 0;
    int end = numXInterfaces;
#pragma omp parallel for
    for (int l = start; l < end; l++) {
        (*r)(PsiRowInds[l]) += (-0.5/H)*(*x)(PsiColDRowInds[l]);
    }
    start = end;
    end = start + numXInterfaces;
#pragma omp parallel for
    for (int l = start; l < end; l++) {
        (*r)(PsiRowInds[l]) += (-0.5/H)*(*x)(PsiColDRowInds[l]);
    }
    start = end;
    end = start + numYInterfaces;
#pragma omp parallel for
    for (int l = start; l < end; l++) {
        (*r)(PsiRowInds[l]) += (-0.5/H)*(*x)(PsiColDRowInds[l]);
    }
    start = end;
    end = start + numYInterfaces;
#pragma omp parallel for
    for (int l = start; l < end; l++) {
        (*r)(PsiRowInds[l]) += (-0.5/H)*(*x)(PsiColDRowInds[l]);
    }
    start = end;
    end = start + numZInterfaces;
#pragma omp parallel for
    for (int l = start; l < end; l++) {
        (*r)(PsiRowInds[l]) += (-0.5/H)*(*x)(PsiColDRowInds[l]);
    }
    start = end;
    end = start + numZInterfaces;
#pragma omp parallel for
    for (int l = start; l < end; l++) {
        (*r)(PsiRowInds[l]) += (-0.5/H)*(*x)(PsiColDRowInds[l]);
    }
}

void EJ_AMatrix::AInv_times_X(puma::Matrix<double> *x)
{

    fftw_plan_with_nthreads(numThreads);

    // Put Input in FFTW Array
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (int n = 0; n < N; n++)
    {
        inData[n] = (*x)(n);
    }

    // FFTW Plan
    pf = fftw_plan_dft_r2c_3d(X, Y, Z, inData, fourierData, FFTW_ESTIMATE);
    pb = fftw_plan_dft_c2r_3d(X, Y, Z, fourierData, outData, FFTW_ESTIMATE);

    // Execute Fourier Transform
    fftw_execute(pf);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (int i = 0; i < X; i++) {
        for (int j = 0; j < Y; j++) {
            for (int k = 0; k < ZFourierSpace; k++) {
                long n = Z*(Y*i + j) + k;
                long nFFTW = ZFourierSpace*(Y*i + j) + k;
                fourierData[nFFTW][0] = fourierData[nFFTW][0]/PDM[n];
                fourierData[nFFTW][1] = fourierData[nFFTW][1]/PDM[n];
            }
        }
    }

    //Execute Inverse Fourier Transform
    fftw_execute(pb);

    // Put Result in Puma Matrix
    x->set(0);
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (long n = 0; n < N; n++)
    {
        (*x)(n) = outData[n]/N;
    }

}

void EJ_AMatrix::D_times_X(puma::Matrix<double> *x, puma::Matrix<double> *r)
{
    omp_set_num_threads(numThreads);
    r->set(0);
    int start = 0;
    int end = numXInterfaces;
#pragma omp parallel for
    for (long l = start; l < end; l++)
    {
        (*r)(PsiColDRowInds[l]) += (2.0/H)*BForDMatrix[l]*(*x)(DColInds[l]);
    }
    start = end;
    end = start + numXInterfaces;
#pragma omp parallel for
    for (long l = start; l < end; l++)
    {
        (*r)(PsiColDRowInds[l]) += (2.0/H)*BForDMatrix[l]*(*x)(DColInds[l]);
    }
    start = end;
    end = start + numYInterfaces;
#pragma omp parallel for
    for (long l = start; l < end; l++)
    {
        (*r)(PsiColDRowInds[l]) += (2.0/H)*BForDMatrix[l]*(*x)(DColInds[l]);
    }
    start = end;
    end = start + numYInterfaces;
#pragma omp parallel for
    for (long l = start; l < end; l++)
    {
        (*r)(PsiColDRowInds[l]) += (2.0/H)*BForDMatrix[l]*(*x)(DColInds[l]);
    }
    start = end;
    end = start + numZInterfaces;
#pragma omp parallel for
    for (long l = start; l < end; l++)
    {
        (*r)(PsiColDRowInds[l]) += (2.0/H)*BForDMatrix[l]*(*x)(DColInds[l]);
    }
    start = end;
    end = start + numZInterfaces;
#pragma omp parallel for
    for (long l = start; l < end; l++)
    {
        (*r)(PsiColDRowInds[l]) += (2.0/H)*BForDMatrix[l]*(*x)(DColInds[l]);
    }
}

void EJ_AMatrix::X_minus_R(puma::Matrix<double> *x, puma::Matrix<double> *r)
{
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (long m = 0; m < numInterfacesTotal; m++) {
        (*r)(m) = (*x)(m) - (*r)(m);
    }
}
