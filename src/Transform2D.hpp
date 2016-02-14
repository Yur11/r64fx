#ifndef R64FX_TRANSFORM2D_HPP
#define R64FX_TRANSFORM2D_HPP

#include <cmath>

namespace r64fx{

template<typename T> class Transform2D{
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

#endif //R64FX_TRANSFORM2D_HPP