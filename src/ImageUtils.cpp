#include "ImageUtils.hpp"

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

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


void alpha_blend(Image* dst, Point<int> pos, Color<unsigned char> color, Image* alpha)
{
#ifdef R64FX_DEBUG
    assert(alpha->channelCount() == 1);
    assert(pos.x() >= 0);
    assert(pos.y() >= 0);
    assert((pos.x() + alpha->width())  < dst->width());
    assert((pos.y() + alpha->height()) < dst->height());
#endif//R64FX_DEBUG

    static const float rcp = 1.0f / float(255);
    float c[3] = {
        float(color.red())   * rcp,
        float(color.green()) * rcp,
        float(color.blue())  * rcp
    };

    for(int y=0; y<alpha->height(); y++)
    {
        for(int x=0; x<alpha->width(); x++)
        {
            float src_alpha            = float(      alpha->pixel(x, y)[0]) * rcp;
            float one_minus_src_alpha  = float(255 - alpha->pixel(x, y)[0]) * rcp;
            auto px = dst->pixel(x + pos.x(), y + pos.y());
            for(int i=0; i<3; i++)
            {
                float result = float(px[i]) * rcp * one_minus_src_alpha;
                result += c[i] * src_alpha;
                px[i] = (unsigned char)(result * 255);
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

}//namespace r64fx