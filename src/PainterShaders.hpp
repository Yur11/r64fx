#ifdef R64FX_USE_GL

#ifndef R64FX_PAINTER_SHADERS_HPP
#define R64FX_PAINTER_SHADERS_HPP

#include "Shader.hpp"

#ifndef R64FX_PAINTER_SHADER_IMPL
#define R64FX_PAINTER_SHADER_EXTERN extern
#define R64FX_PAINTER_SHADER_NULLPTR
#else
#define R64FX_PAINTER_SHADER_EXTERN
#define R64FX_PAINTER_SHADER_NULLPTR = nullptr
#endif

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

class PainterShader_Color : public PainterShader{
    GLint unif_color;

public:
    PainterShader_Color();

    ~PainterShader_Color();

    void setColor(float r, float g, float b, float a);
};

R64FX_PAINTER_SHADER_EXTERN PainterShader_Color* g_PainterShader_Color R64FX_PAINTER_SHADER_NULLPTR;


class PainterShader_Texture : public PainterShader{
    GLint unif_sampler;
    GLint attr_tex_coord;

public:
    PainterShader_Texture();

    ~PainterShader_Texture();

    void setSampler(int sampler);

    void bindTexCoordAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer);

protected:
    void fetchCommonIndices();
};

R64FX_PAINTER_SHADER_EXTERN PainterShader_Texture* g_PainterShader_Texture R64FX_PAINTER_SHADER_NULLPTR;


class PainterShader_ColorBlend : public PainterShader_Texture{
    GLint unif_color;
    GLint unif_texture_component;

public:
    PainterShader_ColorBlend();

    ~PainterShader_ColorBlend();

    void setColor(float r, float g, float b, float a);

    void setTextureComponent(int component);
};

R64FX_PAINTER_SHADER_EXTERN PainterShader_ColorBlend* g_PainterShader_ColorBlend R64FX_PAINTER_SHADER_NULLPTR;


class PainterShader_Waveform : public PainterShader_Texture{
    GLint unif_color;
    GLint unif_gain;

public:
    PainterShader_Waveform();

    ~PainterShader_Waveform();

    void setColor(float r, float g, float b, float a);

    void setGain(float gain);
};

R64FX_PAINTER_SHADER_EXTERN PainterShader_Waveform* g_PainterShader_Waveform R64FX_PAINTER_SHADER_NULLPTR;


void init_painter_shaders();

void cleanup_painter_shaders();

}//namespace r64fx

#undef R64FX_PAINTER_SHADER_EXTERN

#endif//R64FX_PAINTER_SHADERS_HPP

#endif//R64FX_USE_GL

