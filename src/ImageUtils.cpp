#include "ImageUtils.hpp"
#include <algorithm>

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

#include <iostream>
using namespace std;

using namespace std;

namespace r64fx{

struct MatchedRects{
    Point<int> dst_offset;
    Point<int> src_offset;
    Size<int>  size;

    MatchedRects(const Rect<int> &dst_rect, const Rect<int> &src_rect)
    {
        match(dst_rect, src_rect);
    }

    MatchedRects(const Image* dst, const Point<int> &src_pos, const Image* src)
    {
        match(dst, src_pos, src);
    }

    inline void match(const Rect<int> &dst_rect, const Rect<int> &src_rect)
    {
        size = intersection(src_rect, dst_rect).size();
        src_offset.setX(src_rect.x() < 0 ? -src_rect.x() : 0);
        src_offset.setY(src_rect.y() < 0 ? -src_rect.y() : 0);
        dst_offset.setX(src_rect.x() > 0 ?  src_rect.x() : 0);
        dst_offset.setY(src_rect.y() > 0 ?  src_rect.y() : 0);
    }

    inline void match(const Image* dst, const Point<int> &src_pos, const Image* src)
    {
        match(
            {0, 0, dst->width(), dst->height()},
            {src_pos.x(), src_pos.y(), src->width(), src->height()}
        );
    }

};


void fill(Image* dst, unsigned char* components, Rect<int> rect)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(components != nullptr);
#endif//R64FX_DEBUG

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            for(int c=0; c<dst->componentCount(); c++)
            {
                dst->pixel(x + rect.x(), y + rect.y())[c] = components[c];
            }
        }
    }
}


void fill(Image* dst, unsigned char* components)
{
    fill(dst, components, {0, 0, dst->width(), dst->height()});
}


void fill(Image* dst, int component, unsigned char value, Rect<int> rect)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
#endif//R64FX_DEBUG

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            dst->pixel(x + rect.x(), y + rect.y())[component] = value;
        }
    }
}


void fill(Image* dst, int component, unsigned char value)
{
    fill(dst, component, value, {0, 0, dst->width(), dst->height()});
}


void implant(Image* dst, Point<int> pos, Image* src)
{
    implant(dst, RectIntersection<int>(
        {0,       0,       dst->width(), dst->height()},
        {pos.x(), pos.y(), src->width(), src->height()}
    ), src);
}


void implant(Image* dst, Point<int> dst_offset, Size<int> size, Point<int> src_offset, Image* src)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(src != nullptr);
#endif//R64FX_DEBUG

    for(int y=0; y<size.height(); y++)
    {
        for(int x=0; x<size.width(); x++)
        {
            auto dstpx = dst->pixel(x + dst_offset.x(), y + dst_offset.y());
            auto srcpx = src->pixel(x + src_offset.x(), y + src_offset.y());

            for(int c=0; c<dst->componentCount(); c++)
            {
                dstpx[c] = srcpx[c];
            }
        }
    }
}


void implant(Image* dst, const RectIntersection<int> &intersection, Image* src)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(src != nullptr);
#endif//R64FX_DEBUG

    for(int y=0; y<intersection.height(); y++)
    {
        for(int x=0; x<intersection.width(); x++)
        {
            auto dstpx = dst->pixel(x + intersection.dstx(), y + intersection.dsty());
            auto srcpx = src->pixel(x + intersection.srcx(), y + intersection.srcy());

            for(int c=0; c<dst->componentCount(); c++)
            {
                dstpx[c] = srcpx[c];
            }
        }
    }
}


void implant(Image* dst, Transform2D<float> transform, Image* src, Rect<int> rect)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(src != nullptr);
    assert(src->componentCount() == dst->componentCount());
