#ifndef R64FX_OFFSET_HPP
#define R64FX_OFFSET_HPP

namespace r64fx{

template<typename T> class OffsetMixin{
    Point<T> m_offset = {0, 0};

public:
    inline void setOffset(Point<T> p) { m_offset = p; }

    inline Point<T> offset() const { return m_offset; }

    inline void setOffsetX(T x) { m_offset.setX(x); }
    inline void setOffsetY(T y) { m_offset.setY(y); }

    inline T offsetX() const { return m_offset.x(); }
    inline T offsetY() const { return m_offset.y(); }
};

}//namespace r64fx

#endif//R64FX_OFFSET_HPP