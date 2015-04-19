#ifndef R64FX_TRANSFORM2D_HPP
#define R64FX_TRANSFORM2D_HPP

#include <cmath>

namespace r64fx{

template<typename T> class Transform2D{
/*
    Let's only relevant keep parts of the matrix.

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
        c += x;
        f += y;
    }

    void rotate(T angle)
    {
        T sinang = sin(angle);
        T cosang = cos(angle);

        auto newa = cosang*a - sinang*d;
        auto newb = cosang*b - sinang*e;
        auto newc = cosang*c - sinang*f;
        auto newd = sinang*a + cosang*d;
        auto newe = sinang*b + cosang*e;
        auto newf = sinang*c + cosang*f;

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

    /** @brief Apply the transform to a point (x, y)*/
    void operator()(T &x, T &y)
    {
        T newx = a*x + b*y + c;
        T newy = d*x + e*y + f;
        x = newx;
        y = newy;
    }

    template<typename P> void operator()(P &p)
    {
        operator()(p.x, p.y);
    }
};

}//namespace r64fx

#endif //R64FX_TRANSFORM2D_HPP