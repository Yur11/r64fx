#ifndef R64FX_RESIZE_EVENT_HPP
#define R64FX_RESIZE_EVENT_HPP

#include "Size.hpp"

namespace r64fx{
    
class ResizeEvent{
    Size<int> m_old_size;
    Size<int> m_new_size;
    
public:
    ResizeEvent(Size<int> old_size, Size<int> new_size)
    : m_old_size(old_size)
    , m_new_size(new_size)
    {}
    
    inline Size<int> oldSize() const { return m_old_size; }
    
    inline Size<int> newSize() const { return m_new_size; }
};
    
}//namespace r64fx

#endif//R64FX_RESIZE_EVENT_HPP