#include <iostream>
#include <limits>

#include "Font.h"
#include "Projection2D.h"
#include "utf8string/Utf8String.h"
#include "ConvertUTF.h"

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
    
FT_Library       Font::freetype;
Painter*         Font::p;

unsigned char* tmp_buffer = nullptr;
const unsigned int tmp_buffer_size = 1024 * 64;
    

void Font::Glyph::render(float x, float y)
{    
    x = float(int(x)) + 0.0;
    y = float(int(y)) + 0.0;
    
    float dy = height() - bearing_y();
    
    float pos[8] = {
        x,              y + dy - height(),
        x + width(),    y + dy - height(),
        x,              y + dy,
        x + width(),    y + dy,
    };
    
    p->bindBuffer();
    p->setPositions(pos, 8);
    p->unbindBuffer();
    
    Painter::setTexture(texture());
    
    p->bindArray();
    p->render(GL_TRIANGLE_STRIP);
    p->unbindArray();
}


Font::Glyph* Font::fetchGlyph(std::string utf8_char)
{
#ifdef DEBUG
    assert(!utf8_char.empty());
#endif//DEBUG
    
    auto it = _index.find(utf8_char);
    if(it != _index.end())
        return it->second;
    
    const UTF8* source_begin = (const UTF8*)utf8_char.c_str();
    UTF32 utf32_code = 0;
    UTF32* target_begin = &utf32_code;
    
    if(ConvertUTF8toUTF32(&source_begin, source_begin+utf8_char.size(), &target_begin, target_begin+1, strictConversion) != conversionOK)
    {
        cerr << "Failed to convert utf8 to utf32!\n";
        return nullptr;
    }

    auto glyph_index = FT_Get_Char_Index(_ft_face, utf32_code);
    
    if(FT_Load_Glyph(_ft_face, glyph_index, FT_LOAD_DEFAULT) != 0)
    {
        cerr << "Failed to load glyph for utf32 code: " << utf32_code << "\n";
        return nullptr;
    }
    
    if(FT_Render_Glyph(_ft_face->glyph, FT_RENDER_MODE_NORMAL) != 0)
    {
        cerr << "Failed to render glyph for utf32 code: " << utf32_code << "\n";
        return nullptr;
    }
    
    auto &bitmap = _ft_face->glyph->bitmap;
#ifdef DEBUG
    assert(_ft_face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY);
#endif//DEBUG
    
    unsigned char* data = bitmap.buffer;
    int w = bitmap.width;
    int h = bitmap.rows;
    
    
    Glyph* g = new Glyph;
    g->m_index = glyph_index;
    
    if(w > 0)
    {
        /* Width has to be divisible by 4. */
        if(w % 4)
        {
            auto diff = 4 - (w % 4);
            w+=diff;
            
            int nbytes = w*h;
#ifdef DEBUG
            assert(nbytes <= (int)tmp_buffer_size);
#endif//DEBUG

            for(int y=0; y<bitmap.rows; y++)
            {
                for(int x=0; x<bitmap.width; x++)
                {
                    *(tmp_buffer + y * w + x) = *(bitmap.buffer + y * bitmap.width + x);
                }
            }

            for(int y=0; y<bitmap.rows; y++)
            {
                for(int i=1; i<=diff; i++)
                    *(tmp_buffer + y * w + w-i) = 0;
            }
            
            data = tmp_buffer;
        }
        
        GLuint tex;
        gl::GenTextures(1, &tex);
        gl::BindTexture(GL_TEXTURE_2D, tex);
        gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl::TexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, GL_RED, GL_UNSIGNED_BYTE, data);
        
        g->m_tex = tex;
    }
    else
    {
        g->m_tex = 0;
    }
    
    g->m_width = w;
    g->m_height = h;
    
    g->m_bearing_x = float(_ft_face->glyph->metrics.horiBearingX) / 64.0;
    g->m_bearing_y = float(_ft_face->glyph->metrics.horiBearingY) / 64.0;
    g->m_advance   = float(_ft_face->glyph->metrics.horiAdvance) / 64.0;
    
    _index[utf8_char] = g;
    return g;
}


bool Font::init()
{
    if(FT_Init_FreeType(&freetype) != 0)
    {
        cerr << "Failed to init FreeType!\n";
        return false;
    }
    
    p = new Painter(4);
    
    float data[16] = {
        0.0, 0.0,
        0.0, 0.0,
        0.0, 0.0,
        0.0, 0.0,
        
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
    };
    
    p->bindBuffer();
    p->setData(data);
    p->unbindBuffer();
    
    tmp_buffer = new unsigned char[tmp_buffer_size];
    
    return true;
}


