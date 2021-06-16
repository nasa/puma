#include "structuretensor.h"


////// STRUCTURE TENSOR METHOD //////

bool puma::compute_orientationST(Workspace *WS, double dogSigma, double gausRho, puma::Cutoff cutoff,
                                 puma::MatVec3<double> *direction, bool print, int numThreads) {

    StructureTensor solver(WS,dogSigma,gausRho,cutoff,direction,print,numThreads);
    return solver.computeGradient();
}


// lowpuma::Cutoff and highpuma::Cutoff are used for both selecting material from a grayscaleWS
// or to select the solid segmented phase, or individual tows (or a range of them)
// the ST filter is applied on the whole image, the cutoffs are only applied when
// computing the Eigenvectors to put in the direction MatVec3
StructureTensor::StructureTensor(puma::Workspace *WS, double dogSigma, double gausRho, puma::Cutoff cutoff, puma::MatVec3<double> *direction, bool print, int numThreads){
    this->WS = WS;
    this->numThreads = numThreads;
    this->print = print;
    this->dogSigma = dogSigma;
    this->gausRho = gausRho;
    this->cutoff = cutoff;
    this->direction = direction;
    X = (int)WS->X();
    Y = (int)WS->Y();
    Z = (int)WS->Z();
}


bool StructureTensor::computeGradient(){

    logInput();

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        std::cout << "Gradient Fiber Direction Method Error: " <<  errorMessage << std::endl;
        return false;
    }

    // Declaring all required big puma matrices (using the least number of them)
    puma::Matrix<double> gy_gpzz(X,Y,Z), gz_gpxxgauss(X,Y,Z), img_gpxx_gpxygauss(X,Y,Z), gpxy_gpxzgauss(X,Y,Z), gpxz_gpyygauss(X,Y,Z), gx_gpyy_gpyzgauss(X,Y,Z), gpyz_gpzzgauss(X,Y,Z);

    if (print) { std::cout << "Start Computation of Structure Tensors:" << std::endl; }

    if (print) { std::cout << "Computing Derivative of Gaussian kernels" << std::endl; }
    // generate derivative of Gaussian (dog) kernel
    puma::Matrix<double> dogkerny;
    kerngen(&dogkerny, 2);

    // Permutation kernels
    int dim = 2*(int)ceil(3.*dogSigma)+1;
    puma::Matrix<double> dogkernx(dim,dim,dim), dogkernz(dim,dim,dim);
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<dim;i++){
        for(int j=0;j<dim;j++){
            for(int k=0;k<dim;k++){
                dogkernx.at(j,i,k) = dogkerny.at(i,j,k); // permute [2,1,3]
                dogkernz.at(i,k,j) = dogkerny.at(i,j,k); // permute [1,3,2]
            }
        }
    }

    // passing workspace to puma matrix
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<X;i++){
        for(int j=0;j<Y;j++){
            for(int k=0;k<Z;k++){
                img_gpxx_gpxygauss.at(i,j,k) = (double)WS->at(i,j,k); // img
            }
        }
    }

    // compute gradients (convolve WS with filter (NB double check xx,yy))
    if (print) { std::cout << "1/3 convolution ... " << std::flush; }
    convn(&img_gpxx_gpxygauss, &dogkernx, &gx_gpyy_gpyzgauss, 's'); // img, x, gx
    if (print) { std::cout << "Done" << std::endl; }
    if (print) { std::cout << "2/3 convolution ... " << std::flush; }
    convn(&img_gpxx_gpxygauss, &dogkerny, &gy_gpzz, 's');           // img, y, gy
    if (print) { std::cout << "Done" << std::endl; }
    if (print) { std::cout << "3/3 convolution ... " << std::flush; }
    convn(&img_gpxx_gpxygauss, &dogkernz, &gz_gpxxgauss, 's');      // img, z, gz
    if (print) { std::cout << "Done" << std::endl; }

    // compute gradient products
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<X;i++){
        for(int j=0;j<Y;j++){
            for(int k=0;k<Z;k++){
                img_gpxx_gpxygauss.at(i,j,k) = gx_gpyy_gpyzgauss.at(i,j,k) * gx_gpyy_gpyzgauss.at(i,j,k); // gpxx = gx * gx
                gpxy_gpxzgauss.at(i,j,k) = gx_gpyy_gpyzgauss.at(i,j,k) * gy_gpzz.at(i,j,k);               // gpxy = gx * gy
                gpxz_gpyygauss.at(i,j,k) = gx_gpyy_gpyzgauss.at(i,j,k) * gz_gpxxgauss.at(i,j,k);          // gpxz = gx * gz
                gx_gpyy_gpyzgauss.at(i,j,k) = gy_gpzz.at(i,j,k) * gy_gpzz.at(i,j,k);                      // gpyy = gy * gy
                gpyz_gpzzgauss.at(i,j,k) = gy_gpzz.at(i,j,k) * gz_gpxxgauss.at(i,j,k);                    // gpyz = gy * gz
                gy_gpzz.at(i,j,k) = gz_gpxxgauss.at(i,j,k) * gz_gpxxgauss.at(i,j,k);                      // gpzz = gz * gz
            }
        }
    }

    if (print) { std::cout << "Start Gauss Sigma" << std::endl; }
    // generate gaussian kernel
    puma::Matrix<double> gausskern;
    kerngen(&gausskern, 1);

    // blur the gradient products with a Gaussian filter
    if (print) { std::cout << "1/6 convolution ... " << std::flush; }
    convn(&img_gpxx_gpxygauss, &gausskern, &gz_gpxxgauss, 's');   // gpxx, gauss, gpxxgauss
    if (print) { std::cout << "Done" << std::endl; }
    if (print) { std::cout << "2/6 convolution ... " << std::flush; }
    convn(&gpxy_gpxzgauss, &gausskern, &img_gpxx_gpxygauss, 's'); // gpxy, gauss, gpxygauss
    if (print) { std::cout << "Done" << std::endl; }
    if (print) { std::cout << "3/6 convolution ... " << std::flush; }
    convn(&gpxz_gpyygauss, &gausskern, &gpxy_gpxzgauss, 's');     // gpxz, gauss, gpxzgauss
    if (print) { std::cout << "Done" << std::endl; }
    if (print) { std::cout << "4/6 convolution ... " << std::flush; }
    convn(&gx_gpyy_gpyzgauss, &gausskern, &gpxz_gpyygauss, 's');  // gpyy, gauss, gpyygauss
    if (print) { std::cout << "Done" << std::endl; }
    if (print) { std::cout << "5/6 convolution ... " << std::flush; }
    convn(&gpyz_gpzzgauss, &gausskern, &gx_gpyy_gpyzgauss, 's');  // gpyz, gauss, gpyzgauss
    if (print) { std::cout << "Done" << std::endl; }
    if (print) { std::cout << "6/6 convolution ... " << std::flush; }
    convn(&gy_gpzz, &gausskern, &gpyz_gpzzgauss, 's');            // gpzz, gauss, gpzzgauss
    if (print) { std::cout << "Done" << std::endl; }

    if (print) { std::cout << "Start Eigen Calculation" << std::endl; }
    // Pick Eigenvector related to the smallest Eigenvalue
    direction->resize(X,Y,Z,puma::Vec3<double>(0,0,0));
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<X;i++){
        Eigen::Matrix<double, 3, 3> localTensor;
        double min_evalue;
        int min_evalueposition;
        for(int j=0;j<Y;j++){
            for(int k=0;k<Z;k++){
                if (WS->at(i,j,k)>=cutoff.first && WS->at(i,j,k)<=cutoff.second) {
                    localTensor(0,0) = gz_gpxxgauss.at(i,j,k), localTensor(0,1) = img_gpxx_gpxygauss.at(i,j,k), localTensor(0,2) = gpxy_gpxzgauss.at(i,j,k);      // gpxxgauss gpxygauss gpxzgauss
                    localTensor(1,0) = img_gpxx_gpxygauss.at(i,j,k), localTensor(1,1) = gpxz_gpyygauss.at(i,j,k), localTensor(1,2) = gx_gpyy_gpyzgauss.at(i,j,k); // gpxygauss gpyygauss gpyzgauss
                    localTensor(2,0) = gpxy_gpxzgauss.at(i,j,k), localTensor(2,1) = gx_gpyy_gpyzgauss.at(i,j,k), localTensor(2,2) = gpyz_gpzzgauss.at(i,j,k);     // gpxzgauss gpyzgauss gpzzgauss

                    Eigen::EigenSolver<Eigen::Matrix<double, 3,3> > s(localTensor);
                    min_evalue = (s.eigenvalues()[0]).real();
                    min_evalueposition = 0;
                    for(int i2=1;i2<3;i2++){
                        if (min_evalue > (s.eigenvalues()[i2]).real()){
                            min_evalue = (s.eigenvalues()[i2]).real();
                            min_evalueposition = i2;
                        }
                    }
                    direction->at(i,j,k).x = (s.eigenvectors()(0,min_evalueposition)).real();
                    direction->at(i,j,k).y = (s.eigenvectors()(1,min_evalueposition)).real();
                    direction->at(i,j,k).z = (s.eigenvectors()(2,min_evalueposition)).real();
                }
            }
        }
    }

    logOutput();

    return true;
}