#endif//R64FX_DEBUG

    if(rect.left() < 0)
    rect.setLeft(0);

    if(rect.top() < 0)
        rect.setTop(0);

    if(rect.right() >= dst->width())
        rect.setRight(dst->width() - 1);

    if(rect.bottom() >= dst->height())
        rect.setBottom(dst->height() - 1);

    for(int y=rect.top(); y<rect.bottom(); y++)
    {
        for(int x=rect.left(); x<rect.right(); x++)
        {
            auto dstpx = dst->pixel(x, y);

            Point<float> p(x, y);
            transform(p);

            float x1 = floor(p.x());
            float x2 = x1 + 1;

            float y1 = floor(p.y());
            float y2 = y1 + 1;

            float fracx = x2 - p.x();
            float fracy = y2 - p.y();
            for(int c=0; c<dst->componentCount(); c++)
            {
                float p11 = dstpx[c];
                float p12 = dstpx[c];
                float p21 = dstpx[c];
                float p22 = dstpx[c];

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

                dstpx[c] = (unsigned char)(val);
            }
        }
    }
}


void implant(Image* dst, Transform2D<float> transform, Image* src)
{
    implant(dst, transform, src, {0, 0, dst->width(), dst->height()});
}


void blend(Image* dst, Point<int> pos, unsigned char** colors, Image* mask)
{
    blend(dst, RectIntersection<int>(
        {0,       0,       dst->width(),  dst->height()},
        {pos.x(), pos.y(), mask->width(), mask->height()}
    ), colors, mask);
}


void blend(Image* dst, Point<int> dst_offset, Size<int> size, Point<int> mask_offset, unsigned char** colors, Image* mask)
{
    #ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(mask != nullptr);
#endif//R64FX_DEBUG

    static const float rcp = 1.0f / float(255);

    for(int y=0; y<size.height(); y++)
    {
        for(int x=0; x<size.width(); x++)
        {
            auto dstpx = dst->pixel(x + dst_offset.x(), y + dst_offset.y());
            auto mskpx = mask->pixel(x + mask_offset.x(), y + mask_offset.y());

            for(int m=0; m<mask->componentCount(); m++)
            {
                float alpha            = float(      mskpx[m]) * rcp;
                float one_minus_alpha  = float(255 - mskpx[m]) * rcp;

                for(int c=0; c<dst->componentCount(); c++)
                {
                    float result = float(dstpx[c]) * one_minus_alpha + float(colors[m][c]) * alpha;
                    dstpx[c] = (unsigned char)result;
                }
            }
        }
    }
}


void blend(Image* dst, const RectIntersection<int> &intersection, unsigned char** colors, Image* mask)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(mask != nullptr);
#endif//R64FX_DEBUG

    static const float rcp = 1.0f / float(255);

    if(intersection.width() <= 0 || intersection.height() <= 0)
        return;

    for(int y=0; y<intersection.height(); y++)
    {
        for(int x=0; x<intersection.width(); x++)
        {
            auto dstpx = dst->pixel(x + intersection.dstx(), y + intersection.dsty());
            auto mskpx = mask->pixel(x + intersection.srcx(), y + intersection.srcy());

            for(int m=0; m<mask->componentCount(); m++)
            {
                float alpha            = float(      mskpx[m]) * rcp;
                float one_minus_alpha  = float(255 - mskpx[m]) * rcp;

                for(int c=0; c<dst->componentCount(); c++)
                {
                    float result = float(dstpx[c]) * one_minus_alpha + float(colors[m][c]) * alpha;
                    dstpx[c] = (unsigned char)result;
                }
            }
        }
    }
}


void blend(Image* dst, Point<int> pos, unsigned char* color, Image* mask)
{
    blend(dst, RectIntersection<int>(
        {0,       0,       dst->width(),  dst->height()},
        {pos.x(), pos.y(), mask->width(), mask->height()}
    ), color, mask);
}


void blend(Image* dst, const RectIntersection<int> &intersection, unsigned char* color, Image* mask)
{
#ifdef R64FX_DEBUG
    assert(mask != nullptr);
    assert(mask->componentCount() == 1);
#endif//R64FX_DEBUG
    blend(dst, intersection, &color, mask);
}


