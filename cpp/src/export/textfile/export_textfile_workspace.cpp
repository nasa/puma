#include "export_textfile_workspace.h"


Export_Textfile_Workspace::Export_Textfile_Workspace(puma::Workspace *work, std::string fileName, bool header, bool scientific, bool showpositive, short precision, short width) {
    this->work = work;
    this->fileName = fileName;
    this->header = header;
    this->scientific =scientific;
    this->showpositive = showpositive;
    this->precision = precision;
    this->width = width;

    if(work->matrix.size()==0) {
        empty = true;
    }
    else {
        empty = false;
    }
}


bool Export_Textfile_Workspace::execute() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed

        std::cout << "Error in ExportTextfile: " << errorMessage << std::endl;
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

bool Export_Textfile_Workspace::exportHelper() {
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
        for (long k=0; k<work->matrix.Z(); k++) {
            for (long j=0; j<work->matrix.Y(); j++) {
                for (long i=0; i<work->matrix.X(); i++) {

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
                    file << work->matrix.operator()(i,j,k) << "\n";
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


bool Export_Textfile_Workspace::errorCheck(std::string *errorMessage) {
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

bool Export_Textfile_Workspace::logInput() {

    work->log->appendLogSection("Export Textfile");
    work->log->appendLogLine(" -- Inputs:");
    work->log->appendLogItem("File Name: ");
    work->log->appendLogItem(fileName);
    work->log->newLine();
    work->log->appendLogItem("Domain Size: ");
    work->log->appendLogItem(work->matrix.X());
    work->log->appendLogItem(", ");
    work->log->appendLogItem(work->matrix.Y());
    work->log->appendLogItem(", ");
    work->log->appendLogItem(work->matrix.Z());
    work->log->newLine();
    work->log->appendLogItem("Header: ");
    if (header) {work->log->appendLogItem("Yes");}
    else {work->log->appendLogItem("No");}
    work->log->newLine();
    work->log->writeLog();
    return true;
}

bool Export_Textfile_Workspace::logOutput() {

    work->log->appendLogItem("Successful Export");
    work->log->newLine();
    work->log->writeLog();

    return true;
}