#include "Mutex.hpp"

namespace r64fx{

void Mutex::lock()
{
    pthread_mutex_lock(&mutex);
}


bool Mutex::tryLock()
{
    return pthread_mutex_trylock(&mutex) == 0;
}


void Mutex::unlock()
{
    pthread_mutex_unlock(&mutex);
}

}//namespace