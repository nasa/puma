#ifndef Import_3DTiffGRAYSCALE_H
#define Import_3DTiffGRAYSCALE_H

#include "importdata.h"
#include "import_3DTiff.h"
#include "workspace.h"

#include "tiffio.h"
#include <utility>


class Import_3DTiff_Workspace : public Import
{
public:


    Import_3DTiff_Workspace(puma::Workspace *work, std::string fileName, int numThreads);
    Import_3DTiff_Workspace(puma::Workspace *work, std::string fileName, int xMin, int xMax, int yMin, int yMax, int zMin, int zMax, int numThreads);

    bool import() override;

private:

    puma::Workspace *work;
    int xMin{};
    int xMax{};
    int yMin{};
    int yMax{};
    int zMin{};
    int zMax{};
    int X{};
    int Y{};
    int Z{};
    std::string fileName;
    uint32 numThreads;
    bool sizeOfDomain();
    bool importHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};



#endif // Import_3DTiff_H

