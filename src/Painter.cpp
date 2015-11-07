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
    int       current_depth = 0;
    Rect<int> current_clip_rect;

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


struct PaintCommand;

struct PainterNormal : public PainterBase{
    Window* window =  nullptr;

    int ri = 4;
    int gi = 4;
    int bi = 4;
    int ai = 4;

    vector<PaintCommand*> commands;

    void insertPaintCommand(PaintCommand* pc);

    PainterNormal(Window* window)
    {
        this->window = window;
        window->getComponentIndices(&ri, &gi, &bi, &ai);
        clear();
    }

    virtual ~PainterNormal() {}

    virtual void fillRect(Rect<int> rect, Color<float> color);

    virtual void putImage(int x, int y, Image* img);

    virtual void putPlot(Rect<int> rect, float* data, int data_size, Orientation orientation);

    virtual void repaint();

    virtual void clear();

};//PainterNormal


struct PaintCommand{
    int depth = 0;
    Rect<int> rect;
    virtual ~PaintCommand() {}
    virtual void paint(PainterNormal* impl) = 0;
};


void PainterNormal::insertPaintCommand(PaintCommand* pc)
{
    if(commands.empty())
    {
        commands.push_back(pc);
    }
    else
    {
        auto it=commands.begin();
        while(it!=commands.end())
        {
            auto ppc = *it;
            if(ppc->depth > pc->depth)
            {
                it--;
                commands.insert(it, pc);
                break;
            };
            it++;
        }

        if(it == commands.end())
        {
            commands.push_back(pc);
        }
    }
}


struct PaintCommand_FillRect : public PaintCommand{
    Color<float> color;

    virtual ~PaintCommand_FillRect() {}

    virtual void paint(PainterNormal* p)
    {
        unsigned char px[5];
        px[p->ri] = color.red() * 255;
        px[p->gi] = color.green() * 255;
        px[p->bi] = color.blue() * 255;
        px[p->ai] = color.alpha();
        auto img = p->window->image();

        for(int y=0; y<rect.height(); y++)
        {
            for(int x=0; x<rect.width(); x++)
            {
                img->setPixel(x + rect.x(), y + rect.y(), px);
            }
        }
    }
};


void PainterNormal::fillRect(Rect<int> rect, Color<float> color)
{
    auto pc = new PaintCommand_FillRect;
    pc->depth = current_depth;
    pc->rect = intersection(
        current_clip_rect,
        rect
    );
    pc->color = color;
    insertPaintCommand(pc);
}


struct PaintCommand_PutImage : public PaintCommand{
    Image* img;

    virtual ~PaintCommand_PutImage() {}

    virtual void paint(PainterNormal* p)
    {
        auto dst = p->window->image();
        auto src = img;

        for(int y=0; y<rect.height(); y++)
        {
            for(int x=0; x<rect.width(); x++)
            {
                for(int c=0; c<img->channelCount(); c++)
                {
                    dst->pixel(x + rect.x(), y + rect.y())[c] = src->pixel(x, y)[c];
                }
            }
        }
    }
};


void PainterNormal::putImage(int x, int y, Image* img)
{
    auto pc = new PaintCommand_PutImage;
    pc->depth = current_depth;
    pc->rect = intersection(
        current_clip_rect,
        Rect<int>(x, y, img->width(), img->height())
    );
    pc->img = img;
    insertPaintCommand(pc);
}


struct PaintCommand_PutDensePlot : public PaintCommand{
    float* data = nullptr;
    Rect<int> orig_rect;
};


struct PaintCommand_PutDensePlotHorizontal : public PaintCommand_PutDensePlot{

    virtual ~PaintCommand_PutDensePlotHorizontal() {}

    virtual void paint(PainterNormal* p)
    {
        auto img = p->window->image();

        int offset = orig_rect.height() / 2;
        float scale = 2.0f / orig_rect.height();

        for(int x=0; x<rect.width(); x++)
        {
            float min = data[x*2];
            float max = data[x*2 + 1];
            for(int y=0; y<rect.height(); y++)
            {
                float val = (y - offset)*scale;
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
};


struct PaintCommand_PutDensePlotVertical : public PaintCommand_PutDensePlot{

    virtual ~PaintCommand_PutDensePlotVertical() {}

    virtual void paint(PainterNormal* p)
    {
        auto img = p->window->image();

        int offset = orig_rect.width() / 2;
        float scale = 2.0f / orig_rect.width();

        for(int y=0; y<rect.height(); y++)
        {
            float min = data[y*2];
            float max = data[y*2 + 1];
            for(int x=0; x<rect.width(); x++)
            {
                float val = (x - offset)*scale;
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
};


void PainterNormal::putPlot(Rect<int> rect, float* data, int data_size, Orientation orientation)
{
    PaintCommand_PutDensePlot* pc = nullptr;
    if(orientation == Orientation::Vertical)
    {
#ifdef R64FX_DEBUG
        assert(data_size >= rect.height()*2);
#endif//R64FX_DEBUG
        pc = new PaintCommand_PutDensePlotVertical;
    }
    else
    {
#ifdef R64FX_DEBUG
        assert(data_size >= rect.width()*2);
#endif//R64FX_DEBUG
        pc = new PaintCommand_PutDensePlotHorizontal;
    }
    pc->depth = current_depth;
    pc->rect = intersection(
        current_clip_rect,
        rect
    );
    pc->orig_rect = rect;
    pc->data = data;
    insertPaintCommand(pc);
}


void PainterNormal::repaint()
{
    for(auto c : commands)
    {
        c->paint(this);
    }
    window->repaint();
}


void PainterNormal::clear()
{
    current_clip_rect = Rect<int>(0, 0, window->image()->width(), window->image()->height());
    for(auto c : commands)
    {
        delete c;
    }
    commands.clear();
}


#ifdef R64FX_USE_GL
class PainterGL : public PainterBase{

    virtual ~PainterGL() {}
public:
};//PainterGL
#endif//R64FX_USE_GL

}//namespace


Painter* Painter::newInstance(Window* window)
{
    if(window->type() == Window::Type::Normal)
    {
        return new PainterNormal(window);
    }
    return nullptr;
}

}//namespace r64fx