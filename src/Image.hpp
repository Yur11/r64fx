#ifndef R64FX_IMAGE_HPP
#define R64FX_IMAGE_HPP

namespace r64fx{
    
namespace Impl{
    class WindowSurface;
}

class Image{
    unsigned char* m_data = nullptr;
    int m_width = 0;
    int m_height = 0;
    int m_channel_count = 0;
    unsigned int m_flags = 0;

public:
    Image(int w, int h, int c);

    Image(Impl::WindowSurface surface);

    ~Image();

    bool isWindowSurface() const;

    inline unsigned char* data() const { return m_data; }

    inline int width() const { return m_width; }

    inline int height() const { return m_height; }

    inline int channelCount() const { return m_channel_count; }
};
    
}//namespace r64fx

#endif//R64FX_IMAGE_HPP