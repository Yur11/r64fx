#ifndef R64FX_GUI_POINT_H
#define R64FX_GUI_POINT_H

namespace r64fx{
    
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
    
    inline bool operator==(const Point<T> &other) { return this->x == other.x && this->y == other.y; }
};
    
}//namespace r64fx

#endif//R64FX_GUI_POINT_H