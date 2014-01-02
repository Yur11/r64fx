#ifndef R64FX_GUI_COLOR_H
#define R64FX_GUI_COLOR_H

#include <GL/glew.h>

namespace r64fx{
    
struct Color{
    float r, g, b, a;
    
    Color(float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0) : r(r), g(g), b(b), a(a) {}
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


inline void glColor(Color c){ glColor4f(c.r, c.g, c.b, c.a); };


}//namespace r64fx

#endif//R64FX_GUI_COLOR_H