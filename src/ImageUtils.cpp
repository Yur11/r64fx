#include "ImageUtils.hpp"
#include <limits>
#include <algorithm>
#include <vector>

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

#include <iostream>
using namespace std;

using namespace std;

namespace r64fx{

namespace{

constexpr float rcp255 = 1.0f / 255.0f;

}//namespace

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


void fill_component(Image* dst, int component, unsigned char value, Rect<int> rect)
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


void fill_component(Image* dst, int component, unsigned char value)
{
    fill_component(dst, component, value, {0, 0, dst->width(), dst->height()});
}


void fill_gradient_vert(Image* img, unsigned char begin_val, unsigned char end_val, int component, int component_count, Rect<int> rect)
{
#ifdef R64FX_DEBUG
    assert(img != nullptr);
    assert(rect.x() >= 0);
    assert(rect.y() >= 0);
    assert(rect.right()  <= img->width());
    assert(rect.bottom() <= img->height());
#endif//R64FX_DEBUG

    float bc = float(begin_val) * rcp255;
    float ec = float(end_val) * rcp255;
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
            for(int c=0; c<component_count; c++)
            {
                img->pixel(x, y)[c + component] = (unsigned char)(color * 255);
            }
        }
        color += cs;
    }
}


void fill_gradient_vert(Image* img, unsigned char begin_val, unsigned char end_val, int component, int component_count)
{
    fill_gradient_vert(img, begin_val, end_val, component, component_count, {0, 0, img->width(), img->height()});
}


void copy(Image* dst, Point<int> dstpos, Image* src, Rect<int> src_rect)
{
#ifdef R64FX_DEBUG
    assert(dst->componentCount() == src->componentCount());
#endif//R64FX_DEBUG

    RectIntersection<int> src_isec({0, 0, src->width(), src->height()}, src_rect);
    RectIntersection<int> dst_isec({0, 0, dst->width(), dst->height()}, {dstpos.x(), dstpos.y(), src_isec.width(), src_isec.height()});

    for(int y=0; y<dst_isec.height(); y++)
    {
        for(int x=0; x<dst_isec.width(); x++)
        {
            auto dstpx = dst->pixel(x + dst_isec.dstx(),                   y + dst_isec.dsty());
            auto srcpx = src->pixel(x + dst_isec.srcx() + src_isec.dstx(), y + dst_isec.srcy() + src_isec.dsty());
            for(int c=0; c<dst->componentCount(); c++)
            {
                dstpx[c] = srcpx[c];
            }
        }
    }
}


void copy(Image* dst, Point<int> dstpos, Image* src)
{
    copy(dst, dstpos, src, Rect<int>(0, 0, src->width(), src->height()));
}


void copy_component(Image* dst, int dst_component, Point<int> dstpos, Image* src, int src_component, Rect<int> src_rect)
{
    RectIntersection<int> src_isec({0, 0, src->width(), src->height()}, src_rect);
    RectIntersection<int> dst_isec({0, 0, dst->width(), dst->height()}, {dstpos.x(), dstpos.y(), src_isec.width(), src_isec.height()});

    for(int y=0; y<dst_isec.height(); y++)
    {
        for(int x=0; x<dst_isec.width(); x++)
        {
            auto dstpx = dst->pixel(x + dst_isec.dstx(),                   y + dst_isec.dsty());
            auto srcpx = src->pixel(x + dst_isec.srcx() + src_isec.dstx(), y + dst_isec.srcy() + src_isec.dsty());
            dstpx[dst_component] = srcpx[src_component];
        }
    }
}


void copy_component(Image* dst, int dst_component, Point<int> dstpos, Image* src, int src_component)
{
    copy_component(dst, dst_component, dstpos, src, src_component, {0, 0, dst->width(), dst->height()});
}


void copy_rgba(Image* dst, Point<int> dstpos, Image* src, Rect<int> src_rect)
{
#ifdef R64FX_DEBUG
    assert(dst->componentCount() == 4);
    assert(src->componentCount() == 4);
#endif//R64FX_DEBUG

    RectIntersection<int> src_isec({0, 0, src->width(), src->height()}, src_rect);
    RectIntersection<int> dst_isec({0, 0, dst->width(), dst->height()}, {dstpos.x(), dstpos.y(), src_isec.width(), src_isec.height()});

    for(int y=0; y<dst_isec.height(); y++)
    {
        for(int x=0; x<dst_isec.width(); x++)
        {
            auto dstpx = dst->pixel(x + dst_isec.dstx(),                   y + dst_isec.dsty());
            auto srcpx = src->pixel(x + dst_isec.srcx() + src_isec.dstx(), y + dst_isec.srcy() + src_isec.dsty());

            float alpha            = float(      srcpx[3]) * rcp255;
            float one_minus_alpha  = float(255 - srcpx[3]) * rcp255;
            for(int c=0; c<3; c++)
            {
                dstpx[c] = (unsigned char)(float(dstpx[c]) * alpha + float(srcpx[c]) * one_minus_alpha);
            }
        }
    }
}


