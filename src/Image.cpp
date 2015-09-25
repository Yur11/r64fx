#include "Image.hpp"
#include <new>

#define IMAGE_OWNS_DATA 1

namespace r64fx{
    
Image::Image(int w, int h, int c, unsigned char* data)
{
    m_flags = 0;

    if(w>0 && h>0 && c>0)
    {
        if(data)
        {
            m_data = data;
            m_flags &= ~IMAGE_OWNS_DATA;
        }
        else
        {
            int size = w * h * c;
            m_data = new (std::nothrow) unsigned char[size];
            m_flags |= IMAGE_OWNS_DATA;
        }
        m_width = w;
        m_height = h;
        m_channel_count = c;
    }
}


Image::~Image()
{
    if(Image::ownsData())
    {
        delete[] m_data;
    }
}


bool Image::ownsData() const
{
    return m_flags & IMAGE_OWNS_DATA;
}


unsigned char* Image::pixel(int x, int y) const
{
    return data() + y * width() * channelCount() + x * channelCount();
}


void Image::fill(unsigned char* pixel)
{
    for(int y=0; y<height(); y++)
    {
        for(int x=0; x<width(); x++)
        {
            for(int c=0; c<channelCount(); c++)
            {
                Image::pixel(x, y)[c] = pixel[c];
            }
        }
    }
}
    
}//namespace r64fx