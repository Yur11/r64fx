#include <iostream>
// #include <memory>
// #include <unistd.h>
// #include "Window_SDL2.hpp"
// #include "Image.hpp"
// #include "Point.hpp"
// #include "Transform2D.hpp"
// #include "BilinearCopy.hpp"

#include "Program.hpp"
#include "Window.hpp"
#include "KeyEvent.hpp"
#include "Keyboard.hpp"

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
// std::shared_ptr<Image> get_window_image(Window* window)
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


class MyProgram : public Program{
    Window* m_Window = nullptr;
    
public:
    MyProgram(int argc, char* argv[]) : Program(argc, argv) {}
    
private:
    virtual void setup()
    {
        cout << "My Program!\n";
        m_Window = createWindow();
        m_Window->show();
    }
    
    virtual void keyPressEvent(Window* window, KeyEvent* event)
    {
        if(event->key() == Keyboard::Key::Escape) 
        {
            quit();
        }
    }
    
    virtual void closeEvent(Window* window)
    {
        quit();
    }
    
    virtual void cleanup()
    {
        cout << "Cleanup!\n";
        destroyWindow(m_Window);
    }
};


int main(int argc, char* argv[])
{
    MyProgram prog(argc, argv);
    return prog.exec();
}
