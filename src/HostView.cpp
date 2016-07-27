#include "HostView.hpp"
#include "Widget.hpp"
#include "Widget_Menu.hpp"

namespace r64fx{

class HostViewPrivate : public Widget{
    HostViewControllerIface* m_controller_iface = nullptr;

    Action*  m_action_hello      = nullptr;
    Action*  m_action_doctor     = nullptr;
    Action*  m_action_name       = nullptr;
    Action*  m_action_continue   = nullptr;
    Action*  m_action_yesterday  = nullptr;
    Action*  m_action_tomorrow   = nullptr;
    Action*  m_action_bla        = nullptr;
    Action*  m_action_exit       = nullptr;

    Widget_Menu* m_main_menu = nullptr;
    Widget_Menu* m_file_menu = nullptr;
    Widget_Menu* m_edit_menu = nullptr;
    Widget_Menu* m_view_menu = nullptr;
    Widget_Menu* m_help_menu = nullptr;

public:
    HostViewPrivate(HostViewControllerIface* controller_iface);

    ~HostViewPrivate();

    void requestExit();

protected:
    virtual void closeEvent();
};

HostView::HostView(HostViewControllerIface* controller_iface)
{
    p = new HostViewPrivate(controller_iface);
    p->show();
}


HostView::~HostView()
{
    if(p)
    {
        delete p;
    }
}


class ActionExit : public Action{
    HostViewPrivate* p = nullptr;

public:
    ActionExit(HostViewPrivate* p) : Action("Exit"), p(p) {}

    virtual void exec()
    {
        p->requestExit();
    }
};



HostViewPrivate::HostViewPrivate(HostViewControllerIface* controller_iface)
: Widget(nullptr)
, m_controller_iface(controller_iface)
{
    setSize({800, 600});

    m_action_hello     = new Action("Hello");
    m_action_doctor    = new Action("Doctor");
    m_action_name      = new Action("Name");
    m_action_continue  = new Action("Continue");
    m_action_yesterday = new Action("Yesterday");
    m_action_tomorrow  = new Action("Tomorrow");
    m_action_bla       = new Action("Bla");
    m_action_exit      = new ActionExit(this);

    m_main_menu = new Widget_Menu(this);
    m_main_menu->setPosition({0, 0});
    m_main_menu->setOrientation(Orientation::Horizontal);

    m_file_menu = new Widget_Menu;
    m_file_menu->setOrientation(Orientation::Vertical);
    m_file_menu->addAction(m_action_hello);
    m_file_menu->addAction(m_action_doctor);
    m_file_menu->addAction(m_action_name);
    m_file_menu->addAction(m_action_continue);
    m_file_menu->addAction(m_action_yesterday);
    m_file_menu->addAction(m_action_tomorrow);
    m_file_menu->addAction(m_action_bla);
    m_file_menu->addAction(m_action_exit);
    m_file_menu->resizeAndRealign();

    m_edit_menu = new Widget_Menu;
    m_edit_menu->setOrientation(Orientation::Vertical);
    m_edit_menu->addAction(m_action_hello);
    m_edit_menu->addAction(m_action_doctor);
    m_edit_menu->addAction(m_action_name);
    m_edit_menu->addAction(m_action_continue);
    m_edit_menu->addAction(m_action_yesterday);
    m_edit_menu->addAction(m_action_tomorrow);
    m_edit_menu->addAction(m_action_bla);
    m_edit_menu->addAction(m_action_exit);
    m_edit_menu->resizeAndRealign();

    m_view_menu = new Widget_Menu;
    m_view_menu->setOrientation(Orientation::Vertical);
    m_view_menu->addAction(m_action_hello);
    m_view_menu->addAction(m_action_doctor);
    m_view_menu->addAction(m_action_name);
    m_view_menu->addAction(m_action_continue);
    m_view_menu->addAction(m_action_yesterday);
    m_view_menu->addAction(m_action_tomorrow);
    m_view_menu->addAction(m_action_bla);
    m_view_menu->addAction(m_action_exit);
    m_view_menu->resizeAndRealign();

    m_help_menu = new Widget_Menu;
    m_help_menu->setOrientation(Orientation::Vertical);
    m_help_menu->addAction(m_action_hello);
    m_help_menu->addAction(m_action_doctor);
    m_help_menu->addAction(m_action_name);
    m_help_menu->addAction(m_action_continue);
    m_help_menu->addAction(m_action_yesterday);
    m_help_menu->addAction(m_action_tomorrow);
    m_help_menu->addAction(m_action_bla);
    m_help_menu->addAction(m_action_exit);
    m_help_menu->resizeAndRealign();

    m_main_menu->addSubMenu(m_file_menu, "File");
    m_main_menu->addSubMenu(m_edit_menu, "Edit");
    m_main_menu->addSubMenu(m_view_menu, "View");
    m_main_menu->addSubMenu(m_help_menu, "Help");
    m_main_menu->resizeAndRealign();
}


HostViewPrivate::~HostViewPrivate()
{
    close();
    delete m_main_menu;
    delete m_file_menu;
    delete m_action_hello;
    delete m_action_doctor;
    delete m_action_name;
    delete m_action_continue;
    delete m_action_yesterday;
    delete m_action_tomorrow;
    delete m_action_bla;
    delete m_action_exit;
}


void HostViewPrivate::requestExit()
{
    m_controller_iface->exitRequest();
}


void HostViewPrivate::closeEvent()
{
    requestExit();
}

}//namespace r64fx