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

    Image(const Image&){}

public:
    Image(int w, int h, int c, unsigned char* data = nullptr);

    ~Image();

    bool ownsData() const;

    inline unsigned char* data() const { return m_data; }

    inline int width() const { return m_width; }

    inline int height() const { return m_height; }

    inline int channelCount() const { return m_channel_count; }

    unsigned char* pixel(int x, int y) const;

    inline unsigned char* operator()(int x, int y) const { return pixel(x, y); }

    void setPixel(int x, int y, unsigned char* px);

    void fill(unsigned char* pixel);
};
    
}//namespace r64fx

#endif//R64FX_IMAGE_HPP