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
    

    /* Remove this! */
    static void initDefaultFont(Font* font);

    /* Remove this! */
    static Font* defaultFont();
    
    class Error{
        std::string _message;
        
    public:
        Error(std::string message) : _message(message) {}
        
        inline std::string message() const { return _message; }
    };
};
    
}//namespace r64fx

#endif//R64FX_GUI_FONT_H