#include "Shader.h"
#include "Error.h"
#include <iostream>

using namespace std;

namespace r64fx{
    
Shader::Shader(const char* text, GLenum shader_type)
{
    _shader = glCreateShader(shader_type);
    CHECK_FOR_GL_ERRORS;
    if(!_shader)
        return;
    
    glShaderSource(_shader, 1, &text, nullptr);
    CHECK_FOR_GL_ERRORS;
    glCompileShader(_shader);
    CHECK_FOR_GL_ERRORS;
}


bool Shader::isOk()
{
    if(!_shader)
        return false;
    
    int param;
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &param);
    CHECK_FOR_GL_ERRORS;
    return param == GL_TRUE;
}


const char* Shader::infoLog()
{
    static char buffer[1024];
    int length;
    glGetShaderInfoLog(_shader, 1024, &length, buffer);
    CHECK_FOR_GL_ERRORS;
    return buffer;
}


void Shader::free()
{ 
    glDeleteShader(_shader); 
    CHECK_FOR_GL_ERRORS;
}


ShadingProgram::ShadingProgram(VertexShader vs, FragmentShader fs) : _vs(vs), _fs(fs) 
{
    _program = glCreateProgram();
    CHECK_FOR_GL_ERRORS;
    if(!_program)
        return;
    
    glAttachShader(_program, vs.id());
    CHECK_FOR_GL_ERRORS;
    glAttachShader(_program, fs.id());
    CHECK_FOR_GL_ERRORS;
    glLinkProgram(_program);
    CHECK_FOR_GL_ERRORS;
}


bool ShadingProgram::isOk()
{
    if(!_program)
        return false;
    
    int param;
    glGetProgramiv(_program, GL_LINK_STATUS, &param);
    CHECK_FOR_GL_ERRORS;
    return param == GL_TRUE;
}


const char* ShadingProgram::infoLog()
{
    static char buffer[1024];
    int length;
    glGetProgramInfoLog(_program, 1024, &length, buffer);
    CHECK_FOR_GL_ERRORS;
    return buffer;
}


void ShadingProgram::use()
{
    glUseProgram(_program);
    CHECK_FOR_GL_ERRORS;
}


void ShadingProgram::free() 
{ 
    glDeleteProgram(_program); 
    CHECK_FOR_GL_ERRORS;
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