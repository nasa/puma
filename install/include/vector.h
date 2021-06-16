#ifndef VECTOR_H
#define VECTOR_H

#include "pstring.h"

#include <cmath>
#include <iostream>
#include <fstream>


namespace puma {

    template<class T>
    class Vec3 {
    public:
        T x;
        T y;
        T z;

        Vec3() {
            x = 0;
            y = 0;
            z = 0;
        }

        Vec3(T xVal, T yVal, T zVal) {
            x = xVal;
            y = yVal;
            z = zVal;
        }

        Vec3(const Vec3& other) {
            x = other.x;
            y = other.y;
            z = other.z;
        }

        void operator () (T xVal, T yVal, T zVal);
        void operator = (const Vec3& other);

        Vec3<T> operator - (const Vec3& other);
        Vec3<T> operator + (const Vec3& other);
        Vec3<T> operator * (const T val);
        Vec3<T> operator / (const T val);
        Vec3<T> operator * (const Vec3& other);
        Vec3<T> operator / (const Vec3& other);
        Vec3<T> cross(const Vec3& other);
        T dot(const Vec3& other);
        Vec3<T> norm();
        T magnitude();

        double distance(Vec3<T> other);
        Vec3<T> midpoint(Vec3<T> other);
        Vec3<T> direction(Vec3<T> other);

        static double distance(Vec3<T> p1, Vec3<T> p2);
        static Vec3<T> midpoint(Vec3<T> p1, Vec3<T> p2);
        static Vec3<T> direction(Vec3<T> p1, Vec3<T> p2);

        std::string toString();
        void print();

    };

    template<class T> void Vec3<T>::operator()(T xVal, T yVal, T zVal) {
        x = xVal;
        y = yVal;
        z = zVal;
    }

    template<class T> Vec3<T> Vec3<T>::operator - (const Vec3& other) {
        return Vec3(x-other.x, y-other.y, z-other.z);
    }

    template<class T> Vec3<T> Vec3<T>::operator + (const Vec3& other) {
        return Vec3(x+other.x, y+other.y, z+other.z);
    }

    template<class T> Vec3<T> Vec3<T>::operator * (const Vec3& other) {
        return Vec3(x*other.x, y*other.y, z*other.z);
    }

    template<class T> Vec3<T> Vec3<T>::operator / (const Vec3& other) {
        return Vec3(x/other.x, y/other.y, z/other.z);
    }


    template<class T> Vec3<T> Vec3<T>::operator * (const T val) {
        return Vec3(x*val, y*val, z*val);
    }

    template<class T> Vec3<T> Vec3<T>::operator / (const T val) {
        return Vec3(x/val, y/val, z/val);
    }

