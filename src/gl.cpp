#ifdef R64FX_USE_GL

#define R64FX_IMPL
#include "gl.hpp"

#ifdef R64FX_DEBUG_GL_ERRORS
#ifdef R64FX_ABORT_ON_GL_ERROR
#include <assert.h>
#endif//R64FX_ABORT_ON_GL_ERROR

#include <iostream>

using namespace std;
#endif//R64FX_DEBUG_GL_ERRORS

namespace r64fx{
    
namespace{
    bool gl_is_setup = false;

}//namespace


#ifdef R64FX_USE_X11
#include <GL/glx.h>
#define FETCH_GL_FUN(name){\
    r64fx_impl_##name = (decltype(r64fx_impl_##name)) glXGetProcAddress((const GLubyte*)#name);\
    if(!r64fx_impl_##name) { cout << "Failed to load " #name << endl; abort(); }}
#endif//R64FX_USE_X11


namespace gl{

void InitIfNeeded()
{
    if(!gl_is_setup)
    {
        FETCH_GL_FUN(glGenBuffers)
        FETCH_GL_FUN(glDeleteBuffers)
        FETCH_GL_FUN(glBindBuffer)
        FETCH_GL_FUN(glBufferData)
        FETCH_GL_FUN(glBufferSubData)
        FETCH_GL_FUN(glGenVertexArrays)
        FETCH_GL_FUN(glDeleteVertexArrays)
        FETCH_GL_FUN(glBindVertexArray)
        FETCH_GL_FUN(glEnableVertexAttribArray)
        FETCH_GL_FUN(glDisableVertexAttribArray)
        FETCH_GL_FUN(glVertexAttribPointer)
        FETCH_GL_FUN(glCreateShader)
        FETCH_GL_FUN(glDeleteShader)
        FETCH_GL_FUN(glShaderSource)
        FETCH_GL_FUN(glCompileShader)
        FETCH_GL_FUN(glGetShaderiv)
        FETCH_GL_FUN(glGetShaderInfoLog)
        FETCH_GL_FUN(glCreateProgram)
        FETCH_GL_FUN(glDeleteProgram)
        FETCH_GL_FUN(glAttachShader)
        FETCH_GL_FUN(glLinkProgram)
        FETCH_GL_FUN(glGetProgramiv)
        FETCH_GL_FUN(glGetProgramInfoLog)
        FETCH_GL_FUN(glGetAttribLocation)
        FETCH_GL_FUN(glGetUniformLocation)
        FETCH_GL_FUN(glUseProgram)
        FETCH_GL_FUN(glUniform1i)
        FETCH_GL_FUN(glUniform4f)
        FETCH_GL_FUN(glUniform4fv)
        FETCH_GL_FUN(glTexStorage1D)
        FETCH_GL_FUN(glTexStorage2D)
        FETCH_GL_FUN(glPrimitiveRestartIndex)
        gl_is_setup = true;
    }
}

    
#ifdef R64FX_DEBUG_GL_ERRORS
void CheckForErrors(const char* fun_name)
{
    auto error = glGetError();
    if(error != GL_NO_ERROR)
    {
        switch(error)
        {
            case GL_INVALID_ENUM:
                cerr << "GL_INVALID_ENUM";
            break;
            
            case GL_INVALID_VALUE:
                cerr << "GL_INVALID_VALUE";
            break;
            
            case GL_INVALID_OPERATION:
                cerr << "GL_INVALID_OPERATION";
            break;
            
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                cerr << "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
            
            case GL_OUT_OF_MEMORY:
                cerr << "GL_OUT_OF_MEMORY";
            break;
            
            case GL_STACK_UNDERFLOW:
                cerr << "GL_STACK_UNDERFLOW";
            break;
            
            case GL_STACK_OVERFLOW:
                cerr << "GL_STACK_OVERFLOW";
            break;
            
            default:
                cerr << "Unknown error\n";
            break;
        }
        
        cerr << " in " << fun_name << " !\n";
        
#ifdef R64FX_ABORT_ON_GL_ERROR
        abort();
#endif//R64FX_ABORT_ON_GL_ERROR
    }
}
#endif//R64FX_DEBUG_GL_ERRORS
    
}//namespace gl
    
}//namespace r64fx

#endif//R64FX_USE_GL
