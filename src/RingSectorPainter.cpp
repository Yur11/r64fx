#include "RingSectorPainter.hpp"
#include "ImageUtils.hpp"
#include <assert.h>

namespace r64fx{

void gen_atan_table(Image* table, int chan)
{
#ifdef R64FX_DEBUG
    assert((table->componentCount() & 3) == 0);
#endif//R64FX_DEBUG

    for(int y=0; y<table->height(); y++)
    {
        for(int x=0; x<table->width(); x++)
        {
            int xx = x - table->width();
            int yy = y - table->height();

            float xyang = atan2(yy, xx) + 0.5f * M_PI;
            if(xyang > M_PI)
                xyang -= 2.0f * M_PI;

            auto px = (float*)table->pixel(x, y);
            px[chan] = xyang;
        }
    }
}


float atan(int x, int y, Image* table, int chan)
{
#ifdef R64FX_DEBUG
    assert(table != nullptr);
    assert((table->componentCount() & 3) == 0);
    assert(table->width() == table->height());
    assert(x < (table->width()*2));
    assert(y < (table->height()*2));
#endif//R64FX_DEBUG

    int s = table->width();

    float a = 1.0f;
    float b = 0.0f;

    if(x >= s)
    {
        x = x - s;
        x = s - x - 1;
        a = -1.0f;
    }

    int xx = x;
    int yy = y;

    if(y >= s)
    {
        y -= s;
        xx = s - y - 1;
        yy = x;
        b = -0.5f * M_PI;
    }

    auto px = (float*)table->pixel(xx, yy);
    return (px[chan] + b) * a;
}


void gen_radius_table(Image* table, int chan)
{
#ifdef R64FX_DEBUG
    assert(table != nullptr);
    assert((table->componentCount() & 3) == 0);
    assert(table->width() == table->height());
#endif//R64FX_DEBUG

    int c = table->width();

    for(int y=0; y<table->height(); y++)
    {
        for(int x=0; x<table->width(); x++)
        {
            float dx = x - c; dx += 0.5f;
            float dy = y - c; dy += 0.5f;
            float rr = sqrt(dx*dx + dy*dy);

            auto px = (float*)table->pixel(x, y);
            px[chan] = rr;
        }
    }
}


float radius(int x, int y, Image* table, int chan)
{
#ifdef R64FX_DEBUG
    assert(table != nullptr);
    assert((table->componentCount() & 3) == 0);
    assert(table->width() == table->height());
#endif//R64FX_DEBUG

    int s = table->width();

    if(x >= s)
    {
        x = x - s;
        x = s - x - 1;
    }

    if(y >= s)
    {
        y = y - s;
        y = s - y - 1;
    }

    auto px = (float*)table->pixel(x, y);
    return px[chan];
}


void copy_rotated(Image* dst, Image* src, int size, float angle, const ImgCopyFlags flags = ImgCopyReplace())
{
    int hs = (size >> 1);
    Transformation2D<float> t;
    t.translate(float(hs) - 0.5f, float(hs) - 0.5f);
    t.rotate(angle);
    t.translate(float(-hs) + 0.5f, float(-hs) + 0.5f);
    copy(dst, t, src, flags);
}


RingSectorPainter::RingSectorPainter(int size)
: m_table((size >> 1), (size >> 1), 8)
, m_radius(size, size, 1)
{
    gen_atan_table(&m_table, 0);
    gen_radius_table(&m_table, 1);
    fill(&m_radius, Color(0));

    for(int y=0; y<(size>>1); y++)
    {
        m_radius(m_radius.width()/2 - 1, y)[0] =
        m_radius(m_radius.width()/2,     y)[0] = 255;
    }
}

void RingSectorPainter::paint(ImgPos dst, float min_angle, float max_angle, float outer_radius, float inner_radius)
{
    int w = (m_table.width() << 1);
    int h = (m_table.height() << 1);

    for(int y=0; y<h; y++)
    {
        for(int x=0; x<w; x++)
        {
            float atanval = atan(x, y, &m_table, 0);

            if(atanval >= min_angle && atanval <= max_angle)
            {
                auto dstpx = dst.img->pixel(x + dst.pos.x(), y + dst.pos.y());
                dstpx[0] = 255;
            }
        }
    }

    copy_rotated(dst.img, &m_radius, size(), min_angle, ImgCopyMax());
    copy_rotated(dst.img, &m_radius, size(), max_angle, ImgCopyMax());

    for(int y=0; y<h; y++)
    {
        for(int x=0; x<w; x++)
        {
            float radiusval = radius(x, y, &m_table, 1);

            float outer = outer_radius - radiusval;
            if(outer < 0.0f)
                outer = 0.0f;
            else if(outer > 1.0f)
                outer = 1.0f;

            float inner = radiusval - inner_radius;
            if(inner < 0.0f)
                inner = 0.0f;
            else if(inner > 1.0f)
                inner = 1.0f;

            auto dstpx = dst.img->pixel(x + dst.pos.x(), y + dst.pos.y());
            dstpx[0] = (unsigned char)(float(dstpx[0]) * inner * outer);
        }
    }
}

}//namespace r64fx
