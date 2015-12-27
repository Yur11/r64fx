#ifndef R64FX_GUI_SIZE_H
#define R64FX_GUI_SIZE_H

namespace r64fx{
  
template<typename T> class Size{
    T mw, mh;
    
public:
    Size<T>(T w = T(), T h = T()) 
    : mw(w)
    , mh(h)
    {}
    
    inline void setWidth(T width) { mw = width; }

    inline T width() const { return mw; }
    
    inline void setHeight(T height) { mh = height; }

    inline T height() const { return mh; }
    
    inline bool operator==(const Size<T> &other) { return mw == other.width() && mh == other.height(); }
    inline bool operator!=(const Size<T> &other) { return mw != other.width() || mh != other.height(); }
};


template<typename Ost, typename T> inline Ost &operator<<(Ost &ost, Size<T> size)
{
    ost << "Size{" << size.w << ", " << size.h << "}";
    return ost;
}
    
}//namespace r64fx

#endif//R64FX_GUI_SIZE_H