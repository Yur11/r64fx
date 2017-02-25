#ifndef R64FX_IMAGE_UTILS_HPP
#define R64FX_IMAGE_UTILS_HPP

#include "Image.hpp"
#include "Rect.hpp"
#include "Transformation2D.hpp"
#include "Color.hpp"

namespace r64fx{

void fill(Image* dst, unsigned char* components, Rect<int> rect);

void fill(Image* dst, unsigned char* components);


void fill_component(Image* dst, int component, unsigned char value, Rect<int> rect);

void fill_component(Image* dst, int component, unsigned char value);


void fill_gradient_vert(Image* img, unsigned char begin_val, unsigned char end_val, int component, int component_count, Rect<int> rect);

void fill_gradient_vert(Image* img, unsigned char begin_val, unsigned char end_val, int component, int component_count);


void copy(Image* dst, Point<int> dstpos, Image* src, Rect<int> src_rect);

void copy(Image* dst, Point<int> dstpos, Image* src);


void copy_component(Image* dst, int dst_component, Point<int> dstpos, Image* src, int src_component, Rect<int> src_rect);

void copy_component(Image* dst, int dst_component, Point<int> dstpos, Image* src, int src_component);


void copy_rgba(Image* dst, Point<int> dstpos, Image* src, Rect<int> src_rect);

void copy_rgba(Image* dst, Point<int> dstpos, Image* src);


void blend_colors(Image* dst, Point<int> dstpos, unsigned char** colors, Image* src, Rect<int> src_rect);

void blend_colors(Image* dst, Point<int> dstpos, unsigned char** colors, Image* src);


void copy_transformed(Image* dst, Transformation2D<float> transform, Image* src, Rect<int> dst_rect);

void copy_transformed(Image* dst, Transformation2D<float> transform, Image* src);


void flip_vert(Image* img);

void flip_hori(Image* img);


void invert(Image* dst, Image* src);


void combine(Image* dst, Image* src1, Point<int> pos1, Image* src2, Point<int> pos2);


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


void draw_waveform(Image* dst, unsigned char* color, unsigned char*  data, const Rect<int> &rect);

void draw_waveform(Image* dst, unsigned char* color, unsigned short* data, const Rect<int> &rect);

void draw_waveform(Image* dst, unsigned char* color, unsigned int*   data, const Rect<int> &rect);

void draw_waveform(Image* dst, unsigned char* color, float*          data, const Rect<int> &rect);


void stroke_circle(Image* dst, unsigned char* color, Point<float> center, float radius, float thickness);


void fill_circle(Image* dst, unsigned char* color, Point<float> center, float radius);


void subtract_image(Image* dst, Point<int> pos, Image* src);


void fill_rounded_rect(Image* dst, unsigned char* color, Rect<int> rect, int corner_radius);


void stroke_plot(
    Image* img, unsigned char* color, Rect<int> rect, float* data, float thickness, float scale = 1.0f, float offset = 0.0f
);

}//namespace r64fx

#endif//R64FX_IMAGE_UTILS_HPP
