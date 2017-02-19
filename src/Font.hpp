#ifndef R64FX_FONT_HPP
#define R64FX_FONT_HPP

#include <string>
#include "Image.hpp"

namespace r64fx{

class Image;

class Font{
    void* p[2];
    int   m_ascender;
    int   m_descender;
    int   m_height;

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

        /* Glyph metrics expressed in pixels. */
        inline int bearing_x() const { return m_bearing_x; }
        inline int width()     const { return m_width; }
        inline int advance()   const { return m_advance; }
        inline int bearing_y() const { return m_bearing_y; }
        inline int height()    const { return m_height; }
    };

    Font(std::string name = "", float size = 16.0f, int dpy = 72);

    ~Font();

    /* Global font metrics expressed in pixels. */
    inline int ascender()   const { return m_ascender; }
    inline int descender()  const { return m_descender; }
    inline int height()     const { return m_height; }

    /* Find and retrieve glyph for the symbol encoded in utf-8. */
    Font::Glyph* fetchGlyph(std::string text);
};

}//namespace r64fx

#endif//R64FX_FONT_HPPs