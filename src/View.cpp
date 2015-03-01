#include "View.hpp"
#include <limits>

using namespace std;

namespace r64fx{
    
View::View(Widget* parent)
: Widget(parent)
{
    dropScrollLimits();
}


void View::setOffset(Point<float> offset)
{
    _offset = offset;
    scroll_limits.fit(_offset);
}


void View::setScrollLimits(Rect<float> limits)
{
    scroll_limits = limits;
    scroll_limits.fit(_offset);
}


void View::dropScrollLimits()
{
    dropLeftScrollLimit();
    dropTopScrollLimit();
    dropRightScrollLimit();
    dropBottomScrollLimit();
}


void View::dropLeftScrollLimit()
{
    scroll_limits.left = -numeric_limits<float>::max();
}


void View::dropTopScrollLimit()
{
    scroll_limits.top = -numeric_limits<float>::max();
}


void View::dropRightScrollLimit()
{
    scroll_limits.right = numeric_limits<float>::max();
}


void View::dropBottomScrollLimit()
{
    scroll_limits.bottom = numeric_limits<float>::max();
}


void View::projectToRootAndClipVisible(Point<float> parent_position, Rect<float> parent_visible_rect)
{
    
    Widget::projectToRootAndClipVisible(parent_position + offset(), parent_visible_rect);
}

}//namespace r64fx