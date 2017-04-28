#ifndef R64FX_GUI_POINT_H
#define R64FX_GUI_POINT_H

namespace r64fx{

template<typename T> struct Point{
    T vec[2];

    Point<T>(T x, T y)
    : vec{x, y}
    {}

    Point<T>()
    : vec{0, 0}
    {}

    inline void setX(T x) { vec[0] = x; }
    inline void setY(T y) { vec[1] = y; }

    inline T x() const { return vec[0]; }
    inline T y() const { return vec[1]; }

    inline Point<T> &operator=(const Point<T> &other)
    {
        vec[0] = other.vec[0];
        vec[1] = other.vec[1];
        return *this;
    }

    inline Point<T> operator-()
    {
        return Point<T>(-x(), -y());
    }

    inline Point<T> operator+(Point<T> other)
    {
        return Point<T>(x() + other.x(), y() + other.y());
    }

    inline Point<T> operator-(Point<T> other)
    {
        return Point<T>(x() - other.x(), y() - other.y());
    }

    inline Point<T> operator+=(Point<T> p)
    {
        return (*this) = (*this) + p;
    }

    inline Point<T> operator-=(Point<T> p)
    {
        return (*this) = (*this) - p;
    }

    inline Point<T> operator*(T coeff)
    {
        return Point<T>(x()*coeff, y()*coeff);
    }

    inline Point<T> operator*(Point<T> p)
    {
        return Point<T>(x()*p.x(), y()*p.y());
    }

    inline Point<T> operator*=(T coeff)
    {
        return (*this) = (*this) * coeff;
    }

    inline Point<T> operator*=(Point<T> p)
    {
        return (*this) = (*this) * p;
    }

    inline bool operator==(const Point<T> &other) { return x() == other.x() && y() == other.y(); }
    inline bool operator!=(const Point<T> &other) { return x() != other.x() || y() != other.y(); }

    template<typename OtherT> Point<OtherT> to() const
    {
        return Point<OtherT>(OtherT(x()), OtherT(y()));
    }

    inline T* vec2() const { return vec; }

    inline Point<T> transposed() const { return {y(), x()}; }
};


template<typename StreamT, typename T> StreamT &operator<<(StreamT &stream, const Point<T> &point)
{
    stream << "Point{" << point.x() << ", " << point.y() << "}";
    return stream;
}

}//namespace r64fx

#endif//R64FX_GUI_POINT_H
