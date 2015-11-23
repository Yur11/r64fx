#include "Painter.hpp"
#include "Window.hpp"
#include "Image.hpp"
#include "LinkedList.hpp"
#include <vector>

#ifdef R64FX_USE_GL
#include "Shader_rgba.hpp"
#endif//R64FX_USE_GL

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>
#endif//R64FX_DEBUG

using namespace std;

namespace r64fx{

class PaintCommand;
class PaintCommandGroup;

struct PainterImpl : public Painter{
    Window* window = nullptr;
    Rect<int> current_clip_rect;

    PaintCommandGroup* root_group = nullptr;
    PaintCommandGroup* curr_group = nullptr;

    PainterImpl(Window* window);

    virtual ~PainterImpl();

    virtual void begin();

    virtual void end();

    virtual void setClipRect(Rect<int> rect);

    virtual void debugDraw();

    virtual void fillRect(Rect<int> rect, Color<float> color);

    virtual void putImage(int x, int y, Image* img);

    virtual void putPlot(Rect<int> rect, float* data, int data_size, Orientation orientation = Orientation::Horizontal);

    virtual void clear();

};//PainterImpl


struct PaintCommand : public LinkedList<PaintCommand>::Node{
    int index = -1;
    Rect<int> rect;

    virtual ~PaintCommand() {}

    virtual void paint(PainterImpl* impl) = 0;
};//PaintCommand


struct PaintCommandGroup : public PaintCommand{
    PaintCommandGroup*         parent = nullptr;
    LinkedList<PaintCommand>   commands;

    virtual ~PaintCommandGroup() {}

    virtual void paint(PainterImpl* impl);

    void clear();

};//PaintCommandGroup


struct PaintCommand_FillRect : public PaintCommand{
    Color<float> color;

    virtual ~PaintCommand_FillRect() {}

    virtual void paint(PainterImpl* p);

};//PaintCommand_FillRect


struct PaintCommand_PutImage : public PaintCommand{
    Image* img;

    virtual ~PaintCommand_PutImage() {}

    virtual void paint(PainterImpl* p);

};//PaintCommand_PutImage


struct PaintCommand_PutDensePlot : public PaintCommand{
    float* data = nullptr;
    Rect<int> orig_rect;

};//PaintCommand_PutDensePlot


struct PaintCommand_PutDensePlotHorizontal : public PaintCommand_PutDensePlot{

    virtual ~PaintCommand_PutDensePlotHorizontal() {}

    virtual void paint(PainterImpl* p);

};//PaintCommand_PutDensePlotHorizontal


struct PaintCommand_PutDensePlotVertical : public PaintCommand_PutDensePlot{

    virtual ~PaintCommand_PutDensePlotVertical() {}

    virtual void paint(PainterImpl* p);

};//PaintCommand_PutDensePlotVertical


PainterImpl* create_normal_painter(Window* window);
#ifdef R64FX_USE_GL
PainterImpl* create_gl_painter(Window* window);
#endif//R64FX_USE_GL

Painter* Painter::newInstance(Window* window)
{
    if(window->type() == Window::Type::Normal)
    {
        return create_normal_painter(window);
    }
#ifdef R64FX_USE_GL
    else if(window->type() == Window::Type::GL)
    {
        return create_gl_painter(window);
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
    curr_group = root_group = new PaintCommandGroup;
}


PainterImpl::~PainterImpl()
{

}


void PainterImpl::begin()
{
    auto group = new PaintCommandGroup;
    group->parent = curr_group;
    curr_group->commands.append(group);
    curr_group = group;
}


void PainterImpl::end()
{
    if(curr_group != root_group)
    {
        curr_group = curr_group->parent;
    }
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
    auto pc = new PaintCommand_FillRect;
    pc->rect = intersection(
        current_clip_rect,
        rect
    );
    pc->color = color;
    curr_group->commands.append(pc);
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
    root_group->clear();
    setClipRect({0, 0, window->width(), window->height()});
}


void PaintCommandGroup::paint(PainterImpl* impl)
{
    for(auto c : commands)
    {
        c->paint(impl);
    }
}


void PaintCommandGroup::clear()
{
    for(;;)
    {
        auto c = commands.first();
        if(c)
        {
            commands.remove(c);
            auto cg = dynamic_cast<PaintCommandGroup*>(c);
            if(cg)
            {
                cg->clear();
            }
            delete c;
        }
        else
        {
            break;
        }
    }
}


#include "PainterImplNormal.cxx"
#ifdef R64FX_USE_GL
#include "PainterImplGL.cxx"
#endif//R64FX_USE_GL

}//namespace r64fx