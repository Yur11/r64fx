#ifndef R64FX_RESIZE_CONTEXT_HPP
#define R64FX_RESIZE_CONTEXT_HPP

#include <vector>
#include "Rect.hpp"

namespace r64fx{
    
class Painter;

class ReconfContext{
    friend class Program;
    friend class Widget;
    Painter*                m_painter = nullptr;
    std::vector<Rect<int>>  rects;
    bool                    obtained_rect = false;

    ReconfContext(Painter* painter) : m_painter(painter) {}

    inline void addRect(Rect<int> rect) { rects.push_back(rect); };

public:
    inline Painter* painter() const { return m_painter; }
};

}//namespace r64fx

#endif//R64FX_RESIZE_CONTEXT_HPP