#ifndef R64FX_IMAGE_UTILS_HPP
#define R64FX_IMAGE_UTILS_HPP

#include "Image.hpp"
#include "Color.hpp"
#include "Rect.hpp"
#include "Transform2D.hpp"

namespace r64fx{

void draw_rect(Image* dst, Color<unsigned char> color, Rect<int> rect);

void draw_border(Image* dst, Color<unsigned char> color);

void fill(Image* dst, unsigned char* components, int ncomponents, Rect<int> rect);

inline void fill(Image* dst, Color<unsigned char> color, Rect<int> rect)
{
    fill(dst, color.vec, 4, rect);
}

void fill(Image* dst, unsigned char* components, int ncomponents);

inline void fill(Image* dst, Color<unsigned char> color)
{
    fill(dst, color.vec, 4);
}

/* Blend a single color into a destination image using an alpha mask. */
void alpha_blend(Image* dst, Point<int> pos, Color<unsigned char> color, Image* alpha);

/* Copy source image into destination one. Source image is clipped if needed. */
void implant(Image* dst, Point<int> pos, Image* src);


void bilinear_copy(
    Image* dst,
    Image* src,
    const Transform2D<float> &transform,
    unsigned char* bg_components, int ncomponents
);


void draw_line(
    Image* dst,
    Point<float> a, Point<float> b,
    int thickness,
    unsigned char* fg_components, unsigned char* bg_components, int ncomponents
);

}//namespace r64fx

#endif//R64FX_IMAGE_UTILS_HPP