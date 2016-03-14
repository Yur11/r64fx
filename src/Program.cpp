#include "Program.hpp"
#include "Widget.hpp"
#include "Timer.hpp"

#include <iostream>
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
        long sleep_time = Timer::runTimers();
        if(sleep_time <= 0 || sleep_time > 100)
            sleep_time = 100;
        usleep(sleep_time);
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