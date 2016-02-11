#ifndef R64FX_IMAGE_UTILS_HPP
#define R64FX_IMAGE_UTILS_HPP

#include "Image.hpp"
#include "Color.hpp"
#include "Rect.hpp"
#include "Transform2D.hpp"
#include <initializer_list>

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


inline void fill(Image* dst, unsigned char px)
{
    fill(dst, &px, 1);
}

/* Blend a single color into a destination image using an alpha mask. */
void alpha_blend(Image* dst, Point<int> pos, Color<unsigned char> color, Image* alpha);

/* Copy source image into destination one. Source image is clipped if needed. */
void implant(Image* dst, Point<int> pos, Image* src);


enum class BilinearCopyMode{
    Replace,
    AddWithSaturation,
    Max,
    Average
};

void bilinear_copy(
    Image* dst,
    Image* src,
    Rect<int> rect,
    const Transform2D<float> &transform,
    const BilinearCopyMode mode,
    unsigned char* bg_components, int ncomponents
);



enum class LineCapStyle{
    Square,
    Triangle,
    Round
};

void draw_line(
    Image* dst,
    Point<float> a, Point<float> b,
    int thickness,
    LineCapStyle cap_style,
    unsigned char* fg_components, unsigned char* bg_components, int ncomponents
);


void draw_lines(
    Image* dst,
    Point<float>* points, int npoints,
    int thickness,
    LineCapStyle cap_style,
    unsigned char* fg_components, unsigned char* bg_components, int ncomponents
);

}//namespace r64fx

#endif//R64FX_IMAGE_UTILS_HPP