    template<class T> void Vec3<T>::operator = (const Vec3& other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    template<class T> Vec3<T> Vec3<T>::cross(const Vec3& other) {
        return Vec3(y*other.z-z*other.y, z*other.x-x*other.z, x*other.y-y*other.x);
    }

    template<class T> T Vec3<T>::dot(const Vec3& other) {
        return x*other.x + y*other.y + z*other.z;
    }

    template<class T> T Vec3<T>::magnitude() {
        return sqrt(x*x + y*y + z*z);
    }

    template<class T> Vec3<T> Vec3<T>::norm() {
        return this->operator /(magnitude());
    }

    template<class T> double Vec3<T>::distance(Vec3<T> other) {
        return sqrt((x-other.x)*(x-other.x)+(y-other.y)*(y-other.y)+(z-other.z)*(z-other.z));
    }

    template<class T> Vec3<T> Vec3<T>::midpoint(Vec3<T> other) {
        return (this->operator +(other)) / 2;
    }

    template<class T> Vec3<T> Vec3<T>::direction(Vec3<T> other) {
        return this->operator -(other);
    }

    template<class T> double Vec3<T>::distance(Vec3<T> p1, Vec3<T> p2) {
        return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z));
    }

    template<class T> Vec3<T> Vec3<T>::midpoint(Vec3<T> p1, Vec3<T> p2) {
        return (p1 + p2) / 2;
    }

    template<class T> Vec3<T> Vec3<T>::direction(Vec3<T> p1, Vec3<T> p2) {
        return p1-p2;
    }

    template<class T> std::string Vec3<T>::toString() {
        std::string returnString = "";
        returnString.append(puma::PString::fromDouble((double)this->x));
        returnString.append(" ");
        returnString.append(puma::PString::fromDouble((double)this->y));
        returnString.append(" ");
        returnString.append(puma::PString::fromDouble((double)this->z));
        returnString.append(" ");
        return returnString;
    }

    template<class T> void Vec3<T>::print() {
        std::cout << toString() << std::endl;
    }


    template<class T>
    class Vec4 {
    public:
        T x;
        T y;
        T z;
        T v;

        Vec4() {
            x = 0;
            y = 0;
            z = 0;
            v = 0;
        }

        Vec4(T xVal, T yVal, T zVal) {
            x = xVal;
            y = yVal;
            z = zVal;
            v = 0;
        }

        Vec4(T xVal, T yVal, T zVal, T vVal) {
            x = xVal;
            y = yVal;
            z = zVal;
            v = vVal;
        }

        Vec4(const Vec4& other) {
            x = other.x;
            y = other.y;
            z = other.z;
            v = other.v;
        }

        void operator () (T xVal, T yVal, T zVal);
        void operator () (T xVal, T yVal, T zVal, T vVal);
        void  operator = (const Vec4& other);

        Vec4<T> operator - (const Vec4& other);
        Vec4<T> operator + (const Vec4& other);
        Vec4<T> operator * (const T val);
        Vec4<T> operator / (const T val);

        bool operator < (const Vec4& other);
        Vec4<T> cross(const Vec4& other);
        T dot(const Vec4& other);
        Vec4<T> norm();
        T magnitude();

        std::string toString();
        std::string print();

        operator Vec3<T>();

    };

    template<class T> Vec4<T>::operator Vec3<T>() {
        return Vec3<T>(x,y,z);
    }

    template<class T> void Vec4<T>::operator()(T xVal, T yVal, T zVal) {
        x=xVal;
        y=yVal;
        z=zVal;
    }

    template<class T> void Vec4<T>::operator()(T xVal, T yVal, T zVal, T vVal) {
        x=xVal;
        y=yVal;
        z=zVal;
        v=vVal;
    }

    template<class T> Vec4<T> Vec4<T>::operator - (const Vec4& other) {
        return Vec4(x-other.x, y-other.y, z-other.z, v-other.v);
    }

    template<class T> Vec4<T> Vec4<T>::operator + (const Vec4& other) {
        return Vec4(x+other.x, y+other.y, z+other.z, v+other.v);
    }

    template<class T> Vec4<T> Vec4<T>::operator * (const T val) {
        return Vec4(x*val, y*val, z*val, v*val);
    }

    template<class T> Vec4<T> Vec4<T>::operator / (const T val) {
        return Vec4(x/val, y/val, z/val, v/val);
    }

    template<class T> void Vec4<T>::operator = (const Vec4& other) {
        x = other.x;
        y = other.y;
        z = other.z;
        v = other.v;
    }

    template<class T> Vec4<T> Vec4<T>::cross(const Vec4& other) {
        return Vec4(y*other.z-z*other.y, z*other.x-x*other.z, x*other.y-y*other.x, v);
    }

    template<class T> T Vec4<T>::dot(const Vec4& other) {
        return x*other.x + y*other.y + z*other.z;
    }

    template<class T> bool Vec4<T>::operator < (const Vec4& other) {
        if (x < other.x)
            return true;
        else if (x > other.x)
            return false;

        if (y < other.y)
            return true;
        else if (y > other.y)
            return false;

        if (z < other.z)
            return true;
        else if (z > other.z)
            return false;

        return false;
    }

    template<class T> T Vec4<T>::magnitude() {
        return sqrt(x*x + y*y + z*z);
    }

    template<class T> Vec4<T> Vec4<T>::norm() {
        return this->operator /(magnitude());
    }

    template<class T> std::string Vec4<T>::toString() {
        std::string returnString = "";
        returnString.append(puma::PString::fromDouble((double)this->x));
        returnString.append(" ");
        returnString.append(puma::PString::fromDouble((double)this->y));
        returnString.append(" ");
        returnString.append(puma::PString::fromDouble((double)this->z));
        returnString.append(" ");
        returnString.append(puma::PString::fromDouble((double)this->v));
        returnString.append(" ");
        return returnString;
    }

    template<class T> std::string Vec4<T>::print() {
        return toString();
    }


}

template<class T>
std::ostream& operator<<(std::ostream &strm, const puma::Vec3<T> &vec)
{
    std::ios_base::fmtflags ff = strm.flags();
    std::streamsize w = strm.width();
    std::streamsize p = strm.precision();
    char f =  strm.fill();

    strm.setf(ff);
    strm.width(w);
    strm.precision(p);
    strm.fill(f);
    strm << '(';
    strm << vec.x;
    strm << ',';

    strm.setf(ff);
    strm.width(w);
    strm.precision(p);
    strm.fill(f);
    strm << vec.y;
    strm << ',';

    strm.setf(ff);
    strm.width(w);
    strm.precision(p);
    strm.fill(f);
    strm << vec.z;
    return strm << ')';
}


template<class T>
puma::Vec3<T> operator*(const T val, puma::Vec3<T> &vec)
{
    return vec.operator*(val);
}

template<class T>
puma::Vec3<T> operator/(const T val, puma::Vec3<T> &vec)
{
    return vec.operator/(val);
}




#endif // VECTOR_H
