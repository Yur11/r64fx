#ifndef R64FX_GUI_RECT_H
#define R64FX_GUI_RECT_H

#include "Point.hpp"
#include "Size.hpp"

namespace r64fx{

template<typename T> T min(const T &a, const T &b) { return a < b ? a : b; }
template<typename T> T max(const T &a, const T &b) { return a > b ? a : b; }


template<typename T> class Rect{
    T vec[4];

public:
    Rect(const Rect<T> &other) : vec{other.x(), other.y(), other.width(), other.height()} {}

    Rect(T x, T y, T w, T h) : vec{x, y, w, h} {}

    Rect(Point<T> pos, Size<T> size) : vec{pos.x(), pos.y(), size.width(), size.height()} {}

    Rect() : vec{T(), T(), T(), T()} {}

    inline T  x()       const { return vec[0]; }
    inline T  y()       const { return vec[1]; }
    inline T  width()   const { return vec[2]; }
    inline T  height()  const { return vec[3]; }

    inline void setX(T val)       { vec[0] = val; }
    inline void setY(T val)       { vec[1] = val; }
    inline void setWidth(T val)   { vec[2] = val; }
    inline void setHeight(T val)  { vec[3] = val; }

    inline Point<T> position() const { return {x(), y()}; }
    inline void setPosition(Point<T> pos) { setX(pos.x()); setY(pos.y()); }

    inline T left()   const { return x(); }
    inline T top()    const { return y(); }
    inline T right()  const { return x() + width(); }
    inline T bottom() const { return y() + height(); }

    inline void setLeft(T val)
    {
        T d = x() - val;
        setX(x() - d) ;
        setWidth(width() + d);
    }

    inline void setTop(T val)
    {
        T d = y() - val;
        setY(y() - d);
        setHeight(height() + d);
    }

    inline void setRight(T val) { setWidth(val - x()); }

    inline void setBottom(T val) { setHeight(val - y()); }

    inline Size<T> size() const { return {width(), height()}; }

    inline void setSize(Size<T> size) { setWidth(size.width()); setHeight(size.height()); }

    inline void setSize(T w, T h) { setSize({w, h}); }

    inline Point<T> topLeft()     { return { left(),  top()    }; }
    inline Point<T> bottomLeft()  { return { left(),  bottom() }; }
    inline Point<T> topRight()    { return { right(), top()    }; }
    inline Point<T> bottomRight() { return { right(), bottom() }; }

    inline T halfWidth()  { return width()/2; }
    inline T halfHeight() { return height()/2; }

    inline Point<T> center() { return { x() + halfWidth(), y() + halfHeight() }; }

    inline bool overlaps(Point<int> p) const
    {
        return p.x() >= left() && p.x() < right() && p.y() >= top() && p.y() < bottom();
    }

    inline Rect<T> &operator=(const Rect<T> &other)
    {
        setX(other.x());
        setY(other.y());
        setWidth(other.width());
        setHeight(other.height());
        return *this;
    }

    inline Rect<T> &operator+=(const Point<T> &other)
    {
        setX(x() + other.x());
        setY(y() + other.y());
        return *this;
    }

    inline Rect<T> &operator-=(const Point<T> &other)
    {
        setX(x() - other.x());
        setY(y() - other.y());
        return *this;
    }

    inline T* vec4() const { return vec; }
};


template<typename T> bool operator==(const Rect<T> &a, const Rect<T> &b)
{
    return
        a.width() == b.width() &&
        a.height() == b.height() &&
        a.x() == b.x() && a.y() == b.y()
    ;
}


template<typename T> bool inline operator!=(const Rect<T> &a, const Rect<T> &b)
{
    return !operator==(a, b);
}


template<typename T> Rect<T> operator+(Rect<T> rect, Point<T> offset)
{
    rect += offset;
    return rect;
}


template<typename T> Rect<T> operator-(Rect<T> rect, Point<T> offset)
{
    rect -= offset;
    return rect;
}


template<typename T> Rect<T> intersection(const Rect<T> &a, const Rect<T> &b)
{
    T max_left  = max(a.left(),  b.left());
    T min_right = min(a.right(), b.right());
    T intersec_width = min_right - max_left;

    T max_top    = max(a.top(),    b.top());
    T min_bottom = min(a.bottom(), b.bottom());
    T intersec_height = min_bottom - max_top;

    T x = (intersec_width  < a.width()  + b.width()  ? max_left        : T());
    T y = (intersec_height < a.height() + b.height() ? max_top         : T());
    T w = (intersec_width  < a.width()  + b.width()  ? intersec_width  : T());
    T h = (intersec_height < a.height() + b.height() ? intersec_height : T());

    return { x, y, w, h };
}


template<typename StreamT, typename T> StreamT &operator<<(StreamT &stream, Rect<T> rect)
{
    stream << "Rect{" << rect.x() << ", " << rect.y() << ", " << rect.width() << ", " << rect.height() << "}";
    return stream;
}


template<typename T> class RectIntersection{
    Rect<T>  m_rect;
    Point<T> m_dst_offset;
    Point<T> m_src_offset;

public:
    RectIntersection(const Rect<T> &dst_rect, const Rect<T> &src_rect)
    {
        Rect<int> rect = intersection(src_rect, dst_rect);
        if(rect.width() > 0 && rect.height() > 0)
        {
            m_rect = rect;
            m_dst_offset = rect.position() - dst_rect.position();
            m_src_offset = rect.position() - src_rect.position();
        }
    }

    RectIntersection(){}

    inline Rect<T>   rect()       const { return m_rect; }

    inline Point<T>  position()   const { return m_rect.position(); }
    inline T         x()          const { return m_rect.x(); }
    inline T         y()          const { return m_rect.y(); }

    inline Size<T>   size()       const { return m_rect.size(); }
    inline T         width()      const { return m_rect.width(); }
    inline T         height()     const { return m_rect.height(); }

    inline Point<T>  dstOffset()  const { return m_dst_offset; }
    inline T         dstx()       const { return m_dst_offset.x(); }
    inline T         dsty()       const { return m_dst_offset.y(); }

    inline Point<T>  srcOffset()  const { return m_src_offset; }
    inline T         srcx()       const { return m_src_offset.x(); }
    inline T         srcy()       const { return m_src_offset.y(); }
};

}//namespace r64fx

#endif//R64FX_GUI_RECT_H
