#include <iostream>
#include <unistd.h>

#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "KeyEvent.hpp"
#include "Keyboard.hpp"
#include "MouseEvent.hpp"
#include "ReconfigureEvent.hpp"
#include "ImageUtils.hpp"
#include "Painter.hpp"
#include "Font.hpp"

using namespace std;
using namespace r64fx;


class MyWidget : public Widget{
    Image* m_Image = nullptr;
    float* data = nullptr;
    int data_size = 256;
    Font* m_Font = nullptr;

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
        m_Font->setSize(72, 72, 96, 96);
        cout << m_Font->ascender() << ", " << m_Font->descender() << "\n";
        auto glyph = m_Font->fetchGlyph("g");
        alpha_blend(m_Image, {10, 10}, {0, 0, 0, 0}, glyph->image());
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

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event)
    {
//         if(event->sizeChanged())
//         {
//             auto size = event->newSize();
// //             cout << "MyWidget::reconfigureEvent(" << size.width() << "x" << size.height() << ")\n";
//         }

        auto size = event->newSize();

        auto p = event->painter();
        p->reconfigure();
        p->fillRect({0, 0, size.width(), size.height()}, {255, 255, 255});
        p->fillRect({10,   10, 100, 100}, {255,    0,   0});
        p->fillRect({210, 210, 100, 100}, {0,    255,   0});
        p->fillRect({410, 410, 100, 100}, {0,      0, 255});
        p->fillRect({550, 310, 100, 100}, {0,    255, 255});
        p->fillRect({150, 330, 100, 100}, {255,  255,   0});
        p->putImage(130, 150, m_Image);
        
//         p->putImage(250, 50, m_Image);
//         p->putImage(180, 250, m_Image);
//         p->putPlot(Rect<int>(100, 100, 100, data_size/2), data, data_size, Orientation::Vertical);
//         p->putPlot(Rect<int>(250, 200, data_size/2, 100), data, data_size, Orientation::Horizontal);
        p->repaint();
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
        m_Widget->resize(800, 600);
        m_Widget->show();
    }

//     virtual void mousePressEvent(Window* window, MouseEvent* event)
//     {
//     }
//
//     virtual void mouseReleaseEvent(Window* window, MouseEvent* event)
//     {
//     }
//
//     virtual void mouseMoveEvent(Window* window, MouseEvent* event)
//     {
//     }
    

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
            m_Widget->resize(400, 200);
            m_Widget->setWindowTitle("A");
        }
        else if(event->key() == Keyboard::Key::K)
        {
            m_Widget->resize(200, 400);
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
