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
    
    bool isBad() const { return left > right || top > bottom; }
    
    bool isGood() const { return left < right && top < bottom; }
        
    inline T x() const { return left; }
    
    inline T y() const { return top; }
    
    /** @brief Set rect position along the x axis. 
        
        Both left and right values are altered.
     */
    void set_x(T x) { auto w = width(); left = x; right = x + w; }
    
    /** @brief Set rect position along the y axis. 
     
        Both top and bottom values are altered.
     */
    void set_y(T y) { auto h = height(); top = y; bottom = y + h; }
    
    inline Point<T> position() const { return Point<T>(x(), y()); }
    
    inline void setPosition(T x, T y) 
    {
        set_x(x);
        set_y(y);
    }
    
    inline void setPosition(Point<T> p)
    {
        setPosition(p.x, p.y);
    }
    
    inline T width() const { return right - left; }
    
    inline T height() const { return bottom - top; }
    
    void setWidth(T width) { right = left + width; }
    
    void setHeight(T height) { bottom = top + height; }
    
    inline Size<T> size() const { return Size<T>(width(), height()); }
    
    inline void setSize(T w, T h)
    {
        setWidth(w);
        setHeight(h);
    }
    
    inline void setSize(Size<T> size)
    {
        setSize(size.w, size.h);
    }
    
    inline bool overlaps(Point<T> point) 
    {
        return point.x > left && point.x < right && point.y > top && point.y < bottom;
    }
    
    inline bool overlaps(T x, T y) 
    {
        return overlaps(Point<T>(x, y));
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


template<typename T> Rect<T> intersection_of(Rect<T> a, Rect<T> b)
{
    return {
        a.left   > b.left   ? a.left   : b.left,
        a.top    > b.top    ? a.top    : b.top,
        a.right  < b.right  ? a.right  : b.right,
        a.bottom < b.bottom ? a.bottom : b.bottom
    };
}

    
}//namespace r64fx

#endif//R64FX_GUI_RECT_H