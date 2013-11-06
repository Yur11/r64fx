/** @file geometry.h defines a bunch of class that are usefull when working with geometric objects. */

#ifndef R64FX_GUI_GEOMETRY_H
#define R64FX_GUI_GEOMETRY_H

/*

    !!! (0, 0) is at the bottom left !!! (OpenGL default)
    
    
     (x, y+h)   top     (x+w, y+h)
         -----------------
         |               |
   left  |               |  right
         |               |
         -----------------
     (x, y)    bottom   (x+w, y)
*/

#include <cmath>

namespace r64fx{
    
/** @brief Point geometric primitive. */
template<typename T> struct Point{
    T x, y;
    
    Point<T>(T x = T(), T y = T()) 
    : x(x)
    , y(y)
    {}
    
    inline Point<T> operator-()
    {
        return Point<T>(-x, -y);
    }
    
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
    
    inline Point<T> rotated(float angle)
    {
        Point<T> p(x, y);
        p.rotate(angle);
        return p;
    }
    
    inline void rotate(float angle)
    {
        float sin_angle = sin(angle);
        float cos_angle = cos(angle);
        float x_cos_angle = x * cos_angle;
        float x_sin_angle = x * sin_angle;
        float y_cos_angle = y * cos_angle;
        float y_sin_angle = y * sin_angle;
        x = x_cos_angle - y_sin_angle;
        y = x_sin_angle + y_cos_angle;
    }
    
    inline bool operator==(const Point<T> &other) { return this->x == other.x && this->y == other.y; }
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
    
    inline bool operator==(const Size<T> &other) { return this->w == other.w && this->h == other.h; }
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
    
    bool overlaps(Rect<float> other)
    {
        auto leftmost = (left < other.left ? left : other.left);
        auto rightmost = (right > other.right ? right : other.right);
        auto topmost = (top > other.top ? top : other.top);
        auto bottommost = (bottom < other.bottom ? bottom : other.bottom);
        return width() + other.width() >= rightmost - leftmost && height() + other.height() >= topmost - bottommost;
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


struct Rubberband{
    Point<float> p1 = {0.0, 0.0};
    Point<float> p2 = {0.0, 0.0};
    
    inline void start(Point<float> p) { p1 = p2 = p; }
    inline void move(Point<float> p) { p2 = p; }
    
    inline Rect<float> rect() 
    { 
        return Rect<float>(
            p1.x < p2.x ? p1.x : p2.x,
            p1.y > p2.y ? p1.y : p2.y,
            p1.x > p2.x ? p1.x : p2.x,
            p1.y < p2.y ? p1.y : p2.y
        );
    }
    
    inline bool isVisible()
    {
        return p1.x - p2.x != 0 && p1.y - p2.y != 0;
    }
};





}//namespace r64fx

#endif//R64FX_GUI_GEOMETRY_H