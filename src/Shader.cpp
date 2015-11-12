#include "Shader.hpp"
#include <iostream>

using namespace std;

namespace r64fx{
    
Shader::Shader(const char* text, GLenum shader_type)
{
    m_shader = gl::CreateShader(shader_type);
    if(!m_shader)
        return;
    
    gl::ShaderSource(m_shader, 1, &text, nullptr);
    gl::CompileShader(m_shader);

    if(!isOk())
    {
        cerr << "Problems in ";
        if(shader_type == GL_VERTEX_SHADER)
        {
            cerr << "vertex";
        }
        else if(shader_type == GL_FRAGMENT_SHADER)
        {
            cerr << "fragment";
        }
        else
        {
            cerr << "unknown";
        }
        cerr << " shader!\n";
        cerr << infoLog() << "\n";
    }
}


bool Shader::isOk()
{
    if(!m_shader)
        return false;
    
    int param;
    gl::GetShaderiv(m_shader, GL_COMPILE_STATUS, &param);
    return param == GL_TRUE;
}


const char* Shader::infoLog()
{
    static char buffer[1024];
    int length;
    gl::GetShaderInfoLog(m_shader, 1024, &length, buffer);
    return buffer;
}


void Shader::free()
{ 
    gl::DeleteShader(m_shader);
}


ShadingProgram::ShadingProgram(VertexShader vs, FragmentShader fs) : m_vs(vs), m_fs(fs)
{
    m_program = gl::CreateProgram();
    if(!m_program)
        return;
    
    gl::AttachShader(m_program, vs.id());
    gl::AttachShader(m_program, fs.id());
    gl::LinkProgram(m_program);

    if(!isOk())
    {
        cerr << infoLog() << "\n";
    }
}


bool ShadingProgram::isOk()
{
    if(!m_program)
        return false;
    
    int param;
    gl::GetProgramiv(m_program, GL_LINK_STATUS, &param);
    return param == GL_TRUE;
}


const char* ShadingProgram::infoLog()
{
    static char buffer[1024];
    int length;
    gl::GetProgramInfoLog(m_program, 1024, &length, buffer);
    return buffer;
}


void ShadingProgram::use()
{
    gl::UseProgram(m_program);
}


void ShadingProgram::getAttribLocation(GLint &out, const char* name)
{
    out = gl::GetAttribLocation(m_program, name);
#ifdef R64FX_DEBUG
    if(out == -1)
    {
        cerr << "Failed to get attribute location for \"" << name << "\"!\n";
        abort();
    }
#endif//R64FX_DEBUG
}


void ShadingProgram::getUniformLocation(GLint &out, const char* name)
{
    out = gl::GetUniformLocation(m_program, name);
#ifdef R64FX_DEBUG
    if(out == -1)
    {
        cerr << "Failed to get uniform location for \"" << name << "\"!\n";
        abort();
    }
#endif//R64FX_DEBUG
}


void ShadingProgram::free() 
{ 
    gl::DeleteProgram(m_program);
}
    
}//namespace r64x