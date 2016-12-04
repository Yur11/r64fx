#include <iostream>
#include "Timer.hpp"
#include "sleep.hpp"

using namespace std;
using namespace r64fx;

bool g_running = true;
int g_num = 0;

int main()
{
    auto timer1 = new Timer(1000);
    timer1->onTimeout([](Timer* timer, void* arg){
        cout << "Timer1: " << (g_num++) << "\n";
        if(g_num == 20)
        {
            g_running = false;
        }
    }, nullptr);
    timer1->start();

    auto timer2 = new Timer(5000);
    timer2->onTimeout([](Timer* timer, void* arg){
        cout << "Timer2 suicide!\n";
        timer->suicide();
        delete timer;
    }, nullptr);
    timer2->start();

    while(g_running)
    {
        auto timeout = Timer::runTimers();
        sleep_microseconds(timeout);
    }
    delete timer1;
    Timer::cleanup();

    return 0;
}
