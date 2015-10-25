#include "Painter.hpp"
#include "Widget.hpp"
#include "Window.hpp"
#include "Image.hpp"

#include <iostream>

using namespace std;

namespace r64fx{


class PainterNormal : public Painter{
    Window* m_window;
    Rect<int> m_rect;
    int ri, gi, bi, ai;

public:
    PainterNormal(Window* window)
    : m_window(window)
    {
        window->getComponentIndices(&ri, &gi, &bi, &ai);
    }

    virtual void clear(float r, float g, float b)
    {
        auto img = m_window->image();
        unsigned char px[4];
        px[ri] = r * 255;
        px[gi] = g * 255;
        px[bi] = b * 255;
        px[ai] = 0;
        img->fill(px);
    }

    virtual void setRect(Rect<int> rect)
    {
        auto img = m_window->image();
        m_rect = intersection(Rect<int>(
            0, 0, img->width(), img->height()
        ), rect);
    }

    virtual void fillRect(float r, float g, float b)
    {
        auto img = m_window->image();
        for(int y=0; y<m_rect.height(); y++)
        {
            for(int x=0; x<m_rect.width(); x++)
            {
                unsigned char px[4];
                px[ri] = r * 255;
                px[gi] = g * 255;
                px[bi] = b * 255;
                px[ai] = 0;
                img->setPixel(x + m_rect.x(), y + m_rect.y(), px);
            }
        }
    }

    virtual void repaint()
    {
        m_window->repaint();
    }
};


Painter* Painter::createNew(Window* window)
{
    if(window->type() == Window::Type::Normal)
    {
        return new PainterNormal(window);
    }
    return nullptr;
}

}//namespace r64fx