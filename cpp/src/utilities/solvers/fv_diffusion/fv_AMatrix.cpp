#include "fv_AMatrix.h"


FV_AMatrix::FV_AMatrix(puma::Matrix<double> *kMat, std::vector<FV_BoundaryCondition*> *bcs, int numThreads) {
    this->kMat = kMat;
    this->bcs = bcs;
    this->numThreads = numThreads;
    this->X = (int)kMat->X();
    this->Y = (int)kMat->Y();
    this->Z = (int)kMat->Z();

    setup_KMinMax();
}

bool FV_AMatrix::setup_KMinMax() {
    kX.resize( X+1, Y,   Z   );
    kY.resize( X,   Y+1, Z   );
    kZ.resize( X,   Y,   Z+1 );

    setup_KMM_Interior();
    setup_KMM_Boundaries();

    for(long i=0;i<kX.size();i++) {
        if(kX(i)!=kX(i)) {
            std::cout << "nan problem" << std::endl;
        }
    }
    for(long i=0;i<kY.size();i++) {
        if(kY(i)!=kY(i)) {
            std::cout << "nan problem" << std::endl;
        }
    }
    for(long i=0;i<kZ.size();i++) {
        if(kZ(i)!=kZ(i)) {
            std::cout << "nan problem" << std::endl;
        }
    }

    return true;
}

bool FV_AMatrix::A_times_X(puma::Matrix<double> *x, puma::Matrix<double> *r) {

    computeInteriorResidual(x,r);
    computeBoundaryResidual(x,r);

    return true;
}


bool FV_AMatrix::setup_KMM_Interior() {
    //setting up kX interior
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=1;i<X;i++){
        for(int j=0;j<Y;j++){
            for(int k=0;k<Z;k++){
                kX(i,j,k) = kMat->at(i,j,k) * kMat->at(i-1,j,k) / ( kMat->at(i,j,k) + kMat->at(i-1,j,k) );
                if(kX(i,j,k)!=kX(i,j,k)) { kX(i,j,k)=0; }
            }
        }
    }

    //setting up kY interior
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<X;i++){
        for(int j=1;j<Y;j++){
            for(int k=0;k<Z;k++){
                kY(i,j,k) = kMat->at(i,j,k) * kMat->at(i,j-1,k) / ( kMat->at(i,j,k) + kMat->at(i,j-1,k) );
                if(kY(i,j,k)!=kY(i,j,k)) { kY(i,j,k)=0; }
            }
        }
    }

    //setting up kZ interior
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<X;i++){
        for(int j=0;j<Y;j++){
            for(int k=1;k<Z;k++){
                kZ(i,j,k) = kMat->at(i,j,k) * kMat->at(i,j,k-1) / ( kMat->at(i,j,k) + kMat->at(i,j,k-1) );
                if(kZ(i,j,k)!=kZ(i,j,k)) { kZ(i,j,k)=0; }
            }
        }
    }

    return true;
}

