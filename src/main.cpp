#include <iostream>
#include <unistd.h>

#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "Font.hpp"
#include "Widget_Button.hpp"
#include "Widget_Container.hpp"
#include "Widget_Control.hpp"
#include "Widget_Dummy.hpp"
#include "Widget_Menu.hpp"
#include "Widget_Text.hpp"
#include "Widget_ScrollArea.hpp"
#include "Widget_ScrollBar.hpp"
#include "ImageAnimation.hpp"
#include "KeyEvent.hpp"
#include "SoundDriver.hpp"
#include "SignalGraph.hpp"
#include "Timer.hpp"
#include "Thread.hpp"
#include "sleep.hpp"
#include "SignalNode_AudioIO.hpp"
#include "SignalNode_Controller.hpp"
#include "SignalNode_Oscillator.hpp"
#include "SignalNode_Player.hpp"
#include "SoundFile.hpp"
#include "SignalData.hpp"


using namespace std;
using namespace r64fx;

Font* g_Font = nullptr;


class HelloAction : public Action{
public:
    using Action::Action;

    virtual void exec()
    {
        cout << "Hello\n";
    }
};


class DoctorAction : public Action{
public:
    using Action::Action;

    virtual void exec()
    {
        cout << "Doctor\n";
    }
};


class NameAction : public Action{
public:
    using Action::Action;

    virtual void exec()
    {
        cout << "Name\n";
    }
};


class ContinueAction : public Action{
public:
    using Action::Action;

    virtual void exec()
    {
        cout << "Continue\n";
    }
};


class YesterdayAction : public Action{
public:
    using Action::Action;

    virtual void exec()
    {
        cout << "Yesterday\n";
    }
};


class TommorowAction : public Action{
public:
    using Action::Action;

    virtual void exec()
    {
        cout << "Tommorow\n";
    }
};


class MyWidget : public Widget_ScrollArea{
    Image m_Image;
    ImageAnimation* m_animation = nullptr;
    float m_angle = 0.0f;

    Widget_Menu* m_menu = nullptr;
    Widget_Menu* m_main_menu = nullptr;

public:
    MyWidget(Widget* parent = nullptr) : Widget_ScrollArea(parent)
    {
        {
            auto wt = new Widget_Text(this);
            wt->setPosition({100, 100});
            wt->setSize({100, 100});
        }

        {
            auto wt = new Widget_Text(this);
            wt->setPosition({210, 100});
            wt->setSize({100, 100});
        }

        {
            auto wt = new Widget_Text(this);
            wt->setPosition({100, 210});
            wt->setSize({100, 100});
        }

        {
            auto wt = new Widget_Text(this);
            wt->setPosition({210, 210});
            wt->setSize({100, 100});
        }

        {
            auto hello_action = new HelloAction("Hello");
            auto doctor_action = new DoctorAction("Doctor");
            auto name_action = new NameAction("Name");
            auto continue_action = new ContinueAction("Continue");
            auto yesterday_action = new YesterdayAction("Yesterday");
            auto tommorow_action = new TommorowAction("Tommorow");

            m_menu = new Widget_Menu;
            m_menu->setOrientation(Orientation::Vertical);
            m_menu->resizeAndReallign();
            m_menu->addAction(hello_action);
            m_menu->addAction(doctor_action);
            m_menu->addAction(name_action);
            m_menu->addAction(continue_action);
            m_menu->addAction(yesterday_action);
            m_menu->addAction(tommorow_action);
            for(int i=0; i<5; i++)
            {
                auto sub_menu = new Widget_Menu;
                sub_menu->setOrientation(Orientation::Vertical);
                sub_menu->addAction(hello_action);
                sub_menu->addAction(doctor_action);
                sub_menu->addAction(name_action);
                sub_menu->addAction(continue_action);
                sub_menu->addAction(yesterday_action);
                sub_menu->addAction(tommorow_action);
                sub_menu->resizeAndReallign();

                m_menu->addSubMenu(sub_menu, "More...");
            }
            m_menu->resizeAndReallign();

            m_main_menu = new Widget_Menu(this);
            m_main_menu->setOrientation(Orientation::Horizontal);
            m_main_menu->setPosition({0, 0});
            string menu_names[5] = { "File", "Edit", "View", "Tools", "Help" };
            for(int i=0; i<5; i++)
            {
                auto menu = new Widget_Menu;
                menu->setOrientation(Orientation::Vertical);
                menu->addAction(hello_action);
                menu->addAction(doctor_action);
                menu->addAction(name_action);
                menu->addAction(continue_action);
                menu->addAction(yesterday_action);
                menu->addAction(tommorow_action);
                menu->resizeAndReallign();

                m_main_menu->addSubMenu(menu, menu_names[i]);
            }
            m_main_menu->resizeAndReallign();
        }
    }

    ~MyWidget()
    {
        if(m_animation)
            delete m_animation;
    }

    virtual void updateEvent(UpdateEvent* event)
    {
        unsigned char fg[4] = { 0,   0,   0,   0 };
        unsigned char bg[4] = { 127, 180, 255, 0 };

        m_Image.load(width(), height(), 4);
        fill(&m_Image, bg);
        auto r = intersection(
            Rect<int>(0, 0, 10, 10) + offset(),
            Rect<int>(0, 0, m_Image.width(), m_Image.height())
        );
        if(r.width() > 0 && r.height() > 0)
        {
            fill(&m_Image, fg, r);
        }

        auto painter = event->painter();
        painter->putImage(&m_Image);

        Widget_ScrollArea::updateEvent(event);
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        if(event->button() == MouseButton::Right())
        {
            m_menu->showAt(event->position() + offset(), this);
        }
        else
        {
            update();
        }
    }

    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        if(event->button() & MouseButton::Left())
        {
            setOffset(offset() + event->delta());
            update();
        }
    }

    virtual void keyPressEvent(KeyPressEvent* event)
    {
        if(event->key() == Keyboard::Key::Escape)
        {
            Program::quit();
        }

        update();
        Widget::keyPressEvent(event);
    }

    virtual void closeEvent()
    {
        Program::quit();
    }
};


class MyProgram : public Program{

public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
        g_Font = new Font("", 20, 72);

        auto mw = new MyWidget();
        mw->setSize({300, 300});
        mw->show();
    }

    virtual void cleanup()
    {
        cout << "Cleanup!\n";

        if(g_Font)
            delete g_Font;
    }
};


int main(int argc, char* argv[])
{
    MyProgram prog(argc, argv);
    return prog.exec();
}
