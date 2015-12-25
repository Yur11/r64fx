#ifndef R64FX_GUI_COLOR_H
#define R64FX_GUI_COLOR_H


namespace r64fx{
    
template<typename T>
union Color{
    T vec[4];
    struct{ T r, g, b, a; } component;
    
    Color(T r = T(), T g = T(), T b = T(), T a = T()) : vec{ r, g, b, a } {}
    
    inline void setRed(T val)   { vec[0] = val; }
    inline void setGreen(T val) { vec[1] = val; }
    inline void setBlue(T val)  { vec[2] = val; }
    inline void setAlpha(T val) { vec[3] = val; }
    
    inline T red()   const  { return vec[0]; }
    inline T green() const  { return vec[1]; }
    inline T blue()  const  { return vec[2]; }
    inline T alpha() const  { return vec[3]; }

    inline T operator[](int i) const { return vec[i]; }

    template<typename OtherT> Color<OtherT> to()
    {
        return Color<OtherT>(
            OtherT(red()),
            OtherT(green()),
            OtherT(blue()),
            OtherT(alpha())
        );
    }
};


template<typename T> Color<T> operator+(const Color<T> &a, const Color<T> &b)
{
    return Color<T>(
        a.red()    +  b.red(),
        a.green()  +  b.green(),
        a.blue()   +  b.blue(),
        a.alpha()  +  b.alpha()
    );
}


template<typename T> Color<T> &operator+=(Color<T> &a, const Color<T> &b)
{
    a = a + b;
    return a;
}


template<typename T> Color<T> operator*(const Color<T> &a, const Color<T> &b)
{
    return Color<T>(
        a.red()    *  b.red(),
        a.green()  *  b.green(),
        a.blue()   *  b.blue(),
        a.alpha()  *  b.alpha()
    );
}


template<typename T> Color<T> &operator*=(Color<T> &a, const Color<T> &b)
{
    a = a * b;
    return a;
}


template<typename T> Color<T> operator*(const Color<T> &color, T c)
{
    return Color<T>(
        c * color.red(),
        c * color.green(),
        c * color.blue(),
        c * color.alpha()
    );
}


template<typename T> Color<T> &operator*=(Color<T> &color, T c)
{
    color = color * c;
    return color;
}

}//namespace r64fx

#endif//R64FX_GUI_COLOR_H