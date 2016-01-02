#include "Image.hpp"
#include <new>

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

#define IMAGE_OWNS_DATA 1

namespace r64fx{
    
Image::Image(int w, int h, int c, unsigned char* data, bool copy_data)
{
    load(w, h, c, data, copy_data);
}


Image::~Image()
{
    free();
}


bool Image::ownsData() const
{
    return m_flags & IMAGE_OWNS_DATA;
}


void Image::load(int w, int h, int c, unsigned char* data, bool copy_data)
{
    free();

    if(w>0 && h>0 && c>0)
    {
        int size = w * h * c;
        if(data)
        {
            if(copy_data)
            {
                m_data = new (std::nothrow) unsigned char[size];
                for(int i=0; i<size; i++)
                {
                    m_data[i] = data[i];
                }
                m_flags |= IMAGE_OWNS_DATA;
            }
            else
            {
                m_data = data;
                m_flags &= ~IMAGE_OWNS_DATA;
            }
        }
        else
        {
            m_data = new (std::nothrow) unsigned char[size];
            m_flags |= IMAGE_OWNS_DATA;
        }
        m_width = w;
        m_height = h;
        m_channel_count = c;
    }
}


void Image::free()
{
    if(m_data && Image::ownsData())
    {
        delete[] m_data;
    }
    
    m_data = nullptr;
    m_width = 0;
    m_height = 0;
    m_channel_count = 0;
    m_flags = 0;
}


unsigned char* Image::pixel(int x, int y) const
{
#ifdef R64FX_DEBUG
    assert(x < width());
    assert(y < height());
#endif//R64FX_DEBUG

    return data() + y * width() * channelCount() + x * channelCount();
}


void Image::setPixel(int x, int y, unsigned char* px)
{
#ifdef R64FX_DEBUG
    assert(x < width());
    assert(y < height());
#endif//R64FX_DEBUG

    for(int c=0; c<channelCount(); c++)
    {
        pixel(x, y)[c] = px[c];
    }
}
    
}//namespace r64fx