#include "Font.hpp"
#include "StringUtils.hpp"
#include "Image.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>

using namespace std;

namespace r64fx{

namespace{
    int font_instance_count = 0;
    FT_Library g_FT_Library;

#ifdef R64FX_USE_BUILTIN_FONTS
#include "FreeMono_ascii.otf.h"
#include "FreeSans_ascii.otf.h"
#include "FreeSansBold_ascii.otf.h"
#endif//R64FX_USE_BUILTIN_FONTS

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

}//namespace


class FontImpl : public Font{
    FT_Face m_FT_Face = nullptr;
    unsigned char m_Index = 0;
    Image m_Image;

public:
    FontImpl(string name)
    {
        if(font_instance_count == 0)
            init_freetype();

#ifdef R64FX_USE_BUILTIN_FONTS
        if(name == "")
        {
            initBuiltinFont(FreeSans_ascii, sizeof(FreeSans_ascii));
        }
        else if(name == "bold")
        {
            initBuiltinFont(FreeSansBold_ascii, sizeof(FreeSansBold_ascii));
        }
        else if(name == "mono")
        {
            initBuiltinFont(FreeMono_ascii, sizeof(FreeMono_ascii));
        }
#endif//R64FX_USE_BUILTIN_FONTS

        font_instance_count++;
    }

#ifdef R64FX_USE_BUILTIN_FONTS
    void initBuiltinFont(unsigned char* buff, unsigned int size)
    {
        int status = FT_New_Memory_Face(
            g_FT_Library,
            buff, size,
            0, &m_FT_Face
        );

        if(status != 0)
        {
            cerr << "Font: Failed to load built-in font!\n";
            abort();
        }
    }
#endif//R64FX_USE_BUILTIN_FONTS

    virtual ~FontImpl()
    {
        font_instance_count--;
        if(font_instance_count == 0)
            cleanup_freetype();
    }

    virtual bool isGood() { return m_FT_Face != nullptr; };

    virtual int glyphCount() { return isGood() ? m_FT_Face->num_glyphs : 0; }

    virtual void setSize(int char_width, int char_height, int horz_res, int vert_res)
    {
        int code = FT_Set_Char_Size(
            m_FT_Face,
            char_width*64, char_height*64,
            horz_res, vert_res
        );

        if(code != 0)
        {
            cerr << "Font: Failed to set char size!\n";
            abort();
        }
    }

    virtual bool findGlyph(string text)
    {
        m_Index = FT_Get_Char_Index(m_FT_Face, to_utf32(text, 0, text.size()));
        if(m_Index == 0)
        {
            cerr << "Font: Failed to find glyph index!\n";
            return false;
        }
        
        int code = FT_Load_Glyph(
            m_FT_Face, m_Index,
            FT_LOAD_DEFAULT
        );
        
        if(code != 0)
        {
            cerr << "Font: Failed to load glyph!\n";
            return false;
        }
        
        code = FT_Render_Glyph(
            m_FT_Face->glyph,
            FT_RENDER_MODE_NORMAL
        );
        
        if(code != 0)
        {
            cerr << "Font: Failed to render glyph!\n";
            return false;
        }
        
        auto &bitmap = m_FT_Face->glyph->bitmap;
        m_Image.load(
            bitmap.width, bitmap.rows, 1, bitmap.buffer
        );
        
        return true;
    }
    
    virtual Image* image()
    {
        return &m_Image;
    }
};


Font* Font::newInstance(string name)
{
    return new FontImpl(name);
}


void Font::deleteInstance(Font* font)
{
    delete font;
}

}//namespace r64fx