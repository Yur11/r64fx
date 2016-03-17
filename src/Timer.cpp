#include "Timer.hpp"
#include <iostream>
#include <vector>
#include <limits>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

using namespace std;

namespace r64fx{

namespace{

void callback_stub(Timer*, void*){}

struct TimerImpl{
    Timer*  iface          = nullptr;
    long    interval       = 100;
    long    wakeup_time    = numeric_limits<long>::max();
    bool    is_running     = false;
    void    (*callback)(Timer* timer, void* data)
                           = callback_stub;
    void*   callback_data  = nullptr;
};

#define m_impl ((TimerImpl*)m)


struct Thread{
    pthread_t thread;
    vector<TimerImpl*> timers;
};


constexpr int max_threads = 16;
Thread g_threads[max_threads];
pthread_mutex_t g_thread_mutex = PTHREAD_MUTEX_INITIALIZER;


Thread* get_thread()
{
    pthread_t this_thread = pthread_self();

    Thread* thread       = nullptr;
    Thread* empty_thread = nullptr;

    pthread_mutex_lock(&g_thread_mutex);

    for(int i=0; i<max_threads; i++)
    {
        if(g_threads[i].timers.empty())
        {
            if(empty_thread == nullptr)
            {
                empty_thread = g_threads + i;
            }
        }
        else
        {
            if(pthread_equal(this_thread, g_threads[i].thread))
                thread = g_threads + i;
            break;
        }
    }

    pthread_mutex_unlock(&g_thread_mutex);

    if(!thread)
    {
        if(empty_thread == nullptr)
        {
            cerr << "Too may timer threads!\n";
        }
        else
        {
            thread = empty_thread;
            thread->thread = this_thread;
        }
    }

    return thread;
}


}//namespace


Timer::Timer(long interval)
{
    m = new TimerImpl;
    m_impl->iface = this;
    setInterval(interval);

    Thread* thread = get_thread();numeric_limits<long>::max();
    if(!thread)
        return;

    thread->timers.push_back(m_impl);
}


Timer::~Timer()
{
    if(m_impl)
        delete m_impl;
}


void Timer::setInterval(long interval)
{
    m_impl->interval = interval;
}


long Timer::interval() const
{
    return m_impl->interval;
}


void Timer::onTimeout(void (*callback)(Timer* timer, void* data), void* data)
{
    if(callback)
        m_impl->callback = callback;
    else
        m_impl->callback = callback_stub;
    m_impl->callback_data = data;
}


void Timer::start()
{
    m_impl->is_running = true;
    m_impl->wakeup_time = current_time();
}


void Timer::stop()
{
    m_impl->is_running = false;
}


bool Timer::isRunning()
{
    return m_impl->is_running;
}


int Timer::runTimers()
{
    Thread* thread = get_thread();
    if(!thread)
        return -1;

    long curr_time = current_time();

    long min_time = numeric_limits<long>::max();
    for(auto &timer : thread->timers)
    {
        if(timer->is_running && timer->interval > 0)
        {
            if(curr_time >= timer->wakeup_time)
            {
                timer->callback(timer->iface, timer->callback_data);
                timer->wakeup_time += timer->interval;
            }
            else
            {
                if(timer->wakeup_time < min_time)
                {
                    min_time = timer->wakeup_time;
                }
            }
        }
    }

    if(min_time == numeric_limits<long>::max())
        return 0;

    long time_diff = min_time - curr_time;
    if(time_diff > numeric_limits<int>::max())
        return numeric_limits<int>::max();
    else
        return time_diff;
}


long current_time()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_usec + tv.tv_sec * 1000 * 1000;
}

}//namespace r64fx