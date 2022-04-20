#include "import_3DTiff.h"


bool puma::import_3DTiff(Workspace *work, std::string fileName, int numThreads) {
    Import_3DTiff_Workspace importer(work,std::move(fileName),numThreads);
    return importer.import();
}

bool puma::import_3DTiff(Workspace *work, std::string fileName, int xMin, int xMax, int yMin, int yMax, int zMin, int zMax, int numThreads) {
    Import_3DTiff_Workspace importer(work,std::move(fileName),xMin,xMax,yMin,yMax,zMin,zMax, numThreads);
    return importer.import();
}


std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

std::string puma::path_to_example_file(std::string example_filename){

    std::string command = "python -c 'import pumapy; print(pumapy.path_to_example_file(\"";
    command += example_filename;
    command += "\"))'";

    std::string out = exec(command.c_str());
    return out;
}
