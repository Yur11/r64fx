#include "Painter.hpp"
#include "Window.hpp"
#include "Image.hpp"
#include "LinkedList.hpp"
#include <vector>

#ifdef R64FX_USE_GL
#include "Shader_rgba.hpp"
#include "Shader_rgba_tex.hpp"
#endif//R64FX_USE_GL

#ifdef R64FX_DEBUG
#include <iostream>
#include <assert.h>
#endif//R64FX_DEBUG

using namespace std;

namespace r64fx{


class PaintContext;

typedef void (*PaintRoutine)(PaintContext* ctx);

/** @brief Collection of values to be passed to paint routines. */
struct PaintContext{
    /** @brief Rectangle given by the user. */
    Rect<int>             orig_rect;

    /** @brief Rectangle relative to the target image. */
    Rect<int>             rect;

    /** @brief Color to use. */
    Color<unsigned char>  color;

    /** @brief Image to put pixels in. */
    Image*                target_image;

    /** @brief Image to read pixels from. */
    Image*                source_image;

    /** @brief Data used to build plots. */
    float*                plot_data;
};


void fill_rect_routine(PaintContext* ctx)
{
    auto img = ctx->target_image;
    auto rect = ctx->rect;
    auto color = ctx->color;

    cout << "fill_rect_routine: " << rect.width() << "x" << rect.height() << "\n";

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            img->setPixel(x + rect.x(), y + rect.y(), color.vec);
        }
    }
}


void put_image_routine(PaintContext* ctx)
{
    auto dst = ctx->target_image;
    auto src = ctx->source_image;
    auto rect = ctx->rect;

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            for(int c=0; c<dst->channelCount(); c++)
            {
                dst->pixel(x + rect.x(), y + rect.y())[c] = src->pixel(x, y)[c];
            }
        }
    }
}


void put_dense_plot_horizontal_routine(PaintContext* ctx)
{
    auto img = ctx->target_image;
    auto rect = ctx->rect;
    auto data = ctx->plot_data;

    float scale = 1.0f / ctx->orig_rect.height();

    for(int x=0; x<rect.width(); x++)
    {
        float min = data[x*2];
        float max = data[x*2 + 1];
        for(int y=0; y<rect.height(); y++)
        {
            float val = y*scale;
            unsigned char px[4];
            if(val > min && val < max)
            {
                px[0] = 0;
                px[1] = 0;
                px[2] = 0;
                px[3] = 0;
            }
            else
            {
                px[0] = 255;
                px[1] = 255;
                px[2] = 255;
                px[3] = 0;
            }

            img->setPixel(x + rect.x(), y + rect.y(), px);
        }
    }
}


void put_dense_plot_vertical_routine(PaintContext* ctx)
{
    auto img = ctx->target_image;
    auto rect = ctx->rect;
    auto data = ctx->plot_data;

    float scale = 1.0f / ctx->orig_rect.width();

    for(int y=0; y<rect.height(); y++)
    {
        float min = data[y*2];
        float max = data[y*2 + 1];
        for(int x=0; x<rect.width(); x++)
        {
            float val = x*scale;
            unsigned char px[4];
            if(val > min && val < max)
            {
                px[0] = 0;
                px[1] = 0;
                px[2] = 0;
                px[3] = 0;
            }
            else
            {
                px[0] = 255;
                px[1] = 255;
                px[2] = 255;
                px[3] = 0;
            }

            img->setPixel(x + rect.x(), y + rect.y(), px);
        }
    }
}


struct PainterImpl : public Painter{
    Window* window = nullptr;

    /** @brief Reusable PaintContext instance; */
    PaintContext* paint_context = nullptr;

    Rect<int> current_clip_rect;

    PainterImpl(Window* window);

    virtual ~PainterImpl();

    virtual void setClipRect(Rect<int> rect);

    /** @brief Clip a rectangle with the current_clip_rect. */
    Rect<int> clip(Rect<int> rect);

};//PainterImpl


PainterImpl::PainterImpl(Window* window) : window(window)
{
    paint_context = new PaintContext;
}


PainterImpl::~PainterImpl()
{
    delete paint_context;
}


void PainterImpl::setClipRect(Rect<int> rect)
{
    current_clip_rect = rect;
}


Rect<int> PainterImpl::clip(Rect<int> rect)
{
    return intersection(
        current_clip_rect,
        rect
    );
}


struct PainterImplNormal : public PainterImpl{
    PainterImplNormal(Window* window);

    virtual ~PainterImplNormal();

    virtual void fillRect(Rect<int> rect, Color<unsigned char> color);

    virtual void putImage(int x, int y, Image* img);

    virtual void putPlot(Rect<int> rect, float* data, int data_size, Orientation orientation = Orientation::Horizontal);

    virtual void finish();

    virtual void reconfigure();
};//PainterImpl


PainterImplNormal::PainterImplNormal(Window* window)
: PainterImpl(window)
{

}


PainterImplNormal::~PainterImplNormal()
{

}


void PainterImplNormal::fillRect(Rect<int> rect, Color<unsigned char> color)
{
    paint_context->rect          = clip(rect);
    paint_context->color         = color;
    fill_rect_routine(paint_context);
}


void PainterImplNormal::putImage(int x, int y, Image* img)
{
    paint_context->source_image  = img;
    paint_context->rect          = clip(Rect<int>(x, y, img->width(), img->height()));
    put_image_routine(paint_context);
}


void PainterImplNormal::putPlot(Rect<int> rect, float* data, int data_size, Orientation orientation)
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


void PainterImplNormal::finish()
{
    window->repaint();
}


void PainterImplNormal::reconfigure()
{
    paint_context->target_image = window->image();
    setClipRect({0, 0, window->image()->width(), window->image()->height()});
}


Painter* Painter::newInstance(Window* window)
{
    if(window->type() == Window::Type::Normal)
    {
        return new PainterImplNormal(window);
    }
#ifdef R64FX_USE_GL
//     else if(window->type() == Window::Type::GL)
//     {
//         return create_gl_painter(window);
//     }
#endif//R64FX_USE_GL

    return nullptr;
}


void Painter::destroyInstance(Painter* painter)
{
    delete painter;
}


}//namespace r64fx