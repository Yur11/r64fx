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

using namespace std;
using namespace r64fx;


// std::shared_ptr<Window> show_image(const Image &image, const char* title = "Image")
// {
// #ifdef USE_SDL2
//     auto window = new Window_SDL2(image.width(), image.height(), title);
//     window->show();
//     SDL_Rect rect = {0, 0, image.width(), image.height()};
//     SDL_LowerBlit(image.sdl_surface(), &rect, window->sdl_surface(), &rect);
//     window->flush();
//     return shared_ptr<Window>(window);
// #endif//USE_SDL2
// }
// 
// 
// std::shared_ptr<Image> get_window_image(Widget* widget)
// {
// #ifdef USE_SDL2
//     auto sdl2window = dynamic_cast<Window_SDL2*>(window);
//     if(sdl2window)
//     {
//         return shared_ptr<Image>(new Image(sdl2window->sdl_surface()));
//     }
// #endif//USE_SDL2
// 
//     return nullptr;
// }
// 
// 
// template<typename T> inline std::ostream &operator<<(std::ostream &ost, const Point<T> &p)
// {
//     ost << "Point(" << p.x << ", " << p.y << ")";
//     return ost;
// }


class ImageWidget : public Widget{

public:
    ImageWidget(Widget* parent = nullptr)
    : Widget(parent)
    {}


};


class MyProgram : public Program{
    
    Widget* m_Widget = nullptr;
    
public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
        cout << "My Program!\n";
        
        m_Widget = new Widget;
        m_Widget->resize(200, 200);
        m_Widget->show();

//         auto ws = m_Widget->windowSurface();
//         cout << ws->width() << "x" << ws->height() << "\n";
//         cout << ws->channelCount() << "\n";
    }

    virtual void mousePressEvent(Window* window, MouseEvent* event)
    {
        cout << "press:   " << event->x() << ", " << event->y() << "\n";
    }

    virtual void mouseReleaseEvent(Window* window, MouseEvent* event)
    {
        cout << "release: " << event->x() << ", " << event->y() << "\n";
    }
    
    virtual void keyPressEvent(Window* window, KeyEvent* event)
    {
        Program::keyPressEvent( window, event );
        cout << "widget: " << window << "\n";

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
        cout << "Resize!\n";
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
