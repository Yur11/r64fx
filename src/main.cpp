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
#include "Widget_Dummy.hpp"
#include "Widget_Text.hpp"

#include "KeyEvent.hpp"

using namespace std;
using namespace r64fx;


class MyWidget : public Widget{
    Image  m_Image;
    float  m_shiftx = 0.0f;
    float  m_shifty = 0.0f;

public:
    MyWidget(Widget* parent = nullptr) : Widget(parent)
    {

    }

    ~MyWidget()
    {
    }

    virtual void reconfigureEvent(ReconfigureEvent* event)
    {
        m_Image.load(width(), height(), 4);
        for(int y=0; y<m_Image.height(); y++)
        {
            for(int x=0; x<m_Image.width(); x++)
            {
                unsigned char px[4] = { 127, 180, 255, 0 };
                m_Image.setPixel(x, y, px);
            }
        }

        unsigned char fg[4] = {0, 0, 0, 0};
        unsigned char bg[4] = {255, 255, 255, 255};

//         Image src(100, 3, 4);
//         fill(&src, {0, 0, 0, 0});
//
//         Transform2D<float> transform;
//         transform.translate(125, 125);
//         transform.rotate(m_shift);
//         transform.translate(0, -1);
//
//         bilinear_copy(&dst, &src, transform, bg, 4);

        Image dst(250, 250, 4);
        fill(&dst, {255, 255, 255, 0});

        draw_line(&dst, {25 + m_shiftx, 125 + m_shifty}, {25 + m_shiftx, 140 + m_shifty}, 3, fg, bg, 4);
        implant(&m_Image, {10, 10}, &dst);

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
        const float step = 1;

        if(event->key() == Keyboard::Key::Up)
        {
            m_shifty -= step;
        }
        else if(event->key() == Keyboard::Key::Down)
        {
            m_shifty += step;
        }
        else if(event->key() == Keyboard::Key::Left)
        {
            m_shiftx -= step;
        }
        else if(event->key() == Keyboard::Key::Right)
        {
            m_shiftx += step;
        }

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
