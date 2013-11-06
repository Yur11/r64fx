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

}//namespace r64fx

#endif//R64FX_GUI_GEOMETRY_IO_H