#ifndef PUMATIMER_H
#define PUMATIMER_H

#include <sys/time.h>
#include <ctime>


namespace puma {
class Timer
{
public:
    Timer();

    bool reset();
    double elapsed();

    bool start();
    bool stop();
    double myTime;

private:

    double currentTime;


};
}
#endif // PUMATIMER_H
