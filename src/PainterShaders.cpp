#include "PainterShaders.hpp"

#ifdef R64FX_USE_GL

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


VertexShader shader_position;
VertexShader shader_pos_and_tex_coord;

FragmentShader shader_color;
FragmentShader shader_texture;
FragmentShader shader_blend_color;
FragmentShader shader_waveform;


int shader_user_count = 0;

void init()
{
    shader_position           = VertexShader(shader_text_position);
    shader_pos_and_tex_coord  = VertexShader(shader_text_pos_and_tex_coord);
    
    shader_color              = FragmentShader(shader_text_color);
    shader_texture            = FragmentShader(shader_text_texture);
    shader_blend_color        = FragmentShader(shader_text_blend_color);
    shader_waveform           = FragmentShader(shader_text_waveform);
}

void cleanup()
{
    shader_position.free();
    shader_pos_and_tex_coord.free();
    shader_color.free();
    shader_texture.free();
}
    
}//namespace


PainterShader::PainterShader()
{
    if(shader_user_count == 0)
    {
        init();
    }
    shader_user_count++;
}
    
    
PainterShader::~PainterShader()
{
    shader_user_count--;
    if(shader_user_count == 0)
    {
        cleanup();
    }
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


Shader_Color::Shader_Color()
{
    if(!load(shader_position, shader_color))
        return;

    fetchCommonIndices();
    R64FX_GET_UNIFORM_LOCATION(color);
}


Shader_Color::~Shader_Color()
{

}


void Shader_Color::setColor(float r, float g, float b, float a)
{
    gl::Uniform4f(unif_color, r, g, b, a);
}


Shader_Texture::Shader_Texture()
{
    if(!load(shader_pos_and_tex_coord, shader_texture))
        return;

    fetchCommonIndices();
}


Shader_Texture::~Shader_Texture()
{

}


void Shader_Texture::setSampler(int sampler)
{
    gl::Uniform1i(unif_sampler, sampler);
}


void Shader_Texture::bindTexCoordAttr(GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer)
{
    bindAttribute(attr_tex_coord, 2, type, normalized, stride, pointer);
}


void Shader_Texture::fetchCommonIndices()
{
    PainterShader::fetchCommonIndices();
    R64FX_GET_UNIFORM_LOCATION(sampler);
    R64FX_GET_ATTRIB_LOCATION(tex_coord);
}


Shader_ColorBlend::Shader_ColorBlend()
{    
    if(!load(shader_pos_and_tex_coord, shader_blend_color))
        return;

    Shader_Texture::fetchCommonIndices();
    
    R64FX_GET_UNIFORM_LOCATION(color);
    R64FX_GET_UNIFORM_LOCATION(texture_component);
}

    
Shader_ColorBlend::~Shader_ColorBlend()
{
    
}


void Shader_ColorBlend::setColor(float r, float g, float b, float a)
{
    gl::Uniform4f(unif_color, r, g, b, a);
}


void Shader_ColorBlend::setTextureComponent(int component)
{
    gl::Uniform1i(unif_texture_component, component);
}


Shader_Waveform::Shader_Waveform()
{
    if(!load(shader_pos_and_tex_coord, shader_waveform))
        return;
    
    Shader_Texture::fetchCommonIndices();
    R64FX_GET_UNIFORM_LOCATION(color);
    R64FX_GET_UNIFORM_LOCATION(gain);
}


Shader_Waveform::~Shader_Waveform()
{
    
}


void Shader_Waveform::setColor(float r, float g, float b, float a)
{
    gl::Uniform4f(unif_color, r, g, b, a);
}


void Shader_Waveform::setGain(float gain)
{
    gl::Uniform1f(unif_gain, gain);
}

}//namespace r64fx

#endif//R64FX_USE_GL
