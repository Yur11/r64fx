#include <iostream>
#include <unistd.h>

#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "ImageUtils.hpp"
#include "ImagePainter.hpp"
#include "Painter.hpp"
#include "Font.hpp"
#include "Widget_Container.hpp"
#include "Widget_Dummy.hpp"
#include "Widget_Text.hpp"

#include "KeyEvent.hpp"

using namespace std;
using namespace r64fx;


class MyWidget : public Widget{
    Image  m_Image;
    float  m_radius = 32;
    float  m_thickness = 3;

public:
    MyWidget(Widget* parent = nullptr) : Widget(parent)
    {

    }

    ~MyWidget()
    {
    }

    virtual void reconfigureEvent(ReconfigureEvent* event)
    {
        unsigned char fg[4] = { 0,   0,   0, 0 };
        unsigned char bg[4] = { 127, 180, 255, 0 };
        unsigned char black[4] = { 0, 0, 0, 0 };

        m_Image.load(width(), height(), 4);
        ImagePainter imp(&m_Image, fg, bg);
        imp.fillBackground();
        imp.fillForeground({10, 10, 10, 10});
        imp.drawArc({100, 100}, m_radius, M_PI * 0.75f, M_PI * 0.25f, m_thickness);
//         imp.drawLine({10, 10}, {110, 110}, 5);

        auto painter = event->painter();
        painter->putImage(&m_Image);
        Widget::reconfigureEvent(event);
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        Widget::mousePressEvent(event);
    }

    virtual void keyPressEvent(KeyPressEvent* event)
    {
        if(event->key() == Keyboard::Key::Up)
        {
            m_radius+= 0.5f;
        }
        else if(event->key() == Keyboard::Key::Down)
        {
            if(m_radius > 1.0f)
            {
                m_radius -= 0.5f;
            }
        }
        else if(event->key() == Keyboard::Key::Left)
        {
            if(m_thickness > 0.5f)
            {
                m_thickness -= 0.5f;
            }
        }
        else if(event->key() == Keyboard::Key::Right)
        {
            m_thickness += 0.5f;
        }

        cout << m_radius << ", " << m_thickness << "\n";

        update();
    }

    virtual void closeEvent()
    {
        Program::quit();
    }
};


class MyProgram : public Program{
    Font*   m_Font = nullptr;
    Widget_Container* m_container = nullptr;

public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
//         m_Widget = new MyWidget;
//         m_Widget->setSize({1000, 600});
//         m_Widget->show();

        m_Font = new Font("", 20, 72);

        m_container = new Widget_Container;
        {
            auto mw = new MyWidget(m_container);
            mw->setWidth(300);
            mw->setHeight(300);
        }

        for(int i=0; i<1; i++)
        {
            auto wt = new Widget_Text("", m_Font, m_container);
            wt->setWidth(300);
            wt->setHeight(300);
            wt->setPadding(5);
            wt->setTextWrap(TextWrap::Anywhere);
            wt->setTextAlignment(TextAlignment::Left);

        }
        m_container->setPadding(5);
        m_container->setSpacing(5);
        m_container->alignHorizontally();
        m_container->show();
    }
    
    virtual void cleanup()
    {
        cout << "Cleanup!\n";
        if(m_Font)
            delete m_Font;

        if(m_container)
            delete m_container;
    }
};


int main(int argc, char* argv[])
{
    MyProgram prog(argc, argv);
    return prog.exec();
}
