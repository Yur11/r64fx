#include "Widget_Slider.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"
#include "WidgetFlags.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

class SliderHandle{
    Image m_image;
    Orientation m_orientation;
    int m_user_cout = 0;

public:
    SliderHandle(int size, Orientation orientation)
    : m_orientation(orientation)
    {
#ifdef R64FX_DEBUG
        assert(size > 10);
#endif//R64FX_DEBUG

        int lateral = int(size * 0.7f) | 1;
        m_image.load(
            orientation == Orientation::Vertical   ? size : lateral,
            orientation == Orientation::Horizontal ? size : lateral,
            1
        );

        fill(&m_image, Color(63));

        int middle = (lateral >> 1);

        if(orientation == Orientation::Vertical)
        {
            for(int x=0; x<m_image.width(); x++)
            {
                m_image(x, 0)[0] = 95;
                m_image(x, m_image.height() - 1)[0] = 15;
                m_image(x, middle               )[0] = 255;
                m_image(x, middle - 1           )[0] =
                m_image(x, middle + 1           )[0] = 191;
            }
        }
        else
        {
            for(int y=0; y<m_image.height(); y++)
            {
                m_image(0, y)[0] =
                m_image(m_image.width() - 1,  y)[0] = 111;
                m_image(middle,               y)[0] = 255;
                m_image(middle - 1,           y)[0] =
                m_image(middle + 1,           y)[0] = 191;
            }
        }
    }

    bool operator==(const SliderHandle &other)
    {
        return
            m_image.isGood() == other.m_image.isGood() &&
            m_orientation == other.m_orientation &&
            m_image.width() == other.m_image.width() &&
            m_image.height() == other.m_image.height()
        ;
    }

    inline Image* handleImage() { return &m_image; }

    inline int thickness() const { return m_orientation == Orientation::Vertical ? m_image.height() : m_image.width(); }

    inline int width() const { return m_orientation == Orientation::Vertical ? m_image.width() : m_image.height(); }
};


Widget_Slider::Widget_Slider(int length, int width, Orientation orientation, Widget* parent)
: Widget(parent)
{
    setOrientation(orientation);
    if(orientation == Orientation::Vertical)
    {
        setWidth(width);
        setHeight(length);
    }
    else
    {
        setWidth(length);
        setHeight(width);
    }

    m_handle = new SliderHandle(width, orientation);

    onValueChanged(nullptr);
}


Widget_Slider::~Widget_Slider()
{
    delete m_handle;
}


bool Widget_Slider::isReversed(bool yes)
{
    if(yes)
        m_flags |= R64FX_WIDGET_IS_REVERSED;
    else
        m_flags &= ~R64FX_WIDGET_IS_REVERSED;
    return yes;
}


bool Widget_Slider::isReversed() const
{
    return m_flags & R64FX_WIDGET_IS_REVERSED;
}


void Widget_Slider::setValueFromPosition(Point<int> position)
{
    int pos;
    if(orientation() == Orientation::Vertical)
    {
        pos = travelDistance() - (position.y() - m_handle->thickness()/2);
    }
    else
    {
        pos = position.x() - m_handle->thickness()/2;
    }

    float new_value = (float(pos)/float(travelDistance())) * valueRange() + minValue();
    setValue(new_value);
    repaint();
}


int Widget_Slider::travelDistance() const
{
    return (orientation() == Orientation::Vertical ? height() : width()) - m_handle->thickness();
}


void Widget_Slider::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, Color(191, 191, 191));

    int pos = ((value() - minValue()) / valueRange()) * travelDistance();

    if(orientation() == Orientation::Vertical)
    {
        pos = travelDistance() - pos;

        p->strokeRect({(width()>>2), 0,       (width()>>1), pos + 1             },  Color(63, 63, 63),   Color(95, 95, 95));
        p->strokeRect({(width()>>2), pos + 2, (width()>>1), height() - (pos + 2)},  Color(63, 96, 127),  Color(127, 191, 255));

        p->putImage(m_handle->handleImage(), {0, pos});
    }
    else
    {
        p->strokeRect({0,       (height()>>2), pos + 1,             (height()>>1)},  Color(63, 63, 63),   Color(95, 95, 95));
        p->strokeRect({pos + 2, (height()>>2), width() - (pos + 2), (height()>>1)},  Color(63, 96, 127),  Color(127, 191, 255));

        p->putImage(m_handle->handleImage(), {pos, 0});
    }
}


void Widget_Slider::mousePressEvent(MousePressEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        grabMouseFocus();
        setValueFromPosition(event->position());
    }
}


void Widget_Slider::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        if(isMouseFocusOwner())
        {
            releaseMouseFocus();
        }
    }
}


void Widget_Slider::mouseMoveEvent(MouseMoveEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        setValueFromPosition(event->position());
    }
}

}//namespace r64fx
