#ifndef R64FX_GUI_RECT_H
#define R64FX_GUI_RECT_H

#include "Point.hpp"
#include "Size.hpp"

namespace r64fx{

template<typename T> T min(const T &a, const T &b) { return a < b ? a : b; }
template<typename T> T max(const T &a, const T &b) { return a > b ? a : b; }


template<typename T> class Rect{
    T mx;
    T my;
    T mw;
    T mh;

public:
    Rect(T x = T(), T y = T(), T w = T(), T h = T())
    : mx(x)
    , my(y)
    , mw(w)
    , mh(h)
    {}

    Rect(Point<T> pos, Size<T> size)
    : mx(pos.x())
    , my(pos.y())
    , mw(size.width())
    , mh(size.height())
    {}

    inline T x() const { return mx; }
    inline T y() const { return my; }
    inline T width()  const  { return mw; }
    inline T height() const  { return mh; }

    inline void setX(T val) { mx = val; }
    inline void setY(T val) { my = val; }
    inline void setWidth(T val)  { mw = val; }
    inline void setHeight(T val) { mh = val; }

    inline Point<T> position() const { return {x(), y()}; }
    inline void setPosition(Point<T> pos) { mx = pos.x(); my = pos.y(); }

    inline T left()   const { return mx; }
    inline T top()    const { return my; }
    inline T right()  const { return mx + mw; }
    inline T bottom() const { return my + mh; }

    inline void setLeft(T val)
    {
        T d = mx - val;
        mx -= d;
        mw += d;
    }

    inline void setTop(T val)
    {
        T d = my - val;
        my -= d;
        mw += d;
    }

    inline void setRight(T val) { mw = val - mx; }

    inline void setBottom(T val) { mh = val - my; }

    inline Size<T> size() const { return { mw, mh }; }

    inline void setSize(Size<T> size) { mw = size.width(); mh = size.height(); }

    inline void setSize(T w, T h) { mw = w; mh = h; }

    inline Point<T> topLeft()     { return { mx,    my    }; }
    inline Point<T> bottomLeft()  { return { mx,    my+mh }; }
    inline Point<T> topRight()    { return { mx+mw, my    }; }
    inline Point<T> bottomRight() { return { mx+mw, my+mh }; }

    inline T halfWidth()  { return mw/2; }
    inline T halfHeight() { return mh/2; }

    inline Point<T> center() { return { mx + halfWidth(), my + halfHeight() }; }

    inline bool overlaps(Point<int> p) const
    {
        return p.x() >= left() && p.x() < right() && p.y() >= top() && p.y() < bottom();
    }

    inline Rect<T> &operator+=(const Point<T> &other)
    {
        mx += other.x();
        my += other.y();
        return *this;
    }

    inline Rect<T> &operator-=(const Point<T> &other)
    {
        mx -= other.x();
        my -= other.y();
        return *this;
    }
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


template<typename T> inline Rect<T> operator&(const Rect<T> &a, const Rect<T> &b)
{
    return intersection(a, b);
}


template<typename StreamT, typename T> StreamT &operator<<(StreamT &stream, Rect<T> rect)
{
    stream << "Rect{" << rect.x() << ", " << rect.y() << ", " << rect.width() << ", " << rect.height() << "}";
    return stream;
}


template<typename T> class RectIntersection{
    Size<T>  m_size;
    Point<T> m_dst_offset;
    Point<T> m_src_offset;

public:
    RectIntersection(Point<T> dst_offset, Size<T> size, Point<T> src_offset)
    : m_size(size)
    , m_dst_offset(dst_offset)
    , m_src_offset(src_offset)
    {

    }

    RectIntersection(const Rect<T> &dst_rect, const Rect<T> &src_rect)
    {
        Rect<int> rect = intersection(src_rect, dst_rect);
        if(rect.width() > 0 && rect.height() > 0)
        {
            m_size = rect.size();
            m_dst_offset = rect.position() - dst_rect.position();
            m_src_offset = rect.position() - src_rect.position();
        }
    }

    RectIntersection(){}

    inline Size<T> size() const { return m_size; }
    inline T width()  const { return m_size.width(); }
    inline T height() const { return m_size.height(); }

    inline Point<T> dstOffset() const { return m_dst_offset; }
    inline T dstx() const { return m_dst_offset.x(); }
    inline T dsty() const { return m_dst_offset.y(); }
//     inline Rect<int> dstRect() const { return {dstOffset(), size()}; }


    inline Point<T> srcOffset() const { return m_src_offset; }
    inline T srcx() const { return m_src_offset.x(); }
    inline T srcy() const { return m_src_offset.y(); }
//     inline Rect<int> srcRect() const { return {srcOffset(), size()}; }
};
    
}//namespace r64fx

#endif//R64FX_GUI_RECT_H
