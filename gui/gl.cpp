#include "gl.hpp"

#ifdef DEBUG_GL_ERRORS
#ifdef ABORT_ON_GL_ERROR
#include <assert.h>
#endif//ABORT_ON_GL_ERROR

#include <iostream>

using namespace std;
#endif//DEBUG_GL_ERRORS

namespace r64fx{
    
namespace gl{
    
#ifdef DEBUG_GL_ERRORS
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
        
#ifdef ABORT_ON_GL_ERROR
        abort();
#endif//ABORT_ON_GL_ERROR
    }
}
#endif//DEBUG_GL_ERRORS
    
}//namespace gl
    
}//namespace r64fx