bool FV_AMatrix::setup_KMM_Boundaries() {

    //xMin boundary
    int x = 0;
    for(int j=0;j<Y;j++){
        for(int k=0;k<Z;k++){
            double tempK = bcs->at(0)->getK_at(x-1,j,k, kMat);
            kX(x,j,k) = kMat->at(x,j,k) * tempK / ( kMat->at(x,j,k) + tempK );
            if(kX(x,j,k)!=kX(x,j,k)) { kX(x,j,k)=0; }
        }
    }

    //xMax boundary
    x = X;
    for(int j=0;j<Y;j++){
        for(int k=0;k<Z;k++){
            double tempK = bcs->at(1)->getK_at(x,j,k, kMat);
            kX(x,j,k) = tempK * kMat->at(x-1,j,k) / ( tempK + kMat->at(x-1,j,k) );
            if(kX(x,j,k)!=kX(x,j,k)) { kX(x,j,k)=0; }
        }
    }

    //yMin boundary
    int y = 0;
    for(int i=0;i<X;i++){
        for(int k=0;k<Z;k++){
            double tempK = bcs->at(2)->getK_at(i,y-1,k, kMat);
            kY(i,y,k) = kMat->at(i,y,k) * tempK / ( kMat->at(i,y,k) + tempK );
            if(kY(i,y,k)!=kY(i,y,k)) { kY(i,y,k)=0; }
        }
    }

    //yMax boundary
    y = Y;
    for(int i=0;i<X;i++){
        for(int k=0;k<Z;k++){
            double tempK = bcs->at(3)->getK_at(i,y,k, kMat);
            kY(i,y,k) = tempK * kMat->at(i,y-1,k) / ( tempK + kMat->at(i,y-1,k) );
            if(kY(i,y,k)!=kY(i,y,k)) { kY(i,y,k)=0; }
        }
    }

    //zMin boundary
    int z = 0;
    for(int i=0;i<X;i++){
        for(int j=0;j<Y;j++){
            double tempK = bcs->at(4)->getK_at(i,j,z-1, kMat);
            kZ(i,j,z) = kMat->at(i,j,z) * tempK / ( kMat->at(i,j,z) + tempK );
            if(kZ(i,j,z)!=kZ(i,j,z)) { kZ(i,j,z)=0; }
        }
    }

    //zMax boundary
    z = Z;
    for(int i=0;i<X;i++){
        for(int j=0;j<Y;j++){
            double tempK = bcs->at(5)->getK_at(i,j,z, kMat);
            kZ(i,j,z) = tempK * kMat->at(i,j,z-1) / ( tempK + kMat->at(i,j,z-1) );
            if(kZ(i,j,z)!=kZ(i,j,z)) { kZ(i,j,z)=0; }
        }
    }

    return true;

}


void FV_AMatrix::computeInteriorResidual(puma::Matrix<double> *x, puma::Matrix<double> *r) {

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long i=1;i<X-1;i++){
        for(long j=1;j<Y-1;j++){
            for(long k=1;k<Z-1;k++){
                r->at(i,j,k) =  ( kX(i+1,j,k) * ( x->at(i+1,j,k)-x->at(i,j,k) ) + kX(i,j,k) * ( x->at(i-1,j,k)-x->at(i,j,k) ) )
                                +       ( kY(i,j+1,k) * ( x->at(i,j+1,k)-x->at(i,j,k) ) + kY(i,j,k) * ( x->at(i,j-1,k)-x->at(i,j,k) ) )
                                +       ( kZ(i,j,k+1) * ( x->at(i,j,k+1)-x->at(i,j,k) ) + kZ(i,j,k) * ( x->at(i,j,k-1)-x->at(i,j,k) ) );

            }
        }
    }


}

