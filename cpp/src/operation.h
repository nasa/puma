#ifndef OPERATION_H
#define OPERATION_H

#include <string>


class Operation {
public:
    Operation() = default;


    virtual bool logInput() = 0;
    virtual bool logOutput() = 0;
    virtual bool errorCheck(std::string *errorMessage) = 0;

};

#endif // OPERATION_H
