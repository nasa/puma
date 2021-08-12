#ifndef EXPORT_STL_H
#define EXPORT_STL_H

#include "export.h"
#include "workspace.h"
#include "vector.h"
#include "triangle.h"
#include "export_STL_helper.h"
#include "isosurface.h"

#include <utility>
#include <vector>


namespace puma {

/** @brief Exports a vector of puma::Triangles to an STL file.
 *
 *  @param triangles Pointer to the triangle data structure to be exported.
 *  @param Ascii False if exporting as binary STL, true if exporting at ASCII stl. Typically false.
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myTriangles.stl.
 *  @return bool True if output was successful, False if an error occured.
 */
bool export_STL(std::vector< puma::Triangle<float> > *triangles, bool Ascii, std::string fileName);

/** @brief Applies Marching Cubes to a puma::Workspace and exports the traingles to an STL file.
 *
 *  @param work Pointer to the workspace to be exported.
 *  @param lowValue Low Grayscale cutoff for material phase. For example FiberForm in pumapy/data/200_fiberform.tif is 87.
 *  @param lowValue High Grayscale cutoff for material phase. Often 255.
 *  @param Ascii False if exporting as binary STL, true if exporting at ASCII stl. Typically false.
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myTriangles.stl.
 *  @return bool True if output was successful, False if an error occured.
 */
bool export_STL(Workspace *work, puma::Cutoff cutoff, bool Ascii, std::string fileName);
}

class Export_STL : public Export
{
public:

    Export_STL(std::vector< puma::Triangle<float> > *triangles, bool Ascii, std::string fileName);

    bool execute() override;

private:

    std::vector< puma::Triangle<float> > *triangles;

    std::string fileName;
    bool Ascii;

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};



#endif // EXPORT_STL_H
