#include "ImageUtils.hpp"
#include <limits>
#include <vector>

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

#include <iostream>
using namespace std;


namespace r64fx{

namespace{

constexpr float rcp255 = 1.0f / 255.0f;

inline unsigned char mix_colors(unsigned char c1, float f1, unsigned char c2, float f2)
{
    float v1 = float(c1) * rcp255;
    float v2 = float(c2) * rcp255;
    return (unsigned char)((v1*f1 + v2*f2) * 255.0f);
}

inline unsigned char mix_colors_accurate(unsigned char c1, float f1, unsigned char c2, float f2)
{
    float v1 = float(c1) * rcp255;
    float v2 = float(c2) * rcp255;
    v1 = v1 * v1;
    v2 = v2 * v2;
    return (unsigned char)(sqrt(v1*f1 + v2*f2) * 255.0f);
}

}//namespace


ImgRect::ImgRect(Image* img, const Rect<int> &rect)
: img(img)
, rect(rect)
{
#ifdef R64FX_DEBUG
    assert(img != nullptr);
    assert(rect.left() >= 0);
    assert(rect.top() >= 0);
    assert(rect.right() <= img->width());
    assert(rect.bottom() <= img->height());
#endif//R64FX_DEBUG
}


ImgPos::ImgPos(Image* img, const Point<int> &pos)
: img(img)
, pos(pos)
{

}

}//namespace r64fx

#include "ImageCopy.cxx"

namespace r64fx{

void fill(const ImgRect &dst, Color components)
{
#ifdef R64FX_DEBUG
    assert(dst.img != nullptr);
    assert(components != nullptr);
#endif//R64FX_DEBUG

    for(int y=0; y<dst.rect.height(); y++)
    {
        for(int x=0; x<dst.rect.width(); x++)
        {
            auto px = dst.img->pixel(x + dst.rect.x(), y + dst.rect.y());
            for(int c=0; c<dst.img->componentCount(); c++)
            {
                px[c] = components[c];
            }
        }
    }
}


void fill(const ImgRect &dst, int dstc, int ndstc, unsigned char value)
{
#ifdef R64FX_DEBUG
    assert(dst.img != nullptr);
    assert((dstc + ndstc) <= dst.img->componentCount());
#endif//R64FX_DEBUG

    for(int y=0; y<dst.rect.height(); y++)
    {
        for(int x=0; x<dst.rect.width(); x++)
        {
            auto px = dst.img->pixel(x + dst.rect.x(), y + dst.rect.y());
            for(int c=0; c<ndstc; c++)
            {
                px[dstc + c] = value;
            }
        }
    }
}


void fill_gradient_vert(Image* dst, int dstc, int ndstc, unsigned char val1, unsigned char val2, const Rect<int> &rect)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(rect.x() >= 0);
    assert(rect.y() >= 0);
    assert(rect.right()  <= dst->width());
    assert(rect.bottom() <= dst->height());
    assert((dstc + ndstc) <= dst->componentCount());
#endif//R64FX_DEBUG

    float bc = float(val1) * rcp255;
    float ec = float(val2) * rcp255;
    float cd = ec - bc;
#ifdef R64FX_DEBUG
    assert(!(cd > 255.0f || cd < -255.0f));
#endif//R64FX_DEBUG

    if(rect.height() <= 0)
        return;
    float cs = cd / float(rect.height());

    float color = bc;
    for(int y=rect.y(); y<rect.bottom(); y++)
    {
        for(int x=rect.x(); x<rect.right(); x++)
        {
            auto px = dst->pixel(x, y);
            for(int c=0; c<ndstc; c++)
            {
                px[dstc + c] = (unsigned char)(color * 255.0f);
            }
        }
        color += cs;
    }
}


void fill_gradient_radial(Image* dst, int dstc, int ndstc, unsigned char val1, unsigned char val2, const Rect<int> &rect)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(rect.x() >= 0);
    assert(rect.y() >= 0);
    assert(rect.right()  <= dst->width());
    assert(rect.bottom() <= dst->height());
    assert((dstc + ndstc) <= dst->componentCount());
    assert(val1 < val2);
