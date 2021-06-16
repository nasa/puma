#include "import_3DTiff.h"


bool puma::import_3DTiff(Workspace *work, std::string fileName, int numThreads) {
    Import_3DTiff_Workspace importer(work,std::move(fileName),numThreads);
    return importer.import();
}

bool puma::import_3DTiff(Workspace *work, std::string fileName, int xMin, int xMax, int yMin, int yMax, int zMin, int zMax, int numThreads) {
    Import_3DTiff_Workspace importer(work,std::move(fileName),xMin,xMax,yMin,yMax,zMin,zMax, numThreads);
    return importer.import();
}
