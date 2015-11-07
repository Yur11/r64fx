#ifndef R64FX_GUI_COLOR_H
#define R64FX_GUI_COLOR_H


namespace r64fx{
    
template<typename T>
union Color{
    T vec[4];
    struct{ T r, g, b, a; } component;
    
    Color(T r = 1.0, T g = 1.0, T b = 1.0, T a = 1.0) : vec{ r, g, b, a } {}
    
    inline void setRed(T val)   { vec[0] = val; }
    inline void setGreen(T val) { vec[1] = val; }
    inline void setBlue(T val)  { vec[2] = val; }
    inline void setAlpha(T val) { vec[3] = val; }
    
    inline T red()   const  { return vec[0]; }
    inline T green() const  { return vec[1]; }
    inline T blue()  const  { return vec[2]; }
    inline T alpha() const  { return vec[3]; }
};


// class ColorMixin{
// protected:
//     Color _color;
//
// public:
//     inline void setColor(Color color) { _color = color; }
//     inline void setColor(T r, T g, T b, T a = 1.0) { _color = Color(r, g, b, a); }
//
//     inline Color color() const { return _color; }
// };
//
//
// class BackgroundColorMixin{
// protected:
//     Color _bg_color;
//
// public:
//     inline void setBackgroundColor(Color color) { _bg_color = color; }
//     inline void setBackgroundColor(T r, T g, T b, T a) { _bg_color = Color(r, g, b, a); }
//
//     inline Color backgroundColor() const { return _bg_color; }
// };


}//namespace r64fx

#endif//R64FX_GUI_COLOR_H