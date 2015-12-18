#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "KeyEvent.hpp"
#include "WidgetFlags.hpp"

#include <iostream>
#include <vector>
#include <unistd.h>

using namespace std;

namespace r64fx{
    
namespace{
    Program*         program_singleton_instance = nullptr;
    MouseButton      pressed_buttons;
}


Program::Program(int argc, char* argv[])
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
        Widget::processEvents();
        usleep(100);
    }
    
    cleanup();
    
    return 0;
}


void Program::quit()
{
    m_should_be_running = false;
}


Program* Program::instance()
{
    return program_singleton_instance;
}


void Program::setup()
{

}


void Program::cleanup()
{

}

}//namespace r64fx