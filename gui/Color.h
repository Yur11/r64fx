#ifndef R64FX_GUI_COLOR_H
#define R64FX_GUI_COLOR_H


namespace r64fx{
    
union Color{
    float vec[4];
    struct{ float r, g, b, a; } component;
    
    Color(float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0) : vec{ r, g, b, a } {}
    
    inline void setRed(float val)   { vec[0] = val; }
    inline void setGreen(float val) { vec[1] = val; }
    inline void setBlue(float val)  { vec[2] = val; }
    inline void setAlpha(float val) { vec[3] = val; }
    
    inline float red()   const  { return vec[0]; }
    inline float green() const  { return vec[1]; }
    inline float blue()  const  { return vec[2]; }
    inline float alpha() const  { return vec[3]; }
};


class ColorMixin{ 
protected:
    Color _color;
   
public:
    inline void setColor(Color color) { _color = color; }
    inline void setColor(float r, float g, float b, float a = 1.0) { _color = Color(r, g, b, a); }
    
    inline Color color() const { return _color; }
};


class BackgroundColorMixin{ 
protected:
    Color _bg_color;   
   
public:
    inline void setBackgroundColor(Color color) { _bg_color = color; }
    inline void setBackgroundColor(float r, float g, float b, float a) { _bg_color = Color(r, g, b, a); }
    
    inline Color backgroundColor() const { return _bg_color; }
};


}//namespace r64fx

#endif//R64FX_GUI_COLOR_H