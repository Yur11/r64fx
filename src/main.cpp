#include <iostream>
#include <unistd.h>

#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "KeyEvent.hpp"
#include "Keyboard.hpp"
#include "MouseEvent.hpp"
#include "ReconfigureEvent.hpp"
#include "Image.hpp"
#include "Painter.hpp"

#include <GL/gl.h>

using namespace std;
using namespace r64fx;


class MyWidget : public Widget{
public:
    MyWidget(Widget* parent = nullptr) : Widget(parent)
    {

    }

protected:
    virtual void reconfigureEvent(ReconfigureEvent* event)
    {
        if(event->sizeChanged())
        {
            auto size = event->newSize();
            cout << "MyWidget::reconfigureEvent(" << size.width() << "x" << size.height() << ")\n";
        }

        auto p = event->painter();
        p->clear();
        p->setRect(10, 10, 100, 100);
        p->fillRect(1.0f, 0.0f, 0.0f);
        p->setRect(210, 210, 100, 100);
        p->fillRect(0.0f, 1.0f, 0.0f);
        p->setRect(410, 410, 100, 100);
        p->fillRect(0.0f, 0.0f, 1.0f);
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
