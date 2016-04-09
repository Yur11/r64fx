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
#include "Widget_Text.hpp"
#include "Widget_View.hpp"
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

class MyWidget : public Widget_View{
    Image m_Image;
    ImageAnimation* m_animation = nullptr;
    float m_angle = 0.0f;

public:
    MyWidget(Widget* parent = nullptr) : Widget_View(parent)
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
            Rect<int>(50, 50, 10, 10) + offset(),
            Rect<int>(0, 0, m_Image.width(), m_Image.height())
        );
        if(r.width() > 0 && r.height() > 0)
        {
            fill(&m_Image, fg, r);
        }

        auto painter = event->painter();
        painter->putImage(&m_Image);

        Widget_View::updateEvent(event);
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        update();
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
    Font*   m_Font = nullptr;

public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
        m_Font = new Font("", 20, 72);

        auto mw = new MyWidget();
        mw->setSize({300, 300});
        mw->show();
    }

    virtual void cleanup()
    {
        cout << "Cleanup!\n";

        if(m_Font)
            delete m_Font;
    }
};


int main(int argc, char* argv[])
{
    MyProgram prog(argc, argv);
    return prog.exec();
}
