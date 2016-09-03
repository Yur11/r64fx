#ifndef R64FX_COMPLEX_HPP
#define R64FX_COMPLEX_HPP

#include <cmath>

namespace r64fx{

template<typename T> class Complex;
template<typename T> class Polar;

template<typename T> struct Complex{
    T re = T();
    T im = T();

public:
    Complex(){}

    Complex(const T &re, const T &im) : re(re), im(im) {}

    Complex(const Polar<T> &polar);

    Complex<T> &operator=(const Polar<T> &polar);

    inline Complex<T> conjugate() const
    {
        return Complex<T>(re, -im);
    }

    inline T magnitude() const
    {
        return sqrt(re*re + im*im);
    }

    inline T phase() const
    {
        return atan2(im, re);
    }
};


template<typename T> Complex<T> operator+(const Complex<T> &a, const Complex<T> &b)
{
    return Complex<T>(a.re + b.re, a.im + b.im);
}

template<typename T> Complex<T> operator-(const Complex<T> &a, const Complex<T> &b)
{
    return Complex<T>(a.re - b.re, a.im - b.im);
}

template<typename T> Complex<T> operator*(const Complex<T> &a, const Complex<T> &b)
{
    return Complex<T>(a.re*b.re - a.im*b.im, a.im*b.re + a.re*b.im);
}

template<typename T> Complex<T> operator/(const Complex<T> &a, const Complex<T> &b)
{
    return Complex<T>(
        (a.re * b.re + a.im * b.im) / (b.re * b.re + b.im * b.im),
        (a.im * b.re - a.re * b.im) / (b.re * b.re + b.im * b.im)
    );
}

template<typename T> Complex<T> &operator+=(Complex<T> &a, const Complex<T> &b)
{
    a.re += b.re;
    a.im += b.im;
    return a;
}

template<typename T> Complex<T> &operator-=(Complex<T> &a, const Complex<T> &b)
{
    a.re -= b.re;
    a.im -= b.im;
    return a;
}

template<typename T> Complex<T> &operator*=(Complex<T> &a, const Complex<T> &b)
{
    a = a * b;
    return a;
}

template<typename T> Complex<T> &operator/=(Complex<T> &a, const Complex<T> &b)
{
    a = a / b;
    return a;
}


template<typename T> struct Polar{
    T magnitude = T();
    T phase = T();

    Polar(){}

    Polar(const T &magnitude, const T &phase) : magnitude(magnitude), phase(phase) {}

    Polar(const Complex<T> &complex)
    {
        operator==(complex);
    }

    Polar<T> &operator=(const Complex<T> &complex)
    {
        magnitude = complex.magnitude();
        phase = complex.phase();
        return *this;
    }

    T re()
    {
        return magnitude * cos(phase);
    }

    T im()
    {
        return magnitude * sin(phase);
    }
};


template<typename T> Complex<T>::Complex(const Polar<T> &polar)
{
    operator=(polar);
}


template<typename T> Complex<T> &Complex<T>::operator=(const Polar<T> &polar)
{
    re = polar.re();
    im = polar.im();
}

}//namespace r64fx

#endif//R64FX_COMPLEX_HPP