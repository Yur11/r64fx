#include "geometry.h"


namespace r64fx{
    
    
void Matrix3x3::fill(float val)
{
    for(int i=0; i<9; i++)
        data[i] = val;
}    


void Matrix3x3::loadMainDiagonal(float val)
{
    for(int i=0; i<3; i++)
        (*this)(i, i) = val;
}
    
    
void Matrix3x3::scale(float w, float h)
{    
    (*this) = Matrix3x3(
        w,   0.0, 0.0,
        0.0, h,   0.0,
        0.0, 0.0, 1.0
    ) * (*this);
}


void Matrix3x3::translate(float x, float y)
{
    (*this) = Matrix3x3(
        1.0, 0.0, x,
        0.0, 1.0, y,
        0.0, 0.0, 1.0
    ) * (*this);
}


void Matrix3x3::rotate(float angle)
{
    (*this) = Matrix3x3(
        cos(angle),  sin(angle), 0.0,
        -sin(angle), cos(angle), 0.0,
        0.0,         0.0,        1.0
    ) * (*this);
}
    
}//namespace r64fx