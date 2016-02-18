#ifndef R64FX_IMAGE_ANIMATION_HPP
#define R64FX_IMAGE_ANIMATION_HPP

#include "Image.hpp"

namespace r64fx{

class ImageAnimation : public Image{
    void* m = nullptr;

public:
    ImageAnimation(int w, int h, int c, int nframes);

    ~ImageAnimation();

    int frameCount() const;

    void pickFrame(int i);
};

}//namespace r64fx

#endif//R64FX_IMAGE_ANIMATION_HPP