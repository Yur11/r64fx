#include "current_time.hpp"
#include <time.h>

namespace r64fx{

constexpr unsigned long bseq = 1000 * 1000 * 1000;

unsigned long current_nanoseconds()
{
    timespec spec;
    clock_gettime(CLOCK_MONOTONIC_RAW, &spec);
    return spec.tv_nsec + spec.tv_sec * bseq;
}

}//namespace r64fx
