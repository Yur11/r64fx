#ifndef R64FX_IMAGE_ARGS_HPP
#define R64FX_IMAGE_ARGS_HPP

#include "GeometryUtils.hpp"

namespace r64fx{

class Image;

struct ImgRect{
    Image* img = nullptr;
    Rect<int> rect;

    ImgRect(Image* img, const Rect<int> &rect);
    ImgRect(Image* img) : img(img), rect(0, 0, img->width(), img->height()) {}

    inline Image* operator->() { return img; }
};

struct ImgPos{
    Image* img = nullptr;
    Point<int> pos;

    ImgPos(Image* img, const Point<int> &pos);
    ImgPos(Image* img) : img(img), pos(0, 0) {}

    inline Image* operator->() { return img; }
};

}//namespace r64fx

#endif//R64FX_IMAGE_ARGS_HPP
