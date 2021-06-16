#ifndef IMPORTDATA_H
#define IMPORTDATA_H

#include "operation.h"


class Import : public Operation
{
public:

    Import() = default;

    virtual bool import() = 0;


};

#endif // IMPORTDATA_H
