#include <iostream>
#include <memory>
#include <unistd.h>
#include "Window_SDL2.hpp"
#include "Image.hpp"

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


int main(int argc, char* argv[])
{
    auto img = new Image(640, 480, 4);
    if(!img->isGood())
    {
        cerr << "Image is bad!\n";
        return 1;
    }
    
    float w = img->width();
    float h = img->height();
    float wrcp = 1.0f / w;
    float hrcp = 1.0f / h;
    
    cout << img->r() << ", " << img->g() << ", " << img->b() << ", " << img->a() << "\n";
    
    img->fill(0, 0, 0, 255);
    
    for(int y=0; y<h; y++)
    {
        for(int x=0; x<w; x++)
        {
            auto p = img->at(x, y);
            p[img->r()] = float(x) * wrcp * 255.0f;
            p[img->g()] = float(y) * hrcp * 255.0f;
            p[img->b()] = 255.0f - float(y) * hrcp * 255.0f;
        }
    }

    auto window = show_image(*img);
    
    sleep(3);
    
    delete img;
    
    return 0;
}