// Function to generate either the derivative of Gaussian (2) or Gaussian (1) kernels
void StructureTensor::kerngen(puma::Matrix<double> *filter, int derivative){

    int halfsize;
    if(derivative==2){
        halfsize = (int)ceil(3.*dogSigma);
        if (print) { std::cout << "Gaussian derivative with Kernel size: " << (halfsize*2+1) << " voxels (sigma=" << dogSigma <<")" << std::endl; }
    } else {
        halfsize = (int)ceil(3.*gausRho);
        if (print) { std::cout << "Gaussian filter with Kernel size: " << (halfsize*2+1) << " voxels (rho=" << gausRho <<")" << std::endl; }
    }

    std::vector<double> x;
    for(int i=0;i<=halfsize*2; i++){
        x.push_back((double)(i-halfsize));
    }
    int dim = x.size();
    filter->resize(dim,dim,dim);

    if(derivative==2){ // derivative of Gaussian kernels
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(int i=0;i<dim;i++){
            for(int j=0;j<dim;j++){
                for(int k=0;k<dim;k++){
                    filter->at(i,j,k) = -x[j] * exp(-pow(x[j],2) / (2*pow(dogSigma,2))) *
                            exp(-pow(x[i],2) / (2*pow(dogSigma,2))) * exp(-pow(x[k],2) / (2*pow(dogSigma,2)));
                }
            }
        }
    } else { // Gaussian kernels
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(int i=0;i<dim;i++){
            for(int j=0;j<dim;j++){
                for(int k=0;k<dim;k++){
                    filter->at(i,j,k) = exp(-pow(x[j],2) / (2*pow(gausRho,2))) *
                            exp(-pow(x[i],2) / (2*pow(gausRho,2))) * exp(-pow(x[k],2) / (2*pow(gausRho,2)));
                }
            }
        }
    }
    double sum=0;
    for(int i=0;i<dim;i++){
        for(int j=0;j<dim;j++){
            for(int k=0;k<dim;k++){
                sum +=  std::abs(filter->at(i,j,k));
            }
        }
    }
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<dim;i++){
        for(int j=0;j<dim;j++){
            for(int k=0;k<dim;k++){
                filter->at(i,j,k) /= sum;
            }
        }
    }
}


