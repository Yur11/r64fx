#ifndef R64FX_IMAGE_UTILS_HPP
#define R64FX_IMAGE_UTILS_HPP

#include "Image.hpp"
#include "Rect.hpp"
#include "Transformation2D.hpp"
#include "Color.hpp"

namespace r64fx{

struct ImgRect{
    Image* img = nullptr;
    Rect<int> rect;

    ImgRect(Image* img, const Rect<int> &rect) : img(img), rect(rect) { crop(); }
    ImgRect(Image* img) : img(img), rect(0, 0, img->width(), img->height()) { crop(); }

private:
    void crop();
};


class PixelOperation{
    unsigned int m_bits;

public:
    PixelOperation(unsigned int bits) : m_bits(bits) {}

    inline unsigned int bits() const { return m_bits; }
};

inline const PixelOperation operator|(const PixelOperation a, const PixelOperation b) { return a.bits() | b.bits(); }

/* nsrcc must be equal to ndstc or be 1. */
PixelOperation ChanShuf(int dstc, int ndstc, int srcc, int nsrcc);

/* Compatible with ChanShuf(). */
PixelOperation PixOpReplace();
PixelOperation PixOpAdd();
PixelOperation PixOpSub();
PixelOperation PixOpMul();
PixelOperation PixOpMin();
PixelOperation PixOpMax();

/* Incompatible with ChanShuf(). */
PixelOperation PixOpBlendAlpha();
PixelOperation PixOpBlendAccurate();


void fill(const ImgRect &dst, unsigned char* components);

void fill(const ImgRect &dst, int dstc, int ndstc, unsigned char value);

void fill_gradient_vert(Image* dst, int dstc, int ndstc, unsigned char val1, unsigned char val2, const Rect<int> &rect);

void fill_gradient_radial(Image* dst, int dstc, int ndstc, unsigned char val1, unsigned char val2, const Rect<int> &rect);

void fill_circle(Image* dst, int dstc, int ndstc, unsigned char* components, Point<int> topleft, int diameter);


void copy
    (Image* dst, Point<int> dstpos, const ImgRect &src);


void copy
    (Image* dst, Point<int> dstpos, const ImgRect &src, const PixelOperation pixop);


void copy
    (const ImgRect &dst, Transformation2D<float> transform, Image* src, const PixelOperation pixop);


void blend_colors
    (Image* dst, Point<int> dstpos, unsigned char** colors, const ImgRect &src, const bool accurate = true);


void threshold(Image* dst, int dstc, int ndstc, Point<int> dstpos, unsigned char* c1, unsigned char* c2, Image* src, unsigned char threshold);


void flip_vert(Image* img);

void flip_hori(Image* img);

void mirror_left2right(Image* img, PixelOperation pixop = 0);

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
