#include "Program.hpp"

#define R64FX_PROGRAM_ACTIONS_IMPL
#include "ProgramActions.hpp"

#include "MainView.hpp"

#include "Project.hpp"

#include "Timer.hpp"
#include "sleep.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

ProgramActions*  g_acts     = nullptr;

struct ProgramPrivate : public MainViewEventIface{
    bool running = true;
    
    MainView* main_view = nullptr;
    
    LinkedList<Project> open_projects;
    Project* current_project = nullptr;

    int exec(int argc, char** argv)
    {
        initActions();

        main_view = new MainView(this);
        main_view->show();
        
        newProject();
        
        while(running)
        {
            Timer::runTimers();
            sleep_microseconds(5000);
        }
        
        main_view->close();
        delete main_view;

        cleanupActions();

        return 0;
    }
    
    void newSession()
    {
        
    }
    
    void openSession()
    {
        
    }

    void saveSession()
    {

    }

    void saveSessionAs()
    {
        
    }

    void quit()
    {
        running = false;
    }

    void newProject()
    {
        auto project = new Project;
        open_projects.append(project);
        current_project = project;
        
        main_view->addMainPartOption(project, "Untitled");
        main_view->setMainPartWidget(project->view());
        main_view->repaint();
    }

    void openProject()
    {
        
    }

    void saveProject()
    {
        
    }

    void saveProjectAs()
    {
        
    }

    void createPlayer()
    {
        cout << "Create Player!\n";
    }

    void closeProject()
    {
        if(current_project)
        {
            open_projects.remove(current_project);
            delete current_project;
        }
    }

    void cut()
    {
        
    }

    void copy()
    {
        
    }

    void paste()
    {
        
    }

    void undo()
    {
        
    }

    void redo()
    {
        
    }
    
    void initActions();

    void cleanupActions();
    
    virtual void mainPartOptionSelected(void* option)
    {
        
    }
};


class Action_NewSession : public Action{
    ProgramPrivate* m;

public:
    Action_NewSession(ProgramPrivate* pp) : Action("New Session"), m(pp) {}

    virtual void exec() { m->newSession(); }
};


class Action_OpenSession : public Action{
    ProgramPrivate* m;

public:
    Action_OpenSession(ProgramPrivate* pp) : Action("Open Session..."), m(pp) {}

    virtual void exec() { m->openSession(); }
};


class Action_SaveSession : public Action{
    ProgramPrivate* m;

public:
    Action_SaveSession(ProgramPrivate* pp) : Action("Save Session"), m(pp) {}

    virtual void exec() { m->saveSession(); }
};


class Action_SaveSessionAs : public Action{
    ProgramPrivate* m;

public:
    Action_SaveSessionAs(ProgramPrivate* pp) : Action("Save Session As..."), m(pp) {}

    virtual void exec() { m->saveSessionAs(); }
};


class Action_Quit : public Action{
    ProgramPrivate* m;

public:
    Action_Quit(ProgramPrivate* pp) : Action("Quit"), m(pp) {}

    virtual void exec() { m->quit(); }
};


class Action_NewProject : public Action{
    ProgramPrivate* m;

public:
    Action_NewProject(ProgramPrivate* pp) : Action("New Project"), m(pp) {}

    virtual void exec() { m->newProject(); }
};


class Action_OpenProject : public Action{
    ProgramPrivate* m;

public:
    Action_OpenProject(ProgramPrivate* pp) : Action("Open Project..."), m(pp) {}

    virtual void exec() { m->openProject(); }
};


class Action_SaveProject : public Action{
    ProgramPrivate* m;

public:
    Action_SaveProject(ProgramPrivate* pp) : Action("Save Project"), m(pp) {}

    virtual void exec() { m->saveProject(); }
};


class Action_SaveProjectAs : public Action{
    ProgramPrivate* m;

public:
    Action_SaveProjectAs(ProgramPrivate* pp) : Action("Save Project As..."), m(pp) {}

    virtual void exec() { m->saveProjectAs(); }
};


