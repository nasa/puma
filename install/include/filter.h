#ifndef FILTER_H
#define FILTER_H

#include "operation.h"

class Filter : public Operation
{
public:
    Filter() = default;

    virtual bool execute() = 0;

};

#endif // FILTER_H
