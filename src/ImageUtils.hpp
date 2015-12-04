#ifndef R64FX_IMAGE_UTILS_HPP
#define R64FX_IMAGE_UTILS_HPP

#include "Image.hpp"
#include "Color.hpp"
#include "Rect.hpp"

namespace r64fx{

void draw_rect(Image* dst, Color<unsigned char> color, Rect<int> rect);

void draw_border(Image* dst, Color<unsigned char> color);

/** @brief Blend a single color into a destination image using an alpha mask. */
void alpha_blend(Image* dst, Point<int> pos, Color<unsigned char> color, Image* alpha);

}//namespace r64fx

#endif//R64FX_IMAGE_UTILS_HPP