// Convolution function
void StructureTensor::convn(puma::Matrix<double> *img, puma::Matrix<double> *filter, puma::Matrix<double> *img_out, char bc){

    int kern_size = (int)filter->X(), kern_origin = floor((double)kern_size/2);
    puma::Matrix<double> flipped_kern(kern_size,kern_size,kern_size);

    // flipping kernel
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<kern_size;i++){
        for(int j=0;j<kern_size;j++){
            for(int k=0;k<kern_size;k++){
                flipped_kern.at(i,j,k) = filter->at(kern_size-i-1,kern_size-j-1,kern_size-k-1);
            }
        }
    }

    if(bc=='c'){ // Cropping kernel at the boundary

        // image for loop
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(int i=0;i<X;i++){
            int i_start, j_start, k_start, i_end, j_end, k_end;
            double acc;
            for(int j=0;j<Y;j++){
                for(int k=0;k<Z;k++){

                    // setting start points for kernel
                    i_start = 0, j_start = 0, k_start = 0;
                    if(i<kern_origin){
                        i_start = kern_origin - i;
                        if(j<kern_origin){
                            j_start = kern_origin - j;
                            if(k<kern_origin){
                                k_start = kern_origin - k;
                            }
                        }
                        if(k<kern_origin){
                            k_start = kern_origin - k;
                        }
                    } else if(j<kern_origin) {
                        j_start = kern_origin - j;
                        if(k<kern_origin){
                            k_start = kern_origin - k;
                        }
                    } else if(k<kern_origin){
                        k_start = kern_origin - k;
                    }

                    // setting end points for kernel
                    i_end = kern_size, j_end = kern_size, k_end = kern_size;
                    if(i>=(X-kern_origin)){
                        i_end = X - i + kern_origin;
                        if(j>=(Y-kern_origin)){
                            j_end = Y - j + kern_origin;
                            if(k>=(Z-kern_origin)){
                                k_end = Z - k + kern_origin;
                            }
                        }
                        if(k>=(Z-kern_origin)){
                            k_end = Z - k + kern_origin;
                        }
                    } else if(j>=(Y-kern_origin)) {
                        j_end = Y - j + kern_origin;
                        if(k>=(Z-kern_origin)){
                            k_end = Z - k + kern_origin;
                        }
                    } else if(k>=(Z-kern_origin)){
                        k_end = Z - k + kern_origin;
                    }

                    // filter kernel for
                    acc = 0;
                    for(int i_kern=i_start;i_kern<i_end;i_kern++){
                        for(int j_kern=j_start;j_kern<j_end;j_kern++){
                            for(int k_kern=k_start;k_kern<k_end;k_kern++){
                                acc += flipped_kern.at(i_kern,j_kern,k_kern)*img->at(i-(kern_origin-i_kern),j-(kern_origin-j_kern),k-(kern_origin-k_kern));
                            }
                        }
                    }
                    img_out->at(i,j,k) = acc;
                }
            }
        }

    } else if (bc == 's') { // Symmetric boundary conditions (reflect image)

        for(int i=0;i<X;i++){

            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for(int j=0;j<Y;j++){
                int i_start, j_start, k_start, i_end, j_end, k_end;
                double acc;
                for(int k=0;k<Z;k++){

                    // setting start and end points for kernel
                    i_start = 0, j_start = 0, k_start = 0;
                    i_end = kern_size, j_end = kern_size, k_end = kern_size;

                    // filter kernel for
                    acc = 0;
                    for(int i_kern=i_start;i_kern<i_end;i_kern++){
                        for(int j_kern=j_start;j_kern<j_end;j_kern++){
                            for(int k_kern=k_start;k_kern<k_end;k_kern++){
                                acc += flipped_kern.at(i_kern,j_kern,k_kern) *
                                        img->at(indexAt(i-(kern_origin-i_kern),(int)img->X()), indexAt(j-(kern_origin-j_kern),(int)img->Y()), indexAt(k-(kern_origin-k_kern),(int)img->Z()));
                            }
                        }
                    }
                    img_out->at(i,j,k) = acc;
                }
            }
        }
    }
}

