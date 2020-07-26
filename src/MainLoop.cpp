#include "MainLoop.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"

namespace r64fx{

void MainLoop::run()
{
    m_running = true;
    while(m_running)
    {
        auto time = Timer::runTimers();
        sleep_nanoseconds(time);
    }
}


void MainLoop::stop()
{
    m_running = false;
}

}//namespace r64fx