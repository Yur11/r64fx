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


#define M_FT_FACE ((FT_Face)p[0])
#define M_FT_FACE_P ((FT_Face*)p)
#define M_GLYPH_CACHE ((GlyphCache*)p[1])


Font::Font(string name)
{
    if(font_instance_count == 0)
        init_freetype();

#ifdef R64FX_USE_BUILTIN_FONTS
    if(name == "")
    {
        init_builtin_font(M_FT_FACE_P, FreeSans_ascii, sizeof(FreeSans_ascii));
    }
    else if(name == "bold")
    {
        init_builtin_font(M_FT_FACE_P, FreeSansBold_ascii, sizeof(FreeSansBold_ascii));
    }
    else if(name == "mono")
    {
        init_builtin_font(M_FT_FACE_P, FreeMono_ascii, sizeof(FreeMono_ascii));
    }
#endif//R64FX_USE_BUILTIN_FONTS

    p[1] = new GlyphCache;
    m_glyph_count = M_FT_FACE->num_glyphs;
    
    font_instance_count++;
}


Font::~Font()
{
    delete M_GLYPH_CACHE;
    
    font_instance_count--;
    if(font_instance_count == 0)
        cleanup_freetype();
}


void Font::setSize(int char_width, int char_height, int horz_res, int vert_res)
{
    if(char_width != m_char_width || char_height != m_char_height)
    {
        M_GLYPH_CACHE->clear();
    }
    
    int code = FT_Set_Char_Size(
        M_FT_FACE,
        char_width << 6, char_height << 6,
        horz_res, vert_res
    );

    if(code != 0)
    {
        cerr << "Font: Failed to set char size!\n";
        abort();
    }
    
    m_height       = M_FT_FACE->height     >> 6;
    m_ascender     = M_FT_FACE->ascender   >> 6;
    m_descender    = M_FT_FACE->descender  >> 6;
    m_char_width   = char_width;
    m_char_height  = char_height;
}


Font::Glyph* Font::fetchGlyph(string text)
{
    auto it = M_GLYPH_CACHE->find(text);
    if(it == M_GLYPH_CACHE->end())
    {
        int index = FT_Get_Char_Index(M_FT_FACE, to_utf32(text, 0, text.size()));
        if(index == 0)
        {
            cerr << "Font: Failed to find glyph index!\n";
            return nullptr;
        }
        
        int code = FT_Load_Glyph(
            M_FT_FACE, index,
            FT_LOAD_DEFAULT
        );
        
        if(code != 0)
        {
            cerr << "Font: Failed to load glyph!\n";
            return nullptr;
        }
        
        code = FT_Render_Glyph(
            M_FT_FACE->glyph,
            FT_RENDER_MODE_NORMAL
        );
        
        if(code != 0)
        {
            cerr << "Font: Failed to render glyph!\n";
            return nullptr;
        }
        
        auto &bitmap = M_FT_FACE->glyph->bitmap;
        
        auto glyph = new Font::Glyph;
        glyph->m_text = text;
        glyph->m_image.load(
            bitmap.width, bitmap.rows, 1, bitmap.buffer
        );
        glyph->m_bearing_x  = M_FT_FACE->glyph->metrics.horiBearingX  >> 6;
        glyph->m_width      = M_FT_FACE->glyph->metrics.width         >> 6;
        glyph->m_advance    = M_FT_FACE->glyph->metrics.horiAdvance   >> 6;
        glyph->m_bearing_y  = M_FT_FACE->glyph->metrics.horiBearingY  >> 6;
        glyph->m_height     = M_FT_FACE->glyph->metrics.height        >> 6;
        
        M_GLYPH_CACHE->insert({text, glyph});
        
        return glyph;
    }
    else
    {
        return it->second;
    }    
}

}//namespace r64fx