void draw_arc(
    Image* dst, unsigned char* color, Point<float> center, float radius, float arca, float arcb, float thickness, Rect<int> rect
)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
#endif//R64FX_DEBUG

    float rcp = 1.0f / 255.0f;
    bool flip_arc = false;
    if(arca > arcb)
    {
        swap(arca, arcb);
        flip_arc = true;
    }

    for(int y=rect.top(); y<rect.bottom(); y++)
    {
        for(int x=rect.left(); x<rect.right(); x++)
        {
            float dx = float(center.x() - x);
            float dy = float(center.y() - y);
            float distance = sqrt(dx*dx + dy*dy);
            if(distance >= (radius - thickness) && distance < radius)
            {
                float angle = atan2(dy, dx) + M_PI;

                bool in_arc = (angle > arca && angle < arcb);
                if(flip_arc)
                    in_arc = !in_arc;

                if(in_arc)
                {
                    for(int c=0; c<dst->componentCount(); c++)
                    {
                        dst->pixel(x, y)[c] = color[c];
                    }
                }
            }
            else
            {
                float d;
                if(distance < (radius - thickness))
                {
                    d = (radius - thickness) - distance;
                }
                else
                {
                    d = distance - radius;
                }

                if(d < 1.0f)
                {
                    float angle = atan2(dy, dx) + M_PI;

                    bool in_arc = (angle > arca && angle < arcb);
                    if(flip_arc)
                        in_arc = !in_arc;

                    if(in_arc)
                    {
                        for(int c=0; c<dst->componentCount(); c++)
                        {
                            auto px = dst->pixel(x, y);
                            float val = float(px[c]) * rcp * d + float(color[c]) * rcp * (1.0f - d);
                            px[c] = (unsigned char)(val * 255.0f);
                        }
                    }
                }
            }
        }
    }

    draw_radius(dst, color, center, arca, radius, (radius - thickness), 1);
    draw_radius(dst, color, center, arcb, radius, (radius - thickness), 1);
}


void draw_arc(Image* dst, unsigned char* color, Point<float> center, float radius, float arca, float arcb, float thickness)
{
    draw_arc(dst, color, center, radius, arca, arcb, thickness, {0, 0, dst->width(), dst->height()});
}


void draw_line(Image* dst, unsigned char* color, Point<float> a, Point<float> b, float thickness)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(color != nullptr);
#endif//R64FX_DEBUG

    float dx = b.x() - a.x();
    float dy = b.y() - a.y();
    float length = sqrt(dx*dx + dy*dy);
    float length_rcp = 1.0f / length;
    float cosang = dx * length_rcp;
    float sinang = dy * length_rcp;

//     int min_x = min(a.x(), b.x());
//     int min_y = min(a.y(), b.y());

    Image src(max(1, int(length)), max(1, int(thickness)), dst->componentCount());
    {
        fill(&src, color);
    }

    Transform2D<float> t;
    t.translate(a.x(), a.y());
    t.rotate(sinang, cosang);
    t.translate(0.0f, -int(float(thickness) * 0.5));

    Rect<int> r = {
        0, 0, dst->width(), dst->height()
    };

    implant(dst, t, &src, r);
}


void draw_radius(Image* dst, unsigned char* color, Point<float> center, float angle, float outer, float inner, float thickness)
{
    float sinang = sin(angle);
    float cosang = cos(angle);
    draw_line(
        dst, color,
        {center.x() + cosang * inner, center.y() + sinang * inner},
        {center.x() + cosang * outer, center.y() + sinang * outer},
        thickness
    );
}


