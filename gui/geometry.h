#ifndef R64FX_GUI_GEOMETRY_H
#define R64FX_GUI_GEOMETRY_H

namespace r64fx{
    
/** @brief Point geometric primitive. */
template<typename T> struct Point{
    T x, y;
    
    Point<T>(T x = T(), T y = T()) 
    : x(x)
    , y(y)
    {}
    
    inline Point<T> operator+(Point<T> other)
    {
        return Point<T>(x + other.x, y + other.y);
    }
    
    inline Point<T> operator-(Point<T> other)
    {
        return Point<T>(x - other.x, y - other.y);
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
        return Point<T>(x*coeff, y*coeff);
    }
    
    inline Point<T> operator*(Point<T> p)
    {
        return Point<T>(x*p.x, y*p.y);
    }
    
    inline Point<T> operator*=(T coeff)
    {
        return (*this) = (*this) * coeff;
    }
    
    inline Point<T> operator*=(Point<T> p)
    {
        return (*this) = (*this) * p;
    }
};


/** @brief Size geometric primitive. */
template<typename T> struct Size{
    T w, h;
    
    Size<T>(T w = T(), T h = T()) 
    : w(w)
    , h(h)
    {}
    
    /** @brief Convert point into size. */
    Size<T>(Point<T> p) : Size<T>(p.x, p.y) {}
    
    inline Point<T> toPoint() const { return Point<T>(w, h); }
};
    
    
/** @brief Rectange geometry primitive. */
template<typename T> struct Rect{
    T left, top, right, bottom;
    
    Rect<T>(T left = T(), T top = T(), T right = T(), T bottom = T()) 
    : left(left)
    , top(top)
    , right(right)
    , bottom(bottom)
    {}
    
    Rect<T>(Point<T> p, Size<T> s) : Rect<T>(p.x, p.y + s.h, p.x + s.w, p.y) {}
    
    inline T x() const { return left; }
    
    inline T y() const { return bottom; }
    
    inline Point<T> position() const { return Point<T>(x(), y()); }
    
    inline T width() const { return right - left; }
    
    inline T height() const { return top - bottom; }
    
    inline Size<T> size() const { return Size<T>(width(), height()); }
    
    inline bool overlaps(Point<T> point) 
    {
        return point.x > left && point.x < right && point.y < top && point.y > bottom;
    }
    
    inline bool overlaps(T x, T y) 
    {
        return overlaps(Point<float>(x, y));
    }
    
    template<typename OtherT> Rect<OtherT> to() { return Rect<OtherT>(left, top, right, bottom); }
};
    

/** @brief Padding mixin. */
class Padding{
    Rect<float> _padding;
    
public:
    inline float paddingLeft()   const { return _padding.left; }
    inline void setPaddingLeft(float padding) { _padding.left = padding; }
    
    inline float paddingTop()    const { return _padding.top; }
    inline void setPaddingTop(float padding) { _padding.top = padding; }
    
    inline float paddingRight()  const { return _padding.right; }
    inline void setPaddingRight(float padding) { _padding.right = padding; }
    
    inline float paddingBottom() const { return _padding.bottom; }
    inline void setPaddingBottom(float padding) { _padding.bottom = padding; }
    
    inline Rect<float> padding() const { return _padding; }
    inline void setPadding(Rect<float> padding) { _padding = padding; }
    inline void setPadding(float left, float top, float right, float bottom) { _padding = Rect<float>(left, top, right, bottom); }
    inline void setPadding(float padding) { _padding = Rect<float>(padding, padding, padding, padding); }
};


enum class Orientation{
    Vertical,
    Horizontal
};

}//namespace r64fx

#endif//R64FX_GUI_GEOMETRY_H