#include "export_STL_helper.h"


bool Export_STL_helper::writeSTL(std::vector<puma::Triangle<float> > *Triangles, std::string fileName) {

    size_t comp_per_vert=3;
    size_t verts_per_tri=3;
    size_t floats_per_tri=comp_per_vert*verts_per_tri;

    std::string outfile = std::move(fileName);

    int ntris = Triangles->size();

    int fout=open(outfile.c_str(),O_WRONLY|O_TRUNC|O_CREAT,0644);

    if( fout == -1 ) {
        return false;
    }

    header(fout,"created by tri2stl");
    fio::put(fout,(const char*)&ntris,sizeof(int));

    std::vector<float> tris(ntris * floats_per_tri);

    int trisC=0;
    for(auto & Triangle : *Triangles){
        tris[trisC++]=Triangle.p0.x;
        tris[trisC++]=Triangle.p0.y;
        tris[trisC++]=Triangle.p0.z;
        tris[trisC++]=Triangle.p1.x;
        tris[trisC++]=Triangle.p1.y;
        tris[trisC++]=Triangle.p1.z;
        tris[trisC++]=Triangle.p2.x;
        tris[trisC++]=Triangle.p2.y;
        tris[trisC++]=Triangle.p2.z;
    }

    unsigned short dummy=0;
    for(int tidx=0;tidx<ntris;++tidx){
        float* tri = &tris[ tidx * floats_per_tri ];

        puma::Vec3<float> norm;
        calcnormal(tri,&norm);


        fio::put(fout,(const char*)&norm.x,3*sizeof(float));
        fio::put(fout,(const char*)tri,floats_per_tri*sizeof(float));

        fio::put(fout,(const char*)&dummy,sizeof(unsigned short));
    }

    close(fout);

    return true;
}

bool Export_STL_helper::writeSTLAscii(std::vector<puma::Triangle<float> > *Triangles, const std::string& fileName) {
    std::ofstream myfile;

    myfile.open(fileName);

    if(!myfile.is_open()) {
        return false;
    }

    //myfile << output;

    myfile << "solid MYSOLID\n";

    for (auto & Triangle : *Triangles){

        std::vector<float> norm(3);
        norm[0] = (((Triangle.p1 - Triangle.p0).cross(Triangle.p2 - Triangle.p0)).norm()).x;
        norm[1] = (((Triangle.p1 - Triangle.p0).cross(Triangle.p2 - Triangle.p0)).norm()).y;
        norm[2] = (((Triangle.p1 - Triangle.p0).cross(Triangle.p2 - Triangle.p0)).norm()).z;

        myfile << "  facet normal";
        for(int t = 0; t<3; t++){
            myfile << "  " << std::setw(10) << norm[t];
        }
        myfile << "\n";
        myfile << "    outer loop\n";

        myfile << "      vertex  ";

        myfile << "  " << std::setw(10) << Triangle.p0.x;
        myfile << "  " << std::setw(10) << Triangle.p0.y;
        myfile << "  " << std::setw(10) << Triangle.p0.z;

        myfile << "\n";

        myfile << "      vertex  ";

        myfile << "  " << std::setw(10) << Triangle.p1.x;
        myfile << "  " << std::setw(10) << Triangle.p1.y;
        myfile << "  " << std::setw(10) << Triangle.p1.z;

        myfile << "\n";

        myfile << "      vertex  ";

        myfile << "  " << std::setw(10) << Triangle.p2.x;
        myfile << "  " << std::setw(10) << Triangle.p2.y;
        myfile << "  " << std::setw(10) << Triangle.p2.z;

        myfile << "\n";

        myfile << "    endloop\n";
        myfile << "  endfacet\n";
    }

    myfile << "endsolid";

    myfile.close();

    return true;
}

void Export_STL_helper::header(int fd, const std::string& mesg) {
    std::ostringstream ostr;
    time_t _ttime_ = time(nullptr);
    ostr<<mesg<<" - "<<ctime(&_ttime_);
    std::string hdr=ostr.str();
    const int len=80;
    char buf[len];
    memset(buf,0,len);
    strncpy(buf,hdr.c_str(),len-1);
    ssize_t bytes_written = write(fd,buf,len);
}

void Export_STL_helper::calcnormal(float* tri, puma::Vec3<float>* norm) {
    puma::Vec3<float> u(tri[3]-tri[0],tri[4]-tri[1],tri[5]-tri[2]);
    puma::Vec3<float> v(tri[6]-tri[0],tri[7]-tri[1],tri[8]-tri[2]);
    *norm = u.cross(v).norm();
}
