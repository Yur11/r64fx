#include <iostream>
#include "Timer.hpp"
#include "TimeUtils.hpp"

using namespace std;
using namespace r64fx;

bool g_running = true;
int g_num = 0;

int main()
{
    auto timer1 = new Timer(2000 * 1000);
    timer1->onTimeout([](Timer* timer, void* arg){
        cout << "Timer1: " << (++g_num) << "\n";
    }, nullptr);
    timer1->start();

    auto timer2 = new Timer(1000 * 1000);
    timer2->onTimeout([](Timer* timer, void* arg){
        cout << "Timer2: " << g_num << "\n";
        if(g_num >= 10)
        {
            g_running = false;
        }
    }, nullptr);
    timer2->start();

    auto timer3 = new Timer(1000 * 1000);
    timer3->onTimeout([](Timer* timer, void* arg){
        cout << "Timer3 suicide!\n";
        timer->suicide();
        delete timer;
        g_running = false;
    }, nullptr);
    timer3->start(10 * 1000 * 1000);

    auto timer_thread_id = Timer::reserveThreadId();
    while(g_running)
    {
        auto sleep_time = Timer::runTimers(timer_thread_id);
        sleep_nanoseconds(sleep_time);
    }
    delete timer1;
    delete timer2;
    Timer::cleanup();
    Timer::freeThreadId(timer_thread_id);

    return 0;
}
