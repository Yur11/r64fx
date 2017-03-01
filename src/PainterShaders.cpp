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

const char* shader_text_uber =
#include "shader_uber.frag.h"
;

VertexShader g_shader_position;
VertexShader g_shader_pos_and_tex_coord;

FragmentShader g_shader_uber;
FragmentShader g_shader_waveform;

}//namespace


PainterShader::PainterShader(VertexShader vert, FragmentShader frag)
{
    if(load(vert, frag))
    {
        R64FX_GET_UNIFORM_LOCATION(sxsytxty);
        R64FX_GET_ATTRIB_LOCATION(position);
    }
}


PainterShader::~PainterShader()
{

}


PainterShader_Uber::PainterShader_Uber()
: PainterShader(g_shader_pos_and_tex_coord, g_shader_uber)
{
    if(isGood())
    {
        R64FX_GET_ATTRIB_LOCATION(tex_coord);
        R64FX_GET_UNIFORM_LOCATION(mode);
        R64FX_GET_UNIFORM_LOCATION(color);
        R64FX_GET_UNIFORM_LOCATION(sampler);
    }
}


void init_painter_shaders()
{
    g_shader_position           = VertexShader(shader_text_position);
    g_shader_pos_and_tex_coord  = VertexShader(shader_text_pos_and_tex_coord);

    g_shader_uber               = FragmentShader(shader_text_uber);

    g_PainterShader_Uber = new PainterShader_Uber;
    if(!g_PainterShader_Uber->isGood())
        abort();
}


void cleanup_painter_shaders()
{
    if(g_PainterShader_Uber)
        delete g_PainterShader_Uber;

    g_shader_position.free();
    g_shader_pos_and_tex_coord.free();
}

}//namespace r64fx

#endif//R64FX_USE_GL
