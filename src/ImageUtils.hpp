#ifndef R64FX_IMAGE_UTILS_HPP
#define R64FX_IMAGE_UTILS_HPP

#include "Image.hpp"
#include "ImageArgs.hpp"
#include "Color.hpp"
#include "FlipFlags.hpp"

namespace r64fx{

class ImgCopyFlags : public FlagBits<ImgCopyFlags>{
public:
    using FlagBits<ImgCopyFlags>::FlagBits;
};

ImgCopyFlags ImgCopyFlipVert();
ImgCopyFlags ImgCopyFlipHori();
ImgCopyFlags ImgCopyFlipDiag();

/* nsrcc must be equal to ndstc or be 1. */
ImgCopyFlags ChanShuf(int dstc, int ndstc, int srcc, int nsrcc);

/* Compatible with ChanShuf(). */
ImgCopyFlags ImgCopyReplace();
ImgCopyFlags ImgCopyAdd();
ImgCopyFlags ImgCopySub();
ImgCopyFlags ImgCopyMul();
ImgCopyFlags ImgCopyMin();
ImgCopyFlags ImgCopyMax();

/* Incompatible with ChanShuf(). */
ImgCopyFlags ImgCopyBlendAlpha();
ImgCopyFlags ImgCopyBlendAlphaAccurate();


void copy(const ImgPos &dst, const ImgRect &src, const ImgCopyFlags flags = ImgCopyBlendAlpha());

void copy(const ImgRect &dst, Transformation2D<float> transform, Image* src, const ImgCopyFlags flags = ImgCopyReplace());



void fill(const ImgRect &dst, Color components);

void fill(const ImgRect &dst, int dstc, int ndstc, unsigned char value);


void fill_gradient_vert(Image* dst, int dstc, int ndstc, unsigned char val1, unsigned char val2, const Rect<int> &rect);

void fill_gradient_radial(Image* dst, int dstc, int ndstc, unsigned char val1, unsigned char val2, const Rect<int> &rect);


void fill_circle(const ImgRect &dst, int dstc, int ndstc, Color color, const Point<int> center, float radius);

// void fill_sector(const ImgRect &dst, int dstc, int ndstc, Color color, const Point<int> center, float min_angle, float max_angle);


void stroke_rect(const ImgRect &dst, Color stroke, Color fill, int stroke_width = 1);


void blend_colors
    (Image* dst, Point<int> dstpos, const Colors &colors, const ImgRect &src, FlipFlags flags = FlipFlags());


void flip_vert(Image* img);

void flip_hori(Image* img);

void mirror_left2right(Image* img, ImgCopyFlags flags = ImgCopyFlags());


void invert(Image* dst, Image* src);

void threshold(Image* dst, Image* src, Color below_or_eq, Color above, unsigned char threshold);


Rect<int> fit_content(ImgRect img, Color nullpixel);


/* Draw a bunch of triangles that point in four different directions
 * and fit in a square with the given size. */
void draw_triangles(int size, Image* up, Image* down, Image* left, Image* right);

void fill_bottom_triangle(Image* dst, int dstc, int ndstc, Color color, Point<int> square_pos, int square_size);


void draw_waveform(Image* dst, Color color, unsigned char*  data, const Rect<int> &rect);

void draw_waveform(Image* dst, Color color, unsigned short* data, const Rect<int> &rect);

void draw_waveform(Image* dst, Color color, unsigned int*   data, const Rect<int> &rect);

void draw_waveform(Image* dst, Color color, float*          data, const Rect<int> &rect);


void stroke_plot(
    Image* img, Color color, Rect<int> rect, float* data, float thickness, float scale = 1.0f, float offset = 0.0f
);

}//namespace r64fx

#endif//R64FX_IMAGE_UTILS_HPP
