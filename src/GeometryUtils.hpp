#ifndef R64FX_GEOMETRY_UTILS_HPP
#define R64FX_GEOMETRY_UTILS_HPP

#include <algorithm>
#include <cmath>

namespace r64fx{

enum class Orientation{
    Vertical,
    Horizontal
};


template<typename T> class Point{
    T vec[2];

public:
    Point<T>() : vec{T(), T()} {}

    Point<T>(T x, T y) : vec{x, y} {}

    Point<T>(const Point<T> &other) : vec{other.x(), other.y()} {}

    inline Point<T> &operator=(const Point<T> &other)
    {
        vec[0] = other.vec[0];
        vec[1] = other.vec[1];
        return *this;
    }

    inline void setX(T x) { vec[0] = x; }
    inline void setY(T y) { vec[1] = y; }

    inline T x() const { return vec[0]; }
    inline T y() const { return vec[1]; }

    inline Point<T> operator-()
    {
        return Point<T>(-x(), -y());
    }

    inline Point<T> operator+(Point<T> other)
    {
        return Point<T>(x() + other.x(), y() + other.y());
    }

    inline Point<T> operator-(Point<T> other)
    {
        return Point<T>(x() - other.x(), y() - other.y());
    }

    inline Point<T> operator+=(Point<T> p)
    {
        return (*this) = (*this) + p;
    }

    inline Point<T> operator-=(Point<T> p)
    {
        return (*this) = (*this) - p;
    }

    inline Point<T> operator*(T coeff)
    {
        return Point<T>(x()*coeff, y()*coeff);
    }

    inline Point<T> operator*(Point<T> p)
    {
        return Point<T>(x()*p.x(), y()*p.y());
    }

    inline Point<T> operator*=(T coeff)
    {
        return (*this) = (*this) * coeff;
    }

    inline Point<T> operator*=(Point<T> p)
    {
        return (*this) = (*this) * p;
    }

    inline bool operator==(const Point<T> &other) { return x() == other.x() && y() == other.y(); }
    inline bool operator!=(const Point<T> &other) { return x() != other.x() || y() != other.y(); }

    template<typename OtherT> Point<OtherT> to() const
    {
        return Point<OtherT>(OtherT(x()), OtherT(y()));
    }

    inline Point<T> transposed() const { return {y(), x()}; }

    inline T* vec2() const { return vec; }
};


template<typename StreamT, typename T> StreamT &operator<<(StreamT &stream, const Point<T> &point)
{
    stream << "Point{" << point.x() << ", " << point.y() << "}";
    return stream;
}


template<typename T> class Size{
    T vec[2];

public:
    Size<T>() : vec{T(), T()} {}

    Size<T>(T w, T h) : vec{w, h} {}

    Size<T>(const Size<T> &other) : vec{other.width(), other.height()} {}

    inline Size<T> &operator=(const Size<T> &other)
    {
        vec[0] = other.vec[0];
        vec[1] = other.vec[1];
        return *this;
    }

    inline void setWidth(T width) { vec[0] = width; }

    inline T width() const { return vec[0]; }

    inline void setHeight(T height) { vec[1] = height; }

    inline T height() const { return vec[1]; }

    inline Size<T> &operator+=(const Size<T> &other)
    {
        setWidth(width() + other.width());
        setHeight(height() + other.height());
        return *this;
    }

    inline Size<T> &operator-=(const Size<T> &other)
    {
        setWidth(width() - other.width());
        setHeight(height() - other.height());
        return *this;
    }

    inline bool operator==(const Size<T> &other) { return width() == other.width() && height() == other.height(); }
    inline bool operator!=(const Size<T> &other) { return width() != other.width() || height() != other.height(); }

    inline Size<T> transposed() const { return {height(), width()}; }

    inline T* vec2() const { return vec; }
};


template<typename T> inline Size<T> operator+(Size<T> a, Size<T> b)
{
    a += b;
    return a;
}


template<typename T> inline Size<T> operator-(Size<T> a, Size<T> b)
{
    a -= b;
    return a;
}


template<typename Ost, typename T> inline Ost &operator<<(Ost &ost, Size<T> size)
{
    ost << "Size{" << size.width() << ", " << size.height() << "}";
    return ost;
}


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

    inline Size<T> size() const { return {width(), height()}; }
    inline void setSize(Size<T> size) { setWidth(size.width()); setHeight(size.height()); }
    inline void setSize(T w, T h) { setSize({w, h}); }

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

    inline Point<T> topLeft()     const { return { left(),  top()    }; }
    inline Point<T> bottomLeft()  const { return { left(),  bottom() }; }
    inline Point<T> topRight()    const { return { right(), top()    }; }
    inline Point<T> bottomRight() const { return { right(), bottom() }; }

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

    inline bool operator==(const Rect<T> &other)
    {
        return position() == other.position() && size() == other.size();
    }

    inline bool operator!=(const Rect<T> &other)
    {
        return !operator==(*this, other);
    }

    inline T* vec4() const { return vec; }
};


template<typename T> inline Rect<T> operator+(Rect<T> rect, Point<T> offset)
{
    return {rect.position() + offset, rect.size()};
}


