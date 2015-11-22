#include "Painter.hpp"
#include "Window.hpp"
#include "Image.hpp"

#ifdef R64FX_USE_GL
#include "Shader_rgba.hpp"
#endif//R64FX_USE_GL

#include <vector>

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>
#endif//R64FX_DEBUG

using namespace std;

namespace r64fx{

class PaintCommandImpl;

struct PainterImpl : public Painter{
    Window* window =  nullptr;
    Rect<int> current_clip_rect;

    vector<PaintCommandImpl*> paint_commands;

    PainterImpl(Window* window);

    virtual ~PainterImpl();

    virtual void begin() {}

    virtual void end() {}

    virtual void setClipRect(Rect<int> rect);

    virtual void debugDraw();

    virtual void fillRect(Rect<int> rect, Color<float> color);

    virtual void putImage(int x, int y, Image* img);

    virtual void putPlot(Rect<int> rect, float* data, int data_size, Orientation orientation = Orientation::Horizontal);

    virtual void clear();

};//PainterImpl


struct PainterImplNormal : public PainterImpl{
    int ri = 4;
    int gi = 4;
    int bi = 4;
    int ai = 4;

    PainterImplNormal(Window* window);

    virtual ~PainterImplNormal();

    virtual void repaint();

    virtual void prepare();

    virtual void clear();

};//PainterImplNormal


#ifdef R64FX_USE_GL
struct PainterImplGL : public PainterImpl{
    VertexArray_rgba* m_VertexArray_rgba = nullptr;

    PainterImplGL(Window* window);

    virtual ~PainterImplGL();

    virtual void debugDraw();

    virtual void repaint();

    virtual void prepare();

    virtual void clear();

    static void initGLStuffIfNeeded();

    static void cleanupGLStuff();

};//PainterImplGL
#endif//R64FX_USE_GL


struct PaintCommandImpl{
    int index = -1;
    Rect<int> rect;

    virtual ~PaintCommandImpl() {}

    virtual void paint(PainterImplNormal* impl) = 0;

#ifdef R64FX_USE_GL
    virtual void prepareGL(PainterImplGL* impl) = 0;
#endif//R64FX_USE_GL

};//PaintCommandImpl


struct PaintCommandImpl_FillRect : public PaintCommandImpl{
    Color<float> color;

    virtual ~PaintCommandImpl_FillRect() {}

    virtual void paint(PainterImplNormal* p);

#ifdef R64FX_USE_GL
    virtual void prepareGL(PainterImplGL* impl);
#endif//R64FX_USE_GL

};//PaintCommandImpl_FillRect


struct PaintCommandImpl_PutImage : public PaintCommandImpl{
    Image* img;

    virtual ~PaintCommandImpl_PutImage() {}

    virtual void paint(PainterImplNormal* p);

#ifdef R64FX_USE_GL
    virtual void prepareGL(PainterImplGL* impl);
#endif//R64FX_USE_GL

};//PaintCommandImpl_PutImage


struct PaintCommandImpl_PutDensePlot : public PaintCommandImpl{
    float* data = nullptr;
    Rect<int> orig_rect;

};//PaintCommandImpl_PutDensePlot


struct PaintCommandImpl_PutDensePlotHorizontal : public PaintCommandImpl_PutDensePlot{

    virtual ~PaintCommandImpl_PutDensePlotHorizontal() {}

    virtual void paint(PainterImplNormal* p);

#ifdef R64FX_USE_GL
    virtual void prepareGL(PainterImplGL* impl);
#endif//R64FX_USE_GL

};//PaintCommandImpl_PutDensePlotHorizontal


struct PaintCommandImpl_PutDensePlotVertical : public PaintCommandImpl_PutDensePlot{

    virtual ~PaintCommandImpl_PutDensePlotVertical() {}

    virtual void paint(PainterImplNormal* p);

#ifdef R64FX_USE_GL
    virtual void prepareGL(PainterImplGL* impl);
#endif//R64FX_USE_GL

};//PaintCommandImpl_PutDensePlotVertical


Painter* Painter::newInstance(Window* window)
{
    if(window->type() == Window::Type::Normal)
    {
        return new PainterImplNormal(window);
    }
#ifdef R64FX_USE_GL
    else if(window->type() == Window::Type::GL)
    {
        return new PainterImplGL(window);
    }
#endif//R64FX_USE_GL

    return nullptr;
}


void Painter::destroyInstance(Painter* painter)
{
    delete painter;
}


PainterImpl::PainterImpl(Window* window) : window(window)
{

}


PainterImpl::~PainterImpl()
{

}


void PainterImpl::setClipRect(Rect<int> rect)
{
    current_clip_rect = rect;
}


void PainterImpl::debugDraw()
{

}


void PainterImpl::fillRect(Rect<int> rect, Color<float> color)
{
    auto pc = new PaintCommandImpl_FillRect;
    pc->rect = intersection(
        current_clip_rect,
        rect
    );
    pc->color = color;
    paint_commands.push_back(pc);
}


void PainterImpl::putImage(int x, int y, Image* img)
{
//     auto pc = new PaintCommandImpl_PutImage;
//     pc->rect = intersection(
//         current_clip_rect,
//         Rect<int>(x, y, img->width(), img->height())
//     );
//     pc->img = img;
//     insertPaintCommandImpl(pc);
}


void PainterImpl::putPlot(Rect<int> rect, float* data, int data_size, Orientation orientation)
{
//     PaintCommandImpl_PutDensePlot* pc = nullptr;
//     if(orientation == Orientation::Vertical)
//     {
// #ifdef R64FX_DEBUG
//         assert(data_size >= rect.height()*2);
// #endif//R64FX_DEBUG
//         pc = new PaintCommandImpl_PutDensePlotVertical;
//     }
//     else
//     {
// #ifdef R64FX_DEBUG
//         assert(data_size >= rect.width()*2);
// #endif//R64FX_DEBUG
//         pc = new PaintCommandImpl_PutDensePlotHorizontal;
//     }
//     pc->rect = intersection(
//         current_clip_rect,
//         rect
//     );
//     pc->orig_rect = rect;
//     pc->data = data;
//     insertPaintCommandImpl(pc);
}


void PainterImpl::clear()
{
    current_clip_rect = Rect<int>(0, 0, window->image()->width(), window->image()->height());
    for(auto pc : paint_commands)
    {
        delete pc;
    }
    paint_commands.clear();
}


#include "PainterImplNormal.cxx"
#ifdef R64FX_USE_GL
#include "PainterImplGL.cxx"
#endif//R64FX_USE_GL

}//namespace r64fx