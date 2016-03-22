#include "current_time.hpp"
#include <sys/time.h>

namespace r64fx{

long current_time()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_usec + tv.tv_sec * 1000 * 1000;
}

}//namespace r64fx