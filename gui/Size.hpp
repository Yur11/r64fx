#ifndef R64FX_GUI_SIZE_H
#define R64FX_GUI_SIZE_H

namespace r64fx{
  
template<typename T> struct Size{
    T w, h;
    
    Size<T>(T w = T(), T h = T()) 
    : w(w)
    , h(h)
    {}
    
    /** @brief Convert point into size. */
    Size<T>(Point<T> p) : Size<T>(p.x, p.y) {}
    
    inline Point<T> toPoint() const { return Point<T>(w, h); }
    
    inline bool operator==(const Size<T> &other) { return this->w == other.w && this->h == other.h; }
};
    
}//namespace r64fx

#endif//R64FX_GUI_SIZE_H