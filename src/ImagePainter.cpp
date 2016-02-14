#include "ImagePainter.hpp"
#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG


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
    assert(img->channelCount() >= 1);
    assert(img->channelCount() <= 4);
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
            for(int c=0; c<m_img->channelCount(); c++)
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
    assert(m_img->channelCount() == img->channelCount());
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

            for(int c=0; c<dst->channelCount(); c++)
            {
                dstpx[c] = srcpx[c];
            }
        }
    }
}


void ImagePainter::blend(Point<int> pos, Image* colors, Image* mask)
{
#ifdef R64FX_DEBUG
    assert(m_img != nullptr);
    assert(mask != nullptr);
    assert(mask->channelCount() == colors->width());
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

            for(int m=0; m<mask->channelCount(); m++)
            {
                float alpha            = float(      mskpx[m]) * rcp;
                float one_minus_alpha  = float(255 - mskpx[m]) * rcp;

                for(int c=0; c<dst->channelCount(); c++)
                {
                    float result = float(dstpx[c]) * one_minus_alpha + float(colors->pixel(m, 0)[c]) * alpha;
                    dstpx[c] = (unsigned char)result;
                }
            }
        }
    }
}

}//namespace r64fx