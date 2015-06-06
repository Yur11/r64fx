#include "Program.hpp"
#include "Window.hpp"

#include "gui_implementation_iface.hpp"

#include <iostream>
#include <vector>
#include <unistd.h>

using namespace std;

namespace r64fx{
    
namespace{
    Program* program_singleton_instance = nullptr;
    std::vector<Window*> all_windows;
}
    
Program::Program(int argc, char* argv[])
: m_argc(argc)
, m_argv(argv)
{
    if(program_singleton_instance != nullptr)
    {
        cerr << "Refusing to create a second program instance!\n";
        abort();
    }
    
    program_singleton_instance = this;
}


Program::~Program()
{
    
}


int Program::exec()
{
    setup();
    
    while(m_should_be_running)
    {
        process_some_events(this);
        usleep(100);
    }
    
    cleanup();
    
    return 0;
}


void Program::quit()
{
    m_should_be_running = false;
}


Window* Program::createWindow(int w, int h, const char* title)
{
    Window* window = Window::createNew();
    if(window)
    {
        all_windows.push_back(window);
    }
    
    return window;
}


void Program::destroyWindow(Window* window)
{
    cleanup_window(window);
    auto it = all_windows.begin();
    while(it != all_windows.end())
    {
        if(*it == window)
        {
            all_windows.erase(it);
            delete window;
            return;
        }
        it++;
    }
}


void Program::mousePressEvent(Window* window, MouseEvent* event)
{
    window->mousePressEvent(event);
}


void Program::mouseReleaseEvent(Window* window, MouseEvent* event)
{
    window->mouseReleaseEvent(event);
}


void Program::mouseMoveEvent(Window* window, MouseEvent* event)
{
    window->mouseMoveEvent(event);
}


void Program::keyPressEvent(Window* window, KeyEvent* event)
{
    window->keyPressEvent(event);
}


void Program::keyReleaseEvent(Window* window, KeyEvent* event)
{
    window->keyReleaseEvent(event);
}


void Program::closeEvent(Window* window)
{
    window->closeEvent();
}


void Program::setup()
{
    
}


void Program::cleanup()
{
    
}
    
}//namespace r64fx