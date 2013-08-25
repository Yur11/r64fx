#ifndef R64FX_GUI_FONT_H
#define R64FX_GUI_FONT_H

#include <FTGL/ftgl.h>
#include <string>

namespace r64fx{
    
/** @brief Font instances and configuration. */
class Font{
    FTGLTextureFont _ftfont;
    
public:
    Font(std::string path) : _ftfont(path.c_str()) {}
    
    Font(const Font &other) : _ftfont(other._ftfont) {};
    
    Font(const unsigned char* bytes, unsigned int nbytes) : _ftfont(FTGLTextureFont(bytes, nbytes)) {}

    Font(FTGLTextureFont ftfont) : _ftfont(ftfont) {}
    
    inline bool isOk() { return !_ftfont.Error(); }
    
    inline void render(const char* text) { _ftfont.Render(text); }
    
    inline void render(std::string str) { render(str.c_str()); }
    
    inline void setFaceSize(unsigned int size, unsigned int resolution = 72) { _ftfont.FaceSize(size, resolution); }
    
    inline float estimatedTextWidth(const char* str) { return _ftfont.Advance(str); }
    
    inline float estimatedTextWidth(std::string str) { return estimatedTextWidth(str.c_str()); }
    
    inline float ascender() { return _ftfont.Ascender(); }
    
    inline float descender() { return _ftfont.Descender(); }
    
    inline FTTextureFont &ftfont() { return _ftfont; }
    
    static void initDefaultFont(Font* font);

    static Font* defaultFont();
    
    /** @brief Load a font from file and add it to the font library with a given font name.
     
        @return Returns nullptr if the font has failed to load.
     */
    static Font* loadFont(std::string fontname, std::string path);
    
    /** @brief Find a font in the font library by the name. 
     
        @return Function returns nullptr on failure.
     */
    static Font* find(std::string fontname);
};
    
}//namespace r64fx

#endif//R64FX_GUI_FONT_H