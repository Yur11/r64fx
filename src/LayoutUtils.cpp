#include "LayoutUtils.hpp"

namespace r64fx{

Size<int> align_vertically(WidgetIterator begin, WidgetIterator end, Point<int> offset, int spacing)
{
    int max_width = 0;
    int running_height = offset.y();

    for(auto w=begin; w!=end; w++)
    {
        w->setPosition({offset.x(), running_height});
        if(w->width() > max_width)
            max_width = w->width();
        running_height += w->height() + spacing;
    }

    return {
        max_width, running_height - spacing - offset.y()
    };
}


Size<int> align_horizontally(WidgetIterator begin, WidgetIterator end, Point<int> offset, int spacing)
{
    int max_height = 0;
    int running_width = offset.x();

    for(auto w=begin; w!=end; w++)
    {
        w->setPosition({running_width, offset.y()});
        if(w->height() > max_height)
            max_height = w->height();
        running_width += w->width() + spacing;
    }

    return {
        running_width - spacing - offset.x(), max_height
    };
}

}//namespace r64x