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
#include "Widget_Container.hpp"
#include "Widget_Control.hpp"
#include "Widget_Dummy.hpp"
#include "Widget_Text.hpp"
#include "Widget_View.hpp"

#include "KeyEvent.hpp"

using namespace std;
using namespace r64fx;

float normalize_angle(float angle)
{
    while(angle > (2.0f * M_PI))
        angle -= (2.0f * M_PI);

    while(angle < 0.0f)
        angle += (2.0f * M_PI);

    return angle;
}


class MyWidget : public Widget{
    Image  m_Image;
    float  m_radius = 32.0f;
    float  m_thickness = 3.0f;
    int    m_val = 0;
    Point<int> m_mouse_pos;

public:
    MyWidget(Widget* parent = nullptr) : Widget(parent)
    {

    }

    ~MyWidget()
    {
    }

    virtual void reconfigureEvent(ReconfigureEvent* event)
    {
        unsigned char fg[4]     = { 0,   0,   0, 0 };
        unsigned char bg[4]     = { 127, 180, 255, 0 };

        m_Image.load(width(), height(), 4);
        fill(&m_Image, bg);

        auto painter = event->painter();
        painter->putImage(&m_Image);
        Widget::reconfigureEvent(event);
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        Widget::mousePressEvent(event);
    }

    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        Widget::mouseMoveEvent(event);
    }

    virtual void keyPressEvent(KeyPressEvent* event)
    {
        Widget::keyPressEvent(event);
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

        auto subcontainer = new Widget_Container(m_container);
        auto wc1 = new Widget_Control(ControlType::UnipolarRadius, {50, 50}, subcontainer);
        auto wc2 = new Widget_Control(ControlType::UnipolarRadius, {50, 50}, subcontainer);
        auto wc3 = new Widget_Control(ControlType::UnipolarSector, {50, 50}, subcontainer);
        auto wc4 = new Widget_Control(ControlType::BipolarSector,  {50, 50}, subcontainer);
        auto wc5 = new Widget_Control(ControlType::BipolarRadius,  {50, 50}, subcontainer);
        subcontainer->setSpacing(5);
        subcontainer->alignVertically();

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
