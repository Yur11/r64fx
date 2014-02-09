#include <iostream>

#include "Font.h"
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
GLint            Font::glyph_width_coeff_uniform;
GLuint           Font::glyph_sampler;
GLint            Font::glyph_sampler_uniform;


unsigned char* tmp_buffer = nullptr;
const unsigned int tmp_buffer_size = 1024 * 64;
    

void Font::Glyph::render(float x, float y)
{
    glUniform1f(x_uniform, x);
    glUniform1f(y_uniform, y - height + bearing_y);
    glUniform1f(w_uniform, width);
    glUniform1f(h_uniform, height);
    glUniform1f(glyph_width_coeff_uniform, width_coeff);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindSampler(tex, glyph_sampler);
    glUniform1f(glyph_sampler_uniform, glyph_sampler);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
    
    Glyph* g = new Glyph;
    g->tex = tex;
    g->width = bitmap.width;
    g->height = h;
    g->width_coeff = float(bitmap.width) / float(w);
    
    g->bearing_x = float(_ft_face->glyph->metrics.horiBearingX) / 64.0;
    g->bearing_y = float(_ft_face->glyph->metrics.horiBearingY) / 64.0;
    g->advance   = float(_ft_face->glyph->metrics.horiAdvance) / 64.0;
    
    g->index = glyph_index;
    
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
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vbo_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    vertex_coord_attribute = glGetAttribLocation(font_shading_program.id(), "vertex_coord");
#ifdef DEBUG
    assert(vertex_coord_attribute != -1);
#endif//DEBUG
    
    
    x_uniform = glGetUniformLocation(font_shading_program.id(), "x");
    y_uniform = glGetUniformLocation(font_shading_program.id(), "y");
    w_uniform = glGetUniformLocation(font_shading_program.id(), "w");
    h_uniform = glGetUniformLocation(font_shading_program.id(), "h");
    r_uniform = glGetUniformLocation(font_shading_program.id(), "r");
    g_uniform = glGetUniformLocation(font_shading_program.id(), "g");
    b_uniform = glGetUniformLocation(font_shading_program.id(), "b");
    a_uniform = glGetUniformLocation(font_shading_program.id(), "a");
    glyph_width_coeff_uniform = glGetUniformLocation(font_shading_program.id(), "glyph_width_coeff");
    glyph_sampler_uniform = glGetUniformLocation(font_shading_program.id(), "glyph_sampler");
   
#ifdef DEBUG
    assert(x_uniform != -1);
    assert(y_uniform != -1);
    assert(w_uniform != -1);
    assert(h_uniform != -1);
    assert(r_uniform != -1);
    assert(g_uniform != -1);
    assert(b_uniform != -1);
    assert(a_uniform != -1);
    assert(glyph_width_coeff_uniform != -1);
    assert(glyph_sampler_uniform != -1);
#endif//DEBUG
    
    glGenSamplers(1, &glyph_sampler);
    
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
    glBindVertexArray(vao[context_id]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(vertex_coord_attribute);
    glVertexAttribPointer(vertex_coord_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);
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
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
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
    
        glBindVertexArray(vao[context_id]);
        glyph->render(_pen_x, _pen_y);
        _pen_x += glyph->advance;
        glBindVertexArray(0);
        
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
