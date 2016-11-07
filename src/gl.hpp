#ifdef R64FX_USE_GL

#ifndef R64FX_GL_HPP
#define R64FX_GL_HPP

#include <GL/gl.h>
#include "MaybeExtern.hpp"

namespace r64fx{
    
namespace gl{
      
void InitIfNeeded();

#ifdef R64FX_DEBUG_GL_ERRORS
#define CHECK_FOR_GL_ERRORS(fun_name) CheckForErrors(fun_name)
void CheckForErrors(const char* fun_name);
#else
#define CHECK_FOR_GL_ERRORS(fun_name)
#endif//R64FX_DEBUG_GL_ERRORS

inline void GetIntegerv(GLenum pname, GLint* data)
{
    glGetIntegerv(pname, data);
    CHECK_FOR_GL_ERRORS("glGetIntegerv");
}

inline void ClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    glClearColor(red, green, blue, alpha);
    CHECK_FOR_GL_ERRORS("glClearColor");
}

inline void Clear(GLbitfield mask)
{
    glClear(mask);
    CHECK_FOR_GL_ERRORS("glClear");
}

inline void Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glViewport(x, y, width, height);
    CHECK_FOR_GL_ERRORS("glViewport");
}

inline void Scissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glScissor(x, y, width, height);
    CHECK_FOR_GL_ERRORS("glScissor");
}

inline void Enable(GLenum cap)
{
    glEnable(cap);
    CHECK_FOR_GL_ERRORS("glEnable");
}

inline void Disable(GLenum cap)
{
    glDisable(cap);
    CHECK_FOR_GL_ERRORS("glDisable");
}

inline void BlendFunc(GLenum sfactor, GLenum dfactor)
{
    glBlendFunc(sfactor, dfactor);
    CHECK_FOR_GL_ERRORS("glBlendFunc");
}

inline void DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    glDrawArrays(mode, first, count);
    CHECK_FOR_GL_ERRORS("glDrawArrays");
}

inline void DrawElements(GLenum mode, GLsizei count, GLenum type, long int indices)
{
    glDrawElements(mode, count, type, (void*)indices);
    CHECK_FOR_GL_ERRORS("glDrawElements");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glGenBuffers)
    (GLsizei n, GLuint* buffers);

inline void GenBuffers(GLsizei n, GLuint* buffers)
{
    r64fx_impl_glGenBuffers(n, buffers);
    CHECK_FOR_GL_ERRORS("glGenBuffers");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glDeleteBuffers)
    (GLsizei n, const GLuint* buffers);

inline void DeleteBuffers(GLsizei n, const GLuint* buffers)
{
    r64fx_impl_glDeleteBuffers(n, buffers);
    CHECK_FOR_GL_ERRORS("glDeleteBuffers");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glBindBuffer)
    (GLenum target, GLuint buffer);

inline void BindBuffer(GLenum target, GLuint buffer)
{
    r64fx_impl_glBindBuffer(target, buffer);
    CHECK_FOR_GL_ERRORS("glBindBuffer");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glBufferData)
    (GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);

inline void BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    r64fx_impl_glBufferData(target, size, data, usage);
    CHECK_FOR_GL_ERRORS("glBufferData");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glBufferSubData)
    (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);

inline void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    r64fx_impl_glBufferSubData(target, offset, size, data);
    CHECK_FOR_GL_ERRORS("glBufferSubData");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glGenVertexArrays)
    (GLsizei n, GLuint* arrays);

inline void GenVertexArrays(GLsizei n, GLuint* arrays)
{
    r64fx_impl_glGenVertexArrays(n, arrays);
    CHECK_FOR_GL_ERRORS("glGenVertexArrays");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glDeleteVertexArrays)
    (GLsizei n, const GLuint* arrays);

inline void DeleteVertexArrays(GLsizei n, const GLuint *arrays)
{
    r64fx_impl_glDeleteVertexArrays(n, arrays);
    CHECK_FOR_GL_ERRORS("glDeleteVertexArrays");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glBindVertexArray)
    (GLuint array);

inline void BindVertexArray(GLuint array)
{
    r64fx_impl_glBindVertexArray(array);
    CHECK_FOR_GL_ERRORS("glBindVertexArray");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glEnableVertexAttribArray)
    (GLuint index);

inline void EnableVertexAttribArray(GLuint index)
{
    r64fx_impl_glEnableVertexAttribArray(index);
    CHECK_FOR_GL_ERRORS("glEnableVertexAttribArray");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glDisableVertexAttribArray)
    (GLuint index);

inline void DisableVertexAttribArray(GLuint index)
{
    r64fx_impl_glDisableVertexAttribArray(index);
    CHECK_FOR_GL_ERRORS("glDisableVertexAttribArray");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glVertexAttribPointer)
    (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, void* pointer);

