#include "Program.hpp"

#include "Strings.hpp"
#include "View_Program.hpp"
#include "Project.hpp"
#include "Timer.hpp"
#include "TimeUtils.hpp"

#include "SoundFileLoader.hpp"
#include "View_Filter.hpp"

#include "Debug.hpp"

#define NEW_MENU(M)\
    Widget_Menu menu_##M; menu = &menu_##M;

#define NEW_ACTION(A)                                   \
    class Action_##A : public Action{                   \
        Program* m;                                     \
                                                        \
    public:                                             \
        Action_##A(Program* pp)                         \
            : Action(gstr[(int)Strings::A]), m(pp) {}   \
                                                        \
        virtual void exec() { m->act##A(); }            \
    } act_##A(this);                                    \
                                                        \
    menu->addAction(&act_##A)

#define ADD_MENU(M)                                     \
    menu_##M.setOrientation(Orientation::Vertical);     \
    menu_##M.resizeAndRealign();                        \
    view_program.addMenu(&menu_##M, gstr[(int)Strings::M]);


/* Default English strings */
extern "C"{ extern const char* Strings_Eng[(int)r64fx::Strings::StringCount]; }

const char** gstr = Strings_Eng;


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

        NEW_MENU   (Session);
        NEW_ACTION (NewSession);
        NEW_ACTION (OpenSession);
        NEW_ACTION (SaveSession);
        NEW_ACTION (SaveSessionAs);
        NEW_ACTION (Quit);
        ADD_MENU   (Session);

        NEW_MENU   (Project);
        NEW_ACTION (NewProject);
        NEW_ACTION (OpenProject);
        NEW_ACTION (SaveProject);
        NEW_ACTION (SaveProjectAs);
        NEW_ACTION (CreatePlayer);
        NEW_ACTION (CloseProject);
        ADD_MENU   (Project);

        NEW_MENU   (Edit);
        NEW_ACTION (Cut);
        NEW_ACTION (Copy);
        NEW_ACTION (Paste);
        NEW_ACTION (Undo);
        NEW_ACTION (Redo);
        ADD_MENU   (Edit);

        NEW_MENU   (View);
        NEW_ACTION (NoView);
        ADD_MENU   (View);

        NEW_MENU   (Help);
        NEW_ACTION (NoHelp);
        ADD_MENU   (Help);

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
