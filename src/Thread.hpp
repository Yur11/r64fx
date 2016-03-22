#ifndef R64FX_THREAD_HPP
#define R64FX_THREAD_HPP

#ifdef R64FX_USE_PTHREAD
#include <pthread.h>
#endif//R64FX_USE_PTHREAD

namespace r64fx{

class Thread{
#ifdef R64FX_USE_PTHREAD
    pthread_t m_pthread;
#endif//R64FX_USE_PTHREAD

public:
    Thread();

#ifdef R64FX_USE_PTHREAD
    Thread(pthread_t pthread);

    inline pthread_t pthread() const { return m_pthread; }
#endif//R64FX_USE_PTHREAD

    bool run(void* fun(void* arg), void* arg = nullptr);

    void* join();

    bool isCurrentThread() const;

    static Thread thisThread();

    bool operator==(const Thread &other);
};

}//namespace

#endif//R64FX_THREAD_HPP