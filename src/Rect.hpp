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
};


template<typename T> Rect<T> operator+(Rect<T> rect, Point<T> offset)
{
    return { rect.position() + offset, rect.size() };
}


template<typename T> Rect<T> intersection(Rect<T> a, Rect<T> b)
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
    
}//namespace r64fx

#endif//R64FX_GUI_RECT_H