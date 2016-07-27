#include "HostView.hpp"
#include "Widget.hpp"
#include "Widget_Menu.hpp"
#include "Painter.hpp"

namespace r64fx{

class HostViewPrivate : public Widget{
    HostViewControllerIface* m_controller_iface = nullptr;

    CallbackAction*  m_action_open_player  = nullptr;
    CallbackAction*  m_action_exit         = nullptr;

    Widget_Menu* m_main_menu = nullptr;
    Widget_Menu* m_menu_menu = nullptr;

public:
    HostViewPrivate(HostViewControllerIface* controller_iface);

    ~HostViewPrivate();

    void requestExit();

    void openPlayer();

protected:
    virtual void paintEvent(PaintEvent* event);

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


HostViewPrivate::HostViewPrivate(HostViewControllerIface* controller_iface)
: Widget(nullptr)
, m_controller_iface(controller_iface)
{
    setSize({800, 600});

    m_action_open_player = new CallbackAction("Open Player", [](void* arg, CallbackAction* act){
        auto controller = (HostViewControllerIface*) arg;
        controller->openPlayer();
    }, m_controller_iface);

    m_action_exit = new CallbackAction("Exit", [](void* arg, CallbackAction* act){
        auto controller = (HostViewControllerIface*) arg;
        controller->requestExit();
    }, m_controller_iface);

    m_main_menu = new Widget_Menu(this);
    m_main_menu->setPosition({0, 0});
    m_main_menu->setOrientation(Orientation::Horizontal);

    m_menu_menu = new Widget_Menu;
    m_menu_menu->setOrientation(Orientation::Vertical);
    m_menu_menu->addAction(m_action_open_player);
    m_menu_menu->addAction(m_action_exit);
    m_menu_menu->resizeAndRealign();

    m_main_menu->addSubMenu(m_menu_menu, "Menu");
    m_main_menu->resizeAndRealign();
}


HostViewPrivate::~HostViewPrivate()
{
    close();
    delete m_main_menu;
    delete m_menu_menu;
    delete m_action_open_player;
    delete m_action_exit;
}


void HostViewPrivate::requestExit()
{
    m_controller_iface->requestExit();
}


void HostViewPrivate::openPlayer()
{
    m_controller_iface->openPlayer();
}


void HostViewPrivate::paintEvent(PaintEvent* event)
{
    unsigned char bg[4] = {127, 127, 127, 0};
    event->painter()->fillRect({0, 0, width(), m_main_menu->height()}, bg);
    Widget::paintEvent(event);
}


void HostViewPrivate::closeEvent()
{
    requestExit();
}

}//namespace r64fx