#ifndef R64FX_GUI_SIZE_H
#define R64FX_GUI_SIZE_H

namespace r64fx{

template<typename T> class Size{
    T vec[2];

public:
    Size<T>(T w, T h) : vec{w, h} {}

    Size<T>() : vec{T(), T()} {}

    inline void setWidth(T width) { vec[0] = width; }

    inline T width() const { return vec[0]; }

    inline void setHeight(T height) { vec[1] = height; }

    inline T height() const { return vec[1]; }

    inline Size<T> &operator+=(const Size<T> &other)
    {
        setWidth(width() + other.width());
        setHeight(height() + other.height());
        return *this;
    }

    inline Size<T> &operator-=(const Size<T> &other)
    {
        setWidth(width() - other.width());
        setHeight(height() - other.height());
        return *this;
    }

    inline T* vec2() const { return vec; }

    inline Size<T> transposed() const { return {height(), width()}; }
};


template<typename T> inline bool operator==(const Size<T> &a, const Size<T> &b)
{
    return a.width() == b.width() && a.height() == b.height();
}


template<typename T> inline bool operator!=(const Size<T> &a, const Size<T> &b)
{
    return !operator==(a, b);
}


template<typename Ost, typename T> inline Ost &operator<<(Ost &ost, Size<T> size)
{
    ost << "Size{" << size.width() << ", " << size.height() << "}";
    return ost;
}

}//namespace r64fx

#endif//R64FX_GUI_SIZE_H
