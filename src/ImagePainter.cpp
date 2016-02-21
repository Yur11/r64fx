#include "ImagePainter.hpp"
#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

#include <iostream>
using namespace std;

namespace r64fx{

namespace{
    unsigned char fg_stub[4] = {255, 255, 255, 255};
    unsigned char bg_stub[4] = {255, 255, 255, 255};
}//namespace

ImagePainter::ImagePainter()
{
    setForegroundComponents(nullptr);
    setBackgroundComponents(nullptr);
}


ImagePainter::ImagePainter(Image* img)
{
    setImage(img);
    setForegroundComponents(nullptr);
    setBackgroundComponents(nullptr);
}


ImagePainter::ImagePainter(Image* img, unsigned char* fg, unsigned char* bg)
{
    setImage(img);
    setForegroundComponents(fg);
    setBackgroundComponents(bg);
}


void ImagePainter::setImage(Image* img)
{
#ifdef R64FX_DEBUG
    assert(img->componentCount() >= 1);
    assert(img->componentCount() <= 4);
#endif//R64FX_DEBUG
    m_img = img;
}


Image* ImagePainter::image() const
{
    return m_img;
}


void ImagePainter::setForegroundComponents(unsigned char* fg)
{
    if(fg)
        m_fg = fg;
    else
        m_fg = fg_stub;
}


unsigned char* ImagePainter::foregroundComponents() const
{
    if(m_fg == fg_stub)
        return nullptr;
    else
        return m_fg;
}


void ImagePainter::setBackgroundComponents(unsigned char* bg)
{
    if(bg)
        m_bg = bg;
    else
        m_bg = bg_stub;
}


unsigned char* ImagePainter::backgroundComponents() const
{
    if(m_bg == bg_stub)
        return nullptr;
    else
        return m_bg;
}


void ImagePainter::fillForeground(Rect<int> rect)
{
    fill(m_fg, rect);
}


void ImagePainter::fillForeground()
{
    fillForeground({0, 0, m_img->width(), m_img->height()});
}


void ImagePainter::fillBackground(Rect<int> rect)
{
    fill(m_bg, rect);
}


void ImagePainter::fillBackground()
{
    fillBackground({0, 0, m_img->width(), m_img->height()});
}


void ImagePainter::fillComponent(int component, unsigned char value, Rect<int> rect)
{
#ifdef R64FX_DEBUG
    assert(m_img != nullptr);
#endif//R64FX_DEBUG

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            m_img->pixel(x + rect.x(), y + rect.y())[component] = value;
        }
    }
}


void ImagePainter::fillComponent(int component, unsigned char value)
{
    fillComponent(component, value, {0, 0, m_img->width(), m_img->height()});
}


void ImagePainter::fill(unsigned char* components, Rect<int> rect)
{
#ifdef R64FX_DEBUG
    assert(m_img != nullptr);
#endif//R64FX_DEBUG

    for(int y=0; y<rect.height(); y++)
    {
        for(int x=0; x<rect.width(); x++)
        {
            for(int c=0; c<m_img->componentCount(); c++)
            {
                m_img->pixel(x + rect.x(), y + rect.y())[c] = components[c];
            }
        }
    }
}


void ImagePainter::fill(unsigned char* components)
{
    fill(components, {0, 0, m_img->width(), m_img->height()});
}


void ImagePainter::implant(Point<int> pos, Image* img)
{
#ifdef R64FX_DEBUG
    assert(m_img != nullptr);
    assert(img != nullptr);
    assert(m_img->componentCount() == img->componentCount());
#endif//R64FX_DEBUG

    Image* dst = m_img;
    Image* src = img;

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

            for(int c=0; c<dst->componentCount(); c++)
            {
                dstpx[c] = srcpx[c];
            }
        }
    }
}


void ImagePainter::implant(Transform2D<float> transform, Image* img, Rect<int> rect)
{
#ifdef R64FX_DEBUG
    assert(m_img != nullptr);
    assert(img != nullptr);
    assert(img->componentCount() == m_img->componentCount());
#endif//R64FX_DEBUG

    Image* src = img;
    Image* dst = m_img;

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


void ImagePainter::implant(Transform2D<float> transform, Image* img)
{
    implant(transform, img, {0, 0, m_img->width(), m_img->height()});
}


void ImagePainter::blend(Point<int> pos, unsigned char** colors, Image* mask)
{
#ifdef R64FX_DEBUG
    assert(m_img != nullptr);
    assert(mask != nullptr);
#endif//R64FX_DEBUG

    Image* dst = m_img;

    static const float rcp = 1.0f / float(255);

    Rect<int> dst_rect = {0, 0, dst->width(), dst->height()};
    Rect<int> src_rect = {pos.x(), pos.y(), mask->width(), mask->height()};
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
            auto mskpx = mask->pixel(x + src_offset_x, y + src_offset_y);

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


void ImagePainter::drawArc(Point<float> center, float radius, float arca, float arcb, float thickness, Rect<int> rect)
{
#ifdef R64FX_DEBUG
    assert(m_img != nullptr);
#endif//R64FX_DEBUG

    Image* dst = m_img;
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
                        dst->pixel(x, y)[c] = m_fg[c];
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
                            float val = float(px[c]) * rcp * d + float(m_fg[c]) * rcp * (1.0f - d);
                            px[c] = (unsigned char)(val * 255.0f);
                        }
                    }
                }
            }
        }
    }

    drawRadius(center, arca, radius, (radius - thickness), 1);
    drawRadius(center, arcb, radius, (radius - thickness), 1);
}


void ImagePainter::drawArc(Point<float> center, float radius, float arca, float arcb, float thickness)
{
    drawArc(center, radius, arca, arcb, thickness, {0, 0, m_img->width(), m_img->height()});
}


void ImagePainter::drawLine(Point<float> a, Point<float> b, float thickness)
{
    float dx = b.x() - a.x();
    float dy = b.y() - a.y();
    float length = sqrt(dx*dx + dy*dy);
    float length_rcp = 1.0f / length;
    float cosang = dx * length_rcp;
    float sinang = dy * length_rcp;

    int min_x = min(a.x(), b.x());
    int min_y = min(a.y(), b.y());

    Image src(max(1, int(length)), max(1, int(thickness)), m_img->componentCount());
    {
        ImagePainter p(&src);
        p.fill(m_fg);
    }

    Transform2D<float> t;
    t.translate(a.x(), a.y());
    t.rotate(sinang, cosang);
    t.translate(0.0f, -int(float(thickness) * 0.5));

    Rect<int> r = {
        min_x - int(thickness) + 1,
        min_y - int(thickness) + 1,
        int(abs(dx)) + int(thickness)*2,
        int(abs(dy)) + int(thickness)*2
    };

//     Rect<int> r = {0, 0, m_img->width(), m_img->height()};

    implant(t, &src, r);
}


void ImagePainter::drawRadius(Point<float> center, float angle, float outer, float inner, float thickness)
{
    float sinang = sin(angle);
    float cosang = cos(angle);
    drawLine(
        {center.x() + cosang * inner, center.y() + sinang * inner},
        {center.x() + cosang * outer, center.y() + sinang * outer},
        thickness
    );
}

}//namespace r64fx