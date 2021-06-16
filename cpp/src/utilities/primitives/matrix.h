#ifndef PUMA_Matrix_H
#define PUMA_Matrix_H

#include "vector.h"
#include "pstring.h"
#include "cutoff.h"

#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include <omp.h>


/* Matrix template class for PuMA.
    Note: T should be of a number type (int, short, float, double, etc.)
*/
namespace puma{

    template<class T>
    class Matrix {
    private:
        T* data;
        long x{};
        long y{};
        long z{};
        long zy;
        long mySize;

    public:

        Matrix(long xR, long yR, long zR, T t) {

            this->x=xR;
            this->y=yR;
            this->z=zR;
            zy = z*y;
            mySize = x*y*z;
            data = new T[mySize];
            set(t);
        }

        Matrix(long xR, long yR, long zR) {

            this->x=xR;
            this->y=yR;
            this->z=zR;
            zy = z*y;
            mySize = x*y*z;
            data = new T[mySize];
        }

        explicit Matrix(Matrix<T> *other) {

            this->x=other->X();
            this->y=other->Y();
            this->z=other->Z();
            zy = z*y;
            mySize = x*y*z;
            data = new T[mySize];

            for(long i=0; i<x; i++) {
                for(long j=0; j<y; j++) {
                    for(long k=0; k<z; k++) {
                        data[zy*i+z*j+k] = other->operator ()(i,j,k);
                    }
                }
            }

        }

        Matrix() {

            this->x=0;
            this->y=0;
            this->z=0;
            zy = z*y;
            mySize = x*y*z;
            data = new T[mySize];

        }

        ~Matrix() {
            delete [] data;
        }

        T& operator()(long i, long j, long k) const;
        T& operator()(long i) const;

        T& at(long i, long j, long k) const;
        T& at(long i) const;

        T& at_safe(long i, long j, long k) const;
        T& at_safe(long i) const;

        bool set(T t);
        bool set(long x1, long x2, long y1, long y2, long z1, long z2, T t);

        bool copy(Matrix<T> *other);
        bool crop(long x1, long x2, long y1, long y2, long z1, long z2);

        double reduce();
        double dot(Matrix<T> *p2) const;

        T min();
        T max();

        bool flipAroundValue(float average);
        bool flipAroundValue(Matrix<float> *newMatrix, float average);

        bool normalize(float minValue, float maxValue);
        bool normalize(Matrix<float> *newMatrix, float minValue, float maxValue);

        double average();

        void resize(long xR, long yR, long zR, T t);
        void resize(long xR, long yR, long zR);

        bool print(int precision = 3) const;
        bool printRange(int xstart, int xend, int ystart, int yend, int zstart, int zend, int precision = 3) const;
        bool printSlice(char alongAxis, int sliceNumber, int precision = 3) const;
        void printSize() const;
        std::string sizeToString() const;

        long X();
        long Y();
        long Z();
        long ZY();
        long size() const;

    };



    template<class T>
    class MatVec3 : public Matrix< puma::Vec3<T> > {
    public:
        MatVec3(long xR, long yR, long zR, puma::Vec3<T> t) {
            this->resize(xR, yR, zR, t);
        }

        MatVec3(long xR, long yR, long zR) {
            this->resize(xR, yR, zR, puma::Vec3<T>(0,0,0));
        }

        MatVec3() = default;
    };



    template<class T> long Matrix<T>::X() {
        return x;
    }

    template<class T> long Matrix<T>::Y() {
        return y;
    }

    template<class T> long Matrix<T>::Z() {
        return z;
    }

    template<class T> long Matrix<T>::ZY() {
        return zy;
    }

    template<class T> long Matrix<T>::size() const {
        return mySize;
    }

    template<class T> T& Matrix<T>::operator()(long i, long j, long k) const {
        return data[zy*i+z*j+k];
    }

    template<class T> T& Matrix<T>::operator()(long i) const {
        return data[i];
    }

    template<class T> T& Matrix<T>::at(long i, long j, long k) const {
        return data[zy*i+z*j+k];
    }

    template<class T> T& Matrix<T>::at(long i) const {
        return data[i];
    }

