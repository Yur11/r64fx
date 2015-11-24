#ifndef R64FX_PAINTER_HPP
#define R64FX_PAINTER_HPP

#include "Color.hpp"
#include "Rect.hpp"
#include "Orientation.hpp"

namespace r64fx{

class Image;
class Window;

class Painter{
    void* m_private = nullptr;

protected:
    virtual ~Painter() {};

public:
    static Painter* newInstance(Window* window);

    static void destroyInstance(Painter* painter);

    virtual void begin() = 0;

    virtual void end() = 0;

    virtual void setClipRect(Rect<int> rect) = 0;

    virtual void fillRect(Rect<int> rect, Color<unsigned char> color) = 0;

    virtual void putImage(int x, int y, Image* img) = 0;

    virtual void putPlot(Rect<int> rect, float* data, int data_size, Orientation orientation = Orientation::Horizontal) = 0;

    virtual void configure() = 0;

    virtual void repaint() = 0;

    virtual void clear() = 0;
};

}//namespace r64fx

#endif//R64FX_PAINTER_HPP