inline void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer)
{
    r64fx_impl_glVertexAttribPointer(index, size, type, normalized, stride, (void*)long(pointer));
    CHECK_FOR_GL_ERRORS("glVertexAttribPointer");
}


R64FX_MAYBE_EXTERN GLuint (*r64fx_impl_glCreateShader)
    (GLenum shaderType);

inline GLuint CreateShader(GLenum shaderType)
{
    auto shader = r64fx_impl_glCreateShader(shaderType);
    CHECK_FOR_GL_ERRORS("glCreateShader");
    return shader;
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glDeleteShader)
    (GLuint shader);

inline void DeleteShader(GLuint shader)
{
    r64fx_impl_glDeleteShader(shader);
    CHECK_FOR_GL_ERRORS("glDeleteShader");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glShaderSource)
    (GLuint shader, GLsizei count, const GLchar** string, const GLint* length);

inline void ShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length)
{
    r64fx_impl_glShaderSource(shader, count, string, length);
    CHECK_FOR_GL_ERRORS("glShaderSource");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glCompileShader)
    (GLuint shader);

inline void CompileShader(GLuint shader)
{
    r64fx_impl_glCompileShader(shader);
    CHECK_FOR_GL_ERRORS("glCompileShader");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glGetShaderiv)
    (GLuint shader, GLenum pname, GLint* params);

inline void GetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
    r64fx_impl_glGetShaderiv(shader, pname, params);
    CHECK_FOR_GL_ERRORS("glGetShaderiv");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glGetShaderInfoLog)
    (GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);

inline void GetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
    r64fx_impl_glGetShaderInfoLog(shader, maxLength, length, infoLog);
    CHECK_FOR_GL_ERRORS("glGetShaderInfoLog");
}


R64FX_MAYBE_EXTERN GLuint (*r64fx_impl_glCreateProgram)();

inline GLuint CreateProgram()
{
    auto program = r64fx_impl_glCreateProgram();
    CHECK_FOR_GL_ERRORS("glCreateProgram");
    return program;
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glDeleteProgram)
    (GLuint program);

inline void DeleteProgram(GLuint program)
{
    r64fx_impl_glDeleteProgram(program);
    CHECK_FOR_GL_ERRORS("glDeleteProgram");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glAttachShader)
    (GLuint program, GLuint shader);

inline void AttachShader(GLuint program, GLuint shader)
{
    r64fx_impl_glAttachShader(program, shader);
    CHECK_FOR_GL_ERRORS("glAttachShader");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glLinkProgram)
    (GLuint program);

inline void LinkProgram(GLuint program)
{
    r64fx_impl_glLinkProgram(program);
    CHECK_FOR_GL_ERRORS("glLinkProgram");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glGetProgramiv)
    (GLuint program, GLenum pname, GLint* params);

inline void GetProgramiv(GLuint program, GLenum pname, GLint* params)
{
    r64fx_impl_glGetProgramiv(program, pname, params);
    CHECK_FOR_GL_ERRORS("glGetProgramiv");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glGetProgramInfoLog)
    (GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog);

inline void GetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
    r64fx_impl_glGetProgramInfoLog(program, maxLength, length, infoLog);
    CHECK_FOR_GL_ERRORS("glGetProgramInfoLog");
}


R64FX_MAYBE_EXTERN GLint (*r64fx_impl_glGetAttribLocation)
    (GLuint program, const GLchar* name);

inline GLint GetAttribLocation(GLuint program, const GLchar* name)
{
    auto loc = r64fx_impl_glGetAttribLocation(program, name);
    CHECK_FOR_GL_ERRORS("glGetAttribLocation");
    return loc;
}


R64FX_MAYBE_EXTERN GLint (*r64fx_impl_glGetUniformLocation)
    (GLuint program, const GLchar* name);

inline GLint GetUniformLocation(GLuint program, const GLchar* name)
{
    auto loc = r64fx_impl_glGetUniformLocation(program, name);
    CHECK_FOR_GL_ERRORS("glGetUniformLocation");
    return loc;
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glUseProgram)
    (GLuint program);

inline void UseProgram(GLuint program)
{
    r64fx_impl_glUseProgram(program);
    CHECK_FOR_GL_ERRORS("glUseProgram");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glUniform1i)
    (GLint location, GLint v0);

inline void Uniform1i(GLint location, GLint v0)
{
    r64fx_impl_glUniform1i(location, v0);
    CHECK_FOR_GL_ERRORS("glUniform1i");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glUniform1f)
    (GLint location, GLfloat v0);

inline void Uniform1f(GLint location, GLfloat v0)
{
    r64fx_impl_glUniform1f(location, v0);
    CHECK_FOR_GL_ERRORS("glUniform1f");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glUniform4f)
    (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

inline void Uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    r64fx_impl_glUniform4f(location, v0, v1, v2, v3);
    CHECK_FOR_GL_ERRORS("glUniform4f");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glUniform4fv)
    (GLint location, GLsizei count, const GLfloat *value);

