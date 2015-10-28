#ifndef R64FX_PAINTER_HPP
#define R64FX_PAINTER_HPP

#include "Rect.hpp"

namespace r64fx{

class Window;

class Painter{
    void* m_private = nullptr;

protected:
    virtual ~Painter() {};

public:
    static Painter* newInstance(Window* window);

    virtual void begin() = 0;

    virtual void end() = 0;

    virtual void setClipRect(Rect<int> rect) = 0;

    inline void setClipRect(int x, int y, int w, int h)
    {
        setClipRect(Rect<int>(x, y, w, h));
    }

    virtual void setRect(Rect<int> rect) = 0;

    inline void setRect(int x, int y, int w, int h)
    {
        setRect(Rect<int>(x, y, w, h));
    }

    virtual void fillRect(float r, float g, float b) = 0;

    virtual void repaint() = 0;

    virtual void clear() = 0;
};

}//namespace r64fx

#endif//R64FX_PAINTER_HPP