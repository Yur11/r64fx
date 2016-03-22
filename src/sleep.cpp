#include "sleep.hpp"
#include <unistd.h>

namespace r64fx{

void sleep_microseconds(long useconds)
{
    ::usleep(useconds);
}

}//namespace r64fx