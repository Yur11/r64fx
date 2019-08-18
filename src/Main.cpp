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

#include "Timer.hpp"
#include "TimeUtils.hpp"

#ifndef R64FX_HEADLESS
#include "MainWindow.hpp"
#endif//R64FX_HEADLESS

using std::cout;

namespace r64fx{

class Main{

bool m_running = false;

#ifndef R64FX_HEADLESS
MainWindow m_main_window;
#endif//R64FX_HEADLESS

public:
int run(int argc, char** argv)
{
    m_running = true;

#ifndef R64FX_HEADLESS
    m_main_window.onClose([](void* data){ auto self = (Main*)data; self->mainWindowClosed(); }, this);
    m_main_window.open();
#endif//R64FX_HEADLESS

    while(m_running)
    {
        auto time = Timer::runTimers();
        sleep_nanoseconds(time);
    }

#ifndef R64FX_HEADLESS
    m_main_window.close();
#endif//R64FX_HEADLESS

    return 0;
}

private:
#ifndef R64FX_HEADLESS
inline void mainWindowClosed()
{
    m_running = false;
}
#endif//R64FX_HEADLESS

};//Main

}//namespace r64fx

int main(int argc, char** argv) { return r64fx::Main().run(argc, argv); }