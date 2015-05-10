#ifndef R64FX_BILINEAR_COPY_HPP
#define R64FX_BILINEAR_COPY_HPP

#include "Image.hpp"
#include "Transform2D.hpp"

namespace r64fx{
  
struct BilinearCopy{
    unsigned char fillr = 0;
    unsigned char fillg = 0;
    unsigned char fillb = 0;
    unsigned char filla = 255;

    void operator()(Image &src, Image &dst, Transform2D<float> transform = {});
};
    
}//namespace r64fx

#endif//R64FX_BILINEAR_COPY_HPP