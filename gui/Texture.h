#ifndef R64FX_GUI_TEXTURE_H
#define R64FX_GUI_TEXTURE_H

#include <GL/gl.h>
#include <string>

namespace r64fx{
    
class Texture{
    GLuint _texture = 0;
    int _width = 0;
    int _height = 0;
    
    void load_to_vram(int width, int height, int channel_count, int mode, unsigned char* bytes);
    
public:
    Texture(std::string path);
    
    Texture(int width, int height, int channel_count, int mode, unsigned char* bytes)
    {
        load_to_vram(width, height, channel_count, mode, bytes);
    }
    
   ~Texture();
    
    inline int width() const { return _width; }
    inline int height() const { return _width; }
    
    GLuint texture() const { return _texture; }
    
    inline void bind(int mode = GL_TEXTURE_2D) { glBindTexture(mode, _texture); }
    
    inline bool isValid() const { return _width && _height; }
    
    static void init();
    
    static Texture* defaultTexture();

    static Texture* transparent16x16();
    
    static void cleanup();
};
    
}//namespace r64fx

#endif//R64FX_GUI_TEXTURE_H