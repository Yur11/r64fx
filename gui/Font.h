#ifndef R64FX_GUI_FONT_H
#define R64FX_GUI_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include "Painter.h"
#include "RenderingContext.h"
#include "Rect.h"
#include <string>
#include <map>

namespace r64fx{
    
class Font{
    static FT_Library freetype;
    static Painter* p;
    
    FT_Face _ft_face;
    float _height;
    float _ascender;
    float _descender;
    bool _has_kerning;
    bool _is_ok = false;
    
    struct Glyph{
        friend class Font;
        GLuint tex;
        
        float width;
        float height;
        
        float bearing_x;
        float bearing_y;
        float advance;
        
        unsigned int index;
        
        void render(float x, float y);        
    };
    
    std::map<std::string, Glyph*> _index;
    
    Font::Glyph* fetchGlyph(std::string utf8_char);
    
    float _pen_x = 0.0;
    float _pen_y = 0.0;
    
public:
    static bool init();
    
    Font(std::string file_path, int size);
    
    virtual ~Font();
    
    inline bool isOk() const { return _is_ok; }
    
    inline float height() const { return _height; }
    
    inline float ascender() const { return _ascender; }
    
    inline float descender() const { return _descender; }
    
    void enable() { /*Painter::enable();*/ Painter::setTexturingMode(Painter::RedAsAlpha); }
    
    inline static void disable() { Painter::setTexturingMode(Painter::RGBA); }
    
//     inline static void useCurrent2dProjection() { Painter::useCurrent2dProjection(); }
        
    inline static void setColor(Color color) { Painter::setColor(color); }
    
    inline void setColor(float r, float g, float b, float a) { setColor({ r, g, b, a }); }
    
    void render(std::string utf8_text);
    
    void renderChar(std::string utf8_char);
    
    Rect<float> calculateBoundingBox(std::string utf8_text);
    
    float lineAdvance(std::string utf8_text);
    
    float charAdvance(std::string utf8_char);
    
    inline void setPenX(float x) { _pen_x = x; };
    inline void setPenY(float y) { _pen_y = y; };
    
    inline float penX() const { return _pen_x; }
    inline float penY() const { return _pen_y; }
   
private:
    static Font* _default_font;
    
    static std::map<std::string, Font*> _common_fonts;
   
public:
    inline static void setDefaultFont(Font* font)
    {
        _default_font = font;
    }
    
    inline static Font* defaultFont()
    {
        return _default_font;
    }
    
    static void addCommonFont(std::string name, Font* font);
    
    static Font* find(std::string name);
};
    
}//namespace r64fx

#endif//R64FX_GUI_FONT_H