void copy_rgba(Image* dst, Point<int> dstpos, Image* src)
{
    copy_rgba(dst, dstpos, src, Rect<int>(0, 0, src->width(), src->height()));
}


void blend_colors(Image* dst, Point<int> dstpos, unsigned char** colors, Image* src, Rect<int> src_rect)
{
    RectIntersection<int> src_isec({0, 0, src->width(), src->height()}, src_rect);
    RectIntersection<int> dst_isec({0, 0, dst->width(), dst->height()}, {dstpos.x(), dstpos.y(), src_isec.width(), src_isec.height()});

    for(int y=0; y<dst_isec.height(); y++)
    {
        for(int x=0; x<dst_isec.width(); x++)
        {
            auto dstpx = dst->pixel(x + dst_isec.dstx(),                   y + dst_isec.dsty());
            auto srcpx = src->pixel(x + dst_isec.srcx() + src_isec.dstx(), y + dst_isec.srcy() + src_isec.dsty());

            for(int m=0; m<src->componentCount(); m++)
            {
                float alpha            = float(      srcpx[m]) * rcp255;
                float one_minus_alpha  = float(255 - srcpx[m]) * rcp255;

                for(int c=0; c<dst->componentCount(); c++)
                {
                    float result = float(dstpx[c]) * one_minus_alpha + float(colors[m][c]) * alpha;
                    dstpx[c] = (unsigned char)result;
                }
            }
        }
    }
}


void blend_colors(Image* dst, Point<int> dstpos, unsigned char** colors, Image* src)
{
    blend_colors(dst, dstpos, colors, src, Rect<int>(0, 0, src->width(), src->height()));
}


