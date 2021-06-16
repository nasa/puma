#include "export_3DTiff_Workspace.h"


Export_3DTiff_Workspace::Export_3DTiff_Workspace(puma::Workspace *work, std::string fileName, bool normalized) {
    this->work = work;
    this->fileName = fileName;
    this->normalized = normalized;

    if(this->work->size()==0) {
        empty = true;
    }
    else {
        empty = false;
    }

    numberOfPages = work->matrix.Z();
    width = work->matrix.X();
    height = work->matrix.Y();

    min = work->matrix.min();
    max = work->matrix.max();
}


bool Export_3DTiff_Workspace::execute() {

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

bool Export_3DTiff_Workspace::exportHelper() {

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
                    svec[i] = (uint8_t)((range*(work->matrix(i,y,page)-min)/(max-min)));
                }
                if (TIFFWriteScanline(output_image,&svec[0], y, 0)==-1) {
                    std::cout << "Complete or error" << std::endl;
                }
            }
        }
        else {
            for (int y = 0; y < height; y++) {
                for(int i=0;i<width;i++) {
                    svec[i] = (uint8_t)work->matrix(i,y,page);
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


bool Export_3DTiff_Workspace::errorCheck(std::string *errorMessage) {
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

bool Export_3DTiff_Workspace::logInput() {

    work->log->appendLogSection("Export 3D Tiff");
    work->log->appendLogLine(" -- Inputs:");
    work->log->appendLogItem("File Name: ");
    work->log->appendLogItem(fileName);
    work->log->appendLogItem("Domain Size: ");
    work->log->appendLogItem(work->matrix.X());
    work->log->appendLogItem(", ");
    work->log->appendLogItem(work->matrix.Y());
    work->log->appendLogItem(", ");
    work->log->appendLogItem(work->matrix.Z());
    work->log->newLine();
    work->log->writeLog();

    return true;
}

bool Export_3DTiff_Workspace::logOutput() {

    work->log->appendLogItem("Successful Export");
    work->log->newLine();
    work->log->writeLog();

    return true;
}