    template<class T> T& Matrix<T>::at_safe(long i, long j, long k) const {
        if(i<0 || i >= x || j<0 || j >= y || k<0 || k >= z) {
            std::cout << "Error in Matrix::at_safe: index " << i << " " << j << " " << k  << " is out of Matrix Range: " << 0 << " " << x << 0 << " " << y  << 0 << " " << z << std::endl;
        }
        return data[zy*i+z*j+k];
    }

    template<class T> T& Matrix<T>::at_safe(long i) const {
        if(i<0 || i >= this->size()) {
            std::cout << "Error in Matrix::at_safe: index " << i << " is out of Matrix Range: " << 0 << " " << this->size()-1 << std::endl;
        }
        return data[i];
    }

    template<class T> bool Matrix<T>::crop(long x1, long x2, long y1, long y2, long z1, long z2) {

        // to simplify function call
        if(x2 == -1){ x2 = x-1; }
        if(y2 == -1){ y2 = y-1; }
        if(z2 == -1){ z2 = z-1; }

        if(x1 < 0 || x1 > x2 || x2 > x-1) {
            std::cout << "Error in x crop" << std::endl;
            return false;
        }

        if(y1 < 0 || y1 > y2 || y2 > y-1) {
            std::cout << "Error in y crop" << std::endl;
            return false;
        }

        if(z1 < 0 || z1 > z2 || z2 > z-1) {
            std::cout << "Error in z crop" << std::endl;
            return false;
        }

        Matrix<T> temMatrix(x2-x1+1, y2-y1+1, z2-z1+1);

#pragma omp parallel for
        for(long i=x1;i<x2+1;i++) {
            for(long j=y1;j<y2+1;j++) {
                for(long k=z1;k<z2+1;k++) {
                    temMatrix(i-x1,j-y1,k-z1) = this->operator ()(i,j,k);
                }
            }
        }

        this->copy(&temMatrix);

        return true;
    }

    template<class T> bool Matrix<T>::copy(Matrix<T> *other) {
        this->x=other->X();
        this->y=other->Y();
        this->z=other->Z();

        zy = z*y;
        mySize = x*y*z;

        delete [] data;
        data = new T[mySize];

#pragma omp parallel for
        for(long i=0; i<mySize; i++) {
            data[i] = other->operator ()(i);

        }

        return true;
    }

    template<class T> bool Matrix<T>::flipAroundValue(float average) {

        if(mySize==0) {
            return false;
        }

#pragma omp parallel for
        for(int i=0;i<x;i++){
            for (int j = 0; j < y; j++) {
                for (int k = 0; k < z; k++) {
                    if(this->operator ()(i,j,k) > average) {
                        this->operator ()(i,j,k) = (T) (2.0*average - this->operator ()(i,j,k));
                    }
                }
            }
        }
        return true;
    }

    template<class T> bool Matrix<T>::flipAroundValue(Matrix<float> *newMatrix, float average) {

        if(mySize==0) {
            return false;
        }

        newMatrix->resize(x,y,z);

#pragma omp parallel for
        for(int i=0;i<x;i++){
            for (int j = 0; j < y; j++) {
                for (int k = 0; k < z; k++) {
                    if(this->operator ()(i,j,k) > average) {
                        newMatrix->operator ()(i,j,k) = (T) (2.0*average - this->operator ()(i,j,k));
                    } else {
                        newMatrix->operator ()(i,j,k) = (float) this->operator ()(i,j,k);
                    }
                }
            }
        }
        return true;
    }


    template<class T> bool Matrix<T>::normalize(float minValue, float maxValue) {

        if(mySize==0) {
            return false;
        }

        float range = maxValue - minValue;

        auto min = (float)this->min();
        auto max = (float)this->max();

#pragma omp parallel for
        for(long i=0;i<mySize;i++){
            data[i] = minValue+(range*(data[i]-min)/(max-min));
        }

        return true;
    }

    template<class T> bool Matrix<T>::normalize(Matrix<float> *newMatrix, float minValue, float maxValue) {

        if(mySize==0) {
            return false;
        }

        float range = maxValue - minValue;

        newMatrix->resize(x,y,z);

        auto min = (float)this->min();
        auto max = (float)this->max();

#pragma omp parallel for
        for(long i=0;i<mySize;i++){
            newMatrix->operator ()(i) = minValue+(range*(data[i]-min)/(max-min));
        }

        return true;
    }

