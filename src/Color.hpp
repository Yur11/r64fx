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
};

}//namespace r64fx

#endif//R64FX_GUI_COLOR_H