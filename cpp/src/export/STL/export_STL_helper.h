#ifndef EXPORT_STL_HELPER_H
#define EXPORT_STL_HELPER_H

#include "vector.h"
#include "triangle.h"
#include "fio.h"

#include <fstream>
#include <vector>
#include <utility>


class Export_STL_helper
{
public:
    static bool writeSTL(std::vector< puma::Triangle<float> > *Triangles, std::string fileName);
    static bool writeSTLAscii(std::vector< puma::Triangle<float> > *Triangles, const std::string& fileName);

private:

    static void header(int fd,const std::string& mesg);
    static void calcnormal(float* tri, puma::Vec3<float>* norm);

};



#endif // EXPORT_STL_H