class Action_CreatePlayer : public Action{
    ProgramPrivate* m;

public:
    Action_CreatePlayer(ProgramPrivate* pp) : Action("Create Player"), m(pp) {}

    virtual void exec() { m->createPlayer(); }
};


class Action_CloseProject : public Action{
    ProgramPrivate* m;

public:
    Action_CloseProject(ProgramPrivate* pp) : Action("Close Project"), m(pp) {}

    virtual void exec() { m->closeProject(); }
};


class Action_Cut : public Action{
    ProgramPrivate* m;

public:
    Action_Cut(ProgramPrivate* pp) : Action("Cut"), m(pp) {}

    virtual void exec() { m->cut(); }
};


class Action_Copy : public Action{
    ProgramPrivate* m;

public:
    Action_Copy(ProgramPrivate* pp) : Action("Copy"), m(pp) {}

    virtual void exec() { m->copy(); }
};


class Action_Paste : public Action{
    ProgramPrivate* m;

public:
    Action_Paste(ProgramPrivate* pp) : Action("Paste"), m(pp) {}

    virtual void exec() { m->paste(); }
};


class Action_Undo : public Action{
    ProgramPrivate* m;

public:
    Action_Undo(ProgramPrivate* pp) : Action("Undo"), m(pp) {}

    virtual void exec() { m->undo(); }
};


class Action_Redo : public Action{
    ProgramPrivate* m;

public:
    Action_Redo(ProgramPrivate* pp) : Action("Redo"), m(pp) {}

    virtual void exec() { m->redo(); }
};


class Action_NoView : public Action{
    ProgramPrivate* m;

public:
    Action_NoView(ProgramPrivate* pp) : Action("No View"), m(pp) {}
    
    virtual void exec() {}
};


class Action_NoHelp : public Action{
    ProgramPrivate* m;

public:
    Action_NoHelp(ProgramPrivate* pp) : Action("No Help"), m(pp) {}

    virtual void exec() {}
};


void ProgramPrivate::initActions()
{
    g_acts = new ProgramActions;

    g_acts->new_session_act      = new Action_NewSession(this);
    g_acts->open_session_act     = new Action_OpenSession(this);
    g_acts->save_session_act     = new Action_SaveSession(this);
    g_acts->save_session_as_act  = new Action_SaveSessionAs(this);
    g_acts->quit_act             = new Action_Quit(this);

    g_acts->new_project_act      = new Action_NewProject(this);
    g_acts->open_project_act     = new Action_OpenProject(this);
    g_acts->save_project_act     = new Action_SaveProject(this);
    g_acts->save_project_as_act  = new Action_SaveProjectAs(this);
    g_acts->create_player_act    = new Action_CreatePlayer(this);
    g_acts->close_project_act    = new Action_CloseProject(this);

    g_acts->cut_act              = new Action_Cut(this);
    g_acts->copy_act             = new Action_Copy(this);
    g_acts->paste_act            = new Action_Paste(this);
    g_acts->undo_act             = new Action_Undo(this);
    g_acts->redo_act             = new Action_Redo(this);

    g_acts->no_view_act          = new Action_NoView(this);
    
    g_acts->no_help_act          = new Action_NoHelp(this);
}


void ProgramPrivate::cleanupActions()
{
    delete g_acts->new_session_act;
    delete g_acts->open_session_act;
    delete g_acts->save_session_act;
    delete g_acts->save_session_as_act;
    delete g_acts->quit_act;

    delete g_acts->new_project_act;
    delete g_acts->open_project_act;
    delete g_acts->save_project_act;
    delete g_acts->save_project_as_act;
    delete g_acts->create_player_act;
    delete g_acts->close_project_act;

    delete g_acts->cut_act;
    delete g_acts->copy_act;
    delete g_acts->paste_act;
    delete g_acts->undo_act;
    delete g_acts->redo_act;

    delete g_acts->no_view_act;

    delete g_acts->no_help_act;

    delete g_acts;
}


int exec(int argc, char** argv)
{
    ProgramPrivate p;
    return p.exec(argc, argv);
}

}//namespace r64fx
