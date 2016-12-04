#include "TimeUtils.hpp"
#include <time.h>

namespace r64fx{

constexpr unsigned long bsec = 1000 * 1000 * 1000;

unsigned long current_nanoseconds()
{
    timespec spec;
    clock_gettime(CLOCK_MONOTONIC_RAW, &spec);
    return spec.tv_nsec + spec.tv_sec * bsec;
}


void sleep_nanoseconds(unsigned long nseconds)
{
    timespec req, rem;
    req.tv_sec  = nseconds / bsec;
    req.tv_nsec = nseconds % bsec;
    nanosleep(&req, &rem);
}

}//namespace r64fx