Font::Font(std::string file_path, int size)
{
    if(FT_New_Face(freetype, file_path.c_str(), 0, &_ft_face))
    {
        cerr << "Failed to read font!\n";
        cerr << file_path << "\n";
        return;
    }
    
    if(FT_Set_Char_Size(_ft_face, 0, size * 64, 96, 96) != 0)
//     if(FT_Set_Pixel_Sizes(_ft_face, 0, size) != 0)
    {
        cerr << "Failed to set font size to " << size << " pixels!\n";
        return;
    }
    
    _height = _ft_face->size->metrics.height / 64;
    _ascender = _ft_face->size->metrics.ascender / 64;
    _descender = _ft_face->size->metrics.descender / 64;
    
    _has_kerning = FT_HAS_KERNING(_ft_face);
    _is_ok = true;
}


Font::~Font()
{
    for(auto p : _index)
    {
        delete p.second;
    }
}


void Font::render(std::string utf8_text)
{    
    Utf8String utf8_str;
    utf8_str.stdstr = utf8_text;
    auto str_size = utf8_str.size();
    
    FT_UInt prev_index;
    
    for(int i=0; i<str_size; i++)
    {
        auto glyph = fetchGlyph(utf8_str[i]);
#ifdef DEBUG
        assert(glyph != nullptr);
#endif//DEBUG
    
        if(_pen_x == 0)
            _pen_x += glyph->bearing_x();
        
        if(_has_kerning && i > 0)
        {
            FT_Vector delta; 
            FT_Get_Kerning(_ft_face, prev_index, glyph->index(), FT_KERNING_DEFAULT, &delta); 
            _pen_x += delta.x >> 6;
        }
    
        if(glyph->texture() != 0)
        {
            glyph->render(_pen_x + glyph->bearing_x(), _pen_y - 0.5);
        }

        _pen_x += glyph->advance();
        
        prev_index = glyph->index();
    }    
}

void Font::renderChar(std::string utf8_char)
{
    auto glyph = fetchGlyph(utf8_char);
#ifdef DEBUG
    assert(glyph != nullptr);
#endif//DEBUG
        
    glyph->render(_pen_x, _pen_y);
    _pen_x += glyph->advance();
}


Rect<float> Font::calculateBoundingBox(std::string utf8_text)
{
    float left = _pen_x;
    float right = left;
    float top = numeric_limits<float>::max();
    float bottom = - numeric_limits<float>::max();
    
    Utf8String utf8_str;
    utf8_str.stdstr = utf8_text;
    auto str_size = utf8_str.size();
    
    FT_UInt prev_index;
    
    for(int i=0; i<str_size; i++)
    {
        auto glyph = fetchGlyph(utf8_str[i]);
#ifdef DEBUG
        assert(glyph != nullptr);
#endif//DEBUG
    
        if(_pen_x == 0)
            _pen_x += glyph->bearing_x();
        
        if(_has_kerning && i > 0)
        {
            FT_Vector delta; 
            FT_Get_Kerning(_ft_face, prev_index, glyph->index(), FT_KERNING_DEFAULT, &delta); 
            _pen_x += delta.x >> 6;
        }

        float top_y = _pen_y - glyph->bearing_y();
        float bottom_y = top_y + glyph->height();
        
        if(top_y < top)
            top = top_y;
        
        if(bottom_y > bottom)
            bottom = bottom_y;

        _pen_x += glyph->advance();
        right = _pen_x;
        
        prev_index = glyph->index();
    }  

    _pen_x = left; // We haven't rendered anyting. 
                   // Return pen to the original position.
    
    return { left, top, right, bottom };
}


float Font::lineAdvance(std::string utf8_text)
{
    float advance = 0.0;
    
    Utf8String utf8_str;
    utf8_str.stdstr = utf8_text;
    auto str_size = utf8_str.size();
    
    FT_UInt prev_index;
    
    for(int i=0; i<str_size; i++)
    {
        auto glyph = fetchGlyph(utf8_str[i]);
#ifdef DEBUG
        assert(glyph != nullptr);
#endif//DEBUG

        if(_has_kerning && i > 0)
        {
            FT_Vector delta; 
            FT_Get_Kerning(_ft_face, prev_index, glyph->index(), FT_KERNING_DEFAULT, &delta); 
            advance -= delta.x >> 6;
        }
        
        advance += glyph->advance();
        prev_index = glyph->index();
    }
    
    return advance;
}
    
    
float Font::charAdvance(std::string utf8_char)
{
    auto glyph = fetchGlyph(utf8_char);
#ifdef DEBUG
    assert(glyph != nullptr);
#endif//DEBUG
    
    return glyph->advance();
}


Font* Font::_default_font = nullptr;

std::map<std::string, Font*> Font::_common_fonts;


void Font::addCommonFont(std::string name, Font* font)
{
    _common_fonts[name] = font;
}


Font* Font::find(std::string name)
{
    auto it = _common_fonts.find(name);
    if(it == _common_fonts.end())
        return _default_font;
    else
        return it->second;
}
    
}//namespace r64fx
