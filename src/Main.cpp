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
#include "GridView.hpp"
#include "Options.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"

using namespace std;

namespace r64fx{

class Main{
    Engine*      m_engine       = nullptr;
    MainWindow*  m_main_window  = nullptr;
    GridView*    m_grid_view_window    = nullptr;
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

    m_grid_view_window = new(std::nothrow) GridView;
    if(!m_grid_view_window)
    {
        cerr << "Failed to create grid view window!\n";
        return 1;
    }

    m_main_window->onClose([](void* data){
        auto self = (Main*)data; self->mainWindowClosed();
    }, this);
    m_main_window->open();

    m_grid_view_window->openWindow();

    m_engine = Engine::newInstance();

    auto timer_thread_id = Timer::reserveThreadId();

    m_running = true;
    while(m_running)
    {
        auto time = Timer::runTimers(timer_thread_id);
        sleep_nanoseconds(time);
    }

    Timer::freeThreadId(timer_thread_id);

    Engine::deleteInstance(m_engine);

    if(m_grid_view_window)
    {
        delete m_grid_view_window;
        m_grid_view_window = nullptr;
    }

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
