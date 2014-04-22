#ifndef R64FX_GUI_RECT_H
#define R64FX_GUI_RECT_H

#include "Point.h"
#include "Size.h"

namespace r64fx{
    
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
    
    inline T y() const { return top; }
    
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
    
    bool overlaps(Rect<float> other)
    {
        auto leftmost = (left < other.left ? left : other.left);
        auto rightmost = (right > other.right ? right : other.right);
        auto topmost = (top > other.top ? top : other.top);
        auto bottommost = (bottom < other.bottom ? bottom : other.bottom);
        return width() + other.width() >= rightmost - leftmost && height() + other.height() >= topmost - bottommost;
    }
    
    Point<float> center() const
    {
        return Point<float>( left + width() * 0.5, bottom + height() * 0.5 );
    }
    
    template<typename OtherT> Rect<OtherT> to() { return Rect<OtherT>(left, top, right, bottom); }
    
    /** @brief Move the rect. */
    inline Rect<T> operator+(Point<T> point) { return Rect<T>(position() + point, size()); }
    inline Rect<T> operator-(Point<T> point) { return Rect<T>(position() - point, size()); }
    
    /** @brief Outset/Inset */
    inline Rect<T> operator+(T val) { return Rect<T>(left - val, top + val, right + val, bottom - val); }
    inline Rect<T> operator-(T val) { return this->operator+(-val); }
    
    inline bool operator==(const Rect<T> &other) 
    { 
        return 
            this->left = other.left && 
            this->top = other.top &&
            this->right = other.right &&
            this->bottom = other.bottom
        ;
    }
    
    inline bool isNull() { return !width() || !height(); }
};
    
}//namespace r64fx

#endif//R64FX_GUI_RECT_H