#endif//R64FX_DEBUG

    float bc = float(val1) * rcp255;
    float ec = float(val2) * rcp255;
    float cd = ec - bc;
    int cx = rect.width() / 2 + rect.x();
    int cy = rect.height() / 2 + rect.y();

    float ang1 = 0.0f;
    float ang2 = 2.0f * M_PI;
    float ang_rcp = 1.0f / (ang2 - ang1);

    for(int y=rect.y(); y<rect.bottom(); y++)
    {
        for(int x=rect.x(); x<rect.right(); x++)
        {
            float pxang = atan2(x - cx, cy - y) + M_PI;
            float val = 0.0f;
            if(pxang >= ang1 && pxang <= ang2)
            {
                val = (pxang - ang1) * ang_rcp * cd;
            }

            auto px = dst->pixel(x, y);
            for(int c=0; c<ndstc; c++)
            {
                px[dstc + c] = (unsigned char)(val * 255.0f);
            }
        }
    }
}


void fill_circle(const ImgRect &dst, int dstc, int ndstc, Color color, const Point<int> center, float radius)
{
    for(int y=dst.rect.top(); y<dst.rect.bottom(); y++)
    {
        for(int x=dst.rect.left(); x<dst.rect.right(); x++)
        {
            float dx = x - center.x(); dx += 0.5f;
            float dy = y - center.y(); dy += 0.5f;
            float rr = sqrt(dx*dx + dy*dy);
            float dd = radius - rr;
            if(dd < 0.0f)
                dd = 0.0f;
            else if(dd > 1.0f)
                dd = 1.0f;

            if(dd >= 0.0f)
            {
                float alpha = dd;
                float one_minus_alpha = 1.0f - alpha;

                auto px = dst.img->pixel(x, y);

                for(int c=0; c<ndstc; c++)
                {
                    px[c + dstc] = (unsigned char) (float(color[c]) * alpha + float(px[c + dstc]) * one_minus_alpha);
                }
            }
        }
    }
}


void stroke_rect(const ImgRect &dst, Color stroke, Color fill, int stroke_width)
{
    for(int y=0; y<dst.rect.height(); y++)
    {
        for(int x=0; x<dst.rect.width(); x++)
        {
            bool border = (
                (x < stroke_width) ||
                (y < stroke_width) ||
                (x >= dst.rect.width() - stroke_width) ||
                (y >= dst.rect.height() - stroke_width)
            );

            Color &color = (border ? stroke : fill);

            auto px = dst.img->pixel(x + dst.rect.x(), y + dst.rect.y());
            for(int c=0; c<dst.img->componentCount(); c++)
            {
                px[c] = color[c];
            }
        }
    }
}


void blend_colors(Image* dst, Point<int> dstpos, const Colors &colors, const ImgRect &src, FlipFlags flags)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
#endif//R64FX_DEBUG

    FlippedIntersection<int> isec(
        {0, 0, dst->width(), dst->height()}, dstpos, src.rect,
        flags & FlipFlags::Vert(), flags & FlipFlags::Hori(), flags & FlipFlags::Diag()
    );
    if(!isec)
        return;

    for(int y=0; y<isec.dstHeight(); y++)
    {
        for(int x=0; x<isec.dstWidth(); x++)
        {
            int srcx = ((flags & FlipFlags::Diag()) ? y : x) + isec.srcx();
            int srcy = ((flags & FlipFlags::Diag()) ? x : y) + isec.srcy();
            auto srcpx = src.img->pixel(srcx, srcy);

            int dstx = ((flags & FlipFlags::Hori()) ? (flipval<true>(x, isec.dstWidth()))  : x) + isec.dstx();
            int dsty = ((flags & FlipFlags::Vert()) ? (flipval<true>(y, isec.dstHeight())) : y) + isec.dsty();
            auto dstpx = dst->pixel(dstx, dsty);

            for(int m=0; m<src.img->componentCount(); m++)
            {
                float alpha = float(srcpx[m]) * rcp255;
                float one_minus_alpha = 1.0f - alpha;

                for(int c=0; c<dst->componentCount(); c++)
                {
                    dstpx[c] = mix_colors(dstpx[c], one_minus_alpha, colors[m][c], alpha);
                }
            }
        }
    }
}