void copy_transformed(Image* dst, Transformation2D<float> transform, Image* src, Rect<int> dst_rect)
{
#ifdef R64FX_DEBUG
    assert(src->componentCount() == dst->componentCount());
#endif//R64FX_DEBUG

    if(dst_rect.left() < 0)
        dst_rect.setLeft(0);

    if(dst_rect.top() < 0)
        dst_rect.setTop(0);

    if(dst_rect.right() >= dst->width())
        dst_rect.setRight(dst->width() - 1);

    if(dst_rect.bottom() >= dst->height())
        dst_rect.setBottom(dst->height() - 1);

    for(int y=dst_rect.top(); y<dst_rect.bottom(); y++)
    {
        for(int x=dst_rect.left(); x<dst_rect.right(); x++)
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


void copy_transformed(Image* dst, Transformation2D<float> transform, Image* src)
{
    copy_transformed(dst, transform, src, {0, 0, dst->width(), dst->height()});
}


void flip_vert(Image* img)
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


void flip_hori(Image* img)
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


void combine(Image* dst, Image* src1, Point<int> pos1, Image* src2, Point<int> pos2)
{
#ifdef R64FX_DEBUG
    assert(dst != nullptr);
    assert(src1 != nullptr);
    assert(src2 != nullptr);
    assert(src1->componentCount() == dst->componentCount());
    assert(src2->componentCount() == dst->componentCount());
#endif//R64FX_DEBUG

    RectIntersection<int> src_isec(
        {pos1.x(), pos1.y(), src1->width(), src1->height()},
        {pos2.x(), pos2.y(), src2->width(), src2->height()}
    );
    if(src_isec.width() <= 0 || src_isec.height() <= 0)
        return;

    auto p1 = src_isec.dstOffset();
    auto p2 = src_isec.srcOffset();

    RectIntersection<int> dst_isec(
        {0, 0, dst->width(), dst->height()},
        src_isec.rect()
    );
    if(dst_isec.width() <=0 || dst_isec.height() <= 0)
        return;

    p1 += dst_isec.srcOffset();
    p2 += dst_isec.srcOffset();

    for(int y=0; y<dst_isec.height(); y++)
    {
        for(int x=0; x<dst_isec.width(); x++)
        {
            auto srcpx1 = src1->pixel(x + p1.x(), y + p1.y());
            auto srcpx2 = src2->pixel(x + p2.x(), y + p2.y());
            auto dstpx = dst->pixel(x + dst_isec.dstx(), y + dst_isec.dsty());
            for(auto c=0; c<dst->componentCount(); c++)
            {
                auto val1 = float(srcpx1[c]) * rcp255;
                auto val2 = float(srcpx2[c]) * rcp255;
                auto val = val1 * val2;
                dstpx[c] = (unsigned char)(val * 255.0f);
            }
        }
    }
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

    Transformation2D<float> t;
    t.translate(a.x(), a.y());
    t.rotate(sinang, cosang);
    t.translate(0.0f, -int(float(thickness) * 0.5));

    Rect<int> r = {
        0, 0, dst->width(), dst->height()
    };

    copy_transformed(dst, t, &src, r);
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


template<typename T> inline float denormalize(T num)
{
    static const float rcp = 1.0f / float(std::numeric_limits<T>::max());
    return float(num) * rcp;
}

template<> inline float denormalize(float num)
{
    return num;
}


template<typename T> void draw_waveform(Image* dst, unsigned char* color, T* data, const Rect<int> &rect)
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


void draw_waveform(Image* dst, unsigned char* color, unsigned char* data, const Rect<int> &rect)
{
    draw_waveform<unsigned char>(dst, color, data, rect);
}


void draw_waveform(Image* dst, unsigned char* color, unsigned short* data, const Rect<int> &rect)
{
    draw_waveform<unsigned short>(dst, color, data, rect);
}


void draw_waveform(Image* dst, unsigned char* color, unsigned int* data, const Rect<int> &rect)
{
    draw_waveform<unsigned int>(dst, color, data, rect);
}


void draw_waveform(Image* dst, unsigned char* color, float* data, const Rect<int> &rect)
{
    draw_waveform<float>(dst, color, data, rect);
}


void stroke_circle(Image* dst, unsigned char* color, Point<float> center, float radius, float thickness)
{
    Rect<int> rect(center.x() - radius - 1, center.y() - radius - 1, radius * 2 + 2, radius * 2 + 2);

    radius -= 1.0f;
    thickness += 1.0f;
    float outer_radius = radius + thickness * 0.5f;
    float inner_radius = radius - thickness * 0.5f;

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

            float outer_dd = outer_radius - rr;
            float inner_dd = rr - inner_radius;

            if(outer_dd < 0.0f)
                outer_dd = 0.0f;
            else if(outer_dd > 1.0f)
                outer_dd = 1.0f;

            if(inner_dd < 0.0f)
                inner_dd = 0.0f;
            else if(inner_dd > 1.0f)
                inner_dd = 1.0f;

            float dd = outer_dd * inner_dd;

            for(int c=0; c<dst->componentCount(); c++)
            {
                if(dd > 0.0f)
                    dst->pixel(xx, yy)[c] = (unsigned char)(float(color[c]) * dd);
            }
        }
    }
}


void fill_circle(Image* dst, unsigned char* color, Point<float> center, float radius)
{
    Rect<float> rect(center.x() - radius - 1, center.y() - radius - 1, radius * 2 + 2, radius * 2 + 2);

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


void fill_rounded_rect(Image* dst, unsigned char* color, Rect<int> rect, int corner_radius)
{
    Image mask(rect.width(), rect.height(), 1);
    {
        unsigned char c = 255;
        fill(&mask, &c);

        Image arc(corner_radius, corner_radius);
        fill_circle(&arc, &c, Point<float>(corner_radius - 1, corner_radius - 1), corner_radius);
        invert(&arc, &arc);
        subtract_image(&mask, {0, 0}, &arc);

        flip_vert(&arc);
        subtract_image(&mask, {0, mask.height() - corner_radius}, &arc);

        flip_hori(&arc);
        subtract_image(&mask, {mask.width() - arc.width(), mask.height() - arc.height()}, &arc);

        flip_vert(&arc);
        subtract_image(&mask, {mask.width() - arc.width(), 0}, &arc);
    }

    unsigned char* colors[1] = {color};
    blend_colors(dst, rect.position(), colors, &mask);
}


struct PlotPoint{
    float minval   = 0.0f;
    float maxval   = 0.0f;
    float minblend = 0.0f;
    float maxblend = 0.0f;
};


void stroke_plot(Image* img, unsigned char* color, Rect<int> rect, float* data, float thickness, float scale, float offset)
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

        pp.minval = int(max(pp.minval, minimum) * scale + offset);
        pp.maxval = int(min(pp.maxval, maximum) * scale + offset);
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
