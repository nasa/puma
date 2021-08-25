#ifndef EXPORT_TEXTFILE_H
#define EXPORT_TEXTFILE_H

#include "export.h"
#include "workspace.h"
#include "pstring.h"
#include "export_textfile_workspace.h"

#include <fstream>


namespace puma {

/*! Exporting a workspace to a txt file
\param work: the workspace to export
\param fileName: the name of the file
\param header: add title header on first line
\param scientific: scientific notation
\param showpositive: show positive sign
\param precision: decimal place for rounding precision
\param width: text width
\return true for success or false for fail
*/
    bool export_Textfile(Workspace *work, std::string fileName, bool header=false, bool scientific=true, bool showpositive=false, short precision=6, short width=15);

/*! Exporting a matrix to a txt file
\param matrix: the matrix to export
\param fileName: the name of the file
\param header: add title header on first line
\param scientific: scientific notation
\param showpositive: show positive sign
\param precision: decimal place for rounding precision
\param width: text width
\return true for success or false for fail
*/
    template<class T>
    bool export_Textfile(Matrix<T> *matrix, std::string fileName, bool header=false, bool scientific=true, bool showpositive=false, short precision=6, short width=15);
}

template<class T>
class Export_Textfile_Matrix :public Export
{

public:

    Export_Textfile_Matrix(puma::Matrix<T> *matrix, std::string fileName, bool header, bool scientific, bool showpositive, short precision, short width);

    bool execute() override;

private:

    puma::Matrix<T> *matrix;

    std::string fileName;

    bool empty;
    bool header;
    bool scientific;
    bool showpositive;
    short precision;
    short width;

    bool exportHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};

template<class T>
bool puma::export_Textfile(Matrix<T> *matrix, std::string fileName, bool header, bool scientific, bool showpositive, short precision, short width) {
    Export_Textfile_Matrix<T> exporter(matrix,fileName,header,scientific,showpositive,precision,width);
    return exporter.execute();
}


template<class T> Export_Textfile_Matrix<T>::Export_Textfile_Matrix(puma::Matrix<T> *matrix, std::string fileName, bool header, bool scientific, bool showpositive, short precision, short width) {
    this->matrix = matrix;
    this->fileName = fileName;
    this->header = header;
    this->scientific = scientific;
    this->showpositive = showpositive;
    this->precision = precision;
    this->width = width;

    if(this->matrix->size()==0) {
        empty = true;
    }
    else {
        empty = false;
    }
}

template<class T> bool Export_Textfile_Matrix<T>::execute() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }


    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed
        std::cout << "Error in Export_Textfile: " << errorMessage << std::endl;
        return false;
    }

    if (precision > 18) {
        precision = 18;
    }
    else if (precision < 0) {
        precision = 0;
    }

    std::cout << "Outputting text file: " << fileName+".txt" << " ... " << std::flush;
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

template<class T> bool Export_Textfile_Matrix<T>::exportHelper() {

    std::ofstream file(fileName+".txt");
    if (file.is_open())
    {
        std::ios_base::fmtflags ff = file.flags();

        ff &= ~file.adjustfield;
        ff |= file.left;

        ff &= ~file.floatfield;
        if (scientific) {
            ff |= file.scientific;
        }
        else {
            ff |= file.fixed;
        }

        if (showpositive) {
            ff |= file.showpos;
        }

        if (header) {
            file.flags(ff);
            file.width(width);
            file.precision(precision);
            file << "i";
            file.flags(ff);
            file.width(width);
            file.precision(precision);
            file << "j";
            file.flags(ff);
            file.width(width);
            file.precision(precision);
            file << "k";
            file.flags(ff);
            file.width(width);
            file.precision(precision);
            file << "value" << "\n";
        }
        for (long k=0; k<matrix->Z(); k++) {
            for (long j=0; j<matrix->Y(); j++) {
                for (long i=0; i<matrix->X(); i++) {

                    file.flags(ff);
                    file.width(width);
                    file.precision(precision);
                    file << i;
                    file.flags(ff);
                    file.width(width);
                    file.precision(precision);
                    file << j;
                    file.flags(ff);
                    file.width(width);
                    file.precision(precision);
                    file << k;
                    file.flags(ff);
                    file.width(width);
                    file.precision(precision);
                    file << matrix->operator()(i,j,k) << "\n";
                }
            }
        }
        file.close();
    }
    else {
        return false;
    }

    return true;
}

template<class T> bool Export_Textfile_Matrix<T>::errorCheck(std::string *errorMessage) {

    if(empty) {
        *errorMessage = "Empty Matrix";
        return false;
    }

    if(fileName == "") {
        *errorMessage = "Blank File Name";
        return false;
    }

    if(puma::PString::ends_with(fileName,".txt")) {
        fileName = puma::PString::replaceAll(fileName,".txt","");
    }

    return true;
}


template<class T> bool Export_Textfile_Matrix<T>::logInput() {

    return true;
}


template<class T> bool Export_Textfile_Matrix<T>::logOutput() {

    return true;
}

#endif // EXPORT_TEXTFILE_H