#ifndef Export_H
#define Export_H

#include "operation.h"


class Export : public Operation
{
public:
    virtual bool execute() = 0;
    Export() = default;
};

#endif // Export_H
