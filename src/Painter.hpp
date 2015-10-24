#ifndef R64FX_PAINTER_HPP
#define R64FX_PAINTER_HPP

#include "Rect.hpp"

namespace r64fx{

class Widget;
class Window;

class Painter{

protected:
    virtual ~Painter() {}

public:
    static Painter* createNew(Window* window);

    virtual void clear(float r, float g, float b) = 0;

    virtual void setRect(Rect<int> rect) = 0;

    inline void setRect(int x, int y, int w, int h)
    {
        setRect(Rect<int>(x, y, w, h));
    }

    virtual void fillRect(float r, float g, float b) = 0;

    virtual void repaint() = 0;
};

}//namespace r64fx

#endif//R64FX_PAINTER_HPP