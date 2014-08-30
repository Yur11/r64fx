#ifndef R64FX_IMAGE_H
#define R64FX_IMAGE_H

#include "Pixel.hpp"

namespace r64fx{
    
template<typename PixelT>
class Image{
    unsigned int _width;
    unsigned int _height;
    PixelT* buff = nullptr;
    
public:
    Image(unsigned int width, unsigned int height)
    : _width(width)
    , _height(height)
    {
        buff = new PixelT[width * height];
    }
    
    inline void free() { delete[] buff; buff = nullptr; }
    
    inline unsigned int width() const { return _width; }
    
    inline unsigned int height() const { return _height; }
    
    inline PixelT &at(unsigned int x, unsigned int y) { return buff[y*width() + x]; }
    
    inline PixelT &operator()(unsigned int x, unsigned int y) { return at(x, y); }
    
    inline PixelT* data() const { return buff; }
    
    static unsigned int componentCount() { return PixelT::componentCount(); }
};
    
}//namespace r64fx

#endif//R64FX_IMAGE_H