#include "Shader.h"

namespace r64fx{
    
Shader::Shader(const char* text, GLenum shader_type)
{
    _shader = glCreateShader(shader_type);
    if(!_shader)
        return;
    
    glShaderSource(_shader, 1, &text, nullptr);
    glCompileShader(_shader);
}


bool Shader::isOk()
{
    if(!_shader)
        return false;
    
    int param;
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &param);
    return param == GL_TRUE;
}


const char* Shader::infoLog()
{
    static char buffer[1024];
    int length;
    glGetShaderInfoLog(_shader, 1024, &length, buffer);
    return buffer;
}


void Shader::free()
{ 
    glDeleteShader(_shader); 
}


ShadingProgram::ShadingProgram(VertexShader vs, FragmentShader fs) : _vs(vs), _fs(fs) 
{
    _program = glCreateProgram();
    if(!_program)
        return;
    
    glAttachShader(_program, vs.id());
    glAttachShader(_program, fs.id());
    glLinkProgram(_program);
}


bool ShadingProgram::isOk()
{
    if(!_program)
        return false;
    
    int param;
    glGetProgramiv(_program, GL_LINK_STATUS, &param);
    return param == GL_TRUE;
}


const char* ShadingProgram::infoLog()
{
    static char buffer[1024];
    int length;
    glGetProgramInfoLog(_program, 1024, &length, buffer);
    return buffer;
}


void ShadingProgram::use()
{
    glUseProgram(_program);
}


void ShadingProgram::free() 
{ 
    glDeleteProgram(_program); 
}
    
}//namespace r64x