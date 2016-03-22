#ifndef R64FX_MUTEX_HPP
#define R64FX_MUTEX_HPP

#ifdef R64FX_USE_PTHREAD
#include <pthread.h>
#endif//R64FX_USE_PTHREAD

namespace r64fx{

class Mutex{
#ifdef R64FX_USE_PTHREAD
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif//R64FX_USE_PTHREAD

public:
    void lock();

    bool tryLock();

    void unlock();
};

}//namespace

#endif//R64FX_MUTEX_HPP