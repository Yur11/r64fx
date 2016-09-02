#ifndef R64FX_COMPLEX_HPP
#define R64FX_COMPLEX_HPP

#include <cmath>

namespace r64fx{

template<typename T> class Complex;
template<typename T> class Polar;

template<typename T> T Re(const Complex<T>&);
template<typename T> T Im(const Complex<T>&);

template<typename T> T Magnitude(const Polar<T>&);
template<typename T> T Phase(const Polar<T>&);

template<typename T> class Complex{
    friend T Re<T>(const Complex<T>&);
    friend T Im<T>(const Complex<T>&);

    T re = T();
    T im = T();

public:
    Complex(){}

    Complex(const T &re, const T &im) : re(re), im(im) {}

    Complex(const Polar<T> &polar);

    Complex<T> &operator=(const Polar<T> &polar);
};

template<typename T> inline T &Re(const Complex<T> &num)
{
    return num.re;
}

template<typename T> inline T &Im(const Complex<T> &num)
{
    return num.im;
}

template<typename T> inline Complex<T> Conjugate(const Complex<T> &num)
{
    return Complex<T>(Re(num), -Im(num));
}

template<typename T> Complex<T> operator+(const Complex<T> &a, const Complex<T> &b)
{
    return Complex<T>(Re(a) + Re(b), Im(a) + Im(b));
}

template<typename T> Complex<T> operator-(const Complex<T> &a, const Complex<T> &b)
{
    return Complex<T>(Re(a) - Re(b), Im(a) - Im(b));
}

template<typename T> Complex<T> &operator*(const Complex<T> &a, const Complex<T> &b)
{
    return Complex<T>(Re(a)*Re(b) - Im(a)*Im(b), Im(a)*Re(b) + Re(a)*Im(b));
}

template<typename T> Complex<T> &operator+=(Complex<T> &a, const Complex<T> &b)
{
    Re(a) += Re(b);
    Im(a) += Im(b);
    return a;
}

template<typename T> Complex<T> &operator-=(Complex<T> &a, const Complex<T> &b)
{
    Re(a) -= Re(b);
    Im(a) -= Im(b);
    return a;
}

template<typename T> Complex<T> &operator*=(Complex<T> &a, const Complex<T> &b)
{
    a = a * b;
    return a;
}


template<typename T> class Polar{
    friend T Magnitude<T>(const Polar<T>&);
    friend T Phase<T>(const Polar<T>&);

    T magnitude = T();
    T phase = T();

public:
    Polar(){}

    Polar(const T &magnitude, const T &phase) : magnitude(magnitude), phase(phase) {}

    Polar(const Complex<T> &complex)
    {
        operator==(complex);
    }

    Polar<T> &operator=(const Complex<T> &complex)
    {
        magnitude = sqrt(Re(complex)*Re(complex) + Im(complex)*Im(complex));
        phase = atan2(Im(complex), Re(complex));
        return *this;
    }
};


template<typename T> Complex<T>::Complex(const Polar<T> &polar)
{
    operator=(polar);
}


template<typename T> Complex<T> &Complex<T>::operator=(const Polar<T> &polar)
{
    re = Magnitude(polar) * cos(Phase(polar));
    im = Magnitude(polar) * sin(Phase(polar));
}

template<typename T> T &Magnitude(const Polar<T> &num)
{
    return num.magnitude;
}

template<typename T> T &Phase(const Polar<T> &num)
{
    return num.phase;
}

}//namespace r64fx

#endif//R64FX_COMPLEX_HPP