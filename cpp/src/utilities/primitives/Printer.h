//
// Created by Joseph Ferguson on 8/25/20.
//

#ifndef PUMA_V3_PRINTER_H
#define PUMA_V3_PRINTER_H

#include <string>
#include <sstream>
#include <vector>
#include <ostream>

namespace puma {

    class StringBuilder {
    public:
        template <typename T> inline StringBuilder& operator<<(const T& t) { myStream << t; return * this; }
        inline std::string getStr() const { return myStream.str(); }
        inline operator std::string () const { return getStr(); }
    private:
        std::stringstream myStream;
    };


    class Printer {

    public:
        virtual void print(std::string str);
        virtual void error(std::string str);

        virtual void passData(std::string tag, std::vector<double> data);

    };
}


#endif //PUMA_V3_PRINTER_H
