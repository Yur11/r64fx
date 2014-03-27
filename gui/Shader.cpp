#include "Shader.h"
#include "Error.h"
#include <iostream>

using namespace std;

namespace r64fx{
    
Shader::Shader(const char* text, GLenum shader_type)
{
    _shader = gl::CreateShader(shader_type);
    if(!_shader)
        return;
    
    gl::ShaderSource(_shader, 1, &text, nullptr);
    gl::CompileShader(_shader);
}

bool Shader::isOk()
{
    if(!_shader)
        return false;
    
    int param;
    gl::GetShaderiv(_shader, GL_COMPILE_STATUS, &param);
    return param == GL_TRUE;
}


const char* Shader::infoLog()
{
    static char buffer[1024];
    int length;
    gl::GetShaderInfoLog(_shader, 1024, &length, buffer);
    return buffer;
}


void Shader::free()
{ 
    gl::DeleteShader(_shader); 
}


ShadingProgram::ShadingProgram(VertexShader vs, FragmentShader fs) : _vs(vs), _fs(fs) 
{
    _program = gl::CreateProgram();
    if(!_program)
        return;
    
    gl::AttachShader(_program, vs.id());
    gl::AttachShader(_program, fs.id());
    gl::LinkProgram(_program);
}


bool ShadingProgram::isOk()
{
    if(!_program)
        return false;
    
    int param;
    gl::GetProgramiv(_program, GL_LINK_STATUS, &param);
    return param == GL_TRUE;
}


const char* ShadingProgram::infoLog()
{
    static char buffer[1024];
    int length;
    gl::GetProgramInfoLog(_program, 1024, &length, buffer);
    return buffer;
}


void ShadingProgram::use()
{
    gl::UseProgram(_program);
}


void ShadingProgram::free() 
{ 
    gl::DeleteProgram(_program); 
}


ShadingProgram ShadingProgram::create(const char* vs_text, const char* fs_text)
{
    VertexShader vs(vs_text);
    if(!vs.isOk())
    {
        cerr << "Problems in vertex shader!\n";
        cerr << vs.infoLog() << "\n";
        return ShadingProgram();
    }
    
    FragmentShader fs(fs_text);
    if(!fs.isOk())
    {
        cerr << "Problems in fragment shader!\n";
        cerr << fs.infoLog() << "\n";
        return ShadingProgram();
    }
    
    return ShadingProgram(vs, fs);    
}
    
}//namespace r64x