void flip_vert(Image* img)
{
#ifdef R64FX_DEBUG
    assert(img != nullptr);
#endif//R64FX_DEBUG

    int hh = img->height() / 2;

    for(int y=0; y<hh; y++)
    {
        for(int x=0; x<img->width(); x++)
        {
            auto px1 = img->pixel(x, y);
            auto px2 = img->pixel(x, img->height() - y - 1);
            for(int c=0; c<img->componentCount(); c++)
            {
                unsigned char tmp = px1[c];
                px1[c] = px2[c];
                px2[c] = tmp;
            }
        }
    }
}


void flip_hori(Image* img)
{
#ifdef R64FX_DEBUG
    assert(img != nullptr);
#endif//R64FX_DEBUG

    int hw = img->width() / 2;

    for(int y=0; y<img->height(); y++)
    {
        for(int x=0; x<hw; x++)
        {
            auto px1 = img->pixel(x, y);
            auto px2 = img->pixel(img->width() - x - 1, y);
            for(int c=0; c<img->componentCount(); c++)
            {
                unsigned char tmp = px1[c];
                px1[c] = px2[c];
                px2[c] = tmp;
            }
        }
    }
}


void mirror_left2right(Image* img, ImgCopyFlags pixop)
{
#ifdef R64FX_DEBUG
    assert(img != nullptr);
#endif//R64FX_DEBUG

    int hw = img->width() / 2;

    for(int y=0; y<img->height(); y++)
    {
        for(int x=0; x<hw; x++)
        {
            auto srcpx = img->pixel(x, y);
            auto dstpx = img->pixel(img->width() - x - 1, y);
            shuf_components<R64FX_IMGOP_REPLACE>(UnpackPixopChanShuf(pixop.bits(), img, img), dstpx, srcpx);
        }
    }
}


void invert(Image* dst, Image* src)
{
#ifdef R64FX_DEBUG
    assert(dst->width() == src->width());
    assert(dst->height() == src->height());
    assert(dst->componentCount() == src->componentCount());
#endif//R64FX_DEBUG

    for(int y=0; y<dst->height(); y++)
    {
        for(int x=0; x<dst->width(); x++)
        {
            for(int c=0; c<dst->componentCount(); c++)
            {
                dst->pixel(x, y)[c] = 255 - src->pixel(x, y)[c];
            }
        }
    }
}


void threshold(Image* dst, Image* src, Color below_or_eq, Color above, unsigned char threshold)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(src != nullptr);
    assert(dst->width() == src->width());
    assert(dst->height() == src->height());
    assert(dst->componentCount() == src->componentCount());
#endif//R64FX_DEBUG

    for(int y=0; y<dst->height(); y++)
    {
        for(int x=0; x<dst->width(); x++)
        {
            auto dstpx = dst->pixel(x, y);
            auto srcpx = src->pixel(x, y);

            for(int c=0; c<dst->componentCount(); c++)
            {
                dstpx[c] = (srcpx[c] <= threshold ? below_or_eq[c] : above[c]);
            }
        }
    }
}


bool cmppixel(unsigned char* a, unsigned char* b, int component_count)
{
    for(int c=0; c<component_count; c++)
    {
        if(a[c] != b[c])
            return false;
    }
    return true;
}


