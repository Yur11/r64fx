#ifndef R64FX_LAYOUT_UTILS_HPP
#define R64FX_LAYOUT_UTILS_HPP

#include "Widget.hpp"

namespace r64fx{

Size<int> align_vertically(
    WidgetIterator begin, WidgetIterator end, Point<int> offset, int spacing
);


Size<int> align_horizontally(
    WidgetIterator begin, WidgetIterator end, Point<int> offset, int spacing
);

}//namespace r64fx

#endif//R64FX_LAYOUT_UTILS_HPP