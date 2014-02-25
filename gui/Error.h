#ifndef R64FX_GUI_ERROR_H
#define R64FX_GUI_ERROR_H

namespace r64fx{
  
#ifdef DEBUG
#define CHECK_FOR_GL_ERRORS _check_for_gl_errors(__FILE__, __LINE__);
void _check_for_gl_errors(const char* file, int line);
#else
#define CHECK_FOR_GL_ERRORS
#endif//DEBUG
   
   
}//namespace r64fx

#endif//R64FX_GUI_ERROR_H