Rect<int> fit_content(ImgRect img, Color nullpixel)
{
    Rect<int> rect = img.rect;
    
    int running = 1;
    while(running && rect.width() > 0)
    {
        for(int y=rect.top(); y<rect.bottom(); y++)
        {
            if(!cmppixel(img->pixel(rect.left(), y), nullpixel, img->componentCount()))
            {
                running = 0;
                break;
            }
        }
        rect.setLeft(rect.left() + running);
    }

    running = 1;
    while(running && rect.height() > 0)
    {
        for(int x=rect.left(); x<rect.right(); x++)
        {
            if(!cmppixel(img->pixel(x, rect.top()), nullpixel, img->componentCount()))
            {
                running = false;
                break;
            }
        }
        rect.setTop(rect.top() + running);
    }

    running = 1;
    while(running && rect.width() > 1)
    {
        for(int y=rect.top(); y<rect.bottom(); y++)
        {
            if(!cmppixel(img->pixel(rect.right()-1, y), nullpixel, img->componentCount()))
            {
                running = false;
                break;
            }
        }
        rect.setRight(rect.right() - running);
    }

    running = 1;
    while(running && rect.height() > 1)
    {
        for(int x=rect.left(); x<rect.right(); x++)
        {
            if(!cmppixel(img->pixel(x, rect.bottom()-1), nullpixel, img->componentCount()))
            {
                running = false;
                break;
            }
        }
        rect.setBottom(rect.bottom() - running);
    }

    return rect;
}


void draw_triangles(int size, Image* up, Image* down, Image* left, Image* right)
{
    Image img(size, size);

    fill(&img, Color(0));

    int half_width = size / 2;
    int triangle_height = sqrt(size * size - half_width * half_width) - 1;
    float slope = float(half_width) / float(triangle_height);
    for(int y=0; y<triangle_height; y++)
    {
        float threshold = y * slope;
        for(int x=0; x<=half_width; x++)
        {
            float distance = half_width - x;
            float diff = distance - threshold;
            unsigned char px;
            if(diff < 0.0f)
                px = 255;
            else if(diff < 1.0f)
                px = 255 - (unsigned char)(diff * 255.0f);
            else
                px = 0;
            img.pixel(x, y)[0] = img.pixel(size - x - 1, y)[0] = px;
        }
    }

    if(up)
    {
        up->load(size, size, 1);
        for(int i=0; i<size; i++)
        {
            for(int j=0; j<size; j++)
            {
                up->pixel(i, j)[0] = img(i, j)[0];
            }
        }
    }

    if(down)
    {
        down->load(size, size, 1);
        for(int i=0; i<size; i++)
        {
            for(int j=0; j<size; j++)
            {
                down->pixel(i, size - j - 1)[0] = img(i, j)[0];
            }
        }
    }

    if(left)
    {
        left->load(size, size, 1);
        for(int i=0; i<size; i++)
        {
            for(int j=0; j<size; j++)
            {
                left->pixel(j, size - i - 1)[0] = img(i, j)[0];
            }
        }
    }

    if(right)
    {
        right->load(size, size, 1);
        for(int i=0; i<size; i++)
        {
            for(int j=0; j<size; j++)
            {
                right->pixel(size - j - 1, i)[0] = img(i, j)[0];
            }
        }
    }
}


void fill_bottom_triangle(Image* dst, int dstc, int ndstc, Color color, Point<int> square_pos, int square_size)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
#endif//R64FX_DEBUG

    for(int y=0; y<(square_size/2); y++)
    {
        for(int x=0; x<(square_size/2); x++)
        {
            float alpha = 1.0f;
            if(x >= y)
                alpha = 0.0f;
            float one_minus_alpha = 1.0f - alpha;

            auto px1 = dst->pixel(x,                   square_size - y - 1);
            auto px2 = dst->pixel(square_size - x - 1, square_size - y - 1);
            for(int c=0; c<ndstc; c++)
            {
                px1[dstc + c] = (unsigned char)(float(px1[dstc + c]) * alpha + float(color[c] * one_minus_alpha));
                px2[dstc + c] = (unsigned char)(float(px2[dstc + c]) * alpha + float(color[c] * one_minus_alpha));
            }
        }
    }
}


template<typename T> inline float denormalize(T num)
{
    constexpr float rcp = 1.0f / float(std::numeric_limits<T>::max());
    return float(num) * rcp;
}

template<> inline float denormalize(float num)
{
    return num;
}


template<typename T> void draw_waveform(Image* dst, Color color, T* data, const Rect<int> &rect)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(color != nullptr);
    assert(rect.left() >= 0);
    assert(rect.top() >= 0);
    assert(rect.right() <= dst->width());
    assert(rect.bottom() <= dst->height());
