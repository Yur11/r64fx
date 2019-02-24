#include "Program.hpp"

#define R64FX_PROGRAM_ACTIONS_IMPL
#include "ProgramActions.hpp"

#include "View_Program.hpp"
#include "Project.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"

#include "SoundFileLoader.hpp"
#include "View_Filter.hpp"

#include "Debug.hpp"

#define NEW_MENU(M)\
    Widget_Menu menu_##M; menu = &menu_##M;

#define NEW_ACTION(A, STR)                                   \
    class Action_##A : public Action{                        \
        Program* m;                                          \
                                                             \
    public:                                                  \
        Action_##A(Program* pp) : Action(STR), m(pp) {}      \
                                                             \
        virtual void exec() { m->act##A(); }                 \
    } act_##A(this);                                         \
                                                             \
    menu->addAction(&act_##A)

#define ADD_MENU(M, S)\
    menu_##M.setOrientation(Orientation::Vertical);\
    menu_##M.resizeAndRealign();\
    view_program.addMenu(&menu_##M, S);


using namespace std;

namespace r64fx{

struct Program : public View_ProgramEventIface{
    bool m_running = true;

    View_Program* m_view_program = nullptr;

    LinkedList<Project> open_projects;
    Project* current_project = nullptr;

    int exec(int argc, char** argv)
    {
        View_Program view_program(this);
        m_view_program = &view_program;

        Widget_Menu* menu = nullptr;

        NEW_MENU  (Session);
        NEW_ACTION(NewSession,     "New Session");
        NEW_ACTION(OpenSession,    "Open Session...");
        NEW_ACTION(SaveSession,    "Save Session");
        NEW_ACTION(SaveSessionAs,  "Save Session As...");
        NEW_ACTION(Quit,           "Quit");
        ADD_MENU  (Session, "Session");

        NEW_MENU  (Project);
        NEW_ACTION(NewProject,     "New Project");
        NEW_ACTION(OpenProject,    "Open Project...");
        NEW_ACTION(SaveProject,    "Save Project");
        NEW_ACTION(SaveProjectAs,  "Save Project As...");
        NEW_ACTION(CreatePlayer,   "Create Player");
        NEW_ACTION(CloseProject,   "Close Project");
        ADD_MENU  (Project, "Project");

        NEW_MENU  (Edit);
        NEW_ACTION(Cut,            "Cut");
        NEW_ACTION(Copy,           "Copy");
        NEW_ACTION(Paste,          "Paste");
        NEW_ACTION(Undo,           "Undo");
        NEW_ACTION(Redo,           "Redo");
        ADD_MENU  (Edit, "Edit");

        NEW_MENU  (View);
        NEW_ACTION(NoView,         "No View");
        ADD_MENU  (View, "View");

        NEW_MENU  (Help);
        NEW_ACTION(NoHelp,         "No Help");
        ADD_MENU  (Help, "Help");

        view_program.resizeAndRealign();
        view_program.openWindow();

        actNewProject();

        while(m_running)
        {
            auto time = Timer::runTimers();
            sleep_nanoseconds(time);
        }

        view_program.closeWindow();
        closeAllProjects();

        return 0;
    }

    void actNewSession()
    {

    }

    void actOpenSession()
    {

    }

    void actSaveSession()
    {

    }

    void actSaveSessionAs()
    {

    }

    void actQuit()
    {
        m_running = false;
    }

    void actNewProject()
    {
        auto project = new Project;
        open_projects.append(project);
        setCurrentProject(project);

        m_view_program->addMainPartOption(project, "Untitled");
        m_view_program->setMainPartWidget(project->view());
        m_view_program->repaint();
    }

    void setCurrentProject(Project* project)
    {
        current_project = project;
    }

    void actOpenProject()
    {

    }

    void actSaveProject()
    {

    }

    void actSaveProjectAs()
    {

    }

    void closeAllProjects()
    {
        while(!open_projects.empty())
        {
            auto proj = open_projects.last();
            open_projects.remove(proj);
            delete proj;
        }
    }

    void actCreatePlayer()
    {
        cout << "Create Player!\n";
    }

    void actCloseProject()
    {
        
    }

    void actCut()
    {

    }

    void actCopy()
    {

    }

    void actPaste()
    {

    }

    void actUndo()
    {

    }

    void actRedo()
    {

    }

    void actNoView()
    {

    }

    void actNoHelp()
    {

    }

    void initActions();

    void cleanupActions();

    virtual void mainPartOptionSelected(void* option)
    {

    }
};

int exec(int argc, char** argv)
{
    return Program().exec(argc, argv);
}

}//namespace r64fx
