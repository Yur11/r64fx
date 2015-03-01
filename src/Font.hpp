#ifndef R64FX_GUI_FONT_H
#define R64FX_GUI_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include "Painter.hpp"
#include "RenderingContext.hpp"
#include "Rect.hpp"
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
    
public:
    class Glyph{
        friend class Font;
        unsigned char* m_bitmap = nullptr;
        
        GLuint m_tex = 0;
        
        int m_width = 0.0;
        int m_height = 0.0;
        
        int m_bearing_x = 0.0;
        int m_bearing_y = 0.0;
        int m_advance = 0.0;
        
        unsigned int m_index;
        
        /** @brief Glyph ctor. 
         
            Build a glyph instance from the given data.
            
            @param width       Width of the bitmap. May be increased to become divisble by 4.
            
            @param height      Height of the bitmap.
            
            @param bearing_x   Distance from the origin.
            
            @param bearing_y   Hight above the base line.
            
            @param index       FreeType glyph index.
            
            @param bitmap      Bitmap buffer of size width*height.
         */
        Glyph(int width, int height, int bearing_x, int bearing_y, int advance, unsigned int index, unsigned char* bitmap);
                
    public:
        inline unsigned char* bitmap() const { return m_bitmap; }
        
        inline bool hasBitmap() const { return m_bitmap != nullptr; }
        
        inline GLuint texture() const { return m_tex; }
        
        inline bool hasTexture() const { return m_tex != 0; }
        
        inline float width() const { return m_width; }
        inline float height() const { return m_height; }
        
        inline float bearing_x() const { return m_bearing_x; }
        inline float bearing_y() const { return m_bearing_y; }
        inline float advance() const { return m_advance; }
        
        inline unsigned int index() const { return m_index; }

        void freeBitmap();
        
        void loadTexture();
        
        void freeTexture();
        
        void render(float x, float y);        
    };
    
private:
    
    std::map<std::string, Glyph*> _index;
    
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
   
    Font::Glyph* fetchGlyph(std::string utf8_char);
   
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