#endif//R64FX_DEBUG

    float height_rcp = 1.0f / rect.height();

    for(int y=0; y<rect.height(); y++)
    {
        float yy = y * height_rcp;

        for(int x=0; x<rect.width(); x++)
        {
            unsigned char* dstpx = dst->pixel(x + rect.x(), y + rect.y());
            float min_value = denormalize<T>(data[x*2]);
            float max_value = denormalize<T>(data[x*2 + 1]);
            if(yy > min_value && yy < max_value)
            {
                for(auto c=0; c<dst->componentCount(); c++)
                {
                    dstpx[c] = color[c];
                }
            }
        }
    }
}


void draw_waveform(Image* dst, Color color, unsigned char* data, const Rect<int> &rect)
{
    draw_waveform<unsigned char>(dst, color, data, rect);
}


void draw_waveform(Image* dst, Color color, unsigned short* data, const Rect<int> &rect)
{
    draw_waveform<unsigned short>(dst, color, data, rect);
}


void draw_waveform(Image* dst, Color color, unsigned int* data, const Rect<int> &rect)
{
    draw_waveform<unsigned int>(dst, color, data, rect);
}


void draw_waveform(Image* dst, Color color, float* data, const Rect<int> &rect)
{
    draw_waveform<float>(dst, color, data, rect);
}


struct PlotPoint{
    float minval   = 0.0f;
    float maxval   = 0.0f;
    float minblend = 0.0f;
    float maxblend = 0.0f;
};

