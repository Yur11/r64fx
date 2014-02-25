#include <iostream>

#include "Font.h"
#include "Projection2D.h"
#include "Error.h"
#include "utf8string/Utf8String.h"
#include "ConvertUTF.h"

#ifdef DEBUG
#include <assert.h>
#endif//DEBUG

using namespace std;

namespace r64fx{
    
GLuint           Font::vao[max_rendering_context_count];
GLuint           Font::vbo;
FT_Library       Font::freetype;
ShadingProgram   Font::font_shading_program;
GLint            Font::vertex_coord_attribute;
GLint            Font::x_uniform;
GLint            Font::y_uniform;
GLint            Font::w_uniform;
GLint            Font::h_uniform;
GLint            Font::r_uniform;
GLint            Font::g_uniform;
GLint            Font::b_uniform;
GLint            Font::a_uniform;
GLint            Font::projection_uniform;
GLint            Font::glyph_width_coeff_uniform;
GLuint           Font::glyph_sampler;
GLint            Font::glyph_sampler_uniform;


unsigned char* tmp_buffer = nullptr;
const unsigned int tmp_buffer_size = 1024 * 64;
    

void Font::Glyph::render(float x, float y)
{
    glUniform1f(x_uniform, x);
    CHECK_FOR_GL_ERRORS; 
    glUniform1f(y_uniform, y - height + bearing_y);
    CHECK_FOR_GL_ERRORS; 
    glUniform1f(w_uniform, width);
    CHECK_FOR_GL_ERRORS; 
    glUniform1f(h_uniform, height);
    CHECK_FOR_GL_ERRORS; 
    glUniform1f(glyph_width_coeff_uniform, width_coeff);
    CHECK_FOR_GL_ERRORS; 
    glActiveTexture(GL_TEXTURE0 + 0);
    CHECK_FOR_GL_ERRORS;
    glBindTexture(GL_TEXTURE_2D, tex);
    CHECK_FOR_GL_ERRORS; 
//     glBindSampler(0, glyph_sampler);
//     CHECK_FOR_GL_ERRORS; 
    glUniform1i(glyph_sampler_uniform, 0);
    CHECK_FOR_GL_ERRORS; 
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    CHECK_FOR_GL_ERRORS; 
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
    g->index = glyph_index;
    
    if(w > 0)
    {
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
        glGenTextures(1, &tex);
        CHECK_FOR_GL_ERRORS; 
        glBindTexture(GL_TEXTURE_2D, tex);
        CHECK_FOR_GL_ERRORS; 
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        CHECK_FOR_GL_ERRORS; 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        CHECK_FOR_GL_ERRORS; 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        CHECK_FOR_GL_ERRORS;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECK_FOR_GL_ERRORS;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//         glTexStorage2D(GL_TEXTURE_2D, 4, GL_R8, w, h);
//         CHECK_FOR_GL_ERRORS;
//         glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RED, GL_UNSIGNED_BYTE, data);
//         CHECK_FOR_GL_ERRORS;
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        
        g->tex = tex;
    }
    else
    {
        g->tex = 0;
    }
    
    g->width = bitmap.width;
    g->height = h;
    g->width_coeff = float(bitmap.width) / float(w);
    
    g->bearing_x = float(_ft_face->glyph->metrics.horiBearingX) / 64.0;
    g->bearing_y = float(_ft_face->glyph->metrics.horiBearingY) / 64.0;
    g->advance   = float(_ft_face->glyph->metrics.horiAdvance) / 64.0;
    
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
    
    VertexShader vs(
        #include "Font.vert.h"
    );
    
    if(!vs.isOk())
    {
        cerr << "Error in vertex shader!\n";
        cerr << vs.infoLog() << "\n";
        return false;
    }
    
    FragmentShader fs(
        #include "Font.frag.h"
    );
    
    if(!fs.isOk())
    {
        cerr << "Error in fragment shader!\n";
        cerr << fs.infoLog() << "\n";
        return false;
    }
    
    font_shading_program = ShadingProgram(vs, fs);
    if(!font_shading_program.isOk())
    {
        cerr << "Error with shading program!\n";
        cerr << font_shading_program.infoLog();
        return false;
    }
    
    glGenBuffers(1, &vbo);
    
    float vbo_data[8] = {
        0.0, 1.0,  1.0, 1.0,
        0.0, 0.0,  1.0, 0.0
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    CHECK_FOR_GL_ERRORS; 
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vbo_data, GL_STATIC_DRAW);
    CHECK_FOR_GL_ERRORS; 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_FOR_GL_ERRORS; 
    
    vertex_coord_attribute = glGetAttribLocation(font_shading_program.id(), "vertex_coord");
    CHECK_FOR_GL_ERRORS; 
#ifdef DEBUG
    assert(vertex_coord_attribute != -1);
#endif//DEBUG
    
    
    x_uniform = glGetUniformLocation(font_shading_program.id(), "x");
    CHECK_FOR_GL_ERRORS; 
    y_uniform = glGetUniformLocation(font_shading_program.id(), "y");
    CHECK_FOR_GL_ERRORS; 
    w_uniform = glGetUniformLocation(font_shading_program.id(), "w");
    CHECK_FOR_GL_ERRORS; 
    h_uniform = glGetUniformLocation(font_shading_program.id(), "h");
    CHECK_FOR_GL_ERRORS; 
    r_uniform = glGetUniformLocation(font_shading_program.id(), "r");
    CHECK_FOR_GL_ERRORS; 
    g_uniform = glGetUniformLocation(font_shading_program.id(), "g");
    CHECK_FOR_GL_ERRORS; 
    b_uniform = glGetUniformLocation(font_shading_program.id(), "b");
    CHECK_FOR_GL_ERRORS; 
    a_uniform = glGetUniformLocation(font_shading_program.id(), "a");
    CHECK_FOR_GL_ERRORS; 
    projection_uniform = glGetUniformLocation(font_shading_program.id(), "sxsytxty");
    CHECK_FOR_GL_ERRORS; 
    glyph_width_coeff_uniform = glGetUniformLocation(font_shading_program.id(), "glyph_width_coeff");
    CHECK_FOR_GL_ERRORS; 
    glyph_sampler_uniform = glGetUniformLocation(font_shading_program.id(), "glyph_sampler");
    CHECK_FOR_GL_ERRORS; 
   
#ifdef DEBUG
    assert(x_uniform != -1);
    assert(y_uniform != -1);
    assert(w_uniform != -1);
    assert(h_uniform != -1);
    assert(r_uniform != -1);
    assert(g_uniform != -1);
    assert(b_uniform != -1);
    assert(a_uniform != -1);
    assert(projection_uniform != -1);
    assert(glyph_width_coeff_uniform != -1);
    assert(glyph_sampler_uniform != -1);
#endif//DEBUG
    
    glGenSamplers(1, &glyph_sampler);
    CHECK_FOR_GL_ERRORS; 
    
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
    
    if(FT_Set_Pixel_Sizes(_ft_face, 0, size) != 0)
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


void Font::setupForContext(RenderingContextId_t context_id)
{
    cout << "Font: setup for context: " << context_id << "\n";
    
    if(vao[context_id] != 0)
    {
#ifdef DEBUG
        cerr << "Font: Double setup for context: " << context_id << "\n";
#endif//DEBUG
        return;
    }
    
    glGenVertexArrays(1, vao + context_id);
    CHECK_FOR_GL_ERRORS; 
    glBindVertexArray(vao[context_id]);
    CHECK_FOR_GL_ERRORS; 
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    CHECK_FOR_GL_ERRORS; 
    glEnableVertexAttribArray(vertex_coord_attribute);
    CHECK_FOR_GL_ERRORS; 
    glVertexAttribPointer(vertex_coord_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
    CHECK_FOR_GL_ERRORS; 
    glBindVertexArray(0);
    CHECK_FOR_GL_ERRORS; 
}


void Font::cleanupForContext(RenderingContextId_t context_id)
{
    cout << "Font: cleanup for context: " << context_id << "\n";
    
    if(vao[context_id] == 0)
    {
#ifdef DEBUG
        cerr << "Font: Double cleanup for context: " << context_id << "\n";
#endif//DEBUG
        return;
    }
    
    glDeleteVertexArrays(1, vao + context_id);
}


void Font::prepare()
{
    font_shading_program.use();
    glUniform4fv(projection_uniform, 1, current_2d_projection->vec);
}
    
    
void Font::setR(float r)
{
    glUniform1f(r_uniform, r);
}


void Font::setG(float g)
{
    glUniform1f(g_uniform, g);
}


void Font::setB(float b)
{
    glUniform1f(b_uniform, b);
}


void Font::setA(float a)
{
    glUniform1f(a_uniform, a);
}


void Font::setRGBA(float r, float g, float b, float a)
{
    setR(r);
    setG(g);
    setB(b);
    setA(a);
}


void Font::render(std::string utf8_text)
{
    auto context_id = RenderingContext::current()->id();
    
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
            FT_Get_Kerning(_ft_face, prev_index, glyph->index, FT_KERNING_DEFAULT, &delta); 
            _pen_x -= delta.x >> 6;
        }
    
        if(glyph->tex != 0)
        {
            glBindVertexArray(vao[context_id]);
            CHECK_FOR_GL_ERRORS; 
            glyph->render(_pen_x, _pen_y);
            glBindVertexArray(0);
            CHECK_FOR_GL_ERRORS; 
        }

        _pen_x += glyph->advance;
        
        prev_index = glyph->index;
    }    
}

void Font::renderChar(std::string utf8_char)
{
    auto glyph = fetchGlyph(utf8_char);
#ifdef DEBUG
    assert(glyph != nullptr);
#endif//DEBUG
    
    font_shading_program.use();
    
    glyph->render(_pen_x, _pen_y);
    _pen_x += glyph->advance;
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
            FT_Get_Kerning(_ft_face, prev_index, glyph->index, FT_KERNING_DEFAULT, &delta); 
            advance -= delta.x >> 6;
        }
        
        advance += glyph->advance;
        prev_index = glyph->index;
    }
    
    return advance;
}
    
    
float Font::charAdvance(std::string utf8_char)
{
    auto glyph = fetchGlyph(utf8_char);
#ifdef DEBUG
    assert(glyph != nullptr);
#endif//DEBUG
    
    return glyph->advance;
}
    
}//namespace r64fx
