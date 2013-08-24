#ifndef R64FX_GUI_TEXTURE_H
#define R64FX_GUI_TEXTURE_H

#include <GL/gl.h>
#include <string>
#include <vector>

namespace r64fx{
    
class Texture{
    GLuint _texture = 0;
    int _width = 0;
    int _height = 0;
    
    void load_to_vram(int width, int height, int channel_count, int mode, unsigned char* bytes);
    
public:
    Texture() {}
    
    Texture(std::string name);
    
    Texture(int width, int height, int channel_count, int mode, unsigned char* bytes)
    {
        load_to_vram(width, height, channel_count, mode, bytes);
    }
    
   ~Texture();
    
    inline bool operator==(const Texture &other)
    {
        return _texture == other._texture;
    }
   
    inline int width() const { return _width; }
    inline int height() const { return _width; }
    
    GLuint id() const { return _texture; }
    
    inline void bind(int mode = GL_TEXTURE_2D) { glBindTexture(mode, _texture); }
    
    inline bool isGood() const { return _width && _height; }
    
    inline void free() { glDeleteTextures(1, &_texture); _width = _height = 0; }
    
    static void init(std::vector<std::string>* data_paths);
    
    static Texture defaultTexture();
    
    static Texture badTexture();

    static Texture transparent16x16();
    
    static void cleanup();
};
    
}//namespace r64fx

#endif//R64FX_GUI_TEXTURE_H