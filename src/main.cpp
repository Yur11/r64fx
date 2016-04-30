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
#include "Widget_ItemTree.hpp"
#include "Widget_Container.hpp"
#include "Widget_Control.hpp"
#include "Widget_Menu.hpp"
#include "Widget_Text.hpp"
#include "Widget_ScrollArea.hpp"
#include "KeyEvent.hpp"
#include "Timer.hpp"
#include "Thread.hpp"


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

public:
    MyWidget(Widget* parent = nullptr) : Widget_ScrollArea(parent)
    {
        auto tree = new Widget_ItemTree("I Like Pie!!", this);
        tree->setPosition({100, 100});
        tree->setSize({100, 100});
        tree->addItem("More Pie!");
        tree->addItem("More Pie!");
        tree->addItem("More Pie!");
        tree->resizeAndReallign();
    }

    ~MyWidget()
    {

    }

    virtual void updateEvent(UpdateEvent* event)
    {
        auto p = event->painter();

        unsigned char grey[4]   = {127, 127, 127, 0};
        unsigned char black[4]  = {  0,   0,   0, 0};

        p->fillRect({0, 0, width(), height()}, grey);
        p->fillRect({offset(), {10, 10}}, black);
        Widget_ScrollArea::updateEvent(event);
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        if(event->button() == MouseButton::Right())
        {

        }
        else
        {
            update();
        }
    }

    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        if(isMouseGrabber() && event->button() & MouseButton::Left())
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
