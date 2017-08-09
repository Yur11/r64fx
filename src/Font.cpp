#include "Font.hpp"
#include "StringUtils.hpp"
#include "Image.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <map>

using namespace std;

namespace r64fx{

namespace{
    int font_instance_count = 0;
    FT_Library g_FT_Library;

    void init_freetype()
    {
        if(FT_Init_FreeType(&g_FT_Library) != 0)
        {
            cerr << "Failed to init freetype2!\n";
            abort();
        }
    }

    void cleanup_freetype()
    {
        FT_Done_FreeType(g_FT_Library);
    }

#ifdef R64FX_USE_BUILTIN_FONTS
#include "FreeMono_ascii.otf.h"
#include "FreeSans_ascii.otf.h"
#include "FreeSansBold_ascii.otf.h"

    void init_builtin_font(FT_Face* face, unsigned char* buff, unsigned int size)
    {
        int status = FT_New_Memory_Face(
            g_FT_Library,
            buff, size,
            0, face
        );

        if(status != 0)
        {
            cerr << "Font: Failed to load built-in font!\n";
            abort();
        }
    }
#endif//R64FX_USE_BUILTIN_FONTS

    class GlyphCache : public map<string, Font::Glyph*>{
    public:
        ~GlyphCache()
        {
            clear();
        }

        void clear()
        {
            for(auto &pair : *this)
                delete pair.second;
            map<string, Font::Glyph*>::clear();
        }
    };
}//namespace


#define m_ft_face ((FT_Face)p[0])
#define m_ft_face_p ((FT_Face*)p)
#define m_glyph_cache ((GlyphCache*)p[1])


Font::Font(string name, float size, int dpi)
{
    if(font_instance_count == 0)
        init_freetype();

#ifdef R64FX_USE_BUILTIN_FONTS
    if(name == "")
    {
        init_builtin_font(m_ft_face_p, FreeSans_ascii, sizeof(FreeSans_ascii));
    }
    else if(name == "bold")
    {
        init_builtin_font(m_ft_face_p, FreeSansBold_ascii, sizeof(FreeSansBold_ascii));
    }
    else if(name == "mono")
    {
        init_builtin_font(m_ft_face_p, FreeMono_ascii, sizeof(FreeMono_ascii));
    }
#endif//R64FX_USE_BUILTIN_FONTS

    p[1] = new GlyphCache;

    font_instance_count++;

    int code = FT_Set_Char_Size(
        m_ft_face,
        size * 64, size * 64,
        dpi, dpi
    );

    if(code != 0)
    {
        cerr << "Font: Failed to set char size!\n";
        abort();
    }

    m_ascender  = m_ft_face->size->metrics.ascender  >> 6;
    m_descender = m_ft_face->size->metrics.descender >> 6;
    m_height    = m_ft_face->size->metrics.height    >> 6;
}


Font::~Font()
{
    delete m_glyph_cache;

    font_instance_count--;
    if(font_instance_count == 0)
        cleanup_freetype();
}


Font::Glyph* Font::fetchGlyph(string text)
{
    auto it = m_glyph_cache->find(text);
    if(it == m_glyph_cache->end())
    {
        Font::Glyph* glyph = nullptr;

        if(text == "\n")
        {
            glyph = new Font::Glyph;
            glyph->m_text = "\n";
        }
        else
        {
            int index = FT_Get_Char_Index(m_ft_face, to_utf32(text, 0, text.size()));
            if(index == 0)
            {
                cerr << "Font: Failed to find glyph index!\n";
                return nullptr;
            }

            int code = FT_Load_Glyph(
                m_ft_face, index,
                FT_LOAD_DEFAULT
            );

            if(code != 0)
            {
                cerr << "Font: Failed to load glyph!\n";
                return nullptr;
            }

            code = FT_Render_Glyph(
                m_ft_face->glyph,
                FT_RENDER_MODE_NORMAL
            );

            if(code != 0)
            {
                cerr << "Font: Failed to render glyph!\n";
                return nullptr;
            }

            auto &bitmap = m_ft_face->glyph->bitmap;

            glyph = new Font::Glyph;
            glyph->m_text = text;
            glyph->m_bearing_x  = m_ft_face->glyph->metrics.horiBearingX  >> 6;
            glyph->m_width      = m_ft_face->glyph->metrics.width         >> 6;
            glyph->m_advance    = m_ft_face->glyph->metrics.horiAdvance   >> 6;
            glyph->m_bearing_y  = m_ft_face->glyph->metrics.horiBearingY  >> 6;
            glyph->m_height     = m_ft_face->glyph->metrics.height        >> 6;
            glyph->m_image.load(
                bitmap.width, bitmap.rows, 1, bitmap.buffer,
                true//copy data
            );
        }

        m_glyph_cache->insert({text, glyph});
        return glyph;
    }
    else
    {
        return it->second;
    }
}

}//namespace r64fx
