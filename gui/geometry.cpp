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


template<typename T1, typename T2, typename T3> inline void mul_matrices_3x3(T1 a, T2 b, T3 out)
{
    out = Matrix3x3(
        a(0, 0) * b(0, 0) + a(1, 0) * b(0, 1) + a(2, 0) * b(0, 2),
        a(0, 0) * b(1, 0) + a(1, 0) * b(1, 1) + a(2, 0) * b(1, 2),
        a(0, 0) * b(2, 0) + a(1, 0) * b(2, 1) + a(2, 0) * b(2, 2),
        
        a(0, 1) * b(0, 0) + a(1, 1) * b(0, 1) + a(2, 1) * b(0, 2),
        a(0, 1) * b(1, 0) + a(1, 1) * b(1, 1) + a(2, 1) * b(1, 2),
        a(0, 1) * b(2, 0) + a(1, 1) * b(2, 1) + a(2, 1) * b(2, 2),
        
        a(0, 2) * b(0, 0) + a(1, 2) * b(0, 1) + a(2, 2) * b(0, 2),
        a(0, 2) * b(1, 0) + a(1, 2) * b(1, 1) + a(2, 2) * b(1, 2),
        a(0, 2) * b(2, 0) + a(1, 2) * b(2, 1) + a(2, 2) * b(2, 2)
    );
}


Matrix3x3 operator*(Matrix3x3 &a, Matrix3x3 &b)
{
    Matrix3x3 out;
    mul_matrices_3x3<Matrix3x3&, Matrix3x3&, Matrix3x3&>(a, b, out);
    return out;
}


Matrix3x3 operator*(const Matrix3x3 &a, Matrix3x3 &b)
{
    Matrix3x3 out;
    mul_matrices_3x3<const Matrix3x3&, Matrix3x3&, Matrix3x3&>(a, b, out);
    return out;
}


Matrix3x3 operator*(Matrix3x3 &a, const Matrix3x3 &b)
{
    Matrix3x3 out;
    mul_matrices_3x3<Matrix3x3&, const Matrix3x3&, Matrix3x3&>(a, b, out);
    return out;
}


Matrix3x3 operator*(const Matrix3x3 &a, const Matrix3x3 &b)
{
    Matrix3x3 out;
    mul_matrices_3x3<const Matrix3x3&, const Matrix3x3&, Matrix3x3&>(a, b, out);
    return out;
}

    
}//namespace r64fx