    template<class T> double Matrix<T>::reduce() {

        if(mySize==0) {
            return 0;
        }

        int splitSize = omp_get_num_procs();
        if(splitSize==0) splitSize=1;

        long iteration = mySize/splitSize;

        std::vector<long> cStart(splitSize);
        std::vector<long> cEnd(splitSize);

        for(long i=0;i<splitSize-1;i++){
            cStart[i]=i*iteration;
            cEnd[i]=(i+1)*iteration;
        }
        cStart[splitSize-1]=(splitSize-1)*iteration;
        cEnd[splitSize-1]=mySize;

        std::vector<double> splitCount(splitSize,0);
        double total = 0;

#pragma omp parallel for
        for(long c=0;c<splitSize;c++){
            for(long i=cStart[c];i<cEnd[c];i++){
                splitCount[c]+=data[i];
            }
        }

        for(int i=0;i<splitSize;i++){
            total+=splitCount[i];
        }

        return total;

    }

    template<class T> double Matrix<T>::dot(Matrix<T> *p2) const {
        if(mySize != p2->size()){
            std::cout << "PMatrix Error: Matrices must be of the same size when taking dot product" << std::endl;
            return -1;
        }

        if(mySize==0) {
            return 0;
        }

        int splitSize = omp_get_num_procs();

        if(splitSize==0) splitSize=1;

        long iteration = mySize/splitSize;

        std::vector<long> cStart(splitSize);
        std::vector<long> cEnd(splitSize);

        for(long i=0;i<splitSize-1;i++){
            cStart[i]=i*iteration;
            cEnd[i]=(i+1)*iteration;
        }
        cStart[splitSize-1]=(splitSize-1)*iteration;
        cEnd[splitSize-1]=mySize;

        std::vector<double> splitCount(splitSize,0);
        double total = 0;

#pragma omp parallel for
        for(long c=0;c<splitSize;c++){
            for(long i=cStart[c];i<cEnd[c];i++){
                splitCount[c]+=data[i]*(*p2)(i);
            }
        }

        for(int i=0;i<splitSize;i++){
            total+=splitCount[i];
        }


        //    double total = 0;

        //#pragma omp parallel for reduction(+:total)
        //    for (long i = 0; i < mySize; i++) {
        //        total += data[i] * p2->at(i);
        //    }

        //    std::cout << "new version" << std::endl;


        return total;
    }

    template<class T> T Matrix<T>::min() {

        if(mySize==0) {
            return 0;
        }

        int splitSize = omp_get_num_procs();
        if(splitSize==0) splitSize=1;

        long iteration = mySize/splitSize;

        std::vector<long> cStart(splitSize);
        std::vector<long> cEnd(splitSize);

        for(long i=0;i<splitSize-1;i++){
            cStart[i]=i*iteration;
            cEnd[i]=(i+1)*iteration;
        }
        cStart[splitSize-1]=(splitSize-1)*iteration;
        cEnd[splitSize-1]=mySize;

        std::vector<double> splitMin(splitSize,1e10);

#pragma omp parallel for
        for(long c=0;c<splitSize;c++){
            for(long i=cStart[c];i<cEnd[c];i++){
                if(data[i] < splitMin[c]) { splitMin[c] = data[i]; }
            }
        }

        double min = splitMin[0];
        for(int i=1;i<splitSize;i++){
            if(splitMin[i] < min) { min = splitMin[i]; }
        }

        return min;

    }

    template<class T> T Matrix<T>::max() {

        if(mySize==0) {
            return 0;
        }

        int splitSize = omp_get_num_procs();
        if(splitSize==0) splitSize=1;

        long iteration = mySize/splitSize;

        std::vector<long> cStart(splitSize);
        std::vector<long> cEnd(splitSize);

        for(long i=0;i<splitSize-1;i++){
            cStart[i]=i*iteration;
            cEnd[i]=(i+1)*iteration;
        }
        cStart[splitSize-1]=(splitSize-1)*iteration;
        cEnd[splitSize-1]=mySize;

        std::vector<double> splitMax(splitSize,-1e10);

#pragma omp parallel for
        for(long c=0;c<splitSize;c++){
            for(long i=cStart[c];i<cEnd[c];i++){
                if(data[i] > splitMax[c]) { splitMax[c] = data[i]; }
            }
        }

        double max = splitMax[0];
        for(int i=1;i<splitSize;i++){
            if(splitMax[i] > max) { max = splitMax[i]; }
        }

        return max;

    }

