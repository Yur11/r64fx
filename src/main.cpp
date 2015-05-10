#include <iostream>
#include <memory>
#include <unistd.h>
#include "Window_SDL2.hpp"
#include "Image.hpp"
#include "Point.hpp"
#include "Transform2D.hpp"
#include "BilinearCopy.hpp"

using namespace std;
using namespace r64fx;


std::shared_ptr<Window> show_image(const Image &image, const char* title = "Image")
{
#ifdef USE_SDL2
    auto window = new Window_SDL2(image.width(), image.height(), title);
    window->show();
    SDL_Rect rect = {0, 0, image.width(), image.height()};
    SDL_LowerBlit(image.sdl_surface(), &rect, window->sdl_surface(), &rect);
    window->flush();
    return shared_ptr<Window>(window);
#endif//USE_SDL2
}


std::shared_ptr<Image> get_window_image(Window* window)
{
#ifdef USE_SDL2
    auto sdl2window = dynamic_cast<Window_SDL2*>(window);
    if(sdl2window)
    {
        return shared_ptr<Image>(new Image(sdl2window->sdl_surface()));
    }
#endif//USE_SDL2

    return nullptr;
}


template<typename T> inline std::ostream &operator<<(std::ostream &ost, const Point<T> &p)
{
    ost << "Point(" << p.x << ", " << p.y << ")";
    return ost;
}



int main(int argc, char* argv[])
{
    Image src(400, 400, 4);
    Image dst(400, 400, 4);
    
    src.fill(255, 255, 255, 255);
    dst.fill(255, 255, 255, 255);
    
    auto r = src.r();
    auto g = src.g();
    auto b = src.b();
//     auto a = src.a();
    
    for(int y=300; y<400-1; y++)
    {
        for(int x=0; x<100; x++)
        {
            auto p = src(x, y);
            p[r] = 0;
            p[g] = 0;
            p[b] = 0;
        }
    }
    
    Transform2D<float> transform;
    transform.translate(-200, 200);
    transform.rotate(M_PI * 0.02);
    transform.translate(50, 50);
    
    BilinearCopy copy;
    copy.fillr = 255;
    copy.fillg = 255;
    copy.fillb = 255;
    copy.filla = 255;
    copy(src, dst, transform);
    
    auto winsrc = show_image(src);
    auto windst = show_image(dst);

    sleep(5);

    return 0;
}
