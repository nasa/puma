#ifndef EXPORT_TEXTFILE_WORKSPACE_H
#define EXPORT_TEXTFILE_WORKSPACE_H

#include "export.h"
#include "workspace.h"


class Export_Textfile_Workspace :public Export
{

public:

    Export_Textfile_Workspace(puma::Workspace *work, std::string fileName, bool header, bool scientific, bool showpositive, short precision, short width);

    bool execute() override;

private:

    puma::Workspace *work;

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

#endif // EXPORT_TEXTFILE_WORKSPACE_H
