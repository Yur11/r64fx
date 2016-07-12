#ifndef R64FX_IMAGE_UTILS_HPP
#define R64FX_IMAGE_UTILS_HPP

#include "Image.hpp"
#include "Rect.hpp"
#include "Transform2D.hpp"

namespace r64fx{

void fill(Image* dst, unsigned char* components, Rect<int> rect);

void fill(Image* dst, unsigned char* components);

void fill(Image* dst, unsigned char value);

void fill(Image* dst, int component, unsigned char value, Rect<int> rect);

void fill(Image* dst, int component, unsigned char value);


void implant(Image* dst, Point<int> pos, Image* src);

void implant(Image* dst, Point<int> dst_offset, Size<int> size, Point<int> src_offset, Image* src);

void implant(Image* dst, const RectIntersection<int> &intersection, Image* src);

void implant(Image* dst, Transform2D<float> transform, Image* src, Rect<int> rect);

void implant(Image* dst, Transform2D<float> transform, Image* src);


void blend(Image* dst, Point<int> pos, unsigned char** colors, Image* mask);

void blend(Image* dst, Point<int> dst_offset, Size<int> size, Point<int> mask_offset, unsigned char** colors, Image* mask);

void blend(Image* dst, const RectIntersection<int> &intersection, unsigned char** colors, Image* mask);

void blend(Image* dst, Point<int> pos, unsigned char* color, Image* mask);

void blend(Image* dst, const RectIntersection<int> &intersection, unsigned char* color, Image* mask);


inline float normalize_angle(float angle)
{
    while(angle > (2.0f * M_PI))
        angle -= (2.0f * M_PI);

    while(angle < 0.0f)
        angle += (2.0f * M_PI);

    return angle;
}

void draw_arc(
    Image* dst, unsigned char* color, Point<float> center, float radius, float arca, float arcb, float thickness, Rect<int> rect
);

void draw_arc(Image* dst, unsigned char* color, Point<float> center, float radius, float arca, float arcb, float thickness);


void draw_line(Image* dst, unsigned char* color, Point<float> a, Point<float> b, float thickness);


void draw_radius(Image* dst, unsigned char* color, Point<float> center, float angle, float outer, float inner, float thickness);



/* Draw a bunch of triangles that point in four different directions
 * and fit in a square with the given size. */
void draw_triangles(int size, Image* up, Image* down, Image* left, Image* right);


void draw_waveform(Image* dst, unsigned char* color, float* data, Rect<int> rect);


void draw_circle(Image* dst, unsigned char* color, Point<int> center, float radius);


void subtract_image(Image* dst, Point<int> pos, Image* src);


void invert_image(Image* dst, Image* src);


void flip_vertically(Image* img);


void flip_horizontally(Image* img);


void fill_rounded_rect(Image* dst, unsigned char* color, Rect<int> rect, int corner_radius);

}//namespace r64fx

#endif//R64FX_IMAGE_UTILS_HPP