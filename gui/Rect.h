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
    
    Rect<T>(Point<T> p, Size<T> s) : Rect<T>(p.x, p.y, p.x + s.w, p.y + s.h) {}
    
    inline T x() const { return left; }
    
    inline T y() const { return top; }
    
    inline Point<T> position() const { return Point<T>(x(), y()); }
    
    inline T width() const { return right - left; }
    
    inline T height() const { return bottom - top; }
    
    inline Size<T> size() const { return Size<T>(width(), height()); }
    
    inline bool overlaps(Point<T> point) 
    {
        return point.x > left && point.x < right && point.y > top && point.y < bottom;
    }
    
    inline bool overlaps(T x, T y) 
    {
        return overlaps(Point<T>(x, y));
    }
    
    bool overlaps(Rect<T> other)
    {
        auto leftmost    =  left   < other.left   ?  left    : other.left;
        auto topmost     =  top    < other.top    ?  top     : other.top;
        auto rightmost   =  right  > other.right  ?  right   : other.right;
        auto bottommost  =  bottom > other.bottom ?  bottom  : other.bottom;
        
        return (rightmost - leftmost) < (width() + other.width()) && (bottommost - topmost) < (height() + other.height());
    }
    
    Point<T> center() const
    {
        return Point<T>( left + width() * 0.5, bottom + height() * 0.5 );
    }
    
    /** @brief  Make sure a point fits inside this rect.*/
    void fit(Point<T> &p)
    {
        if(p.x < left)
            p.x = left;
        else if(p.x > right)
            p.x = right;
        
        if(p.y < top)
            p.y = top;
        else if(p.y > right)
            p.y = right;
    }
    
    template<typename OtherT> Rect<OtherT> to() { return Rect<OtherT>(left, top, right, bottom); }
    
    /** @brief Move the rect. */
    inline Rect<T> operator+(Point<T> point) { return Rect<T>(position() + point, size()); }
    inline Rect<T> operator-(Point<T> point) { return Rect<T>(position() - point, size()); }
    
    /** @brief Outset/Inset */
    inline Rect<T> operator+(T val) { return Rect<T>(left + val, top + val, right + val, bottom + val); }
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
};
    
}//namespace r64fx

#endif//R64FX_GUI_RECT_H