    template<class T> double Matrix<T>::average() {

        if(mySize==0) {
            return 0;
        }

        double red = reduce();
        return red/(double)mySize;
    }

    template<class T> bool Matrix<T>::set(T t) {
#pragma omp parallel for
        for(long i=0; i<mySize; i++) {
            data[i] = t;
        }
        return true;
    }

    template<class T> bool Matrix<T>::set(long x1, long x2, long y1, long y2, long z1, long z2, T t) {

        // to simplify function call
        if(x2 == -1){ x2 = x-1; }
        if(y2 == -1){ y2 = y-1; }
        if(z2 == -1){ z2 = z-1; }

        if( x1<0 || x1>x2 || x2>=x ||
            y1<0 || y1>y2 || y2>=y ||
            z1<0 || z1>z2 || z2>=z ) {
            std::cout << "Out of boundary" << std::endl;
            return false;
        }

#pragma omp parallel for
        for(long i=x1; i<=x2; i++) {
            for(long j=y1; j<=y2; j++) {
                for(long k=z1; k<=z2; k++) {
                    data[zy*i+z*j+k] = t;
                }
            }
        }

        return true;

    }

    template<class T> void Matrix<T>::resize(long xR, long yR, long zR, T t) {
        this->x=xR;
        this->y=yR;
        this->z=zR;
        zy = z*y;
        mySize = x*y*z;
        delete [] data;
        data = new T[mySize];
        set(t);
    }

    template<class T> void Matrix<T>::resize(long xR, long yR, long zR) {
        this->x=xR;
        this->y=yR;
        this->z=zR;
        zy = z*y;
        mySize = x*y*z;
        delete [] data;
        data = new T[mySize];
    }

    template<class T> std::string Matrix<T>::sizeToString() const {
        std::string returnString;
        returnString.append(puma::PString::fromLong(this->x));
        returnString.append(" ");
        returnString.append(puma::PString::fromLong(this->y));
        returnString.append(" ");
        returnString.append(puma::PString::fromLong(this->z));
        returnString.append(" ");
        return returnString;
    }

    template<class T> void Matrix<T>::printSize() const {
        std::cout << sizeToString() << std::endl;
    }


    template<class T> bool Matrix<T>::print(int precision) const {

        // Printing coordinate system used
        std::cout << "z o---> y" << std::endl;
        std::cout << "  |" << std::endl;
        std::cout << "x v" << std::endl << std::endl;

        std::cout << '[' << std::flush;
        for(int k=0; k<z; k++){ // slices
            std::cout << "(:,:," << k << ") = " << std::endl;
            std::cout << '[' << std::flush;
            for(int i=0; i<x; i++){ // rows
                std::cout << '[' << std::flush;
                for(int j=0; j<y; j++){ // columns
                    std::cout << std::setprecision(precision) << data[zy*i+z*j+k] << std::flush;
                    if(j!=y-1) std::cout << ' ' << std::flush;
                }
                std::cout << ']' << std::flush;
                if(i!=x-1) std::cout << std::endl;
            }
            std::cout << ']' << std::flush;
            if(k!=z-1) std::cout << std::endl << std::endl;
        }
        std::cout << ']' << std::endl;

        return true;
    }

    template<class T> bool Matrix<T>::printRange(int xstart, int xend, int ystart, int yend, int zstart, int zend, int precision) const {

        // to simplify function call
        if(xend == -1){ xend = x-1; }
        if(yend == -1){ yend = y-1; }
        if(zend == -1){ zend = z-1; }

        if(xstart<0 || xstart>xend || xend>=x ||
           ystart<0 || ystart>yend || yend>=y ||
           zstart<0 || zstart>zend || zend>=z ) {
            std::cout << "Out of boundary" << std::endl;
            return false;
        }

        // Printing coordinate system used
        std::cout << "z o---> y" << std::endl;
        std::cout << "  |" << std::endl;
        std::cout << "x v" << std::endl << std::endl;

        std::cout << '[' << std::flush;
        for(int k=zstart; k<zend; k++){ // slices
            if(xstart==0 && xend==x && ystart==0 && yend==y){
                std::cout << "(:,:," << k << ") = " << std::endl;
            } else if (xstart==0 && xend==x) {
                std::cout << "(:," << ystart << ':' << yend << ',' << k << ") = " << std::endl;
            } else if (ystart==0 && yend==y) {
                std::cout << '(' << xstart << ':' << xend << ",:," << k << ") = " << std::endl;
            } else {
                std::cout << '(' << xstart << ':' << xend << ',' << ystart << ':' << yend << ',' << k << ") = " << std::endl;
            }
            std::cout << '[' << std::flush;
            for(int i=xstart; i<xend; i++){ // rows
                std::cout << '[' << std::flush;
                for(int j=ystart; j<yend; j++){ // columns
                    std::cout << std::setprecision(precision) << data[zy*i+z*j+k] << std::flush;
                    if(j!=yend-1) std::cout << ' ' << std::flush;
                }
                std::cout << ']' << std::flush;
                if(i!=xend-1) std::cout << std::endl;
            }
            std::cout << ']' << std::flush;
            if(k!=zend-1) std::cout << std::endl << std::endl;
        }
        std::cout << ']' << std::endl;

        return true;
    }

