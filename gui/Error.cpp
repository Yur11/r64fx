#include "Error.h"
#include "GL/gl.h"
#include <iostream>

using namespace std;

namespace r64fx{
    
#ifdef DEBUG
void _check_for_gl_errors(const char* file, int line)
{
    auto error = glGetError();
    if(error != GL_NO_ERROR)
    {
        cerr << file << "\nline: " << line << "\n";
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
                cerr << "Unknown gl error!\n";
            break;
        }
        
        cerr << "\n";
        abort();
    }
}
#endif//DEBUG
    
}//namespace r64fx