void FV_AMatrix::computeBoundaryResidual(puma::Matrix<double> *x, puma::Matrix<double> *r) {

    //X Boundaries residual
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int j=0;j<Y;j++){
        for(int i=0;i<X;i+= std::max(X-1,1) ) {
            for(int k=0;k<Z;k++){
                r->at(i,j,k) =  ( kX(i+1,j,k) * ( bcs->at(1)->getX_at(i+1,j,k,x)-x->at(i,j,k) ) + kX(i,j,k) * ( bcs->at(0)->getX_at(i-1,j,k,x)-x->at(i,j,k) ) )
                                +       ( kY(i,j+1,k) * ( bcs->at(3)->getX_at(i,j+1,k,x)-x->at(i,j,k) ) + kY(i,j,k) * ( bcs->at(2)->getX_at(i,j-1,k,x)-x->at(i,j,k) ) )
                                +       ( kZ(i,j,k+1) * ( bcs->at(5)->getX_at(i,j,k+1,x)-x->at(i,j,k) ) + kZ(i,j,k) * ( bcs->at(4)->getX_at(i,j,k-1,x)-x->at(i,j,k) ) );

            }
        }
    }

    //Y Boundaries residual
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<X;i++) {
        for(int j=0;j<Y;j+= std::max(Y-1,1) ){
            for(int k=0;k<Z;k++){
                r->at(i,j,k) =  ( kX(i+1,j,k) * ( bcs->at(1)->getX_at(i+1,j,k,x)-x->at(i,j,k) ) + kX(i,j,k) * ( bcs->at(0)->getX_at(i-1,j,k,x)-x->at(i,j,k) ) )
                                +       ( kY(i,j+1,k) * ( bcs->at(3)->getX_at(i,j+1,k,x)-x->at(i,j,k) ) + kY(i,j,k) * ( bcs->at(2)->getX_at(i,j-1,k,x)-x->at(i,j,k) ) )
                                +       ( kZ(i,j,k+1) * ( bcs->at(5)->getX_at(i,j,k+1,x)-x->at(i,j,k) ) + kZ(i,j,k) * ( bcs->at(4)->getX_at(i,j,k-1,x)-x->at(i,j,k) ) );
            }
        }
    }

    //Z Boundaries residual
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<X;i++) {
        for(int j=0;j<Y;j++){
            for(int k=0;k<Z;k+= std::max(Z-1,1) ){
                r->at(i,j,k) =  ( kX(i+1,j,k) * ( bcs->at(1)->getX_at(i+1,j,k,x)-x->at(i,j,k) ) + kX(i,j,k) * ( bcs->at(0)->getX_at(i-1,j,k,x)-x->at(i,j,k) ) )
                                +       ( kY(i,j+1,k) * ( bcs->at(3)->getX_at(i,j+1,k,x)-x->at(i,j,k) ) + kY(i,j,k) * ( bcs->at(2)->getX_at(i,j-1,k,x)-x->at(i,j,k) ) )
                                +       ( kZ(i,j,k+1) * ( bcs->at(5)->getX_at(i,j,k+1,x)-x->at(i,j,k) ) + kZ(i,j,k) * ( bcs->at(4)->getX_at(i,j,k-1,x)-x->at(i,j,k) ) );
            }
        }
    }
}


