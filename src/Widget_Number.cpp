#include "Widget_Number.hpp"
#include "InstanceCounter.hpp"
#include "Font.hpp"
#include "Painter.hpp"
#include "TextPainter.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

namespace{

class Widget_Number_Global : public InstanceCounter{
    Font* m_font = nullptr;

    virtual void initEvent()
    {
        m_font = new Font("mono", 14);
    }

    virtual void cleanupEvent()
    {
        delete m_font;
    }

public:
    inline Font* font() const { return m_font; }
} g;

}//namespace


Widget_Number::Widget_Number(Widget* parent)
: Widget(parent)
{
    g.created();
    setSize(find_text_bbox("+0.000000", TextWrap::None, g.font()));
    setMinValue(-1.0f);
}


Widget_Number::~Widget_Number()
{
    g.destroyed();
}


void Widget_Number::addedToWindowEvent(WidgetAddedToWindowEvent* event)
{
    
}


void Widget_Number::removedFromWindowEvent(WidgetRemovedFromWindowEvent* event)
{
    
}


void Widget_Number::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();

    p->fillRect({0, 0, width(), height()}, Color(255, 255, 255));

    char buff[32];
    int n = sprintf(buff, "%+f", value());
    auto img = text2image(std::string(buff), TextWrap::None, g.font());
    p->blendColors({0, 0}, Color(0, 0, 0), img);
    delete img;
}


void Widget_Number::mousePressEvent(MousePressEvent* event)
{
    if(event->doubleClick())
    {
        
    }
    else
    {
        grabMouseFocus();
    }
}


void Widget_Number::mouseReleaseEvent(MouseReleaseEvent* event)
{
    releaseMouseFocus();
}


void Widget_Number::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        int dy = event->dy();
        if(event->dy() != 0)
        {
            setValue(value() + float(-dy) * valueStep(), true);
            repaint();
        }
    }
}

}//namespace r64fx