// Symmetric BC
long StructureTensor::indexAt(int index, int size) {

    if(index<0) index=abs(index);
    if(index>=size) index=size-(index-size+1);

    return index;
}



bool StructureTensor::logInput() {
    puma::Logger *logger = WS->log;

    logger->appendLogSection("Compute Orientation using Structure Tensor");
    logger->appendLogLine(" -- Inputs:");
    logger->appendLogItem("interior Size: ");

    logger->appendLogItem(WS->X());
    logger->appendLogItem(", ");
    logger->appendLogItem(WS->Y());
    logger->appendLogItem(", ");
    logger->appendLogItem(WS->Z());
    logger->newLine();


    logger->appendLogItem("Kernel Sizes: ");
        logger->appendLogItem("Sigma: ");
        logger->appendLogItem(dogSigma);
        logger->appendLogItem("Rho: ");
        logger->appendLogItem(gausRho);
        logger->newLine();
    logger->writeLog();

    return true;
}


bool StructureTensor::logOutput() {
    puma::Logger *logger = WS->log;

    logger->appendLogItem("Completed Orientation computation using the Structure Tensor (ST) method");

    logger->newLine();
    logger->writeLog();

    return true;
}


bool StructureTensor::errorCheck(std::string *errorMessage) {
    if(WS->matrix.size() == 0) {
        *errorMessage = "Empty Material Matrix";
        return false;
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    if(dogSigma<=0 || gausRho<=0) {
        *errorMessage = "Kernel window size must be > 0";
        return false;
    }

    if(cutoff.first > cutoff.second) {
        *errorMessage = "Low cutoff cannot be higher than the high one";
        return false;
    }
    if(cutoff.first<=-1 || cutoff.second<=-1) {
        *errorMessage = "puma::Cutoff cannot be negative";
        return false;
    }

    if(gausRho<=0 || dogSigma<=0) {
        *errorMessage = "Kernel sizes standard deviations have to be > 0";
        return false;
    }

    return true;
}
