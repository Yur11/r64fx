#ifndef R64FX_GUI_PROJECTION_2D_H
#define R64FX_GUI_PROJECTION_2D_H

#include "geometry.h"

namespace r64fx{
    
class Projection2D{
    float sxsytxty[4];
    
public:
    Projection2D(float sx = 1.0, float sy = 1.0, float tx = 0.0, float ty = 0.0)
    : sxsytxty{ sx, sy, tx, ty }
    {}
    
    static Projection2D ortho2d(float left, float right, float bottom, float top)
    {
        float sx = 2.0 / (right - left);
        float sy = 2.0 / (top - bottom);
        float tx = - (right + left) / (right - left);
        float ty = - (top + bottom) / (top - bottom);
        
        return Projection2D(sx, sy, tx, ty);
    }
    
    inline float* vec() { return sxsytxty; }
    
    inline void scale_x(float x) { sxsytxty[0] *= x; }
    inline void scale_y(float y) { sxsytxty[1] *= y; }
    inline void scale(float x, float y)
    {
        scale_x(x);
        scale_y(y);
    }
    
    inline void translate_x(float x) { sxsytxty[2] += x * sxsytxty[0]; }
    inline void translate_y(float y) { sxsytxty[3] += y * sxsytxty[1]; }
    inline void translate(float x, float y)
    {
        translate_x(x);
        translate_y(y);
    }
    
    inline float sx() const { return sxsytxty[0]; }
    inline float sy() const { return sxsytxty[1]; }
    inline float tx() const { return sxsytxty[2]; }
    inline float ty() const { return sxsytxty[3]; }
    
    inline Point<float> project(Point<float> p)
    {
        return { p.x * sx() + tx(), p.y * sy() + ty() };
    }
    
    static void resetCurrentProjection();
};

#ifdef R64FX_PROJECTION_2D_IMPLEMENTATION
Projection2D _initial_2d_projection;
#else
extern
#endif

Projection2D* current_2d_projection

#ifdef R64FX_PROJECTION_2D_IMPLEMENTATION
= &_initial_2d_projection;
#endif

;

}//namespace r64fx

#endif//R64FX_GUI_PROJECTION_2D_H