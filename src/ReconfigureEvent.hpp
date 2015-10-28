#ifndef R64FX_RESIZE_EVENT_HPP
#define R64FX_RESIZE_EVENT_HPP

#include "Point.hpp"
#include "Size.hpp"

namespace r64fx{
    
class Painter;

class ReconfigureEvent{
    Point<int> m_old_position;
    Point<int> m_new_position;
    Size<int>  m_old_size;
    Size<int>  m_new_size;
    Painter*   m_painter;
    
public:
    ReconfigureEvent(
        Point<int> old_position,
        Point<int> new_position,
        Size<int> old_size,
        Size<int> new_size,
        Painter* painter
    )
    : m_old_position(old_position)
    , m_new_position(new_position)
    , m_old_size(old_size)
    , m_new_size(new_size)
    , m_painter(painter)
    {}
    
    inline Point<int> oldPosition() const { return m_old_position; }

    inline Point<int> newPosition() const { return m_new_position; }

    inline bool positionChanged() const { return oldPosition() != newPosition(); }

    inline Size<int> oldSize() const { return m_old_size; }
    
    inline Size<int> newSize() const { return m_new_size; }

    inline bool sizeChanged() const { return oldSize() != newSize(); }

    inline Painter* painter() const { return m_painter; }
};
    
}//namespace r64fx

#endif//R64FX_RESIZE_EVENT_HPP