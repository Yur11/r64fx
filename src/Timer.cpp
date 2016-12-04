#include "Timer.hpp"
#include <iostream>
#include <limits>
#include "LinkedList.hpp"
#include "Thread.hpp"
#include "Mutex.hpp"
#include "current_time.hpp"

using namespace std;

namespace r64fx{

namespace{

void callback_stub(Timer*, void*){}

struct TimerImpl : public LinkedList<TimerImpl>::Node{
    Timer*  iface          = nullptr;
    long    interval       = 100;
    long    wakeup_time    = numeric_limits<long>::max();
    bool    is_running     = false;
    void    (*callback)(Timer* timer, void* data)
                           = callback_stub;
    void*   callback_data  = nullptr;
};

#define m_impl ((TimerImpl*)m)


struct TimerThread{
    Thread thread;
    LinkedList<TimerImpl> active_timers;
    LinkedList<TimerImpl> spare_timers;
};


constexpr int max_threads = 16;
TimerThread g_threads[max_threads];
Mutex g_thread_mutex;


TimerThread* get_thread()
{
    Thread this_thread = Thread::thisThread();

    TimerThread* thread       = nullptr;
    TimerThread* empty_thread = nullptr;

    g_thread_mutex.lock();

    for(int i=0; i<max_threads; i++)
    {
        if(g_threads[i].active_timers.isEmpty() && g_threads[i].spare_timers.isEmpty())
        {
            if(empty_thread == nullptr)
            {
                empty_thread = g_threads + i;
            }
        }
        else
        {
            if(this_thread == g_threads[i].thread)
            {
                thread = g_threads + i;
                break;
            }
        }
    }

    g_thread_mutex.unlock();

    if(!thread)
    {
        if(empty_thread == nullptr)
        {
            cerr << "Too may timer threads!\n";
            abort();
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
    TimerThread* thread = get_thread();
    if(!thread)
        return;

    if(thread->spare_timers.isEmpty())
    {
        m = new TimerImpl;
    }
    else
    {
        m = thread->spare_timers.last();
        thread->spare_timers.remove(m_impl);
    }

    m_impl->iface = this;
    setInterval(interval);

    thread->active_timers.append(m_impl);
}


Timer::~Timer()
{
    if(m_impl)
    {
        TimerThread* thread = get_thread();
        if(thread)
        {
            thread->active_timers.remove(m_impl);
        }

        delete m_impl;
    }
}


bool Timer::isGood() const
{
    return m != nullptr;
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


void Timer::suicide()
{
    if(m_impl)
    {
        TimerThread* thread = get_thread();
        thread->active_timers.remove(m_impl);
        thread->spare_timers.append(m_impl);
        m_impl->iface = nullptr;
        m = nullptr;
    }
}


int Timer::runTimers()
{
    TimerThread* thread = get_thread();
    if(!thread)
        return -1;

    long curr_time = current_time();

    long min_time = numeric_limits<long>::max();
    auto timer = thread->active_timers.first();
    while(timer)
    {
        //Make sure we fetch the next_timer before the callback
        //so that the timer could be removed from the active_timers list.
        auto next_timer = timer->next();
        if(timer->is_running && timer->interval >= 0)
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
        timer = next_timer;
    }

    if(min_time == numeric_limits<long>::max())
        return 0;

    long time_diff = min_time - curr_time;
    if(time_diff > numeric_limits<int>::max())
        return numeric_limits<int>::max();
    else
        return time_diff;
}


void Timer::cleanup()
{
    auto thread = get_thread();

    for(;;)
    {
        auto timer = thread->spare_timers.first();
        if(!timer)
            break;
        thread->spare_timers.remove(timer);
        delete timer;
    }
}

}//namespace r64fx
