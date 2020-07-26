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

#include "MainLoop.hpp"
#include "Options.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"

#ifndef R64FX_HEADLESS
#include "MainWindow.hpp"
#endif//R64FX_HEADLESS

using std::cout;

namespace r64fx{

class Main : public MainLoop{
    MainWindow m_main_window;
#endif//R64FX_HEADLESS

public:
    int run()
    {

#ifndef R64FX_HEADLESS
        m_main_window.onClose([](void* data){ auto self = (Main*)data; self->mainWindowClosed(); }, this);
        m_main_window.open();

        MainLoop::run();

        return 0;
    }

private:
#ifndef R64FX_HEADLESS
    inline void mainWindowClosed()
    {
        MainLoop::stop();
    }
#endif//R64FX_HEADLESS
};//Main

}//namespace r64fx


int main(int argc, char** argv)
{
    using namespace r64fx;

    if(int status = g_options.parse(argc, argv); status != 0)
            return status > 0 ? 0 : 1;

    Main program;

    return program.run();
}
