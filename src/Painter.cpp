#include "Painter.hpp"
#include "Window.hpp"
#include "Image.hpp"
#include "ImageUtils.hpp"
#include "LinkedList.hpp"
#include <cstring>

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>
#endif//R64FX_DEBUG

using namespace std;

namespace{

constexpr float uchar2float_rcp = 1.0f / 255.0f;    

}//namespace

#include "PainterImage.cxx"

#ifdef R64FX_USE_GL
#include "PainterGL.cxx"
#endif//R64FX_USE_GL


namespace r64fx{

Painter* Painter::newInstance(Window* window)
{
    if(window->type() == Window::Type::Image)
    {
        return new PainterImplImage(window);
    }
#ifdef R64FX_USE_GL
    else if(window->type() == Window::Type::GL)
    {
        return new PainterImplGL(window);
    }
#endif//R64FX_USE_GL

    return nullptr;
}


void Painter::deleteInstance(Painter* painter)
{
    delete painter;
}

}//namespace r64fx

