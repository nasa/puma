#ifndef Import_3DTiff_H
#define Import_3DTiff_H

#include "importdata.h"
#include "import_3DTiff_Workspace.h"
#include "workspace.h"
#include "tiffio.h"
#include <string>
#include <utility>


// Declare Global Import 3D Tiff Functions
namespace puma {

/*! Imports a 3D Tiff into the passed workspace.
\param work is a pointer to a workspace
\param fileName is a string that has the path to the tiff: ex: "/home/jsmith/Desktop/myTiff.tif"
\param numThreads: the number of threads used in the operation. 0 for maximum number of threads
*/
    bool import_3DTiff(Workspace *work, std::string fileName, int numThreads = 0);

/*! Imports a subsection 3D Tiff into the passed workspace.
\param work is a pointer to a workspace
\param fileName is a string that has the path to the tiff: ex: "/home/jsmith/Desktop/myTiff.tif"
\param xMin is the startIndex in the x direction. uses c++ convention is min value is 0 and max value is size-1
\param xMax is the EndIndex in the x direction. uses c++ convention is min value is 0 and max value is size-1
\param yMin is the startIndex in the y direction. uses c++ convention is min value is 0 and max value is size-1
\param yMax is the EndIndex in the y direction. uses c++ convention is min value is 0 and max value is size-1
\param zMin is the startIndex in the z direction. uses c++ convention is min value is 0 and max value is size-1
\param zMax is the EndIndex in the z direction. uses c++ convention is min value is 0 and max value is size-1
\param numThreads: the number of threads used in the operation. 0 for maximum number of threads
*/
    bool import_3DTiff(Workspace *work, std::string fileName, int xMin, int xMax, int yMin, int yMax, int zMin, int zMax, int numThreads = 0);

/*! Imports a 3D Tiff into the passed matrix.
 * \param matrix is a pointer to a matrix
 * \param fileName is a string that has the path to the tiff: ex: "/home/jsmith/Desktop/myTiff.tif"
 * \param numThreads is the number of threads used in the input. 0 defaults to max number of threads
 */
    template <class T>
    bool import_3DTiff(puma::Matrix<T> *matrix, std::string fileName, int numThreads = 0);

    std::string path_to_example_file(std::string example_filename);
}

// Import 3D Tiff Matrix Class
template<class T>
class Import_3DTiff_Matrix : public Import
{
public:

    Import_3DTiff_Matrix(puma::Matrix<T> *matrix, std::string fileName, int numThreads);

    bool import();

private:

    puma::Matrix<T> *matrix;
    int xMin;
    int xMax;
    int yMin;
    int yMax;
    int zMin;
    int zMax;
    int X;
    int Y;
    int Z;
    std::string fileName;
    int numThreads;

    bool importHelper();
    bool sizeOfDomain();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};

// Define Global Import 3D Tiff Matrix Function
template <class T>
bool puma::import_3DTiff(puma::Matrix<T> *matrix, std::string fileName, int numThreads) {
    Import_3DTiff_Matrix<T> importer(matrix,fileName,numThreads);
    return importer.import();
}

// Define Import 3D Tiff Matrix Member Functions
template<class T> Import_3DTiff_Matrix<T>::Import_3DTiff_Matrix(puma::Matrix<T> *matrix, std::string fileName, int numThreads) {
    this->matrix = matrix;
    this->fileName = fileName;
    this->numThreads = numThreads;

    sizeOfDomain();

}

template<class T> bool Import_3DTiff_Matrix<T>::import() {

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

template<class T> bool Import_3DTiff_Matrix<T>::importHelper() {

    long localX = xMax - xMin + 1;
    long localY = yMax - yMin + 1;
    long localZ = zMax - zMin + 1;


    TIFF* input_Image;
    TIFFSetWarningHandler(NULL);

    if((input_Image = TIFFOpen(fileName.c_str(), "r")) == NULL){
        std::cout << "Unable to read tif file: " << fileName << std::endl;
        return false;
    }

    matrix->resize(localX, localY, localZ);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (int page = (int)zMin; page <= (int)zMax; page++){
        TIFF* temp_Image = TIFFOpen(fileName.c_str(), "r");
        TIFFSetDirectory(temp_Image, page);

        std::vector<uint8_t> svec(X);
        for(int row =yMin;row<=yMax;row++){
            TIFFReadScanline(temp_Image, &svec[0], row,0);
            for(int i=xMin;i<=xMax;i++){
                matrix->operator ()(i-xMin,row-yMin,page-zMin) = svec[i];
            }
        }

        TIFFClose(temp_Image);
    }

    TIFFClose(input_Image);

    return true;
}

template<class T> bool Import_3DTiff_Matrix<T>::sizeOfDomain() {
    int bitsPerSample=-1;

    TIFF* input_Image;
    TIFFSetWarningHandler(NULL);
    if((input_Image = TIFFOpen(fileName.c_str(), "r")) == NULL){
        X=-1;
        return false;
    }

    if(input_Image) {
        TIFFGetField(input_Image, TIFFTAG_IMAGEWIDTH, &X);
        TIFFGetField(input_Image, TIFFTAG_IMAGELENGTH, &Y);
        TIFFGetField(input_Image, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
    }

    if(bitsPerSample!=8){
        if(bitsPerSample == 16 || bitsPerSample == 32){
            std::cout << "Image must be 8-bit" << std::endl;
            X=-1;
            return false;
        }
        else {
            bitsPerSample = 8; //seems to be a bug that happens occasionally where bitsPerSample isn't read correctly.
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

    return true;
}

template<class T> bool Import_3DTiff_Matrix<T>::logInput() {

    return true;
}

template<class T> bool Import_3DTiff_Matrix<T>::logOutput() {

    return true;
}

template<class T> bool Import_3DTiff_Matrix<T>::errorCheck(std::string *errorMessage) {
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

#endif // Import_3DTiff_H