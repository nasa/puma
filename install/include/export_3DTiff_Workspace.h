#ifndef EXPORT_3DTIFFWORKSPACE_H
#define EXPORT_3DTIFFWORKSPACE_H

#include "export.h"
#include "workspace.h"
#include "tiffio.h"


class Export_3DTiff_Workspace :public Export
{

public:
    Export_3DTiff_Workspace(puma::Workspace *work, std::string fileName, bool normalized);
    bool execute() override;

private:

    puma::Workspace *work;

    std::string fileName;
    bool normalized;

    int numberOfPages;
    int width;
    int height;

    float min;
    float max;
    float range;

    bool empty;

    bool normalizeArray();

    bool exportHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


#endif // EXPORT_3DTIFF_H