template<typename T> inline Rect<T> operator-(Rect<T> rect, Point<T> offset)
{
    return {rect.position() - offset, rect.size()};
}


template<typename T> inline Rect<T> operator+(Rect<T> rect, Size<T> offset)
{
    return {rect.position(), rect.size() + offset};
}


template<typename T> inline Rect<T> operator-(Rect<T> rect, Size<T> offset)
{
    return {rect.position(), rect.size() - offset};
}



template<typename StreamT, typename T> StreamT &operator<<(StreamT &stream, Rect<T> rect)
{
    stream << "Rect{" << rect.x() << ", " << rect.y() << ", " << rect.width() << ", " << rect.height() << "}";
    return stream;
}


template<typename T> Rect<T> intersection(const Rect<T> &a, const Rect<T> &b)
{
    T max_left  = std::max(a.left(),  b.left());
    T min_right = std::min(a.right(), b.right());
    T intersec_width = min_right - max_left;

    T max_top    = std::max(a.top(),    b.top());
    T min_bottom = std::min(a.bottom(), b.bottom());
    T intersec_height = min_bottom - max_top;

    T x = (intersec_width  < a.width()  + b.width()  ? max_left        : T());
    T y = (intersec_height < a.height() + b.height() ? max_top         : T());
    T w = (intersec_width  < a.width()  + b.width()  ? intersec_width  : T());
    T h = (intersec_height < a.height() + b.height() ? intersec_height : T());

    return { x, y, w, h };
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

    inline operator bool() const { return m_rect.width() > 0 && m_rect.height() > 0; }
};


template<typename T> class FlippedIntersection{
    Size<int>   m_dst_size;
    Size<int>   m_src_size;
    Point<int>  m_dst_offset;
    Point<int>  m_src_offset;

public:
    FlippedIntersection(const Rect<T> &dst_rect, const Point<T> pos, const Rect<T> &src_rect, bool flip_vert, bool flip_hori, bool flip_diag)
    {
        RectIntersection<T> isec(
            dst_rect, Rect<T>(pos, flip_diag ? src_rect.size().transposed() : src_rect.size())
        );
        if(!isec)
            return;

        T src_offset_x = src_rect.x();
        T src_offset_y = src_rect.y();

        auto isec_src_offset  = (flip_diag ?  isec.srcOffset().transposed()  :  isec.srcOffset());
        auto isec_src_size    = (flip_diag ?  isec.size().transposed()       :  isec.size());

        T dw = src_rect.width() - isec_src_size.width();
        T dh = src_rect.height() - isec_src_size.height();

        if(flip_diag)
            std::swap(flip_hori, flip_vert);
        if(flip_hori || isec_src_offset.x() > 0)
            src_offset_x += dw;
        if(flip_vert || isec_src_offset.y() > 0)
            src_offset_y += dh;

        m_src_offset  = {src_offset_x, src_offset_y};
        m_dst_offset  = isec.dstOffset();
        m_src_size    = isec_src_size;
        m_dst_size    = isec.size();
    }

    inline Size<T>   dstSize()    const { return m_dst_size; }
    inline T         dstWidth()   const { return m_dst_size.width(); }
    inline T         dstHeight()  const { return m_dst_size.height(); }

    inline Size<T>   srcSize()    const { return m_src_size; }
    inline T         srcWidth()   const { return m_src_size.width(); }
    inline T         srcHeight()  const { return m_src_size.height(); }

    inline Point<T>  dstOffset()  const { return m_dst_offset; }
    inline T         dstx()       const { return m_dst_offset.x(); }
    inline T         dsty()       const { return m_dst_offset.y(); }

    inline Point<T>  srcOffset()  const { return m_src_offset; }
    inline T         srcx()       const { return m_src_offset.x(); }
    inline T         srcy()       const { return m_src_offset.y(); }

    inline operator bool() const { return m_dst_size.width() > 0 && m_dst_size.height() > 0; }
};


template<typename T> class Transformation2D{
/*
    Let's keep only relevant parts of the matrix.

    a b c   x
    d e f * y
    0 0 1   1
 */
    T a = T(1);
    T b = T(0);
    T c = T(0);
    T d = T(0);
    T e = T(1);
    T f = T(0);

public:
    void translate(T x, T y)
    {
        c -= x;
        f -= y;
    }

    void rotate(T angle)
    {
        rotate(sin(angle), cos(angle));
    }

    void rotate(T sinang, T cosang)
    {
        auto newa = cosang*a + sinang*d;
        auto newb = cosang*b + sinang*e;
        auto newc = cosang*c + sinang*f;
        auto newd = cosang*d - sinang*a;
        auto newe = cosang*e - sinang*b;
        auto newf = cosang*f - sinang*c;

        a = newa;
        b = newb;
        c = newc;
        d = newd;
        e = newe;
        f = newf;
    }

    void scale(T sx, T sy)
    {
        a *= sx;
        b *= sx;
        c *= sx;
        d *= sy;
        e *= sy;
        f *= sy;
    }

    template<typename P> void operator()(P &p) const
    {
        T newx = a*p.x() + b*p.y() + c;
        T newy = d*p.x() + e*p.y() + f;
        p = {newx, newy};
    }
};

}//namespace r64fx

#endif//R64FX_GEOMETRY_UTILS_HPP