inline void Uniform4fv(GLint location, GLsizei count, const GLfloat *value)
{
    r64fx_impl_glUniform4fv(location, count, value);
    CHECK_FOR_GL_ERRORS("glUniform4fv");
}

inline void ActiveTexture(GLenum texture)
{
    glActiveTexture(texture);
    CHECK_FOR_GL_ERRORS("glActiveTexture");
}

inline void GenTextures(GLsizei n, GLuint* textures) 
{ 
    glGenTextures(n, textures); 
    CHECK_FOR_GL_ERRORS("glGenTextures"); 
}

inline void DeleteTextures(GLsizei n, const GLuint* textures)
{
    glDeleteTextures(n, textures);
    CHECK_FOR_GL_ERRORS("glDeleteTextures");
}

inline void BindTexture(GLenum target, GLuint texture)
{
    glBindTexture(target, texture);
    CHECK_FOR_GL_ERRORS("glBindTexture");
}

inline void TexImage1D(
    GLenum target,
    GLint level,
    GLint internalFormat,
    GLsizei width,
    GLsizei height,
    GLenum format,
    GLenum type,
    const GLvoid * data
)
{
    glTexImage2D(target, level, internalFormat, width, height, 0, format, type, data);
    CHECK_FOR_GL_ERRORS("glTexImage1D");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glTexStorage1D)
    (GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);

inline void TexStorage1D(
    GLenum target,
    GLsizei levels,
    GLenum internalFormat,
    GLsizei width
)
{
    r64fx_impl_glTexStorage1D(target, levels, internalFormat, width);
    CHECK_FOR_GL_ERRORS("glTexStorage1D");
}

inline void TexSubImage1D(
    GLenum target,
    GLint level,
    GLint xoffset,
    GLsizei width,
    GLenum format,
    GLenum type,
    const GLvoid * data
)
{
    glTexSubImage1D(target, level, xoffset, width, format, type, data);
    CHECK_FOR_GL_ERRORS("glTexSubImage1D");
}

inline void TexImage2D(
    GLenum target,
    GLint level,
    GLint internalFormat,
    GLsizei width,
    GLsizei height,
    GLenum format,
    GLenum type,
    const GLvoid * data
)
{
    glTexImage2D(target, level, internalFormat, width, height, 0, format, type, data);
    CHECK_FOR_GL_ERRORS("glTexImage2D");
}


R64FX_MAYBE_EXTERN void (*r64fx_impl_glTexStorage2D)
    (GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);

inline void TexStorage2D(
    GLenum target,
    GLsizei levels,
    GLenum internalFormat,
    GLsizei width,
    GLsizei height
)
{
    r64fx_impl_glTexStorage2D(target, levels, internalFormat, width, height);
    CHECK_FOR_GL_ERRORS("glTexStorage2D");
}

inline void TexSubImage2D(
    GLenum target,
    GLint level,
    GLint xoffset,
    GLint yoffset,
    GLsizei width,
    GLsizei height,
    GLenum format,
    GLenum type,
    const GLvoid * data
)
{
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, data);
    CHECK_FOR_GL_ERRORS("glTexSubImage2D");
}

inline void TexParameteri(GLenum target, GLenum pname, GLint param)
{
    glTexParameteri(target, pname, param);
    CHECK_FOR_GL_ERRORS("glTexParameterf");
}

inline void TexParameterf(GLenum target, GLenum pname, GLfloat param)
{
    glTexParameterf(target, pname, param);
    CHECK_FOR_GL_ERRORS("glTexParameteri");
}

R64FX_MAYBE_EXTERN void (*r64fx_impl_glPrimitiveRestartIndex)
    (GLuint index);

inline void PrimitiveRestartIndex(GLuint index)
{
    r64fx_impl_glPrimitiveRestartIndex(index);
    CHECK_FOR_GL_ERRORS("glPrimitiveRestartIndex");
}


inline void PixelStoref(GLenum pname, GLfloat param)
{
    glPixelStoref(pname, param);
    CHECK_FOR_GL_ERRORS("glPixelStoref");
}


inline void PixelStorei(GLenum pname, GLint param)
{
    glPixelStorei(pname, param);
    CHECK_FOR_GL_ERRORS("glPixelStorei");
}


inline void Finish()
{
    glFinish();
    CHECK_FOR_GL_ERRORS("glFinish");
}
    
#ifdef R64FX_DEBUG_GL_ERRORS
#undef CHECK_FOR_GL_ERRORS
#endif//R64FX_DEBUG_GL_ERRORS

}//namespace gl
    
}//namespace r64fx

#endif//R64FX_GL_HPP

#endif//R64FX_USE_GL
