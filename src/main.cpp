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
#include "Widget_Control.hpp"
#include "Widget_Dummy.hpp"
#include "Widget_Text.hpp"

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
        unsigned char black[4]  = { 0, 0, 0, 0 };
//         unsigned char red[4]    = { 255, 0, 0, 0 };
        unsigned char orange[4] = { 242, 96, 1 };

        m_Image.load(width(), height(), 4);
        ImagePainter imp(&m_Image, fg, bg);
        imp.fillBackground();
        imp.fillForeground({10, 10, 10, 10});
        float angle = normalize_angle((float(m_val) / 512) * 1.5f * M_PI + 0.75f * M_PI);
        imp.setForegroundComponents(orange);
        imp.drawArc({100, 100}, m_radius, M_PI * 0.75f, angle, m_thickness);
        imp.setForegroundComponents(black);
        imp.drawArc({100, 100}, m_radius, angle, M_PI * 0.25f, m_thickness);
        imp.setForegroundComponents(m_val > 0 ? orange : black);
        imp.drawRadius(
            {100, 100}, angle, m_radius + 1, 0, m_thickness + 1
        );
        imp.setForegroundComponents(black);

        auto painter = event->painter();
        painter->putImage(&m_Image);
        Widget::reconfigureEvent(event);
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        if(event->button() == MouseButton::Left())
        {
            m_mouse_pos = event->position();
        }
        Widget::mousePressEvent(event);
    }

    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        if(event->button() & MouseButton::Left())
        {
            float dy = m_mouse_pos.y() - event->y();
            if(dy != 0)
            {
                if(m_val >= 0 && m_val <= 511)
                {
                    m_val += dy;
                    if(m_val < 0)
                    {
                        m_val = 0;
                    }
                    else if(m_val > 511)
                    {
                        m_val = 511;
                    }
                    update();
                }
            }
            m_mouse_pos = event->position();
        }
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

        auto subcontainer = new Widget_Container(m_container);
        auto wc1 = new Widget_Control(ControlType::Knob, {64, 64}, subcontainer);
        auto wc2 = new Widget_Control(ControlType::Knob, {64, 64}, subcontainer);
        auto wc3 = new Widget_Control(ControlType::Knob, {64, 64}, subcontainer);
        auto wc4 = new Widget_Control(ControlType::Knob, {64, 64}, subcontainer);
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
