#include "import_3DTiff_Workspace.h"


Import_3DTiff_Workspace::Import_3DTiff_Workspace(puma::Workspace *work, std::string fileName, int numThreads) {
    this->work = work;
    this->fileName = std::move(fileName);
    this->numThreads = numThreads;

    sizeOfDomain();
}

Import_3DTiff_Workspace::Import_3DTiff_Workspace(puma::Workspace *work, std::string fileName, int xMin, int xMax, int yMin, int yMax, int zMin, int zMax, int numThreads) {
    this->work = work;
    this->fileName = std::move(fileName);
    this->numThreads = numThreads;

    sizeOfDomain();
    this->xMin = xMin;
    this->xMax = xMax;
    this->yMin = yMin;
    this->yMax = yMax;
    this->zMin = zMin;
    this->zMax = zMax;
}

bool Import_3DTiff_Workspace::import() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    if(X==-1) { // SizeOfDomain failed;
        return false;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed
        std::cout << "Error in Import3D_Tiff: " << errorMessage << std::endl;
        return false;
    }

    std::cout << "Reading Tiff file: " << fileName << " ... " << std::flush;
    bool success = importHelper();

    if(!success) {
        return false;
    }

    if( !logOutput() ) {
        std::cout << "Failed to log Outputs" << std::endl;
        return false;
    }

    std::cout << "Done" << std::endl;

    return true;

}

bool Import_3DTiff_Workspace::sizeOfDomain() {
    uint32 bitsPerSample = 0;
    uint16 rgb_check;

    TIFF* input_Image;
    TIFFSetWarningHandler(nullptr);
    if((input_Image = TIFFOpen(fileName.c_str(), "r")) == nullptr){
        X=-1;
        return false;
    }


    if(input_Image) {
        TIFFGetField(input_Image, TIFFTAG_IMAGEWIDTH, &X);
        TIFFGetField(input_Image, TIFFTAG_IMAGELENGTH, &Y);
        TIFFGetField(input_Image, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
        TIFFGetField(input_Image, TIFFTAG_SAMPLESPERPIXEL, &rgb_check);
    }

    if(rgb_check == 3){
        std::cout << "Image must be grayscale, input is RGB." << std::endl;
        X=-1;
        return false;
    }

    if(bitsPerSample!=8){
        if(bitsPerSample == 16 || bitsPerSample == 32){
            std::cout << "Image must be 8-bit" << std::endl;
            X=-1;
            return false;
        }
        else {
            bitsPerSample = 8;
        }
    }

    int page=0;
    while(true){
        TIFFSetDirectory(input_Image, page);
        page++;
        if(!TIFFReadDirectory(input_Image)) {
            break;
        }
    }
    Z=page;

    xMin = 0;
    yMin = 0;
    zMin = 0;
    xMax = X-1;
    yMax = Y-1;
    zMax = Z-1;

    TIFFClose(input_Image);

    return true;
}

bool Import_3DTiff_Workspace::importHelper() {

    long localX = xMax - xMin + 1;
    long localY = yMax - yMin + 1;
    long localZ = zMax - zMin + 1;

    work->matrix.resize(localX, localY, localZ);

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (int page = (int) zMin; page <= (int) zMax; page++) {
            TIFF *temp_Image = TIFFOpen(fileName.c_str(), "r");
            TIFFSetDirectory(temp_Image, page);

            std::vector<uint8_t> svec(X);
            for (int row = yMin; row <= yMax; row++) {
                TIFFReadScanline(temp_Image, &svec[0], row, 0);
                for (int i = xMin; i <= xMax; i++) {
                    work->matrix(i - xMin, row - yMin, page - zMin) = svec[i];
                }
            }
            TIFFClose(temp_Image);
        }

    return true;
}



bool Import_3DTiff_Workspace::logInput() {
    puma::Logger *log = work->log;

    (*log).appendLogSection("Import 3D Tiff");
    (*log).appendLogLine(" -- Inputs:");
    (*log).appendLogItem("File Name: ");
    (*log).appendLogItem(fileName);
    (*log).appendLogItem("Domain Size: ");
    (*log).appendLogItem(X);
    (*log).appendLogItem(", ");
    (*log).appendLogItem(Y);
    (*log).appendLogItem(", ");
    (*log).appendLogItem(Z);
    (*log).newLine();
    (*log).appendLogItem("X Range: ");
    (*log).appendLogItem(xMin);
    (*log).appendLogItem(", ");
    (*log).appendLogItem(xMax);
    (*log).newLine();
    (*log).appendLogItem("Y Range: ");
    (*log).appendLogItem(yMin);
    (*log).appendLogItem(", ");
    (*log).appendLogItem(yMax);
    (*log).newLine();
    (*log).appendLogItem("Z Range: ");
    (*log).appendLogItem(zMin);
    (*log).appendLogItem(", ");
    (*log).appendLogItem(zMax);
    (*log).newLine();
    (*log).writeLog();

    return true;
}

bool Import_3DTiff_Workspace::logOutput() {
    puma::Logger *log = work->log;

    (*log).appendLogItem("Successful Import");
    (*log).newLine();
    (*log).writeLog();

    return true;
}

bool Import_3DTiff_Workspace::errorCheck(std::string *errorMessage) {
    if(xMin<0) { *errorMessage = "Invalid xMin. Must be > 0"; return false; }
    if(xMin>xMax) { *errorMessage = "Invalid xMin&xMax. xMin must be <= xMax"; return false; }
    if(xMax>=X) { *errorMessage = "Invalid xMax. Must be < total domain size in X. (Because of c++ indexing, an xMax of X-1 is the full domain)"; return false; }
    if(yMin<0) { *errorMessage = "Invalid yMin. Must be > 0"; return false; }
    if(yMin>yMax) { *errorMessage = "Invalid yMin&yMax. yMin must be <= yMax"; return false; }
    if(yMax>=Y) { *errorMessage = "Invalid yMax. Must be < total domain size in Y. (Because of c++ indexing, an yMax of Y-1 is the full domain)"; return false; }
    if(zMin<0) { *errorMessage = "Invalid zMin. Must be > 0"; return false; }
    if(zMin>zMax) { *errorMessage = "Invalid zMin&zMax. zMin must be <= zMax"; return false; }
    if(zMax>=Z) { *errorMessage = "Invalid zMax. Must be < total domain size in Z. (Because of c++ indexing, an zMax of Z-1 is the full domain)"; return false; }
    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    return true;
}