    template<class T> bool Matrix<T>::printSlice(char alongAxis, int sliceNumber, int precision) const {

        // to simplify function call
        if (alongAxis == 'z') {

            // Error checking
            if(sliceNumber<0 || sliceNumber>=z) {
                std::cout << "Slice number out of boundary" << std::endl;
                return false;
            }

            // Printing coordinate system used
            std::cout << "z o---> y" << std::endl;
            std::cout << "  |" << std::endl;
            std::cout << "x v" << std::endl << std::endl;

            std::cout << "[(:,:," << sliceNumber << ") = " << std::endl;
            std::cout << '[' << std::flush;
            for(int i=0; i<x; i++){ // rows
                std::cout << '[' << std::flush;
                for(int j=0; j<y; j++){ // columns
                    std::cout << std::setprecision(precision) << data[zy*i+z*j+sliceNumber] << std::flush;
                    if(j!=y-1) std::cout << ' ' << std::flush;
                }
                std::cout << ']' << std::flush;
                if(i!=x-1) std::cout << std::endl;
            }
            std::cout << "]]" << std::endl;

        } else if (alongAxis == 'y') {

            // Error checking
            if(sliceNumber<0 || sliceNumber>=y) {
                std::cout << "Slice number out of boundary" << std::endl;
                return false;
            }

            // Printing coordinate system used
            std::cout << "y o---> x" << std::endl;
            std::cout << "  |" << std::endl;
            std::cout << "z v" << std::endl << std::endl;

            std::cout << "[(:," << sliceNumber << ",:) = " << std::endl;
            std::cout << '[' << std::flush;
            for(int k=0; k<z; k++){ // rows
                std::cout << '[' << std::flush;
                for(int i=0; i<x; i++){ // columns
                    std::cout << std::setprecision(precision) << data[zy*i+z*sliceNumber+k] << std::flush;
                    if(i!=x-1) std::cout << ' ' << std::flush;
                }
                std::cout << ']' << std::flush;
                if(k!=x-1) std::cout << std::endl;
            }
            std::cout << "]]" << std::endl;

        } else if (alongAxis == 'x') {

            // Error checking
            if(sliceNumber<0 || sliceNumber>=x) {
                std::cout << "Slice number out of boundary" << std::endl;
                return false;
            }

            // Printing coordinate system used
            std::cout << "x o---> y" << std::endl;
            std::cout << "  |" << std::endl;
            std::cout << "z v" << std::endl << std::endl;

            std::cout << "[(" << sliceNumber << ",:,:) = " << std::endl;
            std::cout << '[' << std::flush;
            for(int k=0; k<z; k++){ // rows
                std::cout << '[' << std::flush;
                for(int j=0; j<y; j++){ // columns
                    std::cout << std::setprecision(precision) << data[zy*sliceNumber+z*j+k] << std::flush;
                    if(j!=y-1) std::cout << ' ' << std::flush;
                }
                std::cout << ']' << std::flush;
                if(k!=z-1) std::cout << std::endl;
            }
            std::cout << "]]" << std::endl;

        } else {
            std::cout << "Invalid axis: can be 'x', 'y' or 'z'" << std::endl;
            return false;
        }

        return true;
    }

    // Naming the datastructures required
    typedef std::map<int, std::vector<double>> MatCond;
    typedef std::map<int, std::vector<double>> MatElast;

}

#endif // PUMA_Matrix_H
