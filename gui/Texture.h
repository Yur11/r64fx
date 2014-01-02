#ifndef R64FX_GUI_TEXTURE_H
#define R64FX_GUI_TEXTURE_H

#include <GL/glew.h>
#include <string>
#include <vector>
#include "geometry.h"


namespace r64fx{
    
/** @brief A texture handle. */
class Texture{
    GLuint _texture = 0;
    int _width = 0;
    int _height = 0;
    
    void load_to_vram(int width, int height, int channel_count, int mode, unsigned char* bytes);
    
public:
    Texture() {}
    
    /** @brief Create a texture from file.*/
    Texture(std::string name);
    
    /** @brief Create a texture from a file descriptor. 
     
        @param fd - must be an open file descriptor.
        @param close_fd - if true, will call fclose() on fd.
     */
    Texture(FILE* fd, bool close_fd = true);
   
    /** @brief Create an empty texture with the parameters give. */
    Texture(int width, int height, int channel_count, int mode)
    {
        load_to_vram(width, height, channel_count, mode, nullptr);
    }
    
    /** @brief Create a texture from a memory buffer. */
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
   
    void free();
    
    inline bool operator==(const Texture &other)
    {
        return _texture == other._texture;
    }
    
    inline bool operator!=(const Texture &other)
    {
        return _texture != other._texture;
    }
   
    inline int width() const { return _width; }
    inline int height() const { return _width; }
    
    GLuint id() const { return _texture; }
    
    inline void bind(int mode = GL_TEXTURE_2D) { glBindTexture(mode, _texture); }
    
    inline bool isGood() const { return _width && _height; }
    
    inline void repeat_s() { glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); }
    inline void repeat_t() { glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); }
    inline void repeat() { repeat_s(); repeat_t(); }
    
    inline void clamp_s() { glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); }
    inline void clamp_t() { glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); }
    inline void clamp() { clamp_s(); clamp_t(); }

    static void init();
    
    static Texture defaultTexture();
    
    static Texture badTexture();
    
    static void cleanup();
};

    
}//namespace r64fx

#endif//R64FX_GUI_TEXTURE_H