#ifndef R64FX_GUI_GEOMETRY_IO_H
#define R64FX_GUI_GEOMETRY_IO_H

namespace r64fx{

    
template<typename T> std::ostream &operator<<(std::ostream &out, const Point<T> &point)
{
    out << "point: " << point.x << ", " << point.y;
    return out;
}


template<typename T> std::ostream &operator<<(std::ostream &out, const Size<T> &size)
{
    out << "size: " << size.w << ", " << size.h;
    return out;
}
    
    
template<typename T> std::ostream &operator<<(std::ostream &out, const Rect<T> &rect)
{
    out << "rect: " << rect.left << ", " << rect.top << ", " << rect.right << ", " << rect.bottom;
    return out;
}

inline std::ostream &operator<<(std::ostream &out, Matrix3x3 &m)
{
    for(int row=0; row<3; row++)
        for(int col=0; col<3; col++)
            out << m(col, row) << (col < 2 ? ", " : "\n");
}

}//namespace r64fx

#endif//R64FX_GUI_GEOMETRY_IO_H