#include "sleep.hpp"
#include <time.h>

namespace r64fx{

constexpr unsigned long bsec = 1000 * 1000 * 1000;

void sleep_microseconds(unsigned long mseconds)
{
    sleep_microseconds(mseconds * 1000);
}


void sleep_nanoseconds(unsigned long nseconds)
{
    timespec req, rem;
    req.tv_sec  = nseconds / bsec;
    req.tv_nsec = nseconds % bsec;
    nanosleep(&req, &rem);
}

}//namespace r64fx
