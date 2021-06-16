#ifndef PUMA_Triangle_H
#define PUMA_Triangle_H

#include "vector.h"

#include <vector>
#include <iostream>


/* Triangle template class for PuMA.
    Note: T should be of a number type (int, short, float, double, etc.)
*/

namespace puma {



    template<class T>
    class Triangle {
    public:
        puma::Vec3<T> p0;
        puma::Vec3<T> p1;
        puma::Vec3<T> p2;
        T normZ;
        int color;

        Triangle() {
            p0.x = 0;
            p0.y = 0;
            p0.z = 0;
            p1.x = 0;
            p1.y = 0;
            p1.z = 0;
            p2.x = 0;
            p2.y = 0;
            p2.z = 0;
            normZ = 0;
            color = 0;
        }

        Triangle(puma::Vec3<T> p0, puma::Vec3<T> p1, puma::Vec3<T> p2, T normZ) {
            this->p0 = p0;
            this->p1 = p1;
            this->p2 = p2;
            this->normZ = normZ;
            color = 0;
        }


        Triangle(const Triangle<T> &other) {
            this->p0 = other.p0;
            this->p1 = other.p1;
            this->p2 = other.p2;
            this->normZ = other.normZ;
            color = other.color;
        }


        std::string toString();

        void  operator = (const Triangle<T>& other);

        Triangle<T> operator - (const puma::Vec3<T> vec);
        Triangle<T> operator + (const puma::Vec3<T> vec);

        Triangle<T> operator - (const T val);
        Triangle<T> operator + (const T val);
        Triangle<T> operator * (const T val);
        Triangle<T> operator / (const T val);

    };


    template<class T> Triangle<T> Triangle<T>::operator - (const puma::Vec3<T> vec) {
        Triangle<T> tri;
        tri.p0=(this->p0-vec);
        tri.p1=(this->p1-vec);
        tri.p2=(this->p2-vec);
        tri.normZ=this->normZ;
        tri.color = this->color;
        return tri;
    }

    template<class T> Triangle<T> Triangle<T>::operator + (const puma::Vec3<T> vec) {
        Triangle<T> tri;
        tri.p0=(this->p0+vec);
        tri.p1=(this->p1+vec);
        tri.p2=(this->p2+vec);
        tri.normZ=this->normZ;
        tri.color = this->color;
        return tri;
    }

    template<class T> Triangle<T> Triangle<T>::operator - (const T val) {
        Triangle<T> tri;
        tri.p0=(this->p0-puma::Vec3<T>(val,val,val));
        tri.p1=(this->p1-puma::Vec3<T>(val,val,val));
        tri.p2=(this->p2-puma::Vec3<T>(val,val,val));
        tri.normZ=this->normZ;
        tri.color = this->color;
        return tri;
    }

    template<class T> Triangle<T> Triangle<T>::operator + (const T val) {
        Triangle<T> tri;
        tri.p0=(this->p0+puma::Vec3<T>(val,val,val));
        tri.p1=(this->p1+puma::Vec3<T>(val,val,val));
        tri.p2=(this->p2+puma::Vec3<T>(val,val,val));
        tri.normZ=this->normZ;
        tri.color = this->color;
        return tri;
    }

    template<class T> Triangle<T> Triangle<T>::operator * (const T val) {
        Triangle<T> tri;
        tri.p0=(this->p0*val);
        tri.p1=(this->p1*val);
        tri.p2=(this->p2*val);
        tri.normZ=this->normZ;
        tri.color = this->color;
        return tri;
    }

    template<class T> Triangle<T> Triangle<T>::operator / (const T val) {
        Triangle<T> tri;
        tri.p0=(this->p0/val);
        tri.p1=(this->p1/val);
        tri.p2=(this->p2/val);
        tri.normZ=this->normZ;
        tri.color = this->color;
        return tri;
    }

    template<class T> void Triangle<T>::operator = (const Triangle& other) {
        this->p0 = other.p0;
        this->p1 = other.p1;
        this->p2 = other.p2;
        this->normZ = other.normZ;
        this->color = other.color;
    }

    template<class T> std::string Triangle<T>::toString() {
        std::string returnString = "Vertex 1: ";
        returnString.append(puma::PString::fromDouble((double)this->p0.x));
        returnString.append(" ");
        returnString.append(puma::PString::fromDouble((double)this->p0.y));
        returnString.append(" ");
        returnString.append(puma::PString::fromDouble((double)this->p0.z));
        returnString.append(" - Vertex 2: ");
        returnString.append(puma::PString::fromDouble((double)this->p1.x));
        returnString.append(" ");
        returnString.append(puma::PString::fromDouble((double)this->p1.y));
        returnString.append(" ");
        returnString.append(puma::PString::fromDouble((double)this->p1.z));
        returnString.append(" ");
        returnString.append(" - Vertex 3: ");
        returnString.append(puma::PString::fromDouble((double)this->p2.x));
        returnString.append(" ");
        returnString.append(puma::PString::fromDouble((double)this->p2.y));
        returnString.append(" ");
        returnString.append(puma::PString::fromDouble((double)this->p2.z));
        returnString.append(" ");
        return returnString;
    }


    typedef std::vector < puma::Triangle<float> > TriangleVector;


}

template<class T>
std::ostream& operator<<(std::ostream &strm, const puma::Triangle<T> &tri)
{
    std::ios_base::fmtflags ff = strm.flags();
    std::streamsize w = strm.width();
    std::streamsize p = strm.precision();
    char f =  strm.fill();

    strm.setf(ff);
    strm.width(w);
    strm.precision(p);
    strm.fill(f);
    strm << tri.p0;

    strm.setf(ff);
    strm.width(w);
    strm.precision(p);
    strm.fill(f);
    strm << tri.p1;

    strm.setf(ff);
    strm.width(w);
    strm.precision(p);
    strm.fill(f);
    strm << tri.p2;

    strm.setf(ff);
    strm.width(w);
    strm.precision(p);
    strm.fill(f);
    return strm << tri.normZ;
}


#endif // PUMA_PMatrix_H
