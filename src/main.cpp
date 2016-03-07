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


class MyWidget : public Widget_View{
    Image  m_Image;

public:
    MyWidget(Widget* parent = nullptr) : Widget_View(parent)
    {
        auto wc1 = new Widget_Control(ControlType::UnipolarRadius, {50, 50}, this);
        wc1->setPosition({100, 100});
        auto wc2 = new Widget_Control(ControlType::BipolarRadius,  {50, 50}, this);
        wc2->setPosition({160, 100});
    }

    ~MyWidget()
    {
    }

    virtual void reconfigureEvent(ReconfigureEvent* event)
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
        Widget_View::reconfigureEvent(event);
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        Widget_View::mousePressEvent(event);
    }

    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        if(event->button() & MouseButton::Left())
        {
            setOffset(offset() + event->delta());
            update();
        }
        else
        {
            Widget_View::mouseMoveEvent(event);
        }
    }

    virtual void keyPressEvent(KeyPressEvent* event)
    {
        int step = 1;
        if(event->key() == Keyboard::Key::Up)
        {
            setOffsetY(offsetY() - step);
        }
        else if(event->key() == Keyboard::Key::Down)
        {
            setOffsetY(offsetY() + step);
        }
        else if(event->key() == Keyboard::Key::Left)
        {
            setOffsetX(offsetX() - step);
        }
        else if(event->key() == Keyboard::Key::Right)
        {
            setOffsetX(offsetX() + step);
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
    Widget_Container* m_container = nullptr;

public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
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

        auto wt = new Widget_Text("", m_Font, m_container);
        wt->setWidth(300);
        wt->setHeight(300);
        wt->setPadding(5);
        wt->setTextWrap(TextWrap::Anywhere);
        wt->setTextAlignment(TextAlignment::Left);

        m_container->setPadding(20);
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
