#ifdef R64FX_USE_GL

#define R64FX_PAINTER_SHADER_IMPL
#include "PainterShaders.hpp"
#undef R64FX_PAINTER_SHADER_IMPL

#include <cstdlib>

#define R64FX_GET_ATTRIB_LOCATION(name) getAttribLocation(attr_##name, #name)
#define R64FX_GET_UNIFORM_LOCATION(name) getUniformLocation(unif_##name, #name)

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

namespace r64fx{

namespace{

const char* shader_text_position =
#include "shader_position.vert.h"
;

const char* shader_text_pos_and_tex_coord =
#include "shader_pos_and_tex_coord.vert.h"
;

const char* shader_text_color =
#include "shader_color.frag.h"
;

const char* shader_text_texture =
#include "shader_texture.frag.h"
;

const char* shader_text_blend_color =
#include "shader_blend_color.frag.h"
;

const char* shader_text_waveform =
#include "shader_waveform.frag.h"
;


VertexShader g_shader_position;
VertexShader g_shader_pos_and_tex_coord;

FragmentShader g_shader_color;
FragmentShader g_shader_texture;
FragmentShader g_shader_blend_color;
FragmentShader g_shader_waveform;

}//namespace


PainterShader::PainterShader()
{

}


PainterShader::~PainterShader()
{

}


void PainterShader::setScaleAndShift(float sx, float sy, float tx, float ty)
{
    gl::Uniform4f(unif_sxsytxty, sx, sy, tx, ty);
}


void PainterShader::bindPositionAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer)
{
    bindAttribute(attr_position, 2, type, normalized, stride, pointer);
}


void PainterShader::fetchCommonIndices()
{
    R64FX_GET_UNIFORM_LOCATION(sxsytxty);
    R64FX_GET_ATTRIB_LOCATION(position);
}


PainterShader_Color::PainterShader_Color()
{
    if(!load(g_shader_position, g_shader_color))
        return;

    fetchCommonIndices();
    R64FX_GET_UNIFORM_LOCATION(color);
}


PainterShader_Color::~PainterShader_Color()
{

}


void PainterShader_Color::setColor(float r, float g, float b, float a)
{
    gl::Uniform4f(unif_color, r, g, b, a);
}


PainterShader_Texture::PainterShader_Texture()
{
    if(!load(g_shader_pos_and_tex_coord, g_shader_texture))
        return;

    fetchCommonIndices();
}


PainterShader_Texture::~PainterShader_Texture()
{

}


void PainterShader_Texture::setSampler(int sampler)
{
    gl::Uniform1i(unif_sampler, sampler);
}


void PainterShader_Texture::bindTexCoordAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer)
{
    bindAttribute(attr_tex_coord, 2, type, normalized, stride, pointer);
}


void PainterShader_Texture::fetchCommonIndices()
{
    PainterShader::fetchCommonIndices();
    R64FX_GET_UNIFORM_LOCATION(sampler);
    R64FX_GET_ATTRIB_LOCATION(tex_coord);
}


PainterShader_ColorBlend::PainterShader_ColorBlend()
{
    if(!load(g_shader_pos_and_tex_coord, g_shader_blend_color))
        return;

    PainterShader_Texture::fetchCommonIndices();

    R64FX_GET_UNIFORM_LOCATION(color);
    R64FX_GET_UNIFORM_LOCATION(texture_component);
}


PainterShader_ColorBlend::~PainterShader_ColorBlend()
{

}


void PainterShader_ColorBlend::setColor(float r, float g, float b, float a)
{
    gl::Uniform4f(unif_color, r, g, b, a);
}


void PainterShader_ColorBlend::setTextureComponent(int component)
{
    gl::Uniform1i(unif_texture_component, component);
}


PainterShader_Waveform::PainterShader_Waveform()
{
    if(!load(g_shader_pos_and_tex_coord, g_shader_waveform))
        return;

    PainterShader_Texture::fetchCommonIndices();
    R64FX_GET_UNIFORM_LOCATION(color);
    R64FX_GET_UNIFORM_LOCATION(gain);
}


PainterShader_Waveform::~PainterShader_Waveform()
{

}


void PainterShader_Waveform::setColor(float r, float g, float b, float a)
{
    gl::Uniform4f(unif_color, r, g, b, a);
}


void PainterShader_Waveform::setGain(float gain)
{
    gl::Uniform1f(unif_gain, gain);
}


void init_painter_shaders()
{
    g_shader_position           = VertexShader(shader_text_position);
    g_shader_pos_and_tex_coord  = VertexShader(shader_text_pos_and_tex_coord);

    g_shader_color              = FragmentShader(shader_text_color);
    g_shader_texture            = FragmentShader(shader_text_texture);
    g_shader_blend_color        = FragmentShader(shader_text_blend_color);
    g_shader_waveform           = FragmentShader(shader_text_waveform);

    g_PainterShader_Color = new PainterShader_Color;
    if(!g_PainterShader_Color->isGood())
        abort();

    g_PainterShader_Texture = new PainterShader_Texture;
    if(!g_PainterShader_Texture->isGood())
        abort();

    g_PainterShader_ColorBlend = new PainterShader_ColorBlend;
    if(!g_PainterShader_ColorBlend->isGood())
        abort();

    g_PainterShader_Waveform = new PainterShader_Waveform;
    if(!g_PainterShader_ColorBlend->isGood())
        abort();
}

void cleanup_painter_shaders()
{
    if(g_PainterShader_Color)
        delete g_PainterShader_Color;

    if(g_PainterShader_Texture)
        delete g_PainterShader_Texture;

    if(g_PainterShader_ColorBlend)
        delete g_PainterShader_ColorBlend;

    if(g_PainterShader_Waveform)
        delete g_PainterShader_Waveform;

    g_shader_position.free();
    g_shader_pos_and_tex_coord.free();
    g_shader_color.free();
    g_shader_texture.free();
}

}//namespace r64fx

#endif//R64FX_USE_GL
