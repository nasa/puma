#include "timer.h"


puma::Timer::Timer()
{
    reset();
    myTime = 0;
}


bool puma::Timer::reset(){
    struct timeval tim11;
    gettimeofday(&tim11, NULL);
    currentTime=(tim11.tv_sec + (tim11.tv_usec / 1000000.0)) * 1000.0;
    return true;
}

double puma::Timer::elapsed(){
    struct timeval tim11;
    gettimeofday(&tim11, NULL);
    double newTime=(tim11.tv_sec + (tim11.tv_usec / 1000000.0)) * 1000.0;

    return (newTime-currentTime)/1000.0;

}

bool puma::Timer::start() {
    reset();
    return true;
}

bool puma::Timer::stop() {
    myTime += elapsed();
    return true;
}

