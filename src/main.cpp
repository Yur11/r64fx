#include <iostream>
#include <unistd.h>

#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "KeyEvent.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "ReconfContext.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "Font.hpp"
#include "Widget_Dummy.hpp"
#include "Widget_Text.hpp"

using namespace std;
using namespace r64fx;


class MyWidget : public Widget{
    Image*  m_Image = nullptr;
    float*  data = nullptr;
    int     data_size = 256;
    Font*   m_Font = nullptr;

public:
    MyWidget(Widget* parent = nullptr) : Widget(parent)
    {
        m_Image = new Image(200, 100, 4);
        {
            unsigned char px[4] = { 255, 255, 255, 255 };
            m_Image->fill(px);
        }
        {
            unsigned char px[4] = { 255, 0, 0, 0 };
            m_Image->setPixel(49, 49, px);
            m_Image->setPixel(50, 49, px);
            m_Image->setPixel(49, 50, px);
            m_Image->setPixel(50, 50, px);
        }
        draw_border(m_Image, {0, 0, 0, 0});

        m_Font = new Font;
        cout << m_Font->glyphCount() << "\n";
        m_Font->setSize(32, 32, 96, 96);
        cout << m_Font->ascender() << ", " << m_Font->descender() << "\n";

        auto glyph = m_Font->fetchGlyph("g");
        alpha_blend(m_Image, {10, 10}, {0, 0, 0, 0}, glyph->image());

        glyph = m_Font->fetchGlyph("A");
        alpha_blend(m_Image, {100, 10}, {0, 0, 0, 0}, glyph->image());

        auto wd1 = new Widget_Dummy({255, 0, 0}, this);
        auto wd2 = new Widget_Dummy({0, 255, 0}, this);
        auto wd3 = new Widget_Dummy({0, 0, 255}, this);
        wd1->setSize({100, 100});
        wd2->setSize({100, 100});
        wd3->setSize({100, 100});
        wd1->setPosition({100, 100});
        wd2->setPosition({200, 200});
        wd3->setPosition({300, 300});

        auto wwd = new Widget_Dummy({230, 240, 210}, this);
        wwd->setPosition({550, 100});
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
        
        if(m_Font)
        {
            delete m_Font;
        }
    }

    virtual void reconfigure(ReconfContext* ctx)
    {
        auto painter = ctx->painter();
        painter->fillRect({255, 255, 255}, rect());
        Widget::reconfigure(ctx);
    }
};


class MyProgram : public Program{
    MyWidget* m_Widget = nullptr;
    Painter* m_painter = nullptr;
    Point<int> m_point = {10, 10};

public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
        m_Widget = new MyWidget;
        m_Widget->setSize({1000, 600});
        m_Widget->show();
    }
    

    virtual void keyPressEvent(Window* window, KeyEvent* event)
    {
        Program::keyPressEvent( window, event );

        cout << Keyboard::Key::toString(event->key()) << "\n";

        if(event->key() == Keyboard::Key::Escape) 
        {
            quit();
        }
        else if(event->key() == Keyboard::Key::J)
        {
            m_Widget->setSize({400, 200});
            m_Widget->setWindowTitle("A");
        }
        else if(event->key() == Keyboard::Key::K)
        {
            m_Widget->setSize({200, 400});
            m_Widget->setWindowTitle("B");
        }
        else if(event->key() == Keyboard::Key::T)
        {
            cout << "Title: " << m_Widget->windowTitle() << "\n";
        }

    }
    

    virtual void closeEvent(Window* window)
    {
        if(window->widget() == m_Widget)
        {
            quit();
        }
    }


    virtual void cleanup()
    {
        cout << "Cleanup!\n";
        delete m_Widget;
    }
};


int main(int argc, char* argv[])
{
    MyProgram prog(argc, argv);
    return prog.exec();
}
