#include <iostream>
#include <memory>
#include <unistd.h>
#include "Window_SDL2.hpp"
#include "Image.hpp"
#include "Point.hpp"
#include "Transform2D.hpp"

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


void bilinear(
    Image &src, Image &dst,
    Transform2D<float> &transform,
    unsigned char fillr,
    unsigned char fillg,
    unsigned char fillb,
    unsigned char filla
)
{
    auto r = src.r();
    auto g = src.g();
    auto b = src.b();
    auto a = src.a();
    int C = min(src.channelCount(), dst.channelCount());

    for(int y=0; y<dst.height(); y++)
    {
        for(int x=0; x<dst.width(); x++)
        {
            auto pdst = dst(x, y);

            Point<float> p(x, y);
            transform(p);

            if(p.x < 0 || p.x >= src.width()-1 || p.y < 0 || p.y >= src.height()-1)
            {
                pdst[r] = fillr;
                pdst[g] = fillg;
                pdst[b] = fillb;
                pdst[a] = filla;
            }
            else
            {
                float x1 = floor(p.x);
                float y1 = floor(p.y);
                float x2 = x1 + 1;
                float y2 = y1 + 1;

                float fracx = x2 - p.x;
                float fracy = y2 - p.y;
                for(int c=0; c<C; c++)
                {
                    float val =
                        float(src(x2, y2)[c]) * (1-fracx) * (1-fracy) +
                        float(src(x1, y2)[c]) * fracx     * (1-fracy) +
                        float(src(x2, y1)[c]) * (1-fracx) * fracy     +
                        float(src(x1, y1)[c]) * fracx     * fracy;
                    dst(x, y)[c] = val;
                }
            }
        }
    }
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
    
    for(int y=10; y<110; y++)
    {
        for(int x=10; x<110; x++)
        {
            auto p = src(x, y);
            p[r] = 0;
            p[g] = 0;
            p[b] = 0;
        }
    }
    
    Transform2D<float> transform;
    transform.translate(-200, -200);
    transform.rotate(M_PI * 0.02);
    transform.translate(50, 50);
    
    bilinear(src, dst, transform, 255, 255, 255, 255);
    
    auto winsrc = show_image(src);
    auto windst = show_image(dst);

    sleep(5);

    return 0;
}
