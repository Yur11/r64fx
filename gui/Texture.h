#ifndef R64FX_GUI_TEXTURE_H
#define R64FX_GUI_TEXTURE_H

#include "gl.h"
#include <string>

namespace r64fx{
    
class Texture{
protected:
    GLuint _gl_name;
    
    Texture(GLuint gl_name)
    : _gl_name(gl_name)
    {}
    
public:
    /* Flags */
    static const unsigned int NoMipmaps = 0;
    static const unsigned int AllocMipmaps = 1;
    
    enum class Kind{
       Tex1D,
       Tex2D 
    };
    
    virtual Texture::Kind kind() = 0;
    
    virtual ~Texture();
    
    inline GLuint glName() const { return _gl_name; }
    
    static void addCommonTexture(std::string name, Texture* tex);
    
    static Texture* find(std::string name);
    
    template<typename T> inline T to() { return (T)this; }
    
    static void freeCommonTextures();
};


class Texture1D : public Texture{
protected:
    int _nchannels;
    int _nlevels;
    GLsizei _w;
    
    Texture1D(GLuint gl_name, int nchannels, int nlevels, GLsizei w);
    
public:
    virtual Texture::Kind kind();
    
    inline int nchannels() const { return _nchannels; }
    
    inline int nlevels() const { return _nlevels; }
    
    inline GLsizei width() const { return _w; }

    inline void bind() const { gl::BindTexture(GL_TEXTURE_1D, glName()); }
};


class Texture2D : public Texture1D{
protected:
    GLsizei _h;
    
    Texture2D(GLuint gl_name, int nchannels, int nlevels, GLsizei w, GLsizei h)
    : Texture1D(gl_name, nchannels, nlevels, w), _h(h)
    {}
    
public:
    virtual Texture::Kind kind();
    
    inline GLsizei height() const { return _h; }
    
    inline void bind() const { gl::BindTexture(GL_TEXTURE_2D, glName()); }
    
    static Texture2D* loadBaseLevel(std::string path, GLenum internal_format, GLenum format, unsigned int flags = Texture::AllocMipmaps);
    
    /** @brief Load mipmaps from a directory.

        The directory must contain mipmap image files calles < mipmap_level >.png
        
        For example:
            0.png
            1.png
            2.png
            etc...
     
        @return Returns a heap allocated Texture2D* handle or nullptr on failure.
     */
    static Texture2D* loadMipmaps(std::string dir_path, GLenum internal_format, GLenum format);
};



    
}//namespace r64fx

#endif//R64FX_GUI_TEXTURE_H