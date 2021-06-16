//
// Created by Joseph Ferguson on 8/25/20.
//

#include "Printer.h"
#include "pstring.h"

#include <iostream>

void puma::Printer::print(std::string str) {
    std::cout << str << std::endl;
}

void puma::Printer::error(std::string str) {
    std::cout << "Error: " << str << std::endl;
}

void puma::Printer::passData(std::string tag, std::vector<double> data) {
    std::string str;
    str.append(tag);
    str.append(" ");
    for(int i = 0; i <data.size() ; i++) {
        str.append(puma::PString::fromDouble(data[i]));
        str.append(" ");
    }
    print(puma::StringBuilder() << str);
}