puma::Vec3<double> FV_AMatrix::computeFluxes(puma::Matrix<double> *x, char dir ) {
    std::vector<double> fluxVec_X(X,0);
    std::vector<double> fluxVec_Y(Y,0);
    std::vector<double> fluxVec_Z(Z,0);

    if(dir == 'x' || dir == 'X' ) {
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=1;i<X-1;i++){
            for(long j=0;j<Y;j++){
                for(long k=0;k<Z;k++){
                    double localFlux_X = ( kX(i+1,j,k) * ( bcs->at(1)->getX_at(i+1,j,k,x)-x->at(i,j,k) ) - kX(i,j,k) * ( bcs->at(0)->getX_at(i-1,j,k,x)-x->at(i,j,k) ) );
                    if(localFlux_X!=localFlux_X){localFlux_X=0;}
                    fluxVec_X[i]+=localFlux_X;
                }
            }
        }

        long i = 0;
        for(long j=0;j<Y;j++){
            for(long k=0;k<Z;k++){
                double localFlux_X = ( kX(i+1,j,k) * ( bcs->at(1)->getX_at(i+1,j,k,x)-x->at(i,j,k) ) - kX(i,j,k) * ( ( -x->at(i,j,k)) - x->at(i,j,k) ) );
                if(localFlux_X!=localFlux_X){localFlux_X=0;}
                fluxVec_X[i]+=localFlux_X;
            }
        }

        i = X-1;
        for(long j=0;j<Y;j++){
            for(long k=0;k<Z;k++){
                double localFlux_X = ( kX(i+1,j,k) * ( (2 - x->at(i,j,k) ) - x->at(i,j,k) ) - kX(i,j,k) * ( bcs->at(0)->getX_at(i-1,j,k,x)-x->at(i,j,k) ) );
                if(localFlux_X!=localFlux_X){localFlux_X=0;}
                fluxVec_X[i]+=localFlux_X;
            }
        }

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long j=0;j<Y;j++){
            for(long i=0;i<X;i++){
                for(long k=0;k<Z;k++){
                    double localFlux_Y = ( kY(i,j+1,k) * ( bcs->at(3)->getX_at(i,j+1,k,x)-x->at(i,j,k) ) - kY(i,j,k) * ( bcs->at(2)->getX_at(i,j-1,k,x)-x->at(i,j,k) ) );
                    if(localFlux_Y!=localFlux_Y){localFlux_Y=0;}
                    fluxVec_Y[j]+=localFlux_Y;
                }
            }
        }

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long k=0;k<Z;k++){
            for(long i=0;i<X;i++){
                for(long j=0;j<Y;j++){
                    double localFlux_Z = ( kZ(i,j,k+1) * ( bcs->at(5)->getX_at(i,j,k+1,x)-x->at(i,j,k) ) - kZ(i,j,k) * ( bcs->at(4)->getX_at(i,j,k-1,x)-x->at(i,j,k) ) );
                    if(localFlux_Z!=localFlux_Z){localFlux_Z=0;}
                    fluxVec_Z[k]+=localFlux_Z;
                }
            }
        }
    }

    else if(dir == 'y' || dir == 'Y' ) {

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<X;i++){
            for(long j=0;j<Y;j++){
                for(long k=0;k<Z;k++){
                    double localFlux_X = ( kX(i+1,j,k) * ( bcs->at(1)->getX_at(i+1,j,k,x)-x->at(i,j,k) ) - kX(i,j,k) * ( bcs->at(0)->getX_at(i-1,j,k,x)-x->at(i,j,k) ) );
                    if(localFlux_X!=localFlux_X){localFlux_X=0;}
                    fluxVec_X[i]+=localFlux_X;
                }
            }
        }

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long j=1;j<Y-1;j++){
            for(long i=0;i<X;i++){
                for(long k=0;k<Z;k++){
                    double localFlux_Y = ( kY(i,j+1,k) * ( bcs->at(3)->getX_at(i,j+1,k,x)-x->at(i,j,k) ) - kY(i,j,k) * ( bcs->at(2)->getX_at(i,j-1,k,x)-x->at(i,j,k) ) );
                    if(localFlux_Y!=localFlux_Y){localFlux_Y=0;}
                    fluxVec_Y[j]+=localFlux_Y;
                }
            }
        }

        long j = 0;
        for(long i=0;i<X;i++){
            for(long k=0;k<Z;k++){
                double localFlux_Y = ( kY(i,j+1,k) * ( bcs->at(3)->getX_at(i,j+1,k,x)-x->at(i,j,k) ) - kY(i,j,k) * ( ( -x->at(i,j,k)) -x->at(i,j,k) ) );
                if(localFlux_Y!=localFlux_Y){localFlux_Y=0;}
                fluxVec_Y[j]+=localFlux_Y;
            }
        }


        j = Y-1;
        for(long i=0;i<X;i++){
            for(long k=0;k<Z;k++){
                double localFlux_Y = ( kY(i,j+1,k) * ( (2 - x->at(i,j,k) ) -x->at(i,j,k) ) - kY(i,j,k) * ( bcs->at(2)->getX_at(i,j-1,k,x)-x->at(i,j,k) ) );
                if(localFlux_Y!=localFlux_Y){localFlux_Y=0;}
                fluxVec_Y[j]+=localFlux_Y;
            }
        }

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long k=0;k<Z;k++){
            for(long i=0;i<X;i++){
                for(long j=0;j<Y;j++){
                    double localFlux_Z = ( kZ(i,j,k+1) * ( bcs->at(5)->getX_at(i,j,k+1,x)-x->at(i,j,k) ) - kZ(i,j,k) * ( bcs->at(4)->getX_at(i,j,k-1,x)-x->at(i,j,k) ) );
                    if(localFlux_Z!=localFlux_Z){localFlux_Z=0;}
                    fluxVec_Z[k]+=localFlux_Z;
                }
            }
        }
    }

    else if(dir == 'z' || dir == 'Z' ) {

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<X;i++){
            for(long j=0;j<Y;j++){
                for(long k=0;k<Z;k++){
                    double localFlux_X = ( kX(i+1,j,k) * ( bcs->at(1)->getX_at(i+1,j,k,x)-x->at(i,j,k) ) - kX(i,j,k) * ( bcs->at(0)->getX_at(i-1,j,k,x)-x->at(i,j,k) ) );
                    if(localFlux_X!=localFlux_X){localFlux_X=0;}
                    fluxVec_X[i]+=localFlux_X;
                }
            }
        }

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long j=0;j<Y;j++){
            for(long i=0;i<X;i++){
                for(long k=0;k<Z;k++){
                    double localFlux_Y = ( kY(i,j+1,k) * ( bcs->at(3)->getX_at(i,j+1,k,x)-x->at(i,j,k) ) - kY(i,j,k) * ( bcs->at(2)->getX_at(i,j-1,k,x)-x->at(i,j,k) ) );
                    if(localFlux_Y!=localFlux_Y){localFlux_Y=0;}
                    fluxVec_Y[j]+=localFlux_Y;
                }
            }
        }

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long k=1;k<Z-1;k++){
            for(long i=0;i<X;i++){
                for(long j=0;j<Y;j++){
                    double localFlux_Z = ( kZ(i,j,k+1) * ( bcs->at(5)->getX_at(i,j,k+1,x)-x->at(i,j,k) ) - kZ(i,j,k) * ( bcs->at(4)->getX_at(i,j,k-1,x)-x->at(i,j,k) ) );
                    if(localFlux_Z!=localFlux_Z){localFlux_Z=0;}
                    fluxVec_Z[k]+=localFlux_Z;
                }
            }
        }

        long k = 0;
        for(long i=0;i<X;i++){
            for(long j=0;j<Y;j++){
                double localFlux_Z = ( kZ(i,j,k+1) * ( bcs->at(5)->getX_at(i,j,k+1,x)-x->at(i,j,k) ) - kZ(i,j,k) * ( ( -x->at(i,j,k)) -x->at(i,j,k) ) );
                if(localFlux_Z!=localFlux_Z){localFlux_Z=0;}
                fluxVec_Z[k]+=localFlux_Z;
            }
        }

        k = Z-1;
        for(long i=0;i<X;i++){
            for(long j=0;j<Y;j++){
                double localFlux_Z = ( kZ(i,j,k+1) * (   (2 - x->at(i,j,k) ) -x->at(i,j,k) ) - kZ(i,j,k) * ( bcs->at(4)->getX_at(i,j,k-1,x)-x->at(i,j,k) ) );
                if(localFlux_Z!=localFlux_Z){localFlux_Z=0;}
                fluxVec_Z[k]+=localFlux_Z;
            }
        }

    }

    // MAC
    puma::Vec3<double> fluxes(0,0,0);

#ifdef MAC
    fluxes.x = accumulate(fluxVec_X.begin(), fluxVec_X.end(),0.0)/((X)*(Y)*(Z));
    fluxes.y = accumulate(fluxVec_Y.begin(), fluxVec_Y.end(),0.0)/((X)*(Y)*(Z));
    fluxes.z = accumulate(fluxVec_Z.begin(), fluxVec_Z.end(),0.0)/((X)*(Y)*(Z));
#elif LINUX
    fluxes.x = __gnu_parallel::accumulate(fluxVec_X.begin(), fluxVec_X.end(),0.0)/((X)*(Y)*(Z));
    fluxes.y = __gnu_parallel::accumulate(fluxVec_Y.begin(), fluxVec_Y.end(),0.0)/((X)*(Y)*(Z));
    fluxes.z = __gnu_parallel::accumulate(fluxVec_Z.begin(), fluxVec_Z.end(),0.0)/((X)*(Y)*(Z));
#endif

    return fluxes;
}



