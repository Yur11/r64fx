#ifndef R64FX_GUI_FONT_H
#define R64FX_GUI_FONT_H

#include <FTGL/ftgl.h>
#include <string>

namespace r64fx{
    
/** @brief Font instances and configuration. */
class Font{
    FTGLTextureFont _ftfont;
    
public:
    Font(const char* path) : _ftfont(path) {}
    
    inline void render(const char* text) { _ftfont.Render(text); }
    
    inline void render(std::string str) { render(str.c_str()); }
    
    inline void setFaceSize(unsigned int size, unsigned int resolution = 72) { _ftfont.FaceSize(size, resolution); }
    
    inline float estimatedTextWidth(const char* str) { return _ftfont.Advance(str); }
    
    inline float estimatedTextWidth(std::string str) { return estimatedTextWidth(str.c_str()); }
    
    inline float ascender() { return _ftfont.Ascender(); }
    
    inline float descender() { return _ftfont.Descender(); }
    
    inline FTTextureFont &ftfont() { return _ftfont; }
};
    
}//namespace r64fx

#endif//R64FX_GUI_FONT_H