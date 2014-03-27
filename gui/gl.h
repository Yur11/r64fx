#ifndef R64FX_GUI_GL_H
#define R64FX_GUI_GL_H

#include <GL/glew.h>

namespace r64fx{
    
class BasicTexture;
    
namespace gl{
      
#ifdef DEBUG_GL_ERRORS
#define CHECK_FOR_GL_ERRORS(fun_name) CheckForErrors(fun_name)
void CheckForErrors(const char* fun_name);
#else
#define CHECK_FOR_GL_ERRORS(fun_name)
#endif//DEBUG_GL_ERRORS
     
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

inline void GenBuffers(GLsizei n, GLuint* buffers)
{
    glGenBuffers(n, buffers);
    CHECK_FOR_GL_ERRORS("glGenBuffers");
}

inline void DeleteBuffers(GLsizei n, const GLuint* buffers)
{
    glDeleteBuffers(n, buffers);
    CHECK_FOR_GL_ERRORS("glDeleteBuffers");
}

inline void BindBuffer(GLenum target, GLuint buffer)
{
    glBindBuffer(target, buffer);
    CHECK_FOR_GL_ERRORS("glBindBuffer");
}

inline void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    glBufferSubData(target, offset, size, data);
    CHECK_FOR_GL_ERRORS("glBufferSubData");
}

inline void GenVertexArrays(GLsizei n, GLuint* arrays)
{
    glGenVertexArrays(n, arrays);
    CHECK_FOR_GL_ERRORS("glGenVertexArrays");
}

inline void DeleteVertexArrays(GLsizei n, const GLuint *arrays)
{
    glDeleteVertexArrays(n, arrays);
    CHECK_FOR_GL_ERRORS("glDeleteVertexArrays");
}

inline void BindVertexArray(GLuint array)
{
    glBindVertexArray(array);
    CHECK_FOR_GL_ERRORS("glBindVertexArray");
}

inline void BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    glBufferData(target, size, data, usage);
    CHECK_FOR_GL_ERRORS("glBufferData");
}

inline void EnableVertexAttribArray(GLuint index)
{
    glEnableVertexAttribArray(index);
    CHECK_FOR_GL_ERRORS("glEnableVertexAttribArray");
}

inline void DisableVertexAttribArray(GLuint index)
{
    glDisableVertexAttribArray(index);
    CHECK_FOR_GL_ERRORS("glDisableVertexAttribArray");
}

inline void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLsizei pointer)
{
    glVertexAttribPointer(index, size, type, normalized, stride, (void*)long(pointer));
    CHECK_FOR_GL_ERRORS("glVertexAttribPointer");
}

inline void DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    glDrawArrays(mode, first, count);
    CHECK_FOR_GL_ERRORS("glDrawArrays");
}

inline GLuint CreateShader(GLenum shaderType)
{
    auto shader = glCreateShader(shaderType);
    CHECK_FOR_GL_ERRORS("glCreateShader");
    return shader;
}

inline void DeleteShader(GLuint shader)
{
    glDeleteShader(shader);
    CHECK_FOR_GL_ERRORS("glDeleteShader");
}

inline void ShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length)
{
    glShaderSource(shader, count, string, length);
    CHECK_FOR_GL_ERRORS("glShaderSource");
}

inline void CompileShader(GLuint shader)
{
    glCompileShader(shader);
    CHECK_FOR_GL_ERRORS("glCompileShader");
}

inline void GetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
    glGetShaderiv(shader, pname, params);
    CHECK_FOR_GL_ERRORS("glGetShaderiv");
}

inline void GetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
    glGetShaderInfoLog(shader, maxLength, length, infoLog);
    CHECK_FOR_GL_ERRORS("glGetShaderInfoLog");
}

inline GLuint CreateProgram()
{
    auto program = glCreateProgram();
    CHECK_FOR_GL_ERRORS("glCreateProgram");
    return program;
}

inline void DeleteProgram(GLuint program)
{
    glDeleteProgram(program);
    CHECK_FOR_GL_ERRORS("glDeleteProgram");
}

inline void AttachShader(GLuint program, GLuint shader)
{
    glAttachShader(program, shader);
    CHECK_FOR_GL_ERRORS("glAttachShader");
}

inline void LinkProgram(GLuint program)
{
    glLinkProgram(program);
    CHECK_FOR_GL_ERRORS("glLinkProgram");
}

inline void GetProgramiv(GLuint program, GLenum pname, GLint* params)
{
    glGetProgramiv(program, pname, params);
    CHECK_FOR_GL_ERRORS("glGetProgramiv");
}

inline void GetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
    glGetProgramInfoLog(program, maxLength, length, infoLog);
    CHECK_FOR_GL_ERRORS("glGetProgramInfoLog");
}

inline GLint GetAttribLocation(GLuint program, const GLchar* name)
{
    auto loc = glGetAttribLocation(program, name);
    CHECK_FOR_GL_ERRORS("glGetAttribLocation");
    return loc;
}

inline GLint GetUniformLocation(GLuint program, const GLchar* name)
{
    auto loc = glGetUniformLocation(program, name);
    CHECK_FOR_GL_ERRORS("glGetUniformLocation");
    return loc;
}

inline void UseProgram(GLuint program)
{
    glUseProgram(program);
    CHECK_FOR_GL_ERRORS("glUseProgram");
}

inline void Uniform1i(GLint location, GLint v0)
{
    glUniform1i(location, v0);
    CHECK_FOR_GL_ERRORS("glUniform1i");
}

inline void Uniform4fv(GLint location, GLsizei count, const GLfloat *value)
{
    glUniform4fv(location, count, value);
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

inline void TexStorage1D(
    GLenum target,
    GLsizei levels,
    GLenum internalFormat,
    GLsizei width
)
{
    glTexStorage1D(target, levels, internalFormat, width);
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

inline void TexStorage2D(
    GLenum target,
    GLsizei levels,
    GLenum internalFormat,
    GLsizei width,
    GLsizei height
)
{
    glTexStorage2D(target, levels, internalFormat, width, height);
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

inline void GenerateMipmap(GLenum target)
{
    glGenerateMipmap(target);
    CHECK_FOR_GL_ERRORS("glGenerateMipmap");
}
    
#ifdef DEBUG_GL_ERRORS
#undef CHECK_FOR_GL_ERRORS
#endif//DEBUG_GL_ERRORS

}//namespace gl
    
}//namespace r64fx

#endif//R64FX_GUI_GL_H