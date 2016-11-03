#ifdef R64FX_USE_GL

#ifndef R64FX_PAINTER_SHADERS_HPP
#define R64FX_PAINTER_SHADERS_HPP

#include "Shader.hpp"

namespace r64fx{
    
class PainterShader : public ShadingProgram{
    GLint unif_sxsytxty;
    GLint attr_position;
    
public:
    PainterShader();
    
    ~PainterShader();
    
    void setScaleAndShift(float sx, float sy, float tx, float ty);
    
    void bindPositionAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer);
    
protected:
    void fetchCommonIndices();
};
    
    
class Shader_Color : public PainterShader{
    GLint unif_color;

public:
    Shader_Color();

    ~Shader_Color();
    
    void setColor(float r, float g, float b, float a);
};
    
    
class Shader_Texture : public PainterShader{
    GLint unif_sampler;
    GLint attr_tex_coord;

public:
    Shader_Texture();

    ~Shader_Texture();

    void setSampler(int sampler);
    
    void bindTexCoordAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer);
    
protected:
    void fetchCommonIndices();
};


class Shader_ColorBlend : public Shader_Texture{
    GLint unif_color;
    GLint unif_texture_component;
    
public:
    Shader_ColorBlend();
    
    ~Shader_ColorBlend();
    
    void setColor(float r, float g, float b, float a);
    
    void setTextureComponent(int component);
};
    
}//namespace r64fx

#endif//R64FX_PAINTER_SHADERS_HPP

#endif//R64FX_USE_GL

