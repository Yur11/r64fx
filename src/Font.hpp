#ifndef R64FX_FONT_HPP
#define R64FX_FONT_HPP

#include <string>
#include "Image.hpp"

namespace r64fx{

class Image;
    
class Font{
    void* p[2];
    int m_glyph_count  = 0;
    int m_height       = 0;
    int m_ascender     = 0;
    int m_descender    = 0;
    int m_char_width   = 0;
    int m_char_height  = 0;
    
public:
    class Glyph{
        friend class Font;
        
        std::string m_text;
        
        Image m_image;
        
        int m_bearing_x  = 0;
        int m_width      = 0;
        int m_advance    = 0;
        int m_bearing_y  = 0;
        int m_height     = 0;
        
    public:
        inline std::string text() const { return m_text; }
        
        inline Image* image() { return &m_image; }
        
        inline int bearing_x() const { return m_bearing_x; }
        inline int width()     const { return m_width; }
        inline int advance()   const { return m_advance; }
        inline int bearing_y() const { return m_bearing_y; }
        inline int height()    const { return m_height; }
        
        inline int ascent()    const { return bearing_y(); }
        inline int descent()   const { return height() - ascent(); }
    };
    
    Font(std::string name = "");
    
    ~Font();

    inline int glyphCount() const { return m_glyph_count; }
    inline int ascender()   const { return m_ascender; };
    inline int descender()  const { return m_descender; };

    void setSize(int char_width, int char_height, int horz_res, int vert_res);

    inline int charWidth()  const { return m_char_width; }
    inline int charHeight() const { return m_char_height; }
    
    /** @brief Find and retrieve glyph for the symbol encoded in utf-8. */
    Font::Glyph* fetchGlyph(std::string text);
};

}//namespace r64fx

#endif//R64FX_FONT_HPPs