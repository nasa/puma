#ifndef EXPORT_3DTIFF_H
#define EXPORT_3DTIFF_H

#include "export.h"
#include "workspace.h"
#include "pstring.h"
#include "export_3DTiff_Workspace.h"
#include "tiffio.h"


// Declare Global Export 3D Tiff Functions
namespace puma {


/** @brief Exports a puma::Workspace to a tif file
 *
 *  @param work Pointer to a puma::Workspace to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myWorkspace.tif
 *  @param normalized Bool value which allows user to normalize the workspace to between 0 and 255 (max range of 8bit tif) - True to normalize, false to leave as is.
 *  @return bool True if output was successful, False if an error occured.
 */
    bool export_3DTiff(Workspace *work, std::string fileName, bool normalized);

/** @brief Exports a puma::Matrix to a tif file
 *
 *  @param matrix Pointer to a puma::Matrix to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myMatrix.tif
 *  @param normalized Bool value which allows user to normalize the workspace to between 0 and 255 (max range of 8bit tif) - True to normalize, false to leave as is.
 *  @return bool True if output was successful, False if an error occured.
 */
    template<class T>
    bool export_3DTiff(Matrix<T> *matrix, const std::string& fileName, bool normalized);

}

// Export 3D Tiff Matrix Class
template<class T>
class Export_3DTiff_Matrix :public Export
{

public:

    Export_3DTiff_Matrix(puma::Matrix<T> *matrix, const std::string& fileName, bool normalized);

    bool execute() override;

private:

    puma::Matrix<T> *matrix;

    std::string fileName;
    bool normalized{};

    int numberOfPages;
    int width;
    int height;

    float min;
    float max;
    float range{};

    bool empty;

    bool exportHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};

// Define Global Export 3D Tiff Matrix Function
template<class T>
bool puma::export_3DTiff(Matrix<T> *matrix, const std::string& fileName, bool normalized) {
    Export_3DTiff_Matrix<T> exporter(matrix,fileName,normalized);
    return exporter.execute();
}

// Define Export 3D Tiff Matrix Member Functions
template<class T> Export_3DTiff_Matrix<T>::Export_3DTiff_Matrix(puma::Matrix<T> *matrix, const std::string& fileName, bool normalized) {
    this->matrix = matrix;
    this->fileName = fileName;
    this->normalized = normalized;

    empty = this->matrix->size() == 0;

    numberOfPages = matrix->Z();
    width = matrix->X();
    height = matrix->Y();

    min = matrix->min();
    max = matrix->max();

}


template<class T> bool Export_3DTiff_Matrix<T>::execute() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }


    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed
        std::cout << "Error in Export3D_Tiff: " << errorMessage << std::endl;
        return false;
    }

    std::cout << "Outputting tiff file: " << fileName << " ... " << std::flush;
    bool success = exportHelper();

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


template<class T> bool Export_3DTiff_Matrix<T>::exportHelper() {

    range = 255;

    const int samplesPerPixel = 1;
    const int bitsPerSample = 8;

    std::vector<uint8_t> svec(width);

    TIFF *output_image;

    if((output_image = TIFFOpen(fileName.c_str(), "w")) == NULL){
        std::cerr << "Unable to write tif file: " << "image.tiff" << std::endl;
        return false;
    }

    for (int page = 0; page < numberOfPages; page++) {

        TIFFSetField(output_image,TIFFTAG_IMAGEWIDTH, width);
        TIFFSetField(output_image,TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
        TIFFSetField(output_image,TIFFTAG_BITSPERSAMPLE, bitsPerSample);
        TIFFSetField(output_image,TIFFTAG_ORIENTATION, (int)ORIENTATION_TOPLEFT);
        TIFFSetField(output_image,TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);


        if (page % 2 == 0)
            TIFFSetField(output_image,TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
        else
            TIFFSetField(output_image,TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);

        TIFFSetField(output_image,TIFFTAG_ROWSPERSTRIP, height / samplesPerPixel);
        TIFFSetField(output_image,TIFFTAG_XRESOLUTION, 100.0);
        TIFFSetField(output_image,TIFFTAG_YRESOLUTION, 100.0);
        TIFFSetField(output_image,TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

        // specify that it's a page within the multipage file
        TIFFSetField(output_image,TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
        // specify the page number
        TIFFSetField(output_image,TIFFTAG_PAGENUMBER, page, numberOfPages);



        if(normalized) {
            for (int y = 0; y < height; y++) {
                for(int i=0;i<width;i++) {
                    svec[i] = (uint8_t)((range*(matrix->operator ()(i,y,page)-min)/(max-min)));
                }
                if (TIFFWriteScanline(output_image,&svec[0], y, 0)==-1) {
                    std::cout << "Complete or error" << std::endl;
                }
            }
        }
        else {
            for (int y = 0; y < height; y++) {
                for(int i=0;i<width;i++) {
                    svec[i] = (uint8_t)matrix->operator ()(i,y,page);
                }
                if (TIFFWriteScanline(output_image,&svec[0], y, 0)==-1) {
                    std::cout << "Complete or error" << std::endl;
                }
            }
        }


        TIFFWriteDirectory(output_image);
    }
    return true;

}


template<class T> bool Export_3DTiff_Matrix<T>::errorCheck(std::string *errorMessage) {

    if(empty) {
        *errorMessage = "Empty Matrix";
        return false;
    }

    if(fileName == "") {
        *errorMessage = "Blank File Name";
        return false;
    }

    if( !puma::PString::ends_with(fileName,".tif") ) {
        if( !puma::PString::ends_with(fileName,".tiff") ) {
            fileName.append(".tif");
        }
    }

    return true;
}


template<class T> bool Export_3DTiff_Matrix<T>::logInput() {

    return true;
}


template<class T> bool Export_3DTiff_Matrix<T>::logOutput() {

    return true;
}

#endif // EXPORT_3DTIFF_H