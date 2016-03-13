#ifndef R64FX_MUTEX_HPP
#define R64FX_MUTEX_HPP

#include <pthread.h>

namespace r64fx{

class Mutex{
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

public:
    void lock();

    bool tryLock();

    void unlock();
};

}//namespace

#endif//R64FX_MUTEX_HPP