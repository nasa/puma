#include "fv_anisotropic_diffusion.h"


bool FV_anisotropic_Diffusion::computeKMatrix(puma::Workspace *segWS, std::map<int, std::vector<double>> matCond, puma::Matrix< std::vector<double> > *kMat,
                                              puma::MatVec3<double> *direction, bool print, int numThreads) {

    if(print) { std::cout << "Assigning conductivities ... " << std::flush;}

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (long i=0; i<kMat->X(); i++) {
            double phi = NAN, theta = NAN;
            Eigen::MatrixXd Ry_k_rot(3,3), Rz_kinit(3,3), R(3,3), R_inv(3,3);
            for (long j=0; j<kMat->Y(); j++) {
                for (long k=0; k < kMat->Z(); k++) {

                    // If 6 entries in matCond, we just assign the input anisotropic tensor, ignoring whether or not direction present
                    // arranged as: kxx, kyy, kzz, kxy, kxz, kyz
                    if (matCond[(*segWS)(i, j, k)].size() == 6) {
                        kMat->at(i,j,k) = matCond[(*segWS)(i,j,k)];

                    } else if (matCond[(*segWS)(i, j, k)].size() == 1) { // only 1 entry means isotropic phase --> place single entry on diagonal
                        kMat->at(i,j,k)[0] = matCond[(*segWS)(i,j,k)][0];
                        kMat->at(i,j,k)[1] = matCond[(*segWS)(i,j,k)][0];
                        kMat->at(i,j,k)[2] = matCond[(*segWS)(i,j,k)][0];

                    } else {  // if 2 entries, compute tensor rotation for non-intersecting fibers

                        // If non intersecting fibers
                        if (direction->at(i, j, k).magnitude() < 1.5) {

                            // Tensor rotation
                            phi = atan2(direction->at(i, j, k).y, direction->at(i, j, k).x);
                            theta = asin(direction->at(i, j, k).z);

                            Rz_kinit << cos(phi), -sin(phi), 0,
                                    sin(phi), cos(phi), 0,
                                    0, 0, 1;
                            Ry_k_rot << cos(theta), 0, sin(theta),
                                    0, 1, 0,
                                    -sin(theta), 0, cos(theta);

                            R = Rz_kinit * Ry_k_rot;

                            R_inv = R.inverse();

                            Rz_kinit << matCond[(*segWS)(i, j, k)][0], 0, 0,
                                    0, matCond[(*segWS)(i, j, k)][1], 0,
                                    0, 0, matCond[(*segWS)(i, j, k)][1];

                            Ry_k_rot = R * Rz_kinit * R_inv;

                            kMat->at(i, j, k)[0] = Ry_k_rot(0); // kxx
                            kMat->at(i, j, k)[1] = Ry_k_rot(4); // kyy
                            kMat->at(i, j, k)[2] = Ry_k_rot(8); // kzz
                            kMat->at(i, j, k)[3] = Ry_k_rot(1); // kxy
                            kMat->at(i, j, k)[4] = Ry_k_rot(2); // kxz
                            kMat->at(i, j, k)[5] = Ry_k_rot(5); // kyz
                        }

                            // fiber intersections are flagged with dir.magnitude = 2
                            // treat these areas as isotropic
                        else {
                            kMat->at(i,j,k)[0] = matCond[(*segWS)(i,j,k)][0];
                            kMat->at(i,j,k)[1] = matCond[(*segWS)(i,j,k)][0];
                            kMat->at(i,j,k)[2] = matCond[(*segWS)(i,j,k)][0];
                        }
                    }
                }
            }
        }
    if(print) { std::cout << "Done" << std::endl;}
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


FV_anisotropic_Diffusion::FV_anisotropic_Diffusion(puma::Matrix<double> *T, puma::MatVec3<double> *q, puma::Matrix<std::vector<double> > *kMat,
                                                   std::string sideBC, puma::Matrix<double> *prescribedBC, std::string solverType, char dir, double voxelLength,
                                                   double solverTol, int solverMaxIt, bool print, std::string method, int numThreads)
{
    this->T = T;
    this->q = q;
    this->kMat = kMat;
    this->sideBC = std::move(sideBC);
    this->prescribedBC = prescribedBC;
    this->solverType = std::move(solverType);
    this->dir = dir;
    this->voxelLength = voxelLength;
    this->solverTol = solverTol;
    this->solverMaxIt = solverMaxIt;
    this->print = print;
    this->method = std::move(method);
    this->numThreads = numThreads;
    this->X = (int)kMat->X();
    this->Y = (int)kMat->Y();
    this->Z = (int)kMat->Z();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


puma::Vec3<double> FV_anisotropic_Diffusion::compute_DiffusionCoefficient()
{
    // Resizing necessary puma matrices
    T->resize(kMat->X(),kMat->Y(),kMat->Z(),0);
    q->resize(kMat->X(),kMat->Y(),kMat->Z());
    flux_Matrix.resize(kMat->X(),kMat->Y(),kMat->Z(), std::vector<double> (24,0));
    E_Matrix.resize(kMat->X()+1,kMat->Y()+1,kMat->Z()+1, std::vector<double> (96,0)); //E_Matrix(i,j,k).resize(96);

    initializeLinearProfile();
    computeEMatrix();

    FV_anisotropic_AMatrix A(&E_Matrix,&flux_Matrix,startend,sideBC,numThreads); // boundaries
    if(!runIterativeSolver(&A)) {
        return puma::Vec3<double>(-1,-1,-1);
    }

    A.computeQuadrantFluxes(T);

    puma::Vec3<double> fluxes(0,0,0),length(0,0,0);
    computeLengthFluxes(&fluxes,&length);

    return diffusionCoefficient;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * Description: Sets the residual starting and ending indices and initial temperature field
 *      for the simulation. Starting positions are for the Dirichlet boundary conditions so
 *      that the residual isn't computed for the first and last plane (because they have fixed
 *      temperatures). A linear temperature profile is initialized, with 0 at 0 and 1 at the end of the dimension
 * Inputs: None
 * Outputs: Initial temperature field and residual indices are set.length
 */
bool FV_anisotropic_Diffusion::initializeLinearProfile() {

    if(prescribedBC->size()==1) {
        if(dir == 'x') {
            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for(int i=0;i<X;i++) {
                for(int j=0;j<Y;j++) {
                    for(int k=0;k<Z;k++) {
                        (*T).at(i,j,k) = (double)i/(X-1); // Applies linear temperature profile in X direction
                    }
                }
            }
        }
        else if(dir == 'y') {
            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for(int i=0;i<X;i++) {
                for(int j=0;j<Y;j++) {
                    for(int k=0;k<Z;k++) {
                        (*T).at(i,j,k) = (double)j/(Y-1); // Applies linear temperature profile in Y direction
                    }
                }
            }
        }
        else {
            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for(int i=0;i<X;i++) {
                for(int j=0;j<Y;j++) {
                    for(int k=0;k<Z;k++) {
                        (*T).at(i,j,k) = (double)k/(Z-1); // Applies linear temperature profile in Z direction
                    }
                }
            }
        }
    }
    else { // if BC prescribed
        if(dir == 'x') {
            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for(int i=1;i<X-1;i++) {
                for(int j=0;j<Y;j++) {
                    for(int k=0;k<Z;k++) {
                        (*T).at(i,j,k) = (double)i/(X-1); // Applies linear temperature profile in X direction
                    }
                }
            }
            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for(int j=0;j<Y;j++) {
                for(int k=0;k<Z;k++) {
                    (*T).at(0,j,k) = (*prescribedBC)(0,j,k);
                    (*T).at(X-1,j,k) = (*prescribedBC)(1,j,k);
                }
            }
        }
        else if(dir == 'y') {
            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for(int i=0;i<X;i++) {
                for(int j=1;j<Y-1;j++) {
                    for(int k=0;k<Z;k++) {
                        (*T).at(i,j,k) = (double)j/(Y-1); // Applies linear temperature profile in Y direction
                    }
                }
            }
            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for(int i=0;i<X;i++) {
                for(int k=0;k<Z;k++) {
                    (*T).at(i,0,k) = (*prescribedBC)(i,0,k);
                    (*T).at(i,Y-1,k) = (*prescribedBC)(i,1,k);
                }
            }
        }
        else {
            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for(int i=0;i<X;i++) {
                for(int j=0;j<Y;j++) {
                    for(int k=1;k<Z-1;k++) {
                        (*T).at(i,j,k) = (double)k/(Z-1); // Applies linear temperature profile in Z direction
                    }
                }
            }
            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for(int i=0;i<X;i++) {
                for(int j=0;j<Y;j++) {
                    (*T).at(i,j,0) = (*prescribedBC)(i,j,0);
                    (*T).at(i,j,Z-1) = (*prescribedBC)(i,j,1);
                }
            }
        }
    }
    // Dirichlet BC
    if(dir == 'x')      startend = {1,X, 0,Y+1, 0,Z+1, 1,X-1, 0,Y, 0,Z};
    else if(dir == 'y') startend = {0,X+1, 1,Y, 0,Z+1, 0,X, 1,Y-1, 0,Z};
    else                startend = {0,X+1, 0,Y+1, 1,Z, 0,X, 0,Y, 1,Z-1};

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool FV_anisotropic_Diffusion::computeEMatrix() {

    if (method == "m" && print) { std::cout << "Computing E matrix using MPFA " << std::endl; }
    else if (method == "e" && print) { std::cout << "Computing E matrix using eMPFA " << std::endl; }

    for(long i=startend[0];i<startend[1];i++){

        if (print) { std::cout << std::setprecision(3) << '\r' << ((double)i/(double)(startend[1]-1))*100 << "% done" << std::flush; }

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long j=startend[2];j<startend[3];j++){
            // Initializing matrices
            std::vector<double> mat(12*12), mat2(12*12);
            Eigen::MatrixXd mat2_eigen(12,12);
            std::vector<double> kl(48);

            for(long k=startend[4];k<startend[5];k++){

                for (int i2=0;i2<6;i2++) {
                    kl[i2]    = (*kMat).at(indexAt(i-1,X),indexAt(j-1,Y),indexAt(k-1,Z))[i2]; // Cell P
                    kl[i2+6]  = (*kMat).at(indexAt(i,X),indexAt(j-1,Y),indexAt(k-1,Z))[i2];   // Cell E
                    kl[i2+12] = (*kMat).at(indexAt(i-1,X),indexAt(j,Y),indexAt(k-1,Z))[i2];   // Cell N
                    kl[i2+18] = (*kMat).at(indexAt(i,X),indexAt(j,Y),indexAt(k-1,Z))[i2];     // Cell NE
                    kl[i2+24] = (*kMat).at(indexAt(i-1,X),indexAt(j-1,Y),indexAt(k,Z))[i2];   // Cell T
                    kl[i2+30] = (*kMat).at(indexAt(i,X),indexAt(j-1,Y),indexAt(k,Z))[i2];     // Cell TE
                    kl[i2+36] = (*kMat).at(indexAt(i-1,X),indexAt(j,Y),indexAt(k,Z))[i2];     // Cell TN
                    kl[i2+42] = (*kMat).at(indexAt(i,X),indexAt(j,Y),indexAt(k,Z))[i2];       // Cell TNE
                }

                // Carrying out: A*C^(-1)*D + B
                if (method == "m") {
                    // MPFA

                    mat2_eigen << kl[6] + kl[0], 0, kl[3], -kl[9], kl[4], -kl[10], 0, 0, 0, 0, 0, 0,
                            0, kl[12] + kl[18], -kl[15], kl[21], 0, 0, kl[16], -kl[22], 0, 0, 0, 0,
                            0, 0, 0, 0, -kl[28], kl[34], 0, 0, kl[24] + kl[30], 0, kl[27], -kl[33],
                            0, 0, 0, 0, 0, 0, -kl[40], kl[46], 0, kl[36] + kl[42], -kl[39], kl[45],
                            kl[3], -kl[15], kl[13] + kl[1], 0, kl[5], 0, -kl[17], 0, 0, 0, 0, 0,
                            -kl[9], kl[21], 0, kl[7] + kl[19], 0, kl[11], 0, -kl[23], 0, 0, 0, 0,
                            0, 0, 0, 0, -kl[29], 0, kl[41], 0, kl[27], -kl[39], kl[25] + kl[37], 0,
                            0, 0, 0, 0, 0, -kl[35], 0, kl[47], -kl[33], kl[45], 0, kl[31] + kl[43],
                            kl[4], 0, kl[5], 0, kl[2] + kl[26], 0, 0, 0, -kl[28], 0, -kl[29], 0,
                            -kl[10], 0, 0, kl[11], 0, kl[8] + kl[32], 0, 0, kl[34], 0, 0, -kl[35],
                            0, kl[16], -kl[17], 0, 0, 0, kl[14] + kl[38], 0, 0, -kl[40], kl[41], 0,
                            0, -kl[22], 0, -kl[23], 0, 0, 0, kl[20] + kl[44], 0, kl[46], 0, kl[47];

                    mat2_eigen = mat2_eigen.inverse();
                    mat2_eigen.transposeInPlace();

                    mat2.resize(12*12);
                    for(int t=0; t<mat2.size(); t++){
                        mat2[t] = mat2_eigen(t);
                    }

                    mat = {-kl[0], 0, -kl[3], 0, -kl[4], 0, 0, 0, 0, 0, 0, 0,
                           0, -kl[12], kl[15], 0, 0, 0, -kl[16], 0, 0, 0, 0, 0,
                           0, 0, 0, 0, kl[28], 0, 0, 0, -kl[24], 0, -kl[27], 0,
                           0, 0, 0, 0, 0, 0, kl[40], 0, 0, -kl[36], kl[39], 0,
                           -kl[3], 0, -kl[1], 0, -kl[5], 0, 0, 0, 0, 0, 0, 0,
                           kl[9], 0, 0, -kl[7], 0, -kl[11], 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, kl[29], 0, 0, 0, -kl[27], 0, -kl[25], 0,
                           0, 0, 0, 0, 0, kl[35], 0, 0, kl[33], 0, 0, -kl[31],
                           -kl[4], 0, -kl[5], 0, -kl[2], 0, 0, 0, 0, 0, 0, 0,
                           kl[10], 0, 0, -kl[11], 0, -kl[8], 0, 0, 0, 0, 0, 0,
                           0, -kl[16], kl[17], 0, 0, 0, -kl[14], 0, 0, 0, 0, 0,
                           0, kl[22], 0, kl[23], 0, 0, 0, -kl[20], 0, 0, 0, 0}; // A 12x12

                    mat2 = matrixMultiply(&mat, 12, 12, &mat2, 12); // E1 = A*C^(-1)

                    mat  = {kl[0] + kl[3] + kl[4], kl[6] - kl[9] - kl[10], 0, 0, 0, 0, 0, 0,
                            0, 0, kl[12] - kl[15] + kl[16], kl[18] + kl[21] - kl[22], 0, 0, 0, 0,
                            0, 0, 0, 0, kl[24] + kl[27] - kl[28], kl[30] - kl[33] + kl[34], 0, 0,
                            0, 0, 0, 0, 0, 0, kl[36] - kl[39] - kl[40], kl[42] + kl[45] + kl[46],
                            kl[3] + kl[1] + kl[5], 0, -kl[15] + kl[13] - kl[17], 0, 0, 0, 0, 0,
                            0, -kl[9] + kl[7] + kl[11], 0, kl[21] + kl[19] - kl[23], 0, 0, 0, 0,
                            0, 0, 0, 0, kl[27] + kl[25] - kl[29], 0, -kl[39] + kl[37] + kl[41], 0,
                            0, 0, 0, 0, 0, -kl[33] + kl[31] - kl[35], 0, kl[45] + kl[43] + kl[47],
                            kl[4] + kl[5] + kl[2], 0, 0, 0, -kl[28] - kl[29] + kl[26], 0, 0, 0,
                            0, -kl[10] + kl[11] + kl[8], 0, 0, 0, kl[34] - kl[35] + kl[32], 0, 0,
                            0, 0, kl[16] - kl[17] + kl[14], 0, 0, 0, -kl[40] + kl[41] + kl[38], 0,
                            0, 0, 0, -kl[22] - kl[23] + kl[20], 0, 0, 0, kl[46] + kl[47] + kl[44]}; // D 12x8

                    mat2 = matrixMultiply(&mat2, 12, 12, &mat, 8); // E2 = E1*D

                    mat = {kl[0] + kl[3] + kl[4], 0, 0, 0, 0, 0, 0, 0,
                           0, 0, kl[12] - kl[15] + kl[16], 0, 0, 0, 0, 0,
                           0, 0, 0, 0, kl[24] + kl[27] - kl[28], 0, 0, 0,
                           0, 0, 0, 0, 0, 0, kl[36] - kl[39] - kl[40], 0,
                           kl[3] + kl[1] + kl[5], 0, 0, 0, 0, 0, 0, 0,
                           0, -kl[9] + kl[7] + kl[11], 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, kl[27] + kl[25] - kl[29], 0, 0, 0,
                           0, 0, 0, 0, 0, -kl[33] + kl[31] - kl[35], 0, 0,
                           kl[4] + kl[5] + kl[2], 0, 0, 0, 0, 0, 0, 0,
                           0, -kl[10] + kl[11] + kl[8], 0, 0, 0, 0, 0, 0,
                           0, 0, kl[16] - kl[17] + kl[14], 0, 0, 0, 0, 0,
                           0, 0, 0, -kl[22] - kl[23] + kl[20], 0, 0, 0, 0}; // B 12x8
                }
                else if (method == "e") {
                    // eMPFA
                    mat2_eigen << kl[6]+kl[0]+kl[9]/2-kl[3]/2+kl[10]/2-kl[4]/2, -(kl[9]/2)+kl[3]/2, -(kl[9]/2)+kl[3]/2, -(kl[9]/2)+kl[3]/2, -(kl[10]/2)+kl[4]/2, -(kl[10]/2)+kl[4]/2, 0, 0, -(kl[10]/2)+kl[4]/2, 0, 0, 0,
                            -(kl[15]/2)+kl[21]/2, kl[12]+kl[18]+kl[15]/2-kl[21]/2-kl[16]/2+kl[22]/2, -(kl[15]/2)+kl[21]/2, -(kl[15]/2)+kl[21]/2, 0, 0, kl[16]/2-kl[22]/2, kl[16]/2-kl[22]/2, 0, kl[16]/2-kl[22]/2, 0, 0,
                            -(kl[28]/2)+kl[34]/2, 0, 0, 0, -(kl[28]/2)+kl[34]/2, -(kl[28]/2)+kl[34]/2, 0, 0, kl[24]+kl[30]-kl[27]/2+kl[33]/2+kl[28]/2-kl[34]/2, kl[27]/2-kl[33]/2, kl[27]/2-kl[33]/2, kl[27]/2-kl[33]/2,
                            0, -(kl[40]/2)+kl[46]/2, 0, 0, 0, 0, -(kl[40]/2)+kl[46]/2, -(kl[40]/2)+kl[46]/2, -(kl[39]/2)+kl[45]/2, kl[36]+kl[42]+kl[39]/2-kl[45]/2+kl[40]/2-kl[46]/2, -(kl[39]/2)+kl[45]/2, -(kl[39]/2)+kl[45]/2,
                            -(kl[15]/2)+kl[3]/2, -(kl[15]/2)+kl[3]/2, kl[15]/2-kl[3]/2+kl[13]+kl[1]+kl[17]/2-kl[5]/2, -(kl[15]/2)+kl[3]/2, -(kl[17]/2)+kl[5]/2, 0, -(kl[17]/2)+kl[5]/2, 0, 0, 0, -(kl[17]/2)+kl[5]/2, 0,
                            -(kl[9]/2)+kl[21]/2, -(kl[9]/2)+kl[21]/2, -(kl[9]/2)+kl[21]/2, kl[9]/2-kl[21]/2+kl[7]+kl[19]-kl[11]/2+kl[23]/2, 0, kl[11]/2-kl[23]/2, 0, kl[11]/2-kl[23]/2, 0, 0, 0, kl[11]/2-kl[23]/2,
                            0, 0, -(kl[29]/2)+kl[41]/2, 0, -(kl[29]/2)+kl[41]/2, 0, -(kl[29]/2)+kl[41]/2, 0, kl[27]/2-kl[39]/2, kl[27]/2-kl[39]/2, -(kl[27]/2)+kl[39]/2+kl[25]+kl[37]+kl[29]/2-kl[41]/2, kl[27]/2-kl[39]/2,
                            0, 0, 0, -(kl[35]/2)+kl[47]/2, 0, -(kl[35]/2)+kl[47]/2, 0, -(kl[35]/2)+kl[47]/2, -(kl[33]/2)+kl[45]/2, -(kl[33]/2)+kl[45]/2, -(kl[33]/2)+kl[45]/2, kl[33]/2-kl[45]/2+kl[31]+kl[43]+kl[35]/2-kl[47]/2,
                            kl[4]/2-kl[28]/2, 0, kl[5]/2-kl[29]/2, 0, -(kl[4]/2)+kl[28]/2-kl[5]/2+kl[29]/2+kl[2]+kl[26], kl[4]/2-kl[28]/2, kl[5]/2-kl[29]/2, 0, kl[4]/2-kl[28]/2, 0, kl[5]/2-kl[29]/2, 0,
                            -(kl[10]/2)+kl[34]/2, 0, 0, kl[11]/2-kl[35]/2, -(kl[10]/2)+kl[34]/2, kl[10]/2-kl[34]/2-kl[11]/2+kl[35]/2+kl[8]+kl[32], 0, kl[11]/2-kl[35]/2, -(kl[10]/2)+kl[34]/2, 0, 0, kl[11]/2-kl[35]/2,
                            0, kl[16]/2-kl[40]/2, -(kl[17]/2)+kl[41]/2, 0, -(kl[17]/2)+kl[41]/2, 0, -(kl[16]/2)+kl[40]/2+kl[17]/2-kl[41]/2+kl[14]+kl[38], kl[16]/2-kl[40]/2, 0, kl[16]/2-kl[40]/2, -(kl[17]/2)+kl[41]/2, 0,
                            0, -(kl[22]/2)+kl[46]/2, 0, -(kl[23]/2)+kl[47]/2, 0, -(kl[23]/2)+kl[47]/2, -(kl[22]/2)+kl[46]/2, kl[22]/2-kl[46]/2+kl[23]/2-kl[47]/2+kl[20]+kl[44], 0, -(kl[22]/2)+kl[46]/2, 0, -(kl[23]/2)+kl[47]/2; // C 12x12

                    mat2_eigen = mat2_eigen.inverse();
                    mat2_eigen.transposeInPlace();

                    mat2.resize(12*12);
                    for(int t=0; t<mat2.size(); t++){
                        mat2[t] = mat2_eigen(t);
                    }

                    mat = {-kl[0]+kl[3]/2+kl[4]/2, -(kl[3]/2), -(kl[3]/2), -(kl[3]/2), -(kl[4]/2), -(kl[4]/2), 0, 0, -(kl[4]/2), 0, 0, 0,
                           kl[15]/2, -kl[12]-kl[15]/2+kl[16]/2, kl[15]/2, kl[15]/2, 0, 0, -(kl[16]/2), -(kl[16]/2), 0, -(kl[16]/2), 0, 0,
                           kl[28]/2, 0, 0, 0, kl[28]/2, kl[28]/2, 0, 0, -kl[24]+kl[27]/2-kl[28]/2, -(kl[27]/2), -(kl[27]/2), -(kl[27]/2),
                           0, kl[40]/2, 0, 0, 0, 0, kl[40]/2, kl[40]/2, kl[39]/2, -kl[36]-kl[39]/2-kl[40]/2, kl[39]/2, kl[39]/2,
                           -(kl[3]/2), -(kl[3]/2), kl[3]/2-kl[1]+kl[5]/2, -(kl[3]/2), -(kl[5]/2), 0, -(kl[5]/2), 0, 0, 0, -(kl[5]/2), 0,
                           kl[9]/2, kl[9]/2, kl[9]/2, -(kl[9]/2)-kl[7]+kl[11]/2, 0, -(kl[11]/2), 0, -(kl[11]/2), 0, 0, 0, -(kl[11]/2),
                           0, 0, kl[29]/2, 0, kl[29]/2, 0, kl[29]/2, 0, -(kl[27]/2), -(kl[27]/2), kl[27]/2-kl[25]-kl[29]/2, -(kl[27]/2),
                           0, 0, 0, kl[35]/2, 0, kl[35]/2, 0, kl[35]/2, kl[33]/2, kl[33]/2, kl[33]/2, -(kl[33]/2)-kl[31]-kl[35]/2,
                           -(kl[4]/2), 0, -(kl[5]/2), 0, kl[4]/2+kl[5]/2-kl[2], -(kl[4]/2), -(kl[5]/2), 0, -(kl[4]/2), 0, -(kl[5]/2), 0,
                           kl[10]/2, 0, 0, -(kl[11]/2), kl[10]/2, -(kl[10]/2)+kl[11]/2-kl[8], 0, -(kl[11]/2), kl[10]/2, 0, 0, -(kl[11]/2),
                           0, -(kl[16]/2), kl[17]/2, 0, kl[17]/2, 0, kl[16]/2-kl[17]/2-kl[14], -(kl[16]/2), 0, -(kl[16]/2), kl[17]/2, 0,
                           0, kl[22]/2, 0, kl[23]/2, 0, kl[23]/2, kl[22]/2, -(kl[22]/2)-kl[23]/2-kl[20], 0, kl[22]/2, 0, kl[23]/2}; // A 12x12

                    mat2 = matrixMultiply(&mat, 12, 12, &mat2, 12); // E1 = A*C^(-1)

                    mat = {kl[0]-kl[9]/4+kl[3]/4-kl[10]/4+kl[4]/4, kl[6]-kl[9]/4+kl[3]/4-kl[10]/4+kl[4]/4, -(kl[9]/4)+kl[3]/4, -(kl[9]/4)+kl[3]/4, -(kl[10]/4)+kl[4]/4, -(kl[10]/4)+kl[4]/4, 0, 0,
                           -(kl[15]/4)+kl[21]/4, -(kl[15]/4)+kl[21]/4, kl[12]-kl[15]/4+kl[21]/4+kl[16]/4-kl[22]/4, kl[18]-kl[15]/4+kl[21]/4+kl[16]/4-kl[22]/4, 0, 0, kl[16]/4-kl[22]/4, kl[16]/4-kl[22]/4,
                           -(kl[28]/4)+kl[34]/4, -(kl[28]/4)+kl[34]/4, 0, 0, kl[24]+kl[27]/4-kl[33]/4-kl[28]/4+kl[34]/4, kl[30]+kl[27]/4-kl[33]/4-kl[28]/4+kl[34]/4, kl[27]/4-kl[33]/4, kl[27]/4-kl[33]/4,
                           0, 0, -(kl[40]/4)+kl[46]/4, -(kl[40]/4)+kl[46]/4, -(kl[39]/4)+kl[45]/4, -(kl[39]/4)+kl[45]/4, kl[36]-kl[39]/4+kl[45]/4-kl[40]/4+kl[46]/4, kl[42]-kl[39]/4+kl[45]/4-kl[40]/4+kl[46]/4,
                           -(kl[15]/4)+kl[3]/4+kl[1]-kl[17]/4+kl[5]/4, -(kl[15]/4)+kl[3]/4, -(kl[15]/4)+kl[3]/4+kl[13]-kl[17]/4+kl[5]/4, -(kl[15]/4)+kl[3]/4, -(kl[17]/4)+kl[5]/4, 0, -(kl[17]/4)+kl[5]/4, 0,
                           -(kl[9]/4)+kl[21]/4, -(kl[9]/4)+kl[21]/4+kl[7]+kl[11]/4-kl[23]/4, -(kl[9]/4)+kl[21]/4, -(kl[9]/4)+kl[21]/4+kl[19]+kl[11]/4-kl[23]/4, 0, kl[11]/4-kl[23]/4, 0, kl[11]/4-kl[23]/4,
                           -(kl[29]/4)+kl[41]/4, 0, -(kl[29]/4)+kl[41]/4, 0, kl[27]/4-kl[39]/4+kl[25]-kl[29]/4+kl[41]/4, kl[27]/4-kl[39]/4, kl[27]/4-kl[39]/4+kl[37]-kl[29]/4+kl[41]/4, kl[27]/4-kl[39]/4,
                           0, -(kl[35]/4)+kl[47]/4, 0, -(kl[35]/4)+kl[47]/4, -(kl[33]/4)+kl[45]/4, -(kl[33]/4)+kl[45]/4+kl[31]-kl[35]/4+kl[47]/4, -(kl[33]/4)+kl[45]/4, -(kl[33]/4)+kl[45]/4+kl[43]-kl[35]/4+kl[47]/4,
                           kl[4]/4-kl[28]/4+kl[5]/4-kl[29]/4+kl[2], kl[4]/4-kl[28]/4, kl[5]/4-kl[29]/4, 0, kl[4]/4-kl[28]/4+kl[5]/4-kl[29]/4+kl[26], kl[4]/4-kl[28]/4, kl[5]/4-kl[29]/4, 0,
                           -(kl[10]/4)+kl[34]/4, -(kl[10]/4)+kl[34]/4+kl[11]/4-kl[35]/4+kl[8], 0, kl[11]/4-kl[35]/4, -(kl[10]/4)+kl[34]/4, -(kl[10]/4)+kl[34]/4+kl[11]/4-kl[35]/4+kl[32], 0, kl[11]/4-kl[35]/4,
                           -(kl[17]/4)+kl[41]/4, 0, kl[16]/4-kl[40]/4-kl[17]/4+kl[41]/4+kl[14], kl[16]/4-kl[40]/4, -(kl[17]/4)+kl[41]/4, 0, kl[16]/4-kl[40]/4-kl[17]/4+kl[41]/4+kl[38], kl[16]/4-kl[40]/4,
                           0, -(kl[23]/4)+kl[47]/4, -(kl[22]/4)+kl[46]/4, -(kl[22]/4)+kl[46]/4-kl[23]/4+kl[47]/4+kl[20], 0, -(kl[23]/4)+kl[47]/4, -(kl[22]/4)+kl[46]/4, -(kl[22]/4)+kl[46]/4-kl[23]/4+kl[47]/4+kl[44]}; // D 12x8

                    mat2 = matrixMultiply(&mat2, 12, 12, &mat, 8); // E2 = E1*D

                    mat = {kl[0]+kl[3]/4+kl[4]/4, kl[3]/4+kl[4]/4, kl[3]/4, kl[3]/4, kl[4]/4, kl[4]/4, 0, 0,
                           -(kl[15]/4), -(kl[15]/4), kl[12]-kl[15]/4+kl[16]/4, -(kl[15]/4)+kl[16]/4, 0, 0, kl[16]/4, kl[16]/4,
                           -(kl[28]/4), -(kl[28]/4), 0, 0, kl[24]+kl[27]/4-kl[28]/4, kl[27]/4-kl[28]/4, kl[27]/4, kl[27]/4,
                           0, 0, -(kl[40]/4), -(kl[40]/4), -(kl[39]/4), -(kl[39]/4), kl[36]-kl[39]/4-kl[40]/4, -(kl[39]/4)-kl[40]/4,
                           kl[3]/4+kl[1]+kl[5]/4, kl[3]/4, kl[3]/4+kl[5]/4, kl[3]/4, kl[5]/4, 0, kl[5]/4, 0,
                           -(kl[9]/4), -(kl[9]/4)+kl[7]+kl[11]/4, -(kl[9]/4), -(kl[9]/4)+kl[11]/4, 0, kl[11]/4, 0, kl[11]/4,
                           -(kl[29]/4), 0, -(kl[29]/4), 0, kl[27]/4+kl[25]-kl[29]/4, kl[27]/4, kl[27]/4-kl[29]/4, kl[27]/4,
                           0, -(kl[35]/4), 0, -(kl[35]/4), -(kl[33]/4), -(kl[33]/4)+kl[31]-kl[35]/4, -(kl[33]/4), -(kl[33]/4)-kl[35]/4,
                           kl[4]/4+kl[5]/4+kl[2], kl[4]/4, kl[5]/4, 0, kl[4]/4+kl[5]/4, kl[4]/4, kl[5]/4, 0,
                           -(kl[10]/4), -(kl[10]/4)+kl[11]/4+kl[8], 0, kl[11]/4, -(kl[10]/4), -(kl[10]/4)+kl[11]/4, 0, kl[11]/4,
                           -(kl[17]/4), 0, kl[16]/4-kl[17]/4+kl[14], kl[16]/4, -(kl[17]/4), 0, kl[16]/4-kl[17]/4, kl[16]/4,
                           0, -(kl[23]/4), -(kl[22]/4), -(kl[22]/4)-kl[23]/4+kl[20], 0, -(kl[23]/4), -(kl[22]/4), -(kl[22]/4)-kl[23]/4}; // B 12x8
                }

                for(int i2=0;i2<12;i2++){
                    for(int j2=0;j2<8;j2++){
                        E_Matrix.at(i,j,k)[8*i2+j2] = mat2[8*i2+j2] + mat[8*i2+j2];  // E = E2+B
                    }
                }
            }
        }
    }
    if (print) { std::cout << std::endl; }

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool FV_anisotropic_Diffusion::runIterativeSolver(FV_anisotropic_AMatrix *A) {
    if (solverType == "bicgstab"){
        IterativeSolver::BiCGSTAB(A,T,solverTol,solverMaxIt,print,numThreads);
    }
    else if (solverType == "cg") {
        IterativeSolver::ConjugateGradient(A,T,solverTol,solverMaxIt,print,numThreads);
    }

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * Description: Computes the flux in each of the three directions.
 * Inputs: None
 * Outputs: Heat flux for the simulation direction, including diagonals. (Example: XX, XY, XZ)
 */
bool FV_anisotropic_Diffusion::computeLengthFluxes(puma::Vec3<double> *fluxes, puma::Vec3<double> *length) {

    for(long i=startend[6];i<startend[7];i++) {
        for(long j=startend[8]; j<startend[9]; j++) {
            for(long k=startend[10]; k<startend[11]; k++) {
                (*fluxes).x += (flux_Matrix.at(i,j,k)[0] + flux_Matrix.at(i,j,k)[1] + flux_Matrix.at(i,j,k)[2] + flux_Matrix.at(i,j,k)[3] +
                                flux_Matrix.at(i,j,k)[4] + flux_Matrix.at(i,j,k)[5] + flux_Matrix.at(i,j,k)[6] + flux_Matrix.at(i,j,k)[7]) / ( 4*voxelLength );
                (*fluxes).y += (flux_Matrix.at(i,j,k)[0+8] + flux_Matrix.at(i,j,k)[1+8] + flux_Matrix.at(i,j,k)[2+8] + flux_Matrix.at(i,j,k)[3+8] +
                                flux_Matrix.at(i,j,k)[4+8] + flux_Matrix.at(i,j,k)[5+8] + flux_Matrix.at(i,j,k)[6+8] + flux_Matrix.at(i,j,k)[7+8]) / ( 4*voxelLength );
                (*fluxes).z += (flux_Matrix.at(i,j,k)[0+16] + flux_Matrix.at(i,j,k)[1+16] + flux_Matrix.at(i,j,k)[2+16] + flux_Matrix.at(i,j,k)[3+16] +
                                flux_Matrix.at(i,j,k)[4+16] + flux_Matrix.at(i,j,k)[5+16] + flux_Matrix.at(i,j,k)[6+16] + flux_Matrix.at(i,j,k)[7+16]) / ( 4*voxelLength );
            }
        }
    }

    // Passing converged voxel fluxes
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<X;i++) {
        for(int j=0; j<Y; j++) {
            for(int k=0; k<Z; k++) {
                (*q).at(i,j,k).x = (flux_Matrix.at(i,j,k)[0] + flux_Matrix.at(i,j,k)[1] + flux_Matrix.at(i,j,k)[2] + flux_Matrix.at(i,j,k)[3] +
                                    flux_Matrix.at(i,j,k)[4] + flux_Matrix.at(i,j,k)[5] + flux_Matrix.at(i,j,k)[6] + flux_Matrix.at(i,j,k)[7]) / ( 4*voxelLength );
                (*q).at(i,j,k).y = (flux_Matrix.at(i,j,k)[0+8] + flux_Matrix.at(i,j,k)[1+8] + flux_Matrix.at(i,j,k)[2+8] + flux_Matrix.at(i,j,k)[3+8] +
                                    flux_Matrix.at(i,j,k)[4+8] + flux_Matrix.at(i,j,k)[5+8] + flux_Matrix.at(i,j,k)[6+8] + flux_Matrix.at(i,j,k)[7+8]) / ( 4*voxelLength );
                (*q).at(i,j,k).z = (flux_Matrix.at(i,j,k)[0+16] + flux_Matrix.at(i,j,k)[1+16] + flux_Matrix.at(i,j,k)[2+16] + flux_Matrix.at(i,j,k)[3+16] +
                                    flux_Matrix.at(i,j,k)[4+16] + flux_Matrix.at(i,j,k)[5+16] + flux_Matrix.at(i,j,k)[6+16] + flux_Matrix.at(i,j,k)[7+16]) / ( 4*voxelLength );
            }
        }
    }

    (*fluxes).x /= (double)((startend[7]-startend[6])*(startend[9]-startend[8])*(startend[11]-startend[10]));
    (*fluxes).y /= (double)((startend[7]-startend[6])*(startend[9]-startend[8])*(startend[11]-startend[10]));
    (*fluxes).z /= (double)((startend[7]-startend[6])*(startend[9]-startend[8])*(startend[11]-startend[10]));

    if(dir == 'x') {
        (*length)((X-1)*voxelLength, Y*voxelLength, Z*voxelLength);
        diffusionCoefficient.x = -(*fluxes).x * (*length).x;
        diffusionCoefficient.y = -(*fluxes).y * (*length).x;
        diffusionCoefficient.z = -(*fluxes).z * (*length).x;

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(int j=0;j<Y;j++) {
            for(int k=0;k<Z;k++) {
                (*q).at(0,j,k).x *= 2;
                (*q).at(X-1,j,k).x *= 2;
            }
        }
    }
    else if (dir == 'y') {

        (*length)(X*voxelLength, (Y-1)*voxelLength, Z*voxelLength);
        diffusionCoefficient.x = -(*fluxes).x * (*length).y;
        diffusionCoefficient.y = -(*fluxes).y * (*length).y;
        diffusionCoefficient.z = -(*fluxes).z * (*length).y;

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(int i=0;i<X;i++) {
            for(int k=0;k<Z;k++) {
                (*q).at(i,0,k).y *= 2;
                (*q).at(i,Y-1,k).y *= 2;
            }
        }
    }
    else {
        (*length)(X*voxelLength, Y*voxelLength, (Z-1)*voxelLength);
        diffusionCoefficient.x = -(*fluxes).x * (*length).z;
        diffusionCoefficient.y = -(*fluxes).y * (*length).z;
        diffusionCoefficient.z = -(*fluxes).z * (*length).z;

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(int i=0;i<X;i++) {
            for(int j=0;j<Y;j++) {
                (*q).at(i,j,0).z *= 2;
                (*q).at(i,j,Z-1).z *= 2;
            }
        }
    }

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// General Functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * Description: Applies the correct boundary condition (periodic or symmetric) on the imported indices and returns the matrix index.
 *      This function should be split into two functions, so that the if isn't being called every time, which isn't optimal.
 * Inputs: i,j,k - indices for the 3D matrix
 * Outputs: Matrix index for the imposed boundary conditions
 */
long FV_anisotropic_Diffusion::indexAt(long index, long size) {

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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* Description: Matrix multiplication function: assuming matrices stored as A[col2*i+j] where i rows, j cols
 * Inputs:
 *      matrix1
 *      number of cols matrix1
 *      number of rows matrix1
 *      matrix2
 *      number of cols matrix2
 * Outputs:
 *      matrix1*matrix2
 * Note: it assumes that matrix dimensions make sense
 */
std::vector<double> FV_anisotropic_Diffusion::matrixMultiply(std::vector<double> *matrix1, int col1, int row1,
                                                             std::vector<double> *matrix2, int col2){
    std::vector<double> returnMatrix(row1*col2);
    for(int i=0;i<row1;i++) {
        for(int j=0;j<col2;j++) {
            for(int index=0;index<col1;index++) {
                returnMatrix[col2*i+j] += (*matrix1)[col1*i+index] * (*matrix2)[col2*index+j];
            }
        }
    }

    return returnMatrix;
}

