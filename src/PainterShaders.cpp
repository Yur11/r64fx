#include "PainterShaders.hpp"

#ifdef R64FX_USE_GL

#define R64FX_GET_ATTRIB_LOCATION(name) getAttribLocation(attr_##name, #name)
#define R64FX_GET_UNIFORM_LOCATION(name) getUniformLocation(unif_##name, #name)

#ifdef R64FX_DEBUG
#include <assert.h>
#endif//R64FX_DEBUG

namespace r64fx{
    
namespace{

const char* shader_position =
#include "shader_position.vert.h"
;

const char* shader_pos_and_tex_coord =
#include "shader_pos_and_tex_coord.vert.h"
;

const char* shader_color =
#include "shader_color.frag.h"
;

const char* shader_texture =
#include "shader_texture.frag.h"
;

const char* shader_blend_color = 
#include "shader_blend_color.frag.h"
;


VertexShader g_position;
VertexShader g_pos_and_tex_coord;

FragmentShader g_color;
FragmentShader g_texture;


int g_user_count = 0;

void init()
{
    g_position           = VertexShader(shader_position);
    g_pos_and_tex_coord  = VertexShader(shader_pos_and_tex_coord);
    
    g_color              = FragmentShader(shader_color);
    g_texture            = FragmentShader(shader_texture);
}

void cleanup()
{
    g_position.free();
    g_pos_and_tex_coord.free();
    g_color.free();
    g_texture.free();
}
    
}//namespace


PainterShader::PainterShader()
{
    if(g_user_count == 0)
    {
        init();
    }
    g_user_count++;
}
    
    
PainterShader::~PainterShader()
{
    g_user_count--;
    if(g_user_count == 0)
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
    if(!load(g_position, g_color))
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
    if(!load(g_pos_and_tex_coord, g_texture))
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
    if(!load(g_pos_and_tex_coord, shader_blend_color))
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

}//namespace r64fx

#endif//R64FX_USE_GL