void draw_triangles(int size, Image* up, Image* down, Image* left, Image* right)
{
    Image img(size, size);

    unsigned char bg = 0;
    fill(&img, &bg);

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


void draw_waveform(Image* dst, unsigned char* color, float* data, Rect<int> rect, float gain)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(color != nullptr);
    assert(rect.left() >= 0);
    assert(rect.top() >= 0);
    assert(rect.right() <= dst->width());
    assert(rect.bottom() <= dst->height());
#endif//R64FX_DEBUG

    float half_height = rect.height() / 2;
    float half_height_rcp = 1.0f / half_height;

    for(int y=0; y<rect.height(); y++)
    {
        float yy = (y - half_height) * half_height_rcp;

        for(int x=0; x<rect.width(); x++)
        {
            unsigned char* dstpx = dst->pixel(x + rect.x(), y + rect.y());
            float min_value = data[x*2] * gain;
            float max_value = data[x*2 + 1] * gain;
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


void draw_circle(Image* dst, unsigned char* color, Point<int> center, float radius)
{
    Rect<int> rect(center.x() - radius - 1, center.y() - radius - 1, radius * 2 + 1, radius * 2 + 1);

    for(int y=0; y<rect.height(); y++)
    {
        float yy = y + rect.y();
        if(yy >= dst->height())
            break;
        if(yy < 0)
            continue;

        for(int x=0; x<rect.width(); x++)
        {
            float xx = x + rect.x();
            if(xx >= dst->width())
                break;

            if(xx < 0)
                continue;

            float dx = xx - center.x();
            float dy = yy - center.y();
            float rr = sqrt(dx*dx + dy*dy);
            float dd = radius - rr;
            if(dd < 0.0f)
                dd = 0.0f;
            else if(dd > 1.0f)
                dd = 1.0f;

            for(int c=0; c<dst->componentCount(); c++)
            {
                dst->pixel(xx, yy)[c] = (unsigned char)(float(color[c]) * dd);
            }
        }
    }
}


void subtract_image(Image* dst, Point<int> pos, Image* src)
{
#ifdef R64FX_DEBUG
    assert(dst->componentCount() == src->componentCount());
#endif//R64FX_DEBUG

    MatchedRects mr(
        Rect<int>(0, 0, dst->width(), dst->height()),
        Rect<int>(pos.x(), pos.y(), src->width(), src->height())
    );

    for(int y=0; y<mr.size.height(); y++)
    {
        for(int x=0; x<mr.size.width(); x++)
        {
            auto dstpx = dst->pixel(x + mr.dst_offset.x(), y + mr.dst_offset.y());
            auto srcpx = src->pixel(x + mr.src_offset.x(), y + mr.src_offset.y());

            for(int c=0; c<dst->componentCount(); c++)
            {
                int dstval = dstpx[c];
                int srcval = srcpx[c];
                int newval = dstval - srcval;
                if(newval < 0)
                    newval = 0;
                dstpx[c] = (unsigned char)newval;
            }
        }
    }
}


void invert_image(Image* dst, Image* src)
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


void flip_vertically(Image* img)
{
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


void flip_horizontally(Image* img)
{
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


void fill_rounded_rect(Image* dst, unsigned char* color, Rect<int> rect, int corner_radius)
{
    Image mask(rect.width(), rect.height(), 1);
    {
        unsigned char c = 255;
        fill(&mask, &c);

        Image arc(corner_radius, corner_radius);
        draw_circle(&arc, &c, {corner_radius - 1, corner_radius - 1}, corner_radius);
        invert_image(&arc, &arc);
        subtract_image(&mask, {0, 0}, &arc);

        flip_vertically(&arc);
        subtract_image(&mask, {0, mask.height() - corner_radius}, &arc);

        flip_horizontally(&arc);
        subtract_image(&mask, {mask.width() - arc.width(), mask.height() - arc.height()}, &arc);

        flip_vertically(&arc);
        subtract_image(&mask, {mask.width() - arc.width(), 0}, &arc);
    }

    unsigned char* colors[1] = {color};
    blend(dst, rect.position(), colors, &mask);
}

}//namespace r64fx
