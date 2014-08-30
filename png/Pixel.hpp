#ifndef R64FX_PIXEL_H
#define R64FX_PIXEL_H

#include <initializer_list>

namespace r64fx{
    
template<typename T, unsigned int N>
struct Pixel{
    T value[N];
    
    Pixel() {};
    
    Pixel(T value[N]) : value{value} {}
        
    T &operator[](unsigned int n) { return value[n]; }
        
    inline static unsigned int componentCount() { return N; }
};
    
}//namespace r64fx

#endif//R64FX_PIXEL_H
