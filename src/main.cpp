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

using namespace std;
using namespace r64fx;


const char* lorem_ipsum =
"Lorem ipsum dolor    sit amet, consectetur adipiscing elit.\n"
"Donec a diam lectus. Sed sit amet ipsum mauris.\n"
"Maecenas congue ligula ac quam viverra nec consectetur ante hendrerit.\n"
"Donec et mollis dolor. Praesent et diam eget libero egestas mattis sit amet vitae augue.\n\n\n"
"Nam tincidunt congue enim, ut porta lorem lacinia consectetur.\n"
"Donec ut libero sed arcu vehicula ultricies a non tortor.\n"
"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean ut gravida lorem.\n\n"
"Ut turpis felis, pulvinar a semper sed, adipiscing id dolor.\n"
"Pellentesque auctor nisi id magna consequat sagittis.\n"
;


class MyWidget : public Widget{
    Image*  m_Image = nullptr;
    float*  data = nullptr;
    int     data_size = 256;

public:
    MyWidget(Widget* parent = nullptr) : Widget(parent)
    {
        m_Image = new Image(200, 100, 4);
        {
            fill(m_Image, { 255, 255, 255, 255 });
        }
        {
            unsigned char px[4] = { 255, 0, 0, 0 };
            m_Image->setPixel(49, 49, px);
            m_Image->setPixel(50, 49, px);
            m_Image->setPixel(49, 50, px);
            m_Image->setPixel(50, 50, px);
        }
        draw_border(m_Image, {0, 0, 0, 0});

        auto wwd = new Widget_Dummy({230, 240, 210}, this);
        wwd->setPosition({600, 100});
        wwd->setSize({300, 300});

        auto wwd1 = new Widget_Dummy({255, 0, 255}, wwd);
        wwd1->setPosition({50, 50});
        wwd1->setSize({100, 100});

        auto wwd2 = new Widget_Dummy({0, 255, 255}, wwd);
        wwd2->setPosition({150, 150});
        wwd2->setSize({100, 100});

        auto wwd3 = new Widget_Dummy({255, 0, 0}, wwd);
        wwd3->setPosition({170, 30});
        wwd3->setSize({80, 80});

        auto wwwd1 = new Widget_Dummy({0, 120, 0}, wwd1);
        wwwd1->setPosition({10, 10});
        wwwd1->setSize({20, 20});

        auto wwwd2 = new Widget_Dummy({0, 120, 0}, wwd1);
        wwwd2->setPosition({30, 30});
        wwwd2->setSize({20, 20});

        auto wwwd3 = new Widget_Dummy({0, 120, 0}, wwd2);
        wwwd3->setPosition({10, 10});
        wwwd3->setSize({20, 20});
    }

    ~MyWidget()
    {
        if(m_Image)
        {
            delete m_Image;
        }

        if(data)
        {
            delete[] data;
        }
    }

    virtual void reconfigureEvent(ReconfigureEvent* event)
    {
        auto painter = event->painter();
        painter->fillRect({255, 255, 255}, rect());
        Widget::reconfigureEvent(event);
    }

    virtual void mousePressEvent(MousePressEvent* event)
    {
        Widget::mousePressEvent(event);
    }

//     virtual void keyPressEvent(KeyPressEvent* event)
//     {
//     }

//     virtual void textInputEvent(TextInputEvent* event)
//     {
//     }

    virtual void closeEvent()
    {
        Program::quit();
    }
};


class MyProgram : public Program{
//     MyWidget* m_Widget = nullptr;
//     Painter* m_painter = nullptr;
//     Point<int> m_point = {10, 10};
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
        for(int i=0; i<2; i++)
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
