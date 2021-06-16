#include "export_3DTiff.h"


bool puma::export_3DTiff(Workspace *work, std::string fileName, bool normalized) {
    Export_3DTiff_Workspace exporter(work,fileName,normalized);
    return exporter.execute();
}
