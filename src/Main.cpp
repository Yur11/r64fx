/* This file is part of r64fx.
 *
 * r64fx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * r64fx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with r64fx. If not, see <https://www.gnu.org/licenses/>.
 */

/* Main Program Controller */

#include <iostream>
#include <signal.h>

#include "Engine.hpp"
#include "MainWindow.hpp"
#include "Options.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"

using namespace std;

namespace r64fx{

class Main{
    Engine*      m_engine       = nullptr;
    MainWindow*  m_main_window  = nullptr;
    bool         m_running      = false;

public:
    int run(int argc, char** argv);

    inline void stop() { m_running = false; }

private:
    void mainWindowClosed();

} g_program;


int Main::run(int argc, char** argv)
{
    if(int status = g_options.parse(argc, argv); status != 0)
        return status > 0 ? 0 : 1;

    signal(SIGINT, [](int sig){
        cout << "\nInterrupted!\n";
        g_program.stop();
    });

    m_main_window = new(std::nothrow) MainWindow;
    if(!m_main_window)
    {
        cerr << "Failed to create main window!\n";
        return 1;
    }

    m_main_window->onClose([](void* data){
        auto self = (Main*)data; self->mainWindowClosed();
    }, this);
    m_main_window->open();

    m_engine = Engine::singletonInstance();

    Module_SoundDriver msd;
    msd.name = "hello123";

    ModuleSink sdout;
    sdout.name = "out";
    sdout.nchannels = 0;
    msd.addObject(&sdout);

    ModuleSource sdin;
    sdin.name = "in";
    sdin.nchannels = 0;
    msd.addObject(&sdin);

    Module_A a;
    Module_B b;
    Module_C c;

    msd.addObject(&a);
    msd.addObject(&b);
    msd.addObject(&c);

    m_engine->update({
        ReplaceRoot(&msd),
        AlterObject(&msd, true),
        RelinkPorts(&sdout, &sdin)
    });

    auto timer_thread_id = Timer::reserveThreadId();

    m_running = true;
    while(m_running)
    {
        auto time = Timer::runTimers(timer_thread_id);
        sleep_nanoseconds(time);
    }

    Timer::freeThreadId(timer_thread_id);

    if(m_main_window)
    {
        delete m_main_window;
        m_main_window = nullptr;
    }

    return 0;
}


void Main::mainWindowClosed() { stop(); }

}//namespace r64fx


int main(int argc, char** argv)
    { return r64fx::g_program.run(argc, argv); }
