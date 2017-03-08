#ifndef R64FX_IMAGE_UTILS_HPP
#define R64FX_IMAGE_UTILS_HPP

#include "Image.hpp"
#include "Rect.hpp"
#include "Transformation2D.hpp"
#include "Color.hpp"

namespace r64fx{

inline const Rect<int> ImgRect(Image* img) { return Rect<int>(0, 0, img->width(), img->height()); }

class PixelOperation{
    unsigned int m_bits;

public:
    PixelOperation(unsigned int bits) : m_bits(bits) {}

    inline unsigned int bits() const { return m_bits; }
};

inline const PixelOperation operator|(const PixelOperation a, const PixelOperation b) { return a.bits() | b.bits(); }

PixelOperation PixOpReplace();
PixelOperation PixOpAdd();
PixelOperation PixOpSub();
PixelOperation PixOpMul();
PixelOperation PixOpMin();
PixelOperation PixOpMax();

PixelOperation ChanShuf(int dstc, int ndstc, int srcc, int nsrcc);


void fill
    (Image* dst, unsigned char* components, const Rect<int> &rect);

inline void fill
    (Image* dst, unsigned char* components)
{
    fill(dst, components, ImgRect(dst));
}


void fill
    (Image* dst, int dstc, int ndstc, unsigned char value, const Rect<int> &rect);

inline void fill
    (Image* dst, int dstc, int ndstc, unsigned char value)
{
    fill(dst, dstc, ndstc, value, ImgRect(dst));
}


void fill_gradient_vert
    (Image* dst, int dstc, int ndstc, unsigned char val1, unsigned char val2, const Rect<int> &rect);

void fill_circle
    (Image* dst,  int dstc, int ndstc, unsigned char* components, Point<int> topleft, int diameter);


void copy
    (Image* dst, Point<int> dstpos, Image* src, const Rect<int> &src_rect, const bool accurate = true);

inline void copy
    (Image* dst, Point<int> dstpos, Image* src, const bool accurate = true)
{
    copy(dst, dstpos, src, ImgRect(src), accurate);
}


void copy
    (Image* dst, Point<int> dstpos, Image* src, const PixelOperation pixop, const Rect<int> &src_rect);

inline void copy
    (Image* dst, Point<int> dstpos, Image* src, const PixelOperation pixop) 
{
    copy(dst, dstpos, src, pixop, ImgRect(src));
}


void copy
    (Image* dst, Transformation2D<float> transform, Image* src, const PixelOperation pixop, Rect<int> dst_rect);

inline void copy
    (Image* dst, Transformation2D<float> transform, Image* src, const PixelOperation pixop)
{
    copy(dst, transform, src, pixop, ImgRect(dst));
}


void blend_colors
    (Image* dst, Point<int> dstpos, unsigned char** colors, Image* src, const Rect<int> &src_rect, const bool accurate = true);

inline void blend_colors
    (Image* dst, Point<int> dstpos, unsigned char** colors, Image* src, const bool accurate = true)
{
    blend_colors(dst, dstpos, colors, src, ImgRect(src), accurate);
}


void flip_vert(Image* img);

void flip_hori(Image* img);

void invert(Image* dst, Image* src);


/* Draw a bunch of triangles that point in four different directions
 * and fit in a square with the given size. */
void draw_triangles(int size, Image* up, Image* down, Image* left, Image* right);

void fill_bottom_triangle(Image* dst, int dstc, int ndstc, unsigned char* color, Point<int> square_pos, int square_size);


void draw_waveform(Image* dst, unsigned char* color, unsigned char*  data, const Rect<int> &rect);

void draw_waveform(Image* dst, unsigned char* color, unsigned short* data, const Rect<int> &rect);

void draw_waveform(Image* dst, unsigned char* color, unsigned int*   data, const Rect<int> &rect);

void draw_waveform(Image* dst, unsigned char* color, float*          data, const Rect<int> &rect);


void stroke_plot(
    Image* img, unsigned char* color, Rect<int> rect, float* data, float thickness, float scale = 1.0f, float offset = 0.0f
);

}//namespace r64fx

#endif//R64FX_IMAGE_UTILS_HPP
