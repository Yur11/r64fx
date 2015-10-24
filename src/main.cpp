#include <iostream>
#include <unistd.h>

#include "Program.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "KeyEvent.hpp"
#include "Keyboard.hpp"
#include "MouseEvent.hpp"
#include "ResizeEvent.hpp"
#include "Image.hpp"
#include "Painter.hpp"

#include <GL/gl.h>

using namespace std;
using namespace r64fx;


class ImageWidget : public Widget{

public:
    ImageWidget(Widget* parent = nullptr)
    : Widget(parent)
    {}
};


class MyProgram : public Program{
    Widget* m_Widget = nullptr;
    Painter* m_painter = nullptr;
    Point<int> m_point = {10, 10};
    
public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
        m_Widget = new Widget;
        m_Widget->resize(200, 200);
        m_Widget->show();
        m_Widget->setWindowTitle( "hello!" );
        m_painter = Painter::createNew(m_Widget->parentWindow());

        repaint();
    }


    void repaint()
    {
//         glClearColor(0.0, 1.0, 0.0, 1.0);
//         glClear(GL_COLOR_BUFFER_BIT);

//         m_painter->setRect(0, 0, m_Widget->width(), m_Widget->height());
//         m_painter->fillRect(1.0f, 0.0f, 0.0f);
        m_painter->setRect(m_point.x, m_point.y, 10, 10);
        m_painter->fillRect(0.0f, 0.0f, 1.0f);
        m_painter->repaint();
    }


    virtual void mousePressEvent(Window* window, MouseEvent* event)
    {
        cout << "press:   " << event->x() << ", " << event->y() << "\n";
        m_point = event->position();
        repaint();
    }


    virtual void mouseReleaseEvent(Window* window, MouseEvent* event)
    {
        cout << "release: " << event->x() << ", " << event->y() << "\n";
    }

    virtual void mouseMoveEvent(Window* window, MouseEvent* event)
    {
        m_point = event->position();
        repaint();
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
    

    virtual void resizeEvent(Window* window, ResizeEvent* event)
    {
        Program::resizeEvent(window, event);
        repaint();
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
