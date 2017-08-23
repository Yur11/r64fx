#include "Program.hpp"

#define R64FX_PROGRAM_ACTIONS_IMPL
#include "ProgramActions.hpp"

#include "View_Program.hpp"
#include "Project.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"

#include "Player.hpp"
#include "SoundFileLoader.hpp"
#include "View_Filter.hpp"
#include "Module_Filter.hpp"
#include "Module_Player.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

ProgramActions*  g_acts     = nullptr;

struct ProgramPrivate : public View_ProgramEventIface{
    bool running = true;

    View_Program* view_program = nullptr;

//     View_Filter* view_filter = nullptr;
//     Module_Filter* module_filter = nullptr;
// 
//     FilterClass fc;

    Module_Player* m_module_player = nullptr;

    SoundFileLoader sfl;
    SoundFileLoader::Port* m_sflp = nullptr;

    LinkedList<Project> open_projects;
    Project* current_project = nullptr;

    int exec(int argc, char** argv)
    {
        initActions();

        view_program = new View_Program(this);
        view_program->openWindow();

//         fc.newRoot<Pole>({0.0f, 0.1f});
//         fc.newRoot<Zero>({-0.5f, 0.5f});
//         fc.newRoot<Pole>({0.0f, 0.1f});
//         fc.newRoot<Zero>({-0.5f, 0.5});
//         fc.newRoot<Pole>({0.0f, 0.1f});
//         fc.newRoot<Zero>({-0.5f, 0.5f});
//         fc.newRoot<Pole>({0.0f, 0.1f});
//         fc.newRoot<Zero>({-0.5f, 0.5});
//         fc.updateIndices();

//         view_filter = new View_Filter(nullptr);
//         view_filter->openWindow();
//         view_filter->setFilterClass(&fc);

//         module_filter = new Module_Filter;
//         module_filter->engage([](Module* module, void* arg){
//             auto pp = (ProgramPrivate*) arg;
//             pp->filterEngaged(pp->module_filter);
//         }, this);
// 
//         view_filter->onChanged([](FilterClass* fc, void* data){
//             auto mf = (Module_Filter*) data;
//             mf->setFilterClass(fc);
//         }, module_filter);

        m_module_player = new Module_Player;
        m_module_player->engage([](Module* module, void* arg){
            auto pp = (ProgramPrivate*) arg;
            pp->playerEngaged(pp->m_module_player);
        }, this);

        newProject();

        m_sflp = sfl.newPort();
        m_sflp->open("./35-Kick1Alt-5.wav", [](SoundFileHandle* handle, void* data){
            auto self = (ProgramPrivate*) data;
            self->fileOpened(handle);
        }, this);

        while(running)
        {
            m_sflp->run();

            auto time = Timer::runTimers();
            sleep_nanoseconds(time);
        }

        sfl.deletePort(m_sflp);

//         view_filter->closeWindow();
//         delete view_filter;
// 
//         delete module_filter;

        delete m_module_player;

        view_program->closeWindow();
        closeAllProjects();
        delete view_program;

        cleanupActions();

        return 0;
    }

    void fileOpened(SoundFileHandle* handle)
    {
        if(handle)
        {
            cout << "fileOpened!\n";
            cout << handle << "\n";
            m_sflp->getFileProperties(handle, [](SoundFileHandle* handle, float sample_rate, int frame_count, int component_count, void* data){
                auto self = (ProgramPrivate*) data;
                self->fileStats(handle, sample_rate, component_count, frame_count);
            }, this);
        }
    }

    void fileStats(SoundFileHandle* handle, float sample_rate, float component_count, float frame_count)
    {
        cout << ">> " << sample_rate << ", " << component_count << ", " << frame_count << "\n";
        m_sflp->loadChunk(handle, 0, 32, [](SoundFileHandle* handle, float* chunk, int index, int nframes, void* data){
            auto self = (ProgramPrivate*) data;
            self->fileChunk(handle, chunk, nframes);
        }, this);
    }

    void fileChunk(SoundFileHandle* handle, float* chunk, int nframes)
    {
        cout << "chunk: " << chunk << " -> " << nframes << "\n";
        m_sflp->freeChunk(handle, chunk, [](SoundFileHandle* handle, void* data){
            auto self = (ProgramPrivate*) data;
            self->freeChunk(handle);
        }, this);
    }

    void freeChunk(SoundFileHandle* handle)
    {
        cout << "free\n";
        m_sflp->close(handle, [](void* data){
            auto self = (ProgramPrivate*) data;
            self->fileClosed();
        }, this);
    }

    void fileClosed()
    {
        cout << "file closed!\n";
    }

    void filterEngaged(Module_Filter* module_filter)
    {
//         cout << "Filter Engaged: " << module_filter << ", " << osc << "\n";
//         module_filter->setFilterClass(&fc);
    }

    void filterDisengaged(Module_Filter* module_filter)
    {
//         cout << "Filter Disengaged: " << module_filter << ", " << osc << "\n";
//         doQuit();
    }

    void playerEngaged(Module_Player* module_player)
    {
        cout << "Player Engaged: " << module_player << "\n";
    }

    void playerDisengaged(Module_Player* module_player)
    {
        cout << "Player Disengaged: " << module_player << "\n";
        doQuit();
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
        if(m_module_player)
        {
            m_module_player->disengage([](Module* module, void* arg){
                auto p = (ProgramPrivate*) arg;
                p->playerDisengaged(static_cast<Module_Player*>(module));
            }, this);
        }
    }

    void doQuit()
    {
        running = false;
    }

    void newProject()
    {
        auto project = new Project;
        open_projects.append(project);
        setCurrentProject(project);

        view_program->addMainPartOption(project, "Untitled");
        view_program->setMainPartWidget(project->view());
        view_program->repaint();
    }

    void setCurrentProject(Project* project)
    {
        current_project = project;
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

    void closeAllProjects()
    {
        while(!open_projects.isEmpty())
        {
            auto proj = open_projects.last();
            open_projects.remove(proj);
            delete proj;
        }
    }

    void createPlayer()
    {
        cout << "Create Player!\n";
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

    virtual void exec() { /*m->closeCurrentProject();*/ }
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
