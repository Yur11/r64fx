#include "Painter.hpp"
#include "Window.hpp"
#include "Image.hpp"


#include <vector>

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>
#endif//R64FX_DEBUG

using namespace std;

namespace r64fx{


namespace{

struct PainterBase : public Painter{
    Window* window =  nullptr;

    int       current_depth = 0;
    Rect<int> current_clip_rect;

    PainterBase(Window* window)
    : window(window)
    {
    }

    virtual ~PainterBase() {}

    virtual void begin()
    {
        current_depth++;
    }

    virtual void end()
    {
#ifdef R64FX_DEBUG
        if(current_depth <= 0)
        {
            cerr << "Painter::end() depth is already " << current_depth << " !\n";
        }
        else
#endif//R64FX_DEBUG
        {
            current_depth--;
        }
    }

    virtual void setClipRect(Rect<int> rect)
    {
        current_clip_rect = rect;
    }
};//PainterBase

}//namespace
}//namespace r64fx

#include "PainterNormal.cxx"
#ifdef R64FX_USE_GL
#include "PainterGL.cxx"
#endif//R64FX_USE_GL


namespace r64fx{

Painter* Painter::newInstance(Window* window)
{
    if(window->type() == Window::Type::Normal)
    {
        return new PainterNormal(window);
    }
#ifdef R64FX_USE_GL
    else if(window->type() == Window::Type::GL)
    {
        return new PainterGL(window);
    }
#endif//R64FX_USE_GL

    return nullptr;
}


void Painter::destroyInstance(Painter* painter)
{
    delete painter;
}

}//namespace r64fx