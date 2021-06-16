#include "export_textfile.h"


bool puma::export_Textfile(Workspace *work, std::string fileName, bool header, bool scientific, bool showpositive, short precision, short width) {
    Export_Textfile_Workspace exporter(work,fileName,header,scientific,showpositive,precision,width);
    return exporter.execute();
}




