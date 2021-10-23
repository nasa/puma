#ifndef Workspace_H
#define Workspace_H

#include "logger.h"
#include "matrix.h"
#include "Printer.h"


namespace puma {

    class Workspace
    {
    public:

        // --- Start Constructors --- //

        Workspace(long x, long y, long z, short val, double voxelLength) {
            log = new puma::Logger();
            matrix.resize(x,y,z,val);
            log->emptyLog();
            this->voxelLength = voxelLength;
            printer = new puma::Printer();
        }

        Workspace(long x, long y, long z, double voxelLength) {
            log = new puma::Logger();
            matrix.resize(x,y,z,0);
            log->emptyLog();
            this->voxelLength = voxelLength;
            printer = new puma::Printer();
        }

        explicit Workspace(double voxelLength) {
            log = new puma::Logger();
            matrix.resize(0,0,0,0);
            log->emptyLog();
            this->voxelLength = voxelLength;
            printer = new puma::Printer();
        }

        Workspace() {
            log = new puma::Logger();
            matrix.resize(0,0,0,0);
            log->emptyLog();
            this->voxelLength = 1e-6;
            printer = new puma::Printer();
        }

        explicit Workspace(Workspace *other) {
            log = new puma::Logger();
            matrix.copy(&other->matrix);
            log->emptyLog();
            this->voxelLength = 1e-6;
            printer = new puma::Printer();
        }

        explicit Workspace(const puma::Vec3<long>& shape) {
            log = new puma::Logger();
            matrix.resize(shape.x,shape.y,shape.z,0);
            log->emptyLog();
            this->voxelLength = 1e-6;
            printer = new puma::Printer();
        }

        Workspace(long x, long y, long z, short val, double voxelLength, Logger *otherLog) {
            log = otherLog;
            matrix.resize(x,y,z,val);
            this->voxelLength = voxelLength;
            myLogger = false;
            printer = new puma::Printer();
        }

        Workspace(long x, long y, long z, double voxelLength, Logger *otherLog) {
            log = otherLog;
            matrix.resize(x,y,z,0);
            this->voxelLength = voxelLength;
            myLogger = false;
            printer = new puma::Printer();
        }

        Workspace(double voxelLength, Logger *otherLog) {
            log = otherLog;
            matrix.resize(0,0,0,0);
            this->voxelLength = voxelLength;
            myLogger = false;
            printer = new puma::Printer();
        }

        explicit Workspace(Logger *otherLog) {
            log = otherLog;
            matrix.resize(0,0,0,0);
            this->voxelLength = 1e-6;
            myLogger = false;
            printer = new puma::Printer();
        }

        Workspace(Workspace *other, Logger *otherLog) {
            log = otherLog;
            matrix.copy(&other->matrix);
            this->voxelLength = 1e-6;
            myLogger = false;
            printer = new puma::Printer();
        }

        Workspace(const puma::Vec3<long>& shape, Logger *otherLog) {
            log = otherLog;
            matrix.resize(shape.x,shape.y,shape.z,0);
            this->voxelLength = 1e-6;
            myLogger = false;
            printer = new puma::Printer();
        }

        Workspace(long x, long y, long z, short val, double voxelLength, bool logBool) {
            log = new puma::Logger(logBool);
            matrix.resize(x,y,z,val);
            log->emptyLog();
            this->voxelLength = voxelLength;
            myLogger = false;
            printer = new puma::Printer();
        }

        Workspace(long x, long y, long z, double voxelLength, bool logBool) {
            log = new puma::Logger(logBool);
            matrix.resize(x,y,z,0);
            log->emptyLog();
            this->voxelLength = voxelLength;
            myLogger = false;
            printer = new puma::Printer();
        }

        Workspace(double voxelLength, bool logBool) {
            log = new puma::Logger(logBool);
            matrix.resize(0,0,0,0);
            log->emptyLog();
            this->voxelLength = voxelLength;
            myLogger = false;
            printer = new puma::Printer();
        }

        explicit Workspace(bool logBool) {
            log = new puma::Logger(logBool);
            matrix.resize(0,0,0,0);
            log->emptyLog();
            this->voxelLength = 1e-6;
            myLogger = false;
            printer = new puma::Printer();
        }

        Workspace(Workspace *other, bool logBool) {
            log = new puma::Logger(logBool);
            matrix.copy(&other->matrix);
            log->emptyLog();
            this->voxelLength = 1e-6;
            myLogger = false;
            printer = new puma::Printer();
        }

        Workspace(const puma::Vec3<long>& shape, bool logBool) {
            log = new puma::Logger(logBool);
            matrix.resize(shape.x,shape.y,shape.z,0);
            log->emptyLog();
            this->voxelLength = 1e-6;
            myLogger = false;
            printer = new puma::Printer();
        }

