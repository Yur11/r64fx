#include "Image.hpp"
#include <new>

#define IS_WINDOW_SURFACE 1

namespace r64fx{
    
Image::Image(int w, int h, int c)
{
    if(w>0 && h>0 && c>0)
    {
        int size = w * h * c;
        m_data = new (std::nothrow) unsigned char[size];
        m_width = w;
        m_height = h;
        m_channel_count = c;
    }
}


// Image::Image(Impl::WindowSurface surface)
// {
//     m_data = (unsigned char*)Impl::pixels(surface);
//     Impl::get_surface_size(m_width, m_height, surface);
//     m_channel_count = Impl::bytes_per_pixel(surface);
//     m_flags |= IS_WINDOW_SURFACE;
// }


Image::~Image()
{
    if(!isWindowSurface())
    {
        delete[] m_data;
    }
}


bool Image::isWindowSurface() const
{
    return m_flags & IS_WINDOW_SURFACE;
}
    
}//namespace r64fx