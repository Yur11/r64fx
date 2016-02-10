#include "ImageUtils.hpp"

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

#include <iostream>

using namespace std;

namespace r64fx{

void draw_rect(Image* dst, Color<unsigned char> color, Rect<int> rect)
{
#ifdef R64FX_DEBUG
    assert(dst->channelCount() <= 4);
#endif//R64FX_DEBUG

    for(int x=0; x<rect.width(); x++)
    {
        auto px1 = dst->pixel(x + rect.x(), rect.y());
        auto px2 = dst->pixel(x + rect.x(), rect.y() + rect.height() - 1);
        for(int c=0; c<dst->channelCount(); c++)
            px1[c] = px2[c] = color.vec[c];
    }

    for(int y=1; y<rect.height()-1; y++)
    {
        auto px1 = dst->pixel(rect.x(),                    y + rect.y());
        auto px2 = dst->pixel(rect.x() + rect.width() - 1, y + rect.y());
        for(int c=0; c<dst->channelCount(); c++)
            px1[c] = px2[c] = color.vec[c];
    }
}


void draw_border(Image* dst, Color<unsigned char> color)
{
    draw_rect(dst, color, {0, 0, dst->width(), dst->height()});
}


void fill(Image* dst, unsigned char* components, int ncomponents, Rect<int> rect)
{
#ifdef DEBUG
    assert(dst->channelCount() >= ncomponents);
#endif//DEBUG

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            for(int c=0; c<ncomponents; c++)
            {
                dst->pixel(x + rect.x(), y + rect.y())[c] = components[c];
            }
        }
    }
}


void fill(Image* dst, unsigned char* components, int ncomponents)
{
    fill(dst, components, ncomponents, Rect<int>(0, 0, dst->width(), dst->height()));
}


void alpha_blend(Image* dst, Point<int> pos, Color<unsigned char> color, Image* alpha)
{
#ifdef R64FX_DEBUG
    assert(alpha->channelCount() == 1);
    assert(dst->channelCount() == 3 || dst->channelCount() == 4);
#endif//R64FX_DEBUG

    static const float rcp = 1.0f / float(255);
    float c[3] = {
        float(color.red())   * rcp,
        float(color.green()) * rcp,
        float(color.blue())  * rcp
    };

    Rect<int> dst_rect = {0, 0, dst->width(), dst->height()};
    Rect<int> src_rect = {pos.x(), pos.y(), alpha->width(), alpha->height()};
    Rect<int> rect = intersection(src_rect, dst_rect);

    if(rect.width() == 0 || rect.height() == 0)
        return;

    int src_offset_x = (pos.x() < 0 ? -pos.x() : 0);
    int src_offset_y = (pos.y() < 0 ? -pos.y() : 0);
    int dst_offset_x = (pos.x() > 0 ?  pos.x() : 0);
    int dst_offset_y = (pos.y() > 0 ?  pos.y() : 0);

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            auto dstpx = dst->pixel(x + dst_offset_x, y + dst_offset_y);
            auto srcpx = alpha->pixel(x + src_offset_x, y + src_offset_y);

            float src_alpha            = float(      srcpx[0]) * rcp;
            float one_minus_src_alpha  = float(255 - srcpx[0]) * rcp;
            for(int i=0; i<3; i++)
            {
                float result = float(dstpx[i]) * rcp * one_minus_src_alpha;
                result += c[i] * src_alpha;
                dstpx[i] = (unsigned char)(result * 255);
            }
        }
    }
}


void implant(Image* dst, Point<int> pos, Image* src)
{
#ifdef R64FX_DEBUG
    assert(dst->channelCount() == src->channelCount());
#endif//R64FX_DEBUG

    Rect<int> dst_rect = {0, 0, dst->width(), dst->height()};
    Rect<int> src_rect = {pos.x(), pos.y(), src->width(), src->height()};
    Rect<int> rect = intersection(src_rect, dst_rect);

    int src_offset_x = (pos.x() < 0 ? -pos.x() : 0);
    int src_offset_y = (pos.y() < 0 ? -pos.y() : 0);
    int dst_offset_x = (pos.x() > 0 ?  pos.x() : 0);
    int dst_offset_y = (pos.y() > 0 ?  pos.y() : 0);

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            auto dstpx = dst->pixel(x + dst_offset_x, y + dst_offset_y);
            auto srcpx = src->pixel(x + src_offset_x, y + src_offset_y);
            for(int c=0; c<dst->channelCount(); c++)
            {
                dstpx[c] = srcpx[c];
            }
        }
    }
}


void bilinear_copy(
    Image* dst,
    Image* src,
    const Transform2D<float> &transform,
    unsigned char* bg_components, int ncomponents
)
{
    for(int y=0; y<dst->height(); y++)
    {
        for(int x=0; x<dst->width(); x++)
        {
            Point<float> p(x, y);
            transform(p);

            float x1 = floor(p.x());
            float x2 = x1 + 1;

            float y1 = floor(p.y());
            float y2 = y1 + 1;

            float fracx = x2 - p.x();
            float fracy = y2 - p.y();
            for(int c=0; c<4; c++)
            {
                float p11 = bg_components[c];
                float p12 = bg_components[c];
                float p21 = bg_components[c];
                float p22 = bg_components[c];

                if(x1 >=0 && x1 < src->width() && y1 >=0 && y1 < src->height())
                {
                    p11 = src->pixel(x1, y1)[c];
                }

                if(x1 >=0 && x1 < src->width() && y2 >=0 && y2 < src->height())
                {
                    p12 = src->pixel(x1, y2)[c];
                }

                if(x2 >=0 && x2 < src->width() && y1 >=0 && y1 < src->height())
                {
                    p21 = src->pixel(x2, y1)[c];
                }

                if(x2 >=0 && x2 < src->width() && y2 >=0 && y2 < src->height())
                {
                    p22 = src->pixel(x2, y2)[c];
                }

                float val =
                    p22 * (1-fracx) * (1-fracy) +
                    p12 * fracx     * (1-fracy) +
                    p21 * (1-fracx) * fracy     +
                    p11 * fracx     * fracy;
                dst->pixel(x, y)[c] = (unsigned char)(val);
            }
        }
    }
}


void draw_line(
    Image* dst,
    Point<float> a, Point<float> b,
    int thickness,
    unsigned char* fg_components, unsigned char* bg_components, int ncomponents
)
{
#ifdef R64FX_DEBUG
    assert(dst->channelCount() >= ncomponents);
#endif//R64FX_DEBUG

    float dx = b.x() - a.x();
    float dy = b.y() - a.y();
    float length = sqrt(dx*dx + dy*dy);
    float length_rcp = 1.0f / length;
    float cosang = dx * length_rcp;
    float sinang = dy * length_rcp;

    Image src(length, thickness, ncomponents);
    fill(&src, fg_components, ncomponents);

    Transform2D<float> transform;
    transform.translate(a.x(), a.y());
    transform.rotate(sinang, cosang);

    bilinear_copy(dst, &src, transform, bg_components, ncomponents);
}

}//namespace r64fx