#ifndef R64FX_RESIZE_CONTEXT_HPP
#define R64FX_RESIZE_CONTEXT_HPP

#include <vector>
#include "Rect.hpp"

namespace r64fx{
    
class Painter;

/** @brief Structure passed down the Widget::reconfigure() recursive invocation. */
class ReconfContext{
    friend class Widget;
    Painter*                m_painter       = nullptr;
    Rect<int>               m_visible_rect;
    Rect<int>*              rects           = nullptr;
    int                     num_rects       = 0;
    bool                    got_rect        = false;

    inline void setPainter(Painter* painter) { m_painter = painter; }

    inline void setVisibleRect(const Rect<int> &rect) { m_visible_rect = rect; }

    void addRect(const Rect<int> &rect);

    void clearRects();

public:
    ReconfContext();

    inline Painter* painter() const { return m_painter; }

    inline Rect<int> visibleRect() const { return m_visible_rect; }
};

}//namespace r64fx

#endif//R64FX_RESIZE_CONTEXT_HPP