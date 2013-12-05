#ifndef R64FX_GUI_TEXTURE_H
#define R64FX_GUI_TEXTURE_H

#include <GL/gl.h>
#include <string>
#include <vector>
#include "geometry.h"


namespace r64fx{
    
class Texture{
    GLuint _texture = 0;
    int _width = 0;
    int _height = 0;
    
    void load_to_vram(int width, int height, int channel_count, int mode, unsigned char* bytes);
    
public:
    Texture() {}
    
    Texture(std::string name);
   
    Texture(int width, int height, int channel_count, int mode)
    {
        load_to_vram(width, height, channel_count, mode, nullptr);
    }
    
    Texture(int width, int height, int channel_count, int mode, unsigned char* bytes)
    {
        load_to_vram(width, height, channel_count, mode, bytes);
    }
    
    /** Load image data from a resource. 
     *  Byte 0 encodes the channel count.
     *  Bytes 1-4 encode the image width as int.
     *  Bytes 5-8 encode the image height as int.
     *  Followed by data.
     */
    Texture(std::vector<unsigned char> resource_bytes);
    
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
    
    inline void repeat_s() { glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); }
    inline void repeat_t() { glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); }
    inline void repeat() { repeat_s(); repeat_t(); }
    
    inline void clamp_s() { glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); }
    inline void clamp_t() { glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); }
    inline void clamp() { clamp_s(); clamp_t(); }
    
    static void init(std::vector<std::string>* data_paths);
    
    static Texture defaultTexture();
    
    static Texture badTexture();

    static Texture transparent16x16();
    
    static void cleanup();
};

    
}//namespace r64fx

#endif//R64FX_GUI_TEXTURE_H