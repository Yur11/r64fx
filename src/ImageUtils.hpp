#ifndef R64FX_IMAGE_UTILS_HPP
#define R64FX_IMAGE_UTILS_HPP

#include "Image.hpp"
#include "GeometryUtils.hpp"
#include "Color.hpp"

namespace r64fx{

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


class ImgCopyFlags{
    unsigned int m_bits = 0;

public:
    ImgCopyFlags() {}

    ImgCopyFlags(unsigned int bits) : m_bits(bits) {}

    inline unsigned int bits() const { return m_bits; }
};

inline const ImgCopyFlags operator|(const ImgCopyFlags a, const ImgCopyFlags b) { return a.bits() | b.bits(); }

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

void fill_circle(Image* dst, int dstc, int ndstc, Color components, Point<int> topleft, int diameter);


void blend_colors
    (Image* dst, Point<int> dstpos, const Colors &colors, const ImgRect &src, const bool accurate = true);


void flip_vert(Image* img);

void flip_hori(Image* img);

void mirror_left2right(Image* img, ImgCopyFlags flags = 0);


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
