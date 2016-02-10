#ifndef R64FX_IMAGE_HPP
#define R64FX_IMAGE_HPP

namespace r64fx{

class Image{
    unsigned char* m_data = nullptr;
    int m_width = 0;
    int m_height = 0;
    int m_channel_count = 0;
    unsigned int m_flags = 0;

    Image(const Image&){}

public:
    Image(int w = 0, int h = 0, int c = 1, unsigned char* data = nullptr, bool copy_data = false);

    ~Image();

    bool ownsData() const;
    
    void load(int w, int h, int c, unsigned char* data = nullptr, bool copy_data = false);
    
    void free();
    
    inline bool isGood() const { return m_data != nullptr; }

    inline unsigned char* data() const { return m_data; }

    inline int width() const { return m_width; }

    inline int height() const { return m_height; }

    inline int channelCount() const { return m_channel_count; }

    unsigned char* pixel(int x, int y) const;

    inline unsigned char* operator()(int x, int y) const { return pixel(x, y); }

    void setPixel(int x, int y, unsigned char* px);
};

}//namespace r64fx

#endif//R64FX_IMAGE_HPP