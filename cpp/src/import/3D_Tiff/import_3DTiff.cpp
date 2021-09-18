#include "import_3DTiff.h"


bool puma::import_3DTiff(Workspace *work, std::string fileName, int numThreads) {
    Import_3DTiff_Workspace importer(work,std::move(fileName),numThreads);
    return importer.import();
}

bool puma::import_3DTiff(Workspace *work, std::string fileName, int xMin, int xMax, int yMin, int yMax, int zMin, int zMax, int numThreads) {
    Import_3DTiff_Workspace importer(work,std::move(fileName),xMin,xMax,yMin,yMax,zMin,zMax, numThreads);
    return importer.import();
}

std::string puma::path_to_example_file(std::string example_filename){
    std::string path = __FILE__;
    std::string filepathname = path.substr(0, path.rfind("\\"));

    std::size_t last_slash = filepathname.find_last_of("/");
    std::string dir = filepathname.substr(0, last_slash);
    last_slash = dir.find_last_of("/");
    dir = filepathname.substr(0, last_slash);
        last_slash = dir.find_last_of("/");
    dir = filepathname.substr(0, last_slash);
        last_slash = dir.find_last_of("/");
    dir = filepathname.substr(0, last_slash);
        last_slash = dir.find_last_of("/");
    dir = filepathname.substr(0, last_slash);


    std::cout << dir << std::endl;
    return dir + "/python/pumapy/data/" + example_filename;
}
