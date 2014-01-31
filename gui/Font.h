#ifndef R64FX_GUI_FONT_H
#define R64FX_GUI_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include "Shader.h"
#include "RenderingContext.h"
#include <string>
#include <map>

namespace r64fx{
    
class Font{
    static GLuint rect_vbo;
    static FT_Library freetype;
    static ShadingProgram font_shading_program;
    static GLint vertex_coord_attribute;
    static GLint x_uniform;
    static GLint y_uniform;
    static GLint w_uniform;
    static GLint h_uniform;
    static GLint r_uniform;
    static GLint g_uniform;
    static GLint b_uniform;
    static GLint a_uniform;
    static GLint glyph_width_coeff_uniform;
    static GLuint glyph_sampler;
    static GLint glyph_sampler_uniform;
    
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
        float width_coeff; //Used to correct the right tex coord in the shaders,
                           //in case the texture image got padded to be a multipe of 4.
                           //This is needed for mipmapping to work properly.
        
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
    
    ~Font();
    
    inline bool isOk() const { return _is_ok; }
    
    inline float height() const { return _height; }
    
    inline float ascender() const { return _ascender; }
    
    inline float descender() const { return _descender; }
    
    void prepare();
    
    void setR(float r);
    
    void setG(float r);
    
    void setB(float r);
    
    void setA(float r);
        
    void setRGBA(float r, float g, float b, float a);
    
    void render(RenderingContextId_t context_id, std::string utf8_text);
    
    void renderChar(std::string utf8_char);
    
    float lineAdvance(std::string utf8_text);
    
    float charAdvance(std::string utf8_char);
    
    inline void setPenX(float x) { _pen_x = x; };
    inline void setPenY(float y) { _pen_y = y; };
    
    inline float penX() const { return _pen_x; }
    inline float penY() const { return _pen_y; }
};
    
}//namespace r64fx

#endif//R64FX_GUI_FONT_H
