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

public:
    PainterNormal(Window* window)
    : m_window(window)
    {}

    virtual void clear(float r, float g, float b)
    {
        auto img = m_window->image();
        unsigned char px[4];
        px[0] = r * 255;
        px[1] = g * 255;
        px[2] = b * 255;
        px[3] = 0;
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
                px[0] = r * 255;
                px[1] = g * 255;
                px[2] = b * 255;
                px[3] = 0;
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