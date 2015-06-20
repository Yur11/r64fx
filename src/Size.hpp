#ifndef R64FX_GUI_SIZE_H
#define R64FX_GUI_SIZE_H

namespace r64fx{
  
template<typename T> struct Size{
    T w, h;
    
    Size<T>(T w = T(), T h = T()) 
    : w(w)
    , h(h)
    {}
    
    inline T width()  const { return w; }
    
    inline T height() const { return h; }
    
    inline bool operator==(const Size<T> &other) { return this->w == other.w && this->h == other.h; }
};


template<typename Ost, typename T> inline Ost &operator<<(Ost &ost, Size<T> size)
{
    ost << "Size{" << size.w << ", " << size.h << "}";
    return ost;
}
    
}//namespace r64fx

#endif//R64FX_GUI_SIZE_H