#include "Program.hpp"

#define R64FX_PROGRAM_ACTIONS_IMPL
#include "ProgramActions.hpp"

#include "MainView.hpp"

#include "Timer.hpp"
#include "sleep.hpp"

#include <iostream>

using namespace std;

namespace r64fx{
    
Program*         g_program  = nullptr;
ProgramActions*  g_acts     = nullptr;

struct ProgramPrivate{
    bool running = true;
    
    MainView* main_view = nullptr;
    
    void exec()
    {
        main_view = new MainView;
        main_view->show();
        
        while(running)
        {
            Timer::runTimers();
            sleep_microseconds(5000);
        }
        
        main_view->close();
        delete main_view;
    }
    
    void quit()
    {
        running = false;
    }
    
    void initActions();
    
    void cleanupActions();
};


class Action_Quit : public Action{
    ProgramPrivate* m;
    
public:
    Action_Quit(ProgramPrivate* pp) : Action("Quit"), m(pp) {}
    
    virtual void exec() { m->quit(); }
};


void ProgramPrivate::initActions()
{
    g_acts = new ProgramActions;
    
    g_acts->quit_act = new Action_Quit(this);
}


void ProgramPrivate::cleanupActions()
{
    delete g_acts->quit_act;
    
    delete g_acts;
}


Program::Program(int argc, char** argv)
{
    if(g_program)
    {
        cerr << "Refusing to create another Program instance!\n";
        abort();
    }
    g_program = this;
    
    m = new ProgramPrivate;
    m->initActions();
}
    
    
Program::~Program()
{
    m->cleanupActions();
    delete m;
}


int Program::exec()
{
    m->exec();
    return 0;
}
    
}//namespace r64fx