        Workspace(const puma::Vec3<long>& shape, double voxelLength, bool logBool) {
            log = new puma::Logger(logBool);
            matrix.resize(shape.x,shape.y,shape.z,0);
            log->emptyLog();
            this->voxelLength = voxelLength;
            myLogger = false;
            printer = new puma::Printer();
        }

        ~Workspace() {
            if(myLogger) { delete log; }
            if(myPrinter) { delete printer; }
        }

        // --- End Constructors --- //



        // --- Start Variables --- //

        puma::Matrix<short> matrix;
        puma::Logger *log;
        puma::Printer *printer;
        bool myPrinter{true};
        bool myLogger{true};

        double voxelLength;

        // --- End Variables --- //


        // --- Start Functions --- //

        void newWorkspace(double voxelLength) {
            matrix.resize(0,0,0,0);
            log->emptyLog();
            this->voxelLength = voxelLength;
        }

        void setLogLocation(std::string log_location) {
            log->emptyLog(log_location);
        }

        void setPrinter(puma::Printer *print) {
            std::cout << "Printer changed to user input" << std::endl;
            printer = print;
            myPrinter = false;
        }

        void newPrinter() {
            std::cout << "Printer returned to default" << std::endl;
            printer = new puma::Printer();
            myPrinter = true;
        }

        short operator() (long i, long j, long k) { return matrix(i,j,k); }
        short& at(long i) { return matrix.at(i); }
        short& at(long i, long j, long k) { return matrix.at(i,j,k); }
        short& at_safe(long i) { return matrix.at_safe(i); }
        short& at_safe(long i, long j, long k) { return matrix.at_safe(i,j,k); }

        long size() { return matrix.size(); }
        long X() { return matrix.X(); }
        long Y() { return matrix.Y(); }
        long Z() { return matrix.Z(); }
        puma::Vec3<long> shape() { return puma::Vec3<long>(matrix.X(),matrix.Y(),matrix.Z()); }


        puma::Vec3<long> getShape() { return puma::Vec3<long>(matrix.X(),matrix.Y(),matrix.Z()); }
        long getLength() { return matrix.size(); }
        long getSize() { return matrix.size(); }
        long getX() { return matrix.X(); }
        long getY() { return matrix.Y(); }
        long getZ() { return matrix.Z(); }

        short min() { return matrix.min(); }
        short max() { return matrix.max(); }
        double average() { return matrix.average(); }

        bool crop(long x1, long x2, long y1, long y2, long z1, long z2) {
            return matrix.crop(x1,x2,y1,y2,z1,z2);
        }

        void setSize(long X, long Y, long Z) {
            if( !( X>0 && Y>0 && Z>0 ) ) {
                std::cout << "Invalid size. X, Y, and Z must be >0" << std::endl;
                return;
            }
            matrix.resize(X,Y,Z,0);
        }

        void resize(long X, long Y, long Z) {
            if( !( X>0 && Y>0 && Z>0 ) ) {
                std::cout << "Invalid size. X, Y, and Z must be >0" << std::endl;
                return;
            }
            matrix.resize(X,Y,Z,0);
        }


        void setMaterialID(puma::Cutoff cutoff, int identifier) {
            if(identifier < 0) {
                return;
            }
            if(identifier > 1000) {
                return;
            }

            int X = (int)matrix.X();
            int Y = (int)matrix.Y();
            int Z = (int)matrix.Z();

#pragma omp parallel for
            for(long i=0; i<X; i++) {
                for(long j=0; j<Y; j++) {
                    for(long k=0; k<Z; k++) {

                        short value = matrix(i,j,k);
                        if(value <= cutoff.second && value >= cutoff.first) {
                            matrix(i,j,k) = identifier;
                        }
                    }
                }
            }

        }

        void setMaterialID(Workspace *other, puma::Cutoff cutoff, int identifier) {
            if(identifier < 0) {
                return;
            }
            if(identifier > 1000) {
                return;
            }

            int X = (int)matrix.X();
            int Y = (int)matrix.Y();
            int Z = (int)matrix.Z();

#pragma omp parallel for
            for(long i=0; i<X; i++) {
                for(long j=0; j<Y; j++) {
                    for(long k=0; k<Z; k++) {

                        short value = other->matrix(i,j,k);
                        if(value <= cutoff.second && value >= cutoff.first) {
                            matrix(i,j,k) = identifier;
                        }
                    }
                }
            }

        }


        // --- End Functions --- //


    };

}

#endif // Workspace
