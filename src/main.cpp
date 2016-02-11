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
    int  m_shiftx = 0;
    int  m_shifty = 1;

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

        unsigned char fg = 255;
        unsigned char bg = 0;

        Image dst(250, 250, 1);
        fill(&dst, 0);

        vector<Point<float>> points;
        for(int i=0; i<50; i++)
        {
            float x = (float(i) / 50.f) * M_PI * 2 + (M_PI * 0.1 * m_shiftx);
            float y = sin(x) * 100 - 50;

            points.push_back({float(i*4 + 10), -y + 60});
        }
        draw_lines(
            &dst, points.data(), points.size(), m_shifty, LineCapStyle::Round,
            &fg, &bg, 1
        );

        alpha_blend(&m_Image, {10, 10}, {0, 0, 0, 0}, &dst);

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
        const int step = 1;

        if(event->key() == Keyboard::Key::Up)
        {
            m_shifty += step;
            cout << m_shifty << "\n";
        }
        else if(event->key() == Keyboard::Key::Down)
        {
            if(m_shifty > 1)
            {
                m_shifty -= step;
                cout << m_shifty << "\n";
            }
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
