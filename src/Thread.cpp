#include "Thread.hpp"

namespace r64fx{

#ifdef R64FX_USE_PTHREAD
Thread::Thread()
{

}


Thread::Thread(pthread_t pthread)
: m_pthread(pthread)
{

}


bool Thread::run(void* fun(void* arg), void* arg)
{
    return pthread_create(&m_pthread, nullptr, fun, arg) == 0;
}


void* Thread::join()
{
    void* result = nullptr;
    pthread_join(m_pthread, &result);
    return result;
}


bool Thread::isCurrentThread() const
{
    pthread_t current_thread = pthread_self();
    return pthread_equal(m_pthread, current_thread);
}


Thread Thread::thisThread()
{
    return pthread_self();
}


bool Thread::operator==(const Thread &other)
{
    return pthread_equal(m_pthread, other.pthread());
}
#endif//R64FX_USE_PTHREAD

}//namespace r64fx