#include "Painter.hpp"
#include "Window.hpp"
#include "Image.hpp"

#include <iostream>
#include <vector>

using namespace std;

namespace r64fx{


namespace{

struct PainterBase : public Painter{
    int       current_depth = 0;
    Rect<int> current_rect;
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

    virtual void setRect(Rect<int> rect)
    {
        current_rect = intersection(current_clip_rect, rect);
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

    PainterNormal(Window* window)
    {
        this->window = window;
        window->getComponentIndices(&ri, &gi, &bi, &ai);
        clear();
    }

    virtual ~PainterNormal() {}

    virtual void fillRect(float r, float g, float b);

    virtual void repaint();

    virtual void clear();

};//PainterNormal


struct PaintCommand{
    int depth = 0;
    Rect<int> rect;
    virtual ~PaintCommand() {}
    virtual void paint(PainterNormal* impl) = 0;
};


struct PaintCommand_FillRect : public PaintCommand{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    virtual ~PaintCommand_FillRect() {}

    virtual void paint(PainterNormal* p)
    {
        unsigned char px[5];
        px[p->ri] = r * 255;
        px[p->gi] = g * 255;
        px[p->bi] = b * 255;
        px[p->ai] = 0;
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


void PainterNormal::fillRect(float r, float g, float b)
{
    auto pc = new PaintCommand_FillRect;
    pc->depth = current_depth;
    pc->rect = current_rect;
    pc->r = r;
    pc->g = g;
    pc->b = b;

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