void stroke_plot(Image* img, Color color, Rect<int> rect, float* data, float thickness, float scale, float offset)
{
    if(!img || !data || !color || rect.width() <= 0 || rect.height() <= 0 || thickness <= 0.0f || scale <= 0.0f)
        return;

    float half_thickness = thickness * 0.5;

    /* Find extrema. */
    struct Extremum{
        int index = 0;
        bool is_minimum = true;

        Extremum(int index, bool is_minimum) : index(index), is_minimum(is_minimum) {}
    };

    vector<Extremum> extrema;
    extrema.push_back({0, true});
    for(int i=0; i<rect.width(); i++)
    {
        if(i > 0 && data[i - 1] >= data[i] && (i + 1) < rect.width() && data[i] <= data[i + 1])
        {
            extrema.push_back({i, true});
        }
        else if(i > 0 && data[i - 1] <= data[i] && (i + 1) < rect.width() && data[i] >= data[i + 1])
        {
            extrema.push_back({i, false});
        }
    }

    if(extrema.size() == 1)
    {
        extrema[0].is_minimum = (data[0] < data[rect.width() - 1]);
        extrema.push_back({int(rect.width()) - 1, !extrema[0].is_minimum});
    }
    else if(extrema.size() > 1)
    {
        extrema[0].is_minimum = !extrema[1].is_minimum;
        if(extrema.back().index < (rect.width() - 1))
        {
            extrema.push_back({int(rect.width()) - 1, !extrema.back().is_minimum});
        }
    }
    else
    {
        return;
    }

    /* Generate line. */
    vector<PlotPoint> pvec(rect.width());
    int curr_extremum = 0;
    float minimum = 0.0f;
    float maximum = 0.0f;
    for(int i=0; i<rect.width(); i++)
    {
        if(i < (rect.width() - 1) && i == extrema[curr_extremum].index)
        {
            if(extrema[curr_extremum].is_minimum)
            {
                minimum = data[extrema[curr_extremum + 0].index] - half_thickness;
                maximum = data[extrema[curr_extremum + 1].index] + half_thickness;
            }
            else
            {
                minimum = data[extrema[curr_extremum + 1].index] - half_thickness;
                maximum = data[extrema[curr_extremum + 0].index] + half_thickness;
            }
            curr_extremum++;
        }

        float dy1;
        float extra1;
        {
            dy1 = 0.0f;
            if(i > 0)
            {
                dy1 = data[i] - data[i - 1];
            }
            extra1 = sqrt(dy1 * dy1 + 1) * half_thickness;
        }

        float dy2;
        float extra2;
        {
            dy2 = 0.0f;
            if((i + 1) < rect.width())
            {
                dy2 = data[i + 1] - data[i];
            }
            extra2 = sqrt(dy2 * dy2 + 1) * half_thickness;
        }

        PlotPoint pp;
        if(dy1 < 0)
        {
            if(dy2 < 0)
            {
                pp.minval = data[i] - extra2;
                pp.maxval = data[i] + extra1;
            }
            else
            {
                pp.minval = data[i] - (extra1 + extra2) * 0.5f;
                pp.maxval = data[i] + (extra1 + extra2) * 0.5f;
            }
        }
        else
        {
            if(dy2 >=0)
            {
                pp.minval = data[i] - extra1;
                pp.maxval = data[i] + extra2;
            }
            else
            {
                pp.minval = data[i] - (extra1 + extra2) * 0.5f;
                pp.maxval = data[i] + (extra1 + extra2) * 0.5f;
            }
        }

        pp.minval = int(max(pp.minval, minimum) * scale + offset + 0.5f);
        pp.maxval = int(min(pp.maxval, maximum) * scale + offset - 0.5f);
        pvec[i] = pp;
    }

    for(int i=0; i<rect.width(); i++)
    {
        pvec[i].minblend = pvec[i].minval;
        pvec[i].maxblend = pvec[i].maxval;
    }


    /* Blend top part. */
    int segment_length = 0;
    int curr_y = pvec[0].minval;
    int prev_y = curr_y - 1;
    for(int i=0; i<rect.width(); i++)
    {
        segment_length++;
        if((i + 1) == rect.width() || int(pvec[i].minval) != int(pvec[i + 1].minval))
        {
            int curr_y = pvec[i].minval;

            int next_y;
            if((i + 1) < rect.width())
            {
                next_y = pvec[i + 1].minval;
            }
            else
            {
                next_y = curr_y - 1;
            }

            if(segment_length > 1)
            {
                float rcp = 1.0f / float(segment_length + 1);

                if(next_y < curr_y)
                {
                    if(prev_y > curr_y)
                    {
                        for(int j=0; j<segment_length; j++)
                        {
                            int x = j + i - segment_length + 1;
                            pvec[x].minblend = curr_y - (j + 1) * rcp;
                        }
                    }
                    else
                    {
                        if(segment_length >= 3)
                        {
                            for(int j=0; j<(segment_length>>1); j++)
                            {
                                int x1 = j + i - segment_length + 1;
                                int x2 = i - j;
                                pvec[x1].minblend = pvec[x2].minblend
                                    = curr_y - (segment_length - j) * rcp;
                            }

                            if(segment_length & 1)
                            {
                                int x = i - (segment_length >> 1);
                                pvec[x].minblend = pvec[x + 1].minblend;
                            }
                        }
                        else
                        {
                            for(int j=0; j<segment_length; j++)
                            {
                                int x = j + i - segment_length + 1;
                                pvec[x].minblend = curr_y - 0.5f;
                            }
                        }
                    }
                }
                else if(prev_y < curr_y)
                {
                    for(int j=0; j<segment_length; j++)
                    {
                        int x = j + i - segment_length + 1;
                        pvec[x].minblend = curr_y - (segment_length - j) * rcp;
                    }
                }
            }
            else // segment_length == 1
            {
                if(next_y < curr_y)
                {
                    if(prev_y > curr_y)
                    {
                        if((curr_y - next_y) == 1)
                        {
                            pvec[i].minblend = curr_y - 0.5f;
                        }
                        else
                        {
                            pvec[i].minblend = next_y;
                        }
                    }
                }
                else if(prev_y < curr_y)
                {
                    if((curr_y - prev_y) == 1)
                    {
                        pvec[i].minblend = curr_y - 0.5f;
                    }
                    else
                    {
                        pvec[i].minblend = prev_y;
                    }
                }
            }

            segment_length = 0;
            prev_y = curr_y;
        }
    }


    /* Blend bottom part. */
    segment_length = 0;
    curr_y = pvec[0].minval;
    prev_y = curr_y - 1;
    for(int i=0; i<rect.width(); i++)
    {
        segment_length++;
        if((i + 1) == rect.width() || int(pvec[i].maxval) != int(pvec[i + 1].maxval))
        {
            int curr_y = pvec[i].maxval;

            int next_y;
            if((i + 1) < rect.width())
            {
                next_y = pvec[i + 1].maxval;
            }
            else
            {
                next_y = curr_y - 1;
            }

            if(segment_length > 1)
            {
                float rcp = 1.0f / (segment_length + 1);

                if(next_y > curr_y)
                {
                    if(prev_y < curr_y)
                    {
                        for(int j=0; j<segment_length; j++)
                        {
                            int x = j + i - segment_length + 1;
                            pvec[x].maxblend = curr_y + (j + 1) * rcp;
                        }
                    }
                    else
                    {
                        if(segment_length >= 3)
                        {
                            for(int j=0; j<(segment_length>>1); j++)
                            {
                                int x1 = j + i - segment_length + 1;
                                int x2 = i - j;
                                pvec[x1].maxblend = pvec[x2].maxblend
                                    = curr_y + (segment_length - j) * rcp;
                            }

                            if(segment_length & 1)
                            {
                                int x = i - (segment_length >> 1);
                                pvec[x].maxblend = pvec[x + 1].maxblend;
                            }
                        }
                        else
                        {
                            for(int j=0; j<segment_length; j++)
                            {
                                int x = j + i - segment_length + 1;
                                pvec[x].maxblend = curr_y + 0.5f;
                            }
                        }
                    }
                }
                else if(prev_y > curr_y)
                {
                    for(int j=0; j<segment_length; j++)
                    {
                        int x = j + i - segment_length + 1;
                        pvec[x].maxblend = curr_y + (segment_length - j) * rcp;
                    }
                }
            }
            else // segment_length == 1
            {
                if(next_y > curr_y)
                {
                    if(prev_y < curr_y)
                    {
                        if((next_y - curr_y) == 1)
                        {
                            pvec[i].maxblend = curr_y + 0.5f;
                        }
                        else
                        {
                            pvec[i].maxblend = next_y;
                        }
                    }
                }
                else if(prev_y > curr_y)
                {
                    if((prev_y - curr_y) == 1)
                    {
                        pvec[i].maxblend = curr_y + 0.5f;
                    }
                    else
                    {
                        pvec[i].maxblend = prev_y;
                    }
                }
            }

            segment_length = 0;
            prev_y = curr_y;
        }
    }


    for(int x=0; x<rect.width(); x++)
    {
        auto &pp = pvec[x];
        for(int y=0; y<rect.height(); y++)
        {
            if(y < pp.minval)
            {
                float diff = pp.minval - pp.minblend;
                if(diff > 0.0f)
                {
                    if(diff <= 1.0f)
                    {
                        if((y + 1) == pp.minval)
                        {
                            img->pixel(x + rect.x(), y + rect.y())[0] = (unsigned char)(255.0f * diff);
                        }
                    }
                    else
                    {
                        float rcp = 1.0f / (diff + 1);
                        float val = 1.0f - (pp.minblend - y + diff) * rcp;
                        if(val > 0.0f && val < 1.0f)
                        {
                            img->pixel(x + rect.x(), y + rect.y())[0] = (unsigned char)(255.0f * val);
                        }
                    }
                }
            }
            else if(y <= pp.maxval)
            {
                img->pixel(x + rect.x(), y + rect.y())[0] = 255;
            }
            else
            {
                float diff = pp.maxblend - pp.maxval;
                if(diff >= 0.0f)
                {
                    if(diff <= 1.0f)
                    {
                        if((y - 1) == pp.maxval)
                        {
                            img->pixel(x + rect.x(), y + rect.y())[0] = (unsigned char)(255.0f * diff);
                        }
                    }
                    else
                    {
                        float rcp = 1.0f / (diff + 1);
                        float val = 1.0f - (y - pp.maxblend + diff) * rcp;
                        if(val > 0.0f && val < 1.0f)
                        {
                            img->pixel(x + rect.x(), y + rect.y())[0] = (unsigned char)(255.0f * val);
                        }
                    }
                }
            }
        }
    }
}

}//namespace r64fx
