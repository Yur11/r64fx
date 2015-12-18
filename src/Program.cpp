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

Program* g_program = nullptr;

namespace{
    bool program_should_be_running = true;

}//namespace

Program::Program(int argc, char* argv[])
{
    if(g_program != nullptr)
    {
        cerr << "Refusing to create a second program instance!\n";
        abort();
    }

    g_program = this;
}


Program::~Program()
{
}


int Program::exec()
{
    g_program->setup();
    
    while(program_should_be_running)
    {
        Widget::processEvents();
        usleep(100);
    }
    
    g_program->cleanup();
    
    return 0;
}


void Program::quit()
{
    program_should_be_running = false;
}


Program* Program::instance()
{
    return g_program;
}


void Program::setup()
{

}


void Program::cleanup()
{

}

}//namespace r64fx